import React from 'react';
import styled from "styled-components";
import ButtonDetail from './ButtonDetail';
import DeletableDeviceTitle from '../../DeviceUtils/DeletableDeviceTitle';

export default class Button extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            id: "",
            name: "BUTTON",
            measure: "-",
            rules: [],
            last_date_on: "-",
            last_date_off: "-",
            last_time_on: "-",
            last_time_off: "-",
            status: "disconnected",
            color: "red",
            expiration: "10",
            description: "sensor - button"
        }
    }

    render() {
        return (
            <ContentContainer>
                <DeletableDeviceTitle {...this.props} {...this.state}/>
                <ButtonDetail {...this.props} {...this.state}/>
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
