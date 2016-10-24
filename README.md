# arduino-lorawan
> **User-friendly library for using the Feather M0 LoRa with The Things Network and LoRaWAN**

The **arduino-lorawan** library provides a structured way of using the [arduino-lmic library][0] to send sensor data over The Things Network or a simlar LoRaWAN-based data network.

It targets devices that are like the [Adafruit Feather M0 LoRa][1], consisting of:

1. An Atmel SAMD21G processor;
2. An RFM95 LoRa radio connected to the SAMD21G per [The Things Network NY][2]'s conventions; and
3. An Arduino run-time environment.

In addition to the basic Feather M0 LoRa, other products match this profile. The MCCI Catena 4410 and 4501 products are upward compatible with the Feather M0 LoRa and therefore also can be used with this library.

[0]: https://github.com/terrillmoore/arduino-lmic
[1]: https://www.adafruit.com/products/3178
[2]: https://thethings.nyc/

**arduino-lorawan** attempts to solve three problems.

1. Separating network maintenance code from your application.
2. Separating the common logic of your sensor app from the details about each individual device.
3. Providing a simple framework for doing low-power programming

This resulting environment is just a little more complicated than basic Arduino, but we intend that it will be almost as easy to use for prototyping, and not too tedious to use when moving to small pilot runs.


> Written with [StackEdit](https://stackedit.io/).
