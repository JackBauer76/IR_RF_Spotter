#include "Timer.h"
#include <avr/interrupt.h>   
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
Timer t; // Timer t is for the timer interrupts

byte addresses[][6] = {"1Node","2Node"};

const int LED_yellow = A3;
const int LED_white = 4;
const int PIR = 2;

const long AlarmOnTime = 500;

volatile bool PIRState;
bool AuthState;

void setup() 
{
  Serial.begin(57600);
  
  pinMode(LED_yellow, OUTPUT);
  pinMode(LED_white, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(PIR), motionDetector, RISING);
  
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);  
  radio.setPALevel(RF24_PA_MAX);
  //radio.setRetries(10,15);         
}

void motionDetector()
{
  PIRState = !PIRState;
}

void loop() { 
  t.update();

  if(PIRState) 
  {
    Serial.print("PIRState is");
    Serial.println(PIRState);
    radio.stopListening();
    //const char text[5] = "4517";
    //Serial.print("message almost sent");
    //radio.write(&text, sizeof(text));
    //Serial.print("message sent");
    radio.write(&PIRState, sizeof(PIRState));
    
    Serial.print("AuthState is");
    Serial.print(AuthState);
      
    radio.startListening();
    if(radio.available()) 
    {
      radio.read(&AuthState, sizeof(AuthState));
      Serial.print("AuthState is");
      Serial.print(AuthState);
      
      if(!AuthState) 
      {
        Serial.println("Authorized!");
        t.pulse(LED_yellow, AlarmOnTime, HIGH);
        t.after(AlarmOnTime, ledOff);
        
      }
    }
  } 
  else 
  {
    radio.startListening();
    
  }
}

void ledOff()
{
  Serial.print("Calling off");
  digitalWrite(LED_yellow, LOW);
  PIRState = !PIRState;
}

