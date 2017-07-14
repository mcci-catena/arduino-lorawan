# arduino-lorawan
> **User-friendly library for using the Feather M0 LoRa with The Things Network and LoRaWAN**

The **arduino-lorawan** library provides a structured way of using the [arduino-lmic library][0] to send sensor data over The Things Network or a simlar LoRaWAN-based data network.

It targets devices that are like the [Adafruit Feather M0 LoRa][1], consisting of:

1. An Atmel SAMD21G processor;
2. An RFM95 LoRa radio connected to the SAMD21G per [The Things Network NY][2]'s conventions; and
3. An Arduino run-time environment.

In addition to the basic Feather M0 LoRa, other products match this profile. The [MCCI][3] [Catena 4450][4] products are upward compatible with the Feather M0 LoRa and therefore also can be used with this library.

[0]: https://github.com/terrillmoore/arduino-lmic
[1]: https://www.adafruit.com/products/3178
[2]: https://thethings.nyc/
[3]: http://www.mcci.com
[4]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4450-lorawan-iot-device

**arduino-lorawan** attempts to solve three problems.

1. It separates network maintenance code from your application.
2. It separates the common logic of your sensor app from the details about each individual device, allowing you to have a common source base that's used for all sensors.
3. It provides a simple framework for doing low-power programming.
4. It includes a framework for managing non-volatile storage (particularly FRAM) in a stable and atomic way.

The resulting programming environment is just a little more complicated than basic Arduino, but we intend that it will be almost as easy to use for prototyping, and not too tedious to use when moving to small pilot runs.

> **Notes**
>  * Terry Moore of MCCI was the principal author of **adafruit-lorawan**.
>  * Many thanks to Bob Fendrick for assistance in preparing initial test units.
>  * **MCCI** and **Catena** are registered trademarks of MCCI Corporation.
>  * This document composed with [StackEdit](https://stackedit.io/).

