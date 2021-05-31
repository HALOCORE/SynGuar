import os
import requests
import random

from .eval_consts import *

def invoke_strprose_evaluation_requests():
    strprose_example_files = [x for x in list(os.listdir(EXAMPLE_STRPROSE_FULLDIR)) if x.endswith(".csv")]
    print("#strprose_example_files", len(strprose_example_files))
    
    random.shuffle(strprose_example_files)
    for example_file in strprose_example_files:
        request_data = {
            "synthesizer": "StrPROSE",
            "example_file": example_file,
            "epsilon": 0.05,
            "delta": 0.02,
            "k": 1
        }
        print("# [eval] POST ", request_data)
        resp = requests.post(SYNGUAR_API_ENDPOINT, json=request_data)
        if resp.status_code != 200:
            print("# [eval] ERROR: status_code", resp.status_code)
            assert(False)

def invoke_strprose_spacedrop_requests():
    strprose_example_files = [x for x in list(os.listdir(EXAMPLE_STRPROSE_FULLDIR)) if x.endswith(".csv")]
    print("#strprose_example_files", len(strprose_example_files))
    
    random.shuffle(strprose_example_files)
    for example_file in strprose_example_files:
        request_data = {
            "synthesizer": "StrPROSE",
            "example_file": example_file,
            "example_size": STRPROSE_SPACEDROP_SAMPLE_SIZE,
            "no_counting": False,
            "cache_only":  False,
            "keepalive": 0
        }
        print("# [eval] POST ", request_data)
        resp = requests.post(SYNTH_API_ENDPOINT, json=request_data)
        if resp.status_code != 200:
            print("# [eval] ERROR: status_code", resp.status_code)
            assert(False)
        

def invoke_strstun_evaluation_requests():
    strstun_example_files = [x for x in list(os.listdir(EXAMPLE_STRSTUN_FULLDIR)) if x.endswith(".eg.txt")]
    print("#strstun_example_files", len(strstun_example_files))
    
    random.shuffle(strstun_example_files)
    for example_file in strstun_example_files:
        request_data = {
            "synthesizer": "StrSTUN",
            "example_file": example_file,
            "epsilon": 0.05,
            "delta": 0.02,
            "k": 20
        }
        print("# [eval] POST ", request_data)
        resp = requests.post(SYNGUAR_API_ENDPOINT, json=request_data)
        if resp.status_code != 200:
            print("# [eval] ERROR: status_code", resp.status_code)
            assert(False)


def invoke_strstun_4examples_requests():
    strstun_example_files = [x for x in list(os.listdir(EXAMPLE_STRSTUN_FULLDIR)) if x.endswith(".eg.txt")]
    print("#strstun_example_files", len(strstun_example_files))
    
    random.shuffle(strstun_example_files)
    for example_file in strstun_example_files:
        request_data = {
            "synthesizer": "StrSTUN",
            "example_file": example_file,
            "example_size": 4,
            "no_counting": False,
            "cache_only":  False,
            "keepalive": 0
        }
        print("# [eval] POST ", request_data)
        resp = requests.post(SYNTH_API_ENDPOINT, json=request_data)
        if resp.status_code != 200:
            print("# [eval] ERROR: status_code", resp.status_code)
            assert(False)