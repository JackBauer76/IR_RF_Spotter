/* Node24 on yellow breadboard
 *  
 *  Blue LED on A0
 *  White LED on A1
 *  Yellow LED on A2
 *  Pushbutton on D3
 *  
 * 
 * 
 * 
 */


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";
const int LED_yellow = A2;
const int LED_white = A1;
const int LED_blue = A0;
const int pushbutton = 3;

bool state=0;
unsigned int variable;

void setup() {
  Serial.begin(57600);
  pinMode(LED_yellow, OUTPUT);
  pinMode(LED_white, OUTPUT);
  pinMode(LED_blue, OUTPUT);
  pinMode(pushbutton, INPUT_PULLUP);
 
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
   
}



void loop() {


 
  long ago = micros();
  if (radio.available()) {
 
    char text[32] = "";
    radio.read(&text, sizeof(text));

    variable =atoi(text);
    if(variable == 4517){
    state = 1;
    long now = micros();
    Serial.println(now - ago);
    Serial.println("received");
    
    digitalWrite(LED_blue,HIGH);
    delay(100);
    digitalWrite(LED_blue,LOW);
    
    
    }
    
    Serial.println(text);
    Serial.println(variable);    
    Serial.println(variable*2);
    Serial.println(state);
       

  }



}
