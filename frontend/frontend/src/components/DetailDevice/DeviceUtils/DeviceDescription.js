import React, { useState } from 'react';
import styled from "styled-components";

export default function DeviceDescription(props) {
    const [duplicatedDeviceName, handleDuplicatedDeviceName] = useState(false);


    const duplicatedDeviceNameFunction = (newName) => {
        var check = false;
        const devices = props.elements;
        if (devices.some(device => device.name === newName)) {
            check = true;
        }
        handleDuplicatedDeviceName(check)
    }

    return (
        <ElementDescription>
            <ul>
                <li style={{ color: "red", display: duplicatedDeviceName ? 'block' : 'none' }}> Error: device name already exist! Choose another name.</li>
                <li>Name: {props.modify ?
                    <ModifyNameForm
                        {...props}
                        duplicatedDeviceNameFunction={duplicatedDeviceNameFunction}
                        duplicatedDeviceName={duplicatedDeviceName}
                    />
                    : props.element.name}
                </li>
                <li>{props.description}</li>
                <li>Id: {props.elementId}</li>
                <li>Expiration (s): {SetExpiration(props)}</li>
            </ul>
        </ElementDescription>
    )

}

function SetExpiration(props) {
    var device = props.element
    if (device.expiration === "no") {
        return "not settable for this device"
    }
    if (props.modify) {
        return (
            <input type="number" id="expiration" name="expiration"
                defaultValue={parseInt(device.expiration)}
                onChange={(e) => {
                    device.expiration = e.target.value
                    props.setDeviceElement(device) 
                }}
            />
        )
    } else {
        return (device.expiration)
    }
}

function ModifyNameForm(props) {
    const submitFunction = (event) => {
        props.updateDeviceRequest(props.elementId);
        props.handleModifyDevice();
        event.preventDefault();
    }
    var device = props.element

    return (
        <form style={{ display: "inline" }} name="ItemName" onSubmit={submitFunction}>
            <input type="text" id="name" name="name"
                defaultValue={device.name}
                onChange={(e) => {
                    props.duplicatedDeviceNameFunction(e.target.value);
                    if (!props.duplicatedDeviceName) {
                        device.name = e.target.value
                        props.setDeviceElement(device)
                    }
                }}
            />
        </form>
    )
}


const ElementDescription = styled.div`
text-align: left;
margin-left: 2%;
margin-top: 5px;
display: flex;
flex-flow: row;
`;

