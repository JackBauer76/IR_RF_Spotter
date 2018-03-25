#include <EEPROM.h>                          //Library for EEPROM
#include <SPI.h>                             //Library for SPI protocol
#include <MFRC522.h>                         //Library for RFID
#include <LiquidCrystal.h>                   //Library for LCD
#include <Keypad_MC17.h>                     //Library for Keypad with Port Expander
#include <Wire.h>

uint8_t successRead;                         //Variable integer to keep if we have Successful Read from Reader

byte masterCard[4];                          //Store master card's ID read from EEPROM
byte storedCard[4];                          //Store an ID read from EEPROM
byte readCard[4];                            //Store scanned ID read from RFID Module

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);            //Create MFRC522 instance.

const int rs = 6, en = 3, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);   //Create an LCD instance

#define I2CADDR 0x20
#define Password_Length 7

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {7, 6, 5, 4};                                                  //Connect to the row pinouts of the keypad
byte colPins[COLS] = {3, 2, 1, 0};                                                  //Connect to the column pinouts of the keypad
Keypad_MC17 customKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);  //Create a Keypad via Port Expander instance

char addDeleteCard[Password_Length]  = "#67890";                                    //Add/Delete PIN to Add/Delete RFID's 
char Initiate[Password_Length]       = "588885";                                    //Initiate PIN to initiate the system
char Master[Password_Length]         = "123456";                                    //Master PIN to deactivate the alarm
char Change[Password_Length]         = "#56789";                                    //Change PIN to change the Master PIN
char Reset[Password_Length]          = "*12345";                                    //Reset PIN to reactivate the alarm
char Data[Password_Length];                                                         //Generic data array (6 is the number of chars it can hold + the null char = 7)

byte data_count   = 0;                                                              //Counter to be used with Data[] array
byte master_count = 0;                                                              //Counter to be used with Master[] array

char customKey;                                                                     //Variable for Keypad input

const int ledPin = 8;
const int buzzerPin = 4;
const int lSpeakerPin = 2; 
const int pirPin = 7;
const int potPin = A6;

bool motionDet = LOW;        //Flag for motion detection
bool auth = LOW;             //Flag to deactivate the alarm
bool change = LOW;           //Flag to change the Master PIN
bool initiate = LOW;         //Flag to initiate the System
bool addDeleteRFID = LOW;    //Flag to Add/Delete RFID


void setup() {
  Serial.begin(9600);    //Initialize serial communications with PC
  SPI.begin();           //Initialize SPI protocol to be used with MFRC522
  mfrc522.PCD_Init();    //Initialize MFRC522 Hardware
  lcd.begin(16, 2);      //Initialize LCD Screen
  customKeypad.begin();  //Initialize Keypad via Port Expander

  pinMode(ledPin, OUTPUT); 
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(lSpeakerPin, OUTPUT);
  
  //wipeEEPROM();        //If wiping EEPROM is desired, uncomment
  while (!initiate) {    //While the initiate flag is off, wait for initiation
    initiateSystem();
  }
  
  setRFID();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");  
  delay(1500);
}


void loop () {
  MotionDetector();           //Sound the alarm if motion is detected
  
  if(!change) {               //If the flag for changing the PIN is off, operate normally
    Password();               //Read PINs for the Keypad
    } else {                  //If the flag is on, change the PIN
      changePassword();
    }
  
  if(successRead = getID()){  //If a card is scanned
    readRFID();               //Deal with the scanned ID
  }
}


///////////////////////////////////////// Adds/Deletes the Scanned PICC's UID (Works ONLY When The Add/Delete PIN iIs Entered) ///////////////////////////////////
void newRFID() {
  
  if (findID(readCard)) {  //If scanned card is known delete it
        deleteID(readCard);
      } else {             //If scanned card is not known add it
        writeID(readCard);
      }
      
  addDeleteRFID = LOW;     //After finishing, reset the Add/Delete RFID flag to low
}


