#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SdFat.h>

#define MFRC_RST_PIN 9
#define MFRC_SS_PIN  10
#define SD_SS_PIN    8

#define STATE_STARTUP       0
#define STATE_STARTUP_ERROR 1
#define STATE_STARTING      2
#define STATE_WAITING       3
#define STATE_SCAN_INVALID  4
#define STATE_SCAN_VALID    5
#define STATE_SCAN_MASTER   6
#define STATE_ADDED_CARD    7
#define STATE_REMOVED_CARD  8

#define REDPIN 6
#define GREENPIN 7

const int cardSize = 4;
byte masterCard[cardSize] = {29,156,78,37};
byte readCard[cardSize] = {0,0,0,0};

// Create MFRC522 instance
MFRC522 mfrc522(MFRC_SS_PIN, MFRC_RST_PIN);
// Set the LCD I2C address
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

SdFat sd;

byte currentState = STATE_STARTUP;
unsigned long LastStateChangeTime;
unsigned long StateWaitTime;

char cardFile[] = "cards.txt";
char cardTempFile[] = "cardsTemp.txt";

//------------------------------------------------------------------------------------
void PrintCard(byte printCard[cardSize])
{
  int index;

  Serial.print("Card - ");
  for(index = 0; index < 4; index++)
  {
    if (index > 0)
    {
      Serial.print(",");
    }
    Serial.print(printCard[index]);
  }
  Serial.println(" ");
}

//------------------------------------------------------------------------------------
boolean findCard()
{
  byte currentCard[cardSize];
  char text[10];
  char c1;
  int  index;
  int  value;

  //Serial.print("find ");
  //PrintCard(readCard);

  // open input file
  ifstream readStr(cardFile);

  // check for open error
  if (!readStr.is_open())
  {
    return false;
  }

  index = 0;
  // read until input fails
  while (!readStr.eof()) 
  {
    readStr >> value >> c1; 

    if (readStr.fail()) 
    {
      break;
    }

    currentCard[index] = value;
    
    index++;
    if (index > 3)
    {
      //Serial.print("file read ");
      //PrintCard(currentCard);
      if ((memcmp(currentCard, readCard, 4)) == 0)
      {
        return true;
      } 
      index = 0;
    }
  }

  return false;
}

//------------------------------------------------------------------------------------
void addCard()
{
  int index;
  SdFile writeFile;

  //Serial.print("add ");
  //PrintCard(readCard);
  if (writeFile.open(cardFile, O_RDWR | O_CREAT | O_AT_END))
  { 
    for(index = 0; index < 4; index++)
    {
      writeFile.print(readCard[index]); 
      writeFile.print(",");
    }
    writeFile.close();
  }
  return;
}

//------------------------------------------------------------------------------------
void removeCard()
{
  byte currentCard[cardSize];
  char text[10];
  char c1;
  int  readIndex, writeIndex;
  int  value;
  SdFile writeFile;

  //Serial.print("remove ");
  //PrintCard(readCard);

  // open input file
  ifstream readStr(cardFile);

  // check for open error
  if (!readStr.is_open())
  {
    return;
  }

  if (writeFile.open(cardTempFile, O_RDWR | O_CREAT | O_AT_END))
  {
    readIndex = 0;

    while (!readStr.eof()) 
    {
      readStr >> value >> c1; 

      if (readStr.fail()) 
      {
        break;
      }

      currentCard[readIndex] = value;
    
      readIndex++;
      if (readIndex > 3)
      {
        //Serial.print("file write ");
        //PrintCard(currentCard);
        if (!((memcmp(currentCard, readCard, 4)) == 0))
        {
          for (writeIndex = 0; writeIndex < 4; writeIndex++)
          {
            writeFile.print(currentCard[writeIndex]); 
            writeFile.print(",");
          }
          writeFile.close();
        }
      } 
      readIndex = 0;
    }
  }

  sd.remove(cardFile);
  sd.rename(cardTempFile, cardFile);
  
  return;
}

//------------------------------------------------------------------------------------
int readCardState()
{
  int index;

  for(index = 0; index < 4; index++)
  {
    readCard[index] = mfrc522.uid.uidByte[index];
  }
  //Serial.print("State ");
  //PrintCard();

  //Check Master Card
  if ((memcmp(readCard, masterCard, 4)) == 0)
  {
    return STATE_SCAN_MASTER;
  }

  if (findCard() == true)
  {
    return STATE_SCAN_VALID;
  }

 return STATE_SCAN_INVALID;
}

//------------------------------------------------------------------------------------
void DisplayInfo(char *Line1Str, char *Line2Str, int RedPinState, int GreenPinState)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Line1Str);
  lcd.setCursor(0,1);
  lcd.print(Line2Str);
  digitalWrite(REDPIN, RedPinState);
  digitalWrite(GREENPIN, GreenPinState);
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
      DisplayInfo("RFID Scanner", "Starting up", HIGH, HIGH);
      StateWaitTime = 1000;
      break;
      
    case STATE_STARTUP_ERROR:
      DisplayInfo("Error", "SD card", HIGH, HIGH);
      StateWaitTime = 1000;
      break;
      
    case STATE_WAITING:
      DisplayInfo("Waiting for Card", "to be swiped", LOW, LOW);
      StateWaitTime = 1000;
      break;
      
    case STATE_SCAN_INVALID:
      if (currentState == STATE_SCAN_MASTER)
      {
        addCard();
        aState = STATE_ADDED_CARD;

        DisplayInfo("Card Scanned", "Card Added", LOW, HIGH);
        StateWaitTime = 2000;
      }
      else if (currentState == STATE_REMOVED_CARD)
      {
        return;
      }
      else
      {
        DisplayInfo("Card Scanned", "Invalid Card", HIGH, LOW);
        StateWaitTime = 2000;
      }
      break;
      
    case STATE_SCAN_VALID:
      if (currentState == STATE_SCAN_MASTER)
      {
        removeCard();
        aState = STATE_REMOVED_CARD;

        DisplayInfo("Card Scanned", "Card Removed", LOW, HIGH);
        StateWaitTime = 2000;
      }
      else if (currentState == STATE_ADDED_CARD)
      {
        return;
      }
      else
      {
        DisplayInfo("Card Scanned", "Valid Card", LOW, HIGH);
        StateWaitTime = 2000;
      }
      break;
      
    case STATE_SCAN_MASTER:
      DisplayInfo("Master Card", "", LOW, HIGH);
      StateWaitTime = 5000;
      break;
  }

  //Serial.print("Current State - ");
  //Serial.print(currentState);
  //Serial.print(", New State - ");
  //Serial.println(aState);

  currentState = aState;
  LastStateChangeTime = millis();
}

//------------------------------------------------------------------------------------
void setup() 
{
  SPI.begin();         // Init SPI Bus
  mfrc522.PCD_Init();  // Init MFRC522

  if (!sd.begin(SD_SS_PIN, SPI_HALF_SPEED))
  {
    updateState(STATE_STARTUP_ERROR);   
  }

  lcd.begin(20,4);

  LastStateChangeTime = millis();
  updateState(STATE_STARTING);

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);

  Serial.begin(9600);
}

//------------------------------------------------------------------------------------
void loop() 
{
  byte cardState;

  if ((currentState != STATE_WAITING) &&
      (currentState != STATE_STARTUP_ERROR) &&
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

  if (currentState != STATE_STARTUP_ERROR)
  {
    cardState = readCardState();
    updateState(cardState);
  }
}
