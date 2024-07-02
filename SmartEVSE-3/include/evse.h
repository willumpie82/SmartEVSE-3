/*
;    Project: Smart EVSE v3
;
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
 */

#ifndef __EVSE_MAIN

#define __EVSE_MAIN



#ifndef FAKE_RFID
//set FAKE_RFID to 1 to emulate an rfid reader with rfid of card = 123456
//showing the rfid card is simulated by executing http://smartevse-xxx.lan/debug?showrfid=1
//don't forget to first store the card before it can activate charging
#define FAKE_RFID 0
#endif

#ifndef AUTOMATED_TESTING
//set AUTOMATED_TESTING to 1 to make hardware-related paramaters like MaxCurrent and MaxCircuit updatable via REST API
//e.g. by executing curl -X POST http://smartevse-xxx.lan/automated_testing?maxcurrent=100
#define AUTOMATED_TESTING 0
#endif

#ifndef FAKE_SUNNY_DAY
//set this to 1 to emulate a sunny day where your solar charger is injecting current in the grid:
#define FAKE_SUNNY_DAY 0
//disclaimer: might not work for CT1 calibration/uncalibration stuff, since I can't test that
//the number of Amperes you want to have fake injected into Lx
#endif

#if FAKE_SUNNY_DAY
#define INJECT_CURRENT_L1 10
#define INJECT_CURRENT_L2 0
#define INJECT_CURRENT_L3 0
#endif

#ifndef MQTT
#define MQTT 1  // Uncomment or set to 0 to disable MQTT support in code
#endif

#ifndef ENABLE_OCPP
#define ENABLE_OCPP 0
#endif

#ifndef MODEM
//the wifi-debugger is available by telnetting to your SmartEVSE device
#define MODEM 0  //0 = no modem 1 = modem
#endif

#ifndef VERSION
//please note that this version will only be displayed with the correct time/date if the program is recompiled
//so the webserver will show correct version if evse.cpp is recompiled
//the lcd display will show correct version if glcd.cpp is recompiled
#define VERSION (__TIME__ " @" __DATE__)
#endif



#define EVSE_LOG_FORMAT(letter, format) "[%6u][" #letter "][%s:%u] %s(): " format , (uint32_t) (esp_timer_get_time() / 1000ULL), pathToFileName(__FILE__), __LINE__, __FUNCTION__

#if DBG == 2
#define DEBUG_DISABLED 1
#if LOG_LEVEL >= 1  // Errors
#define _LOG_A(fmt, ... ) Serial.printf(EVSE_LOG_FORMAT(E, fmt), ##__VA_ARGS__)
#define _LOG_A_NO_FUNC( ... ) Serial.printf ( __VA_ARGS__ )
#else
#define _LOG_A( ... )
#define _LOG_A_NO_FUNC( ... )
#endif
#if LOG_LEVEL >= 2  // Warnings
#define _LOG_W(fmt, ... ) Serial.printf(EVSE_LOG_FORMAT(W, fmt), ##__VA_ARGS__)
#define _LOG_W_NO_FUNC( ... ) Serial.printf ( __VA_ARGS__ )
#else
#define _LOG_W( ... ) 
#define _LOG_W_NO_FUNC( ... )
#endif
#if LOG_LEVEL >= 3  // Info
#define _LOG_I(fmt, ... ) Serial.printf(EVSE_LOG_FORMAT(I, fmt), ##__VA_ARGS__)
#define _LOG_I_NO_FUNC( ... ) Serial.printf ( __VA_ARGS__ )
#else
#define _LOG_I( ... )
#define _LOG_I_NO_FUNC( ... )
#endif
#if LOG_LEVEL >= 4  // Debug
#define _LOG_D(fmt, ... ) Serial.printf(EVSE_LOG_FORMAT(D, fmt), ##__VA_ARGS__)
#define _LOG_D_NO_FUNC( ... ) Serial.printf ( __VA_ARGS__ )
#else
#define _LOG_D( ... ) 
#define _LOG_D_NO_FUNC( ... )
#endif
#if LOG_LEVEL >= 5  // Verbose
#define _LOG_V(fmt, ... ) Serial.printf(EVSE_LOG_FORMAT(V, fmt), ##__VA_ARGS__)
#define _LOG_V_NO_FUNC( ... ) Serial.printf ( __VA_ARGS__ )
#else
#define _LOG_V( ... ) 
#define _LOG_V_NO_FUNC( ... )
#endif
#endif  // if DBG == 2



