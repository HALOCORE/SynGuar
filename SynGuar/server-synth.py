from flask import Flask, request
from flask_restful import Api
from flask_cors import CORS, cross_origin
import sys
import os
import json
from server_synth.scheduler import Scheduler, load_json

abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))
SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
CACHE_STRPROSE_FULLDIR = abs_join(SCRIPT_DIR, "../outputs/cache/StrPROSE")
CACHE_STRSTUN_FULLDIR = abs_join(SCRIPT_DIR, "../outputs/cache/StrSTUN")
MEM_ESTIMATION_FILEPATH = abs_join(SCRIPT_DIR, "../outputs/cache/synth-mem.json")

app = Flask(__name__)
CORS(app)
api = Api(app)

config = {
    "synth_port": 5261,
    "synth_thread_limit": 4,
    "synth_memory_limit": 8192
}
# update config
print("\n\n      SYNTH Server\n\n")
if len(sys.argv) == 1:
    print("# using default configuration.")
elif len(sys.argv) == 3 and sys.argv[1] == "--config":
    print("# reading configuration from:", sys.argv[2])
    file_config = {}
    with open(sys.argv[2], 'r') as f: 
        file_config = json.load(f)
    for k in file_config: config[k] = file_config[k]
else:
    print("# ERROR: The parameters should be --config <json-file>")
    exit(1)
print("# Configuration:")
print(json.dumps(config, indent=2))


# create & run the scheduler
tasksched = Scheduler(
    CACHE_STRPROSE_FULLDIR, 
    CACHE_STRSTUN_FULLDIR,
    MEM_ESTIMATION_FILEPATH,
    thread_limit=config["synth_thread_limit"], #6,
    memory_limit=config["synth_memory_limit"])  #24576)

tasksched.start()


@app.route('/')
@cross_origin()
def welcome():
    return """
<h2>This is SYNTH server</h2>

<p>For Web interface, run webui.py, then:</p>
<p>Please go to <webui-host>/ui/synth/dashboard.html to see the server status.</p>
<p>Please go to <webui-host>/ui/synth/help.html to see usage.</p>
"""

@app.route('/synth', methods=["POST"])
@cross_origin()
def api_synth():
    exe_result = tasksched.process_synth_task(request.json)
    return exe_result

@app.route('/status/perf')
@cross_origin()
def api_status_perf():
    return tasksched.get_perf_status()

@app.route('/status/tasks')
@cross_origin()
def api_status_tasks():
    return {
        "tasks_waiting": tasksched.get_tasks_waiting(),
        "tasks_history": tasksched.get_tasks_history(0, -1)
    }

@app.route('/status/workers')
@cross_origin()
def api_status_workers():
    return tasksched.get_workers_status()

@app.route('/status/failures')
@cross_origin()
def api_status_failures():
    return tasksched.get_failure_info()


# running the server
print("Api running on port : {} ".format(config["synth_port"]))
app.run(host="0.0.0.0", port=config["synth_port"], debug=True)