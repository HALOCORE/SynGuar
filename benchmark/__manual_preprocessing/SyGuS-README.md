# Original Benchmark & Transformation
## Syntax-Guided Synthesis Competition (SyGuS-Comp)
- Website: https://sygus.org/comp/2019/
- Specification Language: https://sygus.org/language/
  - The SyGuS input format (SyGuS-IF) is closely modeled on SMT-Lib 2.
  - comprehensive description: [SyGuS Language Standard Version 2.0](https://sygus.org/assets/pdf/SyGuS-IF_2.0.pdf)

- [2019 PBE-String Benchmark](https://github.com/SyGuS-Org/benchmarks/tree/master/comp/2019/PBE_SLIA_Track)

## Preprocessing

The benchmark is preprocessed (script + manual fix) to transform to the format acceptable by our tool `StrSTUN`. The processed benchmark files can be found at `benchmark/strstun`.
## Generating more examples

Run `benchmark/generate_examples_strstun_onefile.py` by specifying the number of examples, path to examples in the SyGus
benchmark that will be used as seeds and path to the target program.
The script creates a file containing the generated examples as a `.eg.txt` file.
