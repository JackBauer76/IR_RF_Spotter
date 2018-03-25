// V0 just Tiny with RF


#include <VirtualWire_Config.h>
#include <VirtualWire.h>


char *controller;

void setup() {
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(0);  // DATA to PIN 10
  vw_setup(1000);// speed of data transfer Kbps

}

void loop() {
  controller="8" ; // This is the message sent, in our case we send a 1
  vw_send((uint8_t *)controller, strlen(controller));
  vw_wait_tx(); // Wait until the whole message is gone 

}
