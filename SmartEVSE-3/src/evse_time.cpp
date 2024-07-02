#include "evse_time.h"

struct DelayedTimeStruct DelayedStartTime;
struct DelayedTimeStruct DelayedStopTime;

uint32_t getDelayedStartTime(void)
{
  return DelayedStartTime.epoch2;
}

uint32_t getDelayedStopTime(void)
{
  return DelayedStopTime.epoch2;
}