const int STATE_OTHER = 0;
const int STATE_NO_MOVEMENT = 1;
const int STATE_MOVEMENT = 2;
const int STATE_OPEN_NO_MOVEMENT = 3;
const int STATE_OPEN_MOVEMENT = 4;
const int STATE_WIRE_CUT = 5;

int getState(int aValue)
{
  if ((aValue >= 360) && (aValue <= 369))
  {
    return STATE_NO_MOVEMENT;
  }
  else if ((aValue >= 490) && (aValue <= 499))
  {
    return STATE_MOVEMENT;
  }
  else if ((aValue >= 530) && (aValue <= 540))
  {
    return STATE_OPEN_MOVEMENT;
  }
  else if ((aValue >= 470) && (aValue <= 476))
  {
    return STATE_OPEN_NO_MOVEMENT;
  }
  else if ((aValue >= 660) && (aValue <= 671))
  {
    return STATE_WIRE_CUT;
  }
  else
  {
    return STATE_OTHER;
  }
}

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  int sensorValue = analogRead(A0);
  int aState = getState(sensorValue);
  
  switch (aState)
  {
    case STATE_NO_MOVEMENT:
      Serial.println("No Movement");
      break;
    case STATE_MOVEMENT:
      Serial.println("Movement");
      break;
    case STATE_OPEN_NO_MOVEMENT:
      Serial.println("Case Open - No Movement");
      break;
    case STATE_OPEN_MOVEMENT:
      Serial.println("Case Open - Movement");
      break;
    case STATE_WIRE_CUT:
      Serial.println("Wire Cut");
      break;
    case STATE_OTHER:
      Serial.print("Error - ");
      Serial.println(sensorValue);
      break;
  }
  
  delay(200);        
}
