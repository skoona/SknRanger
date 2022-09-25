/**
 * Automaton Class for VL53L1x.
 * - Atm_digital pulse;
 */

#pragma once

#include <Homie.hpp>
#include <Wire.h>
#include <VL53L1X.h>
#include <Preferences.h>

class SknLoxRanger  : public HomieNode {

public:
  SknLoxRanger( const char *id, const char *name, const char *cType, unsigned int timingBudgetUS = 250000, unsigned int interMeasurementMS = 1000 );
           bool isActive() { return bActive && bVL53L1xInitialized; }
           bool isAutoLearn() { return (bAutoLearnUp || bAutoLearnDown) && bVL53L1xInitialized; }
           bool isInitialized() { return bVL53L1xInitialized; }
           void broadcastStatus();
    const char* movementString();
  SknLoxRanger& start();
  SknLoxRanger& stop();
  
protected:
  virtual void setup() override;
  virtual void onReadyToOperate() override;
  virtual bool handleInput(const HomieRange& range, const String& property, const String& value);
  virtual void loop() override;


  enum eDirection {MOVING_UP,MOVING_DOWN,STOPPED,LEARNING_UP,LEARNING_DOWN,REBOOTING, EXIT};
  SknLoxRanger& begin();
           void manageAutoLearn(long mmPos);
   unsigned int relativeDistance(bool wait=false);
     eDirection movement();
  SknLoxRanger& vlxLoop();
   unsigned int currentMM() { return uiDistanceValueMM; };  
   unsigned int currentPos() { return uiDistanceValuePos; };  

/*
 * Door travel: 86.5" or 2198 mm
 * Mount point: 13"   or  330 mm
 * maximum range:        2528 mm
*/
#define MM_MIN 330
#define MM_MAX 2528
          bool limitsSave();
          bool limitsRestore();          
  unsigned int readValue(bool wait=true);
           int iLimitMin = MM_MIN;    // logical UP,   or closest to sensor
           int iLimitMax = MM_MAX;    // logical DOWN, or farest from sensor
  
  unsigned int uiDistanceValueMM = 0;
  unsigned int uiDistanceValuePos = 0;
  unsigned int uiTimingBudget;
  unsigned int uiInterMeasurement;


private :   

      VL53L1X lox;                        // Ranging Device
  Preferences prefs;                      // stored ranger limit min - max

  char cBuffer[128];
  const char *cSknRangerID  = "Ranger";   // memory key
  const char *cCurrentState = "DOWN";     // current door state/label
  const char *cCurrentMode  = "Ready";    // current door state/label
  const char *cDir[6]       = {"MOVING_UP","MOVING_DOWN","STOPPED","LEARNING_UP","LEARNING_DOWN","REBOOTING"};
  const char *cMode[4]      = {"READY","AUTO_LEARN_UP","AUTO_LEARN_DOWN","REBOOT"};

  #define MAX_SAMPLES 5
      const int capacity = (MAX_SAMPLES);
  unsigned  int distances[MAX_SAMPLES + 2];
  unsigned long readings = 0;
  unsigned long cycleCount = 0;
  
  #define AUTO_LEARN_READINGS 10
  unsigned long autoLearnUpReadings = 0;
  unsigned long autoLearnDownReadings = 0;
           bool bAutoLearnUp;
           bool bAutoLearnDown;
           bool bActive = false;
           bool bVL53L1xInitialized;

  VL53L1X::RangingData sDat, *PSDat;
  // {
  //   uint16_t range_mm;
  //   RangeStatus range_status;
  //   float peak_signal_count_rate_MCPS;
  //   float ambient_count_rate_MCPS;
  // };


  const char *cCaption   = "• Garage Door Automaton Module:";
  const char *cIndent    = " ✖  ";
  const char *cSknState  = "State";         // Direction name; UP, DOWN, STOPPED,...
  const char *cSknPosID  = "Position";      // Range 0 to 100;
  const char *cSknModeID = "Service";       // service commander to force reboot of node

};
