#include "solar.h"

uint16_t SolarStopTimer = 0;


/**
 * Set the solar stop timer
 *
 * @param unsigned int Timer (seconds)
 */
void setSolarStopTimer(uint16_t Timer)
{
  SolarStopTimer = Timer;
}