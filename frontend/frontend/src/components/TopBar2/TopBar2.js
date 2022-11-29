import React from "react";
import styled from "styled-components";
import ButtonGroup from "@material-ui/core/ButtonGroup";
import { withRouter } from "react-router-dom";
import TopBar2Button from "./TobBar2Button";
import TobBar2ButtonWithRequest from "./TobBar2ButtonWithRequest";

function TopBar2(props) {
  return (
    <TopBar2Div>
      <ButtonGroup
        variant="text"
        color="default"
        aria-label="text primary button group"
      >
        <TopBar2Button
          {...props}
          path={process.env.REACT_APP_LOCATION_URL}
          page={process.env.REACT_APP_PAGE_LOCATION}
        />
        <TobBar2ButtonWithRequest
          {...props}
          path={process.env.REACT_APP_DEVICES_URL}
          page={process.env.REACT_APP_PAGE_DEVICES}
        />
        <TobBar2ButtonWithRequest
          {...props}
          path={process.env.REACT_APP_RULES_URL}
          page={process.env.REACT_APP_PAGE_RULES}
        />
      </ButtonGroup>
    </TopBar2Div>
  );
}

const TopBar2Div = styled.div`
  background-color: #333333;
  width: 100%;
  text-align: center;
  display: flex;
  flex-flow: row;
  justify-content: center;
`;

export default withRouter(TopBar2);
