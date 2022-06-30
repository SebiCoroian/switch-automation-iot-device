/**
 * src/log.cpp
 * 
 * Logging utilities.
 */

#include "log.hpp"

#include <ESP8266WiFi.h>

// Identifier for the debug log level.
#define LOG_LEVEL_DEBUG ((const char *)("DEBUG"))
// Identifier for the info log level.
#define LOG_LEVEL_INFO ((const char *)("INFO"))

// Pointer to a logging function.
typedef void (*log_func_t)(const char *);

// Log a message.
void log(const char * level, const char * msg);

// Implementation for the log_debug function.
//
// This is used when debug logging is enabled.
void log_debug_impl(const char * msg);

// Dummy implementation for the log_debug function.
//
// This is used when debug logging is disabled.
void log_debug_dummy(const char * msg);

// Variable that holds the pointer to the actual log_debug implementation.
log_func_t _log_debug;

void log_init() {
    // begin serial communication
    Serial.begin(SERIAL_BAUD_RATE);

    // set up debug logging pin and read whether debug logs should be enabled
    // or not
    pinMode(LOG_ENABLE_DEBUG_PIN, INPUT); // initialize debug log pin
    int enable_debug = digitalRead(LOG_ENABLE_DEBUG_PIN); // read pin value
    if (enable_debug) { // if debug is enabled
        // set _log_debug function pointer to the actual impl
        _log_debug = log_debug_impl; 
    } else { // otherwise
        // set _log_debug to the dummy impl
        _log_debug = log_debug_dummy; 
    }
}

void log_debug(const char * msg) {
    // call the actual log_debug function.
    //
    // this uses either the dummy implementation or the actual one, see
    // log_init.
    _log_debug(msg);
}

void log_info(const char * msg) {
    // log the message
    log(LOG_LEVEL_INFO, msg);
}

void log_debug_impl(const char * msg) {
    // log the message
    log(LOG_LEVEL_DEBUG, msg);
}

void log_debug_dummy(const char * msg) {
    // don't do anything
}

void log(const char * level, const char * msg) {
    // static log message buffer
    static char buf[1024];
    // print the log message to the buffer
    sprintf(buf, "[%s] %s\n", level, msg);
    // print the log message to serial
    Serial.print(buf);
}