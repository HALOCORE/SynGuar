#!/usr/bin/env python3

import argparse
import random
import string
import subprocess
import os
abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))


MUTATIONS = ['remove', 'add']
MAX_MUTATIONS = 10
SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
INTERPRETER = abs_join(SCRIPT_DIR, '../build-strstun/oracle')
# substring means this type is a substring of another argument that is a string
# 'progid': (str idx, length of the substring)
# if str idx < 0 then can be any prev argument that is string
# if length < 0 then can be any size up to len(str)
CATEGORY_TYPES = [
    {
        'String': {},
        'Substring': {},
        'Int': {},
        'LenInt': {}
    },
    {
        'String': {},
        'Substring': {'position-of-2nd-3rd-etc-instance-of-character': (0, 1), 
                      'count-specific-characters-in-a-cell': (-1, 1),
                      'remove-unwanted-characters': (0, 1),
                      'stackoverflow7': (0, -1)},
        'Int': {},
        'LenInt': {'19558979': 0,
                   '44789427': 0,
                   'find-nth-occurrence-of-character': 0,
                   'get-first-name-from-name-with-comma': 0,
                   'remove-characters-from-left': 0,
                   'split-numbers-from-units-of-measure_1': 0,
                   'split-numbers-from-units-of-measure_2': 0,
                   'split-text-and-numbers': 0,
                   'split-text-string-at-specific-character': 0}
    },
    {
        'String': {},
        'Substring': {'37534494': (-1, -1)},
        'Int': {},
        'LenInt': {'position-of-2nd-3rd-etc-instance-of-character': 0}
    },

]

# create-email-address-with-name-and-domain 3rd String is actually smth.domain
# exceljet1 - should allow second String to be of the type (smth<sep>*smth)+[.com,uk.co,.co,.edu]
# same issues with exceljet2
# same issue with get-domain-from-email-address_2
# exceljet3,exceljet4 should keep the = 
# get-first-name-from-name-with-comma should make sure range of int is <= number of words
# most-frequently-occurring-text should probably add substring that is equal to the len(str)
# split-* second argument of type Int is actually the place where the split is performed and not
# just any int inside the vector

def random_generate(type_spec, int_upper_bound=1000):
    if type_spec == 'String':
        return random.choice(string.printable.replace("\t\n\r\x0b\x0c", ""))
    elif type_spec == 'Int':
        return random.randint(0, int_upper_bound)

def parse_seed_file(seed_file):
    print('Using {} as seed examples'.format(seed_file))
    type_spec = []
    with open(seed_file) as f:
        # Read the input type
        for line in f:
            line = line.strip()
            if line == '------':
                break
            type_spec.append(line)
            print(line)

        examples = []
        while True:
            example = []
            for i in range(len(type_spec)):
                eg = f.readline().strip()
                example.append(eg)
            # Read separator
            examples.append(example)
            sep = f.readline().strip()

            if '$$$$$$' in sep:
                break
            if len(sep) == 0:
                break
            if '------' != sep:
                print('Expecting separator to be ------. Got {}. Exiting'.format(sep))
                exit()

    return type_spec, examples

