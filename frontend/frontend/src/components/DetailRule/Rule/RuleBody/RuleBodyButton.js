import React from "react";
import Button from "@material-ui/core/Button";
import styled from "styled-components";
import AddIcon from "@material-ui/icons/Add";

export default function RuleBodyButton(props) {
  const consequents =
    props.ruleBody === process.env.REACT_APP_RULE_BODY_CONSEQUENTS ||
    props.ruleBody === process.env.REACT_APP_RULE_BODY_ADD_CONSEQUENTS;
  var antecedentColor = "#eead4c";
  var consequentColor = "#cccccc5d";
  var addButtonColor = "#f6ca88";
  var variantAntecedent = true;
  if (consequents) {
    variantAntecedent = false;
    antecedentColor = "#cccccc5d";
    consequentColor = "#eead4c";
  }
  const addConsequentElement =
    props.ruleBody === process.env.REACT_APP_RULE_BODY_ADD_CONSEQUENTS;
  if (addConsequentElement) {
    consequentColor = "#f6ca88";
    addButtonColor = "#eead4c";
  }
  const addAntecedentElement =
    props.ruleBody === process.env.REACT_APP_RULE_BODY_ADD_ANTECEDENTS;
  if (addAntecedentElement) {
    antecedentColor = "#f6ca88";
    addButtonColor = "#eead4c";
  }
  return (
    <RuleHeaderButtons>
      <Button
        variant={variantAntecedent ? "contained" : "outlined"}
        style={{
          backgroundColor: antecedentColor,
          padding: 3,
          paddingTop: 5,
          paddingBottom: 5,
          fontWeight: variantAntecedent ? "bold" : "",
        }}
        id="ruleAntecedentButton"
        onClick={() => {
          props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ANTECEDENTS);
        }}
      >
        {process.env.REACT_APP_RULE_BODY_ANTECEDENTS}
      </Button>
      <Button
        style={{
          display: variantAntecedent ? "" : "none",
          backgroundColor: addButtonColor,
          padding: 3,
          paddingTop: 5,
          paddingBottom: 5,
          minWidth: 0,
        }}
        id="addRuleAntecedentButton"
        onClick={() => {
          props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ADD_ANTECEDENTS);
        }}
      >
        <AddIcon />
      </Button>
      <Button
      variant={variantAntecedent ? "outlined" : "contained"}
        style={{
          backgroundColor: consequentColor,
          padding: 3,
          paddingTop: 5,
          paddingBottom: 5,
          fontWeight: variantAntecedent ? "" : "bold",
        }}
        id="ruleConsequentButton"
        onClick={() => {
          console.log("press conesquent");
          props.handleRuleBody(process.env.REACT_APP_RULE_BODY_CONSEQUENTS);
        }}
      >
        {process.env.REACT_APP_RULE_BODY_CONSEQUENTS}
      </Button>
      <Button
        style={{
          display: variantAntecedent ? "none" : "",
          backgroundColor: addButtonColor,
          padding: 3,
          paddingTop: 5,
          paddingBottom: 5,
          minWidth: 0,
        }}
        id="addRuleConsequentButton"
        onClick={() => {
          props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ADD_CONSEQUENTS);
        }}
      >
        <AddIcon />
      </Button>
    </RuleHeaderButtons>
  );
}

const RuleHeaderButtons = styled.div`
  width: 100%;
  display: flex;
  flex-direction: row;
  justify-content: center;
  margin-top: 1%;
`;
