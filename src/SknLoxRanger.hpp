/**
 * Automaton Class for VL53L1x.
 * - Atm_digital pulse;
 */

#pragma once

#include <Arduino.h>
#include <VL53L1X.h>
#include <Preferences.h>

class SknLoxRanger {

public:
  SknLoxRanger( unsigned int timingBudgetMS =250, unsigned int interMeasurementMS = 1000 );

  enum eDirection {MOVING_UP,MOVING_DOWN,STOPPED};

  SknLoxRanger& begin();
  SknLoxRanger& start();
  SknLoxRanger& stop();
   unsigned int relativeDistance(bool wait=false);
     eDirection movement();
    const char* movementString();
  SknLoxRanger& loop();
   unsigned int currentMM() { return uiDistanceValueMM; };  
   unsigned int currentPos() { return uiDistanceValuePos; };  


protected:
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
  const char *cDir[3] = {"MOVING_UP","MOVING_DOWN","STOPPED"};
  #define MAX_SAMPLES 5
      const int capacity = (MAX_SAMPLES);
  unsigned  int distances[MAX_SAMPLES + 2];
     const char *cSknRangerID = "Ranger";     // memory key

  VL53L1X::RangingData sDat, *PSDat;
  // {
  //   uint16_t range_mm;
  //   RangeStatus range_status;
  //   float peak_signal_count_rate_MCPS;
  //   float ambient_count_rate_MCPS;
  // };

      VL53L1X lox;                         // Ranging Device
  Preferences prefs;                       // stored ranger limit min - max
};
