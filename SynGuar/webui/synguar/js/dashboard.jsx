const {
  colors,
  CssBaseline,
  ThemeProvider,
  createMuiTheme,

  Typography,
  Container,
  Box,
  Button,
  List,
  ListItem,
  ListItemText,

  Card,
  CardActions,
  CardContent,

  Grid,
  Paper
} = MaterialUI;

const {
  createStore
} = window.RTK;

const {
  Provider,
  useSelector
} = window.ReactRedux;

const store = configureStore({
  reducer,
  devTools: true,
  preloadedState
});
window.myReduxStore = store;

setInterval(() => {
  store.dispatch(aSessionGetIds());
}, 3000);

setTimeout(() => setInterval(() => {
  store.dispatch(aSessionGetRunnings());
}, 3000), 500);

const theme = createMuiTheme({
  palette: {
    primary: {
      main: '#556cd6',
    },
    secondary: {
      main: '#19857b',
    },
    error: {
      main: colors.red.A400,
    },
    background: {
      default: '#fff',
    },
  },
});

function SessionIdStat() {
  const ids = useSelector(state => state.sessions.ids);
  let running_ids = [];
  let running_count = 0;
  let waiting_ids = [];
  let waiting_count = 0;
  let done_ids = [];
  let done_count = 0;
  if ("RUNNING" in ids) {
    running_ids = ids["RUNNING"];
    running_count = running_ids.length;
  }
  if ("WAITING" in ids) {
    waiting_ids = ids["WAITING"];
    waiting_count = waiting_ids.length;
  }
  if("DONE" in ids) {
    done_ids = ids["DONE"];
    done_count = done_ids.length;
  }
  return (
    <div>
      <h3>DONE: {done_count}</h3>
      <h3>RUNNING: {running_count}</h3>
      <h3>WAITING: {waiting_count}</h3>
    </div>
  )
}

function SessionStatusList(props) {
  let {filter} = props;
  const sessions = useSelector(state => state.sessions.status[filter]);
  let sessionsList = [];
  if(sessions) {
    for(let session_id in sessions){
      sessionsList.push(sessions[session_id]);
    }
  }
  return (<div className="panel-sessions-list">
    <List>
      {
        sessionsList.map((session) => {
          return (<ListItem alignItems="flex-start" key={session.key}>
            <ListItemText
              primary={"[Session] " + session.key + "   " + session.status}
            ></ListItemText>
          </ListItem>)
        })
      }
    </List>
  </div>);
}

function App() {
  return (
    <Provider store={store}>
      <ThemeProvider theme={theme}>
        <div className="top-provider">
          {/* CssBaseline kickstart an elegant, consistent, and simple baseline to build upon. */}
          <CssBaseline />
          <h1>SYNGUAR Server Dashboard</h1>
          <SessionIdStat></SessionIdStat>
          <h2>Running List</h2>
          <SessionStatusList filter={"RUNNING"}/>
          <div>
            <div className="block-inline margin-right-medium">
              <h2>Waining List</h2>
            </div>
            <div className="block-inline">
              <Button variant="contained" color="primary" onClick={() => window.myReduxStore.dispatch(aSessionGetWaitings())}>Update Waiting Sessions</Button>
            </div>
          </div>
          <SessionStatusList filter={"WAITING"}/>
        </div>
      </ThemeProvider>
    </Provider>
  );
}

function RenderDashboardInteractives() {
  let app = document.getElementById("app");
  ReactDOM.render(<App />, app);
}

console.log("# RenderDashboardInteractives ...");
RenderDashboardInteractives();