/**
 * src/mqtt.cpp
 * 
 * Auto-discovery for devices.
 */

#include "mqtt.hpp"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<WiFiClient.h>

#include "autodiscovery.hpp"
#include "log.hpp"

// WiFi client.
WiFiClient wifi_client;
// MQTT client.
PubSubClient client(wifi_client);

// Capacity of the consumer list.
#define CONSUMER_LIST_CAPACITY ((uint8_t)(4))
// List of consumers.
mqtt_consume_t consumer_list[CONSUMER_LIST_CAPACITY];
// Lenght of the consumer list.
uint8_t consumer_list_len = 0;

// MQTT callback used internally.
//
// This will try to dispatch the event to any registered consumer, stopping at
// the first one that reports successfully consuming the message.
void mqtt_callback(char * topic, byte *payload, unsigned int len);


void mqtt_init(const char * client_id) {
    client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
    client.setCallback(mqtt_callback);
    if (!client.connect(client_id)) {
        log_info("mqtt connection failed, please reset the board");
        while (true) {}
    }
    log_debug("initialized mqtt");
}

void mqtt_register_consumer(mqtt_consume_t consumer) {
    if (consumer_list_len + 1 > CONSUMER_LIST_CAPACITY) {
        log_info("mqtt consumer list overflowed");
        while (true) {}
    }
    consumer_list[consumer_list_len] = consumer;
    consumer_list_len += 1;
}

void mqtt_loop() {
    client.loop();
}

void mqtt_publish(const char * topic, const char * payload) {
    client.publish(topic, payload);
}

void mqtt_callback(char * topic, byte *payload, unsigned int len) {
    for (uint8_t i = 0; i < consumer_list_len; i++) {
        mqtt_consume_t consumer = consumer_list[i];
        if (consumer(topic, payload, len) == MQTT_MESSAGE_CONSUMED) {
            return;
        }
    }
    log_debug("no consumers consumed this message");
}

// uint8_t mqtt_connect(const char * device_uid, const char * device_name)
// {
//     static auto backoff_ms = MQTT_CONNECTION_BACKOFF_MS;

//     log_debug("connecting to mqtt");

//     if (client.connect(device_uid, "", "")) {
//         backoff_ms = 250;
//         autodiscovery_send_notice(device_uid, device_name);
//         log_info("connected to mqtt");
//     } else {
//         log_info("failed to connect to mqtt");
//         delay(backoff_ms);
//         backoff_ms *= 2;
//     }

//     return backoff_ms == MQTT_CONNECTION_BACKOFF_MS;
// }

void mqtt_subcribe(const char * topic) {
    client.subscribe(topic);
}

void mqtt_unsubscribe(const char * topic) {
    client.unsubscribe(topic);
}
