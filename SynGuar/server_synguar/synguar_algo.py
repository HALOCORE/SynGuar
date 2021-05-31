
import math

def epsilon_factor(epsilon):
    # return 1.0 / math.log(1.0 / (1.0 - epsilon))
    return 1.0 / epsilon

DEFAULT_G_FUNC = lambda x, epsilon, delta : max(0, 1 * epsilon_factor(epsilon) * (math.log(x) - math.log(1/delta)))

def union_sample_bound(epsilon, delta, size_H):
    epsilonFactor = epsilon_factor(epsilon)
    deltaTerm = math.log(1.0 / delta)
    logH = math.log(float(size_H))
    sampleSize = epsilonFactor * (logH + deltaTerm)
    return sampleSize


###################### Single H

def synguar_single_H_param_g(function_g, epsilon, delta, k, full_trace_out, get_result_with_sample_size_func):
    if (k != 1):
        print("# [synguar-algo] WARNING: Single H called with k=", k)
    middle_steps_out = {}
    full_trace_out["middle_steps"] = middle_steps_out
    isBoundTerminate = False

    seen_eg_count = 0
    n_limit = None
    while True: 
        # while s < n
        if n_limit is not None and seen_eg_count > n_limit:
            assert seen_eg_count == math.floor(n_limit/k) + k
            isBoundTerminate = True
            break

        # sample k
        seen_eg_count = seen_eg_count + k
        
        # hs = updatehypothesis
        result_i = get_result_with_sample_size_func(seen_eg_count)
        assert(len(result_i["hs"]) == 1)
        current_H = int(result_i["hs"][0])
        
        # n = min(n, s+g(h))
        new_n_limit = seen_eg_count + max(0, function_g(current_H, epsilon, delta))
        assert new_n_limit >= seen_eg_count
        if n_limit is None or new_n_limit < n_limit:
            n_limit = new_n_limit

        # n + m
        current_bound = (math.floor(n_limit/k) + k) + math.ceil(union_sample_bound(epsilon, delta, current_H))
        
        # save middle steps
        middle_steps_out[seen_eg_count] = result_i
        middle_steps_out[seen_eg_count]["phase1_n_limit"] = [n_limit]
        middle_steps_out[seen_eg_count]["current_bound"] = [current_bound]

    assert isBoundTerminate

    final_result = get_result_with_sample_size_func(current_bound, no_counting=False)
    
    # middle steps already in full_trace_out
    full_trace_out["total_bound"] = [current_bound]
    full_trace_out["last_middle_step"] = seen_eg_count
    full_trace_out["final_result"] = final_result
    full_trace_out["space_count"] = 1
    full_trace_out["chosen_space"] = 1
    


def synguar_singleH(epsilon, delta, k, full_trace_out, get_result_with_sample_size_func):
    print("# [algo] synguar_singleH called.")
    synguar_single_H_param_g(DEFAULT_G_FUNC, epsilon, delta, k, full_trace_out, get_result_with_sample_size_func)



################################### 3 H

