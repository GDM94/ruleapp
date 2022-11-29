import React from 'react';
import styled from "styled-components";
import { withRouter } from 'react-router';
import AccountCircleIcon from '@material-ui/icons/AccountCircle';

class Profile extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
        }
    }

    render(){
        return(
            <ContentContainer>
                <ElementTitle>
                     <h2> <AccountCircleIcon fontSize="large" style={{ color: 'balck' }}/>  {process.env.REACT_APP_PAGE_SETTINGS_PROFILE}</h2>
                </ElementTitle>
                <ElementContent>
                    <ul>
                        <li>name:  {this.props.name}</li>
                        <li>surname:  {this.props.surname}</li>
                        <li>email:  {this.props.email}</li>
                        <li>password:  {this.props.password}</li>
                    </ul>
                </ElementContent>

            </ContentContainer>
        )
    }
}

export default withRouter(Profile)

const ContentContainer = styled.div`
  display: flex;
  flex-flow: column;
  float:left;
  text-align: center;
  max-height:100%;
  overflow-y: auto;
`;

const ElementTitle = styled.div`
text-align: left;
margin-left: 2%;
margin-right: 2%;
margin-top: 2%;
display: flex;
flex-flow: row;
`;

const ElementContent = styled.div`
border: solid #d9d9d9 1px;
height: 100%;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
margin-bottom: 2%;
text-align: left;
padding: 2%;
background-color: #cccccc;
`;