
#include <Arduino.h>


#include "peripherals/peripherals.h"
#include "lcd/lcd.h"
#include "sensors_state.h"


//Timers
unsigned long systemHealthTimer;
unsigned long pageRefreshTimer;
unsigned long pressureTimer;
unsigned long brewingTimer;
unsigned long thermoTimer;
unsigned long scalesTimer;
unsigned long flowTimer;
unsigned long steamTime;