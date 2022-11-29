import React from 'react';
import styled from "styled-components";
import ServoDetail from './ServoDetail';
import DeletableDeviceTitle from '../../DeviceUtils/DeletableDeviceTitle';

export default class Servo extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            device_id: "",
            name: "servo",
            measure: "-",
            rules: [],
            automatic: "true",
            manual_measure: "false",
            setting_on: "0",
            setting_off: "90",
            last_date_on: "-",
            last_date_off: "-",
            last_time_on: "-",
            last_time_off: "-",
            status: "disconnected",
            color: "red",
            expiration: "10",
            description: "device - servo"
        }
    }

    render() {
        return (
            <ContentContainer>
                <DeletableDeviceTitle {...this.props} {...this.state}/>
                <ServoDetail {...this.props} {...this.state}/>
            </ContentContainer>
        )
    }
}

const ContentContainer = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-flow: column;
  float:left;
  text-align: center;
  max-height:100%;
  overflow-y: auto;
`;
