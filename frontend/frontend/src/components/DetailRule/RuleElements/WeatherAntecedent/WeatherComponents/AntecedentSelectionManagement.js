
export default function AntecedentSelectionManagement(props) {
    return (
        <input type="radio"
            checked={props.check === "true"}
            value={props.check === "true" ? "true" : "false"}
            onClick={e => { props.onClickAntecedent(e)}}
            onChange={e => {
                console.log(e.target.value)
            }}
        />
    )
}