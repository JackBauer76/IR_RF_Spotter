#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";
const int LED = 4;
bool state=0;
unsigned int variable;
void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);
 
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
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
    Serial.println("me cago de felicidad");
    
    digitalWrite(5,HIGH);
    delay(100);
    digitalWrite(5,LOW);
    
    
    }
    
    Serial.println(text);
    Serial.println(variable);    
    Serial.println(variable*2);
    Serial.println(state);
       

  }



}
