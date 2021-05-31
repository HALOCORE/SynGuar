import threading 
import time

from flask.globals import session

from server_synguar.session import Session
from server_synguar.session_cache import SessionCache

SESSION_KEY_SEP = "$"
class SessionManager():
    def __init__(self, api_endpoint, cache_folder, thread_limit=3):
        self.session_status_dict = {}
        self.session_running = {}
        self.THREAD_LIMIT = thread_limit
        self._manager_thread = None
        self.api_endpoint = api_endpoint
        self.session_cache = SessionCache(cache_folder)
        print("# [SessionManager] init. thread_limit:", thread_limit, " api_endpoint:", api_endpoint)
    
    def process_session_request(self, session_spec):
        synthesizer = session_spec["synthesizer"]
        example_file = session_spec["example_file"]
        epsilon = float(session_spec["epsilon"])
        delta = float(session_spec["delta"])
        k = int(session_spec["k"])
        cache_only = False if "cache_only" not in session_spec else session_spec["cache_only"]
        assert(synthesizer == "StrPROSE" or synthesizer == "StrSTUN")
        assert(epsilon > 0 and delta > 0 and k > 0)
        assert(k == int(session_spec["k"]))
        session_key = synthesizer + SESSION_KEY_SEP + example_file + SESSION_KEY_SEP + str(epsilon) + SESSION_KEY_SEP + str(delta) + SESSION_KEY_SEP + str(k)
        # TODO: session key dedup
        # print(session_key, self.session_status_dict)
        if session_key not in self.session_status_dict:
            self.session_status_dict[session_key] = {
                "synthesizer": synthesizer,
                "example_file": example_file,
                "epsilon": epsilon,
                "delta": delta,
                "k": k,
                "status": "WAITING",
                "cache_only": cache_only,
                "key": session_key
            }
        else:
            print("# [SessionManager] duplicated session: ", session_key)
        return {
            "status": self.session_status_dict[session_key],
            "trace": self.get_session_trace(session_key)
        }
    
    def get_session_trace(self, session_id):
        return self.session_cache.get_trace(session_id)

    def get_session_status_filtered(self, filter="RUNNING"):
        filtered_dict = {}
        for session_id in self.session_status_dict:
            status_data = self.session_status_dict[session_id]
            if status_data["status"] == filter:
                filtered_dict[session_id] = status_data
        return filtered_dict
    
    def start(self):
        self._manager_thread = threading.Thread(target = self._run)
        print("# [SessionManager] start session manager thread ...")
        self._manager_thread.start()

    def _check_release_sessions(self):
        for session_id in list(dict.keys(self.session_running)):
            session = self.session_running[session_id]
            if session.running_status == "DONE":
                self.session_status_dict[session_id]["status"] = "DONE"
                del self.session_running[session_id]
                self.session_cache.save_trace(session_id)
   
    def _run(self):
        print("# [SessionManager-thread] thread is running ...")
        while(True):
            time.sleep(2)
            self._check_release_sessions()
            while(len(self.session_running) < self.THREAD_LIMIT):
                self._check_release_sessions()
                time.sleep(2)
                for session_id in list(dict.keys(self.session_status_dict)):
                    session_status = self.session_status_dict[session_id]
                    if session_status["status"] == "WAITING":
                        self.session_cache.fetch_trace(session_id)
                        if self.session_cache.can_get_trace(session_id):
                            print("# [SessionManager] Ignored (nothing to do). session already in traces (this run)", session_id)
                            session_status["status"] = "DONE"
                        elif session_status["cache_only"] == True:
                            print("# [SessionManager] cache_only but Cache Miss (return None)", session_id)
                            session_status["status"] = "CACHEMISS"
                        else:
                            trace_dict_out = self.session_cache.create_trace_dict_for_session_id(session_id)
                            session = Session(
                                session_status["synthesizer"],
                                session_status["example_file"],
                                session_status["epsilon"],
                                session_status["delta"],
                                session_status["k"],
                                self.api_endpoint,
                                trace_dict_out)
                            self.session_running[session_id] = session
                            self.session_status_dict[session_id]["status"] = "RUNNING"
                            session.start()
                            # break the for loop
                            break