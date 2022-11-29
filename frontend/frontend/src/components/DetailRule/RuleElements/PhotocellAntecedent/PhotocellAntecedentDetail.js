import React from 'react';
import styled from "styled-components";
import Select from 'react-select';
import BetweenRangeSlider from '../../../Sliders/BetweenRangeSlider';
import GreatSingleRangeSlider from '../../../Sliders/GreatSingleRangeSlider';
import LowerRangeSlider from '../../../Sliders/LowerRangeSlider';

export default function PhotocellAntecedentDetail(props){
    const slider = (condition) => {
        switch (condition) {
            case "between":
                return <BeetweenRangeSliderSetting {...props} />
            case ">":
                return <GreaterRangeSliderSetting {...props} />
            case "<":
                return <LowerRangeSliderSetting {...props} />
            default:
                return <BeetweenRangeSliderSetting {...props} />
        }
    }

    return (
        <ElementContent>
            <DeviceElementMeasure>
                <h1>{props.ruleElement.measure} %</h1>
            </DeviceElementMeasure>
            <br></br>
            <EvaluateConditionSetting {...props} />
            <br></br>
            {slider(props.ruleElement.condition_measure)}
        </ElementContent>
    )
}

function EvaluateConditionSetting(props) {
    var ruleElement = props.ruleElement;
    return (
        <ElementMeasure>
            <h5>- CONDITION SETTING</h5>
            <Select
                menuPortalTarget={document.body}
                styles={{ menuPortal: base => ({ ...base, zIndex: 9999 }) }}
                options={props.options}
                defaultValue={{ label: ruleElement.condition_measure, value: ruleElement.condition_measure }}
                onChange={e => {
                    ruleElement.condition_measure = e.value
                    props.setRuleElementObject(ruleElement);
                }}
            />
        </ElementMeasure>
    )
}

function LowerRangeSliderSetting(props) {
    var ruleElement = props.ruleElement;
    var start_value = ruleElement.start_value === "//" ? "0" : ruleElement.start_value
    return (
        <ElementMeasure>
            <h5>- VALUE SETTING</h5>
            <p>Start Value: {start_value} %</p>
            <LowerRangeSlider
                min={0}
                max={100}
                step={1}
                minValue={"0"}
                maxValue={start_value}
                ruler={false}
                label={true}
                onInput={(e) => {
                    ruleElement.start_value = e.maxValue.toString()
                    ruleElement.stop_value = e.maxValue.toString()
                    props.setRuleElementObject(ruleElement);
                }}

            />
        </ElementMeasure>
    )
}

function GreaterRangeSliderSetting(props) {
    var ruleElement = props.ruleElement;
    var start_value = ruleElement.start_value === "//" ? "0" : ruleElement.start_value
    return (
        <ElementMeasure>
            <h5>- VALUE SETTING</h5>
            <p>Start Value: {start_value} %</p>
            <GreatSingleRangeSlider
                min={0}
                max={100}
                step={1}
                minValue={start_value}
                maxValue={"100"}
                ruler={false}
                label={true}
                onInput={(e) => {
                    ruleElement.start_value = e.minValue.toString()
                    ruleElement.stop_value = e.minValue.toString()
                    props.setRuleElementObject(ruleElement);
                }}

            />
        </ElementMeasure>
    )
}

function BeetweenRangeSliderSetting(props) {
    var ruleElement = props.ruleElement;
    var start_value = ruleElement.start_value === "//" ? "0" : ruleElement.start_value
    var stop_value = ruleElement.start_value === "//" ? "100" : ruleElement.stop_value
    return (
        <ElementMeasure>
            <h5>- VALUE SETTING</h5>
            <p>Start Value: {start_value} %</p>
            <p>Stop Value: {stop_value} %</p>
            <BetweenRangeSlider
                min={0}
                max={100}
                step={1}
                ruler={false}
                label={true}
                preventWheel={false}
                minValue={ruleElement.start_value === "//" ? "0" : ruleElement.start_value}
                maxValue={ruleElement.stop_value === "//" ? "100" : ruleElement.stop_value}
                onInput={(e) => {
                    ruleElement.start_value = e.minValue.toString()
                    ruleElement.stop_value = e.maxValue.toString()
                    props.setRuleElementObject(ruleElement);
                }}
            />
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