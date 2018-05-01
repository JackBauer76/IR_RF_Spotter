#include <avr/interrupt.h>  
#include "Timer.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte addresses[][6] = {"1Node","2Node"};

const int LED_white = 4;
const int LED_blue = A5;
const int pushbutton = 3;
unsigned int variable;

const long AlarmOnTime = 5000;
volatile bool AuthState = 0;
bool PIRState;
Timer t;

void setup() 
{
  Serial.begin(57600);
  
  pinMode(LED_white, OUTPUT);
  pinMode(LED_blue, OUTPUT);
  pinMode(pushbutton, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(pushbutton), Auth, RISING);
  
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(10,15);         
}

void Auth() {
  AuthState = !AuthState;
}


void loop() {
  t.update();
  Serial.print("AuthState is");
  Serial.println(AuthState);
  Serial.println("");
  digitalWrite(LED_white, AuthState);

    if (!AuthState) {
      radio.startListening();
      if(radio.available()) {
        char text[32] = "";
        radio.read(&text, sizeof(text)); 
        variable =atoi(text);
        Serial.print("variable received: ");
        Serial.println(variable);
        radio.read(&PIRState, sizeof(PIRState)); 
        Serial.print("PIRstate received: ");
        Serial.println(PIRState);
        if(PIRState>0) 
        {
          Serial.print(variable);
          
          radio.stopListening();
          radio.write(&AuthState, sizeof(AuthState));
          Serial.println("****************Motion Detected!");
          t.pulse(LED_white, AlarmOnTime, HIGH);
          
          t.after(AlarmOnTime, ledOff);
          radio.stopListening();
                    
        }
      }
    } else 
    {
      radio.startListening();
    }
}


void ledOff()
{
  digitalWrite(LED_white, LOW);
  //PIRState=LOW;
}

