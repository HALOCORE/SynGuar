import os
import sys
from helper_eval import eval_invoke
from helper_eval import data_process_strprose
from helper_eval import data_process_strstun

ARG_RUN_SESSIONS_STRPROSE = "run-prose"
ARG_RUN_SPACEDROP_STRPROSE = "sdrop-prose"
ARG_RUN_4EXAMPLES_STRSTUN = "4eg-stun"
ARG_RUN_SESSIONS_STRSTUN = "run-stun"
ARG_ANALYSIS_STRPROSE = "data-prose"
ARG_ANALYSIS_STRSTUN = "data-stun"

possible_args = [
    ARG_ANALYSIS_STRPROSE,
    ARG_ANALYSIS_STRSTUN,
    ARG_RUN_SESSIONS_STRPROSE,
    ARG_RUN_SPACEDROP_STRPROSE,
    ARG_RUN_SESSIONS_STRSTUN,
    ARG_RUN_4EXAMPLES_STRSTUN
]

if __name__ == "__main__":
    print("============ SynGuar Evaluation Script ============")
    if (len(sys.argv) != 2):
        print("# ERROR need to provide 1 argment, can be: " + " ".join(possible_args))
        exit(1)
    
    if (not sys.argv[1] in possible_args):
        print("# ERROR unknown argument. Acceptable argument: " + " ".join(possible_args))
        exit(1)

    arg = sys.argv[1]
    if arg == ARG_RUN_SESSIONS_STRPROSE:
        eval_invoke.invoke_strprose_evaluation_requests()
    elif arg == ARG_RUN_SESSIONS_STRSTUN:
        eval_invoke.invoke_strstun_evaluation_requests()
    elif arg == ARG_RUN_SPACEDROP_STRPROSE:
        eval_invoke.invoke_strprose_spacedrop_requests()
    elif arg == ARG_RUN_4EXAMPLES_STRSTUN:
        eval_invoke.invoke_strstun_4examples_requests()
    elif arg == ARG_ANALYSIS_STRPROSE:
        data_process_strprose.analysis_sample_size()
        data_process_strprose.analysis_result_program()
        data_process_strprose.analysis_baseline_4examples()
        data_process_strprose.analysis_spacedrop()
    elif arg == ARG_ANALYSIS_STRSTUN:
        data_process_strstun.analysis_sample_size()
        data_process_strstun.analysis_result_program()
        data_process_strstun.analysis_resource_usage()
        data_process_strstun.analysis_baseline_4examples()
        data_process_strstun.analysis_choose_h()