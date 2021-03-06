#include "Sense_ADS131A04.h"
#include <SPI.h>

/* ----------------------- Internal Functions --------------------------------*/
uint32_t parseM(unsigned char BUFFER[3], int SRT) {
  uint32_t STATUS = ((BUFFER[0] << 8) & 0xFF00) | ((BUFFER[1]) & 0x00FF); // Read first 16 bits of buffer

  // Send or receive switch case
  switch (SRT) {
    case 0:
      Serial.print("R: ");
      break;
    case 1:
      Serial.print("S: ");
      break;
  }

  // Decode cases and determine returned STATUS messages
  switch (STATUS) {
    case 0x0000:
      Serial.print("NULL ");
      break;
    case 0x0011:
      Serial.print("RESET ");
      break;
    case 0xFF04:
      Serial.print("READY ");
      break;
    case 0x0022:
      Serial.print("STANDBY ");
      break;
    case 0x0033:
      Serial.print("WAKING UP ");
      break;
    case 0x0555:
      Serial.print("LOCKED ");
      break;
    case 0x0655:
      Serial.print("UNLOCK ");
      break;

    default:
      //If none of these, check R/W REG
      switch ((STATUS & 0xE000) >> 13) {
        case 0b010:
          Serial.print("WREG\t");
          break;
        case 0b001:
          Serial.print("RREG\t");
          break;
        default:
          Serial.print("STATUS ERROR");
          break;
      }
      break;
  }
  Serial.print("\t");
  Serial.println(STATUS, HEX);
  return (STATUS);
}


/* ----------------------- Main External Functions ---------------------------*/
Sense_ADS131A04::Sense_ADS131A04(const uint8_t _ID, const uint8_t _csPin, const uint8_t _misoPin, const uint8_t _mosiPin, const uint8_t _sclkPin, const uint8_t _drdyPin){
  ID = _ID;
  csPin = _csPin;
  misoPin = _misoPin;
  mosiPin = _mosiPin;
  sclkPin = _sclkPin;
  drdyPin = _drdyPin;
}

void Sense_ADS131A04::displayInfo(){
  char cbuff[32];
  Serial.print("AFE Status for Well #");
  Serial.println(ID);
  sprintf(cbuff, "CS[%d] MISO[%d] MOSI[%d] SCLK[%d] DRDY[%d]\n", csPin, misoPin, mosiPin, sclkPin, drdyPin);
  Serial.println(cbuff);
}

void Sense_ADS131A04::configure(){
  Serial.print("---------------Setting up Well #");
  Serial.print(ID);
  Serial.println("---------------");
  pinMode(csPin, OUTPUT);

  delay(100); // Allow for startup time
  adsWriteCommand(UNLOCK, UNLOCK); // Unlock registers for setting WREG
  adsWriteCommand(RESET, READY); // Ensure POR
  adsWriteCommand(UNLOCK, UNLOCK); // Unlock registers for setting WREG

  // Default hardware register settings
  adsWriteRegister(A_SYS_CFG, A_SYS_CFG_DEFAULT); //0B
  adsWriteRegister(D_SYS_CFG, D_SYS_CFG_DEFAULT); //0C
  adsWriteRegister(CLK1, CLK1_DEFAULT);           //0D
  adsWriteRegister(CLK2, CLK2_DEFAULT);           //0E
  adsWriteRegister(ADC1, GAIN_16);                 //11
  adsWriteRegister(ADC2, GAIN_16);                 //12
  adsWriteRegister(ADC3, GAIN_16);                 //13
  adsWriteRegister(ADC4, GAIN_16);                 //14
  adsWriteRegister(ADC_ENA, ADC_ENA_DEFAULT);     //0F
  Serial.println("Configuration Complete");
}

void Sense_ADS131A04::adsWriteCommand(uint32_t COMMAND, uint32_t STATUS) {
  unsigned char SND_BUF[3];
  unsigned char REC_BUF[3];
  uint32_t RETURNED_STATUS = 0x00;

  // Rerun transaction if status not received
  while (RETURNED_STATUS != STATUS) {

    // Format command into I/O buffer MSB -> first 16 bits

    SND_BUF[0] = (COMMAND & 0xFF00) >> 8;
    SND_BUF[1] = COMMAND & 0xFF;
    SND_BUF[2] = 0x00;

    REC_BUF[0] = 0x00;
    REC_BUF[1] = 0x00;
    REC_BUF[2] = 0x00;

    parseM(SND_BUF, SENT); // Parse received message

    // Enable ~CS then transfer I/O buffer, get response
    SPI.beginTransaction(SPI_SETTINGS);
    digitalWrite(csPin, LOW);
    SPI.transfer(&SND_BUF, 3);
    digitalWrite(csPin, HIGH);

    digitalWrite(csPin, LOW);
    SPI.transfer(&REC_BUF, 3);
    digitalWrite(csPin, HIGH);

    // Response is given to message on subsequent data frame
    SPI.endTransaction();
    RETURNED_STATUS = parseM(REC_BUF, RECEIVED);
    delay(20);
  }
}