#define CP_CHANNEL 0
#define RED_CHANNEL 2                                                           // PWM channel 2 (0 and 1 are used by CP signal)
#define GREEN_CHANNEL 3
#define BLUE_CHANNEL 4
#define LCD_CHANNEL 5                                                           // LED Backlight LCD

#define PWM_5 50                                                                // 5% of PWM
#define PWM_95 950                                                              // 95% of PWM
#define PWM_100 1000                                                            // 100% of PWM

#define ICAL 1024                                                               // Irms Calibration value (for Current transformers)
#define MAX_MAINS 25                                                            // max Current the Mains connection can supply
#define MAX_SUMMAINS 600                                                        // only used for capacity rate limiting, max current over the sum of all phases
#define MAX_CURRENT 13                                                          // max charging Current for the EV
#ifndef MIN_CURRENT
#define MIN_CURRENT 6                                                           // minimum Current the EV will accept
#endif
#define LOCK 0                                                                  // No Cable lock
#define MAX_CIRCUIT 16                                                          // Max current of the EVSE circuit breaker
#define CONFIG 0                                                                // Configuration: 0= TYPE 2 socket, 1= Fixed Cable
#define LOADBL 0                                                                // Load Balancing disabled
#define CHARGEDELAY 60                                                          // Seconds to wait after overcurrent, before trying again
#define RFIDLOCKTIME 60                                                         // Seconds delay for the EVSE to lock again (RFIDreader = EnableOne)
#define START_CURRENT 4                                                         // Start charging when surplus current on sum of all phases exceeds 4A (Solar)
#define STOP_TIME 10                                                            // Stop charging after 10 minutes at MIN charge current (Solar)
#define IMPORT_CURRENT 0                                                        // Allow the use of grid power when solar charging (Amps)
#define MAINS_METER 0                                                           // Mains Meter, 0=Disabled, 1= Sensorbox, 2=Phoenix, 3= Finder, 4= Eastron, 5=Custom
#define GRID 0                                                                  // Grid, 0= 4-Wire CW, 1= 4-Wire CCW, 2= 3-Wire CW, 3= 3-Wire CCW
#define MAINS_METER_ADDRESS 10
#define PV_METER 0
#define PV_METER_ADDRESS 11
#define EV_METER 0
#define EV_METER_ADDRESS 12
#define MIN_METER_ADDRESS 10
#define MAX_METER_ADDRESS 247
#define EMCUSTOM_ENDIANESS 0
#define EMCUSTOM_DATATYPE 0
#define EMCUSTOM_FUNCTION 4
#define EMCUSTOM_UREGISTER 0
#define EMCUSTOM_UDIVISOR 8
#define EMCUSTOM_IREGISTER 0
#define EMCUSTOM_IDIVISOR 8
#define EMCUSTOM_PREGISTER 0
#define EMCUSTOM_PDIVISOR 8
#define EMCUSTOM_EREGISTER 0
#define EMCUSTOM_EDIVISOR 8
#define RFID_READER 0
#define ACCESS_BIT 1
#define WIFI_MODE 0
#define AP_PASSWORD "00000000"
#define INITIALIZED 0
#define MAX_TEMPERATURE 65
#define DELAYEDSTARTTIME 0                                                             // The default StartTime for delayed charged, 0 = not delaying
#define DELAYEDSTOPTIME 0                                                       // The default StopTime for delayed charged, 0 = not stopping
#define SOLARSTARTTIME 40                                                       // Seconds to keep chargecurrent at 6A
#define OCPP_MODE 0
#define AUTOUPDATE 0                                                            // default for Automatic Firmware Update: 0 = disabled, 1 = enabled



#define MODBUS_BAUDRATE 9600
#define MODBUS_TIMEOUT 4
#define ACK_TIMEOUT 1000                                                        // 1000ms timeout
#define NR_EVSES 8
#define BROADCAST_ADR 0x09
#define COMM_TIMEOUT 11                                                         // Timeout for MainsMeter
#define COMM_EVTIMEOUT 8*NR_EVSES                                               // Timeout for EV Energy Meters


