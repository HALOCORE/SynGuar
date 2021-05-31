# imports
import os
import json
import subprocess
abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))


# constants
SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
SEED_RELPATH = "./strprose/example_files/_seeds.json"
SEED_FULLPATH = abs_join(SCRIPT_DIR, SEED_RELPATH)
SEED_INFO = None
with open(SEED_FULLPATH, 'r') as f:
    SEED_INFO = json.load(f)
TOOL_RELPATH = "../StrPROSE-synthesizer/StrPROSE/bin/Debug/netcoreapp3.1/StrPROSE.dll"
TOOL_FULLPATH = abs_join(SCRIPT_DIR, TOOL_RELPATH)
TARGET_RELDIR = "./strprose/targets"
TARGET_FULLDIR = abs_join(SCRIPT_DIR, TARGET_RELDIR)
MAX_SAMPLE_SIZE = 2000
EXAMPLE_RELDIR = "./strprose/example_files"
EXAMPLE_FULLDIR = abs_join(SCRIPT_DIR, EXAMPLE_RELDIR)
TIME_OUT = 120

# methods
def generate_examples(bench_id, seed):
    command_line_args = [
        "dotnet",
        TOOL_FULLPATH,
        "--samplegen",
        TARGET_FULLDIR,
        str(bench_id),
        str(seed),
        str(MAX_SAMPLE_SIZE),
        EXAMPLE_FULLDIR
    ]
    try:
        print(f"# -------- Start Process ({bench_id}, {seed}) --------")
        done_result = subprocess.run(command_line_args, timeout=TIME_OUT)
        print(f"# ^^^^^^^^ Done: {done_result.returncode} ({bench_id}, {seed}) ^^^^^^^^")
    except subprocess.TimeoutExpired:
        print('# Error: subprocess TIMEOUT !!!')


if __name__ == "__main__":
    for bench_id in SEED_INFO["bench_seeds"]:
        for seed in SEED_INFO["bench_seeds"][bench_id]:
            generate_examples(bench_id, seed)