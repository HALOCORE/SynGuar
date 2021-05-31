import requests
import csv
from .eval_consts import *
import time

FUNC_GET_EPS_DELTA_K = lambda x : "$".join(x.split("$")[2:])

# ---------------------- get data functions

def get_session_data(session_id):
    params = session_id.split("$")
    return get_session_data_p(*params)

def get_session_data_p(synthesizer, example_file, epsilon, delta, k):
    request_data = {
        "synthesizer": synthesizer,
        "example_file": example_file,
        "epsilon": epsilon,
        "delta": delta,
        "cache_only":  True,
        "k": k
    }
    print("# [eval] POST ", request_data)
    resp = requests.post(SYNGUAR_API_ENDPOINT, json=request_data)
    if resp.status_code != 200:
        print("# [eval] ERROR: status_code", resp.status_code)
        assert(False)
    data = resp.json()
    return data

def get_synth_data(synth_id):
    params = synth_id.split("$")
    return get_synth_data_p(*params)

def get_synth_data_p(synthesizer, example_file, sample_size):
    payload = {
        "synthesizer": synthesizer,
        "example_file": example_file,
        "example_size": sample_size,
        "no_counting": False,
        "cache_only":  True,
        "keepalive": 0
    }
    print("# [eval] POST ", payload)
    resp = requests.post(SYNTH_API_ENDPOINT, json=payload)
    if resp.status_code != 200:
        print("# [eval] ERROR: status_code", resp.status_code)
        assert(False)
    data = resp.json()
    return data

def get_session_dataset(list_of_session_ids):
    # return a dict
    dataset = {}
    for session_id in list_of_session_ids:
        dataset[session_id] = get_session_data(session_id)
    return dataset

def get_synth_dataset(list_of_synth_ids):
    dataset = {}
    for synth_id in list_of_synth_ids:
        dataset[synth_id] = get_synth_data(synth_id)
    return dataset



# ---------------------- general functions

def group_by(dataset, key_funcs):
    # dataset must be a plain dict.
    # multi-level dict grouped by keys
    if (len(key_funcs) == 0):
        return dataset
    assert(len(key_funcs) > 0)
    current_func = key_funcs[0]
    rest_funcs = key_funcs[1:]
    # group it using current_func
    temp_dict = {}
    for datakey in dataset:
        group_val = current_func(datakey)
        if group_val not in temp_dict:
            temp_dict[group_val] = {}
        temp_dict[group_val][datakey] = dataset[datakey]
    result_dict = {group_val : group_by(temp_dict[group_val], rest_funcs) for group_val in temp_dict}
    return result_dict

def map(datadict, map_func, map_depth, current_depth=1):
    # return a dict
    # same structure, each element mapped
    mapped_dict = {}
    for dkey in datadict:
        delem = datadict[dkey]
        if current_depth == map_depth:
            mapped_dict[dkey] = map_func(delem)
        else:
            assert current_depth < map_depth
            mapped_dict[dkey] = map(delem, map_func, map_depth, current_depth + 1)
    return mapped_dict


def aggregate(datadict, aggregate_func, agg_depth, current_depth=1):
    # aggregate at the specified depth    
    if current_depth == agg_depth:
        return aggregate_func(datadict)
    else:
        mapped_dict = {}
        for dkey in datadict:
            delem = datadict[dkey]
            assert current_depth < agg_depth
            mapped_dict[dkey] = aggregate(delem, aggregate_func, agg_depth, current_depth + 1)
        return mapped_dict

def dict_to_csv(datadict, csvpath, col_order_func=lambda x : x, row_order_func=lambda x : x, is_transpose=False):
    unsorted_colnames = list(dict.keys(datadict))
    unsorted_rownames = list(dict.keys(datadict[unsorted_colnames[0]]))
    colnames = [v[1] for v in sorted([(col_order_func(k), k) for k in unsorted_colnames])]
    rownames = [v[1] for v in sorted([(row_order_func(k), k) for k in unsorted_rownames])]
    rows = [[""] + colnames]
    for rowname in rownames:
        row = [rowname]
        for colname in colnames:
            row.append(datadict[colname][rowname])
        rows.append(row)
    if is_transpose:
        rows = list(zip(*rows))
    with open(csvpath, 'w', newline='') as f:
        writer = csv.writer(f, quoting=csv.QUOTE_MINIMAL)
        for row in rows:
            assert(len(row) == len(rows[0]))
            writer.writerow(row)
    print("# [eval] dict_to_csv. head:", rows[0], " path:", csvpath)

