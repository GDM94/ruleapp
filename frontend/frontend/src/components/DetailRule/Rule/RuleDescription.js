import React, { useState } from 'react';
import styled from "styled-components";

export default function RuleDescription(props) {
    const [checkRuleName, handleCheckRuleName] = useState(false);
    const rule = props.element;

    const checkRuleNameFunction = (ruleName) => {
        const rules = props.elements;
        var checkRuleName = false;
        if (rules.some(rule => rule.name === ruleName)) {
            checkRuleName = true;
        }
        handleCheckRuleName(checkRuleName)
    }
    return (
        <ElementDescription>
            <ul>
                <li style={{ color: "red", display: checkRuleName ? 'block' : 'none' }}> Error: rule name already exist! Choose another name.</li>
                <li>Name: {props.modify ?
                    <ModifyNameForm
                        {...props}
                        checkRuleNameFunction={checkRuleNameFunction}
                        checkRuleName={checkRuleName}
                    />
                    : props.element.name}
                </li>
                <li key={"last on"}>Last on: {rule.last_date_on} {rule.last_time_on}</li>
                <li key={"last_off"}>Last off: {rule.last_date_off} {rule.last_time_off}</li>
            </ul>
        </ElementDescription>
    )

}

function ModifyNameForm(props) {
    const submitFunction = (event) => {
        props.setRuleRequest(props.elementIdx);
        props.handleModify(false);
        event.preventDefault();
    }

    return (
        <form style={{ display: "inline" }} name="ItemName" onSubmit={submitFunction}>
            <input type="text" id="name" name="name"
                defaultValue={props.element.name}
                onChange={(e) => {
                    const NewName = e.target.value;
                    props.checkRuleNameFunction(NewName);
                    if (!props.checkDeviceName) {
                        props.modifyElementName(NewName)
                    }
                }}
            />
        </form>
    )
}

const ElementDescription = styled.div`
text-align: left;
margin-left: 2%;
margin-top: 5px;
display: flex;
flex-flow: row;
`;

