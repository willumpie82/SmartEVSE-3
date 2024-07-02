#include "evse_task.h"

// Task that handles EVSE State Changes
// Reads buttons, and updates the LCD.
//
// called every 10ms
void EVSEStates(void *parameter)
{

  // uint8_t n;
  uint8_t leftbutton = 5;
  uint8_t DiodeCheck = 0;
  uint16_t StateTimer = 0; // When switching from State B to C, make sure pilot is at 6v for 100ms

  // infinite loop
  while (1)
  {
    // Sample the three < o > buttons.
    // As the buttons are shared with the SPI lines going to the LCD,
    // we have to make sure that this does not interfere by write actions to the LCD.
    // Therefore updating the LCD is also done in this task.

    if (sampleButtons())
    {
      GLCDMenu(getButtonState());
    }

    // Update/Show Helpmenu§
    showhelp();

    // Left button pressed, Loadbalancing is Master or Disabled, switch is set to "Sma-Sol B" and Mode is Smart or Solar?
    if (menuIdle() && getButtonState() == 0x6 && getMode() && !leftbutton && getSwitchMode() == 3)
    {
      setMode(~getSwitchMode() & 0x3); // Change from Solar to Smart mode and vice versa.
      ClearErrors();                   // Clear All errors
      resetCharger();                  // Clear any Chargedelay
      setSolarStopTimer(0);            // Also make sure the SolarTimer is disabled.
      resetLCD();
      leftbutton = 5;
    }
    else if (leftbutton && getButtonState() == 0x7)
      leftbutton--;

    // Check the external switch and RCM sensor
    CheckSwitch();

    // sample the Pilot line
    samplePilot();

    // ############### EVSE State A #################
    handleEvseStateA();

    // ############### EVSE State B #################
    handleEvseStateB();

    // ############### EVSE State C1 #################
    handleEvseStateC1();

    // ############### EVSE State C #################
    handleEvseStateC();

    // update LCD (every 1000ms) when not in the setup menu
    doLCDupdate();

    // Pause the task for 10ms
    vTaskDelay(10 / portTICK_PERIOD_MS);
  } // while(1) loop
}