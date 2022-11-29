import React from "react";
import styled from "styled-components";
import Button from "@material-ui/core/Button";
import ButtonGroup from "@material-ui/core/ButtonGroup";

export default function FilterDeviceList(props) {
  return (
    <FilterComponent>
      <ButtonGroup>
        <Button
          onClick={() => {
            props.handleFilterDevicesList("all");
          }}
          variant={props.devicesFilter === "all" ? "contained" : "outlined"}
          style={{backgroundColor: props.devicesFilter === "all" ? " #cccccc" : ""}}
        >
          all
        </Button>
        <Button
          onClick={() => {
            props.handleFilterDevicesList("sensors");
          }}
          variant={props.devicesFilter === "sensors" ? "contained" : "outlined"}
          style={{backgroundColor: props.devicesFilter === "sensors" ? " #cccccc" : ""}}
        >
          sensors
        </Button>
        <Button
          onClick={() => {
            props.handleFilterDevicesList("actuators");
          }}
          variant={
            props.devicesFilter === "actuators" ? "contained" : "outlined"
          }
          style={{backgroundColor: props.devicesFilter === "actuators" ? " #cccccc" : ""}}
        >
          actuators
        </Button>
      </ButtonGroup>
    </FilterComponent>
  );
}

const FilterComponent = styled.div`
  width: 100%;
  text-align: center;
  display: flex;
  flex-flow: row;
  justify-content: center;
`;
