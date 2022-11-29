#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// variables of INTERNET CONNECTION
//===========================
IPAddress mqtt_broker(192, 168, 1, 9);
const int broker_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
const char* ssid = "TIM-33230256";
const char* password = "xEQApPEPtdKA2hu44AY727Q6";
const String clientID = "00105";
unsigned long lastConnect_mqtt = 0;
unsigned long T_reconnect_MQTT = 0;

//SENSOR INFO
//==============================
const String WaterLevel = "WATERLEVEL-";
const String Photocell = "PHOTOCELL-";
const String Switch = "SWITCH-";
const String SoilMoisture = "SOILMOISTURE-";
const String Button = "BUTTON-";
int MY_number_sensors = 0;
struct SensorInfo {
  String Name;
  String measure;
  long timestamp;
  long measure_refresh;
  int measure_stability;
  bool flag;
  long expiration;
  long measure_delay;
};
struct SensorInfo Sensors[3];
unsigned long now = 0;
long mqtt_keep_alive = 10 * 1000;
long measure_refresh = 1 * 1000;
int measure_resolution = 1;
int measure_stability = 0;
long lastReconnectAttempt = 0;
int tryInternetReconnect = 0;


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  //
  WiFi.begin(ssid, password);
  WIFI_CONNECT();
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
  SWITCH();
  BUTTON();
  Serial.println("DEVICE INITILIZED!");
}

void loop() {
  now = millis();
  client.loop();
  reconnect();
  SWITCH();
  BUTTON();
}

void WIFI_CONNECT() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte *payload, unsigned int length) {
  int i = 0;
  String topic_info[4] = {};
  message_split(const_cast<char*>(topic), &topic_info[0]);
  String argument_topic = topic_info[0];
  String S_name = topic_info[1];
  String msgIn;
  for (int j = 0; j < length; j++) {
    msgIn = msgIn + char(payload[j]);
  }
  Serial.println("message for Sensor: " + S_name + " with topic: " + argument_topic + " with payload: " + msgIn);

  //_________________________________
  for (byte j = 0; j < MY_number_sensors; j++) {
    if (S_name == Sensors[j].Name) {
      if (argument_topic == "switch") {
        String message_info[4] = {};
        message_split(const_cast<char*>(msgIn.c_str()), &message_info[0]);
        Sensors[j].measure = message_info[0];
        Sensors[j].measure_delay = (message_info[1].toInt() * 1000) + now;
      }
      else if (argument_topic == "expiration") {
        Sensors[j].expiration = msgIn.toInt() * 1000;
      }
      break;
    }
  }
}

void message_split(char* msgCopy, String *msg_info) {
  unsigned int i = 0;
  char* token;
  while ((token = strtok_r(msgCopy, "/", &msgCopy)) != NULL) {
    msg_info[i] = String(token);
    i = i + 1;
  }
}

void BUTTON() {
  String S_name = Button + clientID;
  int pin_BUTTON = 14;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].expiration) {
        Sensors[i].flag = true;
      }

      // DUTY-CICLE ----------------------------------
      int val = digitalRead(pin_BUTTON);
      String measure = "off";
      if (val == 1) {
        measure = "on";
      }
      if (Sensors[i].measure != measure) {
        Sensors[i].measure = measure;
        Sensors[i].flag = true;
      }

      // PUBLISH-CICLE ----------------------------------
      if (Sensors[i].flag == true) {
        MQTT_PUBLISH(i);
      }
      break;
    }
  }
  // SETUP -----------------------------------------
  if (flag_syncro == 0) {
    Sensors[MY_number_sensors] = {S_name, "off", now, now, 0, true, mqtt_keep_alive, now};
    MY_number_sensors += 1;
    pinMode(pin_BUTTON, INPUT);
    Serial.println("setup device " + S_name);
  }
}

void SWITCH() {
  String S_name = Switch + clientID;
  int pin_SWITCH = 12;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].expiration) {
        Sensors[i].flag = true;
      }

      // DUTY-CICLE ----------------------------------
      if (now > Sensors[i].measure_delay) {
        int val = digitalRead(pin_SWITCH);
        String switch_status = "off";
        if (val == 1) {
          switch_status = "on";
        }
        if (Sensors[i].measure != switch_status) {
          Sensors[i].flag = true;
          if (Sensors[i].measure == "on") {
            digitalWrite(pin_SWITCH, HIGH);
          }
          else {
            digitalWrite(pin_SWITCH, LOW);
          }
        }
      }
      // PUBLISH-CICLE ----------------------------------
      if (Sensors[i].flag == true) {
        MQTT_PUBLISH(i);
      }
      break;
    }
  }
  // SETUP -----------------------------------------
  if (flag_syncro == 0) {
    Sensors[MY_number_sensors] = {S_name, "off", now, now, 0, true, mqtt_keep_alive, now};
    MY_number_sensors += 1;
    pinMode(pin_SWITCH, OUTPUT);
    digitalWrite(pin_SWITCH, LOW);
    Serial.println("setup device " + S_name);
  }
}

