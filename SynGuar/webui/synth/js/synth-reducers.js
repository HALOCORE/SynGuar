const {
  createAction, 
  createReducer,
  configureStore
} = window.RTK;

const preloadedState = {
  status: {
      perf: {},
      tasks: {
        "task_waiting": [],
        "task_history": []
      },
      workers: {},
      failures: {
        failure_count: 0,
        failure_worker_ids: []
      }
  },
  results: {}
}

const aStatusGetPerf = createAction('status/getperf');
const aStatusGetTasks = createAction('status/gettasks');
const aStatusGetWorkers = createAction('status/getworkers');
const aStatusGetFailures = createAction('status/getfailures');

const aStatusRespPerf = createAction('status/respperf', (data) => {return {payload: data};});
const aStatusRespTasks = createAction('status/resptasks', (data) => {return {payload: data};});
const aStatusRespWorkers = createAction('status/respworkers', (data) => {return {payload: data};});
const aStatusRespFailures = createAction('status/respfailures', (data) => {return {payload: data};});

const statusReducer = createReducer(preloadedState.status, (builder) => {
  builder.addCase(aStatusGetPerf, (state, action) => {
    synth_api_status_perf((data) => {
      // console.log("# aStatusGetPerf:", data);
      window.myReduxStore.dispatch(aStatusRespPerf(data));
    })
  });
  builder.addCase(aStatusGetTasks, (state, action) => {
    synth_api_status_tasks((data) => {
      console.log("# aStatusGetTasks:", data);
      window.myReduxStore.dispatch(aStatusRespTasks(data));
    })
  });
  builder.addCase(aStatusGetWorkers, (state, action) => {
    synth_api_status_workers((data) => {
      console.log("# aStatusGetWorkers:", data);
      window.myReduxStore.dispatch(aStatusRespWorkers(data));
    })
  });
  builder.addCase(aStatusGetFailures, (state, action) => {
    synth_api_status_failures((data) => {
      console.log("# aStatusGetFailures:", data);
      window.myReduxStore.dispatch(aStatusRespFailures(data));
    })
  });
  builder.addCase(aStatusRespPerf, (state, action) => {
    let data = action.payload;
    state.perf = data;
  });
  builder.addCase(aStatusRespTasks, (state, action) => {
    let data = action.payload;
    state.tasks = data;
  });
  builder.addCase(aStatusRespWorkers, (state, action) => {
    let data = action.payload;
    state.workers = data;
  });
  builder.addCase(aStatusRespFailures, (state, action) => {
    let data = action.payload;
    state.failures = data;
  })
});

const aSynthGet = createAction('synth');
const resultsReducer = createReducer(preloadedState.results, (builder) => {
  builder.addCase(aSynthGet, (state, action) => {
    console.log(action);
    return state;
  });
});


const reducer = {
  status: statusReducer,
  results: resultsReducer
}
