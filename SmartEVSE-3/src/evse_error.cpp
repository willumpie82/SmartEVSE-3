#include "evse_error.h"


uint8_t ErrorFlags = NO_ERROR;

uint8_t getErrorFlags(void)
{
  return ErrorFlags;
}

void setError(uint8_t errorflag)
{
  ErrorFlags = errorflag;
}

void ClearErrors(void)
{
  ErrorFlags &= ~(NO_SUN | LESS_6A);                              // Clear All errors
}

void clearError(uint8_t mask)
{
  ErrorFlags &= ~mask;
}