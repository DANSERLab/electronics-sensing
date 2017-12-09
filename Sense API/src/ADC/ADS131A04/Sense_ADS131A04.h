#include <SPI.h>


/* ----------------------- DEVICE REGISTERS ----------------------------------*/

#define ID_MSB    0X00
#define ID_LSB    0X01
#define STAT_1    0X02
#define STAT_P    0X03
#define STAT_N    0X04
#define STAT_S    0X05
#define ERROR_CNT 0X06
#define STAT_M2   0X07
#define A_SYS_CFG 0X0A
#define D_SYS_CFG 0x0C
#define CLK1      0X0D
#define CLK2      0X0E
#define ADC_ENA   0X0F
#define ADC1      0X11
#define ADC2      0X12
#define ADC3      0X13
#define ADC4      0X14

/* ----------------------- SPI CONFIG ----------------------------------------*/

#define SPI_CLOCK_SPEED 2000000
#define SSP 6
#define DRDY 4
#define SENT 1
#define RECEIVED 0

// Command List for ADS131A04

#define  NUL       0x0000
#define  RESET     0x0011
#define  STANDBY   0x0022
#define  WAKEUP    0x0033
#define  LOCK      0x0555
#define  UNLOCK    0x0655


/* ----------------------- DEVICE CONFIG -------------------------------------*/

/* ----------------------- ID_MSB --------------------------------------------*/

typedef union F_ID_MSB{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- ID_LSB --------------------------------------------*/

// RESERVED

/* ----------------------- STAT_1 --------------------------------------------*/

typedef union F_STAT_1{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- STAT_P --------------------------------------------*/

typedef union F_STAT_P{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- STAT_N --------------------------------------------*/

typedef union F_STAT_N{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- STAT_S --------------------------------------------*/

typedef union F_STAT_S{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- ERROR_CNT -----------------------------------------*/

typedef union F_ERROR_CNT{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- STAT_M2 -------------------------------------------*/

typedef union F_STAT_M2{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- A_SYS_CFG -----------------------------------------*/

typedef union F_CONF_A_SYS_CFG{
  struct{
    unsigned VNCPEN   : 1;
    unsigned HRM      : 1;
    unsigned RSRVD    : 1;
    unsigned VREF_4V  : 1;
    unsigned INT_REFN : 1;
    unsigned COMP_TH  : 3;
  };
  uint8_t raw;
};

/* ----------------------- D_SYS_CFG -----------------------------------------*/

typedef union F_CONF_D_SYS_CFG {
  struct{
    unsigned WDT_EN   : 1;
    unsigned CRC_MODE : 1;
    unsigned DNDLY    : 3;
    unsigned HIZDLY   : 3;
    unsigned FIXED    : 1;
    unsigned CRC_EN   : 1;
  };
  uint8_t raw;
};

/* ----------------------- CLK1 ----------------------------------------------*/
typedef union F_CLK1 {
  struct{
    unsigned CLKSRC   : 1;
    unsigned RSRVD_0  : 3;
    unsigned CLK_DIV  : 3;
    unsigned RSRVD_1  : 1;
  };
  uint8_t raw;
};

/* ----------------------- CLK2 ----------------------------------------------*/

typedef union F_CLK2 {
  struct{
    unsigned ICLK_DIV  : 3;
    unsigned RSRVD     : 1;
    unsigned OSR       : 3;
  };
  uint8_t raw;
};

typedef enum{
  OSR_4096  = 0x0;
  OSR_2048  = 0x1;
  OSR_1024  = 0x2;
  OSR_800   = 0x3;
  OSR_768   = 0x4;
  OSR_512   = 0x5;
  OSR_400   = 0x6;
  OSR_384   = 0x7;
  OSR_256   = 0x8;
  OSR_200   = 0x9;
  OSR_192   = 0xA;
  OSR_128   = 0xB;
  OSR_96    = 0xC;
  OSR_64    = 0xD;
  OSR_48    = 0xE;
  OSR_32    = 0xF;
} ADS_OSR;

/* ----------------------- ADC_ENA -------------------------------------------*/
typedef union F_ADC_ENA {
  struct{
    unsigned RSRVD  : 4;
    unsigned OSR    : 4;
  };
  uint8_t raw;
};

typedef enum{
  ADC_OFF = 0x0;
  ADC_ON  = 0xF;
} ADS_ADC_POWER;

/* ----------------------- ADC -----------------------------------------------*/
typedef enum{
  GAIN_0  = 0x0;
  GAIN_2  = 0x1;
  GAIN_4  = 0x2;
  GAIN_8  = 0x3;
  GAIN_16 = 0x4;
} ADS_GAIN;

typedef union F_ADC1 {
  struct{
    unsigned RSRVD     : 5;
    ADS_GAIN OSR       : 3;
  };
  uint8_t raw;
};

typedef union F_ADC2 {
  struct{
    unsigned RSRVD     : 5;
    ADS_GAIN OSR       : 3;
  };
  uint8_t raw;
};

typedef union F_ADC3 {
  struct{
    unsigned RSRVD     : 5;
    ADS_GAIN OSR       : 3;
  };
  uint8_t raw;
};

typedef union F_ADC4 {
  struct{
    unsigned RSRVD     : 5;
    ADS_GAIN OSR       : 3;
  };
  uint8_t raw;
};

/*============================================================================*/


class Sense_ADS131A04{
  protected:

  public:

  private:
}
