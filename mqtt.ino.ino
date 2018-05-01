#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt protocol support

// Required for LIGHT_SLEEP_T mode
extern "C" {
#include "user_interface.h"
}

//Wifi credentials
#define wifi_ssid "naanu unknown"
#define wifi_password "Noexcuse$#@!"

//mqtt server settings
#define mqtt_server "192.168.43.93" // local server IP which runs musquitto
#define mqtt_user ""
#define mqtt_password ""

#define motion_topic "sensor/motion1"

#define motion_pin 2  //PIR is connected to GPIO 2

int motionState = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Serial.println("Boot complete!");
  Serial.println("Stabilizing PIR sensor...");
  //delay(5000);
  setup_wifi();          // Connect to WiFi
  client.setServer(mqtt_server, 1883);  //Set mqtt server IP and port

  pinMode(motion_pin, INPUT);
  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
//    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
//      Serial.println("Connected to MQTT server!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();  // connect to mqtt server
  }
  client.loop();
  // Read the PIR sensor
  int currentRead = digitalRead(motion_pin);
  Serial.println(currentRead);
  // If motion is detected we don't want to 'spam' the service
  // Send only state change
  if(currentRead != motionState) {

    // Change in state 
    motionState = currentRead;
    String message = motionState ? "controlOn" : "controlOff";
    Serial.print("New Motion:");
    Serial.println(String(message).c_str());
    client.publish(motion_topic, String(message).c_str(), true);
  }

  wifi_set_sleep_type(LIGHT_SLEEP_T); //Light sleep, wifi Modem is sent to sleep when possible. CPU sleeps when possible(when nothing is there to do!!)
  delay(1000);  
  
}

