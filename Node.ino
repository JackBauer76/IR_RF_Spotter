// Test im RF_Receiver_Nano_V1

#include <IRremote.h>
#include <RCSwitch.h>
#include <TimerOne.h>
#include "Timer.h"
#include <avr/interrupt.h>   

const int IRrecvPin = 9; // IR Sensor pin
const int LED = 13; // LED pin 
const int BLUE = 6; // LED pin
const int BUZZER = 5;
const int RFrecv = 2;
 
const long AlarmOnTime = 12000;// divide by 1000 --> seconds
volatile int MotionState=0;
volatile int lastMotion = 0;

IRrecv irrecv(IRrecvPin);
decode_results results;
RCSwitch mySwitch = RCSwitch();
Timer t;

void setup() 
{
  Serial.begin(9600); // configure the baudrate
  irrecv.enableIRIn(); // Start the receiver
  pinMode(LED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RFrecv, INPUT);
  pinMode(IRrecvPin,INPUT);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() 
{
  t.update();
  if (mySwitch.available()||(irrecv.decode(&results))) 
  {
   
    if (mySwitch.getReceivedValue()==5365||(0x4== results.value))
    {
      MotionState=1;
      
    }
    else 
    {
      digitalWrite(BLUE,LOW);
      MotionState=0;
      
    }
    mySwitch.resetAvailable();
    irrecv.resume(); // prepare to receive the next value
    
  }
  if(MotionState==1)
  {
    alarmed();
  }
  else
  {
    digitalWrite(BLUE,LOW);
    lastMotion=0;
  }
  Serial.print("MotionState = ");
  Serial.print(MotionState);
  Serial.print(" lastMotion = ");
  Serial.println(lastMotion);
}

void alarmed()
{
  t.pulse(BLUE, AlarmOnTime,HIGH);// a buzzer can be inserted
  lastMotion=MotionState;
  t.after(AlarmOnTime,lightsOff);
}

void lightsOff()
{
  digitalWrite(BLUE,LOW);
  MotionState=0;
}


