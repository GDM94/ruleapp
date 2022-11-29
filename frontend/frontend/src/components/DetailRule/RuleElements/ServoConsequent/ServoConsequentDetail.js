import React from 'react';
import styled from "styled-components";

export default function ServoConsequentDetail(props) {
    return (
        <ElementContent>
            <DeviceElementMeasure>
                <h1>{props.ruleElement.measure}</h1>
            </DeviceElementMeasure>
            <br></br>
            <DelaySetting {...props} />
        </ElementContent>
    )
}

function DelaySetting(props) {
    var ruleElement = props.ruleElement;
    if (ruleElement.delay) {
        return (
            <ElementMeasure>
                <h5>- DELAY</h5>
                <div>
                    <input type="number" id="delay" name="delay" min="0"
                        defaultValue={parseInt(props.ruleElement.delay)}
                        onChange={(e) => {
                            ruleElement.delay = e.target.value.toString();
                            props.setRuleElementObject(ruleElement);
                        }} />
                    (Seconds)
                </div>
            </ElementMeasure>
        )
    }
    else {
        return (<></>)
    }

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

