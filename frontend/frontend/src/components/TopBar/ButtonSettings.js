import React from "react";
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import { withRouter } from 'react-router-dom';
import AccountCircleIcon from '@material-ui/icons/AccountCircle';

function ButtonSettings(props) {
    return (
        <ListItem button onClick={() => {
            props.handleSettings();
            props.history.push({ pathname: process.env.REACT_APP_SETTINGS_URL, state: props.location.state })
        }}>
            <ListItemIcon>
                <AccountCircleIcon fontSize="small" style={{ color: 'white' }} />
            </ListItemIcon>
            <ListItemText primary="profile" />
        </ListItem>
    )
}

export default withRouter(ButtonSettings)