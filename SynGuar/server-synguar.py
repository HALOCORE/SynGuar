from flask import Flask, request
from flask_restful import Api
from flask_cors import CORS, cross_origin
import sys
import os
import json

from server_synguar.session_mgr import SessionManager

abs_join = lambda p1, p2 : os.path.abspath(os.path.join(p1, p2))
SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
CACHE_SYNGUAR_RELDIR = "../outputs/cache/SynGuar"
CACHE_SYNGUAR_FULLDIR = abs_join(SCRIPT_DIR, CACHE_SYNGUAR_RELDIR)

app = Flask(__name__)
CORS(app)
api = Api(app)

config = {
    "synth_port": 5261,
    "synguar_thread_limit": 16,
    "synguar_port": 5262
}
# update config
print("\n\n      SYNGUAR Server\n\n")
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



SYNTH_API_ENDPOINT = f"http://localhost:{str(config['synth_port'])}/synth"

session_mgr = SessionManager(
    api_endpoint=SYNTH_API_ENDPOINT, 
    cache_folder=CACHE_SYNGUAR_FULLDIR,
    thread_limit=config["synguar_thread_limit"])
session_mgr.start()

@app.route('/')
@cross_origin()
def welcome():
    return """
<h2>This is SYNGUAR server</h2>

<p>For Web interface, run webui.py, then:</p>
<p>Please go to <webui-host>/ui/synguar/dashboard.html to see the server status.</p>
<p>Please go to <webui-host>/ui/synguar/help.html to see usage.</p>
"""

@app.route('/synguar', methods=["POST"])
@cross_origin()
def api_synguar():
    exe_result = session_mgr.process_session_request(request.json)
    return exe_result

@app.route('/sessions/running')
@cross_origin()
def api_sessions_running():
    return session_mgr.get_session_status_filtered("RUNNING")

@app.route('/sessions/waiting')
@cross_origin()
def api_sessions_waiting():
    return session_mgr.get_session_status_filtered("WAITING")

@app.route('/sessions/ids')
@cross_origin()
def api_session_ids():
    return {
        "RUNNING": [x for x in session_mgr.get_session_status_filtered("RUNNING")],
        "WAITING": [x for x in session_mgr.get_session_status_filtered("WAITING")],
        "DONE": [x for x in session_mgr.get_session_status_filtered("DONE")]
    }

@app.route('/sessions/trace/<session_id>')
@cross_origin()
def api_sessions_trace(session_id):
    return session_mgr.get_session_trace(session_id)


# Running the server
print("Api running on port : {} ".format(config['synguar_port']))
app.run(host="0.0.0.0", port=config['synguar_port'], debug=True)