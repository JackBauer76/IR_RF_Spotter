/*Spotter24 on green breadboard
 * 
 * Yellow LED on A3
 * White LED on D4
 * PIR on Pin2
 * 
 * to do: Install Library RF24, if you cant get both module to 
 * communicate, load the example getting started from 
 * RF24 Library
 * 
 * The spotter should detect an motion via PIR sensor (already implemented
 * and send a message to node. the message should imply "I sensed movement, do I need to activate alarm?
 * then the node should change its state from listening to sending and respond via a message if there is 
 * an authentification (auth = True or False).
 * The authentification in the nano should be done via a toggling 
 * variable based on the pushbutton
 * If Pushbutton was pushed, then auth= True, if pushed again auth = false
 * 
 * The main goal of the sketch should be to change the node from listening mode to transmitting mode if certain message 
 * (for example certain code) is received) and the so on back and forth from node to spotter
 * 
 * for example:
 * 
 * Spotter senses through PIR sensor and transmits code meaning "i sensed something, do you have authentification?
 * Node receives Code from Spotter and sends another code either meaning, authentificatiobn = true or false
 * Spotter receives code from node with Authentification information and lights another LED.
 * 
 * 
 * You have 2 and 3 LED to show the current state, for example:
 * 
 * Spotter activates yellow LED if PIRstate = high
 * and White LED if PIR high and authentification = True
 * 
 * Nano activates blue LED if message from Spotter received,
 * yellow if auth state is true and white if motion detection received from spotter if there was no authentification
 * 
 * if auth = 0 (false) the "alarm LED" should be ON.
 * 
 *
 * 
 * 
 * 
 * 
 */








// Version 0 merges Spotter & RF_Transmitter_simple_v2
// Version v0 works with RF_Receiver_simple_v2

#include "Timer.h"
#include <avr/interrupt.h>   
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
Timer t;

const byte address[6] = "00001";
const int LED_yellow = A3;
const int LED_white = 4;
const int PIR = 2;
volatile int PIRstate = 0;
const long AlarmOnTime = 5000;



void setup() 
{
  Serial.begin(57600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  pinMode(LED_yellow, OUTPUT);
  pinMode(LED_white, OUTPUT);
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
   digitalWrite(LED_yellow,LOW); 
  }
}

void transmission()
{
  t.pulse(LED_yellow, AlarmOnTime,HIGH);
  t.after(AlarmOnTime,lightsOff);
  // here it goes the transmission
  const char text[5] = "4517";
  radio.write(&text, sizeof(text));
}

void motionDetector()
{
  PIRstate = !PIRstate;
}

void lightsOff()
{
  PIRstate = 0;
}

