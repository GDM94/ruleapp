import React from 'react';
import { FormControlLabel, Switch } from '@material-ui/core';
import styled from "styled-components";
import RuleNameList from '../../DeviceUtils/RuleNameList'
import DeviceDescription from '../../DeviceUtils/DeviceDescription';

export default function SwitchDetail(props) {
    return (
        <ElementContent>
            <DeviceDescription
                {...props}
            />
            <ElementMeasure>
                <h1>{props.element.measure}</h1>
                <p>last on ({props.element.last_date_on} - {props.element.last_time_on})</p>
                <p>last off ({props.element.last_date_off} - {props.element.last_time_off})</p>
                <ElementSettings>
                    <SetAutomaticButton {...props} />
                    <SetManualMeasureButton {...props} />
                </ElementSettings>
            </ElementMeasure>
            <br></br>
            <RuleNameList {...props} />
        </ElementContent>
    )

}


function SetAutomaticButton(props) {
    var automatic = false;
    if (props.element.automatic === "true") {
        automatic = true;
    }
    return (
        <FormControlLabel
            control={
                <Switch
                    checked={automatic}
                    onChange={(e) => { props.setConsequentAutomaticRequest(e.target.checked) }}
                    value={automatic}
                    color="primary"
                />
            }
            label="automatic"
        />
    )
}


function SetManualMeasureButton(props) {
    return (
        <FormControlLabel
            control={
                <Switch
                    checked={props.element.manual_measure === "on"}
                    onChange={(e) => { 
                        var newValue = "on";
                        if (e.target.value === "on"){
                            newValue = "off";
                        }
                        props.setConsequentManualMeasureRequest(newValue) 
                    }}
                    value={props.element.manual_measure}
                    color="primary"
                    disabled={props.element.automatic === "true"}
                    defaultValue={props.element.measure}
                />
            }
            label="on"
        />
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

const ElementSettings = styled.div`
margin-left: 2%;
margin-right: 2%;
justify-content: center;
padding: 1%;
display: flex;
flex-flow: column;
align-items: center;
`;
