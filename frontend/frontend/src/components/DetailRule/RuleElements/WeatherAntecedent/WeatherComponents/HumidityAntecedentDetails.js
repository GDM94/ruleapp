import React from 'react';
import styled from "styled-components";
import AntecedentSelectionManagement from './AntecedentSelectionManagement';
import SliderManagement from './Sliders';

export default function HumidityDescription(props) {
    var ruleElement = props.ruleElement;
    const onClickAntecedent = (e) => {
        ruleElement.check_humidity = e.target.value === "true" ? "false" : "true"
        props.setRuleElementObject(ruleElement);
    }
    const onConditionChange = (e) => {
        var ruleElement = props.ruleElement;
        ruleElement.humidity_condition = e.value
        props.setRuleElementObject(ruleElement);
    }
    const onSliderInput = (e, condition) => {
        var ruleElement = props.ruleElement;
        if (condition === "between") {
            ruleElement.humidity_start_value = e.minValue.toString()
            ruleElement.humidity_stop_value = e.maxValue.toString()
        }
        else if (condition === ">") {
            ruleElement.humidity_start_value = e.minValue.toString()
            ruleElement.humidity_stop_value = e.minValue.toString()
        }
        else if (condition === "<") {
            ruleElement.humidity_start_value = e.maxValue.toString()
            ruleElement.humidity_stop_value = e.maxValue.toString()
        }
        props.setRuleElementObject(ruleElement);
    }
    return (
        <ElementMeasure>
            <h5>
                <AntecedentSelectionManagement
                    {...props}
                    check={props.ruleElement.check_humidity}
                    onClickAntecedent={onClickAntecedent}
                />
                HUMIDITY: {props.ruleElement.humidity_value} {props.ruleElement.humidity_unit}
            </h5>
            <SliderManagement
                {...props}
                condition={props.ruleElement.humidity_condition}
                onChange={onConditionChange}
                check={props.ruleElement.check_humidity}
                min_value={"0"}
                max_value={"100"}
                start_value={props.ruleElement.humidity_start_value}
                stop_value={props.ruleElement.humidity_stop_value}
                value_unit={props.ruleElement.humidity_unit}
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
