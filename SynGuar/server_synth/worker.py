from os.path import join
import subprocess
from sys import stdin
import threading
import time
import os, signal
from .common import *

import psutil
abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))
SYNTHESIZER_STRPROSE = "StrPROSE"
SYNTHESIZER_STRSTUN = "StrSTUN"

SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
TOOL_STRPROSE_RELPATH = "../../StrPROSE-synthesizer/StrPROSE/bin/Debug/netcoreapp3.1/StrPROSE.dll"
TOOL_STRPROSE_FULLPATH = abs_join(SCRIPT_DIR, TOOL_STRPROSE_RELPATH)
TOOL_STRSTUN_RELPATH = "../../build-strstun/strstun"
TOOL_STRSTUN_FULLPATH = abs_join(SCRIPT_DIR, TOOL_STRSTUN_RELPATH)

CACHE_STRPROSE_FULLDIR = abs_join(SCRIPT_DIR, "../../outputs/cache/StrPROSE")
CACHE_STRSTUN_FULLDIR = abs_join(SCRIPT_DIR, "../../outputs/cache/StrSTUN")

EXAMPLE_STRPROSE_RELDIR = "../../benchmark/strprose/example_files"
EXAMPLE_STRPROSE_FULLDIR = abs_join(SCRIPT_DIR, EXAMPLE_STRPROSE_RELDIR)
EXAMPLE_STRSTUN_RELDIR = "../../benchmark/strstun/example_files"
EXAMPLE_STRSTUN_FULLDIR = abs_join(SCRIPT_DIR, EXAMPLE_STRSTUN_RELDIR)
COMP_STRSTUN_RELDIR = "../../benchmark/strstun/targets"
COMP_STRSTUN_FULLDIR = abs_join(SCRIPT_DIR, COMP_STRSTUN_RELDIR)

