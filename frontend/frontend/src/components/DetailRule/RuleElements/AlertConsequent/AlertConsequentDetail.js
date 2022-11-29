import React from 'react';
import styled from "styled-components";

export default function AlertConsequentDetail(props) {
    return (
        <ElementContent>
            <MessageSetting {...props} />
        </ElementContent>
    )
}

function MessageSetting(props) {
    var ruleElement = props.ruleElement;
    return (
        <ElementMeasure>
            <h5>- MESSAGE</h5>
            <div>
                <input type="text" id="name" name="name"
                    defaultValue={props.ruleElement.message}
                    onChange={(e) => {
                        ruleElement.message = e.target.value;
                        props.setRuleElementObject(ruleElement);
                    }} />
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


