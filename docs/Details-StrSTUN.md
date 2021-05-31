# StrSTUN synthesizer

## Dependencies & Build

### Step 1: Check `gcc` and `cmake`

Check if `gcc` and `cmake 3.10+` are installed.
If not, use the following command to install:
```
sudo apt-get install build-essential cmake
```

### Step 2: Compile `gmp 6.2.1`

First make sure m4 is installed. If not, run the following command:
```
apt-get install m4
```

Then download `gmp 6.2.1` from [here](https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz) (See build options at [here](https://gmplib.org/manual/Build-Options)), or find the version in this repo at `StrSTUN-synthesizer/dependencies/gmp-6.2.1.tar.xz`. 
Run commands below to install (replace `/path/to/this/repo/` to the absolute path of this repo):
```
cd ./StrSTUN-synthesizer/dependencies
tar -xf ./gmp-6.2.1.tar.xz
cd ./gmp-6.2.1
./configure --enable-cxx --prefix=/path/to/this/repo/StrSTUN-synthesizer/dependencies/libs/_gmp621
make install
```

### Step 3: Create build folder and build
Create a folder called `build-strstun` under 
the root directory of this repo:
```
mkdir build-strstun
cd build-strstun
```

Then run CMake and make:
```
cmake -DCMAKE_BUILD_TYPE=Release ../StrSTUN-synthesizer/
make
```
If want to build the Debug version, use
```
cmake -DCMAKE_BUILD_TYPE=Debug ../StrSTUN-synthesizer/
make
```

## Tool Usage (Automated by evaluation scripts)

### Interactive (Read standard input)
To run the tool interactively (providing examples from standard input):
```
./build-strstun/strstun
```

### Nont-interactive (from files)
To run the tool with example files and syntax contraint files, please provide argments to it:

- `-eN` how many examples in use (first N examples)
- `-f<file>` set the path to component file and example file. Component file will be `<file>.comp.txt`, example file will be `<file>.eg.txt`.
- `-x<file>` set the path to example file to `<file>` (override `-f`). `-x` should be after `-f`.
- `-nl` Synthesize without hypothesis space counting (By default, hypothesis space counting is turned on).

For example:
- Run the synthesizer with constraint file `.../targets/19558979.sl.comp.txt` and first 10 examples from the example file `.../example_files/19558979.seed0.eg.txt`.
- 
  ```
  ./build-strstun/strstun -e10 -f./benchmark/strstun/targets/19558979.sl -x./benchmark/strstun/example_files/19558979.seed0.eg.txt
  ```
