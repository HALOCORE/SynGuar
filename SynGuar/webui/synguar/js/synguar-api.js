const hostport = 'http://localhost:5262';
function synguar_api_get(path, data, callback) {
    axios.get(hostport + path)
    .then(function (response) {
        // handle success
        if (response.status != 200) {
            console.error("# [synguar_api_get] Response not 200:", response);
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

function synguar_api_session_ids(callback) {
    synguar_api_get("/sessions/ids", {}, callback);
}

function synguar_api_session_runnings(callback) {
    synguar_api_get("/sessions/running", {}, callback);
}

function synguar_api_session_waitings(callback) {
    synguar_api_get("/sessions/waiting", {}, callback);
}
