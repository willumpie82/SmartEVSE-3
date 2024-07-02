#include "evse_charger.h"

uint8_t MainsMeter = MAINS_METER; // Type of Mains electric meter (0: Disabled / Constants EM_*)
uint8_t Access_bit = 0;           // 0:No Access 1:Access to SmartEVSE
uint16_t OverrideCurrent = 0;     // Temporary assigned current (Amps *10) (modbus)
EnableC2_t EnableC2 = ENABLE_C2;  // Contactor C2

uint8_t Mode = MODE;              // EVSE mode (0:Normal / 1:Smart / 2:Solar)
uint8_t NodeNewMode = 0;

Preferences preferences;

uint8_t ChargeDelay = 0;                // Delays charging at least 60 seconds in case of not enough current available.
uint8_t State = STATE_A;
uint8_t TestState = 0;
uint8_t RCmon = RC_MON;                 // Residual Current Monitor (0:Disable / 1:Enable)

uint8_t pilot;
bool PilotDisconnected = false;
uint8_t PilotDisconnectTime = 0;         // Time the Control Pilot line should be disconnected (Sec)

uint8_t ResetKwh = 2;                    // if set, reset EV kwh meter at state transition B->C



void resetCharger(void)
{
    ChargeDelay = 0;              // Clear any Chargedelay
}

uint8_t getMode(void)
{
    return Mode;
}

uint8_t getAccess_bit(void)
{
    return Access_bit;
}

uint8_t getState(void)
{
    return State;
}

uint8_t getTestState(void)
{
    return TestState;
}

void setChargeDelay(uint8_t newDelay)
{
    ChargeDelay = newDelay;
}

uint8_t getRcMonState(void)
{
    return RCmon;
}


/**
 * Set EVSE mode
 *
 * @param uint8_t Mode
 */
void setMode(uint8_t NewMode)
{
    // If mainsmeter disabled we can only run in Normal Mode
    if (!MainsMeter && NewMode != MODE_NORMAL)
        return;

    // Take care of extra conditionals/checks for custom features
    setAccess(!getDelayedStartTime()); // if DelayedStartTime not zero then we are Delayed Charging
    if (NewMode == MODE_SOLAR)
    {
        // Reset OverrideCurrent if mode is SOLAR
        OverrideCurrent = 0;
    }

    // when switching modes, we just keep charging at the phases we were charging at;
    // it's only the regulation algorithm that is changing...
    // EXCEPT when EnableC2 == Solar Off, because we would expect C2 to be off when in Solar Mode and EnableC2 == Solar Off
    // and also the other way around, multiple phases might be wanted when changing from Solar to Normal or Smart
    bool switchOnLater = false;
    if (EnableC2 == SOLAR_OFF)
    {
        if ((Mode != MODE_SOLAR && NewMode == MODE_SOLAR) || (Mode == MODE_SOLAR && NewMode != MODE_SOLAR))
        {
            // we are switching from non-solar to solar
            // since we EnableC2 == SOLAR_OFF C2 is turned On now, and should be turned off
            setAccess(0); // switch to OFF
            switchOnLater = true;
        }
    }

#if MQTT
    // Update MQTT faster
    lastMqttUpdate = 10;
#endif

    if (NewMode == MODE_SMART)
    {
        ClearErrors(); // Clear All errors
        setSolarStopTimer(0);              // Also make sure the SolarTimer is disabled.
    }
    ChargeDelay = 0;            // Clear any Chargedelay
    setBacklightTimer(BACKLIGHT); // Backlight ON
    if (Mode != NewMode)
        NodeNewMode = NewMode + 1;
    Mode = NewMode;

    if (switchOnLater)
        setAccess(1);

    // make mode and start/stoptimes persistent on reboot
    if (preferences.begin("settings", false))
    { // false = write mode
        preferences.putUChar("Mode", Mode);
        preferences.putULong("DelayedStartTim", getDelayedStartTime()); // epoch2 only needs 4 bytes
        preferences.putULong("DelayedStopTime", getDelayedStopTime());  // epoch2 only needs 4 bytes
        preferences.end();
    }
}

