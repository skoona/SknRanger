/**
 * Automaton Class for VL53L1x.
 *
 */
#include "SknLoxRanger.hpp"

extern volatile bool gbEnableDoorOperations;


SknLoxRanger::SknLoxRanger( const char *id, const char *name, const char *cType, unsigned int timingBudgetUS, unsigned int interMeasurementMS )  
  : HomieNode(id, name, cType)
  {
  uiTimingBudget=timingBudgetUS; // required in micros
  if(timingBudgetUS<1000) {uiTimingBudget=timingBudgetUS*1000; }
  uiInterMeasurement=interMeasurementMS;

  /*
   * Initialze averaging array */
  for (int idx = 0; idx < (MAX_SAMPLES); idx++) {
      distances[idx] = 0;
  }

}

/**
 * @brief Processing Loop
 * 
 */
SknLoxRanger& SknLoxRanger::vlxLoop() {
    if( isReady() ) {
      relativeDistance(true);   
    }
  return *this;
}

/**
 * VL53L1x Device Init
 * - delay(1000); Delay is not working in this class ???
 * - one reading every second
 */
SknLoxRanger& SknLoxRanger::begin( ) {
  if(isInitialized()) return *this;     // no double dips

  limitsRestore(); // load the limits

  Serial.printf("✖  SknLoxRanger initialization starting.\n");
  lox.setTimeout(uiInterMeasurement+(2 * (uiTimingBudget/1000)));

  while (!lox.init()) {
    Serial.printf("✖  Failed to detect and initialize sensor!\n");
    delay(1000);
  }
  Serial.printf(" 〽  Exited initialize sensor!\n");

  lox.setTimeout(uiInterMeasurement+(2 * (uiTimingBudget/1000)));

  if (lox.setDistanceMode(VL53L1X::Long)) {  
    Serial.printf(" 〽 Medium distance mode accepted.\n");
  }

  if (lox.setMeasurementTimingBudget(uiTimingBudget)) {
    Serial.printf(" 〽 %ums timing budget accepted.\n", uiTimingBudget/1000);
  } 
  Serial.printf("✖  SknLoxRanger initialization Complete.\n");
  
  bVL53L1xInitialized=true;
  
  cCurrentState=cDir[OPEN];
  cCurrentState=cMode[ACTIVE];

  return(*this);
}

/**
 * @brief Start device
 * 
 *  250ms read insode 1000ms wait = cycle time of 1000ms
*/
SknLoxRanger&  SknLoxRanger::start() {
  if(!isInitialized()) return *this;
  if(!isActive()) {
    lox.startContinuous(uiInterMeasurement + (2 * (uiTimingBudget/1000)));
    bActive=true;
    cycleCount = readings + 60;
    Serial.printf("✖  SknLoxRanger startContinuous(%ums) accepted.\n", uiInterMeasurement + (2 * (uiTimingBudget/1000)));
  }
  return *this;
}

/**
 * Stop device
*/
SknLoxRanger& SknLoxRanger::stop() {
  if(isActive()) {    
    lox.stopContinuous();  
    bActive=false;
    Serial.printf("✖  SknLoxRanger stopContinuous() accepted.\n");
    broadcastStatus();
  }
  return *this;
}

/**
 * determine direction of movement
*/
SknLoxRanger::eDirection SknLoxRanger::movement() {
  eDirection eDir = OPEN;
  /* 
    * a > b = UP
    * a < b = DOWN 
    * a == b = STOPPED */
  if ( uiDistanceValuePos==0) { eDir=UP; }
  if ( uiDistanceValuePos==100) { eDir=DOWN; }
  
  if(bAutoLearnUp) {
    cCurrentMode=cMode[AUTO_LEARN_UP];
    cCurrentState=cDir[LEARNING];
    eDir=LEARNING;
  } else if(bAutoLearnDown) {
    cCurrentMode=cMode[AUTO_LEARN_DOWN];
    cCurrentState=cDir[LEARNING];
    eDir=LEARNING;
  }
  return eDir;
}

/**
 * @brief movement converted to string
 * 
 */
const char* SknLoxRanger::movementString() {
  return cDir[ movement() ];
}

