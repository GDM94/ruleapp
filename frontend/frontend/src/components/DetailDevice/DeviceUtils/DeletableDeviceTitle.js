import React from "react";
import Button from "@material-ui/core/Button";
import ButtonGroup from "@material-ui/core/ButtonGroup";
import DeleteIcon from "@material-ui/icons/Delete";
import RefreshIcon from "@material-ui/icons/Refresh";
import EditIcon from "@material-ui/icons/Edit";
import DoneIcon from "@material-ui/icons/Done";
import styled from "styled-components";
import CloseIcon from "@material-ui/icons/Close";
import FiberManualRecordIcon from "@material-ui/icons/FiberManualRecord";

export default function DeletableDeviceTitle(props) {
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
          style={{ display: props.modifyDevice ? "" : "none" }}
          onClick={() => {
            props.handleModifyDevice();
            props.deleteElementRequest(props.elementId);
          }}
        >
          <DeleteIcon fontSize="large" style={{ color: "red" }} />
        </Button>
        <Button
          onClick={() => {
            props.getElementById(props.elementId);
          }}
        >
          <RefreshIcon fontSize="large" style={{ color: "black" }} />
        </Button>
        <Button
          onClick={() => {
            if (props.modifyDevice) {
              props.updateDeviceRequest(props.elementId);
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