void setAccess(bool Access)
{
    Access_bit = Access;
    if (Access == 0)
    {
        // TODO:setStatePowerUnavailable() ?
        if (State == STATE_C)
            setState(STATE_C1); // Determine where to switch to.
        else if (State != STATE_C1 && (State == STATE_B || State == STATE_MODEM_REQUEST || State == STATE_MODEM_WAIT || State == STATE_MODEM_DONE || State == STATE_MODEM_DENIED))
            setState(STATE_B1);
    }

    // make mode and start/stoptimes persistent on reboot
    if (preferences.begin("settings", false))
    { // false = write mode
        preferences.putUChar("Access", Access_bit);
        preferences.putUChar("CardOffset", getCardOffset());
        preferences.end();
    }

#if MQTT
    // Update MQTT faster
    lastMqttUpdate = 10;
#endif
}

// Determine the state of the Pilot signal
//
uint8_t Pilot()
{

    uint32_t sample, Min = 3300, Max = 0;
    uint32_t voltage;
    uint8_t n;

    // calculate Min/Max of last 25 CP measurements
    Min = getADCsampleMin();
    Max = getADCsampleMax();
    //_LOG_A("min:%u max:%u\n",Min ,Max);

    // test Min/Max against fixed levels
    if (Min >= 3055)
        return PILOT_12V; // Pilot at 12V (min 11.0V)
    if ((Min >= 2735) && (Max < 3055))
        return PILOT_9V; // Pilot at 9V
    if ((Min >= 2400) && (Max < 2735))
        return PILOT_6V; // Pilot at 6V
    if ((Min >= 2000) && (Max < 2400))
        return PILOT_3V; // Pilot at 3V
    if ((Min > 100) && (Max < 300))
        return PILOT_DIODE; // Diode Check OK
    return PILOT_NOK;       // Pilot NOT ok
}

void samplePilot(void)
{
    pilot = Pilot();
}

void handleEvseStateA(void)
{
    if (State == STATE_A || State == STATE_COMM_B || State == STATE_B1)
    {
        // When the pilot line is disconnected, wait for PilotDisconnectTime, then reconnect
        if (PilotDisconnected)
        {
            if (PilotDisconnectTime == 0 && pilot == PILOT_NOK)
            { // Pilot should be ~ 0V when disconnected
                PILOT_CONNECTED;
                PilotDisconnected = false;
                _LOG_A("Pilot Connected\n");
            }
        }
        else if (pilot == PILOT_12V)
        { // Check if we are disconnected, or forced to State A, but still connected to the EV
            // If the RFID reader is set to EnableOne or EnableAll mode, and the Charging cable is disconnected
            // We start a timer to re-lock the EVSE (and unlock the cable) after 60 seconds.
            if ((getRFIDreaderMode() == 2 || getRFIDreaderMode() == 1) && getAccessTimer() == 0 && Access_bit == 1)
                setAccessTimer(RFIDLOCKTIME);

            if (State != STATE_A)
                setState(STATE_A); // reset state, incase we were stuck in STATE_COMM_B
            ChargeDelay = 0;       // Clear ChargeDelay when disconnected.

            if (!ResetKwh)
                ResetKwh = 1; // when set, reset EV kWh meter on state B->C change.
        }
        else if (pilot == PILOT_9V && ErrorFlags == NO_ERROR && ChargeDelay == 0 && Access_bit && State != STATE_COMM_B
#if MODEM
                 && State != STATE_MODEM_REQUEST && State != STATE_MODEM_WAIT && State != STATE_MODEM_DONE // switch to State B ?
#endif
        )
        { // Allow to switch to state C directly if STATE_A_TO_C is set to PILOT_6V (see EVSE.h)
            DiodeCheck = 0;

            ProximityPin(); // Sample Proximity Pin

            _LOG_I("Cable limit: %uA  Max: %uA\n", MaxCapacity, MaxCurrent);
            if (MaxCurrent > MaxCapacity)
                ChargeCurrent = MaxCapacity * 10; // Do not modify Max Cable Capacity or MaxCurrent (fix 2.05)
            else
                ChargeCurrent = MinCurrent * 10; // Instead use new variable ChargeCurrent

            // Load Balancing : Node
            if (LoadBl > 1)
            {                           // Send command to Master, followed by Max Charge Current
                setState(STATE_COMM_B); // Node wants to switch to State B

                // Load Balancing: Master or Disabled
            }
            else if (IsCurrentAvailable())
            {
                BalancedMax[0] = MaxCapacity * 10;
                Balanced[0] = ChargeCurrent; // Set pilot duty cycle to ChargeCurrent (v2.15)
#if MODEM
                if (ModemStage == 0)
                    setState(STATE_MODEM_REQUEST);
                else
#endif
                    setState(STATE_B); // switch to State B
                ActivationMode = 30;   // Activation mode is triggered if state C is not entered in 30 seconds.
                AccessTimer = 0;
            }
            else if (Mode == MODE_SOLAR)
            {                         // Not enough power:
                ErrorFlags |= NO_SUN; // Not enough solar power
            }
            else
                ErrorFlags |= LESS_6A; // Not enough power available
        }
        else if (pilot == PILOT_9V && State != STATE_B1 && State != STATE_COMM_B && Access_bit)
        {
            setState(STATE_B1);
        }
    } // State == STATE_A || State == STATE_COMM_B || State == STATE_B1

    if (State == STATE_COMM_B_OK)
    {
        setState(STATE_B);
        ActivationMode = 30; // Activation mode is triggered if state C is not entered in 30 seconds.
        AccessTimer = 0;
    }
}

