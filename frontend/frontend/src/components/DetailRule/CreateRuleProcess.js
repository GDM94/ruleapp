import React from 'react';
import styled from "styled-components";
import Button from '@material-ui/core/Button';
import CloseIcon from "@material-ui/icons/Close";

export default class CreateRuleProcess extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      checkCreateNewRule: false,
      ruleName: "",
    }

  }

  checkCreateRuleFunction(rules, ruleName) {
    if (rules.some(rule => rule.name === ruleName)) {
      this.setState({
        checkCreateNewRule: true
      })
    }
    else {
      this.setState({
        checkCreateNewRule: false,
        ruleName: ruleName
      })
    }
  }

  createRule = (event) => {
    if (!this.state.checkCreateNewRule) {
      const ruleName = this.state.ruleName
      this.props.createRuleRequest(ruleName);
    }
    event.preventDefault();
  }

  render() {
    return (
      <ContentContainer>
        <ElementTitle>
          <Button style={{float: "right" }}
            onClick={() => {
              this.props.handleRegisterDevicePopUp();
            }}
          >
            <CloseIcon fontSize="large" style={{ color: "black" }} />
          </Button>
        </ElementTitle>
        <ElementContent>
          <p style={{ color: "red", display: this.state.checkCreateNewRule ? 'block' : 'none' }}> Error: rule name already exist! Choose another name.</p>
          <ElementSettings>
            <form style={{ marginRight: "10px" }} onSubmit={this.createRule}>
              <label style={{ marginRight: "10px" }} htmlFor="ruleName">ADD RULE NAME: </label>
              <input name="ruleName" id="ruleName" type="text"
                onChange={(e) => {
                  const ruleName = e.target.value;
                  this.checkCreateRuleFunction(this.props.rules, ruleName);
                }}
              />
            </form>
            <MyButton onClick={() => {
              if (!this.state.checkCreateNewRule) {
                const ruleName = this.state.ruleName;
                this.props.createRuleRequest(ruleName);
              }
            }}>
              NEXT
            </MyButton>
          </ElementSettings>
        </ElementContent>
      </ContentContainer>)
  }

}

const ContentContainer = styled.div`
  display: flex;
  flex-flow: column;
  float:left;
  text-align: center;
`;

const ElementContent = styled.div`
border: solid #d9d9d9 1px;
height: 100%;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
margin-bottom: 2%;
text-align: center;
padding: 2%;
background-color: #cccccc;
`;


const ElementTitle = styled.div`
margin-left: 2%;
margin-right: 2%;
margin-top: 2%;
`;

const ElementSettings = styled.div`
margin-left: 2%;
margin-right: 2%;
justify-content: center;
padding: 1%;
display: flex;
flex-flow: row;
align-items: center;
`;

const MyButton = styled(Button)`
border: black solid 1px !important;
`;
