import threading
import time
import psutil
from .cachereader import CacheReader
from .worker import Worker
import math
import json
import os
from .common import *

# read value to a dict by keys path
def dict_keys_write(mydict, keys, value):
    assert(len(keys) > 0)
    cur_key = keys[0]
    rest_keys = keys[1:]
    if len(rest_keys) == 0:
        mydict[cur_key] = value
    else:
        if cur_key not in mydict:  
            mydict[cur_key] = {}
        dict_keys_write(mydict[cur_key], keys[1:], value)
    

# read from a dict by keys path
def dict_keys_read(mydict, keys):
    assert(len(keys) > 0)
    cur_key = keys[0]
    if cur_key in mydict:
        rest_keys = keys[1:]
        if len(rest_keys) == 0:
            return mydict[cur_key]
        return dict_keys_read(mydict[cur_key], keys[1:])
    else:
        return None

def load_json(filename):
    print("# [scheduler] load_json:", filename)
    data = None
    if not os.path.exists(filename):
        return None
    with open(filename, 'r') as f:
        data = json.load(f)
    return data

def save_json(filename, data):
    print("# [scheduler] save_json:", filename)
    with open(filename, 'w') as f:
        json.dump(data, f, indent=2)

class Scheduler():
    def __init__(self, strprose_cache_dir, strstun_cache_dir, mem_estimation_filepath, thread_limit=6, memory_limit=24576):
        self.task_history = []
        self.task_dict = {}
        self.running_workers = {}
        self.task_index = 0
        self.perf_status = None
        
        self.THREAD_LIMIT = thread_limit
        self.MEMORY_LIMIT = memory_limit
        self.MEMORY_RESERVATION_UNIT = 256 # smallest memory alloc unit
        self.DEFAULT_MEMORY_RESERVATION_LIMIT = math.floor(memory_limit / self.MEMORY_RESERVATION_UNIT / thread_limit) * self.MEMORY_RESERVATION_UNIT
        self.MEM_ESTIMATION_FILEPATH = mem_estimation_filepath
        self._mem_estimation_by_task = load_json(self.MEM_ESTIMATION_FILEPATH)
        if self._mem_estimation_by_task is None: self._mem_estimation_by_task = {}
        self._mem_estimation_by_task_dirty = False
        self._failure_worker_ids_history = []
        
        self.cache_reader = CacheReader(strprose_cache_dir, strstun_cache_dir)
        print("# [scheduler] init. thread_limit:", thread_limit, " memory_limit:", memory_limit)

    def _get_task_index(self, taskdata):
        self.task_index += 1
        return self.task_index

    def _get_worker_id(self, taskdata):
        return taskdata["synthesizer"] + \
            "." + taskdata["example_file"] + \
            "." + str(taskdata["no_counting"]) + \
            ".init" + str(taskdata["example_size"])
    
    def _put_task_mem_estimation(self, taskdata, reslimit):
        keys_for_res_limit = [
            taskdata["synthesizer"],
            taskdata["example_file"],
            str(taskdata["no_counting"]),
            str(taskdata["example_size"])]
        dict_keys_write(self._mem_estimation_by_task, keys_for_res_limit, reslimit)
        self._mem_estimation_by_task_dirty = True
        print("# [scheduler] _put_task_mem_estimation: ", keys_for_res_limit, reslimit)

    def _memory_estimation_to_reservation(self, mem_estimation):
        ceil_ratio = math.ceil(mem_estimation / self.DEFAULT_MEMORY_RESERVATION_LIMIT)
        reservation_mem = ceil_ratio * self.DEFAULT_MEMORY_RESERVATION_LIMIT
        if reservation_mem < self.DEFAULT_MEMORY_RESERVATION_LIMIT:
            return self.DEFAULT_MEMORY_RESERVATION_LIMIT
        return reservation_mem
    
    def _failed_memory_estimation_to_increased_estimation(self, mem_estimation):
        ceil_ratio = math.ceil(mem_estimation / self.DEFAULT_MEMORY_RESERVATION_LIMIT)
        round_double_ratio = max(ceil_ratio, round(mem_estimation * 2 / self.DEFAULT_MEMORY_RESERVATION_LIMIT))
        new_estimation = round_double_ratio * self.DEFAULT_MEMORY_RESERVATION_LIMIT
        if new_estimation >= self.MEMORY_LIMIT:
            if new_estimation > mem_estimation:
                return self.MEMORY_LIMIT
            else:
                print(bcolors.warn("# [scheduler] _failed_memory_estimation_to_increased_estimation unexpected mem_estimation (> limit):"), mem_estimation)
                return new_estimation
        return new_estimation
    
    def _get_task_reslimit(self, taskdata):
        keys_for_res_limit = [
            taskdata["synthesizer"],
            taskdata["example_file"],
            str(taskdata["no_counting"])]
        eg_size =  int(taskdata["example_size"])
        
        # function to approximate memory using a dict {10:1GB, 20:2GB ...}
        def approxinate_mem(eg_size, mem_by_eg_dict):
            apx_mem = None
            smallest_eg_distance = 9999999
            for key in mem_by_eg_dict:
                eg_key = int(key)
                if abs(eg_key - eg_size) < smallest_eg_distance:
                    smallest_eg_distance = abs(eg_key - eg_size) 
                    apx_mem = mem_by_eg_dict[key]
            return apx_mem

        
        mem_by_eg_dict = dict_keys_read(self._mem_estimation_by_task, keys_for_res_limit)
        if mem_by_eg_dict is not None:
            apx_mem = approxinate_mem(eg_size, mem_by_eg_dict)
            if apx_mem is not None:
                # (obselete) reservation_memory = math.ceil(apx_mem/self.MEMORY_RESERVATION_UNIT) * self.MEMORY_RESERVATION_UNIT
                return self._memory_estimation_to_reservation(apx_mem)
        
        # no info to calculate reservation_memory
        return self.DEFAULT_MEMORY_RESERVATION_LIMIT

    
    def _find_existing_worker(self, taskdata):
        expected_prefix = taskdata["synthesizer"] + \
            "." + taskdata["example_file"] + \
            "." + str(taskdata["no_counting"]) + "."
        for key in self.running_workers:
            if key.startswith(expected_prefix):
                return self.running_workers[key]
        return None

    def _find_waiting_duplicated_task(self, taskdata):
        for idx in self.task_dict:
            target_taskdata = self.task_dict[idx]
            any_difference = False
            for compare_key in ["synthesizer", "example_file", "no_counting", "example_size"]:
                if target_taskdata[compare_key] != taskdata[compare_key]:
                    any_difference = True
            if not any_difference: # means duplication!
                # print(f"# [scheduler] duplicated task data. ignored.")
                return True
        return False

    def process_synth_task(self, taskdata):
        synthesizer = taskdata["synthesizer"]
        example_file = taskdata["example_file"]
        no_counting = taskdata["no_counting"]
        example_size = int(taskdata["example_size"])
        keepalive = float(taskdata["keepalive"])
        taskdata["example_size"] = example_size
        taskdata["keepalive"] = keepalive
        
        # running the hook function
        if self.process_synth_task.__qualname__ in MY_HOOKS:
            MY_HOOKS[self.process_synth_task.__qualname__](taskdata)

        # optional
        _cache_only = False
        if "_cache_only" in taskdata: _cache_only = taskdata["_cache_only"]
        if DBG_FLAGS.DEBUG and DBG_FLAGS.SYNTH_TASK_CACHE_ONLY:
            _cache_only = True

        cache_result = self.cache_reader.check_cache(taskdata)
        response_data = {
            'syntask_data': taskdata,
            'syntask_status': None,
            # 'syntask_perf': {
            #     'current_memory': 0,
            #     'max_memory': 0
            # },
            'syntask_submit_time': None,
            'syntask_finish_time': None,
            'result': None
        }
        if cache_result is not None:
            response_data['syntask_status'] = "DONE"
            response_data['result'] = cache_result
        elif _cache_only:
            response_data['syntask_status'] = "CACHE_MISS"
            response_data['result'] = None
        else:
            is_duplicated = self._find_waiting_duplicated_task(taskdata)
            if is_duplicated: return response_data

            print("# [scheduler] add_synth_task: ", synthesizer, example_file, example_size, " KEEPALIVE:", keepalive, "NO_COUNTING:", no_counting)
            idx = self._get_task_index(taskdata)
            self.task_dict[idx] = taskdata
            response_data['syntask_status'] = "CREATED"
        return response_data

    def get_tasks_waiting(self):
        return [self.task_dict[x] for x in self.task_dict]

    def get_tasks_history(self, start_idx, count):
        if start_idx < 0 or start_idx >= len(self.task_history) or count <= 0:
            return []
        return self.task_history[start_idx:start_idx + count]

    def get_workers_status(self):
        status_dict = {}
        for key in self.running_workers:
            worker = self.running_workers[key]
            worker_info = {
                "worker_id": worker.worker_id,
                "synthesizer": worker.synthesizer,
                "no_counting": worker.no_counting,
                "example_file": worker.example_file,
                "mem_reservation_limit": worker.mem_reservation_limit,
                "cmd": worker.cmd,
                "last_example_size": worker._example_size_running,
                "keepalive": worker.keepalive,
                "is_idle": worker.is_idle,
                "is_failure": worker.is_failure,
                "is_released": worker.is_released,
                "process_pid": None if worker.synth_process is None else worker.synth_process.pid,
                "perf_status": worker.perf_status
            }
            status_dict[key] = worker_info
        return status_dict

    def get_perf_status(self):
        if self.perf_status == None:
            self._update_perf_status_once()
        return self.perf_status

    def get_failure_info(self):
        return {
            "failure_count": len(self._failure_worker_ids_history),
            "failure_worker_ids": self._failure_worker_ids_history
        }
    
    def _update_perf_and_workers_perf_status_run(self): 
        while True:
            self._update_perf_status_once()
            time.sleep(0.5)
            for key in list(dict.keys(self.running_workers)):
                if key not in self.running_workers: continue
                worker = self.running_workers[key]
                worker.check_perf_status_once()

    
    def _update_perf_status_once(self):
        vmem_info = psutil.virtual_memory()
        self.perf_status = {
            "cpu_overall": psutil.cpu_percent(interval=1, percpu=False),
            "cpu_percpu": psutil.cpu_percent(interval=1, percpu=True),
            "memory_percent": vmem_info.percent, 
            "memory_total": vmem_info.total, 
            "memory_available": vmem_info.available
        }

    def start(self):
        x = threading.Thread(target=self._run)
        print("# [scheduler] start scheduler thread ...")
        x.start()
        update_perf_thread = threading.Thread(target=self._update_perf_and_workers_perf_status_run)
        print("# [scheduler] start update_perf thread ...")
        update_perf_thread.start()
    
    def _check_release_workers(self):
        for key in list(dict.keys(self.running_workers)):
            worker = self.running_workers[key]
            if worker.is_released:
                # check if failure
                if worker.is_failure:
                    print("# [scheduler] remove released (failure) worker:", key)
                    self._failure_worker_ids_history.append(worker.worker_id)
                else:
                    print("# [scheduler] remove released (normal) worker:", key)
                # save memory estimation
                if worker.perf_status["max_rss"] is not None:
                    mem_estimation = worker.perf_status["max_rss"] / 1024 / 1024 # MB
                    if worker.is_failure:
                        if mem_estimation > self.MEMORY_LIMIT:
                            mem_estimation = self.MEMORY_LIMIT + 1  # memory estimation becomes larger than memory limit.
                        else:
                            mem_estimation = self._failed_memory_estimation_to_increased_estimation(mem_estimation)
                    self._put_task_mem_estimation({
                        "synthesizer": worker.synthesizer,
                        "example_file": worker.example_file,
                        "no_counting": worker.no_counting,
                        "example_size": worker.maximum_example_size_run
                    }, mem_estimation)
                else:
                    print("# [scheduler] WARNING: worker doesn't have max_rss data.", worker.worker_id, " | ", worker.perf_status)
                # delete worker
                del self.running_workers[key]

    def _check_worker_resources(self, taskdata):
        # TODO: task data can be none or dict
        worker_count = len(self.running_workers)
        if worker_count >= self.THREAD_LIMIT:
            return False, None, False
        current_mem_res_total = 0
        for key in self.running_workers:
            worker = self.running_workers[key]
            current_mem_res_total += worker.mem_reservation_limit
        new_mem_res_limit = self._get_task_reslimit(taskdata)
        if new_mem_res_limit > self.MEMORY_LIMIT:
            return False, None, True
        if current_mem_res_total + new_mem_res_limit > self.MEMORY_LIMIT:
            return False, None, False
        return True, new_mem_res_limit, False

    def _create_worker(self, taskdata, mem_res_limit):
        print("# [scheduler-thread] create worker with MEM_RES_LIMIT:", mem_res_limit)
        worker_id = self._get_worker_id(taskdata)

        worker = Worker(
            worker_id,
            taskdata["synthesizer"],
            taskdata["example_file"],
            taskdata["no_counting"],
            mem_res_limit,
            self.cache_reader.check_cache # callback for cache check. Better perf.
        )
        
        worker.run_example_size(taskdata['example_size'], taskdata["keepalive"], taskdata["no_counting"])
        self.running_workers[worker_id] = worker

    def _run(self):
        print("# [scheduler-thread] thread is running ...")
        DELTA_T = 0.125
        total_t = 0
        while True:
            time.sleep(DELTA_T)
            total_t += DELTA_T
            
            if total_t % 10 == 0:
                if self._mem_estimation_by_task_dirty:
                    # write mem estimation to file
                    save_json(self.MEM_ESTIMATION_FILEPATH, self._mem_estimation_by_task)
                    self._mem_estimation_by_task_dirty = False
            
            self._check_release_workers()
            
            for idx in list(dict.keys(self.task_dict)):
                taskdata = self.task_dict[idx]
                if self.cache_reader.if_exist_result_for_task_false_negative_allowed(taskdata):
                    del self.task_dict[idx]
                    continue
                if "_is_not_enough_memory" in taskdata and taskdata["_is_not_enough_memory"]:
                    continue
                existing_worker = self._find_existing_worker(taskdata)
                if existing_worker is None:
                    can_create, mem_res_limit, is_not_enough_memory = self._check_worker_resources(taskdata)
                    if can_create:
                        self._create_worker(taskdata, mem_res_limit)
                        del self.task_dict[idx]
                        self.task_history.append(taskdata)
                    elif is_not_enough_memory:
                        taskdata["_is_not_enough_memory"] = True
                else:
                    if existing_worker.keepalive > 5 and existing_worker.is_idle:
                        existing_worker.run_example_size(taskdata["example_size"], taskdata["keepalive"], taskdata["no_counting"])
                        del self.task_dict[idx]
                        self.task_history.append(taskdata)
                        