void Sense_ADS131A04::adsWriteRegister(unsigned char REG_ADDRESS, uint32_t REG_DATA){

  // Ensure register message is in format 0b010a aaaa nnnn nnnn with reg address a* and data n*
  uint32_t FORMATTED_WREG_COMMAND = ((0b010 << 13) | (REG_ADDRESS << 8) | (REG_DATA & 0xFF)) & 0xFFFFFF;
  uint32_t EXPECTED_STATUS = (0b001 << 13) | (REG_ADDRESS << 8) | (REG_DATA & 0xFF);
  adsWriteCommand(FORMATTED_WREG_COMMAND, EXPECTED_STATUS);
}

void Sense_ADS131A04::adsReadRegister(unsigned char REG_ADDRESS){
  unsigned char REC_BUF[3] = {0x00, 0x00, 0x00};
  unsigned char SND_BUF[3];
  uint32_t FORMATTED_RREG_COMMAND = ((0b001 << 13) | (REG_ADDRESS << 8)) & 0xFFFF00;

  SPI.beginTransaction(SPI_SETTINGS);
  SND_BUF[0] = (FORMATTED_RREG_COMMAND >> 8) & 0xFF;
  SND_BUF[1] = FORMATTED_RREG_COMMAND & 0xFF;
  SND_BUF[2] = 0x00;
  parseM(SND_BUF, SENT);

  // Enable ~CS then transfer I/O buffer, get response

  digitalWrite(csPin, LOW);
  SPI.transfer(&SND_BUF, 3);
  digitalWrite(csPin, HIGH);

  digitalWrite(csPin, LOW);
  SPI.transfer(&REC_BUF, 3);
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  parseM(REC_BUF, RECEIVED);
}

void Sense_ADS131A04::setGain(uint8_t gainSetting){
  adsWriteCommand(ADC1, gainSetting);
  adsWriteCommand(ADC2, gainSetting);
  adsWriteCommand(ADC3, gainSetting);
  adsWriteCommand(ADC4, gainSetting);
}

void Sense_ADS131A04::wakeUp(){
  Serial.print("---------------Waking up AFE #");
  Serial.print(ID);
  Serial.println("---------------");
  delay(100);
  adsWriteRegister(ADC_ENA, 0b00001111);
  adsWriteCommand(WAKEUP, WAKEUP);
  adsWriteCommand(LOCK, LOCK);
}

void Sense_ADS131A04::adsSample(){
  uint8_t BUF_SIZE = 15;
  unsigned char SND_BUF[BUF_SIZE];
  long CH1, CH2, CH3, CH4;
  uint8_t i;

  for(i = 0; i < BUF_SIZE; i++){
    SND_BUF[i] = 0x00;
  }
  // Begin SPI transaction using designated settings
  SPI.begin();
  SPI.beginTransaction(SPI_SETTINGS);
  digitalWrite(csPin, LOW);
  SPI.transfer(&SND_BUF, BUF_SIZE);
  digitalWrite(csPin, HIGH);

  SPI.endTransaction();
  
  CH1 = (SND_BUF[3] << 16) | (SND_BUF[4] << 8) | SND_BUF[5];
  CH2 = (SND_BUF[6] << 16) | (SND_BUF[7] << 8) | SND_BUF[8];
  CH3 = (SND_BUF[9] << 16) | (SND_BUF[10] << 8) | SND_BUF[11];
  CH4 = (SND_BUF[12] << 16) | (SND_BUF[13] << 8) | SND_BUF[14];

  if (CH1 & 0x00800000){
    CH1 |= 0xFF000000 ;
  }
  if (CH2 & 0x00800000){
    CH2 |= 0xFF000000 ;
  }
  if (CH3 & 0x00800000){
    CH3 |= 0xFF000000 ;
  }
  if (CH4 & 0x00800000){
    CH4 |= 0xFF000000 ;
  }

  CH1_f = (CH1 / 3495253.3333333);
  CH2_f = (CH2 / 3495253.3333333);
  CH3_f = (CH3 / 3495253.3333333);
  CH4_f = (CH4 / 3495253.3333333);

  Serial.print(CH1_f, 24);
  Serial.print(",");
  Serial.print(CH2_f, 24);
  Serial.print(",");
  Serial.print(CH3_f, 24);
  Serial.print(",");
  Serial.print(CH4_f, 24);
}