///////////////////////////////////////// Reads RFID Input And Act Differently Upon Each Scanned ID ///////////////////////////////////
void readRFID() {  
  
  if (auth) {
    if (isMaster(readCard)) {                            //If Authority is already high, indicate it to the user
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm already OFF");
      delay(1000);
      lcd.clear();
      return;
    } else if (!isMaster(readCard) && addDeleteRFID) {   //If scanned RFID does not match the Master Card, and the AddDeleteRFID flag was on
        newRFID();                                       //Add RFID if not defined, delete it if defined
    }
  } else {
    if (isMaster(readCard)) {                            //If scanned card's ID matches Master Card's ID
      auth = HIGH;                                       //Set Authority to high
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Off");
      uint8_t count = EEPROM.read(0);                    //Read the first Byte of EEPROM that
      delay(1000);
      lcd.clear();
    } else {
      if (findID(readCard)) {                            //If not, see if the card is in the EEPROM
        auth = HIGH;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alarm Off");
        delay(1000);
        lcd.clear();
      } else {                                           //If not, show that the ID was not valid
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid ID");
        delay(1000);
        lcd.clear();
      }
    }
  }
}


///////////////////////////////////////// Sets Everything For Reading the RFID And Checks If A Master Card Is Defined ///////////////////////////////////
void setRFID() {
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);   //If maximal antenna range is desired, uncomment 

  if (EEPROM.read(1) != 143) {                        //Check if a Master card defined by checking if address '1' holds the number '143, if not let user define a master card
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan New ID");
    
    do {
      successRead = getID();                          //Set successRead to 1 when we get read from reader otherwise 0
    } while (!successRead);                           //Program will not go further while you not get a successful read
    
    for ( uint8_t j = 0; j < 4; j++ ) {               //Loop 4 times
      EEPROM.write( 2 + j, readCard[j] );             //Write scanned PICC's UID to EEPROM, start from address 3
    }
    
    EEPROM.write(1, 143);                             //Indicate to EEPROM that a Master Card is now defined
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Master ID added");
  }
  
  for ( uint8_t i = 0; i < 4; i++ ) {                 //Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i);               //Write it to the Master Card
  }
}


///////////////////////////////////////// Gets Scanned PICC's UID ///////////////////////////////////
uint8_t getID() {
  String content = "";                                                   //Auxiliary variable for printing on lcd
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {                              //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {                                //Since a PICC placed get Serial and continue
    return 0;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanned UID:");
  
  for ( uint8_t i = 0; i < mfrc522.uid.size; i++) {                      //Store read ID in array readCard[]
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));  //For printing on lcd
    content.concat(String(mfrc522.uid.uidByte[i], HEX));                 //For printing on lcd
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  
  content.toUpperCase();
  lcd.setCursor(0, 1);
  lcd.print(content.substring(1)); 
  mfrc522.PICC_HaltA();                                                  //Stop reading
  delay(1000);
  lcd.clear();
  return 1;
}


//////////////////////////////////////// Reads An ID From EEPROM //////////////////////////////
void readID( uint8_t number ) {
  uint8_t start = (number * 4 ) + 2;          //Figure out starting position
  
  for ( uint8_t i = 0; i < 4; i++ ) {         //Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   //Assign values read from EEPROM to array
  }
}


