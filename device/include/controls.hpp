/**
 * include/controls.hpp
 * 
 * Control the actions of the device.
 */

#include <ESP8266WiFi.h>

#ifndef CONTROLS_HPP_
#define CONTROLS_HPP_

// Pin connected to a servo.
#define CONTROLS_SERVO_PIN (D6)

#define CONTROLS_ANGLE_MIN 0   // minimum angle that can be received over MQTT
#define CONTROLS_ANGLE_MAX 180 // maximum angle that can be received over MQTT

// Initialize device controls.
void controls_init();

// Set the topic on which control commands will be received via MQTT.
void controls_set_control_mqtt_topic(const char * topic);

// Set the angle of the servo that enables device controls.
void controls_set_angle(int angle);

#endif // CONTROLS_HPP_