EXAMPLE_SIZE_THROTTLE_DEFAULT = 5
class Worker():
    def __init__(self, worker_id, synthesizer, example_file, no_counting, mem_reservation_limit, check_cache_callback):
        self.worker_id = worker_id
        self.synthesizer = synthesizer
        self.example_file = example_file
        self.no_counting = no_counting
        self.mem_reservation_limit = mem_reservation_limit
        self.keepalive = 9999999
        self.thread = None
        self.is_idle = True
        self.is_failure = False
        self.is_released = False
        self._example_size_running = None
        self.maximum_example_size_run = -1
        self._example_size_throttle = EXAMPLE_SIZE_THROTTLE_DEFAULT
        self._use_example_size_throttling = False
        if self.synthesizer == SYNTHESIZER_STRPROSE:
            self._use_example_size_throttling = True
        self.synth_process = None
        self.perf_status = {"rss": None, "time": None, "max_rss": None}
        self.check_cache_callback = check_cache_callback
        self.cmd = None
    
    def run_example_size(self, example_size, keepalive, no_counting):
        assert(self._example_size_running is None and self.is_idle and self.keepalive > 2)
        if self.no_counting: assert(no_counting)
        self._example_size_running = example_size
        if self._example_size_running > self.maximum_example_size_run:
            self.maximum_example_size_run = self._example_size_running
        self.is_idle = False
        self.keepalive = keepalive
        if self.thread is None:
            self.thread = threading.Thread(target=self._run_worker)
            print(f"# [worker] run_example_size. start worker thread ... ({self.synthesizer}, #eg: {self._example_size_running}, ka: {self.keepalive}, {self.example_file})")
            self.thread.start()
        else:
            print(f"# [worker] run_example_size. set example_size ... ({self.synthesizer}, #eg: {self._example_size_running}, ka: {self.keepalive}, {self.example_file})")
        
        # if self.keepalive <= 0:
        #     assert(self.thread is not None)

    def _run_example_size_autostart_blocking(self, example_size, no_counting):
        assert(self.no_counting == no_counting)
        if self.synthesizer == SYNTHESIZER_STRPROSE:
            # create process if not yet
            if self.synth_process is None:
                command_line_args = [
                    "dotnet",
                    TOOL_STRPROSE_FULLPATH,
                    "--synthloop",
                    os.path.join(EXAMPLE_STRPROSE_FULLDIR, self.example_file),
                    CACHE_STRPROSE_FULLDIR
                ]
                self.cmd = " ".join(command_line_args)
                print("# [worker] cmd:", self.cmd)
                self.synth_process = subprocess.Popen(self.cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
            # write input
            try:
                self.synth_process.stdin.write((str(example_size) + "\n").encode())
                self.synth_process.stdin.flush()

                # Blocking wait
                info_prefix1 = "# synth.currentVSASize: "
                info_prefix2 = "# synth.currentProgram: "
                vsa_size = None
                program = None
                while True: # read until results are shown
                    line = self.synth_process.stdout.readline().decode()
                    if len(line) == 0:
                        print("# [worker-readproc] WARNING: empty line! ")
                        break
                    line = line[:-1]
                    # print("# [worker-readproc-dbg]", line)
                    if line.startswith(info_prefix1):
                        vsa_size = int(line.replace(info_prefix1, ""))
                    if line.startswith(info_prefix2):
                        assert(vsa_size is not None)
                        program = line.replace(info_prefix2, "")
                        break
                
            except Exception as e:
                print(bcolors.error("# [worker] ERROR: StrPROSE subprocess failure."), e)
                self.is_failure = True
        
        elif self.synthesizer == SYNTHESIZER_STRSTUN:
            assert(self.synth_process is None)
            constraint_filename = self.example_file.split(".")[0] + ".sl"
            constraint_fullpath = os.path.join(COMP_STRSTUN_FULLDIR, constraint_filename)
            cache_log_filename = f"{self.example_file}.{example_size}.log"
            cache_log_fullpath = os.path.join(CACHE_STRSTUN_FULLDIR, cache_log_filename)
            # start StrSTUN
            no_counting_flag = " -nl" if no_counting else ""
            self.cmd = f"{TOOL_STRSTUN_FULLPATH}{no_counting_flag} -f{constraint_fullpath} -x{os.path.join(EXAMPLE_STRSTUN_FULLDIR, self.example_file)} -e{example_size} > {cache_log_fullpath} 2>&1"
            print("# [worker] cmd:", self.cmd)
            self.synth_process = subprocess.Popen(self.cmd, shell=True, preexec_fn=os.setsid)
            try:
                _, _ = self.synth_process.communicate()
            except Exception as e:
                print("# [worker] ERROR: StrSTUN subprocess failure.", e)
                self.is_failure = True

        else:
            print("# [worker] unknown synthesizer:", self.synthesizer)
            assert(False)
            

        cache_result = self.check_cache_callback({
            "synthesizer": self.synthesizer,
            "example_file": self.example_file,
            "no_counting": no_counting,
            "example_size": example_size
        })
        print("# [worker] _run_example_size blockingcall DONE.  eg:", example_size, " nc:", no_counting, " check_cache:", cache_result)

    def check_perf_status_once(self):
        if self.synth_process is not None:
            rss_mem = None
            try:
                proc_children = psutil.Process(self.synth_process.pid).children()
                if not (len(proc_children) == 1):
                    print("# [worker] WARN proc_children unexpected length. proc_children:", proc_children)
                    assert(False)
                rss_mem = proc_children[0].memory_info().rss
            except psutil.NoSuchProcess as e:
                print(bcolors.warn("# [worker] WARN: memory stat no such process:"), e)
                self.perf_status["rss"] = None
                self.perf_status["time"] = None
            except Exception as e:
                print(bcolors.error("# [worker] ERROR: memory stat is not working: Message:"), e)
                self.perf_status["rss"] = None
                self.perf_status["time"] = None
            
            if rss_mem is not None:
                self.perf_status["rss"] = rss_mem
                if self.perf_status["max_rss"] is None or self.perf_status["max_rss"] < rss_mem:
                    self.perf_status["max_rss"] = rss_mem
                self.perf_status["time"] = time.time()
                rss_mem_in_mb = rss_mem / 1024 / 1024
                if rss_mem_in_mb > self.mem_reservation_limit:
                    print(bcolors.warn("# [worker] FAILURE! worker exceed reservation limit:"), self.worker_id, " " + str(rss_mem_in_mb) + " > " + str(self.mem_reservation_limit))
                    print(bcolors.warn("# [worker] FAILURE! Killing process:"), self.synth_process.pid)
                    self.is_failure = True
                    # https://stackoverflow.com/questions/4789837/how-to-terminate-a-python-subprocess-launched-with-shell-true
                    # self.synth_process.kill() # This has problems
                    try:
                        os.killpg(os.getpgid(self.synth_process.pid), signal.SIGTERM)
                    except Exception as e:
                        print(bcolors.error(f"# [worker] ERROR failed to kill process group {self.synth_process.pid}: "), e)
                    self._release_synth_process()
        else:
            self.perf_status["rss"] = None
            self.perf_status["time"] = None

    def _release_synth_process(self):
        if self.synthesizer == SYNTHESIZER_STRPROSE:
            try:
                self.synth_process.stdin.write("-1\n".encode())
                self.synth_process.stdin.close()
            except Exception as e:
                print("# [worker] WARNING while releasing StrPROSE synth process:", e)
                try:
                    self.synth_process.kill()
                except Exception as e2:
                    print("# [worker] WARNING while killing StrPROSE synth process:", e2)
            self.synth_process.wait()
            print(bcolors.info("# [worker] " + self.worker_id + " is released."))
            self.is_released = True
        elif self.synthesizer == SYNTHESIZER_STRSTUN:
            self.synth_process.wait()
            print(bcolors.info("# [worker] " + self.worker_id + " is released."))
            self.is_released = True
        else:
            print("# [worker] _release_synth_process ERROR: unknown synthesizer", self.synthesizer)
            return
        # print("# [worker] _release_synth_process done. ", self.synthesizer, self.example_file)
    
    def _run_worker(self):
        delta_t = 0.125
        while True:
            if self._example_size_running is not None:
                assert self.is_idle == False
                if self._use_example_size_throttling:
                    throttled_example_size = self._example_size_running + self._example_size_throttle
                    print("# [worker] example_size throttled: ", self._example_size_running, "->", throttled_example_size)
                    self._run_example_size_autostart_blocking(throttled_example_size, self.no_counting)
                else:
                    self._run_example_size_autostart_blocking(self._example_size_running, self.no_counting)
                
                self.is_idle = True
                self._example_size_running = None
            if self.is_released: break
            self.keepalive -= delta_t
            if self.keepalive < 0 or self.is_failure: 
                self._release_synth_process()
                break
            time.sleep(delta_t)
        print(f"# [worker] thread DONE. ({self.synthesizer}, #eg: {self._example_size_running}, #nc: {self.no_counting}, ka: {self.keepalive}, {self.example_file})")
        