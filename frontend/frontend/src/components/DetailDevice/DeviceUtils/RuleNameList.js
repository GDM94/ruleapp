import React, { useState } from 'react';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemText from '@material-ui/core/ListItemText';
import Divider from '@material-ui/core/Divider';
import { withRouter } from 'react-router-dom';
import Button from '@material-ui/core/Button';
import Collapse from '@material-ui/core/Collapse';
import ExpandLess from '@material-ui/icons/ExpandLess';
import ExpandMore from '@material-ui/icons/ExpandMore';


function RuleNameList(props) {
    const [openRule, handleOpenRule] = useState(false);
    const handleClick = () => {
        handleOpenRule(!openRule);
    };

    if (props.element.rules && props.element.rules.length > 0) {
        const rules = props.element.rules;
        const rulesNameList = rules.map(rule => {
            if (rule.name === null){
                return null;
            }
            return (
                <div key={rule.id}>
                    <ListItem key={rule.id} button onClick={() => {
                        props.handleRuleBody(process.env.REACT_APP_RULE_BODY_ANTECEDENTS);
                        props.setNewRule(rule.id);
                        props.setRouteUrl(process.env.REACT_APP_RULES_URL);
                        props.location.state.page = process.env.REACT_APP_PAGE_RULES
                        props.location.state.path = process.env.REACT_APP_RULES_URL;
                        props.history.push({ pathname: process.env.REACT_APP_RULES_URL, state: props.location.state })
                    }}>
                        <ListItemText primary={ rule.name} />
                    </ListItem>
                    <Divider />
                </div>

            )
        })
        return (
            <ul>
                <li><Button onClick={() => { handleClick(); }}>
                    RULES  {openRule ? <ExpandLess /> : <ExpandMore />}
                </Button></li>
                <Collapse in={openRule} timeout="auto" unmountOnExit>
                    <List component="div" aria-label="main mailbox folders">
                        {rulesNameList}
                    </List>
                </Collapse>
            </ul>

        )
    }
    else {
        return (
            <ul>
                <li><Button onClick={() => { handleClick(); }}>
                    RULES  {openRule ? <ExpandLess /> : <ExpandMore />}
                </Button></li>
                <Collapse in={openRule} timeout="auto" unmountOnExit>
                    <List component="div" aria-label="main mailbox folders">
                        <ListItem>
                            <ListItemText primary="no rules setted" />
                        </ListItem>
                        <Divider />
                    </List>
                </Collapse>
            </ul>
        )
    }
}

export default withRouter(RuleNameList)