#define PILOT_12V 1                                                             // State A - vehicle disconnected
#define PILOT_9V 2                                                              // State B - vehicle connected
#define PILOT_6V 3                                                              // State C - EV charge
#define PILOT_3V 4
#define PILOT_DIODE 5
#define PILOT_NOK 0




#define LED_OFF 255
#define LED_ON 0
#define STATE_A_LED_BRIGHTNESS 60
#define STATE_B_LED_BRIGHTNESS LED_ON
#define ERROR_LED_BRIGHTNESS LED_ON
#define WAITING_LED_BRIGHTNESS LED_ON
#define LCD_BRIGHTNESS 255


#define CP_ON digitalWrite(PIN_CPOFF, LOW);
#define CP_OFF digitalWrite(PIN_CPOFF, HIGH);

#define PILOT_CONNECTED digitalWrite(PIN_CPOFF, LOW);
#define PILOT_DISCONNECTED digitalWrite(PIN_CPOFF, HIGH);

#define CONTACTOR1_ON _LOG_A("Switching Contactor1 ON.\n"); digitalWrite(PIN_SSR, HIGH);
#define CONTACTOR1_OFF _LOG_A("Switching Contactor1 OFF.\n"); digitalWrite(PIN_SSR, LOW);

#define CONTACTOR2_ON _LOG_A("Switching Contactor2 ON.\n"); digitalWrite(PIN_SSR2, HIGH);
#define CONTACTOR2_OFF _LOG_A("Switching Contactor2 OFF.\n"); digitalWrite(PIN_SSR2, LOW);

#define BACKLIGHT_ON digitalWrite(PIN_LCD_LED, HIGH);
#define BACKLIGHT_OFF digitalWrite(PIN_LCD_LED, LOW);

#define ACTUATOR_LOCK { _LOG_A("Locking Actuator.\n"); digitalWrite(PIN_ACTB, HIGH); digitalWrite(PIN_ACTA, LOW); }
#define ACTUATOR_UNLOCK { _LOG_A("Unlocking Actuator.\n"); digitalWrite(PIN_ACTB, LOW); digitalWrite(PIN_ACTA, HIGH); }
#define ACTUATOR_OFF { digitalWrite(PIN_ACTB, HIGH); digitalWrite(PIN_ACTA, HIGH); }

#define ONEWIRE_LOW { digitalWrite(PIN_SW_IN, LOW); pinMode(PIN_SW_IN, OUTPUT); }   // SW set to 0, set to output (driven low)
#define ONEWIRE_HIGH { digitalWrite(PIN_SW_IN, HIGH); pinMode(PIN_SW_IN, OUTPUT); } // SW set to 1, set to output (driven high)
#define ONEWIRE_FLOATHIGH pinMode(PIN_SW_IN, INPUT_PULLUP );                        // SW input (floating high)

#define RCMFAULT digitalRead(PIN_RCM_FAULT)
#define FREE(x) free(x); x = NULL;
#define FW_DOWNLOAD_PATH "http://smartevse-3.s3.eu-west-2.amazonaws.com"

#define MODBUS_INVALID 0
#define MODBUS_OK 1
#define MODBUS_REQUEST 2
#define MODBUS_RESPONSE 3
#define MODBUS_EXCEPTION 4

#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS 0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE 0x03

#define MODBUS_EVSE_STATUS_START 0x0000
#define MODBUS_EVSE_STATUS_COUNT 12
#define MODBUS_EVSE_CONFIG_START 0x0100
#define MODBUS_EVSE_CONFIG_COUNT 10
#define MODBUS_SYS_CONFIG_START  0x0200
#define MODBUS_SYS_CONFIG_COUNT  26

#define MODBUS_MAX_REGISTER_READ MODBUS_SYS_CONFIG_COUNT
#define MODBUS_BUFFER_SIZE MODBUS_MAX_REGISTER_READ * 2 + 10

