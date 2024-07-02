#pragma once

#include "Arduino.h"
#include <esp_adc_cal.h>
#include "utils.h"

#include <soc/sens_reg.h>
#include <soc/sens_struct.h>
#include <driver/adc.h>
#include <soc/rtc_io_struct.h>

#define ADCBUFFERSIZE 25

//uint32_t getvoltage(uint32_t sample);
uint32_t getADCsampleMin(void);
uint32_t getADCsampleMax(void)

void sampleADC(void);


void ProximityPin();