import os
import csv
from . import stun_log_parser



class CacheReader():
    def __init__(self, strprose_cache_dir, strstun_cache_dir):
        print("# [cache] object created.")
        print("# [cache] strprose_cache_dir: ", strprose_cache_dir)
        print("# [cache] strstun_cache_dir: ", strstun_cache_dir)
        self.strprose_cache_dir = strprose_cache_dir
        self.strstun_cache_dir = strstun_cache_dir
        self._result_cache_dict = {}

    def get_result_cache_key(self, taskdata):
        synthesizer = taskdata["synthesizer"]
        example_file = taskdata["example_file"]
        example_size = taskdata["example_size"]
        return f"{synthesizer}|{example_file}|{example_size}"

    def if_exist_result_for_task_false_negative_allowed(self, taskdata):
        cache_key = self.get_result_cache_key(taskdata)
        if cache_key in self._result_cache_dict:
            return True
        else:
            return False

    def check_cache(self, taskdata):
        synthesizer = taskdata["synthesizer"]
        example_file = taskdata["example_file"]
        example_size = taskdata["example_size"]
        no_counting = taskdata["no_counting"]
        
        # if in memory, no need to read file
        cache_key = self.get_result_cache_key(taskdata)
        if cache_key in self._result_cache_dict:
            return self._result_cache_dict[cache_key]
        
        # not in memory, check file
        result = None
        if synthesizer == "StrPROSE":
            result = self._check_strprose_cache(example_file, example_size, no_counting=no_counting)
        elif synthesizer == "StrSTUN":
            result = self._check_strstun_cache(example_file, example_size, no_counting=no_counting)
        if result is not None:
            self._result_cache_dict[cache_key] = result
        # return result
        return result

    def _check_strprose_cache(self, example_file, example_size, no_counting):
        cache_file = example_file + ".cache.csv"
        cache_fullpath = os.path.join(self.strprose_cache_dir, cache_file)
        if os.path.exists(cache_fullpath):
            with open(cache_fullpath, 'r') as f:
                rows = [x for x in csv.reader(f)]
                if len(rows) == 0:
                    print("# [cachereader] WARNING empty cache file:", cache_file)
                    return None
                assert(len(rows[0]) == 5)
                # TODO: check index here.
                if len(rows) > example_size:
                    cache_data_raw = rows[example_size]
                    assert(len(cache_data_raw) == 5)
                    return {
                        "hs": [cache_data_raw[2]],
                        "program": cache_data_raw[4],
                        "valid_hs": True
                    }
                else:
                    return None
                

    def _check_strstun_cache(self, example_file, example_size, no_counting):
        cache_file = example_file + "." + str(example_size) + ".log"
        cache_fullpath = os.path.join(self.strstun_cache_dir, cache_file)
        if os.path.exists(cache_fullpath):
            with open(cache_fullpath, 'r') as f:
                content = f.read()
                parsed, is_complete, valid_hs = stun_log_parser.parse_stun_log(content)
                if is_complete:
                    if no_counting:
                        return parsed
                    if valid_hs:
                        return parsed
                    print("# [CacheReader] WARNING StrSTUN cache: require counting but no counting. Return None.")
                    return None
                return None
        return None