// EVSE status
#define STATUS_STATE 64                                                         // 0x0000: State
#define STATUS_ERROR 65                                                         // 0x0001: Error
#define STATUS_CURRENT 66                                                       // 0x0002: Charging current (A * 10)
#define STATUS_MODE 67                                                          // 0x0003: EVSE Mode
#define STATUS_SOLAR_TIMER 68                                                   // 0x0004: Solar Timer
#define STATUS_ACCESS 69                                                        // 0x0005: Access bit
#define STATUS_CONFIG_CHANGED 70                                                // 0x0006: Configuration changed
#define STATUS_MAX 71                                                           // 0x0007: Maximum charging current (RO)
#define STATUS_PHASE_COUNT 72                                                   // 0x0008: Number of used phases (RO) (ToDo)
#define STATUS_REAL_CURRENT 73                                                  // 0x0009: Real charging current (RO) (ToDo)
#define STATUS_TEMP 74                                                          // 0x000A: Temperature (RO)
#define STATUS_SERIAL 75                                                        // 0x000B: Serial number (RO)



#define _RSTB_0 digitalWrite(PIN_LCD_RST, LOW);
#define _RSTB_1 digitalWrite(PIN_LCD_RST, HIGH);
#define _A0_0 digitalWrite(PIN_LCD_A0_B2, LOW);
#define _A0_1 digitalWrite(PIN_LCD_A0_B2, HIGH);

#define EM_SENSORBOX 1                                                          // Mains meter types
#define EM_PHOENIX_CONTACT 2
#define EM_FINDER_7E 3
#define EM_EASTRON3P 4
#define EM_EASTRON3P_INV 5
#define EM_ABB 6
#define EM_SOLAREDGE 7
#define EM_WAGO 8
#define EM_API 9
#define EM_EASTRON1P 10
#define EM_FINDER_7M 11
#define EM_UNUSED_SLOT1 12
#define EM_UNUSED_SLOT2 13
#define EM_UNUSED_SLOT3 14
#define EM_UNUSED_SLOT4 15
#define EM_CUSTOM 16

#define ENDIANESS_LBF_LWF 0
#define ENDIANESS_LBF_HWF 1
#define ENDIANESS_HBF_LWF 2
#define ENDIANESS_HBF_HWF 3

#define OWNER_FACT "SmartEVSE"
#define REPO_FACT "SmartEVSE-3"
#define OWNER_COMM "dingo35"
#define REPO_COMM "SmartEVSE-3.5"

typedef enum mb_datatype {
    MB_DATATYPE_INT32 = 0,
    MB_DATATYPE_FLOAT32 = 1,
    MB_DATATYPE_INT16 = 2,
    MB_DATATYPE_MAX,
} MBDataType;


extern portMUX_TYPE rtc_spinlock;   //TODO: Will be placed in the appropriate position after the rtc module is finished.

// #define RTC_ENTER_CRITICAL()    portENTER_CRITICAL(&rtc_spinlock)
// #define RTC_EXIT_CRITICAL()     portEXIT_CRITICAL(&rtc_spinlock)


extern String APpassword;
extern struct tm timeinfo;


extern uint16_t ICal;                                                           // CT calibration value
extern uint8_t Mode;                                                            // EVSE mode
extern uint8_t LoadBl;                                                          // Load Balance Setting (Disable, Master or Node)
extern uint8_t Grid;
extern uint8_t MainsMeterAddress;
extern uint8_t EVMeter;                                                         // Type of EV electric meter (0: Disabled / Constants EM_*)
extern uint8_t EVMeterAddress;
#if FAKE_RFID
extern uint8_t Show_RFID;
#endif

extern int16_t Irms[3];                                                         // Momentary current per Phase (Amps *10) (23 = 2.3A)
extern int16_t Irms_EV[3];                                                         // Momentary current per Phase (Amps *10) (23 = 2.3A)

extern uint8_t State;
extern uint8_t ErrorFlags;
extern uint8_t NextState;

extern int16_t Isum;
extern uint16_t Balanced[NR_EVSES];                                             // Amps value per EVSE

extern uint8_t LCDTimer;
extern uint16_t BacklightTimer;                                                 // remaining seconds the LCD backlight is active
extern uint8_t ButtonState;                                                     // Holds latest push Buttons state (LSB 2:0)
extern uint8_t OldButtonState;                                                  // Holds previous push Buttons state (LSB 2:0)
extern uint8_t LCDNav;
extern uint8_t LCDupdate;
extern uint8_t SubMenu;
extern uint32_t ScrollTimer;
extern uint8_t ChargeDelay;                                                     // Delays charging in seconds.
extern uint8_t TestState;
extern uint8_t Access_bit;
extern uint16_t CardOffset;

