#include "rfid.h"

uint16_t CardOffset = CARD_OFFSET;        // RFID card used in Enable One mode
uint8_t RFIDReader = RFID_READER;         // RFID Reader (0:Disabled / 1:Enabled / 2:Enable One / 3:Learn / 4:Delete / 5:Delete All)
uint8_t AccessTimer = 0;

uint8_t getAccessTimer(void)
{
  return AccessTimer;
}


uint8_t getRFIDreaderMode(void)
{
  return RFIDReader;
}

uint16_t getCardOffset(void)
{
  return CardOffset;
}
