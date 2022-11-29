import React from "react";
import styled from "styled-components";
import Button from "@material-ui/core/Button";
import Loader from "react-loader-spinner";
import CloseIcon from "@material-ui/icons/Close";

export default class RegisterDeviceProcess extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      newDeviceId: "",
    };
  }

  NewDeviceIdRegistration(newDeviceId) {
    this.setState({
      newDeviceId: newDeviceId,
    });
  }

  createDevice = (event) => {
    this.props.registerDeviceRequest(this.state.newDeviceId);
    event.preventDefault();
  };

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
          <p
            style={{
              color: "red",
              display: this.props.registerElementError ? "block" : "none",
            }}
          >
            {" "}
            Error: device Id is not correct because already registered!
          </p>
          <ElementSettings>
            <form style={{ marginRight: "10px" }} onSubmit={this.createDevice}>
              <label style={{ marginRight: "10px" }} htmlFor="deviceId">
               ADD NEW DEVICE ID:{" "}
              </label>
              <input
                name="deviceId"
                id="deviceId"
                type="text"
                onChange={(e) => {
                  var newDeviceId = e.target.value;
                  this.props.handleRegisterElementError(false);
                  this.NewDeviceIdRegistration(newDeviceId);
                }}
              />
            </form>
            <MyButton
              onClick={() => {
                this.props.registerDeviceRequest(this.state.newDeviceId);
              }}
            >
              NEXT
            </MyButton>
          </ElementSettings>
        </ElementContent>
        <h3 style={{ display: this.props.loading === true ? "block" : "none" }}>
          Please wait 10s for initialization
          <Loader type="Puff" color="#00BFFF" height={100} width={100} />
        </h3>
      </ContentContainer>
    );
  }
}

const ContentContainer = styled.div`
  display: flex;
  flex-flow: column;
  float: left;
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
