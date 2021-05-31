let {
  CircularProgress,
  DialogTitle,
  Dialog,
  DialogActions,
  DialogContent
} = MaterialUI;

class ScreenLoading extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      isLoading: false,
      statusText: "Loading ..."
    }
  }
  componentDidMount() {
    window._show_loading = (statusText) => {
      this.setState({ "isLoading": true, "statusText": statusText });
    }
    window._hide_loading = () => {
      this.setState({ "isLoading": false, statusText: "" })
    }
  }
  render() {
    let { isLoading, statusText } = this.state;
    if (isLoading) {
      return (
        <div className="screen-cover">
          <div className="screen-cover-center">
            <CircularProgress />
            <h3>{statusText}</h3>
          </div>
        </div>);
    } else {
      return (null);
    }
  }
}


class DisplayDialog extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      "isOpen": false,
      "title": "<TITLE>",
      "messag_data": "<MESSAGE>"
    };
    this.rendering_func = (data) => {
      return (<span>{data}</span>);
    }
  }
  handleOnClose = () => {
    this.setState({ "isOpen": false });
  }
  componentDidMount() {
    window._open_display_dialog = (title, message_data, rendering_func) => {
      this.setState({
        "isOpen": true,
        "title": title,
        "message_data": message_data,
      });
      if (rendering_func != null) {
        this.rendering_func = rendering_func;
      }
    }
  }
  render() {
    let { isOpen, title, message_data } = this.state;
    return (
      <Dialog
        aria-labelledby="display-dialog-title"
        open={isOpen}
        onClose={() => this.handleOnClose(null)}>
        <DialogTitle id="display-dialog-title">
          {title}
        </DialogTitle>
        <DialogContent>
          {this.rendering_func(message_data)}
        </DialogContent>
        <DialogActions>
          <Button onClick={() => this.handleOnClose()} color="primary" autoFocus>
            Close
            </Button>
        </DialogActions>
      </Dialog>
    );
  }
}