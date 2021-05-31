path_to_bench_1 = "./PBE_SLIA_Track/euphony"
path_to_processed_1 = "./Processed/PBE_SLIA_Track/euphony"

import os
from nltk import Tree

def group_lines_by_empty_line(lines):
    groups = []
    temp_group = []
    is_check_synth_found = False
    for line in lines:
        if line.strip() == "":
            groups.append(temp_group)
            temp_group = []
        elif line.strip() == "(check-synth)":
            break
        else:
            temp_group.append(line)
    return groups


def parse_sig(fargs, ftype):
    assert ftype in ["Bool", "Int", "String"]
    arg_idx = 0
    argtypes = []
    names = []
    for arg in fargs:
        name = arg._label
        if name not in ["name", "col1", "col2", "firstname", "lastname"]:
            assert(name  == "_arg_" + str(arg_idx))
        arg_idx += 1
        argtype = arg[0]
        argtypes.append(argtype)
        names.append(name)
    return argtypes + [ftype], names


def pars_prod(prod):
    fname = prod._label
    paramtypes = []
    for i in range(len(prod)):
        assert isinstance(prod[i], str)
        assert prod[i] in ["ntInt", "ntBool", "ntString"]
        paramtype = prod[i][2:]
        paramtypes.append(paramtype)
    return fname, paramtypes

all_seen_compIds = {"Int":set(), "Bool":set(), "String":set()}
def extract_compid(tp, comp_prod):
    if comp_prod._label == "ite":
        return None
    else:
        all_seen_compIds[tp].add(comp_prod._label)
        return comp_prod._label


def parse_comps(typecomp, argnames):
    nttype = typecomp[0]
    assert typecomp._label == "nt" + nttype
    assert len(typecomp) == 2
    prods = [typecomp[1]._label] + [typecomp[1][i] for i in range(len(typecomp[1]))]
    ts = []
    inputs = []
    typepds = []
    temp_ts = ""
    escape_a_quote = False
    for prod in prods:
        if escape_a_quote:
            assert prod == "\""
            escape_a_quote = False
            continue
        if isinstance(prod, Tree):
            if temp_ts != "":
                assert prod._label == "\"" and prod[0] == "\""
                ts.append("\"(\"")
                ts.append("\")\"")
                temp_ts = ""
                escape_a_quote = True
            else:
                compId = extract_compid(nttype, prod)
                if compId is not None:
                    typepds.append(compId)
        else:
            assert isinstance(prod, str)
            if prod in argnames:
                assert temp_ts == ""
                inputs.append("input" + str(argnames.index(prod)))
            elif (prod.count("\"") == 0 or prod.count("\"") == 2):
                if temp_ts == "":
                    ts.append(prod)
                else:
                    temp_ts += prod
            else:
                assert(prod.count("\"") == 1)
                if temp_ts == "":
                    temp_ts += prod + " "
                else:
                    temp_ts += prod
                    ts.append(temp_ts)
                    temp_ts = ""
    # process typepds
    print(ts, inputs)
    return nttype, ts, inputs, typepds


def parse_cst(cst, numargs):
    assert cst.startswith("(constraint (= (f ")
    args = []
    current_arg = ""
    cst = cst.replace("(constraint (= (f ", "").strip()
    charCount = 0
    is_in_string = False
    to_break = False
    for c in cst:
        if to_break: break
        charCount += 1
        if c == ")":
            to_break = True
            c = " "
        if not is_in_string: 
            if c == "\"":
                is_in_string = True
                assert current_arg == ""
                current_arg += "\""
            elif c == " ":
                if current_arg == "": continue
                else:
                    args.append(current_arg)
                    current_arg = ""
            else: current_arg += c
        elif is_in_string:
            if c == "\"":
                assert current_arg != ""
                current_arg += "\""
                args.append(current_arg)
                current_arg = ""
                is_in_string = False 
            else:
                current_arg += c
    assert len(args) == numargs
    result = cst[charCount+1:]
    assert result.endswith("))") and result[0] != "("
    result = result[:-2]
    # print(cst, args, result)
    return args, result

def parse_groups(groups):
    assert len(groups[0]) == 1
    assert groups[0][0].strip() == "(set-logic SLIA)"
    
    syn_group = groups[1]
    syn_code = " ".join(syn_group).replace("\n", " ")
    t = Tree.fromstring(syn_code)
    assert(t._label == "synth-fun")
    assert(t[0] == 'f')
    fargs = t[1]
    ftype = t[2]
    sig, argnames = parse_sig(fargs, ftype)
    print(sig)
    syns = t[3]
    start = syns[0]

    compdict = {}
    for i in range(1, len(syns)):
        comptype, ts, inputs, typepds = parse_comps(syns[i], argnames)
        compdict[comptype] = (ts, inputs, typepds)

    csts = groups[2]
    examples = []
    for cst in csts:
        if cst.startswith("(declare-var ") and cst.endswith(" String)\n"): continue
        params, result = parse_cst(cst, len(sig) - 1)
        examples.append((params, result))
    # print(t)
    # sigline = syn_group[0].replace("(synth-fun f ", "")
    # assert(sigline.startswith("("))
    return sig, compdict, examples


LINE_SEP = "------"
def write_eg_file(filename, sig, examples):
    print("# write to example: ", filename)
    lines = []
    lines.extend(sig)
    lines.append(LINE_SEP)
    for eg in examples:
        lines.extend(eg[0])
        lines.append(eg[1])
        lines.append("------")
    lines[-1] = "$$$$$$"
    lines_with_newline = [x + "\n" for x in lines]
    with open(filename, 'w') as f:
        f.writelines(lines_with_newline)

def write_comp_file(filename, compdict):
    print("# write to comp: ", filename)
    lines = []
    for k in ["Int", "Bool", "String"]:
        lines.append(k)
        lines.append("------")
        lines.append("Const")
        (ts, inputs, typepds) = compdict[k]
        lines.extend(ts)
        lines.append("------")
        lines.append("Operators")
        lines.extend(typepds)
        lines.append("------")
    lines_with_newline = [x + "\n" for x in lines]
    with open(filename, 'w') as f:
        f.writelines(lines_with_newline)

        


ESCAPE_LIST = []

def process_folder(folderpath, processed_path):
    files = os.listdir(folderpath)
    for filename in files:
        fullname = os.path.join(folderpath, filename)
        if fullname in ESCAPE_LIST: continue
        if fullname.endswith(".sl"):
            with open(fullname, 'r') as f:
                print("# " + fullname)
                lines = f.readlines()
                groups = group_lines_by_empty_line(lines)
                assert len(groups) == 3
                sig, compdict, examples = parse_groups(groups)
                processed_egfile = os.path.join(processed_path, filename + ".eg.txt")
                processed_compfile = os.path.join(processed_path, filename + ".comp.txt")
                write_eg_file(processed_egfile, sig, examples)
                write_comp_file(processed_compfile, compdict)
                

if __name__ == "__main__":
    process_folder(path_to_bench_1, path_to_processed_1)
    print(all_seen_compIds)