///////////////////////////////////////// Adds ID to EEPROM   ///////////////////////////////////
void writeID(byte a[]) {
  
  if ( !findID( a ) ) {                   //Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(0);         //Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = ( num * 4 ) + 6;      //Figure out where the next slot starts
    num++;                                //Increment the counter by one
    EEPROM.write( 0, num );               //Write the new count to the counter
    
    for ( uint8_t j = 0; j < 4; j++ ) {   //Loop 4 times
      EEPROM.write( start + j, a[j] );    //Write the array values to EEPROM in the right position
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID Added!");
    delay(1000);
    lcd.clear();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error! Try Again!");
  }
}

///////////////////////////////////////// Remove ID from EEPROM  ///////////////////////////////////
void deleteID(byte a[]) {
  
  if (!findID(a)) {                                           //Before we delete from the EEPROM, check to see if we have this card!
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error Deleting!");
  } else {
    uint8_t num = EEPROM.read(0);                             //Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot;                                             //Figure out the slot number of the card
    uint8_t start;                                            //=(num * 4) + 6; // Figure out where the next slot starts
    uint8_t looping;                                          //The number of times the loop repeats
    uint8_t j;
    uint8_t count = EEPROM.read(0);                           //Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );                                   //Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;                                                    // Decrement the counter by one
    EEPROM.write( 0, num );                                   // Write the new count to the counter
    
    for (j = 0; j < looping; j++) {                           // Loop the card shift times
      EEPROM.write(start + j, EEPROM.read(start + 4 + j));    // Shift the array values to 4 places earlier in the EEPROM
    }
    
    for (uint8_t k = 0; k < 4; k++) {                         // Shifting loop
      EEPROM.write(start + j + k, 0);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID Removed!");
    delay(1000);
    lcd.clear();
  }
}


///////////////////////////////////////// Checks 2 Given Bytes  ///////////////////////////////////
bool checkTwo (byte a[], byte b[]) {
  bool match = false;
  
  if (a[0] != 0) {                      // Make sure there is something in the array first
    match = true;                       // Assume they match at first
  }
  
  for (uint8_t k = 0; k < 4; k++) {     // Loop 4 times
    
    if (a[k] != b[k]) {                 // If a != b then set match = false, one fails, all fail
      match = false;
    }
  }
  
  if ( match ) {                        // Check to see if match is still true
    return true;
  } else  {
    return false;
  }
}


///////////////////////////////////////// Finds Memory Slot Of Stored ID  ///////////////////////////////////
uint8_t findIDSLOT( byte findID[] ) {
  uint8_t count = EEPROM.read(0);            //Read the first Byte of EEPROM that
  
  for (uint8_t i = 1; i <= count; i++) {   //Loop once for each EEPROM entry
    readID(i);                               //Read an ID from EEPROM, it is stored in storedCard[4]   
    
    if (checkTwo(findID, storedCard)) {        //Check to see if the storedCard read from EEPROM is the same as the find[] ID card passed 
      return i;                              //The slot number of the card
      break;                                 //Stop looking we found it
    }
  }
}


///////////////////////////////////////// Finds ID In EEPROM  ///////////////////////////////////
bool findID( byte toFind[] ) {
  uint8_t count = EEPROM.read(0);         //Read the first byte of EEPROM
  
  for (uint8_t i = 1; i <= count; i++) {  //Loop once for each EEPROM entry
    readID(i);                            // Read an ID from EEPROM, it is stored in storedCard[4]
    
    if (checkTwo(toFind, storedCard)) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;                              // Stop looking we found it
    }
  }
  
  return false;
}


////////////////////// Checks If Scanned ID Is The Master Card  ///////////////////////////////////
bool isMaster(byte test[]) {
  
  if (checkTwo(test, masterCard)) {
    return true;
  } else {
    return false;
  }
}


////////////////////// Asks The User for PIN to Initiate The System (Runs Only ONCE In The Setup) ///////////////////////////////////
void initiateSystem() { 
  lcd.setCursor(0, 0);
  lcd.print("Enter Key:");
  customKey = customKeypad.getKey(); 
  
  if (customKey) {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }
  
  if(data_count == Password_Length-1) {  //If Data[] array is full
    
    if(!strcmp(Data, Initiate)) {        //If PIN entered is equal to the InitiationPin 
    initiate = HIGH;                     //Flag to initiate the system
    } else {                             //If PIN entered is not equal to the InitiationPin
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Try Again");
      delay(1000);
      lcd.clear();
    }
    
    clearData();                         //Clear Data[] array
  }
}


////////////////////// Raises An Alarm If Motion Is Detected ///////////////////////////////////
void MotionDetector() {
  motionDet = digitalRead(pirPin);  //Read the motion detector
  
  if (motionDet && !auth) {         //If motion is detected and authority is low
    digitalWrite(buzzerPin, HIGH);  //Sound the alarm
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ALARM V6!!!");
    digitalWrite(ledPin, HIGH);
    delay(500);
    lcd.clear();
  } else {                          //If motion is detected but authority is high, or no motion is detected
    noTone(buzzerPin);              //Sound off the alarm
    noTone(lSpeakerPin);
    digitalWrite(ledPin, LOW);
  }
}


////////////////////// Reads The Keypad & Compares Input To The MasterPin, ChangePin, ResetPin, & AddDeletePin ///////////////////////////////////
void Password() {  
  customKey = customKeypad.getKey();
  
  if (customKey) {                //If a key is actually pressed
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN:");
    Data[data_count] = customKey; //Store key into the generic Data array
    lcd.setCursor(data_count, 1); //Move cursor to show each new char
    lcd.print(Data[data_count]);  //Print buttons entered
    data_count++;                 //Increment Data[] array by 1 to store new char, also keep track of the number of chars entered
  }   
  
  if(data_count == Password_Length-1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if(strcmp(Data, Change) && strcmp(Data, Reset) && strcmp(Data, addDeleteCard)) { //if PIN entered is not equal to any stored PIN
      
      if(!strcmp(Data, Master)) {  //if Valid PIN is entered 
        lcd.print("Alarm OFF");
        auth = HIGH;
      } else {  //If invalid PIN
        lcd.print("Invalid PIN!");
      }
            
      delay(1000);
      lcd.clear();
      clearData();      
    
    } else if(!strcmp(Data, Change)) {         //If PIN entered is to Change PIN
      change = HIGH;                           //Change the Master PIN
    
    } else if(!strcmp(Data, Reset)) {          //If PIN entered is to Reset PIN
      resetAlarm(); //reactivate the alarm
    
    } else if(!strcmp(Data, addDeleteCard)) {  //If PIN entered is to Add/Delete RFID
      addDeleteRFID = HIGH;                    //Set Add/Delete RFID flag to high
      clearData();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Scan ID");
    }
  } 
}


////////////////////// Changes The Master PIN   ///////////////////////////////////
void changePassword() {
  
  lcd.setCursor(0, 0);
  lcd.print("Enter new PIN:");
  customKey = customKeypad.getKey(); 
  
  if (customKey) {
    Master[master_count] = customKey;
    lcd.setCursor(master_count, 1);
    lcd.print(Master[master_count]);
    master_count++;
  }
  
  if(master_count == Password_Length-1) {  //If Master[] array is full
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password Changed!");
    master_count = 0;                      //Restart master_count
    change = LOW;                          //Switch the change flag off
    clearData();                           //Clear Data[] array
    delay(1000);
    lcd.clear();
  }
}


////////////////////// Reactivates The Alarm ///////////////////////////////////
void resetAlarm() {  
  auth = LOW;   //Set Authority to LOW
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm ON!");
  clearData();  //Clear Data[] arrary
  delay(2000);
  lcd.clear();
}


////////////////////// Clears Data[] Array To Be Used Again ///////////////////////////////////
void clearData() {
  
  while(data_count != 0) {
    Data[data_count--] = 0;
  }
  
  return;
}


////////////////////// Wipes EEPROM ///////////////////////////////////
void wipeEEPROM() {
  for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) {    //Loop end of EEPROM address
  
    if (EEPROM.read(x) == 0) {              //If EEPROM address 0
                                          // do nothing, already clear, go to the next address in order to save time and reduce writes to EEPROM
    } else {
     EEPROM.write(x, 0);       // if not write 0 to clear, it takes 3.3mS
    }
  }
}
