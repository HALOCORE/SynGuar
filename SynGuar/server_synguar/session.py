from os import wait
import threading
import requests
import time

from .common import *
from .synguar_algo import synguar_3H, synguar_singleH

KEEPALIVE_STRPROSE = 15
KEEPALIVE_STRSTUN = 0

is_number = lambda x : type(x) == int or type(x) == float
class Session():
    def __init__(self, synthesizer, example_file, epsilon, delta, k, api_endpoint, full_trace_ref):
        self.synthesizer = synthesizer
        self.example_file = example_file
        assert(is_number(epsilon))
        assert(epsilon > 0 and epsilon < 1)
        assert(is_number(delta))
        assert(epsilon > 0 and epsilon < 1)
        assert(is_number(k))
        assert(k > 0 and k == int(k))
        self.epsilon = epsilon
        self.delta = delta
        self.k = k
        
        self.full_trace_ref = full_trace_ref
        
        self.running_status = "WAITING"
        self.session_thread = None
        self.api_endpoint = api_endpoint
        
    def start(self):
        self.session_thread = threading.Thread(target=self._run_session)
        print(f"# [session] start: {self.synthesizer} | {self.example_file} | e={self.epsilon} | d={self.delta} | k={self.k} ...")
        self.running_status = "RUNNING"
        self.session_thread.start()

    def wait_for_api_result(self, synthesizer, example_file, example_size, cache_only, no_counting):
        request_data = {
            "synthesizer": synthesizer,
            "example_file": example_file,
            "example_size": example_size,
            "keepalive": None,
            "no_counting": no_counting,
            "_cache_only": cache_only
        }
        if synthesizer == "StrSTUN": 
            request_data["keepalive"] = KEEPALIVE_STRSTUN
        elif synthesizer == "StrPROSE":
            request_data["keepalive"] = KEEPALIVE_STRPROSE
        else:
            assert(False)
        
        # TODO: send api request
        resp_data = None
        result_data = None
        wait_time = 0.0625
        while(True):
            resp_data = None
            try:
                resp = requests.post(self.api_endpoint, json=request_data)
                if resp.status_code != 200:
                    print("# [session] WARNING try send api request failed. status_code:", resp_data.status_code)
                    raise Exception("Status_Code_Not_200")
                resp_data = resp.json()
            except Exception as e:
                print(bcolors.warn("# [session] WARNING request failed:"), e)
            # check if resp_data is OK
            if resp_data is not None and resp_data["syntask_status"] == "DONE":
                assert(resp_data["syntask_data"]["example_file"] == example_file)
                assert(resp_data["syntask_data"]["example_size"] == example_size)
                result_data = resp_data["result"]
                break
            else:
                time.sleep(wait_time)
                wait_time *= 2
                if wait_time > 4:
                    wait_time = 4
        print(f"# [session] wait_for_api_result Returning  {synthesizer} | {example_file} | {example_size} : {result_data['hs']}")
        return result_data

    def _run_session(self):
        def get_result_with_sample_size(sample_size, no_counting=False):
            return self.wait_for_api_result(self.synthesizer, self.example_file, sample_size, cache_only=False, no_counting=no_counting)
        def get_result_with_sample_size_cache_only(sample_size, no_counting=False):
            return self.wait_for_api_result(self.synthesizer, self.example_file, sample_size, cache_only=True, no_counting=no_counting)
        if self.synthesizer == "StrPROSE":
            self.full_trace = synguar_singleH(self.epsilon, self.delta, self.k, self.full_trace_ref, get_result_with_sample_size)
        elif self.synthesizer == "StrSTUN":
            self.full_trace = synguar_3H(self.epsilon, self.delta, self.k, self.full_trace_ref, get_result_with_sample_size) # get_result_with_sample_size_cache_only
        else:
            print("# [session] ERROR Unknown synthesizer:", self.synthesizer)
            assert(False)
        print(f"# [session] Done. {self.synthesizer} | {self.example_file} | {self.epsilon} | {self.delta} | {self.k}")
        self.running_status = "DONE"