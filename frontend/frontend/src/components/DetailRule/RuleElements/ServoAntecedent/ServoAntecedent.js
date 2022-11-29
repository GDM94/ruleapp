import React from 'react';
import styled from "styled-components";
import RuleElementTitle from '../RuleElementTitle';
import ServoAntecedentDetail from './ServoAntecedentDetail';

export default class ServoAntecedent extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            device_id: "",
            device_name: "",
            condition: "between",
            last_time_on: "",
            last_time_off: "",
            last_date_on: "",
            last_date_off: "",
            date_start_value: "",
            date_stop_value: "",
            time_start_value: "",
            time_stop_value: "",
            evaluation: "false"
        }
    }


    render() {
        return (
            <ContentContainer>
                <RuleElementTitle {...this.props}/>
                <ServoAntecedentDetail {...this.props}/>
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
