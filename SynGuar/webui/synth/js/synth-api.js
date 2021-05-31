const hostport = 'http://localhost:5261';
function synth_api_get(path, data, callback) {
    axios.get(hostport + path)
    .then(function (response) {
        // handle success
        if (response.status != 200) {
            console.error("# [synth_api_get] Response not 200:", response);
        } else {
            callback(response.data);
        }
    })
    .catch(function (error) {
        console.log(error);
    })
    .then(function () {
        // always executed
    });
}

function synth_api_status_perf(callback) {
    synth_api_get("/status/perf", {}, callback);
}

function synth_api_status_tasks(callback) {
    synth_api_get("/status/tasks", {}, callback);
}

function synth_api_status_workers(callback) {
    synth_api_get("/status/workers", {}, callback);
}

function synth_api_status_failures(callback) {
    synth_api_get("/status/failures", {}, callback);
}