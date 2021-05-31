import os
import json



class SessionCache():
    def __init__(self, synguar_cache_dir):
        print("# [cache] object created.")
        print("# [cache] synguar_cache_dir: ", synguar_cache_dir)
        self.synguar_cache_dir = synguar_cache_dir
        self.traces = {}

    def fetch_trace(self, session_id):
        if session_id in self.traces: return
        filename = session_id + ".json"
        full_filename = os.path.join(self.synguar_cache_dir, filename)
        if os.path.exists(full_filename):
            with open(full_filename, 'r') as f:
                trace_data = json.load(f)
                self.traces[session_id] = trace_data
            print("# [SessionCache] fetch " + filename)

    def get_trace(self, session_id):
        if session_id in self.traces:
            return self.traces[session_id]
        else:
            return {}

    def can_get_trace(self, session_id):
        return session_id in self.traces

    def create_trace_dict_for_session_id(self, session_id):
        assert session_id not in self.traces
        self.traces[session_id] = {}
        return self.traces[session_id]
    
    def save_trace(self, session_id):
        if session_id not in self.traces:
            print("# [SessionCache] WARNING: save_trace get empty.")
            return
        data = self.traces[session_id]
        filename = session_id + ".json"
        full_filename = os.path.join(self.synguar_cache_dir, filename)
        with open(full_filename, 'w') as f:
            json.dump(data, f, indent=2)
        print("# [SessionCache] save " + filename)
