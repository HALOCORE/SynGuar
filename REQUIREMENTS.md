# Requirements for build and run the artifact

## With Docker
- linux system with Docker and bash shell
- Docker is accessible without `sodo`
- Local TCP port 5261, 5262, 5265 available
- 64bit x86 processor suggested
## Without Docker
- Ubuntu 18.04 / Ubuntu 20.04
- Python 3.7/3.8 or higher
- GCC (support C11), CMake (3.10+)
- dotnet SDK 5.0 and dotnet runtime 3.1 (We will show how to install them)
- Local TCP port 5261, 5262, 5265 available
- 64bit x86 processor suggested
# Requirements for running the evaluation
- Evaluation tasks for `StrPROSE` baseline and `SynGuar-PROSE`
  - 8GB memory or more
- Evaluation tasks for `StrSTUN` baseline
  - 8GB memory or more
- Evaluation tasks for `SynGuar-STUN`
  - 16GB memory (can run some of the benchmarks)
  - 32GB memory (can run more than half of the benchmarks)
  - 128GB memory (can run 98% of the benchmarks)
  - 512GB memory (can run all of the benchmarks)