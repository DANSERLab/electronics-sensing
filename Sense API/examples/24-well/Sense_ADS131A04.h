#ifndef Sense_ADS131A04_H
#define Sense_ADS131A04_H

#include <Stdio.h>
#include <SPI.h>
/* ----------------------- DEVICE REGISTERS ----------------------------------*/

#define ID_MSB    0x00
#define ID_LSB    0x01
#define STAT_1    0x02
#define STAT_P    0x03
#define STAT_N    0x04
#define STAT_S    0x05
#define ERROR_CNT 0x06
#define STAT_M2   0x07
#define A_SYS_CFG 0x0B
#define D_SYS_CFG 0x0C
#define CLK1      0x0D
#define CLK2      0x0E
#define ADC_ENA   0x0F
#define ADC1      0x11
#define ADC2      0x12
#define ADC3      0x13
#define ADC4      0x14

#define  NUL       0x0000
#define  RESET     0x0011
#define  STANDBY   0x0022
#define  WAKEUP    0x0033
#define  LOCK      0x0555
#define  UNLOCK    0x0655
#define  READY     0xFF04


/* ----------------------- SPI CONFIG ----------------------------------------*/

#define SPI_CLOCK_SPEED 2000000
#define SENT 1
#define RECEIVED 0

const SPISettings SPI_SETTINGS(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE1);

// Command List for ADS131A04

#define  NUL       0x0000
#define  RESET     0x0011
#define  STANDBY   0x0022
#define  WAKEUP    0x0033
#define  LOCK      0x0555
#define  UNLOCK    0x0655


/* ----------------------- DEVICE CONFIG -------------------------------------*/

#define A_SYS_CFG_DEFAULT 0b01101000
#define D_SYS_CFG_DEFAULT 0b00111100
#define CLK1_DEFAULT      0b00000010
#define CLK2_DEFAULT      0b00100000
#define ADC_ENA_DEFAULT   0b00000000

enum ADS_OSR{
  OSR_4096  = 0x0,
  OSR_2048  = 0x1,
  OSR_1024  = 0x2,
  OSR_800   = 0x3,
  OSR_768   = 0x4,
  OSR_512   = 0x5,
  OSR_400   = 0x6,
  OSR_384   = 0x7,
  OSR_256   = 0x8,
  OSR_200   = 0x9,
  OSR_192   = 0xA,
  OSR_128   = 0xB,
  OSR_96    = 0xC,
  OSR_64    = 0xD,
  OSR_48    = 0xE,
  OSR_32    = 0xF
};

#define GAIN_1  0b00000000
#define GAIN_2  0b00000001
#define GAIN_4  0b00000010
#define GAIN_8  0b00000011
#define GAIN_16 0b00000100
  
/*============================================================================*/

class Sense_ADS131A04{
  private:
    uint8_t ID, csPin, misoPin, mosiPin, sclkPin, drdyPin;
    float CH1_f, CH2_f, CH3_f, CH4_f;
    
  public:
    Sense_ADS131A04(const uint8_t _ID, const uint8_t _csPin, const uint8_t _misoPin, const uint8_t _mosiPin, const uint8_t _sclkPin, const uint8_t _drdyPin);
    
    void configure();
    void adsWriteCommand(uint32_t COMMAND, uint32_t STATUS);
    void adsWriteRegister(unsigned char REG_ADDRESS, uint32_t REG_DATA);
    void adsReadRegister(unsigned char REG_ADDRESS);
    void adsSample();
    void displayInfo();
    void wakeUp();
    void setGain(uint8_t gainSetting);

};

#endif





