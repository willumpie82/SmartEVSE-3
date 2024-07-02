#pragma once

#include "Arduino.h"

#define NO_ERROR 0
#define LESS_6A 1
#define CT_NOCOMM 2
#define TEMP_HIGH 4
#define EV_NOCOMM 8
#define RCM_TRIPPED 16                                                          // RCM tripped. >6mA DC residual current detected.
#define NO_SUN 32
#define Test_IO 64
#define BL_FLASH 128

uint8_t getErrorFlags(void);
void ClearErrors(void);
void clearError(uint8_t mask);
void setError(uint8_t errorflag);

