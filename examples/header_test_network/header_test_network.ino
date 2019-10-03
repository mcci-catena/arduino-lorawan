/*

Module:  header_test_ttn.ino

Function:
        Simple compile-test app for Arduino_LoRaWAN_ttn.h

Copyright notice and License:
        See LICENSE file accompanying this project.

Author:
        Terry Moore, MCCI Corporation	April 2018

*/

#include <Arduino_LoRaWAN_network.h>

// set up the data structures.
Arduino_LoRaWAN_network myLoRaWAN {};

void setup() {
    myLoRaWAN.begin();
}

void loop() {
    myLoRaWAN.loop();
}
