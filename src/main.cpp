#include <Arduino.h>
#include "main.h"

// put function declarations here:
int myFunction(int, int);

SensorState currentState;



void setup() {

Serial.begin(9600);
  pinInit();
  lcdInit();
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.println(result);
}

static void lcdRefresh(void);

void loop() {

    lcdListen();
    lcdRefresh();

  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

//#############################################################################################
//################################____LCD_REFRESH_CONTROL___###################################
//#############################################################################################

static void lcdRefresh(void) {
  uint16_t tempDecimal;

  if (millis() > pageRefreshTimer) {
    /*LCD pressure output, as a measure to beautify the graphs locking the live pressure read for the LCD alone*/
    #ifdef BEAUTIFY_GRAPH
      lcdSetPressure(currentState.smoothedPressure * 10.f);
    #else
      lcdSetPressure(
        currentState.pressure > 0.f
          ? currentState.pressure * 10.f
          : 0.f
      );
    #endif

    /*LCD temp output*/
//    float brewTempSetPoint = ACTIVE_PROFILE(runningCfg).setpoint + runningCfg.offsetTemp;
    // float liveTempWithOffset = currentState.temperature - runningCfg.offsetTemp;
//    currentState.waterTemperature = (currentState.temperature > (float)ACTIVE_PROFILE(runningCfg).setpoint && currentState.brewSwitchState)
//      ? currentState.temperature / (float)brewTempSetPoint + (float)ACTIVE_PROFILE(runningCfg).setpoint
//      : currentState.temperature;

    lcdSetTemperature(std::floor((uint16_t)currentState.waterTemperature));

    /*LCD weight & temp & water lvl output*/
    switch (lcdCurrentPageId) {
      case NextionPage::Home:
        // temp decimal handling
        tempDecimal = (currentState.waterTemperature - (uint16_t)currentState.waterTemperature) * 10;
        lcdSetTemperatureDecimal(tempDecimal);
        // water lvl
        lcdSetTankWaterLvl(currentState.waterLvl);
        //weight
 //       if (homeScreenScalesEnabled) lcdSetWeight(currentState.weight);
        break;
      case NextionPage::BrewGraph:
      case NextionPage::BrewManual:
        // temp decimal handling
        tempDecimal = (currentState.waterTemperature - (uint16_t)currentState.waterTemperature) * 10;
        lcdSetTemperatureDecimal(tempDecimal);
        // If the weight output is a negative value lower than -0.8 you might want to tare again before extraction starts.
        if (currentState.shotWeight) lcdSetWeight(currentState.shotWeight > -0.8f ? currentState.shotWeight : -0.9f);
        /*LCD flow output*/
        lcdSetFlow( currentState.smoothedPumpFlow * 10.f);
        break;
      default:
        break; // don't push needless data on other pages
    }

  #ifdef DEBUG_ENABLED
    lcdShowDebug(readTempSensor(), getAdsError());
  #endif

    /*LCD timer and warmup*/
//    if (brewActive) {
//      lcdSetBrewTimer((millis() > brewingTimer) ? (int)((millis() - brewingTimer) / 1000) : 0);
//      lcdBrewTimerStart(); // nextion timer start
//      lcdWarmupStateStop(); // Flagging warmup notification on Nextion needs to stop (if enabled)
//    } else {
//      lcdBrewTimerStop(); // nextion timer stop
//    }

//    pageRefreshTimer = millis() + REFRESH_SCREEN_EVERY;
  }
}

// Save the desired temp values to EEPROM
void lcdSaveSettingsTrigger(void) {
  //LOG_VERBOSE("Saving values to EEPROM");

  //eepromValues_t eepromCurrentValues = eepromGetCurrentValues();
  //lcdFetchPage(eepromCurrentValues, lcdCurrentPageId, runningCfg.activeProfile);
  //tryEepromWrite(eepromCurrentValues);
}

void lcdSaveProfileTrigger(void) {
  //LOG_VERBOSE("Saving profile to EEPROM");

  //eepromValues_t eepromCurrentValues = eepromGetCurrentValues();
  //lcdFetchCurrentProfile(eepromCurrentValues);
  //tryEepromWrite(eepromCurrentValues);
}

void lcdResetSettingsTrigger(void) {
  //tryEepromWrite(eepromGetDefaultValues());
}

void lcdLoadDefaultProfileTrigger(void) {
  //lcdSwitchActiveToStoredProfile(eepromGetDefaultValues());

  lcdShowPopup("Profile loaded!");
}

void lcdScalesTareTrigger(void) {
  //LOG_VERBOSE("Tare scales");
  if (currentState.scalesPresent) currentState.tarePending = true;
}

void lcdHomeScreenScalesTrigger(void) {
  //LOG_VERBOSE("Scales enabled or disabled");
  //homeScreenScalesEnabled = lcdGetHomeScreenScalesEnabled();
}

void lcdBrewGraphScalesTareTrigger(void) {
  //LOG_VERBOSE("Predictive scales tare action completed!");
  if (currentState.scalesPresent) {
    currentState.tarePending = true;
  }
  else {
    currentState.shotWeight = 0.f;
    //predictiveWeight.setIsForceStarted(true);
  }
}


void lcdRefreshElementsTrigger(void) {

  //eepromValues_t eepromCurrentValues = eepromGetCurrentValues();

  switch (lcdCurrentPageId) {
    case NextionPage::BrewPreinfusion:
      //ACTIVE_PROFILE(eepromCurrentValues).preinfusionFlowState = lcdGetPreinfusionFlowState();
      break;
    case NextionPage::BrewProfiling:
      //ACTIVE_PROFILE(eepromCurrentValues).mfProfileState = lcdGetProfileFlowState();
      break;
    case NextionPage::BrewTransitionProfile:
      //ACTIVE_PROFILE(eepromCurrentValues).tpType = lcdGetTransitionFlowState();
      break;
    default:
      lcdShowPopup("Nope!");
      break;
  }

  // Make the necessary changes
  //uploadPageCfg(eepromCurrentValues, systemState);
  // refresh the screen elements
  //pageValuesRefresh();
}

void lcdQuickProfileSwitch(void) {
  //lcdSwitchActiveToStoredProfile(eepromGetCurrentValues());
  lcdShowPopup("Profile switched!");
}