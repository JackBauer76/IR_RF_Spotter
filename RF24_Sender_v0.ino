/*  New Sketch to develop RF2.4Ghz
 *  Sender Nano in Green mini Protoboard
 *  with Joystick Poti
 */

#include <nRF24L01.h>
#include <SPI.h>
#include "RF24.h"
/******************* User Config ***************/
RF24 radio(7,8);
bool radioNumber = 0;

const byte address[6] = "00001";
//byte addresses[][6] = {"1Node","2Node"};
bool role = 0;
const int BlueLed = 4;
const int BlueLed2 = A5;
const int Pushbutton = 3;

unsigned int variable;

void setup() 
{
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  //radio.openWritingPipe(addresses[0]);
  //radio.openReadingPipe(1,addresses[1]);
  radio.startListening();
}

void loop() 
{
  if ( role == 0 )
  {
    Serial.println("waiting");
    radio.startListening();
    unsigned int message;

    if (radio.available()) 
    {
 
       radio.read( &message, sizeof(unsigned int) );
       Serial.print("I listened this = ");
       Serial.println(message);
       variable = int(&message);
       Serial.print("           with pointer = ");
       Serial.println(variable);
       if(variable == 4517)
        {
          Serial.println("me cago de felicidad");
          digitalWrite(BlueLed,HIGH);
          delay(100);
          digitalWrite(BlueLed,LOW);
        }
    }
  }
}
