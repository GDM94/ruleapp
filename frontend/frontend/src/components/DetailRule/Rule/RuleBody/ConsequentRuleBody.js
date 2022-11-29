import React, { Component } from 'react';
import EditIcon from '@material-ui/icons/Edit';
import styled from "styled-components";
import { SortableContainer, SortableElement } from 'react-sortable-hoc';
import { arrayMoveImmutable } from 'array-move';

export default class ConsequentRuleBody extends Component {
    onSortEnd = ({ oldIndex, newIndex }) => {
        const items = arrayMoveImmutable(this.props.element.rule_consequents, oldIndex, newIndex)
        const orderedItems = items.map((element, index) => {
            element.order = index.toString()
            element.delay = "0"
            return element
        })
        const orderedElementsId = items.map(element => {
            return element.device_id
        })
        this.props.updateRuleConsequentOrderLocal(orderedItems);
        this.props.updateRuleConsequentOrderRequest(orderedElementsId)
    };

    render() {
        return <ConsequentRuleBodyList
            {...this.props}
            items={this.props.element.rule_consequents}
            onSortEnd={this.onSortEnd}
            lockAxis='y'
            lockToContainerEdges={true}
            lockOffset='0%'
            distance={1} />
    }
}

const ConsequentRuleBodyList = SortableContainer((props) => {
    return (
        <List>
            {props.items.map((item, index) => {
                return <ConsequentRuleElement {...props} item={item} key={index} index={index} />;
            })}
        </List>
    )
})

const ConsequentRuleElement = SortableElement((props) => {
    var itemInfo = ""
    if (props.item.device_id.includes("alert")) {
        itemInfo = props.item.order + " - " + props.item.device_name
    }
    else {
        itemInfo = props.item.order + " - " + props.item.device_name + " (delay " + props.item.delay + " s) "
    }

    return (
        <RuleElement
            onClick={() => {
                props.handleSetRuleConsequent(true);
                props.setRuleElement(props.item.device_id);
                props.getRuleConsequentById(props.item.device_id);
            }}>

            <span> {itemInfo} </span>
            <EditIcon fontSize="small" style={{ color: "black", float: "right", marginRight: "10px" }} />
        </RuleElement>
    )
})

const List = styled.ul`
list-style: none;
padding-left: 0%;
padding-top: 5px;
`;


const RuleElement = styled.li`
color: balck;
background-color: #cccccc;
border-radius: 25px;
margin: 2%;
margin-top: 0%;
padding: 1%;
padding-left: 5%;
text-align: left;
&:hover {
    background: #d5d8d8;
}
`;


