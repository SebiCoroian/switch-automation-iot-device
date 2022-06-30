/**
 * src/controls.cpp
 * 
 * Auto-discovery for devices.
 */

#include "controls.hpp"

#include <ESP8266WiFi.h>
#include <Servo.h>

#include "log.hpp"
#include "mqtt.hpp"

Servo servo; 

// MQTT topic for receiving controls.
const char * controls_mqtt_topic = NULL;

// Function that receives control messages over mqtt.
int controls_mqtt_consume(const char *topic, const byte *payload, unsigned int len);

void controls_init() {
    // attach servo to pin
    servo.attach(CONTROLS_SERVO_PIN);
    // register control consumer
    mqtt_register_consumer(controls_mqtt_consume);
    log_info("initialized controls");
}

void controls_set_control_mqtt_topic(const char * topic) {
    mqtt_subcribe(topic);
    if (controls_mqtt_topic != NULL) {
        mqtt_unsubscribe(controls_mqtt_topic);
    }
    controls_mqtt_topic = topic;
}

int controls_mqtt_consume(const char *topic, const byte *payload, unsigned int len) {
    static char buf[12];
    if (strcmp(topic, controls_mqtt_topic)) {
        log_debug("not a control message");
        return MQTT_MESSAGE_NOT_CONSUMED;
    }
    if (len == 0) {
        log_debug("empty control message");
        return MQTT_MESSAGE_CONSUMED;
    }
    memcpy(buf, payload, len * sizeof(char));
    int angle = atoi(buf);
    if (angle < CONTROLS_ANGLE_MIN || angle > CONTROLS_ANGLE_MAX) {
        log_debug("illegal control message");
        return MQTT_MESSAGE_CONSUMED;
    }
    servo.write(angle);
    return MQTT_MESSAGE_CONSUMED;
}