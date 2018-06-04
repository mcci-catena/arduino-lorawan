/*

Module:  header_test_lmic.ino

Function:
        Simple compile-test app for Arduino_LoRaWAN_ttn.h

Copyright notice and License:
        See LICENSE file accompanying this project.

Author:
        Terry Moore, MCCI Corporation	April 2018

*/

#include <Arduino_LoRaWAN_ttn.h>

// example pinmap - this is for Feather M0 LoRa
const Arduino_LoRaWAN::lmic_pinmap myPinmap = {
     .nss = 8,
     .rxtx = Arduino_LoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN,
     .rst = 4,
     .dio = { 3, 6, Arduino_LoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN },
     .rxtx_rx_active = 0,
     .rssi_cal = 0,
     .spi_freq = 8000000,
};

// set up the data structures.
Arduino_LoRaWAN_ttn myLoRaWAN(myPinmap);

void setup() {
    myLoRaWAN.begin();
}

void loop() {
    myLoRaWAN.loop();
}
