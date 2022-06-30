/**
 * include/log.hpp
 * 
 * Logging utilities.
 */

#include <ESP8266WiFi.h>

#ifndef LOG_HPP_
#define LOG_HPP_

// Baud rate used for communicating over the serial connection.
#define SERIAL_BAUD_RATE ((long)(115200))

// Pin that, if set to high on startup, will enable debug messages.
#define LOG_ENABLE_DEBUG_PIN ((const uint8_t)(D2)) // changeme

// Initialize logging.
void log_init();

// Log a debug message.
void log_debug(const char * msg);

// Log an informational message.
void log_info(const char * msg);

#endif // LOG_HPP_