void setup() {
  Serial.begin(9600); //Start Serial Monitor to display current read value on Serial monitor
}

void loop() {
  float AcsValue = 0.0, Samples = 0.0;
  for (int x = 0; x < 100; x++) { 
    AcsValue = analogRead(A0);     
    Samples = Samples + AcsValue;  
    delay (1); 
  }
  int measure = (Samples / 1024.0) + 0.5;

  Serial.println(measure);//Print the read current on Serial monitor
  delay(50);
}
