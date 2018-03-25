#include "Timer.h"
#include <avr/interrupt.h>  
Timer t;
const int IRsensor =2;
const int LED =4;
const int Poty = A6;
const int Potx = A7;
int obstacle;
bool blocked=0;

void setup() {
  Serial.begin(9600);
  pinMode(IRsensor,INPUT);
  pinMode(LED,OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(IRsensor),IRobstacle,RISING);
}

void loop() {
  t.update();
 
  Serial.print("Potentiometer X = ");
  Serial.println(analogRead(Potx));
  Serial.print("Potentiometer Y = ");
  Serial.println(analogRead(Poty));
  
  blocked ? digitalWrite(LED,HIGH): digitalWrite(LED,LOW);
  Serial.print("Sensor IR = ");
  Serial.println(blocked);
  delay(500);
}

int IRobstacle(){
  blocked =!blocked;
}


