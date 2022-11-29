import React from 'react';
import styled from "styled-components";

export default function ServoAntecedentDetail(props) {
    return (
        <ElementContent>
            <DeviceElementMeasure>
                <h1>{props.ruleElement.measure}</h1>
                <p>last on ({props.ruleElement.last_date_on} - {props.ruleElement.last_time_on})</p>
                <p>last off ({props.ruleElement.last_date_off} - {props.ruleElement.last_time_off})</p>
            </DeviceElementMeasure>
            <br></br>
            <TurnOnSetting {...props} />
            <br></br>
            <TurnOffSetting {...props} />
        </ElementContent>
    )

}

function TurnOnSetting(props) {
    var ruleElement = props.ruleElement;
    const manageStartTime = (startTime) => {
        ruleElement.time_start_value = startTime
        props.setRuleElementObject(ruleElement);
    }
    const manageStartDate = (date) => {
        ruleElement.date_start_value = date.toString()
        props.setRuleElementObject(ruleElement);
    }
    const defaultStartTimeValue = () => {
        if (ruleElement.time_start_value !== "-") {
            return ruleElement.time_start_value
        }
        else {
            return "00:00:00"
        }
    }
    const defaultDateValue = () => {
        var dateValue = 0
        if (ruleElement.date_start_value) {
            dateValue = parseInt(ruleElement.date_start_value)
        }
        return dateValue
    }
    return (
        <ElementMeasure>
            <h5>- WAITING TO TURN ON</h5>
            <div>
                time:
                <input type="time" id="start_time"
                    defaultValue={defaultStartTimeValue()}
                    onChange={e => { manageStartTime(e.target.value) }} />
            </div>
            <br />
            <div>
                days:
                <input type="number" id="start_date"
                    defaultValue={defaultDateValue()}
                    onChange={e => { manageStartDate(e.target.value) }} />
            </div>
        </ElementMeasure>
    )

}



function TurnOffSetting(props) {
    var ruleElement = props.ruleElement;
    const manageStopTime = (time) => {
        ruleElement.time_stop_value = time
        props.setRuleElementObject(ruleElement);
    }
    const manageStopDate = (date) => {
        ruleElement.date_stop_value = date.toString()
        props.setRuleElementObject(ruleElement);
    }
    const defaultStopTimeValue = () => {
        if (ruleElement.time_stop_value !== "-") {
            return ruleElement.time_stop_value
        }
        else {
            return "00:00:00"
        }
    }
    const defaultDateValue = () => {
        var dateValue = 0
        if (ruleElement.date_stop_value) {
            dateValue = parseInt(ruleElement.date_stop_value)
        }
        return dateValue
    }
    return (
        <ElementMeasure>
            <h5>- WAITING TO TURN OFF</h5>
            <div>
                time:
                <input type="time" id="stop_time"
                    defaultValue={defaultStopTimeValue()}
                    onChange={e => { manageStopTime(e.target.value) }} />
            </div>
            <br />
            <div>
                days:
                <input type="number" id="stop_date"
                    defaultValue={defaultDateValue()}
                    onChange={e => { manageStopDate(e.target.value) }} />
            </div>
        </ElementMeasure>
    )

}



const ElementContent = styled.div`
border: solid #d9d9d9 1px;
height: 100%;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
margin-bottom: 2%;
text-align: left;
padding: 2%;
background-color: #cccccc;
`;

const ElementMeasure = styled.div`
display: flex;
flex-flow: column;
border: solid black 2px;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
padding: 2%;
background-color: #a7b4a8;
text-align: left;
justify-content: left;
background-color: #e6e6e6;
`;

const DeviceElementMeasure = styled.div`
border: solid black 2px;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
padding: 2%;
background-color: #a7b4a8;
text-align: center;
justify-content: center;
background-color: #e6e6e6;
`;








