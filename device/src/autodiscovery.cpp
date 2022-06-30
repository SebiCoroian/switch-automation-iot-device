/**
 * src/autodiscovery.cpp
 * 
 * Auto-discovery for devices.
 */

#include "autodiscovery.hpp"

#include <ESP8266WiFi.h>

#include "log.hpp"
#include "mqtt.hpp"

void autodiscovery_init() {
    log_debug("initialized autodiscovery");
}

void autodiscovery_send_notice(const char * device_id, const char * device_name) {
    // static buffer for formatting the autodiscovery payload
    static char buf[256];
    // format the autodiscovery payload into the buffer
    sprintf(buf, "%s%s%s", device_id, AUTODISCOVERY_PAYLOAD_SEPARATOR, device_name);
    // publish the autodiscovery payload to the autodiscovery topic
    mqtt_publish(AUTODISCOVERY_TOPIC, buf);
    log_debug("sent autodiscovery payload");
}