import os
import json
abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))


SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
SEED_RELPATH = "./strstun/example_files/_seeds.json"
SEED_FULLPATH = abs_join(SCRIPT_DIR, SEED_RELPATH)
SEED_INFO = None
with open(SEED_FULLPATH, 'r') as f:
    SEED_INFO = json.load(f)
TOOL_RELPATH = './generate_examples_strstun_onefile.py'
TOOL_WORKINGDIR = SCRIPT_DIR
TARGET_RELDIR = "./strstun/targets"
TARGET_FULLDIR = abs_join(SCRIPT_DIR, TARGET_RELDIR)
PROCESSED_PATH = os.path.abspath("../PBE_SLIA_Track/")
SAMPLE_PATH = os.path.abspath("./")
MAX_SAMPLE_SIZE = 2000
EXAMPLE_RELDIR = "./strstun/example_files"
EXAMPLE_FULLDIR = abs_join(SCRIPT_DIR, EXAMPLE_RELDIR)


def get_examples(filename):
    with open(filename, 'r') as f:
        all_parts = f.read().split("\n------\n")
        head = all_parts[0]
        all_parts = all_parts[1:]
        all_parts[-1] = all_parts[-1].replace("\n$$$$$$", "")
        all_parts = [x.strip() for x in all_parts]
        return all_parts, head

import random
def get_a_random_sampling(examples, sample_size):
    random_sample = []
    for _ in range(sample_size):
        idx = random.randint(0, len(examples)-1)
        random_sample.append(examples[idx])
    assert len(random_sample) == sample_size
    return random_sample


from subprocess import Popen, PIPE
def run_generate_io_eg(output_path, example_file, target_program_file, random_seed):
    cmds = ['python3', TOOL_RELPATH, '--out_file', output_path, str(MAX_SAMPLE_SIZE), example_file, target_program_file, str(random_seed)]
    print(" ".join(cmds))
    outs, errs = Popen(cmds, cwd=TOOL_WORKINGDIR, stdout=PIPE, stderr=PIPE).communicate()
    print(outs.decode('utf8', 'strict'))
    print(errs.decode('utf8', 'strict'))
    print("# out_file:", output_path)

if __name__ == "__main__":
    for bench_id in SEED_INFO["bench_seeds"]:
        eg_file = os.path.join(TARGET_FULLDIR, bench_id + ".sl.eg.txt")
        target_file = os.path.join(TARGET_FULLDIR, bench_id + ".sl.manual.strec")
        for seed in SEED_INFO["bench_seeds"][bench_id]:
            output_path = os.path.join(EXAMPLE_FULLDIR, f"{bench_id}.seed{seed}.eg.txt")
            print(f"# -------- Start Process ({bench_id}, {seed}) --------")
            run_generate_io_eg(output_path, eg_file, target_file, seed)
            print(f"# ^^^^^^^^ Done ({bench_id}, {seed}) ^^^^^^^^")

                

