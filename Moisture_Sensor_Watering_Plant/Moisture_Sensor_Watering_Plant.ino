#define WATERPIN 2
#define READSOILPIN A0

// higher number is more dry
#define MAXDRYNESS 700

#define WATERDELAY 750
#define WATERPOSTDELAY 5000

void setup()
{
  Serial.begin(9600);
  pinMode(READSOILPIN, INPUT);
  pinMode(WATERPIN, OUTPUT);
}

void loop()
{
  int SensorValue = analogRead(READSOILPIN); //take a sample
  Serial.print(SensorValue); Serial.print(" - ");
  
  if(SensorValue >= MAXDRYNESS) 
  {
   // if the soil is too dry start watering for 3/4 a second then
   // wait for 5 seconds before monitoring again
   Serial.println("Soil dry start watering");
   digitalWrite(WATERPIN, HIGH);
   delay(WATERDELAY);
   digitalWrite(WATERPIN, LOW);
   delay(WATERPOSTDELAY);
  }
  
  delay(50);
}

