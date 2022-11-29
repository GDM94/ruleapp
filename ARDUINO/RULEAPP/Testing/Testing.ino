
const int SWITCH_pin = 12;

void setup() {
  // put your setup code here, to run once:
  pinMode(SWITCH_pin, OUTPUT);
  digitalWrite(SWITCH_pin, LOW);
  Serial.println("setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  digitalWrite(SWITCH_pin, HIGH);
  delay(1000);
  digitalWrite(SWITCH_pin, LOW);
}
