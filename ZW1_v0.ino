/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

To load a programm into the Huzzah, first maintain a reset pressed and then press GPIOE (at the
left of Resetm with the Pins seeing at the top, at the end release reset

go to to see the dashboard:
https://io.adafruit.com/pioniere76/dashboards/mytutorialdashboard


  
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "EasyBox-286702"
#define WLAN_PASS       "Merkava4"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "pioniere76"
#define AIO_KEY         "4436b9927a8e4db4bc7aa87d402da928"

/******************* Arduino variables****************************************/
const int ledPin = 13;
const int buzzerPin = 14;
const int pirPin = 12; // choose the input pin (for PIR sensor)
bool motionDet;  
uint16_t sliderfreq=6666;
uint16_t onoffval=88;


/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

const char PIRSENSOR1_FEED[] PROGMEM = AIO_USERNAME "/feeds/pirsensor1"; //1.12.17 E.Calva inserted the new feed

//const char ONOFF_FEED[] PROGMEM = AIO_USERNAME "/feeds/onoff";  //1.12.17 E.Calva inserted the new feed


// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");
Adafruit_MQTT_Publish chococell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/chococell");
Adafruit_MQTT_Publish pirsensor1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pirsensor1");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");
Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/slider");
/*************************** Sketch Code ************************************/
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
  mqtt.subscribe(&slider);

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  uint8_t z=0;
 
} //////////////////// End of Setup ////////////////////////////////


void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
        Serial.print(F("OnOff Button: "));
        Serial.println((char *)onoffbutton.lastread);
        onoffval = atoi((char *)onoffbutton.lastread);  // convert to a number
      
    }
    
    if (subscription == &slider) {
      Serial.print(F("Slider: "));
      Serial.println((char *)slider.lastread);
      sliderfreq = atoi((char *)slider.lastread);  // convert to a number
      tone(buzzerPin, sliderfreq);
    }
    }



  
bool motionDet = digitalRead(pirPin); // need to implement a toogling safe motionDet

Serial.print(F("the slider frequency is : "));
Serial.print(sliderfreq);
Serial.print(F("the onoff variable is "));
Serial.print(onoffval);
  if (motionDet == HIGH && onoffval == 1)  
    {
    tone(buzzerPin,sliderfreq);  
    digitalWrite(ledPin,HIGH);
    delay(2500);
    }

  else 
    {
    noTone(buzzerPin);
    digitalWrite(ledPin,LOW);
    }



  
  if (! photocell.publish(sliderfreq / 1000)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }


int8_t z = digitalRead(pirPin);
  // Now we can publish stuff!
  Serial.print(F("\nSending pirsensor1 data "));
  Serial.print(z);
  Serial.print("...");
  if (! pirsensor1.publish(z)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