void handleEvseStateB(void)
{
    if (State == STATE_B || State == STATE_COMM_C)
    {

        if (pilot == PILOT_12V)
        {                      // Disconnected?
            setState(STATE_A); // switch to STATE_A
        }
        else if (pilot == PILOT_6V && ++StateTimer > 50)
        { // When switching from State B to C, make sure pilot is at 6V for at least 500ms
          // Fixes https://github.com/dingo35/SmartEVSE-3.5/issues/40
            if ((DiodeCheck == 1) && (ErrorFlags == NO_ERROR) && (ChargeDelay == 0))
            {
                if (EVMeter && ResetKwh)
                {
                    EnergyMeterStart = EnergyEV;                 // store kwh measurement at start of charging.
                    EnergyCharged = EnergyEV - EnergyMeterStart; // Calculate Energy
                    ResetKwh = 0;                                // clear flag, will be set when disconnected from EVSE (State A)
                }

                // Load Balancing : Node
                if (LoadBl > 1)
                {
                    if (State != STATE_COMM_C)
                        setState(STATE_COMM_C); // Send command to Master, followed by Charge Current

                    // Load Balancing: Master or Disabled
                }
                else
                {
                    BalancedMax[0] = ChargeCurrent;
                    if (IsCurrentAvailable())
                    {

                        Balanced[0] = 0;        // For correct baseload calculation set current to zero
                        CalcBalancedCurrent(1); // Calculate charge current for all connected EVSE's

                        DiodeCheck = 0;    // (local variable)
                        setState(STATE_C); // switch to STATE_C
                        if (!LCDNav)
                            GLCD(); // Don't update the LCD if we are navigating the menu
                                    // immediately update LCD (20ms)
                    }
                    else if (Mode == MODE_SOLAR)
                    {                         // Not enough power:
                        ErrorFlags |= NO_SUN; // Not enough solar power
                    }
                    else
                        ErrorFlags |= LESS_6A; // Not enough power available
                }
            }

            // PILOT_9V
        }
        else if (pilot == PILOT_9V)
        {

            StateTimer = 0; // Reset State B->C transition timer
            if (ActivationMode == 0)
            {
                setState(STATE_ACTSTART);
                ActivationTimer = 3;

                SetCPDuty(0); // PWM off,  channel 0, duty cycle 0%
                              // Control pilot static -12V
            }
        }
        if (pilot == PILOT_DIODE)
        {
            DiodeCheck = 1; // Diode found, OK
            _LOG_A("Diode OK\n");
            timerAlarmWrite(timerA, PWM_5, false); // Enable Timer alarm, set to start of CP signal (5%)
        }
    }
}

