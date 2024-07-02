#pragma once

#include "Arduino.h"
#include <Preferences.h>
#include <esp_adc_cal.h>

#include "evse_pins.h"
#include "evse_time.h"
#include "evse_error.h"
#include "solar.h"
#include "rfid.h"
#include "adc.h"
#include "evse_Debug.h"
#include "evse_lcd.h"



enum EnableC2_t { NOT_PRESENT, ALWAYS_OFF, SOLAR_OFF, ALWAYS_ON, AUTO };

// Mode settings
#define MODE_NORMAL 0
#define MODE_SMART 1
#define MODE_SOLAR 2

#define MAINS_METER 0                                                           // Mains Meter, 0=Disabled, 1= Sensorbox, 2=Phoenix, 3= Finder, 4= Eastron, 5=Custom

#define ENABLE_C2 ALWAYS_ON

#define MODE 0                                                                  // Normal EVSE mode

#define STATE_A 0                                                               // A Vehicle not connected
#define STATE_B 1                                                               // B Vehicle connected / not ready to accept energy
#define STATE_C 2                                                               // C Vehicle connected / ready to accept energy / ventilation not required
#define STATE_D 3                                                               // D Vehicle connected / ready to accept energy / ventilation required (not implemented)
#define STATE_COMM_B 4                                                          // E State change request A->B (set by node)
#define STATE_COMM_B_OK 5                                                       // F State change A->B OK (set by master)
#define STATE_COMM_C 6                                                          // G State change request B->C (set by node)
#define STATE_COMM_C_OK 7                                                       // H State change B->C OK (set by master)
#define STATE_ACTSTART 8                                                        // I Activation mode in progress
#define STATE_B1 9                                                              // J Vehicle connected / EVSE not ready to deliver energy: no PWM signal
#define STATE_C1 10                                                             // K Vehicle charging / EVSE not ready to deliver energy: no PWM signal (temp state when stopping charge from EVSE)
#define STATE_MODEM_REQUEST 11                                                          // L Vehicle connected / requesting ISO15118 communication, 0% duty
#define STATE_MODEM_WAIT 12                                                          // M Vehicle connected / requesting ISO15118 communication, 5% duty
#define STATE_MODEM_DONE 13                                                // Modem communication succesful, SoCs extracted. Here, re-plug vehicle
#define STATE_MODEM_DENIED 14                                                // Modem access denied based on EVCCID, re-plug vehicle and try again

#define NOSTATE 255

#define RC_MON 0                                                                // Residual Current Monitoring on IO3. Disabled=0, RCM14=1
#define RCM14 1

#define PILOT_12V 1                                                             // State A - vehicle disconnected
#define PILOT_9V 2                                                              // State B - vehicle connected
#define PILOT_6V 3                                                              // State C - EV charge
#define PILOT_3V 4
#define PILOT_DIODE 5
#define PILOT_NOK 0

#define PILOT_CONNECTED digitalWrite(PIN_CPOFF, LOW);
#define PILOT_DISCONNECTED digitalWrite(PIN_CPOFF, HIGH);


uint8_t getMode(void);
uint8_t getAccess_bit(void);
uint8_t getState(void);
uint8_t getTestState(void);

void setMode(uint8_t NewMode);
void setAccess(bool Access);
void setState(uint8_t NewState);
void setChargeDelay(uint8_t newDelay);
uint8_t getRcMonState(void);
void samplePilot(void);

uint8_t Pilot();
void handleEvseStateA(void);
void handleEvseStateB(void);
void handleEvseStateC1(void);
void handleEvseStateC(void);
void resetCharger(void);

