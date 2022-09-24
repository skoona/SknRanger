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
  SknLoxRanger( int gpio_pin, unsigned int timingBudgetMS =250, unsigned int interMeasurementMS = 1000 );

  enum eDirection {MOVING_UP,MOVING_DOWN,STOPPED};

  SknLoxRanger& begin();
  SknLoxRanger& start();
           void stop();
   unsigned int relativeDistance(bool wait=false);
     eDirection movement();
    const char* movementString();
  SknLoxRanger& loop(bool wait=true) ;

           bool isDataReady();
           void setDataReady ();

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
  unsigned int readValue(bool wait=false);
           int iLimitMin = MM_MIN;    // logical UP,   or closest to sensor
           int iLimitMax = MM_MAX;    // logical DOWN, or farest from sensor
  
  unsigned int uiDistanceValueMM = 0;
  unsigned int uiTimingBudget;
  unsigned int uiInterMeasurement;
           int iDataReadyPin;         // TODO: Wire this up


private :   
  const char *cDir[3] = {"MOVING_UP","MOVING_DOWN","STOPPED"};
  #define MAX_SAMPLES 5
      const int capacity = (MAX_SAMPLES);
  unsigned  int distances[MAX_SAMPLES + 2];
  unsigned long time_now = millis();
     const char *cSknRangerID = "Ranger";     // Door Positon Label; UP, DOWN, STOPPED,...
  volatile bool bDataReady;

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
