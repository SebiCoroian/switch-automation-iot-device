/**
 * include/mqtt.hpp
 * 
 * Connect and use MQTT on the device.
 */

#include <ESP8266WiFi.h>

#ifndef MQTT_HPP_
#define MQTT_HPP_

// MQTT server ip.
#define MQTT_SERVER_IP ((const char *)("192.168.123.59"))
// MQTT server port.
#define MQTT_SERVER_PORT ((uint16_t)(1883))

// Marker that specifies an MQTT message was consumed.
#define MQTT_MESSAGE_CONSUMED ((uint8_t)(0))
// Marker that specifies an MQTT message was not consumed. 
#define MQTT_MESSAGE_NOT_CONSUMED ((uint8_t)(1))
// // Exponential backoff for establishing a connection to MQTT.
// #define MQTT_CONNECTION_BACKOFF_MS ((uint8_t)(250))

// Pointer to a function that may or may not consume an MQTT message.
//
// The consumer should return MQTT_MESSAGE_CONSUMED if the message was consumed
// or MQTT_MESSAGE_NOT_CONSUMED otherwise.
typedef int (*mqtt_consume_t)(const char *topic, const byte *payload, unsigned int len);

// Initialize mqtt.
void mqtt_init(const char * client_id);

// Register an MQTT consumer in the list of the MQTT consumer.
void mqtt_register_consumer(mqtt_consume_t consumer);

// Run MQTT loop.
void mqtt_loop();

// Publish a message via MQTT.
void mqtt_publish(const char * topic, const char * payload);

// Subscribe to a topic on MQTT.
void mqtt_subcribe(const char * topic);

// Unsubscribe from a topic on MQTT.
void mqtt_unsubscribe(const char * topic);

#endif // MQTT_HPP_