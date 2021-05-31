import sys

line_extract = {
    "exampleCount": (
        "[synthesizer] finishAddingExamples. #example =", 
        lambda x : int(x)
    ),
    "time": (
        "[synthesizer-stat] total time:",
        lambda x : float(x.replace("seconds.", ""))
    ),
    "enum_time": (
        "# StrecSynthesizer::synthesis ENUM TIME:",
        lambda x : float(x)
    ),
    "gg_time": (
        "# StrecSynthesizer::synthesis GG TIME:",
        lambda x : float(x)
    ),
    "HS": (
        "# GoalGraphManager::buildCondGraph. Upper bound of H after synth:",
        lambda x : int(x)
    ),
    "H": (
        "# GoalGraphManager::buildCondGraph. Upper bound of H:",
        lambda x : int(x)
    ),
    "termMatchCount": (
        "# GoalGraphManager::buildBottomValueGoals done:",
        lambda x : int(x.split("totalTerm=")[0].split("concrete=")[1])
    ),
    "termTotalCount": (
        "# GoalGraphManager::buildBottomValueGoals done:",
        lambda x : int(x.split("totalTerm=")[1])
    ),
    "condMatchCount": (
        "# GoalGraphManager::buildBottomCondGoals done:",
        lambda x : int(x.split("totalCond=")[0].split("concrete=")[1])
    ),
    "condTotalCount": (
        "# GoalGraphManager::buildBottomCondGoals done:",
        lambda x : int(x.split("totalCond=")[1])
    ),
    "H1": (
        "solutionCountByLevel ",
        lambda x : int(x.split("size:3")[-1].split(",")[0].split("[0]:")[1])
    ),
    "H2": (
        "solutionCountByLevel ",
        lambda x : int(x.split("size:3")[-1].split(",")[1].split("[1]:")[1])
    ),
    "H3": (
        "solutionCountByLevel ",
        lambda x : int(x.split("size:3")[-1].split(",")[2].split("[2]:")[1])
    )
}

def extract_info_from_lines(lines, extractor):
    info = {}
    for line in lines:
        for key in extractor:
            match, convf = extractor[key]
            if line.find(match) >= 0:
                info[key] = convf(line.replace(match, ""))
    return info


def parse_stun_log(content):
    lines = content.split("\n")
    info = extract_info_from_lines(lines, line_extract)
    try:
        is_valid_hs = not (content.find("SYNTHESIS_WITHOUT_COUNTING") >= 0)
        is_not_found = content.find("NOT_FOUND") >= 0
        is_found = (not is_not_found) and content.find("FOUND") >= 0
        result = None
        if is_found:
            result = content.split("BEGIN Synthesize Result\n")[1].split("END Synthesize Result")[0]
        info["is_found"] = is_found
        info["synth"] = result
        result = {
            "hs": [info["H1"], info["H2"], info["H3"]],
            "program": info["synth"],
            "info": info,
            "valid_hs": is_valid_hs
        }
        return result, is_found, is_valid_hs
    except:
        # print("# [stun-log-parser] WARNING: incomplete log.")
        return None, False, None

if __name__ == "__main__":
    logfile = sys.args[1]
    with open(logfile, 'r') as f:
        content = f.read()
        parse_result = parse_stun_log(content)
        print(parse_result)