import ConsequentRuleBody from './ConsequentRuleBody';
import AntecedentRuleBody from './AntecedentRuleBody';
import AddRuleAntecedent from './AddRuleAntecedent';
import AddRuleConsequent from './AddRuleConsequent';



export default function RuleBody(props) {
    switch (props.ruleBody) {
        case process.env.REACT_APP_RULE_BODY_ANTECEDENTS:
            return <AntecedentRuleBody {...props} />
        case process.env.REACT_APP_RULE_BODY_CONSEQUENTS:
            return <ConsequentRuleBody {...props} />
        case process.env.REACT_APP_RULE_BODY_ADD_ANTECEDENTS:
            return <AddRuleAntecedent {...props} /> 
        case process.env.REACT_APP_RULE_BODY_ADD_CONSEQUENTS:
            return <AddRuleConsequent {...props} />    
        default:
            return <></>;
    }
}