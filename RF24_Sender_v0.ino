/*  New Sketch to develop RF2.4Ghz
 *  Sender Nano in mimi green Protoboard
 *  with Poty
 */

#include <nRF24L01.h>
#include <SPI.h>
#include "RF24.h"
/******************* User Config ***************/
RF24 radio(9,10);
bool radioNumber = 1;
bool role = 1;
const int BlueLed = 4;
const int Led2 = A5;
const int VRx = A7;
const int VRy = A6;
const int Pushbutton = 3;

int message; 
const byte address[6] = "00001";

void setup() 
{
  Serial.begin(9600);
  pinMode(BlueLed, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Pushbutton, INPUT_PULLUP);
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(1000,20);
  radio.stopListening();
}

void loop() 
{
  //digitalWrite(Led2, HIGH);
  //delay(20);
  //digitalWrite(Led2, LOW);
  message = analogRead(VRx);
  
  if(radio.write(&message, sizeof(int) ))
  {
    Serial.print("Message was sent = ");
    Serial.println(message);
    digitalWrite(BlueLed, HIGH);
    delay(20);
    digitalWrite(BlueLed, LOW);
   
  }
  else
  {
    //Serial.println("Message was not sent");
  }
  
}



