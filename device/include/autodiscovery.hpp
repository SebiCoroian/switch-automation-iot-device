/**
 * include/autodiscovery.hpp
 * 
 * Auto-discover the device with the server.
 */

#ifndef AUTODISCOVERY_HPP_
#define AUTODISCOVERY_HPP_

// MQTT topic where device should send payloads for server-side autodiscovery.
#define AUTODISCOVERY_TOPIC ((const char *)("connected-device"))
// Separator for autodiscovery payload fields.
#define AUTODISCOVERY_PAYLOAD_SEPARATOR ((const char *)(";"))

// Initialize autodiscovery.
void autodiscovery_init();

// Send an autodiscovery notice.
//
// This is used for automatically registering devices on startup.
void autodiscovery_send_notice(const char * device_id, const char * device_name);

#endif // AUTODISCOVERY_HPP_