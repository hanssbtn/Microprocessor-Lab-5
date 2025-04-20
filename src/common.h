#ifndef COMMON_H__
#define COMMON_H__

// #include <setjmp.h>
// #include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#ifdef Arduino_h

#define DEBUG_INIT(baud) Serial.begin(baud)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)

#else
#define DEBUG_INIT(baud) (void)(baud)
#define DEBUG_PRINT(...) (void)(__VA_ARGS__)
#define DEBUG_PRINTLN(...) (void)(__VA_ARGS__)
#ifndef HEX
#define HEX 16
#endif // HEX
#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif // RAD_TO_DEG

#endif // Arduino_h

#endif // COMMON_H__