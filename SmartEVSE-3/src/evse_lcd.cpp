#include "evse_lcd.h"

uint8_t LCDNav = 0;
uint32_t ScrollTimer = 0;
uint8_t SubMenu = 0;
uint8_t LCDTimer = 0;
uint8_t LCDupdate = 0;              // flag to update the LCD every 1000ms
uint16_t BacklightTimer = 0;        // Backlight timer (sec)
uint8_t BacklightSet = 0;


void showhelp(void)
{
  if (LCDNav > MENU_ENTER && LCDNav < MENU_EXIT && (ScrollTimer + 5000 < millis()) && (!SubMenu))
    GLCDHelp();
}

void resetLCD(void)
{
  LCDTimer = 0;
}

bool menuIdle(void)
{
  return !LCDNav || LCDNav == MENU_OFF;
}

void doLCDupdate(void)
{
  if (LCDupdate)
  {
    // This is also the ideal place for debug messages that should not be printed every 10ms
    //_LOG_A("EVSEStates task free ram: %u\n", uxTaskGetStackHighWaterMark( NULL ));
    GLCD();
    LCDupdate = 0;
  }
}

void setBacklightTimer(uint8_t newtime)
{
  BacklightTimer = newtime;
}
