

/*  New Sketch to develop RF2.4Ghz
 *  Sender Nano in Green mini Protoboard
 *  with Joystick Poti
 */

#include <nRF24L01.h>
#include <SPI.h>
#include "RF24.h"
/******************* User Config ***************/
RF24 radio(9,10);
bool radioNumber = 0;

const byte address[6] = "00001";
bool role = 0;
const int BlueLed = 4;
const int Led2 = A3;
const int PIR = 2;

unsigned int data;
uint8_t pipeNum;

void setup() 
{
  Serial.begin(9600);
  pinMode(BlueLed, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(PIR, INPUT);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() 
{
  if ( role == 0 )
  {
    //Serial.println("waiting");
    radio.startListening();
   

    if (radio.available(&pipeNum))
    
    {
       
       radio.read(&data, sizeof(int));
       Serial.print("Got data on pipe");
       Serial.println(pipeNum);
       Serial.print("I listened this = ");
       
       Serial.println(data);
       if(data > 2)
       {
          digitalWrite(BlueLed, HIGH);
       }
       else
       {
          digitalWrite(BlueLed, LOW);
       }
    
     }
   }
}

