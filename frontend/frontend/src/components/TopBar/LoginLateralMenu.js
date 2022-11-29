import React from "react";
import styled from "styled-components";
import List from '@material-ui/core/List';
import Divider from '@material-ui/core/Divider';
import ButtonLogin from "./ButtonLogin";
import ButtonSingup from "./ButtonSingup";

function LoginLateralMenu(props) {
    return (
        <PopperStyled id='menu-popper' style={{ display: props.menuPopUp ? "" : "none" }}>
            <List component="nav" aria-label="main mailbox folders">
                <ButtonLogin
                    {...props}
                />
                <Divider style={{ color: 'white !important' }} />
                <ButtonSingup
                    {...props}
                />
            </List>
            <Divider />
        </PopperStyled>
    );
}

export default LoginLateralMenu;

const PopperStyled = styled.div`
color: white;
background-color: #737373;
height:100%;
position: absolute;
top: 50px;
right: 0px;
`;