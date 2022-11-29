import React from "react";
import styled from "styled-components";
import { withRouter } from "react-router";
import LocationOnIcon from "@material-ui/icons/LocationOn";

class Location extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      name: "",
      displaySearch: false,
    };
  }

  render() {
    return (
      <ContentContainer>
        <AddDeviceElement
          onClick={() => {
            this.props.handleSearchNewLocation();
          }}
        >
          <h5>SEARCH NEW LOCATION</h5>
        </AddDeviceElement>

        <ElementContent>
          <ElementDescription>
            <h5>{this.props.locationName}, {this.props.locationCountry}</h5>
            <h5> <LocationOnIcon fontSize="large" style={{ color: "balck" }} />({this.props.locationLat}, {this.props.locationLon})</h5>
           
          </ElementDescription>
        </ElementContent>
      </ContentContainer>
    );
  }
}



const AddDeviceElement = styled.div`
  color: balck;
  background-color: #cccccc;
  border-radius: 25px;
  margin: 10%;
  margin-top: 2%;
  margin-bottom: 2%;
  padding: 1%;
  &:hover {
    background: #d5d8d8;
  }
`;

export default withRouter(Location);

const ContentContainer = styled.div`
  display: flex;
  flex-flow: column;
  float: left;
  text-align: center;
`;

const ElementContent = styled.div`
  border: solid #d9d9d9 1px;
  height: 100%;
  border-radius: 25px;
  margin: 20%;
  margin-top: 1%;
  text-align: left;
  padding: 2%;
  background-color: #cccccc;
`;

const ElementDescription = styled.div`
  text-align: center;
  margin-left: 2%;
  margin-top: 5px;
`;
