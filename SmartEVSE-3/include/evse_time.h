#pragma once

#include "Arduino.h"

struct DelayedTimeStruct {
    uint32_t epoch2;        // in case of Delayed Charging the StartTime in epoch2; if zero we are NOT Delayed Charging
                            // epoch2 is the number of seconds since 1/1/2023 00:00 UTC, which equals epoch 1672531200
                            // we avoid using epoch so we don't need expensive 64bits arithmetics with difftime
                            // and we can store dates until 7/2/2159
    int32_t diff;           // StartTime minus current time in seconds
};


uint32_t getDelayedStartTime(void);
uint32_t getDelayedStopTime(void);