void handleEvseStateC1(void)
{
    if (State == STATE_C1)
    {
        if (pilot == PILOT_12V)
        {                      // Disconnected or connected to EV without PWM
            setState(STATE_A); // switch to STATE_A
            GLCD_init();       // Re-init LCD
        }
        else if (pilot == PILOT_9V)
        {
            setState(STATE_B1); // switch to State B1
            GLCD_init();        // Re-init LCD
        }
    }

    if (State == STATE_ACTSTART && ActivationTimer == 0)
    {
        setState(STATE_B);    // Switch back to State B
        ActivationMode = 255; // Disable ActivationMode
    }

    if (State == STATE_COMM_C_OK)
    {
        DiodeCheck = 0;
        setState(STATE_C); // switch to STATE_C
                           // Don't update the LCD if we are navigating the menu
        if (!LCDNav)
            GLCD(); // immediately update LCD
    }
}

void handleEvseStateC(void)
{
    if (State == STATE_C)
    {

        if (pilot == PILOT_12V)
        {                      // Disconnected ?
            setState(STATE_A); // switch back to STATE_A
            GLCD_init();       // Re-init LCD
        }
        else if (pilot == PILOT_9V)
        {
            setState(STATE_B); // switch back to STATE_B
            DiodeCheck = 0;
            GLCD_init(); // Re-init LCD (200ms delay)
                         // Mark EVSE as inactive (still State B)
        }

    } // end of State C code
}


/**
 * Checks all parameters to determine whether
 * we are going to force single phase charging
 * Returns true if we are going to do single phase charging
 * Returns false if we are going to do (traditional) 3 phase charing
 * This is only relevant on a 3f mains and 3f car installation!
 * 1f car will always charge 1f undetermined by CONTACTOR2
 */
uint8_t Force_Single_Phase_Charging() {                                         // abbreviated to FSPC
    switch (EnableC2) {
        case NOT_PRESENT:                                                       //no use trying to switch a contactor on that is not present
        case ALWAYS_OFF:
            return 1;
        case SOLAR_OFF:
            return (Mode == MODE_SOLAR);
        case AUTO:
        case ALWAYS_ON:
            return 0;   //3f charging
    }
    //in case we don't know, stick to 3f charging
    return 0;
}

void setStatePowerUnavailable(void) {
    if (State == STATE_A)
       return;
    //State changes between A,B,C,D are caused by EV or by the user
    //State changes between x1 and x2 are created by the EVSE
    //State changes between x1 and x2 indicate availability (x2) of unavailability (x1) of power supply to the EV
    if (State == STATE_C) setState(STATE_C1);                       // If we are charging, tell EV to stop charging
    else if (State != STATE_C1) setState(STATE_B1);                 // If we are not in State C1, switch to State B1
}

