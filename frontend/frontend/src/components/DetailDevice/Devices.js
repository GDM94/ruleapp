import React from "react";
import { withRouter } from "react-router";
import styled from "styled-components";
import RegisterDeviceProcess from "./RegisterDeviceProcess";
import Switch from "./deviceElements/Switch/Switch";
import Timer from "./deviceElements/Timer/Timer";
import Alert from "./deviceElements/Alert/Alert";
import Button from "./deviceElements/Button/Button";
import WaterLevel from "./deviceElements/WaterLevel/WaterLevel";
import Weather from "./deviceElements/Weather/Weather";
import Photocell from "./deviceElements/Photocell/Photocell";
import Servo from "./deviceElements/Servo/Servo";
import DevicesList from "./DeviceUtils/DevicesList";
import FilterDeviceList from "./DeviceUtils/FilterDeviceList";

function Devices(props) {
  if (
    props.elementId &&
    props.elementId !== "" &&
    props.elements.length > 0 &&
    props.addNewElement === false
  ) {
    if (props.elementId.includes("SWITCH")) {
      return <Switch {...props} />;
    } else if (props.elementId.includes("alert")) {
      return <Alert {...props} />;
    } else if (props.elementId.includes("timer")) {
      return <Timer {...props} />;
    } else if (props.elementId.includes("WATERLEVEL")) {
      return <WaterLevel {...props} />;
    } else if (props.elementId.includes("BUTTON")) {
      return <Button {...props} />;
    } else if (props.elementId.includes("WEATHER")) {
      return <Weather {...props} />;
    } else if (props.elementId.includes("PHOTOCELL")) {
      return <Photocell {...props} />;
    } else if (props.elementId.includes("SERVO")) {
      return <Servo {...props} />;
    }
  } else if (props.elementId === "" && props.addNewElement === true) {
    return (
      <ContentContainer>
        <RegisterDeviceProcess {...props} />
      </ContentContainer>
    );
  } else {
    return (
      <ContentContainer>
        <AddDeviceElement
          onClick={() => {
            props.handleRegisterDevicePopUp();
            props.setNewElement("");
            props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ANTECEDENTS);
          }}
        >
          <h5>REGISTER NEW DEVICE</h5>
        </AddDeviceElement>
        <FilterDeviceList {...props} />
        <DevicesList {...props} />
      </ContentContainer>
    );
  }
}

export default withRouter(Devices);

const ContentContainer = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-flow: column;
  float: left;
  text-align: center;
`;

const AddDeviceElement = styled.div`
  color: balck;
  background-color: #cccccc;
  border-radius: 25px;
  margin: 10%;
  margin-top: 2%;
  margin-bottom: 2%;
  padding: 1%;
  &:hover {
    background: #d5d8d8;
  }
`;
