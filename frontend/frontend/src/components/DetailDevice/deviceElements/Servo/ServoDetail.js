import React from 'react';
import { FormControlLabel, Switch } from '@material-ui/core';
import styled from "styled-components";
import RuleNameList from '../../DeviceUtils/RuleNameList'
import DeviceDescription from '../../DeviceUtils/DeviceDescription';

export default function ServoDetail(props) {
    return (
        <ElementContent>
            <DeviceDescription
                {...props}
            />
            <ElementMeasure>
                <h1>{props.element.measure}</h1>
                <ElementSettings>
                    <SettingOff {...props}/>
                    <SettingOn {...props}/>
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

function SettingOn(props) {
    var element = props.element;
    const manageSettingOn = (setting_on) => {
        element.setting_on = setting_on
        props.setDeviceAntecedentObject(element);
    }
    const modifySetting_on= () => {
        return (
            <input type="number" id="settingOn"
                defaultValue={props.element.setting_on}
                onChange={e => { manageSettingOn(e.target.value) }} />
        )
    }
    return (
        <ElementDescription>
            Position when ON:
            <ElementDescriptionDetail>{props.modify ? modifySetting_on() : props.element.setting_on}</ElementDescriptionDetail>
            {props.element.setting_unit_measure}
        </ElementDescription>
    )
}

function SettingOff(props) {
    var element = props.element;
    const manageSettingOff = (setting_off) => {
        element.setting_off = setting_off
        props.setDeviceAntecedentObject(element);
    }
    const modifySetting_off= () => {
        return (
            <input type="number" id="settingOff"
                defaultValue={props.element.setting_off}
                onChange={e => { manageSettingOff(e.target.value) }} />
        )
    }
    return (
        <ElementDescription>
            Position when OFF:
            <ElementDescriptionDetail>{props.modify ? modifySetting_off() : props.element.setting_off}</ElementDescriptionDetail>
            {props.element.setting_unit_measure}
        </ElementDescription>
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

const ElementDescription = styled.div`
display: flex;
flex-flow: row;
`;

const ElementDescriptionDetail = styled.div`
margin-left: 5px;
margin-right: 2px;
`;
