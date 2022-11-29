import React from "react";
import styled from "styled-components";
import DevicePreview from "./DevicePreview";

export default function DevicesList(props) {
  if (props.devicesFilter === "all") {
    return (
      <List>
        {props.elements.map((item) => {
          return DevicePreview(props, item);
        })}
      </List>
    );
  } else if (props.devicesFilter === "sensors") {
    return (
      <List>
        {props.elements
          .filter(function checkDevicetype(device) {
            return device.type === "sensor";
          })
          .map((item) => {
            return DevicePreview(props, item);
          })}
      </List>
    );
  } else if (props.devicesFilter === "actuators") {
    return (
      <List>
        {props.elements
          .filter(function checkDevicetype(device) {
            return device.type === "actuator";
          })
          .map((item) => {
            return DevicePreview(props, item);
          })}
      </List>
    );
  }
}

function checkDevicetype(device, type) {
  return device.type === type;
}

const List = styled.ul`
  list-style: none;
  padding-left: 0%;
  padding-top: 5px;
`;