extern uint8_t GridActive;                                                      // When the CT's are used on Sensorbox2, it enables the GRID menu option.
extern uint8_t CalActive;                                                       // When the CT's are used on Sensorbox(1.5 or 2), it enables the CAL menu option.
extern uint16_t Iuncal;
extern uint16_t SolarStopTimer;
extern int32_t EnergyCharged;
extern int32_t EnergyCapacity;
extern int16_t PowerMeasured;
extern uint8_t RFIDstatus;
extern bool LocalTimeSet;
extern uint32_t serialnr;

extern uint8_t MenuItems[MENU_EXIT];

enum EnableC2_t { NOT_PRESENT, ALWAYS_OFF, SOLAR_OFF, ALWAYS_ON, AUTO };
const static char StrEnableC2[][12] = { "Not present", "Always Off", "Solar Off", "Always On", "Auto" };
enum Single_Phase_t { FALSE, GOING_TO_SWITCH, AFTER_SWITCH };
extern Single_Phase_t Switching_To_Single_Phase;
extern uint8_t Nr_Of_Phases_Charging;

const struct {
    char LCD[10];
    char Desc[52];
    uint16_t Min;
    uint16_t Max;
    uint16_t Default;
} MenuStr[MENU_EXIT + 1] = {
    {"", "Not in menu", 0, 0, 0},
    {"", "Hold 2 sec", 0, 0, 0},

    // Node specific configuration
    /* LCD,       Desc,                                                 Min, Max, Default */
    {"CONFIG",  "Fixed Cable or Type 2 Socket",                       0, 1, CONFIG},
    {"LOCK",    "Cable locking actuator type",                        0, 2, LOCK},
    {"MIN",     "MIN Charge Current the EV will accept (per phase)",  MIN_CURRENT, 16, MIN_CURRENT},
    {"MAX",     "MAX Charge Current for this EVSE (per phase)",       6, 80, MAX_CURRENT},
    {"PWR SHARE", "Share Power between multiple SmartEVSEs (2-8)",    0, NR_EVSES, LOADBL},
    {"SWITCH",  "Switch function control on pin SW",                  0, 4, SWITCH},
    {"RCMON",   "Residual Current Monitor on pin RCM",                0, 1, RC_MON},
    {"RFID",    "RFID reader, learn/remove cards",                    0, 5, RFID_READER},
    {"EV METER","Type of EV electric meter",                          0, EM_CUSTOM, EV_METER},
    {"EV ADDR", "Address of EV electric meter",                       MIN_METER_ADDRESS, MAX_METER_ADDRESS, EV_METER_ADDRESS},

    // System configuration
    /* LCD,       Desc,                                                 Min, Max, Default */
    {"MODE",    "Normal, Smart or Solar EVSE mode",                   0, 2, MODE},
    {"CIRCUIT", "EVSE Circuit max Current",                           10, 160, MAX_CIRCUIT},
    {"GRID",    "Grid type to which the Sensorbox is connected",      0, 1, GRID},
    {"CAL",     "Calibrate CT1 (CT2+3 will also change)",             (unsigned int) (ICAL * 0.3), (unsigned int) (ICAL * 2.0), ICAL}, // valid range is 0.3 - 2.0 times measured value
    {"MAINS",   "Max MAINS Current (per phase)",                      10, 200, MAX_MAINS},
    {"START",   "Surplus energy start Current (sum of phases)",       0, 48, START_CURRENT},
    {"STOP",    "Stop solar charging at 6A after this time",          0, 60, STOP_TIME},
    {"IMPORT",  "Allow grid power when solar charging (sum of phase)",0, 48, IMPORT_CURRENT},
    {"MAINS MET","Type of mains electric meter",                       0, EM_CUSTOM, MAINS_METER},
    {"MAINS ADR","Address of mains electric meter",                    MIN_METER_ADDRESS, MAX_METER_ADDRESS, MAINS_METER_ADDRESS},
    {"BYTE ORD","Byte order of custom electric meter",                0, 3, EMCUSTOM_ENDIANESS},
    {"DATA TYPE","Data type of custom electric meter",                 0, MB_DATATYPE_MAX - 1, EMCUSTOM_DATATYPE},
    {"FUNCTION","Modbus Function of custom electric meter",           3, 4, EMCUSTOM_FUNCTION},
    {"VOL REGI","Register for Voltage (V) of custom electric meter",  0, 65530, EMCUSTOM_UREGISTER},
    {"VOL DIVI","Divisor for Voltage (V) of custom electric meter",   0, 7, EMCUSTOM_UDIVISOR},
    {"CUR REGI","Register for Current (A) of custom electric meter",  0, 65530, EMCUSTOM_IREGISTER},
    {"CUR DIVI","Divisor for Current (A) of custom electric meter",   0, 7, EMCUSTOM_IDIVISOR},
    {"POW REGI","Register for Power (W) of custom electric meter",    0, 65534, EMCUSTOM_PREGISTER},
    {"POW DIVI","Divisor for Power (W) of custom electric meter",     0, 7, EMCUSTOM_PDIVISOR},
    {"ENE REGI","Register for Energy (kWh) of custom electric meter", 0, 65534, EMCUSTOM_EREGISTER},
    {"ENE DIVI","Divisor for Energy (kWh) of custom electric meter",  0, 7, EMCUSTOM_EDIVISOR},
    {"READ MAX","Max register read at once of custom electric meter", 3, 255, 3},
    {"WIFI",    "Connect to WiFi access point",                       0, 2, WIFI_MODE},
    {"AUTOUPDAT","Automatic Firmware Update",                         0, 1, AUTOUPDATE},
    {"CONTACT 2","Contactor2 (C2) behaviour",                          0, sizeof(StrEnableC2) / sizeof(StrEnableC2[0])-1, ENABLE_C2},
    {"MAX TEMP","Maximum temperature for the EVSE module",            40, 75, MAX_TEMPERATURE},
    {"SUM MAINS","Capacity Rate limit on sum of MAINS Current (A)",    10, 600, MAX_SUMMAINS},
#if ENABLE_OCPP
    {"OCPP",    "Select OCPP mode",                                   0, 1, OCPP_MODE},
#endif
    {"", "Hold 2 sec to stop charging", 0, 0, 0},
    {"", "Hold 2 sec to start charging", 0, 0, 0},

    {"EXIT", "EXIT", 0, 0, 0}
};


