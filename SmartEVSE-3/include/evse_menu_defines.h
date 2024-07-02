#pragma once
// Node specific configuration
#define MENU_ENTER 1
#define MENU_CONFIG 2                                                           // 0x0100: Configuration
#define MENU_LOCK 3                                                             // 0x0101: Cable lock
#define MENU_MIN 4                                                              // 0x0102: MIN Charge Current the EV will accept
#define MENU_MAX 5                                                              // 0x0103: MAX Charge Current for this EVSE
#define MENU_LOADBL 6                                                           // 0x0104: Load Balance
#define MENU_SWITCH 7                                                           // 0x0105: External Start/Stop button
#define MENU_RCMON 8                                                            // 0x0106: Residual Current Monitor
#define MENU_RFIDREADER 9                                                       // 0x0107: Use RFID reader
#define MENU_EVMETER 10                                                         // 0x0108: Type of EV electric meter
#define MENU_EVMETERADDRESS 11                                                  // 0x0109: Address of EV electric meter

// System configuration (same on all SmartEVSE in a LoadBalancing setup)
#define MENU_MODE 12                                                            // 0x0200: EVSE mode
#define MENU_CIRCUIT 13                                                         // 0x0201: EVSE Circuit max Current
#define MENU_GRID 14                                                            // 0x0202: Grid type to which the Sensorbox is connected
#define MENU_CAL 15                                                             // 0x0203: CT calibration value
#define MENU_MAINS 16                                                           // 0x0204: Max Mains Current
#define MENU_START 17                                                           // 0x0205: Surplus energy start Current
#define MENU_STOP 18                                                            // 0x0206: Stop solar charging at 6A after this time
#define MENU_IMPORT 19                                                          // 0x0207: Allow grid power when solar charging
#define MENU_MAINSMETER 20                                                      // 0x0208: Type of Mains electric meter
#define MENU_MAINSMETERADDRESS 21                                               // 0x0209: Address of Mains electric meter
#define MENU_EMCUSTOM_ENDIANESS 22                                              // 0x020D: Byte order of custom electric meter
#define MENU_EMCUSTOM_DATATYPE 23                                               // 0x020E: Data type of custom electric meter
#define MENU_EMCUSTOM_FUNCTION 24                                               // 0x020F: Modbus Function (3/4) of custom electric meter
#define MENU_EMCUSTOM_UREGISTER 25                                              // 0x0210: Register for Voltage (V) of custom electric meter
#define MENU_EMCUSTOM_UDIVISOR 26                                               // 0x0211: Divisor for Voltage (V) of custom electric meter (10^x)
#define MENU_EMCUSTOM_IREGISTER 27                                              // 0x0212: Register for Current (A) of custom electric meter
#define MENU_EMCUSTOM_IDIVISOR 28                                               // 0x0213: Divisor for Current (A) of custom electric meter (10^x)
#define MENU_EMCUSTOM_PREGISTER 29                                              // 0x0214: Register for Power (W) of custom electric meter
#define MENU_EMCUSTOM_PDIVISOR 30                                               // 0x0215: Divisor for Power (W) of custom electric meter (10^x)
#define MENU_EMCUSTOM_EREGISTER 31                                              // 0x0216: Register for Energy (kWh) of custom electric meter
#define MENU_EMCUSTOM_EDIVISOR 32                                               // 0x0217: Divisor for Energy (kWh) of custom electric meter (10^x)
#define MENU_EMCUSTOM_READMAX 33                                                // 0x0218: Maximum register read (ToDo)
#define MENU_WIFI 34                                                            // 0x0219: WiFi mode
#define MENU_AUTOUPDATE 35
#define MENU_C2 36
#define MENU_MAX_TEMP 37
#define MENU_SUMMAINS 38
#if ENABLE_OCPP == 0
#define MENU_OFF 39                                                             // so access bit is reset and charging stops when pressing < button 2 seconds
#define MENU_ON 40                                                              // so access bit is set and charging starts when pressing > button 2 seconds
#define MENU_EXIT 41
#else
#define MENU_OCPP 39                                                            // OCPP Disable / Enable / Further modes
#define MENU_OFF 40                                                             // so access bit is reset and charging stops when pressing < button 2 seconds
#define MENU_ON 41                                                              // so access bit is set and charging starts when pressing > button 2 seconds
#define MENU_EXIT 42
#endif

#define MENU_STATE 50