import BetweenRangeSlider from '../../../../Sliders/BetweenRangeSlider';
import GreatSingleRangeSlider from '../../../../Sliders/GreatSingleRangeSlider';
import LowerRangeSlider from '../../../../Sliders/LowerRangeSlider';
import Select from 'react-select';

export default function SliderManagement(props) {
    return (
        <div style={{ display: props.check === "true" ? "flex" : "none", flexFlow: "column" }}>
            <EvaluateConditionSetting {...props} />
            <br></br>
            <Sliders {...props} />
        </div>
    )
}

function EvaluateConditionSetting(props) {
    return (
        <>
            <h5>- CONDITION SETTING</h5>
            <Select
                menuPortalTarget={document.body}
                styles={{ menuPortal: base => ({ ...base, zIndex: 9999 }) }}
                defaultValue={{ label: props.condition, value: props.condition }}
                options={props.options}
                onChange={e => {
                    props.onChange(e)
                }}
            />
        </>
    )
}


function Sliders(props) {
    switch (props.condition) {
        case "between":
            return <BeetweenRangeSliderSetting {...props} />
        case ">":
            return <GreaterRangeSliderSetting {...props} />
        case "<":
            return <LowerRangeSliderSetting {...props} />
        default:
            return <BeetweenRangeSliderSetting {...props} />
    }
}

function LowerRangeSliderSetting(props) {
    return (
        <>
            <h5>- VALUE SETTING</h5>
            <p>Start Value: {props.min_value.toString()} {props.value_unit}</p>
            <p>Stop Value: {props.start_value} {props.value_unit}</p>
            <LowerRangeSlider
                min={props.min_value}
                max={props.max_value}
                step={1}
                minValue={props.min_value.toString()}
                maxValue={props.start_value}
                ruler={false}
                label={true}
                onInput={(e) => {
                    props.onInput(e, props.condition, props.ruleElement)
                }}

            />
        </>
    )
}

function GreaterRangeSliderSetting(props) {
    return (
        <>
            <h5>- VALUE SETTING</h5>
            <p>Start Value: {props.start_value} {props.value_unit}</p>
            <p>Stop Value: {props.max_value.toString()} {props.value_unit}</p>
            <GreatSingleRangeSlider
                min={props.min_value}
                max={props.max_value}
                step={1}
                minValue={props.start_value}
                maxValue={props.max_value.toString()}
                ruler={false}
                label={true}
                onInput={(e) => {
                    props.onInput(e, props.condition, props.ruleElement)
                }}

            />
        </>
    )
}

function BeetweenRangeSliderSetting(props) {
    return (
        <>
            <h5>- VALUE SETTING</h5>
            <p>Start Value: {props.start_value} {props.value_unit}</p>
            <p>Stop Value: {props.stop_value} {props.value_unit}</p>
            <BetweenRangeSlider
                min={props.min_value}
                max={props.max_value}
                step={1}
                ruler={false}
                label={true}
                preventWheel={false}
                minValue={props.start_value}
                maxValue={props.stop_value}
                onInput={(e) => {
                    props.onInput(e, props.condition, props.ruleElement)
                }}
            />
        </>
    )
}
