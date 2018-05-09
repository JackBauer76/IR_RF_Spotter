
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(8, 7); // CE, CSN
//const byte address[6] = "00001";
byte addresses[][6] = {"1Node","2Node"};

const int pir = 2;
const int blueLED = 4;
const int whiteLED = A3;

unsigned int variable;
char text[32];
unsigned long text1;
unsigned long text2;
volatile unsigned int X;

void setup() {
  Serial.begin(9600);
  pinMode(pir,INPUT);
  pinMode(blueLED,OUTPUT);
  pinMode(whiteLED,OUTPUT);
  
  radio.begin();
//  radio.openReadingPipe(0, address);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if (radio.available()) {

    radio.read(&text1, sizeof(text1));
    X= text1;
    Serial.print("receiving = ");
    Serial.print("X =");
    Serial.println(X);
    
  }
Serial.print("X =");
Serial.println(X);
radio.startListening();

  if (X ==505 && digitalRead(pir))
  {
    radio.stopListening();
    text2 = X*2;
    delay(100);
    digitalWrite(whiteLED, HIGH);
    radio.stopListening();
    radio.write(&text2, sizeof(text2));
  }
  else 
  {
    digitalWrite(whiteLED, LOW);
    radio.startListening();
    Serial.println("Goodbye");
    
  }


  /*
  if(digitalRead(pir))
  {
    digitalWrite(blueLED,HIGH);
    radio.stopListening();
    radio.write(&text, sizeof(text));
  }
  else
  {
    digitalWrite(blueLED,LOW);
    radio.startListening();
  }

  */
  
}

