#include "evse_buttons.h"

uint8_t ButtonState = 0x0f;              // Holds latest push Buttons state (LSB 3:0)
uint8_t OldButtonState = 0x0f;           // Holds previous push Buttons state (LSB 3:0)

uint8_t Switch = SWITCH;                 // External Switch (0:Disable / 1:Access B / 2:Access S / 3:Smart-Solar B / 4:Smart-Solar S)

uint8_t getSwitchMode(void)
{
    return Switch;
}

bool sampleButtons(void)
{
  pinMatrixOutDetach(PIN_LCD_SDO_B3, false, false);       // disconnect MOSI pin
  pinMode(PIN_LCD_SDO_B3, INPUT);
  pinMode(PIN_LCD_A0_B2, INPUT);
  // sample buttons                                       < o >
  if (digitalRead(PIN_LCD_SDO_B3)) ButtonState = 4;       // > (right)
  else ButtonState = 0;
  if (digitalRead(PIN_LCD_A0_B2)) ButtonState |= 2;       // o (middle)
  if (digitalRead(PIN_IO0_B1)) ButtonState |= 1;          // < (left)

  pinMode(PIN_LCD_SDO_B3, OUTPUT);
  pinMatrixOutAttach(PIN_LCD_SDO_B3, VSPID_IN_IDX, false, false); // re-attach MOSI pin
  pinMode(PIN_LCD_A0_B2, OUTPUT);

  return ((getButtonState() != 0x07) || (getButtonState() != OldButtonState));
}


uint8_t getButtonState(void)
{
  return ButtonState;
}


// CheckSwitch (SW input)
//
void CheckSwitch(void)
{
    static uint8_t RB2count = 0, RB2last = 1, RB2low = 0;
    static unsigned long RB2Timer = 0;                                                 // 1500ms

    // External switch changed state?
    if ( (digitalRead(PIN_SW_IN) != RB2last) || RB2low) {
        // make sure that noise on the input does not switch
        if (RB2count++ > 20 || RB2low) {
            RB2last = digitalRead(PIN_SW_IN);
            if (RB2last == 0) {
                // Switch input pulled low
                switch (Switch) {
                    case 1: // Access Button
                        setAccess(!getAccess_bit());                             // Toggle Access bit on/off
                        _LOG_I("Access: %d\n", getAccess_bit());
                        break;
                    case 2: // Access Switch
                        setAccess(true);
                        break;
                    case 3: // Smart-Solar Button or hold button for 1,5 second to STOP charging
                        if (RB2low == 0) {
                            RB2low = 1;
                            RB2Timer = millis();
                        }
                        if (RB2low && millis() > RB2Timer + 1500) {
                            if (getState() == STATE_C) {
                                setState(STATE_C1);
                                if (!getTestState()) setChargeDelay(15);           // Keep in State B for 15 seconds, so the Charge cable can be removed.
                            RB2low = 2;
                            }
                        }
                        break;
                    case 4: // Smart-Solar Switch
                        if (getMode() == MODE_SOLAR) {
                            setMode(MODE_SMART);
                            setSolarStopTimer(0);                           // Also make sure the SolarTimer is disabled.
                        }
                        break;
                    default:
                        if (getMode() == STATE_C) {                             // Menu option Access is set to Disabled
                            setState(STATE_C1);
                            if (!getTestState) setChargeDelay(15);               // Keep in State B for 15 seconds, so the Charge cable can be removed.
                        }
                        break;
                }

                // Reset RCM error when button is pressed
                // RCM was tripped, but RCM level is back to normal
                if (getRcMonState() == 1 && (getErrorFlags() & RCM_TRIPPED) && digitalRead(PIN_RCM_FAULT) == LOW) {
                    // Clear RCM error
                    clearError(RCM_TRIPPED);
                }
                // Also light up the LCD backlight
                // BacklightTimer = BACKLIGHT;                                 // Backlight ON

            } else {
                // Switch input released
                switch (Switch) {
                    case 2: // Access Switch
                        setAccess(false);
                        break;
                    case 3: // Smart-Solar Button
                        if (RB2low != 2) {
                            if (getMode() == MODE_SMART) {
                                setMode(MODE_SOLAR);
                            } else if (getMode() == MODE_SOLAR) {
                                setMode(MODE_SMART);
                            }
                            ClearErrors();                   // Clear All errors
                            setChargeDelay(0);                                // Clear any Chargedelay
                            setSolarStopTimer(0);                           // Also make sure the SolarTimer is disabled.
                            resetLCD();
                        }
                        RB2low = 0;
                        break;
                    case 4: // Smart-Solar Switch
                        if (getMode() == MODE_SMART) setMode(MODE_SOLAR);
                        break;
                    default:
                        break;
                }
            }

            RB2count = 0;
        }
    } else RB2count = 0;

    // Residual current monitor active, and DC current > 6mA ?
    if (getRcMonState() == RCM14 && digitalRead(PIN_RCM_FAULT) == HIGH) {                   
        delay(1);
        // check again, to prevent voltage spikes from tripping the RCM detection
        if (digitalRead(PIN_RCM_FAULT) == HIGH) {                           
            if (getState()) setState(STATE_B1);
            setError(RCM_TRIPPED);
            resetLCD();                 // display the correct error message on the LCD
        }
    }


}
