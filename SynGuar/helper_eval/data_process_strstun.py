from .eval_consts import *
from . import data_process_ops
import time
import math

FUNC_GET_STRSTUN_BENCHID = lambda x : x.split("$")[1].split(".")[0]
FUNC_GET_STRSTUN_SYNTH_EXAMPLESIZE = lambda x : int(x.split("$")[-1])

# ----------------------------- dataset collection
def collect_strstun_dataset():
    strstun_example_files = [x for x in list(os.listdir(EXAMPLE_STRSTUN_FULLDIR)) if x.endswith(".eg.txt")]
    print("#strstun_example_files", len(strstun_example_files))
    session_ids = []
    for example_file in strstun_example_files: 
        session_id = "$".join(["StrSTUN", example_file, "0.05", "0.02", "20"])
        session_ids.append(session_id)
    assert(len(session_ids) == 177)
    data_process_ops.get_session_dataset(session_ids) # preload
    print("# waiting ...")
    time.sleep(5)
    dataset = data_process_ops.get_session_dataset(session_ids) # collect
    return dataset

def collect_strstun_baseline4_dataset():
    strstun_example_files = [x for x in list(os.listdir(EXAMPLE_STRSTUN_FULLDIR)) if x.endswith(".eg.txt")]
    print("#strstun_example_files", len(strstun_example_files))
    synth_ids = []
    for example_file in strstun_example_files: 
        synth_id = "$".join(["StrSTUN", example_file, "4"])
        synth_ids.append(synth_id)
    assert(len(synth_ids) == 177)
    data_process_ops.get_synth_dataset(synth_ids)
    print("# waiting ...")
    time.sleep(5)
    dataset = data_process_ops.get_synth_dataset(synth_ids) # collect
    return dataset

def collect_strstun_chooseh_dataset():
    bench_info = load_json(SEED_STRSTUN_FULLPATH)
    h0_bench_ids = bench_info["found_H0_bench_ids"]
    assert(len(h0_bench_ids) == 45)
    print("#strstun h0_bench_ids:", len(h0_bench_ids))
    session_ids = []
    for bench_id in h0_bench_ids: 
        random_seeds = bench_info["bench_seeds"][bench_id]
        for random_seed in random_seeds:
            example_file = f"{bench_id}.seed{random_seed}.eg.txt"
            session_id = "$".join(["StrSTUN", example_file, "0.05", "0.02", "20"])
            session_ids.append(session_id)
    data_process_ops.get_session_dataset(session_ids) # preload
    print("# waiting ...")
    time.sleep(5)
    dataset = data_process_ops.get_session_dataset(session_ids) # collect
    return dataset

