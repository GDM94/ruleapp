import React from 'react';
import styled from "styled-components";
import WaterLevelDetail from './WaterLevelDetail';
import DeletableDeviceTitle from '../../DeviceUtils/DeletableDeviceTitle';

export default class WaterLevel extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            id: "",
            name: "WATERLEVEL",
            measure: "-",
            absolute_measure: "",
            setting_error: "0",
            setting_max: "100",
            setting_unit_measure: "cm",
            rules: [],
            status: "disconnected",
            color: "red",
            unit_measure: "%",
            max_measure: "-",
            max_measure_time: "-",
            max_measure_date: "-",
            min_measure: "-",
            min_measure_time: "-",
            min_measure_date: "-",
            expiration: "10",
            description: "sensor - water level"
        }
    }

    render() {
        return (
            <ContentContainer>
                <DeletableDeviceTitle {...this.props} {...this.state}/>
                <WaterLevelDetail {...this.props} {...this.state}/>
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
