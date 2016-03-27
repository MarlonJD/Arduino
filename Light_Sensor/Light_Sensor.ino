const int STATE_WHITE            = 0;
const int STATE_VERY_BRIGHT      = 1;
const int STATE_BRIGHT           = 2;
const int STATE_LIGHT            = 3;
const int STATE_DIM_LIGHT        = 4;
const int STATE_DIM              = 5;
const int STATE_DIM_DARK         = 6;
const int STATE_DARK             = 7;
const int STATE_VERY_DARK        = 8;

int getState(int aValue)
{
  return (int(aValue/128));
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
    case STATE_WHITE:
      Serial.println("White");
      break;
    case STATE_VERY_BRIGHT:
      Serial.println("Very Bright");
      break;
    case STATE_BRIGHT:
      Serial.println("Bright");
      break;
    case STATE_LIGHT:
      Serial.println("Light");
      break;
    case STATE_DIM_LIGHT:
      Serial.println("Dim Light");
      break;
    case STATE_DIM:
      Serial.println("Dim");
      break;
    case STATE_DIM_DARK:
      Serial.println("Dim Dark");
      break;
    case STATE_DARK:
      Serial.println("Dark");
      break;
    case STATE_VERY_DARK:
      Serial.println("Very Dark");
      break;
  }
  
  delay(200);        
}
