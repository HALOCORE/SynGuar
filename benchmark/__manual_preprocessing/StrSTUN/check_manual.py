import os
import time
import json
from subprocess import Popen, PIPE

PROCESSED_FOLDER = "path/to/targets"
BENCH_IDS = ["euphony"]

missing_ids = []
code_count = 0
total_count = 0
def get_prog_ids(filenames):
    fileids1 = set()
    fileids2 = set()
    for fname in filenames:
        if fname.endswith(".eg.txt"):
            fid = fname.replace(".eg.txt", "")
            fileids1.add(fid)
        elif fname.endswith(".comp.txt"):
            fid = fname.replace(".comp.txt", "")
            fileids2.add(fid)
        elif fname.endswith(".strec"):
            continue
        else:
            print("# ignore filename: ", fname)
    assert(fileids1 == fileids2)
    return sorted(list(fileids1))

def get_payloads(tps, egs):
    lines = []
    for eg in egs:
        lines.append("y")
        for inp in eg[:-1]:
            lines.append(str(inp))
    return "\n".join(lines) + "\nn\n"

def get_examples(eg_content):
    tp = []
    tp_done = False
    lines = eg_content.strip().split("\n")
    egs = []
    current_eg = []
    for line in lines:
        line = line.strip()
        if line == '$$$$$$':
            egs.append(current_eg)
        elif line == '------': 
            if not tp_done: tp_done = True
            else: 
                egs.append(current_eg)
                current_eg = []
        else:
            if not tp_done: tp.append(line)
            else: 
                idx = len(current_eg)
                if tp[idx] == 'String':
                    current_eg.append(line[1:-1])
                elif tp[idx] == 'Int':
                    current_eg.append(int(line))
                elif tp[idx] == 'Bool':
                    if line == 'true': current_eg.append(True)
                    elif line == 'false': current_eg.append(False)
                    else: assert(False)
    return tp, egs

def parse_output(output):
    if output.find("Syntax error") >= 0:
        print(output)
    out_lines = output.split("\n")
    str_results = []
    read_status = 0
    last_pause = False
    for line in out_lines:
        line = line.replace("\n", "")
        if line == "BEGIN OUTPUT": 
            last_pause = False
            read_status = 1
        elif read_status == 1: 
            str_results.append(line)
            read_status = 2
        elif line == "END OUTPUT": 
            assert(read_status == 2)
            read_status = 0
        if line == "PAUSE Request Example (y/n):": last_pause = True
    if not last_pause:
        print("# WARNING: missing last PAUSE line.")
    return str_results

def parse_meta(err_out):
    meta = {}
    lines = err_out.split("\n")
    for line in lines:
        if line.find("num_conds:") >= 0:
            meta['num_conds'] = int(line.split("num_conds:")[1])
        elif line.find("max_term_size:") >= 0:
            meta['max_term_size'] = int(line.split("max_term_size:")[1])
        elif line.find("total_component_size:") >= 0:
            meta['total_component_size'] = int(line.split("total_component_size:")[1])
    return meta

def run_prog_on_examples(progfile, tps, examples):
    payload = get_payloads(tps, examples)
    print("\n# progfile: ", progfile, " | tp:", tps)
    cmdline = ['path/to/oracle', 'SYNFUNC', progfile]
    str_results = None
    metas = None
    with Popen(cmdline, stdin=PIPE, stdout=PIPE, stderr=PIPE, bufsize=1, universal_newlines=True) as shell:
        output, err = shell.communicate(payload) 
        if output != None:
            str_results = parse_output(output)
            metas = parse_meta(err)
            print(metas)
            if len(str_results) == 0:
                print("# === WARN 0-result ===\nOUTPUT:\n" + output + "ERR:\n" + err)
        else:
            print("# WARNING: output is None.")
            print("ERR: ", err, "\nOUTPUT:", output)
    if str_results == []:
        str_results = None
    return str_results, metas


def check_results(egs, str_results):
    assert(len(egs) == len(str_results))
    mismatch_count = 0
    for eg, str_result in zip(egs, str_results):
        if(str(eg[-1]) != str_result):
            print("# ERROR MISMATCH:", eg, str_result)
            mismatch_count += 1
    return mismatch_count

bench_info = {}

for bid in BENCH_IDS:
    current_info = {}
    bench_info[bid] = current_info
    total_count = 0
    code_count = 0
    processed_sub_folder = os.path.join(PROCESSED_FOLDER, bid)
    prog_ids = get_prog_ids(os.listdir(processed_sub_folder))
    # print(prog_ids)
    for prog_id in prog_ids:
        example_path = os.path.join(processed_sub_folder, prog_id + ".eg.txt")
        manual_prog_path = os.path.join(processed_sub_folder, prog_id + ".manual.strec")
        if not os.path.exists(manual_prog_path):
            # print("# not exists:", manual_prog_path)
            continue
        with open(example_path, 'r') as f:
            eg_content = f.read()
            tp, egs = get_examples(eg_content)
            str_results, metas = run_prog_on_examples(manual_prog_path, tp, egs)
            if str_results is not None:
                mismatch_count = check_results(egs, str_results)
                if (mismatch_count > 0):
                    print("# mismatch_count: ", mismatch_count)
                    current_info[prog_id] = "MISMATCH"
                else:
                    pfx = "MATCH"
                    if metas['max_term_size'] <= 9:
                        pfx += "_BELOW9"
                    current_info[prog_id] = pfx + " " + json.dumps(metas)
            else:
                current_info[prog_id] = "NONE"

import json
with open("./bench_info.json", 'w') as f:
    print("# write to file:", "./bench_info.json")
    json.dump(bench_info, f, indent=2)