def synguar_3H_param_g(function_g, epsilon, delta, k, full_trace_out, get_result_with_sample_size_func):
    alloc_delta = delta / 3
    seen_eg_count = 0
    n1 = -1     # infinity
    n2 = -1
    n3 = -1

    IDX1 = 0
    IDX2 = 1
    IDX3 = 2
    is_phase1_done = [False, False, False]

    # top level info TODO
    full_trace_out["space_count"] = 3
    full_trace_out["middle_steps"] = {}
    
    is_terminated = False
    while True:
        assert(not is_terminated)
        # sample k
        seen_eg_count += k
        result_s = get_result_with_sample_size_func(seen_eg_count, no_counting=False)
        assert(len(result_s["hs"]) == 3)
        H1 = int(result_s["hs"][IDX1])
        H2 = int(result_s["hs"][IDX2])
        H3 = int(result_s["hs"][IDX3])

        step_info = {
            # filled
            "hs": [H1, H2, H3], 
            "program": result_s["program"], 
            # to fill
            "phase1_n_limit": [None, None, None],
            "current_bound": [None, None, None]
        }
        full_trace_out["middle_steps"][seen_eg_count] = step_info

        # ---------- for space H1
        if not is_phase1_done[IDX1]:
            g1 = -1 if H1 <= 0 else max(0, function_g(H1, epsilon, alloc_delta))
            n1 = -2 if g1 < 0 else (seen_eg_count + g1 if n1 == -1 else min(n1, seen_eg_count + g1))
            if n1 != -2:
                step_info["phase1_n_limit"][IDX1] = n1
                step_info["current_bound"][IDX1] = (math.floor(n1/20)+1)*20 + math.ceil(union_sample_bound(epsilon, alloc_delta, H1))
            if n1 != -2 and seen_eg_count > n1:
                # exiting phase1 for H1
                is_phase1_done[IDX1] = True
                assert(seen_eg_count == (math.floor(n1/20)+1)*20)

                # get phase2 result
                sample_h1 = seen_eg_count + math.ceil(union_sample_bound(epsilon, alloc_delta, H1)) if n1 > 0 else -9
                result_s = get_result_with_sample_size_func(sample_h1, no_counting=True)
                program = result_s["program"]
                if program is not None:
                    full_trace_out["total_bound"] = sample_h1
                    full_trace_out["chosen_space"] = 1
                    full_trace_out["final_result"] = result_s
                    is_terminated = True

        # ---------- for space H2
        if not is_phase1_done[IDX2]:
            g2 = -1 if H2 <= 0 else max(0, function_g(H2, epsilon, alloc_delta))
            n2 = -2 if g2 < 0 else (seen_eg_count + g2 if n2 == -1 else min(n2, seen_eg_count + g2))
            if n2 != -2:
                step_info["phase1_n_limit"][IDX2] = n2
                step_info["current_bound"][IDX2] = (math.floor(n2/20)+1)*20 + math.ceil(union_sample_bound(epsilon, alloc_delta, H2))
            if n2 != -2 and (not is_terminated) and seen_eg_count > n2:
                # exiting phase1 for H2
                is_phase1_done[IDX2] = True
                assert(seen_eg_count == (math.floor(n2/20)+1)*20)

                # get phase2 result
                sample_h2 = seen_eg_count + math.ceil(union_sample_bound(epsilon, alloc_delta, H2)) if n2 > 0 else -9
                result_s = get_result_with_sample_size_func(sample_h2, no_counting=True)
                program = result_s["program"]
                if program is not None:
                    full_trace_out["total_bound"] = sample_h2
                    full_trace_out["chosen_space"] = 2
                    full_trace_out["final_result"] = result_s
                    is_terminated = True
        
        # ---------- for space H3
        if not is_phase1_done[IDX3]:
            g3 = -1 if H3 <= 0 else max(0, function_g(H3, epsilon, alloc_delta))
            n3 = -2 if g3 < 0 else (seen_eg_count + g3 if n3 == -1 else min(n3, seen_eg_count + g3))
            if n3 != -2:
                step_info["phase1_n_limit"][IDX3] = n3
                step_info["current_bound"][IDX3] = (math.floor(n3/20)+1)*20 + math.ceil(union_sample_bound(epsilon, alloc_delta, H3))
            if n3 != -2 and (not is_terminated) and seen_eg_count > n3:
                # exiting phase1 for H3
                is_phase1_done[IDX3] = True
                assert(seen_eg_count == (math.floor(n3/20)+1)*20)

                # get phase2 result
                sample_h3 = seen_eg_count + math.ceil(union_sample_bound(epsilon, alloc_delta, H3)) if n3 > 0 else -9
                result_s = get_result_with_sample_size_func(sample_h3, no_counting=True)
                program = result_s["program"]
                if program is not None:
                    full_trace_out["total_bound"] = sample_h3
                    full_trace_out["chosen_space"] = 3
                    full_trace_out["final_result"] = result_s
                    is_terminated = True
        
        # -------- check termination
        if is_terminated: 
            full_trace_out["last_middle_step"] = seen_eg_count
            break


def synguar_3H(epsilon, delta, k, full_trace_out, get_result_with_sample_size_func):
    print("# [algo] synguar_3H called.")
    synguar_3H_param_g(DEFAULT_G_FUNC, epsilon, delta, k, full_trace_out, get_result_with_sample_size_func)