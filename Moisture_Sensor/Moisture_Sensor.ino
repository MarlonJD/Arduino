void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop()
{
  int SensorValue = analogRead(A0); //take a sample
  Serial.print(SensorValue); Serial.print(" - ");
  
  if(SensorValue >= 1000) {
   Serial.println("Sensor is not in the Soil or DISCONNECTED");
  }
  if(SensorValue < 1000 && SensorValue >= 600) { 
   Serial.println("Soil is DRY");
  }
  if(SensorValue < 600 && SensorValue >= 370) {
   Serial.println("Soil is HUMID"); 
  }
  if(SensorValue < 370) {
   Serial.println("Sensor in WATER");
  }
  delay(50);
}

