import React from 'react';
import styled from "styled-components";

export default function ButtonAntecedentDetail(props) {
    return (
        <ElementContent>
            <DeviceElementMeasure>
                <h1>{props.ruleElement.measure}</h1>
            </DeviceElementMeasure>
            <br></br>
            <ElementMeasure>
                IF PRESSED: {props.ruleElement.start_value}
                <br></br>
                IF NOT PRESSED: {props.ruleElement.stop_value}
            </ElementMeasure>
        </ElementContent>
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


