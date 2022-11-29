import React from "react";
import styled from "styled-components";
import List from '@material-ui/core/List';
import Divider from '@material-ui/core/Divider';
import ButtonLogout from "./ButtonLogout";
import ButtonSettings from "./ButtonSettings";
import ButtonMain from "./ButtonMain";

function LogoutLateralMenu(props) {
    return (
        <PopperStyled id='menu-popper' style={{ display: props.menuPopUp ? "" : "none" }}>
            <List component="nav" aria-label="main mailbox folders">
                <ButtonMain {...props} />
                <Divider />
                <ButtonSettings {...props} />
                <Divider />
                <ButtonLogout {...props} />
            </List>
            <Divider />
        </PopperStyled>
    );
}

export default LogoutLateralMenu;

const PopperStyled = styled.div`
position: absolute;
top: 51px;
right: 0px;
color: white;
background-color: #737373;
height:100%;
width: 20%;
`;