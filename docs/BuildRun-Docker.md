# Build & Run (Docker)

## Build or download the environment

Build with Docker is straightforward. 
It requires `docker` installed and can be controlled under the current user without `sudo`.
Running the following script at the root directory of this repo:
```
./docker-build.sh
```
Wait for several minutes until it finishes. It will create a docker image `synguar:v1`.

**Alternative:** The docker image is also available at [here](https://hub.docker.com/r/unionss/synguar/tags?page=1&ordering=last_updated). Use the following commands to replace the docker build (`./docker-build.sh`) in case the build is not successful:
```
docker pull unionss/synguar:v0.1.0
docker tag unionss/synguar:v0.1.0 synguar:v1
```

Notice that the above commands only replace the build step. You still need this repo to start the container with mounted data folders. Please see the steps below.

## Run, stop and connect

- Run the container forever in the background:
  ```
  ./docker-run.sh
  ```
  It will create a container named `synguar-all`.

- Stop the container:
  ```
  ./docker-stop.sh
  ```
  It will stop and remove the container named `synguar-all`.
- Connect to the container after it is running:
  ```
  ./docker-connect.sh
  ```
  It will run a bash shell for interactive operation.
  To check if the build is successful, after connecting to the container, run the following command:
  ```
  root@d1fbb70858d4:/home/synguar/SynGuarAll# ./build-strstun/strstun -hello
  [synthesizer] Argument Error: unknown flag -hello
  root@d1fbb70858d4:/home/synguar/SynGuarAll# dotnet ./StrPROSE-synthesizer/StrPROSE/bin/Debug/netcoreapp3.1/StrPROSE.dll --help

          StrPROSE 0.1
          2021.3.20


  Command line argments required.

  --samplegen <benchmark_dir> <benchmark_id> <seed> <count> <output_dir>
            Generate examples based on a benchmark program specified by <benchmark_id>.csv and <benchmark_id>.txt
  --synthloop <example_file> <cache_dir>
            Start a synthesize loop. <example_file> is a csv file with I/O examples.
  --test <test_dir>
            Select a csv file in <testpath> to do synthesize. (for debugging)
  root@d1fbb70858d4:/home/synguar/SynGuarAll#
  ```
  These outputs indicate that the build is successful.
## Port mapping

The ports `5261`, `5262`, `5265` are mapped to the host machine. From the host machine can access them directly through `localhost:526X`.

## Directory Structure inside Docker

Part of the current repo is mapped to `/home/synguar/SynGuarAll/` folder, and the directory structure is preserved.
- `/home/synguar/SynGuarAll/` is the root of this repo inside the docker container.

- 3 folders (code folders) are copied into docker image during build time:
    - StrPROSE-synthesizer
    - StrSTUN-synthesizer
    - SynGuar
  
- 1 folder is created during build time:
    - build-strstun
  
- 2 folders (data folders) are mounted while running:
    - benchmark
    - outputs

This means that data folders are mounted. Generating examples and running the evaluation will create persistent files in those folders on the host machine.

Any changes to the code folders require rebuilding the docker image.

## Running evaluation/synthesizers/SynGuar

To run the evaluation, please see [Evaluation.md](./Evaluation.md).

To run the synthesizers manually, please see [Details-StrPROSE.md](./Details-StrPROSE.md) and [Details-StrSTUN.md](./Details-StrSTUN.md).

To run the SynGuar algorithm with specified synthesizer on i.i.d. samples, see [Details-SynGuar.md](./Details-SynGuar.md).