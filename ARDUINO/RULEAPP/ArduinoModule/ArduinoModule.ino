#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Servo.h>

// UNIQUE CLIENT ID TO PERSONALIZE
//=================================
const String clientID = "00101";

// PIN Variables
//==============================
const int SWITCH_pin = 12;
const int BUTTON_pin = 14;
const int PHOTOCELL_pin = A0;
const int SOIL_MOISTURE_pin = A0;const int WATER_LEVEL_trigPin = 13;
const int WATER_LEVEL_echoPin = 12;
const int pinLed = 14;
const int SERVO_pin = 26;
Servo servo;

// variables of INTERNET CONNECTION
//===========================
const char* mqtt_broker = "main.ruleapp.org";
// IPAddress mqtt_broker(93, 51, 10, 117);
const int broker_port = 1883;
byte mac[] = { 0x90, 0xAD  , 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient internetClient;
PubSubClient client(internetClient);
unsigned long lastConnect_mqtt = 0;
unsigned long T_reconnect_MQTT = 0;

//SENSOR INFO
//==============================
const String WaterLevel = "WATERLEVEL-";
const String Photocell = "PHOTOCELL-";
const String Switch = "SWITCH-";
const String SoilMoisture = "SOILMOISTURE-";
const String Button = "BUTTON-";
const String ServoMotor = "SERVO-";
int MY_number_sensors = 0;
struct SensorInfo {
  String Name;
  String measure;
  long timestamp;
  long measure_refresh;
  int measure_stability;
  bool flag;
  long threshold_expiration;
};
struct SensorInfo Sensors[3] = {};
unsigned long now = 0;
long threshold_expiration = 10 * 1000;
long threshold_refresh = 1 * 1000;
int threshold_stability = 2;
int threshold_resolution = 1;
long lastReconnectAttempt = 0;
int tryInternetReconnect = 0;
float minLedBlinkFreq = 1000.0;
unsigned long ledTimestamp = 0;

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
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
  Serial.println("DEVICE INITILIZED!");
}

void loop() {
  now = millis();
  Ethernet.maintain();
  SWITCH(SWITCH_pin);
  ManageMqttConnection();
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
  for (byte j = 0; j < MY_number_sensors; j++) {
    if (S_name == Sensors[j].Name) {
      if (argument_topic == "switch" || argument_topic == "servo") {
        String message_info[4] = {};
        message_split(const_cast<char*>(msgIn.c_str()), &message_info[0]);
        Sensors[j].measure = message_info[0];
        Sensors[j].measure_refresh = (message_info[1].toInt() * 1000) + now;
      }
      else if (argument_topic == "servo") {
        String message_info[4] = {};
        message_split(const_cast<char*>(msgIn.c_str()), &message_info[0]);
        Sensors[j].measure = message_info[0];
        Sensors[j].measure_refresh = (message_info[1].toInt() * 1000) + now;
      }
      else if (argument_topic == "expiration") {
        Sensors[j].threshold_expiration = msgIn.toInt() * 1000;
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

void SWITCH(int pin_SWITCH) {
  String S_name = Switch + String(pin_SWITCH) + "-" + clientID;
  int flag_syncro = -1;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = i;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].threshold_expiration) {
        Sensors[i].flag = true;
      }
      // DUTY-CICLE ----------------------------------
      if (now > Sensors[i].measure_refresh) {
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
  if (flag_syncro == -1) {
    Sensors[MY_number_sensors] = {S_name, "off", now, now, 0, true, threshold_expiration};
    MY_number_sensors += 1;
    pinMode(pin_SWITCH, OUTPUT);
    digitalWrite(pin_SWITCH, LOW);
    Serial.println("setup device " + S_name);
  }
}

void SERVO_MOTOR(int pin_SERVO) {
  String S_name = ServoMotor + String(pin_SERVO) + "-" + clientID;
  int flag_syncro = -1;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = i;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].threshold_expiration) {
        Sensors[i].flag = true;
      }
      // DUTY-CICLE ----------------------------------
      if (now > Sensors[i].measure_refresh) {
        int val = servo.read();
        if (val != Sensors[i].measure.toInt()) {
          servo.write(Sensors[i].measure.toInt());
          Sensors[i].flag = true;
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
  if (flag_syncro == -1) {
    Sensors[MY_number_sensors] = {S_name, "90", now, now, 0, true, threshold_expiration};
    servo.attach(pin_SERVO);
    servo.write(Sensors[MY_number_sensors].measure.toInt());
    MY_number_sensors += 1;
    Serial.println("setup device " + S_name);
  }
}

void BUTTON(int pin_BUTTON) {
  String S_name = Button + String(pin_BUTTON) + "-" + clientID;
  int flag_syncro = -1;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = i;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].threshold_expiration) {
        Sensors[i].flag = true;
      }
      // DUTY-CICLE ----------------------------------
      String measure = "off";
      if (now - Sensors[i].measure_refresh > threshold_refresh || Sensors[i].flag == true) {
        Sensors[i].measure_refresh = now;
        int val = digitalRead(pin_BUTTON);
        if (val == 1) {
          measure = "on";
        }
        if (Sensors[i].measure != measure) {
          Sensors[i].measure_stability += 1;
        }
      }
      if (Sensors[i].measure_stability >= threshold_stability) {
        Sensors[i].measure_stability = 0;
        Sensors[i].flag = true;
      }

      // PUBLISH-CICLE ----------------------------------
      if (Sensors[i].flag == true) {
        Sensors[i].measure = measure;
        MQTT_PUBLISH(i);
      }
      break;
    }
  }
  // SETUP -----------------------------------------
  if (flag_syncro == -1) {
    Sensors[MY_number_sensors] = {S_name, "off", now, now, 0, true, threshold_expiration};
    MY_number_sensors += 1;
    pinMode(pin_BUTTON, INPUT);
    Serial.println("setup device " + S_name);
  }
}

