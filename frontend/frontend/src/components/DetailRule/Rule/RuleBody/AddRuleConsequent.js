import styled from "styled-components";
import AddIcon from "@material-ui/icons/Add";

export default function AddRuleConsequent(props) {
  var consequentsId = props.element.device_consequents;
  var idx = -1;
  return (
    <List>
      {props.devices
        .filter(function checkFilter(device) {
          return device.type === "actuator";
        })
        .map((item) => {
          idx++;
          if (!consequentsId.some((c) => c === item.id)) {
            return (
              <RuleElement
                key={idx}
                onClick={() => {
                  props.setRuleElement(item.id);
                  props.addNewRuleAConsequentRequest(item.id);
                  props.handleSetRuleConsequent(true);
                }}
              >
                <span> {item.name} </span>
                <AddIcon
                  fontSize="small"
                  style={{
                    color: "black",
                    float: "right",
                    marginRight: "10px",
                  }}
                />
              </RuleElement>
            );
          } else {
            return null;
          }
        })}
    </List>
  );
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
