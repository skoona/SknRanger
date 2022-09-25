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

  limitsRestore();
}

/**
 * @brief Processing Loop
 * 
 */
SknLoxRanger& SknLoxRanger::vlxLoop() {
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

  while (!lox.init()) {
    Serial.printf(" ✖  Failed to detect and initialize sensor!\n");
    delay(1000);
  }
  Serial.printf(" ✖  Exited initialize sensor!\n");

  lox.setTimeout(uiInterMeasurement+(uiTimingBudget/1000));

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
  
  if(bAutoLearnUp) {
    cCurrentMode=cDir[LEARNING_UP];
    setProperty(cSknModeID).send(cCurrentMode);
  } else if(bAutoLearnDown) {
    cCurrentMode=cDir[LEARNING_DOWN];
    setProperty(cSknModeID).send(cCurrentMode);
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

  if (lox.ranging_data.range_status == VL53L1X::RangeValid)
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
    avg = 0;
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
  
  /*
   * get valid value */
  mmPos = (long)readValue(wait);
  while(mmPos==0) {        // get a vaild value before proceeding
    Serial.printf(" ✖  SknLoxRanger relativeDistance(0 mm) NOT accepted.\n");
    mmPos = (long)readValue(wait);
  }

  /*
   * determine the new ranges and save to eeprom */
  readings++;
  if(bAutoLearnUp) {
    iLimitMin = mmPos;
    if(readings>=autoLearnUpReadings) {
      bAutoLearnUp=false;
      limitsSave();
      cCurrentMode=cMode[0];
      Serial.printf(" ✖  SknLoxRanger Auto Learn Up(%d mm) accepted.\n", iLimitMin);
    }
  } else if (bAutoLearnDown) {
    iLimitMax = mmPos;
    if(readings>=autoLearnDownReadings) {
      bAutoLearnDown=false;
      limitsSave();
      cCurrentMode=cMode[0];
      setProperty(cSknModeID).send(cCurrentMode);
      Serial.printf(" ✖  SknLoxRanger Auto Learn Down(%d mm) accepted.\n", iLimitMax);
    }
  }

  /*
   * use fixed ranges while determining new range */
  if(bAutoLearnUp || bAutoLearnDown) {
    posValue = map(mmPos, MM_MIN, MM_MAX, 0, 100);
  } else {
    posValue = map(mmPos, iLimitMin, iLimitMax, 0, 100);
  }

  mmPos = constrain( posValue, 0, 100);
  if(mmPos!=uiDistanceValuePos) {
    setProperty(cSknPosID).send(String(uiDistanceValuePos));
  }
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

/**
 * Handles the received MQTT messages from Homie.
 *
 */
bool SknLoxRanger::handleInput(const HomieRange& range, const String& property, const String& value) {
  bool rc = false;
  Homie.getLogger() << cIndent << "〽 handleInput -> property '" << property << "' value=" << value << endl;

    // Node Services
  if(property.equalsIgnoreCase(cSknModeID)) {
    if(value.equalsIgnoreCase("reboot")) {
      stop();
      Homie.getLogger() << cIndent << "〽 RESTARTING OR REBOOTING MACHINE ";
      cCurrentMode =  "Rebooting in 5 seconds";
      ESP.restart();
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_up")) {
      Homie.getLogger() << cIndent << "〽 Auto Learn Up ";
      cCurrentMode =  "Auto Learn Up";
      bAutoLearnUp = true;
      autoLearnUpReadings = readings + AUTO_LEARN_READINGS;
      // door.cmd_auto_learn_up();
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_down")) {
      Homie.getLogger() << cIndent << "〽 Auto Learn Down ";
      cCurrentMode =  "Auto Learn Down";
      bAutoLearnDown = true;
      autoLearnDownReadings = readings + AUTO_LEARN_READINGS;
      // door.cmd_auto_learn_down();
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
    setProperty(cSknPosID).send(String(uiDistanceValuePos));
    setProperty(cSknState).send(String(cCurrentState));
    setProperty(cSknModeID).send(cCurrentMode);
  }
}

/**
 *
 */
void SknLoxRanger::onReadyToOperate() {
  begin().start();

  Homie.getLogger()
      << "〽 "
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

   advertise(cSknState)
    .setName("State")
    .setDatatype("enum")
    .setFormat("MOVING_UP,MOVING_DOWN,STOPPED,LEARN_UP,LEARN_DOWN,REBOOTING")
    .setRetained(true);

  advertise(cSknPosID)
    .setName("Position")
    .setDatatype("percent")
    .setFormat("0:100")
    .setUnit("%")
    .setRetained(true);
    // Commands: digits:0-100,

  snprintf(cBuffer, sizeof(cBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iLimitMin, iLimitMax);
  cCurrentMode = cBuffer;
  advertise(cSknModeID)
    .setName("Services")
    .setDatatype("string")
    .setFormat("READY,AUTO_LEARN_UP,AUTO_LEARN_DOWN,REBOOT")
    .settable();
    // Commands: auto_learn_up, auto_learn_down, reboot

  Homie.getLogger()
      << "〽 "
      << "Node: " << getName()
      << " SknGarageDoor::setup() "
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
