import React from 'react';
import styled from "styled-components";

export default function TimerAntecedentDetail(props) {

    return (
        <ElementContent>
            <DeviceElementMeasure>
                <h1>{props.ruleElement.measure}</h1>
            </DeviceElementMeasure>
            <br></br>
            <WeekDaySetting {...props} />
            <br></br>
            <TimeSetting {...props} />
        </ElementContent>
    )

}


function TimeSetting(props) {
    var ruleElement = props.ruleElement;
    const manageStartTime = (startTime) => {
        ruleElement.time_start_value = startTime
        props.setRuleElementObject(ruleElement);
    }
    const manageStopTime = (stopTime) => {
        ruleElement.time_stop_value = stopTime
        props.setRuleElementObject(ruleElement);
    }
    const defaultStartTimeValue = () => {
        if (ruleElement.time_start_value !== "") {
            return ruleElement.time_start_value
        }
        else {
            return "00:00:00"
        }
    }
    const defaultStopTimeValue = () => {
        if (ruleElement.time_stop_value !== "") {
            return ruleElement.time_stop_value
        }
        else {
            return "00:00:00"
        }
    }
    return (

        <ElementMeasure>
            <h5>
                <input type="radio"
                    checked={ruleElement.check_time === "true"}
                    value={ruleElement.check_time === "true" ? "true" : "false"}
                    onClick={e => {
                        ruleElement.check_time = e.target.value === "true" ? "false" : "true"
                        props.setRuleElementObject(ruleElement);
                    }}
                    onChange={e => {
                        console.log(e.target.value)
                    }}
                />
                TIMER SETTING
            </h5>
            <div style={{ display: ruleElement.check_time === "true" ? "" : "none" }}>
                <div>
                    Start Time
                    <input type="time" id="start"
                        defaultValue={defaultStartTimeValue()}
                        onChange={e => { manageStartTime(e.target.value) }} />
                </div>
                <br />
                <div>
                    Stop Time
                    <input type="time" id="stop"
                        defaultValue={defaultStopTimeValue()}
                        onChange={e => { manageStopTime(e.target.value) }} />
                </div>
            </div>
        </ElementMeasure>
    )

}

function WeekDaySetting(props) {
    var ruleElement = props.ruleElement;

    const manageWeekDays = (day, check) => {
        if (check) {
            ruleElement.day_start_value.push(day)
        }
        else {
            const idx = ruleElement.day_start_value.indexOf(day)
            ruleElement.day_start_value.splice(idx, 1)
        }
        props.setRuleElementObject(ruleElement);
    }

    const checkedWeekDays = (day) => {
        if (ruleElement.day_start_value && ruleElement.day_start_value.length > 0) {
            return ruleElement.day_start_value.some(d => d === day)
        }
        else {
            return false
        }
    }

    const daySelection = (dayName, dayNumberInt) => {
        const dayNumber = dayNumberInt.toString();
        return (
            <DaySelection>
                <input style={{ marginRight: "5px" }} type='checkbox'
                    name={dayNumber} id={dayNumber} value={dayNumber}
                    checked={checkedWeekDays(dayNumber)}
                    onChange={e => { manageWeekDays(dayNumber, e.target.checked) }}
                />
                {dayName}
            </DaySelection>
        )
    }
    return (
        <ElementMeasure>
            <h5>
                <input type="radio"
                    checked={ruleElement.check_date === "true"}
                    value={ruleElement.check_date === "true" ? "true" : "false"}
                    onClick={e => {
                        ruleElement.check_date = e.target.value === "true" ? "false" : "true"
                        props.setRuleElementObject(ruleElement);
                    }}
                    onChange={e => {
                        console.log(e.target.value)
                    }}
                />
                DAYS SETTING
            </h5>
            <div style={{ display: ruleElement.check_date === "true" ? "flex" : "none", flexFlow: "column" }}>
                {daySelection("Lunedì", 0)}
                {daySelection("Martedì", 1)}
                {daySelection("Mercoledì", 2)}
                {daySelection("Giovedì", 3)}
                {daySelection("Venerdì", 4)}
                {daySelection("Sabato", 5)}
                {daySelection("Domenica", 6)}
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

const DaySelection = styled.label`
width: fit-content; 
cursor: default;
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





