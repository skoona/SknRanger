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
  #define LOX_GPIO   13    
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
 * create the base class instance */
SknLoxRanger lox(LOX_GPIO, 250, 1000);

/*
 * link inerrupt handler to this class */
SknLoxRanger *instances[] = { &lox, NULL};

/*
 * interrupt handler for ranging */
void IRAM_ATTR loxDataReadyHandler() {
  if ( instances[0] != NULL ) {
    instances[0]->setDataReady ();
  }
}

/*
 * Arduino Setup: Initialze Homie */
void setup() {
  delay(100);
  Serial.begin(115200);
  delay(100);
  
  // FALLING, RISING, CHANGE
  pinMode( LOX_GPIO, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(LOX_GPIO), loxDataReadyHandler, FALLING);

  Wire.begin(SDA, SCL);
  instances[0]->begin().start();
}

/*
 * Arduino Loop: Cycles Homie Nodes */
void loop() {
  instances[0]->loop(true);
}
