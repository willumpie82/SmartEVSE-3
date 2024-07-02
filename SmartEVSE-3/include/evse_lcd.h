#pragma once

#include "Arduino.h"
#include "evse_menu_defines.h"
#include "glcd.h"

#define BACKLIGHT 120               // Seconds delay for the LCD backlight to turn off.


void resetLCD(void);
void showhelp(void);
bool menuIdle(void);
void doLCDupdate(void);
void setBacklightTimer(uint8_t newtime);