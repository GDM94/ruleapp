import React from "react";
import { withRouter } from 'react-router-dom';
import styled from "styled-components";
import TopBar from '../components/TopBar/TopBar';
import LoginLateralMenu from "../components/TopBar/LoginLateralMenu";

class Login extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      userName: "",
      password: ""
    }
  }

  setUserName = (userName) => {
    this.setState({ userName: userName })
  }
  setPassword = (password) => {
    this.setState({ password: password })
  }

  loginSubmit = (event) => {
    if (this.state.userName.length > 0 && this.state.password.length > 0) {
      this.props.UserLoginRequest(this.state.userName, this.state.password);
    }
    event.preventDefault();

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
                <Title>LOGIN</Title>
              </FormTitle>
              <FormBody>
                <div>
                  <p style={{ color: "red", display: this.props.userLoginError ? '' : 'none' }}> Error: User Name or Password are not correct!</p>
                </div>
                <div>
                  <form name="login" onSubmit={this.loginSubmit}>
                    <input type="text" id="name" name="name" placeholder="email"
                      onChange={(e) => {
                        this.props.handleUserLoginError(false);
                        const NewName = e.target.value;
                        this.setUserName(NewName)
                      }}
                    />
                    <br></br>
                    <input type="password" id="password" name="password" placeholder="password"
                      onChange={(e) => {
                        this.props.handleUserLoginError(false);
                        const password = e.target.value;
                        this.setPassword(password)
                      }}
                    />
                    <br></br>
                    <input type="submit" style={{ visibility: "hidden" }} />
                  </form>
                </div>
              </FormBody>
              <FormBottom>
                <div>
                  <button onClick={() => {
                    if (this.state.userName.length > 0 && this.state.password.length > 0) {
                      this.props.UserLoginRequest(this.state.userName, this.state.password);
                    }
                  }}>
                    LOGIN
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

export default withRouter(Login);
