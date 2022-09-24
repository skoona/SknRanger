/**
 * Automaton Class for VL53L1x.
 *
 */
#include "SknLoxRanger.hpp"

SknLoxRanger::SknLoxRanger(uint8_t resetPin, unsigned int timingBudgetMS, unsigned int interMeasurementMS)  {
  loxResetPin = resetPin;
  uiTimingBudget=(timingBudgetMS * 1000); // required in micros
  uiInterMeasurement=interMeasurementMS;

  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, HIGH);  // Keep this HIGH as we need to pull it low when we reset the sensor

  /*
   * Initialze averaging array */
  for (int idx = 0; idx < (MAX_SAMPLES); idx++) {
      distances[idx] = 0;
  }

  limitsRestore();
}

SknLoxRanger& SknLoxRanger::toogleShutdown(uint8_t resetPin) {
  digitalWrite(resetPin, LOW);
  delay(1000);
  digitalWrite(resetPin, HIGH);
  delay(1000);
  Serial.printf(" ✖  SknLoxRanger toggled shutdown pin.\n");
  return *this;
}

/**
 * @brief Processing Loop
 * 
 */
SknLoxRanger& SknLoxRanger::loop() {
      relativeDistance(true);   
  return *this;
}

/**
 * VL53L1x Device Init
 * - delay(1000); Delay is not working in this class ???
 * - one reading every second
 */
SknLoxRanger& SknLoxRanger::begin( ) {
  Serial.printf(" ✖  SknLoxRanger initialization starting.\n");
  int beforeRetry = 0;

  while (!lox.init()) {
    Serial.printf(" ✖  Failed to detect and initialize sensor!\n");
    delay(1000);
    beforeRetry++;
    if(beforeRetry>RETRIES_BEFORE_RESET) {
      toogleShutdown(loxResetPin);
      beforeRetry=0;
    }
  }
  Serial.printf(" ✖  Exited initialize sensor!\n");

  lox.setTimeout(uiInterMeasurement+uiTimingBudget);

  if (lox.setDistanceMode(VL53L1X::Medium)) {  
    Serial.printf("〽 Medium distance mode accepted.\n");
  }

  if (lox.setMeasurementTimingBudget(uiTimingBudget)) {
    Serial.printf("〽 %ums timing budget accepted.\n", uiTimingBudget/1000);
  } 
  Serial.printf(" ✖  SknLoxRanger initialization Complete.\n");

  return(*this);
}

/**
 * @brief Start device
 * 
 *  250ms read insode 1000ms wait = cycle time of 1000ms
*/
SknLoxRanger&  SknLoxRanger::start() {
  lox.startContinuous(uiInterMeasurement);
  Serial.printf(" ✖  SknLoxRanger startContinuous(%ums) accepted.\n", uiInterMeasurement);
  return *this;
}

/**
 * Stop device
*/
SknLoxRanger& SknLoxRanger::stop() {
  lox.stopContinuous();  
  Serial.printf(" ✖  SknLoxRanger stopContinuous() accepted.\n");
  return *this;
}

/**
 * determine direction of movement
*/
SknLoxRanger::eDirection SknLoxRanger::movement() {
  eDirection eDir = STOPPED;
  /* 
    * a > b = UP
    * a < b = DOWN 
    * a == b = STOPPED */
  if (distances[0]  >  distances[capacity]) { eDir=MOVING_UP; }
  if (distances[0]  <  distances[capacity]) { eDir=MOVING_DOWN; }
  if (distances[0]  == distances[capacity]) { eDir=STOPPED; }

  return eDir;
}

/**
 * @brief movement converted to string
 * 
 */
const char* SknLoxRanger::movementString() {
  return cDir[ movement() ];
}

/**
 * @brief read value when available ready and return average value.
 * 
 * - array was initialized in class init.
 * - gpio is active LOW
 * - affect uiDistanceValueMM if valid, return avgerage
 * - copied result struct if valid
 * - return raw value if invalid
*/
unsigned int SknLoxRanger::readValue(bool wait)
{
  unsigned long sum = 0;
  unsigned long avg = 0;
  unsigned int value = 0;

  // value = (unsigned int)lox.readRangeContinuousMillimeters(wait);
  value = (unsigned int)lox.read(wait);
  
  if (lox.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  if (lox.ranging_data.range_status == 0)
  {
    sDat.range_mm = lox.ranging_data.range_mm;
    sDat.range_status = lox.ranging_data.range_status;
    sDat.peak_signal_count_rate_MCPS = lox.ranging_data.peak_signal_count_rate_MCPS;
    sDat.ambient_count_rate_MCPS = lox.ranging_data.ambient_count_rate_MCPS;

    for (int idx = 0; idx < capacity; idx++)
    {
      distances[idx] = distances[idx + 1]; // move all down
      sum += distances[idx];
    }

    distances[capacity] = value;
    avg = (sum / capacity);
    uiDistanceValueMM = (unsigned int)avg;
  }
  else
  {
    distances[capacity] = value;
    return 0;
  }

  Serial.printf("〽 range: %u mm  avg: %lu mm\tstatus: %s\traw status: %u\tsignal: %3.1f MCPS\tambient: %3.1f MCPS\tmove: %s\n",
                    value,
                    avg,
                    lox.rangeStatusToString(sDat.range_status),
                    sDat.range_status,
                    sDat.peak_signal_count_rate_MCPS,
                    sDat.ambient_count_rate_MCPS,
                    movementString()); 
  
  return avg;
}

/**
 * Transform mm to relative range
 */
unsigned int SknLoxRanger::relativeDistance(bool wait) {
  long posValue;
  long mmPos;
  
  mmPos = (long)readValue(wait);

  if(mmPos==0) return uiDistanceValueMM;

  posValue = map(mmPos, iLimitMin, iLimitMax, 0, 100);
  
  mmPos = constrain( posValue, 0, 100);
  uiDistanceValuePos = mmPos;

  Serial.printf(" ✖  SknLoxRanger relativeDistance(%ld %%) accepted.\n", mmPos);

  return (unsigned int) mmPos;
}

/*
  * Save position limits */
bool SknLoxRanger::limitsSave() {
  bool rc = false;
  prefs.begin(cSknRangerID, false);
      prefs.putInt("mm_min", iLimitMin);
      prefs.putInt("mm_max", iLimitMax);
      rc = prefs.isKey("mm_min");
  prefs.end();
  
  Serial.printf("〽  SknLoxRanger::limitsSave(%s) \tmin: %d \tmax: %d\n", (rc ? "True": "False"), iLimitMin, iLimitMax);
  return rc;
}

 /*
  * Restore position limits */
bool SknLoxRanger::limitsRestore() {
  bool rc = false;
  prefs.begin(cSknRangerID, false);
  if (prefs.isKey("mm_min")) {
    iLimitMin = prefs.getInt("mm_min", MM_MIN);
    iLimitMax = prefs.getInt("mm_max", MM_MAX);
    rc = true;
  }
  prefs.end();

  Serial.printf("〽  SknLoxRanger::limitsRestore(%s) \tmin: %d \tmax: %d\n", (rc ? "True": "False"), iLimitMin, iLimitMax);
  return rc;
}