# Build & Run (Directly on Ubuntu)

If want to use docker, please see [BuildRun-Docker.md](./BuildRun-Docker.md) instead.

Follow the steps below to build direcly on the host machine. `Ubuntu 18.04` and `Ubuntu 20.04` are tested.

## Build Steps

### Step 1: Build `StrPROSE` synthesizer

It requires `dotnet-sdk-5.0` and `dotnet-runtime-3.1` to build and run. 

The source of `StrPROSE` is in the `./StrPROSE-synthesizer` folder.

See [Details-StrPROSE.md](./Details-StrPROSE.md) for detailed building process.

### Step 2: Build `StrSTUN` synthesizer

It requires `gcc (support C++11)`, `CMake(3.10+)`, `gmp 6.2.1` to build.

The source of `StrSTUN` is in the `./StrSTUN-synthesizer` folder.

See [Details-StrSTUN.md](./Details-StrSTUN.md) for detailed building process.

### Step 3: Install dependencies for `SynGuar` servers

It requires `python3` (tested on `python3.7` and `python3.8`) with the following package installed:
`Flask`, `Flask-Cors`, 
`Flask-RESTful`, `psutil`, and `requests`.

See [Details-SynGuar.md](./Details-SynGuar.md) for detailed installation process.

## Running

To run the evaluation, please see [Evaluation.md](./Evaluation.md).

To run the synthesizers manually, please see [Details-StrPROSE.md](./Details-StrPROSE.md) and [Details-StrSTUN.md](./Details-StrSTUN.md).