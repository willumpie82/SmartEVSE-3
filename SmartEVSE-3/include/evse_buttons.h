#pragma once

#include "Arduino.h"
#include "evse_pins.h"
#include "evse_charger.h"
#include "evse_debug.h"
#include "evse_lcd.h"

#define SWITCH 0          // 0= Charge on plugin, 1= (Push)Button on IO2 is used to Start/Stop charging.


bool sampleButtons(void);
uint8_t getButtonState(void);
uint8_t getSwitchMode(void);
void CheckSwitch(void);