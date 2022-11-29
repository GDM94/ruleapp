#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>


// variables of INTERNET CONNECTION
//===========================
byte mac[] = { 0x90, 0xAD  , 0xBE, 0xEF, 0xFE, 0xED };
IPAddress catalog_ip(93, 51, 10, 117);
const int broker_port = 1883;
EthernetClient internetClient;
PubSubClient client(internetClient);
const String clientID = "00103";
unsigned long lastConnect_mqtt = 0;
unsigned long T_reconnect_MQTT = 0;

//SENSOR INFO
//==============================
const String WaterLevel = "WATERLEVEL-";
const String Photocell = "PHOTOCELL-";
const String Switch = "SWITCH-";
const String SoilMoisture = "SOILMOISTURE-";
const String Ammeter = "AMMETER-";
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
struct SensorInfo Sensors[3] = {};
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
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }
  }
  delay(1000);
  client.setServer(catalog_ip, 1883);
  client.setCallback(callback);
  AMMETER();
  SWITCH();
  Serial.println("DEVICE INITILIZED!");
}

void loop() {
  now = millis();
  Ethernet.maintain();
  client.loop();
  reconnect();
  AMMETER();
  SWITCH();
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

void SWITCH() {
  String S_name = Switch + clientID;
  int pin_SWITCH = 2;
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
  const int trigPin = 16;
  const int echoPin = 5;
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
        delayMicroseconds(5);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        long duration = pulseIn(echoPin, HIGH);
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

void AMMETER() {
  String S_name = Ammeter + clientID;
  int flag_syncro = 0;
  int measure_resolution_ammeter = 2;
  int measure_stability_ammeter = 2;
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
        measure = power_measure();
        if ( abs(measure - Sensors[i].measure.toInt()) > float(measure_resolution_ammeter)) {
          Sensors[i].measure_stability += 1;
        }
        else {
          Sensors[i].measure_stability = 0;
        }
      }
      if (Sensors[i].measure_stability > measure_stability_ammeter) {
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

int power_measure() {
  int PinGND = A2;  // sensor Virtual Ground
  int PinVClamp = A1;    // Sensor SCT-013-000
  const unsigned int numReadings = 200; //samples to calculate Vrms.
  int readingsVClamp[numReadings];    // samples of the sensor SCT-013-000
  float I = 0;
  float SumSqVClamp = 0;
  float total = 0;
  for (unsigned int i = 0; i < numReadings; i++) {
    readingsVClamp[i] = analogRead(PinVClamp) - analogRead(PinGND);
    delay(1); //
  }
  delay(5);
  //Calculate Vrms
  for (unsigned int i = 0; i < numReadings; i++) {
    SumSqVClamp += sq((float)readingsVClamp[i]);
  }
  total = sqrt(SumSqVClamp / numReadings);
  I = total * float(0.14648);
  float Power_float = total * float(31.187);
  int Power = (int) Power_float;
  if (Power <= 12) { // error management
    Power = 0;
  }
  return Power;
  // Rburden=62 ohms, LBS= 0,004882 V (5/1024)
  // Transformer of 1800 laps (SCT-013-000).
  // 5*220*1800/(62*1024)= 31.187
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
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
    if (tryInternetReconnect > 3) {
      Serial.println("Power restart");
    }
  }
}

void switchOff() {
  Serial.println("all switches OFF");
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
