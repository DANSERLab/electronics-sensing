#include <SPI.h>

#define SPI_CLOCK_SPEED 16384000
#define CHIP_SELECT 6
#define SENT 1
#define RECEIVED 0

// Command List for ADS131A04

#define  NUL       0x0000
#define  RESET     0x0011
#define  STANDBY   0x0022
#define  WAKEUP    0x0033
#define  LOCK      0x0555
#define  UNLOCK    0x0655

// Response List for ADS131A04

const uint32_t READY = 0xFF04;

// Timing List for ADS131A04

const int TIME_RESET = 4.5;

// Misc

int RETURN_WORD_LENGTH = 3;

// Data

uint32_t CH1, CH2, CH3, CH4;

/* Setup primarily involves resetting the ADS, writing desired settings, and
 * locking registers before reading ADC data. */

void setup() {
  Serial.begin(9600);
  Serial.println("Setting up...");
  SPI.begin();

  pinMode(CHIP_SELECT, OUTPUT);

  delay(1000); // Allow for startup time

  adsWriteCommand(RESET, READY); // Ensure POR
  adsWriteCommand(UNLOCK, UNLOCK); // Unlock registers for setting WREG
  adsWriteRegister(0x0D, 0b10000010); // Set internal clock to use SLK
  adsWriteRegister(0x0E, 0b00100000); // Set ICLK divider ratio and OSR

  // Register Status Checks

  int i;
  for (i = 0; i < 0x14; i++) {
    adsReadRegister(i);
  }

  
  // After register settings are cleared, start ADC process
  adsWriteRegister(0x0F, 0b00001111); // Power on ADCs
  adsWriteCommand(WAKEUP, WAKEUP); // Wake up ADCs
  adsWriteCommand(LOCK, LOCK); // Lock ADC
}

void loop() {
  delay(1000);
  adsSample(&CH1, &CH2, &CH3, &CH4); // Sample ADCS

  // Use these when checking STAT_X registers.
  /*
  adsWriteCommand(NULL, 0x2220);
  adsReadRegister(0x05);
  adsReadRegister(0x03);
  adsReadRegister(0x04);
  */
}

/* Message Parsing: decode received chars from ADS131A04 and
 * display commands/STATUS words. */

uint32_t parseM(unsigned char BUFFER[3], int SRT) {
  uint32_t STATUS = ((BUFFER[0] << 8) & 0xFF00) | ((BUFFER[1]) & 0x00FF); // Read first 16 bits of buffer

  // Send or receive switch case
  switch (SRT) {
    case 0:
      Serial.print("Returned: ");
      break;
    case 1:
      Serial.print("Sent: ");
      break;
  }
  Serial.print(STATUS, HEX);
  Serial.print(" = ");

  // Decode cases and determine returned STATUS messages
  switch (STATUS) {
    case 0x0000:
      Serial.println("NULL");
      break;
    case 0x0011:
      Serial.println("RESET");
      break;
    case 0xFF82:
      Serial.println("RESETTING...");
      break;
    case 0xFF04:
      Serial.println("READY");
      break;
    case 0x0022:
      Serial.println("STANDBY...");
      break;
    case 0x0033:
      Serial.println("WAKING UP...");
      break;
    case 0x0555:
      Serial.println("LOCKED");
      break;
    case 0x0655:
      Serial.println("UNLOCKED");
      break;

    default:
      //If none of these, check R/W REG
      switch ((STATUS & 0xE000) >> 13) {
        case 0b010:
          Serial.println("WREG SENT");
          break;
        case 0b001:
          Serial.println("RREG");
          break;
        default:
          Serial.print("STATUS ERROR\n");
          break;
      }
      break;
  }
  return (STATUS);
}


// Write Command to ADS131A04 and get status response

