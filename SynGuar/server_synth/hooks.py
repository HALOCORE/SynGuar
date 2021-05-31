import json
import os

abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))
SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
TEMP_FULLDIR = abs_join(SCRIPT_DIR, "../../outputs/temp")


def _print_line_to_temp_file(line, filename):
    full_filename = os.path.join(TEMP_FULLDIR, filename)
    with open(full_filename, 'a') as f:
        f.write(line + "\n")

_hook_SCHEDULER_process_synth_task_dedup_dict = {}
def hook_SCHEDULER_process_synth_task(data):
    try:
        if "no_counting" in data and data["no_counting"]:
            dataline = json.dumps(data)
            if dataline in _hook_SCHEDULER_process_synth_task_dedup_dict:
                return
            _hook_SCHEDULER_process_synth_task_dedup_dict[dataline] = True
            _print_line_to_temp_file(dataline, "synth_task_no_counting.txt")
    except Exception as e:
        print("# [hook] execution failed:", e)