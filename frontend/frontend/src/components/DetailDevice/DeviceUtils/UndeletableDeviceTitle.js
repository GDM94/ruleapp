import React from "react";
import Button from "@material-ui/core/Button";
import RefreshIcon from "@material-ui/icons/Refresh";
import EditIcon from "@material-ui/icons/Edit";
import DoneIcon from "@material-ui/icons/Done";
import ButtonGroup from "@material-ui/core/ButtonGroup";
import styled from "styled-components";
import FiberManualRecordIcon from "@material-ui/icons/FiberManualRecord";
import CloseIcon from "@material-ui/icons/Close";

export default function UndeletableDeviceTitle(props) {
  return (
    <ElementTitle>
      <h2>
        {" "}
        <FiberManualRecordIcon style={{ color: props.element.color }} />{" "}
        {props.element.name}{" "}
      </h2>
      <ButtonGroup
        variant="text"
        color="primary"
        aria-label="text primary button group"
      >
        <Button
          onClick={() => {
            props.getElementById(props.element.id);
          }}
        >
          <RefreshIcon fontSize="large" style={{ color: "black" }} />
        </Button>
        <Button
          onClick={() => {
            if (props.modifyDevice) {
              props.updateDeviceRequest(props.element.id);
              props.handleModifyDevice();
            } else {
              props.handleModifyDevice();
            }
          }}
        >
          {props.modifyDevice ? (
            <DoneIcon fontSize="large" style={{ color: "black" }} />
          ) : (
            <EditIcon fontSize="large" style={{ color: "black" }} />
          )}
        </Button>
        <Button
          onClick={() => {
            props.setNewElement("");
          }}
        >
          <CloseIcon fontSize="large" style={{ color: "black" }} />
        </Button>
      </ButtonGroup>
      
    </ElementTitle>
  );
}

const ElementTitle = styled.div`
  text-align: left;
  margin-left: 2%;
  margin-right: 2%;
  margin-top: 2%;
  display: flex;
  flex-flow: row;
`;

