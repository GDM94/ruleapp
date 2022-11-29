import React from "react";
import styled from "styled-components";
import { withRouter } from 'react-router-dom';
import TopBar from '../components/TopBar/TopBar';
import LoginLateralMenu from "../components/TopBar/LoginLateralMenu";

class SingUp extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      checkError: false,
      name: "",
      surname: "",
      email: "",
      password: "",
      repeatPassword: "",
      errorMessage: "",
      password_length: 2,

    }
  }

  setName = (name) => {
    this.setState({ name: name })
  }

  setSurname = (surname) => {
    this.setState({ surname: surname })
  }

  setEmail = (email) => {
    this.setState({ email: email })
  }
  setPassword = (password) => {
    this.setState({ password: password })
  }

  setRepeatPassword = (password) => {
    this.setState({ repeatPassword: password })
  }

  registrationSubmit = (event) => {
    this.registrationFunction();
    event.preventDefault();
  }

  registrationFunction = () => {
    if (
      this.state.password.length > this.state.password_length &&
      this.state.email.includes("@") &&
      this.state.password === this.state.repeatPassword &&
      this.state.name.length > 0 &&
      this.state.surname.length > 0
    ) {
      this.setState({ checkError: false });
      this.props.UserRegistrationRequest(this.state.email, this.state.password, this.state.name, this.state.surname);
    }
    else {
      this.setState({ checkError: true });
      if (this.state.password.length <= this.state.password_length) {
        this.setState({ errorMessage: "Error: password don't attempt security standard. Password must be at least 2 character long." });
      }
      else if (!this.state.email.includes("@")) {
        this.setState({ errorMessage: "Error: email non valid" });
      }
      else if (this.state.password === this.state.repeatPassword) {
        this.setState({ errorMessage: "Error: the repeated password is not equal to previous one" });
      }
      else {
        this.setState({ errorMessage: "Generic Error" });
      }
    }
  }

  render() {
    return (
      <Container>
        <TopBar
          {...this.props}
        />
        <Rect2>
          <Rect>
            <Form>
              <FormTitle>
                <Title>SINGUP</Title>
              </FormTitle>
              <FormBody>
                <div>
                  <div>
                    <p style={{ color: "red", display: this.state.checkError ? 'block' : 'none' }}> {this.state.errorMessage}</p>
                    <p style={{ color: "red", display: this.props.duplicateUserError ? 'block' : 'none' }}> Error: Email already registerd</p>
                  </div>
                  <form name="login" onSubmit={this.registrationSubmit}>
                    <input type="text" id="name" name="name" placeholder="name"
                      onChange={(e) => {
                        const name = e.target.value;
                        this.setName(name)
                      }} />
                    <br></br>
                    <input type="text" id="surname" name="surname" placeholder="surname"
                      onChange={(e) => {
                        const surname = e.target.value;
                        this.setSurname(surname)
                      }} />
                    <br></br>
                    <input type="text" id="email" name="email" placeholder="email"
                      onChange={(e) => {
                        const email = e.target.value;
                        this.props.changeUserSingUpCredentials();
                        this.setEmail(email)
                      }} />
                    <br></br>
                    <input type="password" id="password" name="password" placeholder="password"
                      onChange={(e) => {
                        const password = e.target.value;
                        this.setPassword(password)
                      }} />
                    <br></br>
                    <input type="password" id="repeat_password" name="repeat_password" placeholder="repeat password"
                      onChange={(e) => {
                        const password = e.target.value;
                        this.setRepeatPassword(password)
                      }} />
                    <br></br>
                    <input type="submit" style={{ visibility: "hidden" }} />
                  </form>
                </div>
              </FormBody>
              <FormBottom>
                <div>
                  <button onClick={() => {
                    this.registrationFunction();
                  }}>
                    SINGUP
                  </button>
                </div>
              </FormBottom>
            </Form>
          </Rect>
          <LoginLateralMenu
            {...this.props}
          />
        </Rect2>
      </Container>
    );
  }
}

const Container = styled.div`
  display: flex;
  flex-direction: column;
  width: 100%;
  height: 100%;
`;

const Rect2 = styled.div`
  display: flex;
  flex-direction: row;
  width: 100%;
  height: 100%;
`;


const Rect = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: row;
  padding: 5%;
`;

const Form = styled.div`
  width: 100%;
  background-color: rgba(243,240,240,1);
  display: flex;
  flex-direction: column;
`;

const FormTitle = styled.div`
  width: 100%;
  background-color: rgba(243,240,240,1);
  display: flex;
  justify-content: center;
`;

const FormBody = styled.div`
  width: 100%;
  background-color: rgba(243,240,240,1);
  display: flex;
  flex-flow: column;
  justify-content: center;
  text-align: center;
  padding: 5%;
`;

const FormBottom = styled.div`
  width: 100%;
  background-color: rgba(243,240,240,1);
  display: flex;
  justify-content: center;
`;

const Title = styled.h1`
  font-family: Roboto;
  font-style: normal;
  font-weight: 700;
  color: #121212;
  font-size: 34px;
  text-decoration-line: underline;
`;

export default withRouter(SingUp);
