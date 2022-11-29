import React from "react";
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import ExitToAppIcon from '@material-ui/icons/ExitToApp';
import { withRouter } from 'react-router-dom';

function ButtonLogout(props) {
    return (
        <ListItem button onClick={() => {
            props.userLogoutRequest();
        }}>
            <ListItemIcon>
                <ExitToAppIcon fontSize="small" style={{ color: 'white' }} />
            </ListItemIcon>
            <ListItemText primary="logout" />
        </ListItem>
    )
}

export default withRouter(ButtonLogout)