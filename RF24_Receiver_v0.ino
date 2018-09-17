/*  New Sketch to develop RF2.4Ghz
 *  Sender Nano in white small Protoboard
 *  with PIR
 */

#include <nRF24L01.h>
#include <SPI.h>
#include "RF24.h"
/******************* User Config ***************/
RF24 radio(7,8);
bool radioNumber = 1;
byte addresses[][6] = {"1Node","2Node"};
bool role = 1;
const int BlueLed = 4;
const int Led2 = A3;
const int PIR = 2;
const int potpin = A6;


const byte address[6] = "00001";

void setup() 
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  //radio.startListening();
  radio.stopListening();
}

void loop() 
{
  unsigned int* message = 16757;
  if (role == 1)  
  {
    radio.stopListening();
    radio.write( &message, sizeof(unsigned int) ); 
   
  }
}


