import React from 'react';
import axios from 'axios';
import "bootstrap/dist/css/bootstrap.min.css";
import { withRouter } from 'react-router-dom';
import styled from "styled-components";
import TopBar from '../components/TopBar/TopBar';
import TopBar2 from '../components/TopBar2/TopBar2';
import LogoutLateralMenu from "../components/TopBar/LogoutLateralMenu";
import DetailRule from '../components/DetailRule/DetailRule';

var jwt = require('jwt-simple');

class RulesPage extends React.Component {
    constructor(props) {
        super(props);
        const decoded = jwt.decode(this.props.location.state.token, process.env.REACT_APP_JWT_SECRET);
        const idToken = jwt.encode({ "user_id": decoded.user_id }, process.env.REACT_APP_JWT_SECRET);
        axios.defaults.headers.common['token'] = idToken;
        axios.defaults.timeout.toFixed(0);
        this.state = {

        }
    }

    componentDidMount() {
        this.props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ANTECEDENTS);
        if (this.props.addNewElement === true) {
            this.props.handleRegisterDevicePopUp();
        }
        this.props.getElements();
        this.props.setRouteUrlWithRequests(this.props.location.state.path)
    }

    render() {
        return (
            <>
                <TopBar
                    {...this.props}
                />
                <TopBar2
                    {...this.props}
                />
                <GreatBody>
                    <ContentContainer>
                        <DetailRule
                            {...this.props}
                        />
                    </ContentContainer>
                    <LogoutLateralMenu
                        {...this.props}
                    />
                </GreatBody>
            </>
        );

    }
}

export default withRouter(RulesPage)


const GreatBody = styled.div`
display: flex;
flex-flow: row;
width: 100%;
height: 100%;
`;

const ContentContainer = styled.div`
width: 100%;
height: 100%;
display: flex;
flex-flow: column;
text-align: center;
`;





