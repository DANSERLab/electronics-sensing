#include "Sense_ADS131A04.h"

// MISO, MOSI, and SCLK pins are shared between AFEs

const uint8_t MISO_PIN = 12;
const uint8_t MOSI_PIN = 11;
const uint8_t SCLK_PIN = 13;

int incomingByte;
int i;

// CS and DRDY pins are unique to

uint8_t CS_1 = 4, CS_2 = 5, CS_3 = 6, CS_4 = 7;
uint8_t DRDY_1 = 14, DRDY_2 = 15, DRDY_3 = 16, DRDY_4 = 17;
uint32_t sampleCount = 0;

float outputArray[5];


// Setup for 24 well version
Sense_ADS131A04 AFE1(1, CS_1, MISO_PIN, MOSI_PIN, SCLK_PIN, DRDY_1);
Sense_ADS131A04 AFE2(2, CS_2, MISO_PIN, MOSI_PIN, SCLK_PIN, DRDY_2);
Sense_ADS131A04 AFE3(3, CS_3, MISO_PIN, MOSI_PIN, SCLK_PIN, DRDY_3);
Sense_ADS131A04 AFE4(4, CS_4, MISO_PIN, MOSI_PIN, SCLK_PIN, DRDY_4);


void setup() {

  Serial.begin(9600);
  SPI.begin();
  // Double check values
  Serial.println("---------------AFE 1 Configuration---------------");
  AFE1.displayInfo();
  AFE1.configure();
  Serial.println("---------------AFE 2 Configuration---------------");
  AFE2.displayInfo();
  AFE2.configure();
  Serial.println("---------------AFE 3 Configuration---------------");
  AFE3.displayInfo();
  AFE3.configure();
  Serial.println("---------------AFE 4 Configuration---------------");
  AFE4.displayInfo();
  AFE4.configure();
  

  AFE1.wakeUp();
  AFE2.wakeUp();
  AFE3.wakeUp();
  AFE4.wakeUp();
  Serial.println("---------------Setup Complete---------------");
  Serial.flush();
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if ( incomingByte == 83) {
      generateOutputArray();
    } else {
      Serial.println(incomingByte);
    }
  }
}

void generateOutputArray() {
  AFE1.adsSample();
  Serial.print(",");
  AFE2.adsSample();
  Serial.print(",");
  AFE3.adsSample();
  Serial.print(",");
  AFE4.adsSample();
  Serial.print("|");
}