# ----------------------------- data analysis
def analysis_sample_size():
    print("\n# StrSTUN data analysis (sample size).")

    step1_path = ANALYSIS_SAMPLESIZE_STEPS_FULLPATH("StrSTUN", 1)
    def step1():
        dataset = collect_strstun_dataset()
        group_funcs = [data_process_ops.FUNC_GET_EPS_DELTA_K, FUNC_GET_STRSTUN_BENCHID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        map_func = lambda x : x["trace"]["total_bound"] if "total_bound" in x["trace"] else None
        mapped_dataset = data_process_ops.map(grouped_dataset, map_func, 3)
        return mapped_dataset
    mapped_dataset = data_process_ops.cached_step(step1_path, step1)


    step2_path = ANALYSIS_SAMPLESIZE_STEPS_FULLPATH("StrSTUN", 2)
    def step2():
        def agg_func(x):
            vals = [int(x[key]) for key in x if x[key] is not None]
            return sum(vals) / len(vals) if len(vals) > 0 else None
        agg_dataset = data_process_ops.aggregate(mapped_dataset, agg_func, 3)
        return agg_dataset
    agg_dataset = data_process_ops.cached_step(step2_path, step2)

    data_process_ops.dict_to_csv(agg_dataset, ANALYSIS_SAMPLESIZE_STRSTUN_FULLPATH)


def analysis_result_program():
    print("\n# StrSTUN data analysis (result program).")

    step1_path = ANALYSIS_RESULTPROG_STEPS_FULLPATH("StrSTUN", 1)
    def step1():
        dataset = collect_strstun_dataset()
        group_funcs = [data_process_ops.FUNC_GET_EPS_DELTA_K, FUNC_GET_STRSTUN_BENCHID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        map_func = lambda x : x["trace"]["final_result"]["program"] if "final_result" in x["trace"] else None
        mapped_dataset_sub = data_process_ops.map(grouped_dataset, map_func, 3)["0.05$0.02$20"]
        return mapped_dataset_sub
    data_process_ops.cached_step(step1_path, step1)


def analysis_resource_usage():
    print("\n# StrSTUN data analysis (resourse usage).")

    step1_path = ANALYSIS_RESOURCE_USAGE_STEPS_FULLPATH("StrSTUN", 1)
    def step1():
        dataset = collect_strstun_dataset()
        group_funcs = [data_process_ops.FUNC_GET_EPS_DELTA_K, FUNC_GET_STRSTUN_BENCHID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        def map_func(x):
            example_file = x["status"]["example_file"]
            if "final_result" in x["trace"]:
                keys = [key for key in x["trace"]["middle_steps"]]
                datas = sorted([(int(key), data_process_ops.get_synth_data_p("StrSTUN", example_file, key)["result"]["info"]["time"]) for key in keys])
                datas.append((x["trace"]["final_result"]["info"]["exampleCount"], x["trace"]["final_result"]["info"]["time"]))
                return datas
            else:
                return None
        mapped_dataset_sub = data_process_ops.map(grouped_dataset, map_func, 3)["0.05$0.02$20"]
        return mapped_dataset_sub
    mapped_dataset_sub = data_process_ops.cached_step(step1_path, step1)

    step2_path = ANALYSIS_RESOURCE_USAGE_STEPS_FULLPATH("StrSTUN", 2)
    def step2():
        def agg_func(x):
            if x is None: return None
            val_lists = [y[1] for y in x]
            return sum(val_lists)       
        agg_dataset = data_process_ops.aggregate(mapped_dataset_sub, agg_func, 3)
        def map_func(x):
            return {key.split(".eg")[0].split(".")[1]: x[key] for key in x}
        agg_dataset = data_process_ops.aggregate(mapped_dataset_sub, agg_func, 3)
        agg_dataset = data_process_ops.map(agg_dataset, map_func, 1)
        return agg_dataset
    agg_dataset = data_process_ops.cached_step(step2_path, step2)
    data_process_ops.dict_to_csv(agg_dataset, ANALYSIS_RESOURCE_USAGE_STRSTUN_FULLPATH, is_transpose=True)

def analysis_baseline_4examples():
    print("\n# StrSTUN data analysis (baseline 4 examples).")

    step1_path = ANALYSIS_BASELINE4_STEPS_FULLPATH("StrSTUN", 1)
    def step1():
        dataset = collect_strstun_baseline4_dataset()
        group_funcs = [FUNC_GET_STRSTUN_BENCHID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        map_func = lambda x : x["result"]["program"]
        mapped_dataset_sub = data_process_ops.map(grouped_dataset, map_func, 2)
        return mapped_dataset_sub
    data_process_ops.cached_step(step1_path, step1)


def analysis_choose_h():
    print("\n# StrSTUN data analysis (choose h).")

    step1_path = ANALYSIS_CHOOSEH_STEPS_FULLPATH("StrSTUN", 1)
    def step1():
        dataset = collect_strstun_chooseh_dataset()
        group_funcs = [FUNC_GET_STRSTUN_BENCHID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        def map_func(entry):
            trace = entry["trace"]
            return {
                "last_middle_step": trace["last_middle_step"],
                "last_middle_step_bound": trace["middle_steps"][str(trace["last_middle_step"])]["current_bound"]
            }
        mapped_dataset_sub = data_process_ops.map(grouped_dataset, map_func, 2)
        return mapped_dataset_sub
    mapped_dataset_sub = data_process_ops.cached_step(step1_path, step1)

    step2_path = ANALYSIS_CHOOSEH_STEPS_FULLPATH("StrSTUN", 2)
    def step2():
        def agg_func(x):
            val_lists = [x[key]["last_middle_step_bound"] for key in x]
            return {f"H{i}": sum([val_list[i] for val_list in val_lists]) / 3 for i in range(3)}
                    
        agg_dataset = data_process_ops.aggregate(mapped_dataset_sub, agg_func, 2)
        return agg_dataset
    agg_dataset = data_process_ops.cached_step(step2_path, step2)
    
    data_process_ops.dict_to_csv(agg_dataset, ANALYSIS_CHOOSEH_STRSTUN_FULLPATH, is_transpose=True)