                    
/* Transmitter Code
*  Code to read Joystick position and transmit it with a RF24L01+ to a receiver 
*/

#include <SPI.h>
#include <RF24.h>

// Radio Configuration

RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};
bool radioNumber=1;
bool role = 1;  //Control transmit 1/receive 0

// Decide where you are going to plug the joystick into the circuit board.

int ForeAft_Pin = 0;        // Plug Joystick Fore/Aft into Analog pin 0
int LeftRight_Pin = 1;      // Plug Joystick Left/Right into Analog pin 1

// Create variables to read joystick values

float ForeAft_Input ;       // Variable to store data for Fore/Aft input from joystick
float LeftRight_Input ;     // Variable to store data for for Left/Right input from joystick

// Create variables to transmit servo value

int ForeAft_Output;       // Expected range 0 - 180 degrees
int LeftRight_Output;     // Expected range 0 - 180 degrees

// These variables allow for math conversions and later error checking as the program evolves.

int Fore_Limit = 800;       // High ADC Range of Joystick ForeAft
int Aft_Limit = 220;        // Low ADC Range of Joystick ForeAft
int Right_Limit = 800;      // High ADC Range of Joystick LeftRight
int Left_Limit = 226;       // Low ADC Range of Joystick LeftRight

void setup() {
    Serial.begin(9600);                   // Get ready to send data back for debugging purposes
    radio.begin();                        // Get the transmitter ready
    radio.setPALevel(RF24_PA_LOW);        // Set the power to low
    radio.openWritingPipe(addresses[1]);  // Where we send data out
    radio.openReadingPipe(1,addresses[0]);// Where we receive data back
}
void loop() {
    ForeAft_Input = analogRead(ForeAft_Pin) ;             // Read the Fore/Aft joystick value
    LeftRight_Input = analogRead(LeftRight_Pin) ;         // Read the Left/Right joystick value
    ForeAft_Output = convertForeAftToServo(ForeAft_Input) ;        // Convert the Fore/Aft joystick value to a Servo value (0-180)
    LeftRight_Output = convertLeftRightToServo(LeftRight_Input) ;  // Convert the Left/Right joystick value to a Servo value (0-180)

    //  Serial.print(ForeAft_Output);
    radio.stopListening();                                 // Stop listening and begin transmitting
    delay(500);                                            // quite a long delay -- causes jittering of servo
    if(radio.write(&ForeAft_Output, sizeof(ForeAft_Output)),Serial.println("sent ForeAft"));              //Send ForeAft data
    if(radio.write(&LeftRight_Output, sizeof(LeftRight_Output)),Serial.println("sent LeftRight"));        //Send LeftRight data
    radio.startListening();                                // Get ready to receive confirmation from receiver
}

// Function to convert and scale the Fore/Aft data

float convertForeAftToServo(float y) {
    int result;
    result = ((y - Aft_Limit) / (Fore_Limit - Aft_Limit) * 180);
    return result;
}

// Function to convert and scale the Left / Right data
// Can be replaced with Map function
float convertLeftRightToServo(float x) {
    int result;
    result = ((x - Left_Limit) / (Right_Limit - Left_Limit) * 180);
    return result;
}

                  


For reference, here's what that jittery movement looks like. Again, it is caused by the delays in our code:

 

Circuit 2 — Servo Receiver

 

                    
/*  Receiver Code 
*   Code to receive data from RF24L01+ and use it to control a servo */

#include <Servo.h>
#include <SPI.h>
#include <RF24.h>

//Radio Configuration

bool radioNumber=0;
RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};
bool role = 0;  //Control transmit/receive

// Create variables to control servo value

Servo ForeAft;
Servo LeftRight;
unsigned int ForeAft_Output;       // Expected range 0 - 180 degrees
unsigned int LeftRight_Output;     // Expected range 0 - 180 degrees

void setup() { 
    Serial.begin(9600);        // Get ready to send data back for debugging purposes
    ForeAft.attach(6);         // Plug a servo signal line into digital output pin 6
    LeftRight.attach(7);       // Plug a servo signal line into digital output pin 7
    radio.begin();             // Initialize radio
    radio.setPALevel(RF24_PA_LOW);    // Set the power output to low
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.startListening();
}
void loop() {
    delay(500);               //increase for debuggy, decrease to decrease jitter
    if(radio.available()){
        radio.read(&ForeAft_Output,sizeof(ForeAft_Output));
        radio.read(&LeftRight_Output,sizeof(ForeAft_Output));
        } else {Serial.print("No radio");
    }
    Serial.print(ForeAft_Output);
    Serial.print("\t");
    Serial.println(LeftRight_Output);
    ForeAft.write(ForeAft_Output);                          // Command the Fore/Aft servo to a position
    LeftRight.write(LeftRight_Output);                    // Command the Left/Right servo to a position
    }

                  


Next Steps

Quite a few features could be added to this code to make it a bit more robust:

    All joysticks are a bit different and different joysticks have different fore/aft/left/right values. Set the limits well inside what is expected and then possibly introduce a bit of code in the main program loop to push the limits to the threshold of the joystick. For example, if foreaft_input < fore_limit, then fore_limit -= 1. (Tip: Expand limits slowly over time, so the user can adapt to change.)
    A bit of feedback from the receiver circuit could let the joystick circuit know it's connected properly. Perhaps you could add a vibrating motor or an LED that lets the user know the joystick has command of the receiver.
    You could add code that determines what happens when the joystick is disconnected. Should the program continue doing what it was doing before? Should it center the servos? Should it stop transmitting entirely?
    You could add code to the convert/scale functions that detects and handles erroneous input, just in case the joystick malfunctions and sends invalid data.
    You might try the I²C interface for the nRF24L01 and let us know how well it works.

 

   

 

You should now have your very own two-channel remote control! Consider using it with a Sabertooth motor controller to create your own robot platform.

 
Learn More About:
remote control joystick nrf24l01 Wireless arduino
 
Share
Share
Share
Share
Share
You May Also Like:
WISP Wireless Power Platform
The WISP microcontroller has the potential to revolutionize the way small IoT devices are powered and distributed.
Michael Greer
May 07, 2016
Using a Serial LCD with a PICAXE
Using an LCD increases the versatility of any microcontroller by displaying data outputs and memory contents. This serial adapter uses...
Charles R. Hampton
September 22, 2015
Introducing the Particle Photon
The Particle Photon is a powerful little board with the ability to easily connect objects to the internet. This series of articles will be...
Leanna Pancoast
January 07, 2016
Make a GPS Navigation System for a Boat with a Raspberry Pi
If you are a boat owner, I'll guess you've seen some of the prices on the...
Jens Christoffersen
May 03, 2016

Load More
Comments
17 Comments
Login
