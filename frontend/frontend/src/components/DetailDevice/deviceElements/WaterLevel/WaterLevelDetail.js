import React from 'react';
import styled from "styled-components";
import RuleNameList from '../../DeviceUtils/RuleNameList'
import DeviceDescription from '../../DeviceUtils/DeviceDescription';

export default function WaterLevelDetail(props) {
    return (
        <ElementContent>
            <DeviceDescription
                {...props}
            />
            <ElementMeasure>
                <h1>{props.element.measure} %</h1>
                <p>max measure: {props.element.max_measure} {props.element.unit_measure}  ({props.element.max_measure_date} - {props.element.max_measure_time})</p>
                <p>min measure: {props.element.min_measure} {props.element.unit_measure} ({props.element.min_measure_date} - {props.element.min_measure_time})</p>
                <ElementSettings>
                    <SetMaxMeasure {...props} />
                    <SetErrorMeasure {...props} />
                </ElementSettings>
            </ElementMeasure>
            <br></br>
            <RuleNameList {...props} />
        </ElementContent>
    )

}


function SetMaxMeasure(props) {
    var element = props.element;
    const manageMaxMeasure = (maxMeasureSetting) => {
        element.setting_max = maxMeasureSetting
        props.setDeviceAntecedentObject(element);
    }
    const modifymaxMeasureForm = () => {
        return (
            <input type="number" id="maxMeasure"
                defaultValue={props.element.setting_max}
                onChange={e => { manageMaxMeasure(e.target.value) }} />
        )
    }
    return (
        <ElementDescription>
            max measure setting:
            <ElementDescriptionDetail>{props.modify ? modifymaxMeasureForm() : props.element.setting_max}</ElementDescriptionDetail>
            {props.element.setting_unit_measure}
        </ElementDescription>
    )
}

function SetErrorMeasure(props) {
    var element = props.element;
    const manageErrorMeasure = (errorMeasureSetting) => {
        element.setting_error = errorMeasureSetting
        props.setDeviceAntecedentObject(element);
    }
    const modifyErrorMeasureForm = () => {
        return (
            <input type="number" id="errorMeasure"
                defaultValue={props.element.setting_error}
                onChange={e => { manageErrorMeasure(e.target.value) }} />
        )
    }
    return (
        <ElementDescription>
            error measure setting:
            <ElementDescriptionDetail>{props.modify ? modifyErrorMeasureForm() : props.element.setting_error}</ElementDescriptionDetail>
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
