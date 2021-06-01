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
