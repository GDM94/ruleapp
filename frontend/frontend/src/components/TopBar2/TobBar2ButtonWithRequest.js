import React from "react";
import Button from "@material-ui/core/Button";
import { withRouter } from "react-router-dom";

function TopBar2ButtonWithRequests(props) {
  return (
    <Button
      style={{
        color: props.routeUrl === props.path ? "#eead4c" : "#bfbfbf",
        fontWeight: props.routeUrl === props.path ? "bold" : "",
        borderBottom: props.routeUrl === props.path ? "#eead4c solid 5px" : "",
      }}
      onClick={() => {
        props.location.state.page = props.page;
        props.location.state.path = props.path;
        props.setRouteUrlWithRequests(props.path);
        props.history.push({
          pathname: props.path,
          state: props.location.state,
        });
      }}
    >
      {props.page}
    </Button>
  );
}

export default withRouter(TopBar2ButtonWithRequests);
