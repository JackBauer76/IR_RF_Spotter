// ATtiny85 sleep mode, wake on pin change interrupt demo
// Author: Nick Gammon
// Date: 12 October 2013

// ATMEL ATTINY 25/45/85 / ARDUINO
//
//                  +-\/-+
// Ain0 (D 5) PB5  1|    |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+
// PCB Version:
// PIR PIR 2
// RF 0



#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <VirtualWire_Config.h>
#include <VirtualWire.h>
char *controller;
const byte LED = 3;  // pin 2
const byte PIR = 2; // pin 3 / PCINT2
const byte RF = 0; // pin 0 


ISR (PCINT0_vect) 
 {
 // do something interesting here
 }
 
void setup ()
  {
  pinMode (LED, OUTPUT);
  pinMode (PIR, INPUT);
    
  // pin change interrupt (example for D4)
  PCMSK  |= bit (PCINT2);  // want pin D4 / pin 3
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts 
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(0);  // DATA to PIN 10
  vw_setup(1000);// speed of data transfer Kbps
  }  // end of setup


void RFbeeping(){
  for(int k=0;k<150;k++){  
  controller="8" ; // This is the message sent, in our case we send a 1
  vw_send((uint8_t *)controller, strlen(controller));
  vw_wait_tx(); // Wait until the whole message is gone 
  digitalWrite (LED, HIGH);
  delay (100); 
  digitalWrite (LED, LOW);
  delay (100); 

}



void loop ()
  {
    RFbeeping()
    }
  
  
  goToSleep ();
  }  // end of loop
  
  
void goToSleep ()
  {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();                             
  sleep_disable();   
  power_all_enable();    // power everything back on
  }  // end of goToSleep 
