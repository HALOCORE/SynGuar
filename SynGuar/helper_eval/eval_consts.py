import os
import json

abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))
SYNTHESIZER_STRPROSE = "StrPROSE"
SYNTHESIZER_STRSTUN = "StrSTUN"

SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
EXAMPLE_STRPROSE_RELDIR = "../../benchmark/strprose/example_files"
EXAMPLE_STRPROSE_FULLDIR = abs_join(SCRIPT_DIR, EXAMPLE_STRPROSE_RELDIR)
EXAMPLE_STRSTUN_RELDIR = "../../benchmark/strstun/example_files"
EXAMPLE_STRSTUN_FULLDIR = abs_join(SCRIPT_DIR, EXAMPLE_STRSTUN_RELDIR)
COMP_STRSTUN_RELDIR = "../../benchmark/strstun/targets"
COMP_STRSTUN_FULLDIR = abs_join(SCRIPT_DIR, COMP_STRSTUN_RELDIR)
SEED_STRSTUN_FULLPATH = abs_join(SCRIPT_DIR, "../../benchmark/strstun/example_files/_seeds.json")

TARGET_STRPROSE_RELDIR = "../../benchmark/strprose/targets"
TARGET_STRPROSE_FULLDIR = abs_join(SCRIPT_DIR, TARGET_STRPROSE_RELDIR)
TARGET_STRSTUN_RELDIR = "../../benchmark/strstun/targets"
TARGET_STRSTUN_FULLDIR = abs_join(SCRIPT_DIR, TARGET_STRSTUN_RELDIR)

SUMMARY_RELDIR = "../../outputs/summaries"
SUMMARY_FULLDIR = abs_join(SCRIPT_DIR, SUMMARY_RELDIR)

ANALYSIS_SAMPLESIZE_STEPS_FULLPATH = lambda synthesizer, x : abs_join(SUMMARY_FULLDIR, f"{synthesizer}_samplesize_step{x}.json")
ANALYSIS_SAMPLESIZE_STRPROSE_FULLPATH = abs_join(SUMMARY_FULLDIR, "StrPROSE-samplesize.csv")
ANALYSIS_SAMPLESIZE_STRSTUN_FULLPATH = abs_join(SUMMARY_FULLDIR, "StrSTUN-samplesize.csv")
ANALYSIS_RESULTPROG_STEPS_FULLPATH = lambda synthesizer, x : abs_join(SUMMARY_FULLDIR, f"{synthesizer}_resultprog_step{x}.json")
ANALYSIS_RESOURCE_USAGE_STEPS_FULLPATH = lambda synthesizer, x : abs_join(SUMMARY_FULLDIR, f"{synthesizer}_resourceusage_step{x}.json")
ANALYSIS_RESOURCE_USAGE_STRSTUN_FULLPATH = abs_join(SUMMARY_FULLDIR, f"StrSTUN-resourceusage.csv")
ANALYSIS_BASELINE4_STEPS_FULLPATH = lambda synthesizer, x : abs_join(SUMMARY_FULLDIR, f"{synthesizer}_baseline4_step{x}.json")
ANALYSIS_CHOOSEH_STEPS_FULLPATH = lambda synthesizer, x : abs_join(SUMMARY_FULLDIR, f"{synthesizer}_chooseh_step{x}.json")
ANALYSIS_CHOOSEH_STRSTUN_FULLPATH = abs_join(SUMMARY_FULLDIR, "StrSTUN-chooseh.csv")
ANALYSIS_SPACEDROP_STEPS_FULLPATH = lambda synthesizer, x : abs_join(SUMMARY_FULLDIR, f"{synthesizer}_spacedrop_step{x}.json")
ANALYSIS_SPACEDROP_STRPROSE_FULLPATH = abs_join(SUMMARY_FULLDIR, "StrPROSE-spacedrop.csv")

SYNGUAR_API_ENDPOINT = "http://localhost:5262/synguar"
SYNTH_API_ENDPOINT = "http://localhost:5261/synth"

STRPROSE_SPACEDROP_SAMPLE_SIZE = 30


def load_json(filename):
    print("# [eval] load_json:", filename)
    data = None
    with open(filename, 'r') as f:
        data = json.load(f)
    return data

def save_json(filename, data):
    print("# [eval] save_json:", filename)
    with open(filename, 'w') as f:
        json.dump(data, f, indent=2)


def cached_step(step_cachepath, step_func):
    print("\n# [eval] cached_step. Checkpath: " + step_cachepath)
    if os.path.exists(step_cachepath):
        step_cache = load_json(step_cachepath)
        if step_cache is not None:
            return step_cache
    # run step_func and save.
    data = step_func()
    save_json(step_cachepath, data)
    return data