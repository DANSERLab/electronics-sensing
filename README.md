# Braingineers Electronics & Sensing Library

This repository contains the low-level communication protocols necessary for communicating with the sensing hardware, as well as the high-level UX to relay data from the hardware's analog front ends (AFEs).

## Hardware

### Overview

The current prototype code allows a user to communicate with the Texas Instruments [ADS131A04](http://www.ti.com/product/ADS131A04), a 4-channel, 24-bit, 128ksps, Delta-Sigma ADC. This IC allows for the collection of biopotential signals with amplitudes comparable to that of invasive electroencephalograms (EEGs), around 1-2mV. With up to 16x gain, the smallest convertable step size in the system (the ADC's LSB) is around 4.5uV (pretty good for a $7 chip)! 

### Workflow

The method for communicating with the ADS131A04 is basically as follows:

1. Power-on sequence, send RESET command to ADS131A04
1. Send UNLOCK command to allow writing of register settings (found in ADS131A04 datasheet, p. 55 onwards)
1. Write register settings using WREG command, check statuses of registers using RREG command
1. Send LOCK command to confirm register settings
1. WAKEUP the four ADCs and begin reading
1. Send NULL commands to obtain STAT_1 register data (general device status), followed by 4, 24-bit packets of returned ADC data

### Receiving Data

The current prototype board is a breakout for the ADS131A04 with an onboard 3.3V regulator and output reference driver. Data are transacted over SPI. The MISO/MOSI signals must be connected to a microncontroller that can handle at least 16.384MHz clock generation, as the internal oscillator on the ADS131A04 is set to use SCLK as the CLKSRC signal (which is further divided down through the ICLK_DIV bits in the CLK1 register).

Further design iterations will incorporate an onboard ARM processor with a single USB output for data acquisition into a UX. Timing for data acquisition is still in the development process.


#### Attributions

- ADS131A04 information by Texas Instruments
- Brain by Wes Breazell from the Noun Project
