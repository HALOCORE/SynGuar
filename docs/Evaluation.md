# Evaluation

## Step 1: Generate examples
Before running the evaluation, the first step is to generate 
examples for the two benchmarks (using the default random seeds).

See [Benchmark.md](./Benchmark.md) for how to **Generate example files for evaluation**.

## Step 2: Start running the local servers

This includes the `server-synth.py`, `server-synguar.py`, and `webui.py` (for the monitoring dashboard).

Notice: Different evaluation task requires a different amount of memory (different configuration files).
Suggested memory capacity is listed in the next step. **Please run `server-synth.py` and `server-synguar.py` with suggested memory/threads configuration according to different evaluation task. Suggestions are listed in Step 3.**

See [Details-SynGuar.md](./Details-SynGuar.md) for how to **Run SynGuar** with configurations.

After starting the servers, open the following link for monitoring the server status:
- Monitoring `server-synth.py`:  http://localhost:5265/ui/synth/dashboard.html
- Monitoring `server-synguar.py`:  http://localhost:5265/ui/synguar/dashboard.html

## Step 3: Post evaluation tasks to local servers

Then post evaluation task (4 of them, each consists of a batch of requests) one by one. After posting each batch, look at dashboards in the browser and wait until all requests finish running. 
To post those tasks, first change the working directory to `./SynGuar`.

There are 4 tasks (4 batches of evaluation requests):

- Run benchmarks for `SynGuar-PROSE`
  - Command: `python3 evaluation.py run-prose`
  - Notice: 2GB memory per thread configuration is enough 
  - Example Configuration: [server-config-8g-4thread.json](../SynGuar/server-config-8g-4thread.json)

- Run benchmarks for `SynGuar-STUN`
  - Command: `python3 evaluation.py run-stun`
  - Notice: 512GB total memory required to finish all benchmarks (Amazon EC2 memory-optimized server suggested). 32GB: more than half; 128GB: 98%; 512GB: 100%. However, you can still run this task with `16GB` or `32GB` total memory configuration to get partial results. OOM jobs will be killed and be marked in red on the monitoring dashboard for `server-synth.py`.
  - Example Configuration: [server-config-512g-16thread.json](../SynGuar/server-config-512g-16thread.json)
  
- Task 3: Hypothesis Space drop for `SynGuar-PROSE`
  - Command: `python3 evaluation.py sdrop-prose`
  - Notice: 2GB memory per thread is enough
  - Example Configuration: [server-config-8g-4thread.json](../SynGuar/server-config-8g-4thread.json)

- Run baseline (4-examples) on `strstun` benchmarks
  - Command: `python3 evaluation.py 4eg-stun`
  - Notice: 4GB memory per thread is enough
  - Example Configuration: [server-config-24g-6thread.json](../SynGuar/server-config-24g-6thread.json)
  
The servers will cache results for all the requests, so it is OK 
to stop those servers and start them later. Finished requests will be fetched from the cache (the `outputs/cache` folder).

Proper configuration for `server-synth.py` and `server-synguar.py` is 
essential for finishing the evaluation:
- The `synth_memory_limit` should be at least the required peak memory for that task
- `synth_thread_limit` must be able to divide `synth_memory_limit` 
- `synguar_thread_limit` can be any value equal or larger than `synth_thread_limit` for maximum parallelization

If under a configuration the task is stuck, it is safe to simply stop the servers and restart with a larger `synth_memory_limit` to run the rest of the task.


### Where to find partial results
If there is not enough memory or time to finish all the tasks, you can find partial results under those folders:
- `outputs/cache/StrPROSE/`: The synthesis results for `strprose` benchmark
- `outputs/cache/StrSTUN/`: The synthesis results for `strstun` benchmark
- `outputs/cache/SynGuar/`: The trace and final results of the finished runs of the `SynGuar` algorithm (for both `strprose` and `strstun` benchmarks)

## Step 4: Collect data

After the above evaluation requests are finished, run the following commands to collect results and output to `./outputs/summaries` (requires `server-synth.py` and `server-synguar.py` to be up and running).

First, change the working directory to `./SynGuar`, then run the following:

- Collect data related to `strprose` benchmark
  `python3 evaluation.py data-prose`

- Collect data related to `strstun` benchmark
  `python3 evaluation.py data-stun`


