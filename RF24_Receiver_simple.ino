#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";
const int LED = 4;
unsigned int variable;
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));

    variable =atoi(text);
    if(variable == 4517){
    Serial.println("me cago de felicidad");
    digitalWrite(LED,HIGH);
    delay(500);
    }
    digitalWrite(LED,LOW);
    Serial.println(text);
    Serial.println(variable);    
    Serial.println(variable*2);
  }


}