void PHOTOCELL() {
  String S_name = Photocell + clientID;
  const int pin_SENSOR = A0;
  int flag_syncro = 0;
  int measure_resolution_photocell = 10;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].expiration) {
        Sensors[i].flag = true;
      }
      int measure;
      //DUTY-CICLE--------------------------------------
      if (now - Sensors[i].measure_refresh > measure_refresh || Sensors[i].flag == true) {
        Sensors[i].measure_refresh = now;
        measure = analogRead(pin_SENSOR);
        delay(5);
        if ( abs(measure - Sensors[i].measure.toInt()) > measure_resolution_photocell) {
          Sensors[i].measure_stability += 1;
        }
        else {
          Sensors[i].measure_stability = 0;
        }
      }
      if (Sensors[i].measure_stability > measure_stability) {
        Sensors[i].flag = true;
        Sensors[i].measure_stability = 0;
      }
      //PUBLISH-CICLE-----------------------------------------
      if (Sensors[i].flag == true) {
        Sensors[i].measure = String(measure);
        MQTT_PUBLISH(i);
      }
      break;
    }
  }
  // SETUP -----------------------------------------
  if (flag_syncro == 0) {
    Sensors[MY_number_sensors] = {S_name, "0", now, now, 0, true, mqtt_keep_alive, now};
    MY_number_sensors += 1;
    Serial.println("setup device " + S_name);
  }
}

void SOIL_MOISTURE() {
  String S_name = SoilMoisture + clientID;
  const int pin_SENSOR = A0;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].expiration) {
        Sensors[i].flag = true;
      }
      //DUTY-CICLE------------------------------------------
      int measure;
      if (now - Sensors[i].measure_refresh > measure_refresh || Sensors[i].flag == true) {
        Sensors[i].measure_refresh = now;
        measure = analogRead(pin_SENSOR);
        delay(5);
        if ( abs(measure - Sensors[i].measure.toInt()) > measure_resolution) {
          Sensors[i].measure_stability += 1;
        }
      }
      if (Sensors[i].measure_stability > measure_stability) {
        Sensors[i].flag = true;
        Sensors[i].measure_stability = 0;
      }
      //PUBLISH-CICLE-----------------------------------------
      if (Sensors[i].flag == true) {
        Sensors[i].measure = String(measure);
        MQTT_PUBLISH(i);
      }
      break;
    }
  }
  // SETUP -----------------------------------------
  if (flag_syncro == 0) {
    Sensors[MY_number_sensors] = {S_name, "0", now, now, 0, true, mqtt_keep_alive, now};
    MY_number_sensors += 1;
    Serial.println("setup device " + S_name);
  }
}


void WATER_LEVEL() {
  String S_name = WaterLevel + clientID;
  const int trigPin = 13;
  const int echoPin = 12;
  int flag_syncro = 0;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = 1;
      // REFRESH-CICLE ---------------------
      if (now - Sensors[i].timestamp > Sensors[i].expiration) {
        Sensors[i].flag = true;
      }
      //DUTY-CICLE--------------------------
      String measure;
      if (now - Sensors[i].measure_refresh > measure_refresh || Sensors[i].flag == true) {
        Sensors[i].measure_refresh = now;
        delay(5);
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(trigPin, LOW);
        long duration = pulseIn(echoPin, HIGH, 26000);
        float distance = float(duration * 0.034) / 2.0;
        measure = String(distance, 2);
        if ( abs(measure.toFloat() - Sensors[i].measure.toFloat()) > float(measure_resolution) ) {
          Sensors[i].measure_stability += 1;
        }
      }
      if (Sensors[i].measure_stability > measure_stability) {
        Sensors[i].flag = true;
        Sensors[i].measure_stability = 0;
      }
      //PUBLISH-CICLE----------------------------
      if (Sensors[i].flag == true) {
        Sensors[i].measure = measure;
        MQTT_PUBLISH(i);
      }
      break;
    }
  }
  // SETUP -----------------------------------------
  if (flag_syncro == 0) {
    Sensors[MY_number_sensors] = {S_name, "0", now, now, 0, true, mqtt_keep_alive, now};
    MY_number_sensors += 1;
    Serial.println("setup device " + S_name);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
    digitalWrite(echoPin, LOW);
  }
}


void MQTT_PUBLISH(byte i) {
  if (client.connected()) {
    Sensors[i].flag = false;
    Sensors[i].timestamp = now;
    String topic = "device/" + Sensors[i].Name;
    String message = String(Sensors[i].expiration / 1000) + "/" + Sensors[i].measure;
    client.publish(const_cast<char*>(topic.c_str()), const_cast<char*>(message.c_str()));
    Serial.println("send sensor messange " + Sensors[i].measure);
  }
}


void reconnect() {
  if (!client.connected()) {
    if (now - lastReconnectAttempt > 5000) {
      if (client.connect(const_cast<char*>(clientID.c_str()))) {
        Serial.println("connected");
        lastReconnectAttempt = 0;
        tryInternetReconnect = 0;
        deviceSubscribe();
      } else {
        lastReconnectAttempt = now;
        tryInternetReconnect++;
        switchOff();
        Serial.print("failed, wifi connection=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
    if (tryInternetReconnect > 3) {
      Serial.println("Power restart");
      ESP.restart();
    }
  }
}

void switchOff() {
  if (MY_number_sensors > 0) {
    for (byte j = 0; j < MY_number_sensors; j++) {
      String Name = Sensors[j].Name;
      if (Name.startsWith("SWITCH")) {
        Sensors[j].measure = "off";
      }
    }
  }
}

void deviceSubscribe() {
  Serial.println(String(MY_number_sensors));
  if (MY_number_sensors > 0) {
    for (byte j = 0; j < MY_number_sensors; j++) {
      String Name = Sensors[j].Name;
      String expiration_topic = "expiration/" + Name;
      client.subscribe(const_cast<char*>(expiration_topic.c_str()));
      delay(5);
      if (Name.startsWith("SWITCH")) {
        String switch_topic = "switch/" + Name;
        client.subscribe(const_cast<char*>(switch_topic.c_str()));
        delay(5);
      }
      Serial.println(Name + " subscribed");
    }
  }
}
