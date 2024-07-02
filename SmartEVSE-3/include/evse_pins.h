#pragma once
// Pin definitions left side ESP32
#define PIN_TEMP 36
#define PIN_CP_IN 39
#define PIN_PP_IN 34
#define PIN_LOCK_IN 35
#define PIN_SSR 32
#define PIN_LCD_SDO_B3 33                                                       // = SPI_MOSI
#define PIN_LCD_A0_B2 25
#define PIN_LCD_CLK 26                                                          // = SPI_SCK
#define PIN_SSR2 27
#define PIN_LCD_LED 14
#define PIN_LEDB 12
#define PIN_RCM_FAULT 13

// Pin definitions right side ESP32
#define PIN_RS485_RX 23
#define PIN_RS485_DIR 22
//#define PIN_RXD 
//#define PIN_TXD
#define PIN_RS485_TX 21
#define PIN_CP_OUT 19
#define PIN_ACTB 18
#define PIN_LCD_RST 5
#define PIN_ACTA 17
#define PIN_SW_IN 16
#define PIN_LEDG 4
#define PIN_IO0_B1 0
#define PIN_LEDR 2
#define PIN_CPOFF 15

#define SPI_MOSI 33                                                             // SPI connections to LCD
#define SPI_MISO -1
#define SPI_SCK 26
#define SPI_SS -1