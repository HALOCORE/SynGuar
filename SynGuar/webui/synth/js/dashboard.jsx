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
  store.dispatch(aStatusGetPerf());
}, 2000);

setTimeout(() => setInterval(() => {
  store.dispatch(aStatusGetWorkers());
}, 4000), 500);

setTimeout(() => setInterval(() => {
  store.dispatch(aStatusGetFailures());
}, 8000), 1500);

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

function PerfCard(props) {
  let { head, title, subtitle } = props;
  return (
    <Card>
      <CardContent className="card-middlewidth">
        <Typography className="card-title" color="textSecondary" gutterBottom>
          {head}
        </Typography>
        <Typography variant="h5" component="h2">
          {title}
        </Typography>
        <Typography className="card-subtitle" color="textSecondary">
          Detail
            </Typography>
        <Typography variant="body2" component="p">
          {subtitle}
        </Typography>
      </CardContent>
      {/* <CardActions>
            <Button size="small">Learn More</Button>
          </CardActions> */}
    </Card>
  );
}

function PerfBar() {
  const perfdata = useSelector(state => state.status.perf);
  //console.log("# PerfBar:", perfdata);
  let { cpu_overall, cpu_percpu, memory_available, memory_percent, memory_total } = perfdata;
  let cpu_title = "N/A";
  if (cpu_overall != undefined) cpu_title = cpu_overall + "%";
  let cpu_sub = "N/A";
  if (cpu_percpu) cpu_sub = cpu_percpu.map(x => x + "%").join("  |  ");
  let memory_title = "N/A";
  if (memory_percent) memory_title = memory_percent + "%";
  let memory_sub = "N/A";
  if (memory_available) memory_sub = (memory_available / (1024 * 1024 * 1024)).toFixed(2) + " GB Free / " + (memory_total / (1024 * 1024 * 1024)).toFixed(2) + " GB Total";
  return (
    <Grid container spacing={3}>
      <Grid item xs={6}>
        <PerfCard head={"CPU Usage"} title={cpu_title} subtitle={cpu_sub}></PerfCard>
      </Grid>
      <Grid item xs={6}>
        <PerfCard head={"Memory Usage"} title={memory_title} subtitle={memory_sub}></PerfCard>
      </Grid>
    </Grid>
  )
}

function WorkerList() {
  const workersData = useSelector(state => state.status.workers);
  let workerList = [];
  for (let key in workersData) workerList.push(workersData[key]);
  return (<List>
    {
      workerList.map((worker, idx) => {
        let infoline = "[" + worker.process_pid + "] " + worker.worker_id;
        let supline = worker.synthesizer + " | " + worker.example_file + " | " + worker.last_example_size;
        let mem = (worker.perf_status.rss / (1024 * 1024 * 1024)).toFixed(2) + " GB,  Reservation: " + (worker.mem_reservation_limit / 1024).toFixed(2) + " GB";
        let infoline2 = "   failure: " + worker.is_failure +  "   released: " + worker.is_released +  "   keepalive: " + worker.keepalive + "s" + "   Idle: " + worker.is_idle;
        return (
          <ListItem alignItems="flex-start" key={idx}>
            <ListItemText
              primary={
                <React.Fragment>
                  <Typography>{"Worker  " + infoline}</Typography>
                  <Typography variant="body2" className="text-list-item-head-sup">{supline}</Typography>
                </React.Fragment>
              }
              secondary={
                <React.Fragment>
                  <Typography
                    component="span"
                    variant="body2"
                    className="text-inline"
                    color="textPrimary"
                  >
                    Memory: {mem}
                  </Typography>
                  &nbsp;&nbsp; {infoline2}
                </React.Fragment>
              }
            />
          </ListItem>
        );
      })
    }
  </List>);
}

function FailureInfo() {
  const failures = useSelector(state => state.status.failures);
  let { failure_count, failure_worker_ids } = failures;
  return (
    <div className="panel-failure-info">
      <h3>{"Failure Count: " + failure_count}</h3>
      {
        failure_worker_ids.map((value, idx) => {
          return (<p key={idx}>{value}</p>);
        })
      }
    </div>
  );
}


function WaitingTasks() {
  const tasks = useSelector(state => state.status.tasks);
  let tasklist = tasks.tasks_waiting;
  if (!tasklist) tasklist = [];
  return (<div className="panel-waiting-tasks">
    <Button variant="contained" color="primary" onClick={() => window.myReduxStore.dispatch(aStatusGetTasks())}>Update Waiting Tasks</Button>
    <List>
      {
        tasklist.map((task, idx) => {
          let is_not_enough_memory = false;
          if ("_is_not_enough_memory" in task) {
            is_not_enough_memory = task["_is_not_enough_memory"];
          }
          return (<ListItem alignItems="flex-start" key={idx}>
            <ListItemText
              primary={"[Task] " + task.synthesizer + " | " + task.example_file + "." + task.example_size}
              secondary={<span style={{color: is_not_enough_memory ? "red": "gray"}}>{"Not_enough_memory: " + is_not_enough_memory}</span>}
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
          <h1>SYNTH Server Dashboard</h1>
          {/* Performance Overall, Running workers, pending tasks, task submit */}
          <PerfBar></PerfBar>
          <WorkerList></WorkerList>
          <FailureInfo></FailureInfo>
          <WaitingTasks></WaitingTasks>
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