void adsWriteCommand(uint32_t COMMAND, uint32_t STATUS) {
  unsigned char REC_BUF[3] = {0x00, 0x00, 0x00};
  unsigned char SND_BUF[3];
  uint32_t RETURNED_STATUS = 0x00;

  Serial.println(); // Newline for serial monitor cleanliness

  // Begin SPI transaction using designated settings
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE1));

  // Rerun transaction if status not received
  while (RETURNED_STATUS != STATUS) {
    
    // Format command into I/O buffer MSB -> first 16 bits

    SND_BUF[0] = (COMMAND >> 8) & 0xFF;
    SND_BUF[1] = COMMAND & 0xFF;
    SND_BUF[2] = 0x00;

    parseM(SND_BUF, SENT); // Parse received message

    // Enable ~CS then transfer I/O buffer, get response

    digitalWrite(CHIP_SELECT, LOW);
    SPI.transfer(&SND_BUF, 3);
    digitalWrite(CHIP_SELECT, HIGH);

    // Response is given to message on subsequent data frame
    
    digitalWrite(CHIP_SELECT, LOW);
    SPI.transfer(&REC_BUF, 3);
    digitalWrite(CHIP_SELECT, HIGH);
    SPI.endTransaction();

    RETURNED_STATUS = parseM(REC_BUF, RECEIVED);
  }

  SPI.endTransaction();
}


void adsWriteRegister(unsigned char REG_ADDRESS, uint32_t REG_DATA) {
  Serial.print("\nWriting Register ");
  Serial.print(REG_ADDRESS, HEX);
  Serial.print(" with data ");

  // Ensure register message is in format 0b010a aaaa nnnn nnnn with reg address a* and data n*
  uint32_t FORMATTED_WREG_COMMAND = ((0b010 << 13) | (REG_ADDRESS << 8) | (REG_DATA & 0xFF)) & 0xFFFFFF;
  Serial.print(REG_DATA, BIN);
  uint32_t EXPECTED_STATUS = (0b001 << 13) | (REG_ADDRESS << 8) | (REG_DATA & 0xFF);
  adsWriteCommand(FORMATTED_WREG_COMMAND, EXPECTED_STATUS);
}


void adsReadRegister(char REG_ADDRESS) {
  unsigned char REC_BUF[3] = {0x00, 0x00, 0x00};
  unsigned char SND_BUF[3];
  Serial.print("\nReading Register ");
  Serial.println(REG_ADDRESS, HEX);
  uint32_t FORMATTED_RREG_COMMAND = ((0b001 << 13) | (REG_ADDRESS << 8)) & 0xFFFF00;

  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE1));
  SND_BUF[0] = (FORMATTED_RREG_COMMAND >> 8) & 0xFF;
  SND_BUF[1] = FORMATTED_RREG_COMMAND & 0xFF;
  SND_BUF[2] = 0x00;
  parseM(SND_BUF, SENT);

  // Enable ~CS then transfer I/O buffer, get response

  digitalWrite(CHIP_SELECT, LOW);
  delay(1);
  SPI.transfer(&SND_BUF, 3);
  digitalWrite(CHIP_SELECT, HIGH);

  digitalWrite(CHIP_SELECT, LOW);
  SPI.transfer(&REC_BUF, 3);
  digitalWrite(CHIP_SELECT, HIGH);
  SPI.endTransaction();
  parseM(REC_BUF, RECEIVED);

}


void adsSample(uint32_t *CH1, uint32_t *CH2, uint32_t *CH3, uint32_t *CH4) {
  uint8_t SIZE_BUF = 15;
  unsigned char SND_BUF[SIZE_BUF];

  int i;

  for(i = 0; i < SIZE_BUF; i++){
    SND_BUF[i] = 0x00;
  }
  
  Serial.println();

  // Begin SPI transaction using designated settings
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE1));

  digitalWrite(CHIP_SELECT, LOW);
  delay(0.1);
  SPI.transfer(&SND_BUF, SIZE_BUF);
  digitalWrite(CHIP_SELECT, HIGH);
  
  SPI.endTransaction();
  
  pHex(SND_BUF, SIZE_BUF);
}

void pHex(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
  char tmp[16];
  for (int i = 0; i < length; i++) {
    sprintf(tmp, "%.2X", data[i]);
    Serial.print(tmp);
    Serial.print(" ");
  }
  Serial.println();
}













