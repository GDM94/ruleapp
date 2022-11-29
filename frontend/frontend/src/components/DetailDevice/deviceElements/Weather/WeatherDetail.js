import React from 'react';
import styled from "styled-components";
import RuleNameList from '../../DeviceUtils/RuleNameList'
import DeviceDescription from '../../DeviceUtils/DeviceDescription';
import Moment from 'react-moment';
import {FaCloudSunRain} from 'react-icons/fa';
import {IoCloudSharp, IoThunderstormSharp} from 'react-icons/io5';
import { BsFillSunFill, BsFillCloudSunFill, BsFillCloudsFill, BsCloudSleetFill, BsSnow } from 'react-icons/bs';
import {RiMistLine} from 'react-icons/ri'


export default function WeatherDetail(props) {
    return (
        <ElementContent>
            <div>
                <DeviceDescription
                    {...props}
                />
                <ElementDescription>
                    <ul>
                        <li>Last update:  {props.element.last_date_update} {props.element.last_time_update}</li>
                    </ul>
                </ElementDescription>

                <LocationDescription {...props} />
                <br></br>
                <WeatherDescription {...props} />
                <br></br>
                <TempDescription {...props} />
                <br></br>
                <CloudsDescription {...props} />
                <br></br>
                <HumidityDescription {...props} />
                <br></br>
                <RuleNameList {...props} />
            </div>
        </ElementContent>
    )

}


function LocationDescription(props) {
    return (
        <ElementMeasure>
            <h5>LOCATION: </h5>
            <ul>
                <li> Position:  {props.element.location_name} ({props.element.country})</li>
                <li> Sunrise: <Moment unix format={'HH:mm'}>{parseInt(props.element.sunrise)}</Moment> </li>
                <li> Sunset: <Moment unix format={'HH:mm'}>{parseInt(props.element.sunset)}</Moment> </li>
            </ul>
        </ElementMeasure>
    )
}

function WeatherDescription(props) {
    var weather_description = "";
    if (props.element.icon) {
        weather_description = props.element.icon.map((cod) => {
            switch (cod) {
                case '01d':
                    return <h5>clear sky {BsFillSunFill()}</h5>
                case '02d':
                    return <h5>few clouds {BsFillCloudSunFill()}</h5>;
                case '03d':
                    return <h5>scattered clouds {IoCloudSharp()}</h5>;
                case '04d':
                    return <h5>broken clouds {BsFillCloudsFill()}</h5>;
                case '09d':
                    return <h5>shower rain {BsCloudSleetFill()}</h5>;
                case '10d':
                    return <h5>rain {FaCloudSunRain()}</h5>;
                case '11d':
                    return <h5>thunderstorm {IoThunderstormSharp()}</h5>;
                case '13d':
                    return <h5>snow {BsSnow()}</h5>;
                case '50d':
                    return <h5>mist {RiMistLine()}</h5>;
                default:
                    return '-';
            }
        })
    }

    return (
        <ElementMeasure>
            <h5>WEATHER: {weather_description}</h5>
        </ElementMeasure>
    )
}

function TempDescription(props) {
    return (
        <ElementMeasure>
            <h5>TEMPERATURE: {props.element.temp} {props.element.temp_unit}</h5>
            <ul>
                <li> max: {props.element.temp_max} {props.element.temp_unit}</li>
                <li> min: {props.element.temp_min} {props.element.temp_unit}</li>
            </ul>
        </ElementMeasure>
    )
}

function CloudsDescription(props) {
    return (
        <ElementMeasure>
            <h5>CLOUDS: {props.element.clouds} {props.element.clouds_unit}</h5>
        </ElementMeasure>
    )
}

function HumidityDescription(props) {
    return (
        <ElementMeasure>
            <h5>HUMIDITY: {props.element.humidity} {props.element.humidity_unit}</h5>
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

const ElementDescription = styled.div`
text-align: left;
margin-left: 2%;
display: flex;
flex-flow: row;
`;


