import React from 'react';
import styled from "styled-components";
import RuleTitle from './RuleTitle';
import RuleBodyButton from './RuleBody/RuleBodyButton';
import RuleDescription from './RuleDescription';
import RuleBody from './RuleBody/RuleBody';

export default class Rule extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            id: "",
            name: "",
            last_time_on: "",
            last_time_off: "",
            last_date_on: "",
            last_date_off: "",
            rule_antecedents: [],
            rule_consequents: [],
            evaluation: "false"
        }
    }
    

    render() {
        return (
            <ContentContainer>
                <RuleTitle {...this.props} />
                <RuleContentDiv>
                    <RuleDescription {...this.props} />
                    <RuleBodyButton {...this.props} />
                    <ElementContent>
                        <RuleBody {...this.props} />
                    </ElementContent>
                </RuleContentDiv>
            </ContentContainer>
        )
    }
}

const ContentContainer = styled.div`
display: flex;
flex-flow: column;
`;


const RuleContentDiv = styled.div`
border: solid #d9d9d9 1px;
height: 100%;
background-color: #cccccc;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
margin-bottom: 2%;
text-align: left;
padding: 2%;
display: flex;
flex-flow: column;
`;

const ElementContent = styled.div`
border: solid black 2px;
border-radius: 25px;
margint: 2%;
padding: 1%;
text-align: center;
background-color: #e6e6e6;
`;