void PHOTOCELL(int pin_SENSOR) {
  String S_name = Photocell + String(pin_SENSOR) + "-" + clientID;
  int flag_syncro = -1;
  int threshold_resolution_photocell = 1;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = i;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].threshold_expiration) {
        Sensors[i].flag = true;
      }
      //DUTY-CICLE--------------------------------------
      int measure;
      if (now - Sensors[i].measure_refresh > threshold_refresh || Sensors[i].flag == true) {
        Sensors[i].measure_refresh = now;
        float AcsValue = 0.0, Samples = 0.0;
        for (int x = 0; x < 100; x++) {
          AcsValue = analogRead(pin_SENSOR);
          Samples = Samples + AcsValue;
          delay (1);
        }
        measure = (Samples / 1024.0) + 0.5;
        if ( abs(measure - Sensors[i].measure.toInt()) > threshold_resolution_photocell) {
          Sensors[i].measure_stability += 1;
        }
      }
      if (Sensors[i].measure_stability >= threshold_stability) {
        Sensors[i].measure_stability = 0;
        Sensors[i].flag = true;
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
  if (flag_syncro == -1) {
    Sensors[MY_number_sensors] = {S_name, "0", now, now, 0, true, threshold_expiration};
    MY_number_sensors += 1;
    pinMode(pin_SENSOR, INPUT);
    pinMode(pinLed, OUTPUT);
    digitalWrite(pinLed, LOW);
    Serial.println("setup device " + S_name);
  }
  // LED MANAGEMENT --------------------------------
  if (client.connected()) {
    int ledBlinkingFrequency = (minLedBlinkFreq - ((Sensors[flag_syncro].measure.toFloat() / 100.0) * minLedBlinkFreq)) + 0.5;
    if (ledBlinkingFrequency < 100) {
      ledBlinkingFrequency = 100;
    }
    if (now - ledTimestamp >= ledBlinkingFrequency) {
      ledTimestamp = now;
      int ledStatus = 0;
      if (digitalRead(pinLed) == 1) {
        digitalWrite(pinLed, LOW);
      }
      else {
        digitalWrite(pinLed, HIGH);
      }
    }
  }
}

void SOIL_MOISTURE(int pin_SENSOR) {
  String S_name = SoilMoisture + String(pin_SENSOR) + "-" + clientID;
  int flag_syncro = -1;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = i;
      // REFRESH-CICLE ----------------------------------
      if (now - Sensors[i].timestamp > Sensors[i].threshold_expiration) {
        Sensors[i].flag = true;
      }
      //DUTY-CICLE------------------------------------------
      int measure;
      if (now - Sensors[i].measure_refresh > threshold_refresh || Sensors[i].flag == true) {
        Sensors[i].measure_refresh = now;
        measure = analogRead(pin_SENSOR);
        delay(5);
        if ( abs(measure - Sensors[i].measure.toInt()) > threshold_resolution) {
          Sensors[i].measure_stability += 1;
        }
      }
      if (Sensors[i].measure_stability >= threshold_stability) {
        Sensors[i].measure_stability = 0;
        Sensors[i].flag = true;
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
  if (flag_syncro == -1) {
    Sensors[MY_number_sensors] = {S_name, "0", now, now, 0, true, threshold_expiration};
    MY_number_sensors += 1;
    Serial.println("setup device " + S_name);
  }
}


void WATER_LEVEL(int trigPin, int echoPin) {
  String S_name = WaterLevel + String(trigPin) + String(echoPin) + "-" + clientID;
  int flag_syncro = -1;
  for (byte i = 0; i < MY_number_sensors; i++) {
    if (Sensors[i].Name == S_name) {
      flag_syncro = i;
      // REFRESH-CICLE ---------------------
      if (now - Sensors[i].timestamp > Sensors[i].threshold_expiration) {
        Sensors[i].flag = true;
      }
      //DUTY-CICLE--------------------------
      String measure;
      if (now - Sensors[i].measure_refresh > threshold_refresh || Sensors[i].flag == true) {
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
        if ( abs(measure.toFloat() - Sensors[i].measure.toFloat()) > float(threshold_resolution) ) {
          Sensors[i].measure_stability += 1;
        }
      }
      if (Sensors[i].measure_stability >= threshold_stability) {
        Sensors[i].measure_stability = 0;
        Sensors[i].flag = true;
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
  if (flag_syncro == -1) {
    Sensors[MY_number_sensors] = {S_name, "0", now, now, 0, true, threshold_expiration};
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
    String message = String(Sensors[i].threshold_expiration / 1000) + "/" + Sensors[i].measure;
    client.publish(const_cast<char*>(topic.c_str()), const_cast<char*>(message.c_str()));
    Serial.println("send sensor messange " + Sensors[i].measure);
  }
}

void(* resetFunc) (void) = 0;

void ManageMqttConnection() {
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
      resetFunc();
      delay(100);
    }
  }
  else {
    client.loop();
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
      if (Name.startsWith("SERVO")) {
        String servo_topic = "servo/" + Name;
        client.subscribe(const_cast<char*>(servo_topic.c_str()));
        delay(5);
      }
      Serial.println(Name + " subscribed");
    }
  }
}
