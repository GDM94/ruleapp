import React from 'react';
import Button from '@material-ui/core/Button';
import DeleteIcon from '@material-ui/icons/Delete';
import DoneIcon from '@material-ui/icons/Done';
import RefreshIcon from '@material-ui/icons/Refresh';
import ButtonGroup from '@material-ui/core/ButtonGroup';
import styled from "styled-components";

export default function RuleElementTitle(props) {
    return (
        <ElementTitle>
            <h2> {props.setRuleAntecedent ? "antecedent" : "consequent"}| {props.ruleElement.device_name} </h2>
            <ButtonGroup variant="text" color="primary" aria-label="text primary button group">
                <Button onClick={() => {
                    props.updateRuleElementRequest(props.newRuleId, props.ruleElementId);
                    props.setRuleAntecedent ? props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ANTECEDENTS) 
                    : props.handleRuleBody(process.env.REACT_APP_RULE_BODY_CONSEQUENTS);
                }}>
                    <DoneIcon fontSize="large" style={{ color: "black" }} />
                </Button>
                <Button
                    onClick={() => {
                        props.getRuleElementMeasure(props.ruleElementId);
                    }}>
                    <RefreshIcon fontSize="large" style={{ color: "black" }} />
                </Button >
                <Button onClick={() => {
                    props.deleteRuleElementRequest(props.newRuleId, props.ruleElementId)
                    props.setRuleAntecedent ? props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ANTECEDENTS) 
                    : props.handleRuleBody(process.env.REACT_APP_RULE_BODY_CONSEQUENTS);
                }}>
                    <DeleteIcon fontSize="large" style={{ color: "red" }} />
                </Button>
            </ButtonGroup>
        </ElementTitle>
    )
}

const ElementTitle = styled.div`
text-align: left;
margin-left: 2%;
margin-right: 2%;
margin-top: 2%;
display: flex;
flex-flow: row;
`;