# Build & Run (Docker)

## Build the environment

Build with Docker is straight-forward. 
It requires `docker` installed and can be controlled under current user without `sudo`.
Running the following script at the root directory of this repo:
```
./docker-build.sh
```
Wait for several minutes until it finishes. It will create a docker image `synguar:v1`.

## Run, stop, and connect

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

## Port mapping

The ports `5261`, `5262`, `5265` are mapped to the host machine. From the host machine can access them directly through `localhost:526X`.

## Directory Structure inside Docker

Part of the current repo are mapped to `/home/synguar/SynGuarAll/` folder, and the directory structure is perserved.
- `/home/synguar/SynGuarAll/` is the root of this repo inside docker container.

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

Any changes to the code folders requires rebuilding the docker image.

## Run the evaluation or the synthesizers 

To run the evaluation, please see [Evaluation.md](./Evaluation.md).

To run the synthesizers manually, please see [Details-StrPROSE.md](./Details-StrPROSE.md) and [Details-StrSTUN.md](./Details-StrSTUN.md).


