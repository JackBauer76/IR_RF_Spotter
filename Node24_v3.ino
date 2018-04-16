/* Node24 on yellow breadboard
 *  
 *  Blue LED on A0
 *  White LED on A1
 *  Yellow LED on A2
 *  Pushbutton on D3
 *  
 * Instructions on Spotter24_v1 sketch
 * 
 * 
 */

#include <avr/interrupt.h>  
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

//const byte address[6] = "00001";
byte addresses[][6] = {"1Node","2Node"};
bool radioNumber = 0;
const int LED_yellow = A0;
const int LED_white = A1;
const int LED_blue = A2;
const int pushbutton = 3;
bool auth = 0;
bool state=0;
unsigned int variable;
volatile int AuthState=0;

void setup() 
{
  Serial.begin(57600);
  pinMode(LED_yellow, OUTPUT);
  pinMode(LED_white, OUTPUT);
  pinMode(LED_blue, OUTPUT);
  pinMode(pushbutton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pushbutton), Auth, RISING);
  
  radio.begin();
  //radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  
  if(radioNumber)
  {
        radio.openWritingPipe(addresses[1]);
        radio.openReadingPipe(1,addresses[0]);
  }
  else
  {
        radio.openWritingPipe(addresses[0]);
        radio.openReadingPipe(1,addresses[1]);
  }
  radio.startListening();
}

void Auth()
{
  AuthState = !AuthState;
}


void loop() {


  digitalWrite(LED_yellow, AuthState);
 
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
    
    digitalWrite(LED_white,HIGH);
    delay(100);
    digitalWrite(LED_white,LOW);
    
    
    }
    
    Serial.println(text);
    Serial.println(variable);    
    Serial.println(variable*2);
    Serial.println("Waiting");
       

  }



  /****************** Ping Out Role ***************************/
    if (role == 1)  {
        radio.stopListening();                                    // First, stop listening so we can talk.
        Serial.println(F("Now sending"));
        unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
        if (!radio.write( &start_time, sizeof(unsigned long) )){
            Serial.println(F("failed"));
        }
        radio.startListening();                                    // Now, continue listening
        unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
        boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
        while ( ! radio.available() ){                             // While nothing is received
            if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
                timeout = true;
                break;
            }
        }
        if ( timeout ){                                             // Describe the results
            Serial.println(F("Failed, response timed out."));
            }else{
            unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
            radio.read( &got_time, sizeof(unsigned long) );
            unsigned long end_time = micros();
            // Spew it
            Serial.print(F("Sent "));
            Serial.print(start_time);
            Serial.print(F(", Got response "));
            Serial.print(got_time);
            Serial.print(F(", Round-trip delay "));
            Serial.print(end_time-start_time);
            Serial.println(F(" microseconds"));
        }
        // Try again 1s later
        delay(1000);
    }
    /****************** Pong Back Role ***************************/
    if ( role == 0 )
    {
        unsigned long got_time;
        if( radio.available()){
            // Variable for the received timestamp
            while (radio.available()) {                                   // While there is data ready
                radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
            }
            radio.stopListening();                                        // First, stop listening so we can talk
            radio.write( &got_time, sizeof(unsigned long) );              // Send the final one back.
            radio.startListening();                                       // Now, resume listening so we catch the next packets.
            Serial.print(F("Sent response "));
            Serial.println(got_time);
        }
    }
    /****************** Change Roles via Serial Commands ***************************/
    if ( Serial.available() )
    {
        char c = toupper(Serial.read());
        if ( c == 'T' && role == 0 ){
            Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
            role = 1;                  // Become the primary transmitter (ping out)
        }else
        if ( c == 'R' && role == 1 ){
            Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
            role = 0;                // Become the primary receiver (pong back)
            radio.startListening();
        }
    }
} // Loop
