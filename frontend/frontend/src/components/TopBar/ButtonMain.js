import React from "react";
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import HomeIcon from '@material-ui/icons/Home';
import { withRouter } from 'react-router-dom';

function ButtonMain(props) {
    return (
        <ListItem button onClick={() => {
            props.handleSettings();
            props.setRouteUrl(process.env.REACT_APP_LOCATION_URL);
            props.location.state.path = process.env.REACT_APP_LOCATION_URL;
            props.history.push({ pathname: process.env.REACT_APP_LOCATION_URL, state: props.location.state })
        }}>
            <ListItemIcon>
                <HomeIcon fontSize="small" style={{ color: 'white' }} />
            </ListItemIcon>
            <ListItemText primary="home" />
        </ListItem>
    )
}

export default withRouter(ButtonMain)