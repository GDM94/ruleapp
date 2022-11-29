import React from 'react';
import styled from "styled-components";
import RuleNameList from '../../DeviceUtils/RuleNameList'
import DeviceDescription from '../../DeviceUtils/DeviceDescription';

export default function ButtonDetail(props) {
    return (
        <ElementContent>
            <DeviceDescription
                {...props}
            />
            <ElementMeasure>
                <h1>{props.element.measure}</h1>
                <p>last on ({props.element.last_date_on} - {props.element.last_time_on})</p>
                <p>last off ({props.element.last_date_off} - {props.element.last_time_off})</p>
            </ElementMeasure>
            <br></br>
            <RuleNameList {...props} />
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
text-align: center;
justify-content: center;
background-color: #e6e6e6;
`;
