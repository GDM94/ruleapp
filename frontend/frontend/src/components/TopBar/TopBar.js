import React from "react";
import styled from "styled-components";
import IconButton from '@material-ui/core/IconButton';
import MenuIcon from '@material-ui/icons/Menu';

function TopBar(props) {
  return (
    <TopBar1>
      <TopBar1Element>
        RULEAPP
      </TopBar1Element>
      <TopBar1Button onClick={(event) => { props.handleMenuPopUp(event) }}>
        <MenuIcon fontSize="large" style={{ color: 'white' }} />
      </TopBar1Button>
    </TopBar1>

  );
}

export default TopBar;

const TopBar1 = styled.div`
  background-color: #737373;
  width: 100%;
  height: 50px;
`;


const TopBar1Element = styled.span`
  float: left;
  color: #eead4c;
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
  font-size: 15px;
  font-style: oblique;
  font-weight: bold !important;
  margin-left: 10px;
`;

const TopBar1Button = styled(IconButton)`
  float: right;
  padding-bottom: 0% !important;
`;