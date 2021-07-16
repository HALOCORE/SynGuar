# StrPROSE synthesizer

## Dependencies & Build

### Step 1: Install dependencies
It requires `dotnet-sdk-5.0` and `dotnet-runtime-3.1` to build and run. Use the following command to install:

```
cd /tmp && wget https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb && \
    dpkg -i packages-microsoft-prod.deb && apt-get update && \
    apt-get install -y apt-transport-https && \
    apt-get update && \
    apt-get install -y dotnet-sdk-5.0 dotnet-runtime-3.1
```
If permission denied, add `sudo` before each command.

Or see official docs for installing .NET on Ubuntu (18.04/20.04)
https://docs.microsoft.com/en-us/dotnet/core/install/linux-ubuntu

Both `.NET runtime 3.1` and `.NET SDK 5.0` are required.

### Step 2: build the synthesizer
Run the following command:
```
cd ./StrPROSE-synthesizer/StrPROSE
dotnet build
dotnet build --configuration Release
```

The debug version of the compiled program will be at `./bin/Debug/netcoreapp3.1/StrPROSE.dll`, and the release version of the compiled program will be at `./bin/Release/netcoreapp3.1/StrPROSE.dll`  
(Current folder is `./StrPROSE-synthesizer/StrPROSE`).

Then run the following command under `./StrPEOSE-synthesizer/StrPROSE` to see if the build is successful:
```
dotnet ./bin/Debug/netcoreapp3.1/StrPROSE.dll --help
dotnet ./bin/Release/netcoreapp3.1/StrPROSE.dll --help
```

## Tool Usage (Automated by evaluation scripts)
The usage is for running benchmark manually.

After compiling the tool, use the following command to see the help information.
  ```
  dotnet ./bin/Debug/netcoreapp3.1/StrPROSE.dll --help
  ```
  It will print the manual as follows:
  ```
  --samplegen <benchmark_dir> <benchmark_id> <seed> <count> <output_dir>
            Generate examples based on a benchmark program specified by <benchmark_id>.csv and <benchmark_id>.txt

  --synthloop <example_file> <cache_folder>
            Start a synthesize loop. <example_file> is a csv file with I/O examples.

  --test <testpath>
            Select a csv file in <testpath> to do synthesize. (for debugging)
  ```

Below are some examples of using the synthesizer (assume working directory is the root directory of this repo):
- Generate examples:
  ```
  dotnet ./StrPROSE-synthesizer/StrPROSE/bin/Debug/netcoreapp3.1/StrPROSE.dll --samplegen ./benchmark/strprose/targets prog3 3014 2000 ./benchmark/strprose/example_files
  ```

- Synthesis Loop using an example file:
  ```
  dotnet ./StrPROSE-synthesizer/StrPROSE/bin/Debug/netcoreapp3.1/StrPROSE.dll --synthloop ./benchmark/strprose/example_files/prog1.seed1000.csv ./outputs/cache/StrPROSE
  ```

- Synthesis test under a test folder:
  ```
  dotnet ./StrPROSE-synthesizer/StrPROSE/bin/Debug/netcoreapp3.1/StrPROSE.dll --test ./StrPROSE-synthesizer/tests
  ```

### Synthesis with an example file
Now look at a running example for synthesis with an example file. The example file is a `.csv` file in the following format:
```
input0,output
T9A4l K4C,t9a4l
OG7 tS 9,og7
"4P f565Y,kd ",4p
"E If w,QsVj",e
jtK gwieR,jtk
" TeY ,2eU2N",tey
......
```

Use the following command to run the synthesizer:
```
dotnet ./StrPROSE-synthesizer/StrPROSE/bin/Debug/netcoreapp3.1/StrPROSE.dll --synthloop ./benchmark/strprose/example_files/prog1.seed1000.csv ./outputs/cache/StrPROSE
```

Then the synthesizer will run a loop to ask for sample size. Input `5` and press `Enter`. The synthesizer will check the cache for the results of sample size 1-5. If not in the cache, the synthesizer will synthesize with sample size 1-5. Then it will ask for sample size again. Input `-1` will stop the synthesis loop.

```

        StrPROSE 0.1
        2021.3.20


================ Synthesize Loop (./benchmark/strprose/example_files/prog1.seed1000.csv)================
# Constructing Synthesizer ...
# Length of a row: 2
# Please input example_size:
5
# Important Example.  #eg=1 newVSAsize=11550491192205431488
# Important Example.  #eg=2 newVSAsize=4326192
# Important Example.  #eg=3 newVSAsize=8220
# Important Example.  #eg=4 newVSAsize=60
## Write 5 examples with H to csv: ./prog1.seed1000.csv.cache.csv
# synth.currentVSASize: 60
# synth.currentProgram: LowerCase(SubString(x, AbsPos(x, 0), RelPos(x, (.*?, \s+, 0, 0))), x)
# Please input example_size:
-1
================
# Bye
```

The synthesizer will also output the results to another `.csv` file in the specified cache folder:
```
input0,output,H,important,program
T9A4l K4C,t9a4l,11550491192205431488,True,"ConstStr(""t9a4l"")"
OG7 tS 9,og7,4326192,True,"LowerCase(SubString(x, ......
"4P f565Y,kd ",4p,8220,True,"LowerCase(SubString(x,......
......
```
The output file contains the synthesized result after each additional  example and the size of the hypothesis space. The `important` column indicates whether the example on each row drop the hypothesis space or not.