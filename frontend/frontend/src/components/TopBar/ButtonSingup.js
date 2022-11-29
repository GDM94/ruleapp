import React from "react";
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import ExitToAppIcon from '@material-ui/icons/ExitToApp';
import { withRouter } from 'react-router-dom';

function ButtonSingup(props) {
    return (
        <ListItem button onClick={() => {
            props.history.push({ pathname: process.env.REACT_APP_SINGUP_URL })
        }}>
            <ListItemIcon>
                <ExitToAppIcon fontSize="small" style={{ color: 'white' }} />
            </ListItemIcon>
            <ListItemText primary="singup" />
        </ListItem>
    )
}

export default withRouter(ButtonSingup)