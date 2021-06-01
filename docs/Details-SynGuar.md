# SynGuar local servers

`SynGuar` algorithm and evaluation pipeline is implemented as a set of REST API servers. 

## Install Dependencies (For manual build only)

If using docker, please ignore this secion.
### Step 1: Check python3
`SynGuar` is tested on `python3.7` and `python3.8`.

Make sure `python3` and `pip3` is available in the environment.

If not, use the following commands to install python3 (`python 3.8` for example):
```
sudo apt-get install python3.8 python3.8-dev python3-pip
```

### Step 2: Install pip packages
Use the following command to install `Flask` related packages, `psutil`, and `requests`.
```
pip3 install Flask==1.1.2 \
      Flask-Cors==3.0.10 \
      Flask-RESTful==0.3.8 \
      psutil==5.8.0 \
      requests==2.18.4
```

## Run local servers

The local servers consists of 3 servers:

- `Synth` Server (`server-synth.py`): The server that schedule synthesis requests and run `StrPROSE` or `StrSTUN` to get synthesis results.
- `SynGuar` Server (`server-synghar.py`): The server that runs `SynGuar` algorihm and communicate with `Synth` Server to get final results that have generalization guarantee.
- WebUI Dashboard (`webui.py`): Host static `html/css/js` files and provides dashboards in browser for monitoring the status of `Synth` Server and `SynGuar` Server. 

To run those servers, open 3 terminal windows under the folder `./SynGuar` (If using docker, use `docker-connect.sh` to connect 3 interactive shells or run them as 3 background tasks). Run each of those 3 commands on a terminal:
```
python3 server-synth.py    # default configuration
python3 server-synguar.py  # default configuration
python3 webui.py
```

The third one is for the monitoring interface in the browser.

The first two commands will start server with default configuration for testing the functionality rather than finishing the evaluation. To run `server-synth.py` and `server-synguar.py` with proper thread limit, memory limit and port, use `--config <config-file>` parameter:
```
python3 server-synth.py --config ./server-config-512g-16thread.json
python3 server-synguar.py --config ./server-config-512g-16thread.json
```
An example of a config file is at `./SynGuar/server-config-512g-16thread.json`. The unit for `synth_memory_limit` is in **MB**. `synth_thread_limit` and `synguar_thread_limit` are suggested to set to the same number, and 
`synguar_thread_limit` should divide `synth_memory_limit`.

After the server starts, go to the following link for monitoring server status (such as memory usage and waiting tasks):
- Monitoring `server-synth.py`:  http://localhost:5265/ui/synth/dashboard.html
- Monitoring `server-synguar.py`:  http://localhost:5265/ui/synguar/dashboard.html

After the servers are up and running, they can accept  `POST` requests from evaluation scripts.

## Run evaluation

See [Evaluation.md](./Evaluation.md) for how to run the evaluation.

## Notice on manual termination

If `server-synth.py` is terminated manually while there are running workers, please 
also manually kill the running synthesizers not killed by `server-synth.py`.

## More details: submit synthesis tasks to SynGuar (Automated by evaluation scripts)

### Submit `SynGuar` task to `server-synguar.py`

The long running `server-synguar.py` accepts `synguar` API call, and query `server-synth.py` to get synthesis results.

- API endpoint: `http://localhost:5261/synguar`
- `server-synguar.py`:

- example request:

  ```
  POST http://localhost:5262/synguar
  content-type: application/json

  {
      "synthesizer": "StrPROSE",
      "example_file": "prog1.seed1010.csv",
      "epsilon": 0.05,
      "delta": 0.02,
      "k": 1
  }
  ```

  Dupliated requests will be automatically ignored. After the `SynGuar` algorithm for the request terminates, the above request will return final result and the trace of the algorithm. 

  The meaning of $\epsilon$, $\delta$ and $k$ is explained in the full paper. Briefly, $\epsilon$ and $\delta$ are parameters for generalization: with probability at least $1-\delta$, the synthesized program has true error smaller than $\epsilon$ on the sampling distribution. $k$ is the granularity of taking I/O examples in the `SynGuar` algorithm. For example, set $k = 1$ the SynGuar algorithm will take I/O examples one by one.

### Submit single synthesis task to `server-synth.py`
The long running `server-synth.py` accepts `synth` API call, and spawn workers to do them.

- API endpoint: `http://localhost:5261/synth`

- example request:
  ```
  POST http://localhost:5261/synth
  content-type: application/json

  {
      "synthesizer": "StrPROSE",
      "example_file": "prog1.seed1029.csv",
      "example_size": 28,
      "keepalive": 12,
      "no_counting": false
  }
  ```

  Dupliated requests will be automatically ignored. After the synthesis job is finished, the result will appear in the response data for the above request.

  The request above means to spawn a `StrPROSE` synthesizer process to synthesize a program with program space counting on the first 28 examples from the file `prog1.seed1029.csv`, and after the result is ready, the spawned sprocess will be killed after 12 seconds without new incomming synthesis requests for this process. 