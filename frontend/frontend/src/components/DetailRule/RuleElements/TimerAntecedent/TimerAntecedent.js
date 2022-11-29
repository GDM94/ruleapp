import React from 'react';
import styled from "styled-components";
import RuleElementTitle from '../RuleElementTitle';
import TimerAntecedentDetail from './TimerAntecedentDetail';

export default class TimerAntecedent extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            device_id: "",
            device_name: "",
            measure_time: "",
            measure_day: "",
            condition_time: "between",
            condition_day: "=",
            day_start_value: "",
            day_stop_value: "",
            time_start_value: "",
            time_stop_value: "",
            evaluation: "false",
            check_time: "true",
            check_date: "true",
            order: ""
        }
    }


    render() {
        return (
            <ContentContainer>
                <RuleElementTitle {...this.props}/>
                <TimerAntecedentDetail {...this.props}/>
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
