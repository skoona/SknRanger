/**
  * Main.cpp
  *
*/

#include "SknLoxRanger.hpp"
#include <Wire.h>


#ifndef ARDUINO_ESP32_DEV
extern "C"
{
#include <user_interface.h>
}
#endif

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
// D1_mini Pins
  #define SDA 4        // D2
  #define SCL 5        // D1
  #define LOX_GPIO   13    // D7
  #define RELAY_GPIO 12    // D6
  #undef LED_BUILTIN
  #define LED_BUILTIN 2
#elif defined(ARDUINO_ESP8266_ESP12)
// Esp8266EX  pins
  #define SDA 5
  #define SCL 4
  #define LOX_GPIO   13    // D7
  #define RELAY_GPIO 12    // D6
  #undef LED_BUILTIN
  #define LED_BUILTIN 2
#elif defined(ARDUINO_ESP8266_NODEMCU)
// nodemcu esp12f  pins  -- ESP8266 Relay Module, ESP-12F Development Board AC 220V DC 12V Single relay module
  #define SDA        14
  #define SCL        12
  #define LOX_GPIO   13    // XSHUT Pin
  #define RELAY_GPIO  5   
  #undef LED_BUILTIN
  #define LED_BUILTIN 16
#elif defined(ARDUINO_ESP32_DEV)
// esp32doit-devkit-v1  pins
  #define SDA 21
  #define SCL 22
  #define ATM_PLAYER_DISABLE_TONE   // disable some Automaton Ardunio issues (noTone, Tone, AnalogWrite)
  #define LOX_GPIO   13    // D7
  #define RELAY_GPIO 12    // D6
  #undef LED_BUILTIN
  #define LED_BUILTIN 4
#endif

/*
 * create the base class instance micros, milli */
SknLoxRanger lox(250000, 500);

/*
 * Arduino Setup: Initialze Homie */
void setup() {
  delay(100);
  Serial.begin(115200);
  delay(100);
  
  Wire.begin(SDA, SCL);
  lox.begin().start();
}

/*
 * Arduino Loop: Cycles Homie Nodes */
void loop() {
  lox.loop();
}
