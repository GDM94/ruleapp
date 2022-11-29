import CreateRuleProcess from "./CreateRuleProcess";
import Rule from "./Rule/Rule";
import styled from "styled-components";
import TimerAntecedent from "./RuleElements/TimerAntecedent/TimerAntecedent";
import AlertConsequent from "./RuleElements/AlertConsequent/AlertConsequent";
import SwitchConsequent from "./RuleElements/SwitchConsequent/SwitchConsequent";
import ButtonAntecedent from "./RuleElements/ButtonAntecedent/ButtonAntecedent";
import WaterLevelAntecedent from "./RuleElements/WaterLevelAntecedent/WaterLevelAntecedent";
import SwitchAntecedent from "./RuleElements/SwitchAntecedent/SwitchAntecedent";
import ServoAntecedent from "./RuleElements/ServoAntecedent/ServoAntecedent";
import WeatherAntecedent from "./RuleElements/WeatherAntecedent/WeatherAntecedent";
import PhotocellAntecedent from "./RuleElements/PhotocellAntecedent/PhotocellAntecedent";
import ServoConsequent from "./RuleElements/ServoConsequent/ServoConsequent";
import RulePreview from "./Rule/RulePreview";

export default function DetailRule(props) {
  if (
    props.element.id &&
    props.elementId !== "" &&
    props.elements.length > 0 &&
    props.addNewElement === false &&
    props.setRuleAntecedent === false &&
    props.setRuleConsequent === false
  ) {
    return <Rule {...props} />;
  } else if (
    props.elementId === "" &&
    props.addNewElement === true &&
    props.setRuleAntecedent === false &&
    props.setRuleConsequent === false
  ) {
    return <CreateRuleProcess {...props} />;
  } else if (
    props.elementId !== "" &&
    props.addNewElement === false &&
    props.ruleElementId !== "" &&
    props.ruleElement.device_id
  ) {
    if (props.setRuleAntecedent === true || props.setRuleConsequent === true) {
      if (props.ruleElement.device_id.includes("timer")) {
        return <TimerAntecedent {...props} />;
      } else if (props.ruleElement.device_id.includes("WATERLEVEL")) {
        return <WaterLevelAntecedent {...props} />;
      } else if (props.ruleElement.device_id.includes("BUTTON")) {
        return <ButtonAntecedent {...props} />;
      } else if (
        props.ruleElement.device_id.includes("SWITCH") &&
        props.setRuleConsequent === true
      ) {
        return <SwitchConsequent {...props} />;
      } else if (
        props.ruleElement.device_id.includes("SWITCH") &&
        props.setRuleAntecedent === true
      ) {
        return <SwitchAntecedent {...props} />;
      } else if (
        props.ruleElement.device_id.includes("SERVO") &&
        props.setRuleAntecedent === true
      ) {
        return <ServoAntecedent {...props} />;
      } else if (props.ruleElement.device_id.includes("alert")) {
        return <AlertConsequent {...props} />;
      } else if (props.ruleElement.device_id.includes("WEATHER")) {
        return <WeatherAntecedent {...props} />;
      } else if (props.ruleElement.device_id.includes("PHOTOCELL")) {
        return <PhotocellAntecedent {...props} />;
      } else if (props.ruleElement.device_id.includes("SERVO")) {
        return <ServoConsequent {...props} />;
      } else {
        return <></>;
      }
    }
  } else {
    return (
      <ContentContainer>
        <AddDeviceElement
          onClick={() => {
            props.handleRegisterDevicePopUp();
            props.setNewElement("");
            props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ANTECEDENTS);
          }}
        >
          <h5>ADD NEW RULE</h5>
        </AddDeviceElement>
        <List>
          {props.elements.map((item) => {
            return RulePreview(props, item);
          })}
        </List>
      </ContentContainer>
    );
  }
}

const ContentContainer = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-flow: column;
  float: left;
  text-align: center;
`;

const List = styled.ul`
  list-style: none;
  padding-left: 0%;
  padding-top: 5px;
`;

const AddDeviceElement = styled.div`
  color: balck;
  background-color: #cccccc;
  border-radius: 25px;
  margin: 10%;
  margin-top: 2%;
  margin-bottom: 2%;
  padding: 1%;
  &:hover {
    background: #d5d8d8;
  }
`;