void setState(uint8_t NewState) 
{
    if (State != NewState) {
        char Str[50];
        snprintf(Str, sizeof(Str), "%02d:%02d:%02d STATE %s -> %s\n",timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, getStateName(State), getStateName(NewState) );

        _LOG_A("%s",Str);
    }

    switch (NewState) {
        case STATE_B1:
            if (!ChargeDelay) ChargeDelay = 3;                                  // When entering State B1, wait at least 3 seconds before switching to another state.
            if (State != STATE_B1 && State != STATE_B && !PilotDisconnected) {
                PILOT_DISCONNECTED;
                PilotDisconnected = true;
                PilotDisconnectTime = 5;                                       // Set PilotDisconnectTime to 5 seconds

                _LOG_A("Pilot Disconnected\n");
            }
            // fall through
        case STATE_A:                                                           // State A1
            CONTACTOR1_OFF;  
            CONTACTOR2_OFF;  
            SetCPDuty(1024);                                                    // PWM off,  channel 0, duty cycle 100%
            timerAlarmWrite(timerA, PWM_100, true);                             // Alarm every 1ms, auto reload 
            if (NewState == STATE_A) {
                ErrorFlags &= ~NO_SUN;
                ErrorFlags &= ~LESS_6A;
                ChargeDelay = 0;
                // Reset Node
                Node[0].Timer = 0;
                Node[0].IntTimer = 0;
                Node[0].Phases = 0;
                Node[0].MinCurrent = 0;                                         // Clear ChargeDelay when disconnected.
            }

#if MODEM
            if (DisconnectTimeCounter == -1){
                DisconnectTimeCounter = 0;                                      // Start counting disconnect time. If longer than 60 seconds, throw DisconnectEvent
            }
            break;
        case STATE_MODEM_REQUEST: // After overriding PWM, and resetting the safe state is 10% PWM. To make sure communication recovers after going to normal, we do this. Ugly and temporary
            ToModemWaitStateTimer = 5;
            DisconnectTimeCounter = -1;                                         // Disable Disconnect timer. Car is connected
            SetCPDuty(1024);
            CONTACTOR1_OFF;
            CONTACTOR2_OFF;
            break;
        case STATE_MODEM_WAIT: 
            SetCPDuty(50);
            ToModemDoneStateTimer = 60;
            break;
        case STATE_MODEM_DONE:  // This state is reached via STATE_MODEM_WAIT after 60s (timeout condition, nothing received) or after REST request (success, shortcut to immediate charging).
            CP_OFF;
            DisconnectTimeCounter = -1;                                         // Disable Disconnect timer. Car is connected
            LeaveModemDoneStateTimer = 5;                                       // Disconnect CP for 5 seconds, restart charging cycle but this time without the modem steps.
#endif
            break;
        case STATE_B:
#if MODEM
            CP_ON;
            DisconnectTimeCounter = -1;                                         // Disable Disconnect timer. Car is connected
#endif
            CONTACTOR1_OFF;
            CONTACTOR2_OFF;
            timerAlarmWrite(timerA, PWM_95, false);                             // Enable Timer alarm, set to diode test (95%)
            SetCurrent(ChargeCurrent);                                          // Enable PWM
            break;      
        case STATE_C:                                                           // State C2
            ActivationMode = 255;                                               // Disable ActivationMode

            if (Switching_To_Single_Phase == GOING_TO_SWITCH) {
                    CONTACTOR2_OFF;
                    setSolarStopTimer(0); //TODO still needed? now we switched contactor2 off, review if we need to stop solar charging
                    //Nr_Of_Phases_Charging = 1; this will be detected automatically
                    Switching_To_Single_Phase = AFTER_SWITCH;                   // we finished the switching process,
                                                                                // BUT we don't know which is the single phase
            }

            CONTACTOR1_ON;
            if (!Force_Single_Phase_Charging() && Switching_To_Single_Phase != AFTER_SWITCH) {                               // in AUTO mode we start with 3phases
                CONTACTOR2_ON;                                                  // Contactor2 ON
            }
            LCDTimer = 0;
            break;
        case STATE_C1:
            SetCPDuty(1024);                                                    // PWM off,  channel 0, duty cycle 100%
            timerAlarmWrite(timerA, PWM_100, true);                             // Alarm every 1ms, auto reload 
                                                                                // EV should detect and stop charging within 3 seconds
            C1Timer = 6;                                                        // Wait maximum 6 seconds, before forcing the contactor off.
            ChargeDelay = 15;
            break;
        default:
            break;
    }
    
    BalancedState[0] = NewState;
    State = NewState;

#if MQTT
    // Update MQTT faster
    lastMqttUpdate = 10;
#endif

    // BacklightTimer = BACKLIGHT;                                                 // Backlight ON
}