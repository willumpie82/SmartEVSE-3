#pragma once

#ifndef DBG
//the wifi-debugger is available by telnetting to your SmartEVSE device
#define DBG 0  //comment or set to 0 for production release, 0 = no debug 1 = debug over telnet, 2 = debug over usb serial
#endif


#if DBG == 0
//used to steer RemoteDebug
#define DEBUG_DISABLED 1
#define _LOG_W( ... ) //dummy
#define _LOG_I( ... ) //dummy
#define _LOG_D( ... ) //dummy
#define _LOG_V( ... ) //dummy
#define _LOG_A( ... ) //dummy
#define _LOG_W_NO_FUNC( ... ) //dummy
#define _LOG_I_NO_FUNC( ... ) //dummy
#define _LOG_D_NO_FUNC( ... ) //dummy
#define _LOG_V_NO_FUNC( ... ) //dummy
#define _LOG_A_NO_FUNC( ... ) //dummy
#endif

#if DBG == 1
#define _LOG_A(fmt, ...) if (Debug.isActive(Debug.ANY))                Debug.printf("(%s)(C%d) " fmt, __func__, xPortGetCoreID(), ##__VA_ARGS__) //Always = Errors!!!
#define _LOG_P(fmt, ...) if (Debug.isActive(Debug.PROFILER))   Debug.printf("(%s)(C%d) " fmt, __func__, xPortGetCoreID(), ##__VA_ARGS__)
#define _LOG_V(fmt, ...) if (Debug.isActive(Debug.VERBOSE))    Debug.printf("(%s)(C%d) " fmt, __func__, xPortGetCoreID(), ##__VA_ARGS__)         //Verbose
#define _LOG_D(fmt, ...) if (Debug.isActive(Debug.DEBUG))              Debug.printf("(%s)(C%d) " fmt, __func__, xPortGetCoreID(), ##__VA_ARGS__) //Debug
#define _LOG_I(fmt, ...) if (Debug.isActive(Debug.INFO))               Debug.printf("(%s)(C%d) " fmt, __func__, xPortGetCoreID(), ##__VA_ARGS__) //Info
#define _LOG_W(fmt, ...) if (Debug.isActive(Debug.WARNING))    Debug.printf("(%s)(C%d) " fmt, __func__, xPortGetCoreID(), ##__VA_ARGS__)         //Warning
#define _LOG_E(fmt, ...) if (Debug.isActive(Debug.ERROR))              Debug.printf("(%s)(C%d) " fmt, __func__, xPortGetCoreID(), ##__VA_ARGS__) //Error not used!
#define _LOG_A_NO_FUNC(fmt, ...) if (Debug.isActive(Debug.ANY))                Debug.printf(fmt, ##__VA_ARGS__)
#define _LOG_P_NO_FUNC(fmt, ...) if (Debug.isActive(Debug.PROFILER))   Debug.printf(fmt, ##__VA_ARGS__)
#define _LOG_V_NO_FUNC(fmt, ...) if (Debug.isActive(Debug.VERBOSE))    Debug.printf(fmt, ##__VA_ARGS__)
#define _LOG_D_NO_FUNC(fmt, ...) if (Debug.isActive(Debug.DEBUG))              Debug.printf(fmt, ##__VA_ARGS__)
#define _LOG_I_NO_FUNC(fmt, ...) if (Debug.isActive(Debug.INFO))               Debug.printf(fmt, ##__VA_ARGS__)
#define _LOG_W_NO_FUNC(fmt, ...) if (Debug.isActive(Debug.WARNING))    Debug.printf(fmt, ##__VA_ARGS__)
#define _LOG_E_NO_FUNC(fmt, ...) if (Debug.isActive(Debug.ERROR))              Debug.printf(fmt, ##__VA_ARGS__)
#include "RemoteDebug.h"  //https://github.com/JoaoLopesF/RemoteDebug
extern RemoteDebug Debug;
#endif