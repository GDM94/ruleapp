import React from 'react';
import styled from "styled-components";
import UndeletableDeviceTitle from '../../DeviceUtils/UndeletableDeviceTitle';
import WeatherDetail from './WeatherDetail';

export default class Weather extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            description: "sensor - weather"
        }
    }

    render() {
        return (
            <ContentContainer>
                <UndeletableDeviceTitle {...this.props} {...this.state} />
                <WeatherDetail {...this.props} {...this.state} />
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
