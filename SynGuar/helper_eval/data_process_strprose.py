from .eval_consts import *
from . import data_process_ops
import time
import math

FUNC_GET_STRPROSE_PROGID = lambda x : x.split("$")[1].split(".")[0]
FUNC_GET_STRPROSE_SYNTH_EXAMPLESIZE = lambda x : int(x.split("$")[-1])

# ----------------------------- dataset collection
def collect_strprose_dataset():
    strprose_example_files = [x for x in list(os.listdir(EXAMPLE_STRPROSE_FULLDIR)) if x.endswith(".csv")]
    print("#strprose_example_files", len(strprose_example_files))
    session_ids = []
    for epsilon in ["0.05", "0.02", "0.1"]:
        for example_file in strprose_example_files: 
            session_id = "$".join(["StrPROSE", example_file, epsilon, "0.02", "1"])
            session_ids.append(session_id)
    assert(len(session_ids) == 1536)
    data_process_ops.get_session_dataset(session_ids) # preload
    print("# waiting ...")
    time.sleep(5)
    dataset = data_process_ops.get_session_dataset(session_ids) # collect
    return dataset

def collect_strprose_targets():
    files = [x for x in list(os.listdir(TARGET_STRPROSE_FULLDIR)) if x.endswith(".csv.txt")]
    target_dict = {}
    for file in files:
        target_path = os.path.join(TARGET_STRPROSE_FULLDIR, file)
        with open(target_path, 'r') as f:
            content = f.read()
            prog_id = file.replace(".csv.txt", "")
            target_dict[prog_id] = content
    return target_dict

def collect_strprose_baseline4_dataset():
    strprose_example_files = [x for x in list(os.listdir(EXAMPLE_STRPROSE_FULLDIR)) if x.endswith(".csv")]
    print("#strprose_example_files", len(strprose_example_files))
    synth_ids = []
    for example_file in strprose_example_files: 
        synth_id = "$".join(["StrPROSE", example_file, "4"])
        synth_ids.append(synth_id)
    assert(len(synth_ids) == 512)
    data_process_ops.get_synth_dataset(synth_ids)
    print("# waiting ...")
    time.sleep(5)
    dataset = data_process_ops.get_synth_dataset(synth_ids) # collect
    return dataset

def collect_strprose_spacedrop_dataset():
    strprose_example_files = [x for x in list(os.listdir(EXAMPLE_STRPROSE_FULLDIR)) if x.endswith(".csv")]
    print("#strprose_example_files", len(strprose_example_files))
    synth_ids = []
    for example_file in strprose_example_files: 
        synth_id = "$".join(["StrPROSE", example_file, str(STRPROSE_SPACEDROP_SAMPLE_SIZE)])
        synth_ids.append(synth_id)
    assert(len(synth_ids) == 512)
    data_process_ops.get_synth_dataset(synth_ids) # preload
    print("# waiting ...")
    time.sleep(5)

    full_synth_ids = []
    for example_file in strprose_example_files: 
        for i in range(1, STRPROSE_SPACEDROP_SAMPLE_SIZE + 1):
            synth_id = "$".join(["StrPROSE", example_file, str(i)])
            full_synth_ids.append(synth_id)
    assert(len(full_synth_ids) == 512 * STRPROSE_SPACEDROP_SAMPLE_SIZE)
    print("# [eval] Collect dataset for max sample size:", STRPROSE_SPACEDROP_SAMPLE_SIZE)
    dataset = data_process_ops.get_synth_dataset(full_synth_ids) # collect
    return dataset