struct EMstruct {
    uint8_t Desc[10];
    uint8_t Endianness;     // 0: low byte first, low word first, 1: low byte first, high word first, 2: high byte first, low word first, 3: high byte first, high word first
    uint8_t Function;       // 3: holding registers, 4: input registers
    MBDataType DataType;    // How data is represented on this Modbus meter
    uint16_t URegister;     // Single phase voltage (V)
    int8_t UDivisor;        // 10^x
    uint16_t IRegister;     // Single phase current (A)
    int8_t IDivisor;        // 10^x
    uint16_t PRegister;     // Total power (W) -- only used for EV/PV meter momentary power
    int8_t PDivisor;        // 10^x
    uint16_t ERegister;     // Total imported energy (kWh); equals total energy if meter doesnt support exported energy
    int8_t EDivisor;        // 10^x
    uint16_t ERegister_Exp; // Total exported energy (kWh)
    int8_t EDivisor_Exp;    // 10^x
};

extern struct EMstruct EMConfig[EM_CUSTOM + 1];



#define EPOCH2_OFFSET 1672531200

extern struct DelayedTimeStruct DelayedStartTime;

void CheckAPpassword(void);
void read_settings();
void write_settings(void);
void setSolarStopTimer(uint16_t Timer);
void setState(uint8_t NewState);
void setAccess(bool Access);
void SetCPDuty(uint32_t DutyCycle);
uint8_t setItemValue(uint8_t nav, uint16_t val);
uint16_t getItemValue(uint8_t nav);
void ConfigureModbusMode(uint8_t newmode);

void handleWIFImode(void);

#if ENABLE_OCPP
void ocppUpdateRfidReading(const unsigned char *uuid, size_t uuidLen);
#endif //ENABLE_OCPP

#endif
