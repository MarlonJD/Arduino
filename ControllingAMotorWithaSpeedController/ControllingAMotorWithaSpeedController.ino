int MotorEnable  = 4; 
int MotorDir1PWM = 5;     
int MotorDir2PWM = 6;

//------------------------------------------------------------------------
void SetMotor(int aDirection, int aSpeed)
{
  if (aDirection == 0)
  {
    analogWrite(MotorDir1PWM, aSpeed); 
    analogWrite(MotorDir2PWM, 0); 
    digitalWrite(MotorEnable, HIGH);
  }
  else
  {
    analogWrite(MotorDir1PWM, 0); 
    analogWrite(MotorDir2PWM, aSpeed); 
    digitalWrite(MotorEnable, HIGH);
  }
}

//------------------------------------------------------------------------
void setup()
{
  pinMode(MotorEnable, OUTPUT);   
  pinMode(MotorDir1PWM, OUTPUT);   
  pinMode(MotorDir2PWM, OUTPUT); 
}

//------------------------------------------------------------------------
void loop()
{
  // Direction 1
  for(int speedindex = 0; speedindex <= 255; speedindex++)
  {
     SetMotor(0, speedindex);
     delay(10);
  }
  delay(200);
  for(int speedindex = 255; speedindex >= 0; speedindex--)
  {
     SetMotor(0, speedindex);
     delay(10);
  }
  delay(200);

  // Direction 2
  for(int speedindex = 0; speedindex <= 255; speedindex++)
  {
     SetMotor(1, speedindex);
     delay(10);
  }
  delay(200);
  for(int speedindex = 255; speedindex >= 0; speedindex--)
  {
     SetMotor(1, speedindex);
     delay(10);
  }
  delay(200);
}
