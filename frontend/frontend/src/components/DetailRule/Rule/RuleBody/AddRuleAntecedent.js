import styled from "styled-components";
import AddIcon from "@material-ui/icons/Add";

export default function AddRuleAntecedent(props) {
  return (
    <List>
      <AddRuleAntecedentsDevice {...props} />
      <AddRuleAntecedentSwitchLastTimeOn {...props} />
    </List>
  );
}

function AddRuleAntecedentsDevice(props) {
  var antecedentsId = props.element.device_antecedents;
  var idx = 500;
  return props.devices
    .filter(function checkFilter(device) {
      return device.type === "sensor";
    })
    .map((item) => {
      idx++;
      if (!antecedentsId.some((c) => c === item.id)) {
        return (
          <RuleElement
            key={idx}
            onClick={() => {
              props.setRuleElement(item.id);
              props.addNewRuleAntecedentRequest(item.id);
              props.handleSetRuleAntecedent(true);
            }}
          >
            <span> {item.name} </span>
            <AddIcon
              fontSize="small"
              style={{ color: "black", float: "right", marginRight: "10px" }}
            />
          </RuleElement>
        );
      } else {
        return null;
      }
    });
}

function AddRuleAntecedentSwitchLastTimeOn(props) {
  var antecedentsId = props.element.device_antecedents;
  var consequents = props.element.rule_consequents;
  var idx = 10000;
  if (consequents.length > 0) {
    idx++;
    return consequents.map((item) => {
      if (!item.device_id.includes("alert")) {
        if (!antecedentsId.some((c) => c === item.device_id)) {
          return (
            <RuleElement
              key={idx}
              onClick={() => {
                props.setRuleElement(item.device_id);
                props.addNewRuleAntecedentRequest(item.device_id);
                props.handleSetRuleAntecedent(true);
              }}
            >
              <span> {item.device_name} </span>
              <AddIcon
                fontSize="small"
                style={{ color: "black", float: "right", marginRight: "10px" }}
              />
            </RuleElement>
          );
        } else {
          return null;
        }
      } else {
        return null;
      }
    });
  } else {
    return null;
  }
}

const List = styled.ul`
  list-style: none;
  padding-left: 0%;
  padding-top: 5px;
`;

const RuleElement = styled.li`
  color: balck;
  background-color: #cccccc;
  border-radius: 25px;
  margin: 2%;
  margin-top: 0%;
  padding: 1%;
  padding-left: 5%;
  text-align: left;
  &:hover {
    background: #d5d8d8;
  }
`;
