import React from 'react';

export default function ModifyName(props) {

    const checkDeviceNameFunction = (newName) => {
        var checkDeviceName = false;
        const devices = props.elements;
        if (devices.some(device => device.name === newName)) {
            checkDeviceName = true;
        }
        return checkDeviceName
    }

    const submitFunction = (event) => {
        props.updateDeviceRequest(props.elementType);
        props.handleModifyDevice();
        event.preventDefault();
    }
    return (
        <form style={{ display: "inline" }} name="ItemName" onSubmit={submitFunction}>
            <input type="text" id="name" name="name"
                defaultValue={props.element.name}
                onChange={(e) => {
                    const NewName = e.target.value;
                    var checkName = checkDeviceNameFunction(NewName);
                    if (!checkName) {
                        props.modifyElementName(NewName)
                    }
                }}
            />
        </form>
    )
}