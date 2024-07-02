#pragma once

#include "Arduino.h"


#define CARD_OFFSET 0
#define RFID_READER 0
#define RFIDLOCKTIME 60         // Seconds delay for the EVSE to lock again (RFIDreader = EnableOne)




uint16_t getCardOffset(void);
uint8_t getRFIDreaderMode(void);
uint8_t getAccessTimer(void);
void setAccessTimer(uint8_t newtime);
