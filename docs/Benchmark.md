# Benchmarks for SynGuar

This folder contains 2 benchmarks `strprose` and `strstun`. Example files need to be generated before running the evaluation.

## Generate example files for evaluation
- Run `python3 generate_examples_strstun.py` to generate all examples for StrSTUN.
- Use `python3 generate_examples_strprose.py` to generate all examples for StrPROSE.  
After those commands succeed, please proceed to the next step in  [Evaluation.md](./Evaluation.md).   
More details about the benchmarks is listed below.

## Structure
- `strprose` The benchmark for the `StrPROSE` and `SynGuar-PROSE` (`SynGuar` parameterized by `StrPROSE`).
- `strstun` The benchmark for the `StrSTUN` and `SynGuar-STUN` (`SynGuar` parameterized by `StrSTUN`).
- `generate_examples_strprose.py` The script to generate example files for `strprose` benchmark.
- `generate_examples_strstun.py` The script to generate example files for `strstun` benchmark.
- `__manual_preprocessing` Information about the original SyGuS benchmark and the format transformation.
## Random Seeds
- random seeds file for StrPROSE: `benchmark/strprose/example_files/seeds.txt`
- random seeds file for StrSTUN: `benchmark/strstun/example_files/seeds.txt`

## More Info: Manually generate example file for a single synthesis task
- Generate example file For `StrSTUN` 
 
  Use `generate_examples_strstun_onefile.py`.  
  
  It is used in the following way: 
  ```
  python3 ./benchmark/generate_examples_strstun_onefile.py --out_file <output_path> <max_sample_size> <example_file> <target_program_file> <random_seed>
  ```
  For example:
  ```
  python3 ./benchmark/generate_examples_strstun_onefile.py --out_file ./benchmark/strstun/example_files/2171308.seed0.eg.txt 1000 ./benchmark/strstun/targets/2171308.sl.eg.txt benchmark/strstun/targets/2171308.sl.manual.strec 0
  ```

- Generate example file for `StrPROSE`
  
  The example generation is a built-in functionality of the `StrPROSE` synthesizer. See [Details-StrPROSE.md](../docs/Details-StrPROSE.md) for details.