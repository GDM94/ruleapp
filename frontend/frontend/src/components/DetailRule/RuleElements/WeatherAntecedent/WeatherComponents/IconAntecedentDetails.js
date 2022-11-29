import React from 'react';
import styled from "styled-components";
import { FaCloudSunRain } from 'react-icons/fa';
import { IoCloudSharp, IoThunderstormSharp } from 'react-icons/io5';
import { BsFillSunFill, BsFillCloudSunFill, BsFillCloudsFill, BsCloudSleetFill, BsSnow } from 'react-icons/bs';
import { RiMistLine } from 'react-icons/ri'

function WeatherIconMapping(icon) {
    switch (icon) {
        case '01d':
            return <>{BsFillSunFill()} clear sky </>
        case '02d':
            return <>{BsFillCloudSunFill()} few clouds </>;
        case '03d':
            return <>{IoCloudSharp()} scattered clouds </>;
        case '04d':
            return <>{BsFillCloudsFill()} broken clouds </>;
        case '09d':
            return <>{BsCloudSleetFill()} shower rain </>;
        case '10d':
            return <>{FaCloudSunRain()} rain </>;
        case '11d':
            return <>{IoThunderstormSharp()} thunderstorm </>;
        case '13d':
            return <>{BsSnow()} snow </>;
        case '50d':
            return <>{RiMistLine()} mist </>;
        default:
            return '-';
    }
}


export default function IconDescription(props) {
    var ruleElement = props.ruleElement;
    var weather_description = "";
    if (props.ruleElement.icon_value) {
        weather_description = props.ruleElement.icon_value.map((icon) => {
            return WeatherIconMapping(icon)
        })
    }

    const manageIcons = (icon, check) => {
        if (check) {
            ruleElement.icon_start_value.push(icon)
        }
        else {
            const idx = ruleElement.icon_start_value.indexOf(icon)
            ruleElement.icon_start_value.splice(idx, 1)
        }
        props.setRuleElementObject(ruleElement);
    }


    const checkedIcon = (icon) => {
        if (ruleElement.icon_start_value && ruleElement.icon_start_value.length > 0) {
            return ruleElement.icon_start_value.some(d => d === icon)
        }
        else {
            return false
        }
    }

    const iconSelection = (icon) => {
        return (
            <IconSelection>
                <input style={{ marginRight: "5px" }} type='checkbox'
                    name={icon} id={icon} value={icon}
                    checked={checkedIcon(icon)}
                    onChange={e => { manageIcons(icon, e.target.checked) }}
                />
                {WeatherIconMapping(icon)}
            </IconSelection>
        )
    }

    return (
        <ElementMeasure>
            <h5>
                <input type="radio"
                    checked={ruleElement.check_icon === "true"}
                    value={ruleElement.check_icon === "true" ? "true" : "false"}
                    onClick={e => {
                        ruleElement.check_icon = e.target.value === "true" ? "false" : "true"
                        props.setRuleElementObject(ruleElement);
                    }}
                    onChange={e => {
                        console.log(e.target.value)
                    }}
                />
                WEATHER: {weather_description}
            </h5>
            <div style={{ display: ruleElement.check_icon === "true" ? "flex" : "none", flexFlow: "column" }}>
                {iconSelection("01d")}
                {iconSelection("02d")}
                {iconSelection("03d")}
                {iconSelection("04d")}
                {iconSelection("09d")}
                {iconSelection("10d")}
                {iconSelection("11d")}
                {iconSelection("13d")}
                {iconSelection("50d")}
            </div>
        </ElementMeasure>
    )
}


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

const IconSelection = styled.label`
width: fit-content; 
cursor: default;
`;
