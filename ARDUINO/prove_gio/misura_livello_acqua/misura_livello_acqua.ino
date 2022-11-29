// defines pins numbers
const int trigPin = 16;
const int echoPin = 5;
// defines variables
long duration;
float distance;
int water_level;
float H_max = 20;
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  digitalWrite(trigPin, LOW);
  digitalWrite(echoPin, LOW);
}
void loop() {
  delay(2000);
  WATER_LEVEL();
  delay(200);
}

void WATER_LEVEL() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  float level_perc= distance / H_max;
  water_level = (1 - level_perc)*100;
  Serial.println(distance);
  Serial.println(water_level);
}
