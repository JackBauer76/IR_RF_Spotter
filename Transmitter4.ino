#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 7); // CE, CSN

const int blueLED = A5;
const int blueLED2 = 4;
const int Ay = A6;
const int Ax = A7;
const int pushbutton = 3;

//const byte address[6] = "00001";
byte addresses[][6] = {"1Node","2Node"};

const char text[32] = "54257";
unsigned long text1;
unsigned long text3 = 3214238321;
byte text2 = 0b11111111;
unsigned int X;

void setup() {
  Serial.begin(9600);
  pinMode(Ax, OUTPUT);
  pinMode(Ay, OUTPUT);
  pinMode(pushbutton, INPUT_PULLUP);
  pinMode(blueLED, OUTPUT);
  pinMode(blueLED2, OUTPUT);
  radio.begin();
//  radio.openWritingPipe(address);

  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}
void loop() {
  unsigned int Axis_x = analogRead(Ax);
  unsigned int Axis_y = analogRead(Ay);
  Serial.println(X);
  if(X==1010){
    digitalWrite(blueLED, HIGH);
    
  }
  else
  {
    digitalWrite(blueLED, LOW);
  }
  //Serial.print("X Axis poti = ");
  //Serial.println(Axis_x);
  //Serial.print("Y Axis poti = ");
  //Serial.println(Axis_y);
  //Serial.print("Pushbutton");
  //Serial.println(digitalRead(pushbutton));
  
  if (digitalRead(pushbutton) == LOW)
  {
    digitalWrite(blueLED2, HIGH);
    radio.stopListening();
    radio.write(&Axis_x, sizeof(Axis_x));
    //radio.write(&Axis_y, sizeof(Axis_y));
     
  }
  else
  {
     digitalWrite(blueLED2, LOW);
     radio.startListening();
     if (radio.available()) 
     {
        radio.read(&text1, sizeof(text1));
        X= text1;
        Serial.print("receiving = ");
        Serial.print("X =");
        Serial.println(X);
     }
    
  }
  
  /*
  radio.startListening();
  delay(410);

  if (radio.available())
  {
    digitalWrite(blueLED, HIGH);
    char text[32] = "";
    radio.read(&text, sizeof(text)); // very similar to here
    Serial.print("Receiving message");
    Serial.println(text);
  }
  else
  {
    digitalWrite(blueLED,LOW);
  }
  
*/

}


