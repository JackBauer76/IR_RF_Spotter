/* Node24 on yellow breadboard
 *  
 *  
 *  White LED on D4
 *  Pushbutton on D3
 *  
 * Instructions on Spotter24_v4 sketch
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
const int LED_white = 5;
const int LED_blue = A5;

const int pushbutton = 3;
bool auth = 0;
bool state=0;
unsigned int variable;
volatile int AuthState=0;
bool role = 0;
void setup() 
{
  Serial.begin(57600);
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


  digitalWrite(LED_white, AuthState);

 /*
  long ago = micros();
  if (radio.available()) 
  {
 
    char text[32] = "";
    radio.read(&text, sizeof(text)); // very similar to here
    // take the received message, transform into integer
    // and check the key to according to auth, to switch role from 
    // 0 to 1 (sending)
    
    variable =atoi(text);
    if(variable == 4517)
    {
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
    Serial.println("Waiting");
  }

*/

  /****************** Ping Out Role ***************************/

    if (role == 1)  {
        radio.stopListening();// First, stop listening so we can talk.

        if (AuthState == 1)
        {
          // Authentificated, no alarm necessary
          // here it goes the transmission
          const char text[5] = "4679";
          radio.write(&text, sizeof(text));
        }
        else
        {
          // Not authentificated, set alarm
          // here it goes the transmission
          const char text[5] = "1762";
          radio.write(&text, sizeof(text));
        }

        /*
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
        */
    }
    /****************** Pong Back Role ***************************/
 
    if ( role == 0 )
    {
        unsigned long got_time;
        if( radio.available())
        {
            char text[32] = "";
            radio.read(&text, sizeof(text)); 
            variable =atoi(text);
            if(variable == 4517)
            {
              state = 1;
              long now = micros();
              //Serial.println(now - ago);
              Serial.println("received");
              digitalWrite(LED_blue,HIGH);
              delay(100);
              digitalWrite(LED_blue,LOW);

              if (AuthState == 1)
              {
                role = 1; //here you can introduce the role switch                
              }

 
            }
            else
            {
              role = 0;
            }
            
            Serial.println(text);
            Serial.println(variable);    
            Serial.println(variable*2);
            Serial.println("Waiting");

            /*         
            // Variable for the received timestamp
            while (radio.available()) {                                   // While there is data ready
                radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
            }
            radio.stopListening();                                        // First, stop listening so we can talk
            radio.write( &got_time, sizeof(unsigned long) );              // Send the final one back.
            radio.startListening();                                       // Now, resume listening so we catch the next packets.
            Serial.print(F("Sent response "));
            Serial.println(got_time);
            */
        }
    }
    /****************** Change Roles via Serial Commands ***************************/
   /* the following code has to be altered, so that instead of pressing key 'T' 
    * or 'R' the received number is being turned into a key which changes the role  
    * by switching the variable role to 1 or 0
    * Then, the status of the "Authentification" is being pong back.
    * The Authentification will be taken simply from the pushbutton status
    * you might have to copy the code into other parts of the program
    */
    
    
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
