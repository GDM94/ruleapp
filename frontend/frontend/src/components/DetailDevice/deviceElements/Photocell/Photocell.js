import React from 'react';
import styled from "styled-components";
import DeletableDeviceTitle from '../../DeviceUtils/DeletableDeviceTitle';
import PhotocellDetail from './PhotocellDetail';


export default class Photocell extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            id: "",
            name: "PHOTOCELL",
            measure: "-",
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
            description: "sensor - photocell"
        }
    }

    render() {
        return(
            <ContentContainer>
                <DeletableDeviceTitle {...this.props} {...this.state}/>
                <PhotocellDetail {...this.props} {...this.state}/>
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
