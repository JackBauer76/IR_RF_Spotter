 
#include <TimerOne.h>
#include "Timer.h"
#include <avr/interrupt.h>   
#include <IRremote.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
IRsend irsend;
Timer t;
//constants for ProMini (black)
const int BLUE = A3;
const int IRled = 3;
const int RF433send = 8;
const int PIR = 2;

/* //constants for Nano (Red/white)
const int BLUE = 4;
const int IRled = 9;
const int RF433send = 11;
const int PIR = 2;
 */

const long AlarmOnTime = 5000; // divide by 1000 --> seconds
volatile int PIRstate=0;

void setup() 
{
  Serial.begin(9600);
  pinMode(BLUE,OUTPUT);
  pinMode(IRled,OUTPUT);
  pinMode(RF433send,OUTPUT);
  pinMode(PIR,INPUT);
  mySwitch.enableTransmit(RF433send);
  mySwitch.setRepeatTransmit(50);
  // more transmission startup here
  attachInterrupt(digitalPinToInterrupt(PIR),motionDetector,RISING);
}

void loop() 
{
  t.update();
  if(PIRstate == 1)
  {
    transmission();
  }
  else
  {
    digitalWrite(BLUE,LOW);
    // turn off buzzer and lights
  }
}

void transmission()
{
  t.pulse(BLUE, AlarmOnTime,HIGH);// a buzzer can be inserted
  t.after(AlarmOnTime,lightsOff);
  // here goes the transmission
  irsend.sendRC5(0x4,8); 
  mySwitch.send(5365,24);
}

void motionDetector()
{
  PIRstate = !PIRstate;  
}

void lightsOff()
{
  PIRstate=0;
}