def generate_inputs(type_spec, seed_examples, N, progid):
    """
    Generate N examples using seeds as inputs and type spec
    """
    print('Using {} examples as seed'.format(seed_examples))
    # transform the strings to arrays
    arr_examples = []
    for example in seed_examples:
        inputs = example[:-1]
        arr_inputs = []
        for in_eg in inputs:
            arr_input  = []
            for char in in_eg:
                arr_input.append(char)
            arr_inputs.append(arr_input)

        arr_examples.append(arr_inputs)
    #for example in seed_examples:
    #    arr_examples.append(example[0])

    gen_examples = []
    for i in range(N):
        mutation = random.choice(MUTATIONS)
        seed = random.choice(arr_examples)
        # We want to not change the starting " and ending " of the string
        inputs = []
        type_idx = 0
        for type_idx, type_in in enumerate(type_spec[:-1]):
            seed_in = seed[type_idx]

            if type_in == 'String':
                if progid in CATEGORY_TYPES[type_idx]['Substring']:
                    if type_idx == 0:
                        print('Substring of what?? Exiting')
                        exit()

                    info_substr = CATEGORY_TYPES[type_idx]['Substring'][progid]
                    if info_substr[0] > 0:
                        str_type_idx = info_substr[0]
                    else:
                        str_type_idx = random.randint(0, len(seed[:type_idx - 1]))

                    # should be the previous mutated string not the seed string when
                    # the argument is a substring
                    seed_str = inputs[str_type_idx]
                    position_start = random.randint(1, len(seed_str) - 2)
                    if info_substr[1] > 0:
                        length = info_substr[1]
                    else:
                        length = max(0, random.randint(1, len(seed_str) - position_start))
                    new_ex = seed_str[position_start:position_start+length]
                else:
                    position = random.randint(1, len(seed_in) - 2)
                    new_ex = []
                    if mutation == 'add':
                        new_ex = seed_in[1:position]
                        for i in range(1, 10):
                            new_ex.append(random_generate(type_in))
                        new_ex.extend(seed_in[position:-1])
                    elif mutation == 'remove':
                        new_ex = seed_in[1:position]
                        new_ex.extend(seed_in[position+1:-1])
                string_new_ex = ''.join([char for char in new_ex])
            elif type_in == 'Int':
                if progid in CATEGORY_TYPES[type_idx]['LenInt']:
                    str_type_idx = CATEGORY_TYPES[type_idx]['LenInt'][progid]
                    seed_str = seed[str_type_idx]
                    new_ex = random_generate(type_in, int(len(seed_str)/2))
                else:
                    new_ex = random_generate(type_in)
                string_new_ex = new_ex
            else:
                print('Unknown type {}. Exiting'.format(type_in))
                exit()

            inputs.append(string_new_ex)
            type_idx += 1
        gen_examples.append(inputs)

    return gen_examples

def write_examples_to_files(type_spec, examples, out_filename):
    with open(out_filename, 'w') as f:
        for ty in type_spec:
            f.write(ty + '\n')
        f.write('------\n')
        i = 0
        for inputs, out_eg in examples:
            for idx, in_eg in enumerate(inputs):
                if type_spec[idx] == 'Int':
                    f.write(str(in_eg) + '\n')
                else:
                    f.write('"' + in_eg + '"\n')
            if type_spec[-1] == 'Int':
                f.write(out_eg + '\n')
            else:
                f.write('"' + out_eg + '"\n')
            if i < len(examples) - 1:
                f.write('------\n')
            i += 1

        f.write('$$$$$$\n')


def parse_oracle_stdout(stdout):
    stdout = stdout.split('\n')
    return_next = False
    for line in stdout:
        if 'BEGIN OUTPUT' in line:
            return_next = True
        elif return_next:
            return line

    return None

def run_target_program(target_program, input_examples):
    input_examples_str = '\n'.join([str(x) for x in input_examples])
    cmdline = [INTERPRETER, 'SYNFUNC', target_program]
    p = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE, shell=False, universal_newlines=True,)
    try:
        stdout = p.communicate(input='y\n{}\nn\n'.format(input_examples_str), timeout=1)[0]
        return parse_oracle_stdout(stdout)
    except subprocess.TimeoutExpired:
        p.kill()

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('N', type=int, help='Number of IO examples to generate')
    parser.add_argument('seed_file', type=str, help='Path to seed file that contains examples')
    parser.add_argument('target_prog', type=str, help='Target program')
    parser.add_argument('seed', type=int, help='Seed number', default=0)
    parser.add_argument('--out_file', type=str, help='Write examples to this file')

    args = parser.parse_args()
    basename = os.path.basename(args.seed_file)
    prefix = basename.split('.')
    progid = prefix[0]
    print('Program ID {}'.format(progid))

    random.seed(args.seed)

    if args.out_file == None:
        args.out_file = progid + '.' + prefix[1] + '.seed_{}.gen_eg.txt'.format(args.seed)
        args.out_file = os.path.join(os.path.dirname(args.seed_file), args.out_file)

    type_spec, examples = parse_seed_file(args.seed_file)
    gen_examples = []
    need_to_redo = 0
    while True:
        gen_inputs = generate_inputs(type_spec, examples, args.N, progid)

        #print('Input examples:\n{}'.format(gen_inputs))
        for in_eg in gen_inputs:
            out_eg = run_target_program(args.target_prog, in_eg)
            if out_eg == '<ERROR>':
                need_to_redo += 1
            else:
                gen_examples.append((in_eg, out_eg))

        print('Hit <ERROR> on {}.'.format(need_to_redo))
        args.N = need_to_redo
        if need_to_redo == 0:
            break
        need_to_redo = 0

    write_examples_to_files(type_spec, gen_examples, args.out_file)


if __name__ == "__main__":
    main()
