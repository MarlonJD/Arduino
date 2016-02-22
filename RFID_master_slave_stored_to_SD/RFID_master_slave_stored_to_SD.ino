#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 8
#define SS_PIN  9

#define STATE_STARTUP       0
#define STATE_STARTING      1
#define STATE_WAITING       2
#define STATE_SCAN_INVALID  3
#define STATE_SCAN_VALID    4
#define STATE_SCAN_MASTER   5
#define STATE_ADDED_CARD    6
#define STATE_REMOVED_CARD  7

#define REDPIN 6
#define GREENPIN 7

const int cardArrSize = 10;
const int cardSize    = 4;
byte cardArr[cardArrSize][cardSize];
byte masterCard[cardSize] = {29,156,78,37};
byte readCard[cardSize];
byte cardsStored = 0;

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);
// Set the LCD I2C address
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

byte currentState = STATE_STARTUP;
unsigned long LastStateChangeTime;
unsigned long StateWaitTime;

//------------------------------------------------------------------------------------
int readCardState()
{
  int index;

  Serial.print("Card Data - ");
  for(index = 0; index < 4; index++)
  {
    readCard[index] = mfrc522.uid.uidByte[index];

    
    Serial.print(readCard[index]);
    if (index < 3)
    {
      Serial.print(",");
    }
  }
  Serial.println(" ");

  //Check Master Card
  if ((memcmp(readCard, masterCard, 4)) == 0)
  {
    return STATE_SCAN_MASTER;
  }

  if (cardsStored == 0)
  {
    return STATE_SCAN_INVALID;
  }

  for(index = 0; index < cardsStored; index++)
  {
    if ((memcmp(readCard, cardArr[index], 4)) == 0)
    {
      return STATE_SCAN_VALID;
    }
  }

 return STATE_SCAN_INVALID;
}

//------------------------------------------------------------------------------------
void addReadCard()
{
  int cardIndex;
  int index;

  if (cardsStored <= 20)
  {
    cardsStored++;
    cardIndex = cardsStored;
    cardIndex--;
  }

  for(index = 0; index < 4; index++)
  {
    cardArr[cardIndex][index] = readCard[index];
  }
}

//------------------------------------------------------------------------------------
void removeReadCard() 
{     
  int cardIndex;
  int index;
  boolean found = false;
  
  for(cardIndex = 0; cardIndex < cardsStored; cardIndex++)
  {
    if (found == true)
    {
      for(index = 0; index < 4; index++)
      {
        cardArr[cardIndex-1][index] = cardArr[cardIndex][index];
        cardArr[cardIndex][index] = 0;
      }
    }
    
    if ((memcmp(readCard, cardArr[cardIndex], 4)) == 0)
    {
      found = true;
    }
  }

  if (found == true)
  {
    cardsStored--;
  }
}

//------------------------------------------------------------------------------------
void updateState(byte aState)
{
  if (aState == currentState)
  {
    return;
  }

  // do state change
  switch (aState)
  {
    case STATE_STARTING:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("RFID Scanner");
      lcd.setCursor(0,1);
      lcd.print("Starting up");
      StateWaitTime = 1000;
      digitalWrite(REDPIN, HIGH);
      digitalWrite(GREENPIN, HIGH);
      break;
    case STATE_WAITING:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Waiting for Card");
      lcd.setCursor(0,1);
      lcd.print("to be swiped");
      StateWaitTime = 0;
      digitalWrite(REDPIN, LOW);
      digitalWrite(GREENPIN, LOW);
      break;
    case STATE_SCAN_INVALID:
      if (currentState == STATE_SCAN_MASTER)
      {
        addReadCard();
        aState = STATE_ADDED_CARD;
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Card Scanned");
        lcd.setCursor(0,1);
        lcd.print("Card Added");
        StateWaitTime = 2000;
        digitalWrite(REDPIN, LOW);
        digitalWrite(GREENPIN, HIGH);
      }
      else if (currentState == STATE_REMOVED_CARD)
      {
        return;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Card Scanned");
        lcd.setCursor(0,1);
        lcd.print("Invalid Card");
        StateWaitTime = 2000;
        digitalWrite(REDPIN, HIGH);
        digitalWrite(GREENPIN, LOW);
      }
      break;
    case STATE_SCAN_VALID:
      if (currentState == STATE_SCAN_MASTER)
      {
        removeReadCard();
        aState = STATE_REMOVED_CARD;
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Card Scanned");
        lcd.setCursor(0,1);
        lcd.print("Card Removed");
        StateWaitTime = 2000;
        digitalWrite(REDPIN, LOW);
        digitalWrite(GREENPIN, HIGH);
      }
      else if (currentState == STATE_ADDED_CARD)
      {
        return;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Card Scanned");
        lcd.setCursor(0,1);
        lcd.print("valid Card");
        StateWaitTime = 2000;
        digitalWrite(REDPIN, LOW);
        digitalWrite(GREENPIN, HIGH);
      }
      break;
    case STATE_SCAN_MASTER:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Master Card");
      lcd.setCursor(0,1);
      lcd.print("Cards = ");
      lcd.setCursor(8,1);
      lcd.print(cardsStored);
      StateWaitTime = 5000;
      digitalWrite(REDPIN, LOW);
      digitalWrite(GREENPIN, HIGH);
      break;
  }

  /*lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(aState);
  lcd.setCursor(0,1);
  lcd.print(currentState);*/

  currentState = aState;
  LastStateChangeTime = millis();
}

void setup() 
{
  SPI.begin();         // Init SPI Bus
  mfrc522.PCD_Init();  // Init MFRC522

  lcd.begin(20,4);

  LastStateChangeTime = millis();
  updateState(STATE_STARTING);

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);

  Serial.begin(9600);
}

void loop() 
{
  byte cardState;

  if ((currentState != STATE_WAITING) &&
      (StateWaitTime > 0) &&
      (LastStateChangeTime + StateWaitTime < millis()))
  {
    updateState(STATE_WAITING);
  }

  // Look for new cards 
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  { 
    return; 
  } 
  
  // Select one of the cards 
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  { 
    return; 
  }

  cardState = readCardState();
  updateState(cardState);
}
