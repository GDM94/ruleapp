import React from 'react';
import styled from "styled-components";
import AntecedentSelectionManagement from './AntecedentSelectionManagement';
import SliderManagement from './Sliders';

export default function TempDescription(props) {
    var ruleElement = props.ruleElement;
    const onClickAntecedent = (e) => {
        ruleElement.check_temp = e.target.value === "true" ? "false" : "true"
        props.setRuleElementObject(ruleElement);
    }
    const onConditionChange = (e) => {
        var ruleElement = props.ruleElement;
        ruleElement.temp_condition = e.value
        props.setRuleElementObject(ruleElement);
    }
    const onSliderInput = (e, condition) => {
        var ruleElement = props.ruleElement;
        if (condition === "between") {
            ruleElement.temp_start_value = e.minValue.toString()
            ruleElement.temp_stop_value = e.maxValue.toString()
        }
        else if (condition === ">") {
            ruleElement.temp_start_value = e.minValue.toString()
            ruleElement.temp_stop_value = e.minValue.toString()
        }
        else if (condition === "<") {
            ruleElement.temp_start_value = e.maxValue.toString()
            ruleElement.temp_stop_value = e.maxValue.toString()
        }
        props.setRuleElementObject(ruleElement);
    }
    return (
        <ElementMeasure>
            <h5>
                <AntecedentSelectionManagement 
                {...props} 
                check={props.ruleElement.check_temp} 
                onClickAntecedent={onClickAntecedent}
                />
                TEMPERATURE: {props.ruleElement.temp_value} {props.ruleElement.temp_unit}</h5>
            <SliderManagement
                {...props}
                condition={props.ruleElement.temp_condition}
                onChange={onConditionChange}
                check={props.ruleElement.check_temp}
                min_value={"-10"}
                max_value={"40"}
                start_value={props.ruleElement.temp_start_value}
                stop_value={props.ruleElement.temp_stop_value}
                value_unit={props.ruleElement.temp_unit}
                onInput={onSliderInput}
            />
        </ElementMeasure>
    )
}


const ElementMeasure = styled.div`
border: solid black 2px;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
padding: 3%;
background-color: #a7b4a8;
text-align: left;
justify-content: left;
background-color: #e6e6e6;
`;
