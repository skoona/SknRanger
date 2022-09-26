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
#elif defined(ARDUINO_ESP8266_ESP01)
// esp01 1m version
  #define SDA  1
  #define SCL  3
  #define ATM_PLAYER_DISABLE_TONE   // disable some Automaton Ardunio issues (noTone, Tone, AnalogWrite)
  #undef LED_BUILTIN
  #define LED_BUILTIN 3
#endif


#define SKN_MOD_NAME "Garage Door Position Monitor"
#define SKN_MOD_VERSION "1.3.0"
#define SKN_MOD_BRAND "SknSensors"

#define SKN_TITLE "Garage Door Position"
#define SKN_TYPE "Rollershutter"
#define SKN_ID "SknRanger"

#define LOX_TIMING_BUDGET_US    250000        // = 250 ms
#define LOX_INTERMEASUREMENT_MS    500        // = 500 ms

 /* *
  *
  * guard-flag to prevent sending properties when mqtt is offline
  * 
*/
volatile bool gbEnableDoorOperations=false;

/* 
 * Homie Nodes */
SknLoxRanger nodePos(SKN_ID, SKN_TITLE, SKN_TYPE, LOX_TIMING_BUDGET_US, LOX_INTERMEASUREMENT_MS); // communication interface

/**
 * look for events that block sending property info */
void onHomieEvent(const HomieEvent& event) {
  switch (event.type) {
    case HomieEventType::MQTT_READY:
      Serial << "MQTT connected" << endl;
      gbEnableDoorOperations=true;
      nodePos.start();
      break;
    case HomieEventType::MQTT_DISCONNECTED:
      Serial << "MQTT disconnected, reason: " << (int8_t)event.mqttReason << endl;
      gbEnableDoorOperations=false;
      nodePos.stop();
      break;
    case HomieEventType::SENDING_STATISTICS:
      Serial << "Sending statistics" << endl;
      nodePos.broadcastStatus();
      if(!nodePos.isAutoLearn()) {   // if not autolearning, cycle ranging with stats
        if(nodePos.isActive()) {
          nodePos.stop();
        } else {
          nodePos.start();
        }
      }
      break;
    case HomieEventType::OTA_STARTED:
      nodePos.stop();
      break;
    case HomieEventType::OTA_SUCCESSFUL:
    case HomieEventType::OTA_FAILED:
      nodePos.stop();
      break;
  }
}

/*
 * Callback for Homie Broadcasts */
bool broadcastHandler(const String &level, const String &value)
{
  Homie.getLogger() << "Received broadcast level " << level << ": " << value << endl;
  return true;
}

/*
 * Arduino Setup: Initialze Homie */
void setup()
{
  delay(100);
  Serial.begin(115200);
  delay(100);
  
  if (!Serial)
  {
    Homie.disableLogging();
  }

  Wire.begin(SDA, SCL);

  Homie_setFirmware(SKN_MOD_NAME, SKN_MOD_VERSION);
  Homie_setBrand(SKN_MOD_BRAND);
  
  Homie.setBroadcastHandler(broadcastHandler)
      .setLedPin(LED_BUILTIN, LOW)
      .disableResetTrigger()
      .onEvent(onHomieEvent);

  Homie.setup();
}

/*
 * Arduino Loop: Cycles Homie Nodes */
void loop()
{
  Homie.loop();
}
