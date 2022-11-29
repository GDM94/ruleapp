import React from "react";
import { Router, Route } from "react-router-dom";
import Login from "./pages/Login";
import SingUp from "./pages/SingUp";
import axios from "axios";
import history from "./history";
import DevicesPage from "./pages/DevicesPage";
import RulesPage from "./pages/RulesPage";
import SettingPage from "./pages/SettingPage";
import LocationPage from "./pages/LocationPage";

var jwt = require("jwt-simple");

export default class MainRouter extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      token: "",
      userLoginError: false,
      duplicateUserError: false,
      registerElementError: false,
      routeUrl: process.env.REACT_APP_LOCATION_URL,
      settingsPage: process.env.REACT_APP_PAGE_SETTINGS,

      menuPopUp: false,

      modifyAlertEmail: false,

      newRuleId: "",
      modify: false,
      addRulePopUp: false,
      setRuleAntecedent: false,
      setRuleConsequent: false,
      ruleBody: process.env.REACT_APP_RULE_BODY_ANTECEDENTS,
      ruleElementId: "",

      antecedentId: "",
      consequentId: "",
      deviceId: "",
      modifyDevice: false,
      registerDevicePopUp: false,

      antecedents: [],
      consequents: [],
      devices: [],
      rules: [],
      antecedent: {},
      consequent: {},
      rule: {},
      device: {},
      ruleElement: {},

      locationName: "",
      locationCountry: "",
      locationLat: "",
      locationLon: "",
      locationList: [],
      loading: false,

      searchLocationPopUp: false,
      devicesFilter: "all"
    };
  }

  loginRedirect = (token) => {
    this.setState({ menuPopUp: false });
    const decoded = jwt.decode(token, process.env.REACT_APP_JWT_SECRET);
    const idToken = jwt.encode(
      { user_id: decoded.user_id },
      process.env.REACT_APP_JWT_SECRET
    );
    axios.defaults.headers.common["token"] = idToken;
    axios.defaults.timeout.toFixed(0);
    history.push({
      pathname: process.env.REACT_APP_LOCATION_URL,
      state: {
        token: token,
        page: process.env.REACT_APP_PAGE_LOCATION,
        path: process.env.REACT_APP_LOCATION_URL,
      },
    });
  };

  handleUserLoginError = (e) => {
    this.setState({ userLoginError: e });
  };

  UserLoginRequest = async (email, password) => {
    console.log("UserAccess GET login");
    var access_token = jwt.encode(
      { email: email, password: password },
      process.env.REACT_APP_JWT_SECRET
    );
    const url =
      process.env.REACT_APP_BACKEND_URL +
      "/user/login?access_token=" +
      access_token;
    try {
      let res = await axios.get(url);
      const tokenId = res.data;
      if (tokenId !== false) {
        this.loginRedirect(tokenId);
      } else {
        this.handleUserLoginError(true);
      }
    } catch (err) {
      console.warn(err);
    }
  };

  userLogoutRequest = async () => {
    console.log("UserAccess POST logout");
    try {
      const url = process.env.REACT_APP_BACKEND_URL + "/user/logout";
      axios.post(url).then(this.handleLogOut());
    } catch (err) {
      console.warn(err);
    }
  };

  changeUserSingUpCredentials = () => {
    this.setState({ duplicateUserError: false });
  };

  UserRegistrationRequest = async (email, password, name, surname) => {
    console.log("UserAccess POST registration");
    var access_token = jwt.encode(
      { email: email, password: password, name: name, surname: surname },
      process.env.REACT_APP_JWT_SECRET
    );
    const url =
      process.env.REACT_APP_BACKEND_URL +
      "/user/registration?access_token=" +
      access_token;
    try {
      let res = await axios.get(url);
      const tokenId = res.data;
      if (tokenId !== false) {
        this.loginRedirect(tokenId);
      } else {
        this.setState({ duplicateUserError: true });
      }
    } catch (err) {
      console.warn(err);
    }
  };

  //GET BY USER
  getElements = async () => {
    try {
      if (this.state.routeUrl === process.env.REACT_APP_RULES_URL) {
        console.log("App GET Rules");
        const url = process.env.REACT_APP_BACKEND_URL + "/rule/user";
        let res = await axios.get(url);
        this.setState({ rules: res.data });
      } else if (this.state.routeUrl === process.env.REACT_APP_DEVICES_URL) {
        console.log("App GET Antecedents");
        const url = process.env.REACT_APP_BACKEND_URL + "/device/get/devices";
        let res = await axios.get(url);
        this.setState({ devices: res.data });
      }
    } catch (err) {
      console.warn(err);
    }
  };

  //GET BY ID
  getDeviceById = async (deviceId) => {
    console.log("App GET device By Id");
    const url = process.env.REACT_APP_BACKEND_URL + "/device/get/" + deviceId;
    try {
      let res = await axios.get(url);
      if (this.state.routeUrl === process.env.REACT_APP_DEVICES_URL) {
        this.setState({ device: res.data });
      } else if (this.state.routeUrl === process.env.REACT_APP_SWITCHES_URL) {
        this.setState({ device: res.data });
      }
    } catch (err) {
      console.warn(err);
    }
  };

  getRuleById = async (ruleId) => {
    console.log("App GET Rule By ID");
    const url = process.env.REACT_APP_BACKEND_URL + "/rule/id/" + ruleId;
    try {
      let res = await axios.get(url);
      this.setState({ rule: res.data });
    } catch (err) {
      console.warn(err);
    }
  };

  getRuleAntecedentById = async (ruleElementId) => {
    console.log("App GET Rule Antecedent");
    const url =
      process.env.REACT_APP_BACKEND_URL +
      "/rule/get/antecedent/" +
      this.state.newRuleId +
      "/" +
      ruleElementId;
    try {
      let res = await axios.get(url);
      this.setState({ ruleElement: res.data });
    } catch (err) {
      console.warn(err);
    }
  };
  getRuleConsequentById = async (ruleElementId) => {
    console.log("App GET Rule Consequent");
    const url =
      process.env.REACT_APP_BACKEND_URL +
      "/rule/get/consequent/" +
      this.state.newRuleId +
      "/" +
      ruleElementId;
    try {
      let res = await axios.get(url);
      this.setState({ ruleElement: res.data });
    } catch (err) {
      console.warn(err);
    }
  };

  getRuleElementMeasure = async (deviceId) => {
    console.log("App GET Device Measure");
    const url =
      process.env.REACT_APP_BACKEND_URL + "/device/get/measure/" + deviceId;
    try {
      let res = await axios.get(url);
      if (res.data.measure !== "false") {
        let ruleElement = this.state.ruleElement;
        ruleElement.measure = res.data.measure;
        this.setState({ ruleElement: ruleElement });
      }
    } catch (err) {
      console.warn(err);
    }
  };

  //ADD and UPDATE ELEMENTS
  createRuleRequest = async (rule_name) => {
    console.log("App POST create rule");
    const url = process.env.REACT_APP_BACKEND_URL + "/rule/create/" + rule_name;
    try {
      let res = await axios.post(url);
      const newRule = res.data;
      const ruleId = newRule.id;
      var rules = this.state.rules;
      var idx = rules.length;
      const newRulesList = rules.concat(newRule);
      this.setState(
        {
          rules: newRulesList,
          newRuleIdx: idx,
          newRuleId: ruleId,
          newRuleName: rule_name,
          rule: newRule,
        },
        () => {
          this.handleModify(true);
          this.handleAddRulePopUp();
        }
      );
    } catch (err) {
      console.warn(err);
    }
  };
  addNewRuleAntecedentRequest = async (ruleElementId) => {
    console.log("App POST add new rule element");
    const url = process.env.REACT_APP_BACKEND_URL + "/rule/add/antecedent";
    try {
      let res = await axios.post(
        url,
        {},
        {
          params: {
            rule_id: this.state.newRuleId,
            device_id: ruleElementId,
          },
        }
      );
      const rule = res.data;
      this.setState({ rule: rule }, () => {
        this.getRuleAntecedentById(ruleElementId);
      });
    } catch (err) {
      console.warn(err);
    }
  };
  addNewRuleAConsequentRequest = async (ruleElementId) => {
    console.log("App POST add new rule element");
    const url = process.env.REACT_APP_BACKEND_URL + "/rule/add/consequent";
    try {
      let res = await axios.post(
        url,
        {},
        {
          params: {
            rule_id: this.state.newRuleId,
            device_id: ruleElementId,
          },
        }
      );
      const rule = res.data;
      this.setState({ rule: rule }, () => {
        this.getRuleConsequentById(ruleElementId);
      });
    } catch (err) {
      console.warn(err);
    }
  };
  updateRuleElementRequest = async (ruleId, ruleElementId) => {
    var url = "";
    if (this.state.setRuleAntecedent) {
      console.log("App POST update rule antecedent");
      url =
        process.env.REACT_APP_BACKEND_URL +
        "/rule/update/antecedent/" +
        ruleId +
        "/" +
        ruleElementId;
    } else {
      console.log("App POST update rule consequent");
      url =
        process.env.REACT_APP_BACKEND_URL +
        "/rule/update/consequent/" +
        ruleId +
        "/" +
        ruleElementId;
    }
    try {
      const ruleElement = this.state.ruleElement;
      const ruleElementJson = JSON.stringify(ruleElement);
      let res = await axios.post(url, { ruleElement: ruleElementJson });
      this.setState({ rule: res.data });
    } catch (err) {
      console.warn(err);
    }
  };
  updateRuleNameRequest = async (rule) => {
    console.log("App POST update rule name");
    const url = process.env.REACT_APP_BACKEND_URL + "/rule/set/name";
    try {
      axios
        .post(
          url,
          {},
          {
            params: {
              rule_id: rule.id,
              rule_name: rule.name,
            },
          }
        )
        .then(this.getElements());
    } catch (err) {
      console.warn(err);
    }
  };
  updateDeviceRequest = async (deviceId) => {
    console.log("App POST update device");
    const url =
      process.env.REACT_APP_BACKEND_URL + "/device/update/" + deviceId;
    try {
      var device = JSON.stringify(this.state.device);
      axios.post(url, { data: device }).then(this.getElements());
    } catch (err) {
      console.warn(err);
    }
  };
  updateRuleConsequentOrderRequest = async (orderedElementsId) => {
    console.log("App POST update consequent order");
    const url =
      process.env.REACT_APP_BACKEND_URL + "/rule/update/consequents/order";
    try {
      const orderedElementsIdJson = JSON.stringify(orderedElementsId);
      let res = await axios.post(
        url,
        { data: orderedElementsIdJson },
        {
          params: {
            rule_id: this.state.rule.id,
          },
        }
      );
      this.setState({ rule: res.data });
    } catch (err) {
      console.warn(err);
    }
  };
  updateRuleConsequentOrderLocal = (items) => {
    var rule = this.state.rule;
    rule.rule_consequents = items;
    this.setState({ rule: rule });
  };
  registerDeviceRequest = async (newDeviceId) => {
    console.log("App POST register device");
    const url =
      process.env.REACT_APP_BACKEND_URL + "/device/register/" + newDeviceId;
    setTimeout(() => {
      this.setState({ loading: false });
      this.handleRegisterDevicePopUp();
      this.getElements();
    }, 11000);
    try {
      let res = await axios.post(url);
      const result = res.data;
      if (result !== false) {
        this.setState({ loading: true });
        setTimeout();
      } else {
        this.handleRegisterElementError(true);
      }
    } catch (err) {
      console.warn(err);
    }
  };
  setConsequentAutomaticRequest = async (checked) => {
    console.log("App POST consequent automatic");
    const url =
      process.env.REACT_APP_BACKEND_URL + "/device/consequent/automatic";
    var automatic = "false";
    if (checked === true) {
      automatic = "true";
    }
    try {
      await axios.post(
        url,
        {},
        {
          params: {
            device_id: this.state.deviceId,
            automatic: automatic,
          },
        }
      );
      var device = this.state.device;
      device.automatic = automatic;
      this.setState({ device: device });
    } catch (err) {
      console.warn(err);
    }
  };
  setConsequentManualMeasureRequest = async (manual_measure) => {
    console.log("App POST consequent manual measure");
    const url = process.env.REACT_APP_BACKEND_URL + "/device/consequent/manual";
    try {
      let res = await axios.post(
        url,
        {},
        {
          params: {
            device_id: this.state.deviceId,
            manual_measure: manual_measure,
          },
        }
      );
      this.setState({ device: res.data });
    } catch (err) {
      console.warn(err);
    }
  };
  modifyEmailRequest = async (email, idx) => {
    console.log("App Modify email alert");
    const url =
      process.env.REACT_APP_BACKEND_URL +
      "/device/alert/modify/" +
      email +
      "/" +
      idx;
    try {
      await axios.post(url);
      var device = this.state.device;
      device.email_list[idx] = email;
      this.setState({
        device: device,
      });
    } catch (err) {
      console.warn(err);
    }
  };
  addNewAlertEmailRequest = async () => {
    console.log("App ADD email alert");
    const url = process.env.REACT_APP_BACKEND_URL + "/device/alert/add/";
    try {
      await axios.post(url);
      this.addEmailLocal();
    } catch (err) {
      console.warn(err);
    }
  };

  // REMOVE ELEMENT FROM ARRAYS
  deleteRuleElementRequest = async (ruleId, deviceId) => {
    var url = "";
    if (this.state.setRuleAntecedent === true) {
      console.log("App DELETE rule antecedent");
      url =
        process.env.REACT_APP_BACKEND_URL +
        "/rule/delete/antecedent/" +
        ruleId +
        "/" +
        deviceId;
    } else {
      console.log("App DELETE rule consequent");
      url =
        process.env.REACT_APP_BACKEND_URL +
        "/rule/delete/consequent/" +
        ruleId +
        "/" +
        deviceId;
    }
    try {
      let res = await axios.delete(url);
      this.setState({ rule: res.data, ruleElement: {} });
    } catch (err) {
      console.warn(err);
    }
  };

  deleteElementRequest = async (elementId) => {
    try {
      if (this.state.routeUrl === process.env.REACT_APP_RULES_URL) {
        const url =
          process.env.REACT_APP_BACKEND_URL + "/rule/delete/" + elementId;
        let res = await axios.delete(url);
        if (res.data === true) {
          this.setState({ newRuleId: "", rule: {} }, () => {
            this.getElements();
          });
        }
      } else if (this.state.routeUrl === process.env.REACT_APP_DEVICES_URL) {
        const url =
          process.env.REACT_APP_BACKEND_URL + "/device/delete/" + elementId;
        let res = await axios.delete(url);
        if (res.data === true) {
          this.setState({ deviceId: "", device: {} }, () => {
            this.getElements();
          });
        }
      }
    } catch (err) {
      console.warn(err);
    }
  };
  removeAlertEmailRequest = async (index) => {
    console.log("App DELETE email alert");
    const url =
      process.env.REACT_APP_BACKEND_URL + "/device/alert/delete/" + index;
    try {
      await axios.delete(url);
      var device = this.state.device;
      device.email_list.splice(index, 1);
      this.setState({
        device: device,
      });
    } catch (err) {
      console.warn(err);
    }
  };

  //MODIFY AND SET RULE LOCAL
  modifyRuleName = (newRuleName) => {
    var rule = this.state.rule;
    rule.name = newRuleName;
    this.setState({
      rule: rule,
    });
  };

  //MODIFY DEVICES LOCAL
  setDeviceAntecedent = (device) => {
    this.setState({ device: device });
  };
  setDeviceConsequent = (device) => {
    this.setState({ device: device });
  };
  addEmailLocal = () => {
    var device = this.state.device;
    device.email_list.push("");
    this.setState(
      {
        device: device,
      },
      () => {
        this.handleModifyAlertEmail(true);
      }
    );
  };
  modifyEmailLocal = (idx, email) => {
    var device = this.state.device;
    device.email_list[idx] = email;
    this.setState({
      device: device,
    });
  };

  // CURRENT SELECTED ITEM VARIABLES

  setRouteUrl = (url) => {
    this.setState(
      { routeUrl: url, deviceId: "", ruleElementId: "", newRuleId: "" }
    );
  };
  setRouteUrlWithRequests = (url) => {
    this.setState(
      { routeUrl: url, deviceId: "", ruleElementId: "", newRuleId: "", searchLocationPopUp: false },
      () => {
        this.getElements();
      }
    );
  };
  setNewRule = (ruleId) => {
    this.setState({
      newRuleId: ruleId,
      routeUrl: process.env.REACT_APP_RULES_URL,
    });
  };
  setDeviceObject = (device) => {
    this.setState({ device: device });
  };
  setNewDeviceId = (deviceId) => {
    this.setState({
      deviceId: deviceId,
    });
  };
  setRuleElementId = (id) => {
    this.setState({ ruleElementId: id });
  };
  setRuleElementObject = (ruleElement) => {
    this.setState({ ruleElement: ruleElement });
  };
  handleMenuPopUp = (event) => {
    this.setState({
      menuPopUp: !this.state.menuPopUp,
    });
  };
  handleAddRulePopUp = () => {
    this.setState({ addRulePopUp: !this.state.addRulePopUp });
  };
  handleSetRuleAntecedent = (handle) => {
    this.setState({ setRuleAntecedent: handle, setRuleConsequent: false });
  };
  handleSetRuleConsequent = (handle) => {
    this.setState({ setRuleConsequent: handle, setRuleAntecedent: false });
  };
  handleModify = (state) => {
    this.setState({ modify: state });
  };
  handleModifyDevice = () => {
    this.setState({ modifyDevice: !this.state.modifyDevice });
  };
  handleRegisterDevicePopUp = () => {
    this.setState({ registerDevicePopUp: !this.state.registerDevicePopUp });
    this.handleRegisterElementError(false);
  };

  handleLogOut = () => {
    console.log("logout");
    this.setState({
      menuPopUp: false,
      antecedentId: "",
      consequentId: "",
      newRuleId: "",
    });
    window.location.assign("http://ruleapp.org");
  };
  handleSettings = () => {
    this.setState({
      menuPopUp: false,
      antecedentId: "",
      consequentId: "",
      newRuleId: "",
      settingsPage: process.env.REACT_APP_PAGE_SETTINGS,
    });
  };
  handleModifyAlertEmail = (state) => {
    this.setState({ modifyAlertEmail: state });
  };

  handleRuleBody = (page) => {
    this.setState({
      ruleBody: page,
      setRuleAntecedent: false,
      setRuleConsequent: false,
    });
  };
  AntecedentRulePopUpBody = () => {
    this.setState({ ruleBody: false });
  };
  ConsequentRulePopUpBody = () => {
    this.setState({ ruleBody: true });
  };

  handleRegisterElementError = (error) => {
    this.setState({ registerElementError: error });
  };

  setSettingsPage = (page) => {
    this.setState({ settingsPage: page });
  };

  // GEOGRAPHIC LOCATION
  getLocationByUserId = async () => {
    console.log("App GET user location");
    const url = process.env.REACT_APP_BACKEND_URL + "/user/location";
    try {
      let res = await axios.get(url);
      const location = res.data;
      this.setState({
        locationName: location.name,
        locationCountry: location.country,
        locationLat: location.lat,
        locationLon: location.lon,
      });
    } catch (err) {
      console.warn(err);
    }
  };
  getCurrentLocation = async (name) => {
    console.log("App GET current location");
    const url =
      process.env.REACT_APP_BACKEND_URL + "/user/search/location/" + name;
    try {
      let res = await axios.get(url);
      const locationList = res.data;
      console.log(locationList);
      if (locationList.constructor === Array) {
        this.setState({
          locationList: locationList,
        });
      } else {
        this.setState({
          locationList: [],
        });
      }
    } catch (err) {
      console.warn(err);
    }
  };
  setNewUserLocation = async (name, country, lat, lon) => {
    console.log("App POST set new user location");
    const url = process.env.REACT_APP_BACKEND_URL + "/user/set/location";
    try {
      await axios.post(
        url,
        {},
        {
          params: {
            name: name,
            country: country,
            lat: lat,
            lon: lon,
          },
        }
      );
      this.setState({
        locationName: name,
        locationCountry: country,
        locationLat: lat,
        locationLon: lon,
      });
    } catch (err) {
      console.warn(err);
    }
  };
  getWeather = async () => {
    console.log("App GET weather");
    const url = process.env.REACT_APP_BACKEND_URL + "/user/get/weather";
    try {
      let res = await axios.get(url);
      console.log(res.data);
    } catch (err) {
      console.warn(err);
    }
  };

  handleSearchNewLocation = () =>{
      this.setState({searchLocationPopUp: !this.state.searchLocationPopUp})
  }

  handleFilterDevicesList = (filter) =>{
    this.setState({devicesFilter: filter})
  }

  render() {
    return (
      <Router history={history}>
        <Route
          exact
          path={process.env.REACT_APP_LOCATION_URL}
          render={(props) => (
            <LocationPage
              {...props}
              {...this.state}
              handleMenuPopUp={this.handleMenuPopUp}
              UserRegistrationRequest={this.UserRegistrationRequest}
              handleSettings={this.handleSettings}
              handleLogOut={this.handleLogOut}
              userLogoutRequest={this.userLogoutRequest}
              addNewElement={false}
              handleRegisterDevicePopUp={this.handleRegisterDevicePopUp}
              setSettingsPage={this.setSettingsPage}
              getCurrentLocation={this.getCurrentLocation}
              getLocationByUserId={this.getLocationByUserId}
              setNewUserLocation={this.setNewUserLocation}
              setRouteUrl={this.setRouteUrl}
              handleSearchNewLocation={this.handleSearchNewLocation}
              setRouteUrlWithRequests={this.setRouteUrlWithRequests}
            />
          )}
        />
        <Route
          exact
          path={process.env.REACT_APP_DEVICES_URL}
          render={(props) => (
            <DevicesPage
              {...props}
              {...this.state}
              handleMenuPopUp={this.handleMenuPopUp}
              handleLogOut={this.handleLogOut}
              userLogoutRequest={this.userLogoutRequest}
              element={this.state.device}
              elements={this.state.devices}
              elementId={this.state.deviceId}
              elementType={"antecedent"}
              setNewElement={this.setNewDeviceId}
              getElements={this.getElements}
              getElementById={this.getDeviceById}
              addNewElement={this.state.registerDevicePopUp}
              handleRegisterDevicePopUp={this.handleRegisterDevicePopUp}
              modify={this.state.modifyDevice}
              setNewRule={this.setNewRule}
              setDeviceElement={this.setDeviceAntecedent}
              handleModifyDevice={this.handleModifyDevice}
              deleteElementRequest={this.deleteElementRequest}
              updateDeviceRequest={this.updateDeviceRequest}
              getAntecedentById={this.getDeviceById}
              getRuleById={this.getRuleById}
              registerDeviceRequest={this.registerDeviceRequest}
              handleRegisterElementError={this.handleRegisterElementError}
              AntecedentRulePopUpBody={this.AntecedentRulePopUpBody}
              handleSettings={this.handleSettings}
              handleRuleBody={this.handleRuleBody}
              setRouteUrl={this.setRouteUrl}
              setDeviceAntecedentObject={this.setDeviceObject}
              getWeather={this.getWeather}
              setConsequentAutomaticRequest={this.setConsequentAutomaticRequest}
              setConsequentManualMeasureRequest={
                this.setConsequentManualMeasureRequest
              }
              removeAlertEmailRequest={this.removeAlertEmailRequest}
              handleModifyAlertEmail={this.handleModifyAlertEmail}
              getConsequentById={this.getDeviceById}
              addEmailLocal={this.addEmailLocal}
              addNewAlertEmailRequest={this.addNewAlertEmailRequest}
              modifyEmailRequest={this.modifyEmailRequest}
              modifyEmailLocal={this.modifyEmailLocal}
              setRouteUrlWithRequests={this.setRouteUrlWithRequests}
              handleFilterDevicesList={this.handleFilterDevicesList}
              handleSearchNewLocation={this.handleSearchNewLocation}
            />
          )}
        />
        <Route
          exact
          path={process.env.REACT_APP_RULES_URL}
          render={(props) => (
            <RulesPage
              {...props}
              {...this.state}
              handleMenuPopUp={this.handleMenuPopUp}
              handleLogOut={this.handleLogOut}
              userLogoutRequest={this.userLogoutRequest}
              element={this.state.rule}
              elements={this.state.rules}
              elementId={this.state.newRuleId}
              addNewElement={this.state.addRulePopUp}
              handleRegisterDevicePopUp={this.handleAddRulePopUp}
              modifyElementName={this.modifyRuleName}
              modify={this.state.modify}
              setNewElement={this.setNewRule}
              getElements={this.getElements}
              getElementById={this.getRuleById}
              deleteRuleElementRequest={this.deleteRuleElementRequest}
              handleModify={this.handleModify}
              updateRuleElementRequest={this.updateRuleElementRequest}
              deleteElementRequest={this.deleteElementRequest}
              modifyRuleName={this.modifyRuleName}
              getRuleById={this.getRuleById}
              handleSetRuleAntecedent={this.handleSetRuleAntecedent}
              handleSetRuleConsequent={this.handleSetRuleConsequent}
              setRuleElement={this.setRuleElementId}
              setRuleElementObject={this.setRuleElementObject}
              addNewRuleAntecedentRequest={this.addNewRuleAntecedentRequest}
              addNewRuleAConsequentRequest={this.addNewRuleAConsequentRequest}
              getRuleAntecedentById={this.getRuleAntecedentById}
              getRuleConsequentById={this.getRuleConsequentById}
              updateRuleNameRequest={this.updateRuleNameRequest}
              updateRuleConsequentOrderRequest={
                this.updateRuleConsequentOrderRequest
              }
              updateRuleConsequentOrderLocal={
                this.updateRuleConsequentOrderLocal
              }
              getRuleElementMeasure={this.getRuleElementMeasure}
              base_url={process.env.REACT_APP_BACKEND_URL}
              handleAddRulePopUp={this.handleAddRulePopUp}
              createRuleRequest={this.createRuleRequest}
              AntecedentRulePopUpBody={this.AntecedentRulePopUpBody}
              ConsequentRulePopUpBody={this.ConsequentRulePopUpBody}
              handleSettings={this.handleSettings}
              handleRuleBody={this.handleRuleBody}
              setRouteUrl={this.setRouteUrl}
              setRouteUrlWithRequests={this.setRouteUrlWithRequests}
              handleSearchNewLocation={this.handleSearchNewLocation}
            />
          )}
        />
        <Route
          exact
          path={process.env.REACT_APP_LOGIN_URL}
          render={(props) => (
            <Login
              {...props}
              {...this.state}
              handleMenuPopUp={this.handleMenuPopUp}
              handleUserLoginError={this.handleUserLoginError}
              UserLoginRequest={this.UserLoginRequest}
            />
          )}
        />
        <Route
          exact
          path={process.env.REACT_APP_SINGUP_URL}
          render={(props) => (
            <SingUp
              {...props}
              {...this.state}
              handleMenuPopUp={this.handleMenuPopUp}
              UserRegistrationRequest={this.UserRegistrationRequest}
              changeUserSingUpCredentials={this.changeUserSingUpCredentials}
            />
          )}
        />
        <Route
          exact
          path={process.env.REACT_APP_SETTINGS_URL}
          render={(props) => (
            <SettingPage
              {...props}
              {...this.state}
              handleMenuPopUp={this.handleMenuPopUp}
              UserRegistrationRequest={this.UserRegistrationRequest}
              handleSettings={this.handleSettings}
              handleLogOut={this.handleLogOut}
              userLogoutRequest={this.userLogoutRequest}
              addNewElement={false}
              handleRegisterDevicePopUp={this.handleRegisterDevicePopUp}
              setSettingsPage={this.setSettingsPage}
              getCurrentLocation={this.getCurrentLocation}
              getLocationByUserId={this.getLocationByUserId}
              setNewUserLocation={this.setNewUserLocation}
              setRouteUrl={this.setRouteUrl}
            />
          )}
        />
      </Router>
    );
  }
}
