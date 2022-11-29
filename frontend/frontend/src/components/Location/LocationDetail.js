import Location from "./Location";
import SearchNewLocation from "./SearchNewLocation";

export default function LocationDetail(props){
    if (props.searchLocationPopUp === false){
        return(<Location {...props}/>)
    }
    else{
        return(<SearchNewLocation {...props}/>)
    }
}