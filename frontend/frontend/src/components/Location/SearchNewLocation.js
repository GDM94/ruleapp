import React from "react";
import styled from "styled-components";
import { withRouter } from "react-router";
import Button from "@material-ui/core/Button";
import List from "@material-ui/core/List";
import ListItem from "@material-ui/core/ListItem";
import ListItemText from "@material-ui/core/ListItemText";
import Divider from "@material-ui/core/Divider";
import CloseIcon from "@material-ui/icons/Close";

class SearchNewLocation extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      name: "",
      displaySearch: false,
    };
  }

  modifyLocationName = (newName) => {
    this.setState({ name: newName });
  };

  showSearchResults = (display) => {
    this.setState({ displaySearch: display });
  };

  submitFunction = (event) => {
    event.preventDefault();
    this.props.getCurrentLocation(this.state.name);
    this.showSearchResults(true);
  };

  render() {
    return (
      <ContentContainer>
        <ElementTitle>
          <Button
            style={{ float: "right" }}
            onClick={() => {
              this.props.handleSearchNewLocation();
            }}
          >
            <CloseIcon fontSize="large" style={{ color: "black" }} />
          </Button>
        </ElementTitle>
        <ElementSettings>
          <form
            style={{ display: "inline" }}
            name="ItemName"
            onSubmit={this.submitFunction}
          >
            <label style={{ marginRight: "10px" }} htmlFor="name">
              search new location:{" "}
            </label>
            <input
              type="text"
              id="name"
              name="name"
              defaultValue=""
              onChange={(e) => {
                const newName = e.target.value;
                this.modifyLocationName(newName);
              }}
            />
          </form>
          <MyButton
            onClick={() => {
              this.props.getCurrentLocation(this.state.name);
              this.showSearchResults(true);
            }}
          >
            search
          </MyButton>
        </ElementSettings>
        <br></br>
        <FoundedLocationList
          {...this.props}
          {...this.state}
          showSearchResults={this.showSearchResults}
        />
      </ContentContainer>
    );
  }
}

function FoundedLocationList(props) {
  var i = 0;
  var locationList = "no locations found";
  if (props.locationList.length > 0) {
    locationList = props.locationList.map((location) => {
      const id = location.name + " (" + location.country + ")";
      i++;
      return (
        <>
          <ListItem
            key={i}
            button
            onClick={() => {
              props.setNewUserLocation(
                location.name,
                location.country,
                location.lat,
                location.lon
              );
              props.handleSearchNewLocation();
            }}
          >
            <ListItemText primary={id} />
          </ListItem>
          <Divider />
        </>
      );
    });
  }

  return (
    <div style={{ display: props.displaySearch ? "" : "none" }}>
      <p>FOUNDED LOCATIONS</p>
      <Divider />
      <List component="div" aria-label="main mailbox folders">
        {locationList}
      </List>
    </div>
  );
}

export default withRouter(SearchNewLocation);

const ElementTitle = styled.div`
  margin-left: 2%;
  margin-right: 2%;
  margin-top: 2%;
`;

const ContentContainer = styled.div`
  display: flex;
  flex-flow: column;
  float: left;
  text-align: center;
`;

const ElementSettings = styled.div`
  justify-content: center;
  padding: 1%;
  display: flex;
  flex-flow: row;
  align-items: center;
  border-radius: 25px;
  text-align: center;
  background-color: #cccccc;
  padding: 2%;
  margin-left: 2%;
  margin-right: 2%;
`;

const MyButton = styled(Button)`
  border: black solid 1px !important;
`;
