import React from 'react';
import styled from "styled-components";
import RuleElementTitle from '../RuleElementTitle';
import WeatherAntecedentDetail from './WeatherAntecedentDetail';

export default class WeatherAntecedent extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            
        }
    }

    


    render() {
        return (
            <ContentContainer>
                <RuleElementTitle {...this.props}/>
                <WeatherAntecedentDetail {...this.props} options={options}/>
            </ContentContainer>
        )
    }
}

const options = [
    { value: 'between', label: 'between' },
    { value: '>', label: '>' },
    { value: '<', label: '<' }
  ]


const ContentContainer = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-flow: column;
  float:left;
  text-align: center;
  max-height:100%;
  overflow-y: auto;
`;