# ----------------------------- data analysis
def analysis_sample_size():
    print("\n# StrPROSE data analysis (sample size).")

    step1_path = ANALYSIS_SAMPLESIZE_STEPS_FULLPATH("StrPROSE", 1)
    def step1():
        dataset = collect_strprose_dataset()
        group_funcs = [data_process_ops.FUNC_GET_EPS_DELTA_K, FUNC_GET_STRPROSE_PROGID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        map_func = lambda x : x["trace"]["total_bound"][0]
        mapped_dataset = data_process_ops.map(grouped_dataset, map_func, 3)
        return mapped_dataset
    mapped_dataset = data_process_ops.cached_step(step1_path, step1)


    step2_path = ANALYSIS_SAMPLESIZE_STEPS_FULLPATH("StrPROSE", 2)
    def step2():
        agg_func = lambda x : sum([x[key] for key in x]) / len(x)
        agg_dataset = data_process_ops.aggregate(mapped_dataset, agg_func, 3)
        return agg_dataset
    agg_dataset = data_process_ops.cached_step(step2_path, step2)

    data_process_ops.dict_to_csv(agg_dataset, ANALYSIS_SAMPLESIZE_STRPROSE_FULLPATH)


def analysis_result_program():
    print("\n# StrPROSE data analysis (result program).")

    step1_path = ANALYSIS_RESULTPROG_STEPS_FULLPATH("StrPROSE", 1)
    def step1():
        dataset = collect_strprose_dataset()
        group_funcs = [data_process_ops.FUNC_GET_EPS_DELTA_K, FUNC_GET_STRPROSE_PROGID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        map_func = lambda x : x["trace"]["final_result"]["program"]
        mapped_dataset_sub = data_process_ops.map(grouped_dataset, map_func, 3)["0.05$0.02$1"]
        return mapped_dataset_sub
    mapped_dataset_sub = data_process_ops.cached_step(step1_path, step1)


    step2_path = ANALYSIS_RESULTPROG_STEPS_FULLPATH("StrPROSE", 2)
    def step2():
        targets = collect_strprose_targets()
        def agg_func(x):
            equal_count = 0
            non_equal_count = 0
            for k in x:
                synth_prog = x[k]
                progid = k.split("$")[1].split(".")[0]
                bench_prog = targets[progid]
                if bench_prog == synth_prog: equal_count += 1
                else: non_equal_count += 1
            assert(equal_count + non_equal_count == 32)
            return equal_count
        agg_dataset_sub = data_process_ops.aggregate(mapped_dataset_sub, agg_func, 2)
        return agg_dataset_sub
    data_process_ops.cached_step(step2_path, step2)


def analysis_baseline_4examples():
    print("\n# StrPROSE data analysis (baseline 4 examples).")

    step1_path = ANALYSIS_BASELINE4_STEPS_FULLPATH("StrPROSE", 1)
    def step1():
        dataset = collect_strprose_baseline4_dataset()
        group_funcs = [FUNC_GET_STRPROSE_PROGID]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        map_func = lambda x : x["result"]["program"]
        mapped_dataset_sub = data_process_ops.map(grouped_dataset, map_func, 2)
        return mapped_dataset_sub
    mapped_dataset_sub = data_process_ops.cached_step(step1_path, step1)
    
    step2_path = ANALYSIS_BASELINE4_STEPS_FULLPATH("StrPROSE", 2)
    def step2():
        targets = collect_strprose_targets()
        def agg_func(x):
            equal_count = 0
            non_equal_count = 0
            for k in x:
                synth_prog = x[k]
                progid = k.split("$")[1].split(".")[0]
                bench_prog = targets[progid]
                if bench_prog == synth_prog: equal_count += 1
                else: non_equal_count += 1
            assert(equal_count + non_equal_count == 32)
            return equal_count
        agg_dataset_sub = data_process_ops.aggregate(mapped_dataset_sub, agg_func, 2)
        sum_all = sum([agg_dataset_sub[k] for k in agg_dataset_sub])
        agg_dataset_sub["__sum"] = sum_all
        return agg_dataset_sub
    data_process_ops.cached_step(step2_path, step2)


def analysis_spacedrop():
    print("\n# StrPROSE data analysis (spacedrop).")

    step1_path = ANALYSIS_SPACEDROP_STEPS_FULLPATH("StrPROSE", 1)
    def step1():
        dataset = collect_strprose_spacedrop_dataset()
        group_funcs = [FUNC_GET_STRPROSE_PROGID, FUNC_GET_STRPROSE_SYNTH_EXAMPLESIZE]
        grouped_dataset = data_process_ops.group_by(dataset, group_funcs)
        map_func = lambda x : x["result"]["hs"][0]
        mapped_dataset_sub = data_process_ops.map(grouped_dataset, map_func, 3)
        def agg_func(x):
            egsize =[k.split("$")[-1] for k in x]
            assert(len(egsize) == 32 and len(set(egsize)) == 1)
            vals = [str(x[k]) for k in sorted(list(dict.keys(x)))]
            return ",".join(vals)
        agg_dataset_sub = data_process_ops.aggregate(mapped_dataset_sub, agg_func, 3)
        return agg_dataset_sub
    agg_dataset_sub = data_process_ops.cached_step(step1_path, step1)
    
    step2_path = ANALYSIS_SPACEDROP_STEPS_FULLPATH("StrPROSE", 2)
    def step2():
        def map_func(x):
            vals = [int(v) for v in x.split(",")]
            return math.exp(sum([math.log(v) for v in vals]) / len(vals))
        log_avg_dataset = data_process_ops.map(agg_dataset_sub, map_func, 2)
        return log_avg_dataset
    log_avg_dataset = data_process_ops.cached_step(step2_path, step2)

    data_process_ops.dict_to_csv(
        log_avg_dataset, 
        ANALYSIS_SPACEDROP_STRPROSE_FULLPATH,
        col_order_func=lambda x : int(x.replace("prog", "")),
        row_order_func=lambda x : int(x))
