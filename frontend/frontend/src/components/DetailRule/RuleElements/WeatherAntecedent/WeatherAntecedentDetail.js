import React from 'react';
import styled from "styled-components";
import Moment from 'react-moment';
import IconDescription from './WeatherComponents/IconAntecedentDetails';
import TempDescription from './WeatherComponents/TempAntecedentDetails';
import CloudsDescription from './WeatherComponents/CloudsAntededentDetails';
import HumidityDescription from './WeatherComponents/HumidityAntecedentDetails';

export default function WeatherAntecedentDetail(props) {
    return (
        <ElementContent>
            <div>
                <IconDescription {...props} />
                <br></br>
                <TempDescription {...props} />
                <br></br>
                <CloudsDescription {...props} />
                <br></br>
                <HumidityDescription {...props} />
                <br></br>
                <TimeDescription {...props} />
            </div>
        </ElementContent>
    )

}


function TimeDescription(props) {
    return (
        <ElementMeasure>
            <h5>TIME:</h5>
            <ul>
                <li> Sunrise: <Moment unix format={'HH:mm'}>{parseInt(props.ruleElement.sunrise)}</Moment> </li>
                <li> Sunset: <Moment unix format={'HH:mm'}>{parseInt(props.ruleElement.sunset)}</Moment> </li>
            </ul>
        </ElementMeasure>
    )
}




const ElementContent = styled.div`
border: solid #d9d9d9 1px;
display: flex;
flex-flow: column;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
margin-bottom: 2%;
text-align: left;
padding: 2%;
background-color: #cccccc;
`;

const ElementMeasure = styled.div`
border: solid black 2px;
border-radius: 25px;
margin-left: 2%;
margin-right: 2%;
padding: 3%;
background-color: #a7b4a8;
text-align: left;
justify-content: left;
background-color: #e6e6e6;
`;

