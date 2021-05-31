const {
  createAction, 
  createReducer,
  configureStore
} = window.RTK;

const preloadedState = {
  sessions: {
      ids: {},
      status:{
        RUNNING: {},
        WAITING: {}
      }
  },
  traces: {},
}

const aSessionGetIds = createAction('status/getids');
const aSessionGetRunnings = createAction('status/getrunnings');
const aSessionGetWaitings = createAction('status/getwaitings');

const aSessionRespIds = createAction('status/respids', (data) => {return {payload: data};});
const aSessionRespRunnings = createAction('status/resprunnings', (data) => {return {payload: data};});
const aSessionRespWaitings = createAction('status/respwaitings', (data) => {return {payload: data};});

const sessionReducer = createReducer(preloadedState.sessions, (builder) => {
  builder.addCase(aSessionGetIds, (state, action) => {
    synguar_api_session_ids((data) => {
      console.log("# aSessionGetIds:", data);
      window.myReduxStore.dispatch(aSessionRespIds(data));
    })
  });
  builder.addCase(aSessionGetRunnings, (state, action) => {
    synguar_api_session_runnings((data) => {
      console.log("# aSessionGetRunnings:", data);
      window.myReduxStore.dispatch(aSessionRespRunnings(data));
    })
  });
  builder.addCase(aSessionGetWaitings, (state, action) => {
    synguar_api_session_waitings((data) => {
      console.log("# aSessionGetWaitings:", data);
      window.myReduxStore.dispatch(aSessionRespWaitings(data));
    })
  });
  builder.addCase(aSessionRespIds, (state, action) => {
    let data = action.payload;
    state.ids = data;
  });
  builder.addCase(aSessionRespRunnings, (state, action) => {
    let data = action.payload;
    state.status.RUNNING = data;
  });
  builder.addCase(aSessionRespWaitings, (state, action) => {
    let data = action.payload;
    state.status.WAITING = data;
  });
  
});

const aTraceGet = createAction('trace/some_session_id');
const traceReducer = createReducer(preloadedState.traces, (builder) => {
  builder.addCase(aTraceGet, (state, action) => {
    console.log("# aTraceGet. Action Data:", action);
    return state;
  });
});


const reducer = {
  sessions: sessionReducer,
  traces: traceReducer
}
