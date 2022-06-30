/**
 * main.cpp
 */

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "autodiscovery.hpp"
#include "controls.hpp"
#include "log.hpp"
#include "mqtt.hpp"

// Unique device ID.
String device_id = ("pusher-" + String(random(0x13af), HEX));
// Password for connecting to the configuration AP.
const char * device_ap_password = "Password1234.";

void setup() {
    // initialize the builtin LED
    pinMode(LED_BUILTIN, OUTPUT);
    // turn on the builtin LED to mark the start of the setup process
    digitalWrite(LED_BUILTIN, HIGH);

    log_init(); // initialize logging
    log_info("setup started");

    // ----- initialize wifi -----
    WiFiManager wifiManager;
    // add parameter for having the ability to specify a custom device name
    WiFiManagerParameter custom_name("name", "device name", "Wi-Fi Pusher", 30);
    wifiManager.addParameter(&custom_name);
    
    // attempt to auto-connect to wi-fi
    if (false) { // set this to false to avoid starting the config portal each time
        if (!wifiManager.startConfigPortal("Configure wi-fi pusher", device_ap_password)) {
            log_info("failed to connect and hit timeout, resetting in 3 seconds..");
            delay(3000);
            ESP.reset();
        }
    } else if (!wifiManager.autoConnect("Configure wi-fi pusher", device_ap_password)) {
        log_info("failed to connect and hit timeout, resetting in 3 seconds..");
        delay(3000);
        ESP.reset();
    }
    
    // ---------------------------

    mqtt_init(device_id.c_str()); // initialize the MQTT connection
    controls_init();      // initialize device controls
    controls_set_control_mqtt_topic(device_id.c_str());
    autodiscovery_init(); // initialize device autodiscovery

    // send the autodiscovery notice
    autodiscovery_send_notice(device_id.c_str(), custom_name.getValue());

    log_info("setup complete");
    // turn off the led to mark the end of the setup process
    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
    // run the mqtt loop
    mqtt_loop();
}