void SknLoxRanger::manageAutoLearn(long mmPos) {
    /*
   * determine the new ranges and save to eeprom */
  readings++;
  if(bAutoLearnUp) {
    iLimitMin = mmPos;
    cCurrentState=cDir[LEARNING];
    cCurrentMode = cMode[AUTO_LEARN_UP];
    if(readings>=autoLearnUpReadings) {
      bAutoLearnUp=false;
      limitsSave();
      snprintf(cBuffer, sizeof(cBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iLimitMin, iLimitMax);
      cCurrentMode = cBuffer;
      cCurrentState=movementString();
      stop();
      Serial.printf("✖  SknLoxRanger Auto Learn Up(%d mm) accepted.\n", iLimitMin);
    }
  } else if (bAutoLearnDown) {
    iLimitMax = mmPos;
    cCurrentState=cDir[LEARNING];
    cCurrentMode = cMode[AUTO_LEARN_DOWN];
    if(readings>=autoLearnDownReadings) {
      bAutoLearnDown=false;
      limitsSave();
      snprintf(cBuffer, sizeof(cBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iLimitMin, iLimitMax);
      cCurrentMode = cBuffer;
      cCurrentState=movementString();
      stop();
      Serial.printf("✖  SknLoxRanger Auto Learn Down(%d mm) accepted.\n", iLimitMax);
    }
  }

}

/**
 * @brief JSON formatted Position
 * 
 */
const char * SknLoxRanger::formatJSON() {
  snprintf(rangingJSON, sizeof(rangingJSON), "{\"range\":%u,\"average\":%u,\"status\":\"%s\",\"raw_status\":%u,\"signal\":%3.1f,\"ambient\":%3.1f,\"movement\":\"%s\"}",
                uiDistanceValue,
                uiDistanceValueMM,
                lox.rangeStatusToString(sDat.range_status),
                sDat.range_status,
                sDat.peak_signal_count_rate_MCPS,
                sDat.ambient_count_rate_MCPS,
                movementString()); 
  cCurrentJSON = rangingJSON;
  return rangingJSON;
}
/**
 * @brief read value when available ready and return average value.
 * 
 * - array was initialized in class init.
 * - affect uiDistanceValueMM if valid, return avgerage
 * - copied result struct if valid
 * - return 0 value if invalid
*/
unsigned int SknLoxRanger::readValue(bool wait)
{
  unsigned long sum = 0;
  unsigned long avg = 0;
  unsigned int value = 0;

  value = (unsigned int)lox.readRangeContinuousMillimeters(wait);
  
  if (lox.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  if (lox.ranging_data.range_status == VL53L1X::RangeValid)  {
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
    uiDistanceValue = value;
  }  else   {
    distances[capacity] = value;
    avg = 0;
  }

  Serial.printf(" 〽 [%6.0lu] range: %u mm  avg: %lu mm\tstatus: %s\traw status: %u\tsignal: %3.1f MCPS\tambient: %3.1f MCPS\tmove: %s\n",
                    readings,
                    value,
                    avg,
                    lox.rangeStatusToString(lox.ranging_data.range_status),
                    lox.ranging_data.range_status,
                    lox.ranging_data.peak_signal_count_rate_MCPS,
                    lox.ranging_data.ambient_count_rate_MCPS,
                    movementString()); 
  
  return avg;
}

/**
 * Transform mm to relative range
 */
unsigned int SknLoxRanger::relativeDistance(bool wait) {
  long posValue;
  long mmPos;
  
  /*
   * get valid value */
  mmPos = (long)readValue(wait);
  while(mmPos==0) {        // get a vaild value before proceeding
    Serial.printf(" 〽  relativeDistance(0 mm) NOT accepted.\n");
    mmPos = (long)readValue(wait);
  }

  /*
   * determine the new ranges and save to eeprom */
  manageAutoLearn(mmPos);

  /*
   * use fixed ranges while determining new range */
  if(isAutoLearn()) {
    posValue = map(mmPos, MM_MIN, MM_MAX, 0, 100);
  } else {
    posValue = map(mmPos, iLimitMin, iLimitMax, 0, 100);
    if(readings>cycleCount) {
      stop(); 
    } 
  }

  mmPos = constrain( posValue, 0, 100);
  if(mmPos!=uiDistanceValuePos) {
    uiDistanceValuePos = mmPos;
    formatJSON();
    setProperty(cSknPosID).send(String(mmPos));
    setProperty(cSknDetailID).send(cCurrentJSON);
  }
  formatJSON();

  Serial.printf("✖  relativeDistance(%ld %%) accepted.\n", mmPos);

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
  
  Serial.printf(" 〽  limitsSave(%s) \tmin: %d \tmax: %d\n", (rc ? "True": "False"), iLimitMin, iLimitMax);
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

  Serial.printf(" 〽  limitsRestore(%s) \tmin: %d \tmax: %d\n", (rc ? "True": "False"), iLimitMin, iLimitMax);
  return rc;
}

/**
 * Handles the received MQTT messages from Homie.
 *
 */
bool SknLoxRanger::handleInput(const HomieRange& range, const String& property, const String& value) {
  bool rc = false;
  Homie.getLogger() << cIndent << "handleInput -> property '" << property << "' value=" << value << endl;

    // Node Services
  if(property.equalsIgnoreCase(cSknModeID)) {
    if(value.equalsIgnoreCase("reboot")) {
      stop();
      Homie.getLogger() << cIndent << "RESTARTING OR REBOOTING MACHINE ";
      cCurrentMode =  cMode[REBOOT];
      cCurrentState = cDir[OPEN];
      ESP.restart();
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_up")) {
      Homie.getLogger() << cIndent << "Auto Learn Up ";
      cCurrentMode =  cMode[AUTO_LEARN_UP];
      cCurrentState = cDir[LEARNING];
      autoLearnUpReadings = readings + AUTO_LEARN_READINGS;
      bAutoLearnUp = true;
      if(!bActive) start();      
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_down")) {
      Homie.getLogger() << cIndent << "Auto Learn Down ";
      cCurrentMode =  cMode[AUTO_LEARN_DOWN];
      cCurrentState = cDir[LEARNING];
      autoLearnDownReadings = readings + AUTO_LEARN_READINGS;
      bAutoLearnDown = true;
      if(!bActive) start();      
      rc = true;
    }     
    
    if(rc) {
      broadcastStatus();
    }
  }

  return rc;
}

/**
 *
 */
void SknLoxRanger::broadcastStatus() {
  if(gbEnableDoorOperations) {
    cCurrentState=movementString();
    setProperty(cSknPosID).send(String(uiDistanceValuePos));
    setProperty(cSknStateID).send(cCurrentState);
    setProperty(cSknModeID).send(cCurrentMode);
    setProperty(cSknDetailID).send(cCurrentJSON);
  }
}

/**
 *
 */
void SknLoxRanger::onReadyToOperate() {
  begin();

  Homie.getLogger()
      << "✖  "
      << "Node: " << getName()
      << " Ready to operate: " 
      << cCurrentState
      << "("
      << uiDistanceValuePos
      << ") Services Mode: "
      << cCurrentMode
      << endl;
  broadcastStatus();
}

/**
 *
 */
void SknLoxRanger::setup() {

   advertise(cSknStateID)
    .setName("State")
    .setDatatype("enum")
    .setFormat("UP,DOWN,OPEN,LEARNING")
    .setRetained(true);

  advertise(cSknPosID)
    .setName("Position")
    .setDatatype("percent")
    .setFormat("0:100")
    .setUnit("%")
    .setRetained(true);

  advertise(cSknModeID)
    .setName("Services")
    .setDatatype("string")
    .setFormat("READY,AUTO_LEARN_UP,AUTO_LEARN_DOWN,REBOOT")
    .settable();
    // Commands: auto_learn_up, auto_learn_down, reboot

   advertise(cSknDetailID)
    .setName("Details")
    .setDatatype("string")
    .setFormat("json");

  Homie.getLogger()
      << "✖ "
      << "Node: " << getName()
      << " Homie setup() Complete"
      << endl;
}

/**
 *
 */
void SknLoxRanger::loop() {
  if(gbEnableDoorOperations) {
    vlxLoop();
  }
}
