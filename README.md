# arduino-lorawan

> **User-friendly library for using the Feather M0 LoRa with The Things Network and LoRaWAN™**

**Contents**
<!-- TOC depthFrom:2 -->

- [Overview](#overview)
- [Required libraries](#required-libraries)
- [Release History](#release-history)
- [Notes](#notes)

<!-- /TOC -->

## Overview

The **arduino-lorawan** library provides a structured way of using the [arduino-lmic library][0] to send sensor data over The Things Network or a simlar LoRaWAN-based data network.

It targets devices that are ARM-based, consisting of:

1. An Atmel SAMD21G or ST Micro STM32L0 processor;
2. An SX1276-based LoRa radio; and
3. An Arduino run-time environment.

The reference target for SAMD21G deployments is [Adafruit Feather M0 LoRa][1].
In addition to the basic Feather M0 LoRa, other products are supported. The [MCCI][3] [Catena 4450][4] and [Catena 4460][5] products are upward compatible with the Feather M0 LoRa and therefore also can be used with this library.

The reference target for STM32L0 deployments is the Murata CMWX1ZZABZ-078, as deployed in the MCCI [Catena 4551][6], with the MCCI Arduino [board support package][7].

[0]: https://github.com/terrillmoore/arduino-lmic
[1]: https://www.adafruit.com/products/3178
[2]: https://thethings.nyc/
[3]: http://www.mcci.com
[4]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4450-lorawan-iot-device
[5]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4460-sensor-wing-w-bme680
[6]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4551-integrated-lorawan-node
[7]: https://github.com/mcci-catena/arduino-boards

**arduino-lorawan** attempts to solve three problems.

1. It separates network maintenance code from your application.
2. It separates the common logic of your sensor app from the details about each individual device, allowing you to have a common source base that's used for all sensors.
3. It provides a simple framework for doing low-power programming.
4. It includes a framework for managing non-volatile storage (particularly FRAM) in a stable and atomic way.

The resulting programming environment is just a little more complicated than basic Arduino, but we intend that it will be almost as easy to use for prototyping, and not too tedious to use when moving to small pilot runs.

## Required libraries

| Library | Version | Comments |
|---------|:-------:|----------|
| [arduino-lmic](https://github.com/mcci-catena/arduino-lmic) | 2.1.0 | Earlier versions will fail to compile due to missing `lmic_pinmap::rxtx_rx_polarity` and `lmic_pinmap::spi_freq` fields. |
| [catena-mcci](https://github.com/mcci-catena/Catena-mcciadk) | 0.1.1 | Needed for miscellaneous definitions |


## Release History

- V0.3.2 is just documentation changes.

- V0.3.1 adds documention (in this file, in the [Required Libraries](#required-libraries) section) describing the need for the [catena-mcciadk](https://github.com/mcci-catena/Catena-mcciadk) library. No code changes.

- V0.3.0 adds support for the Murata module. It requires V2.1.0 of the arduino-lmic library.

- V0.2.5 added support for the extended bandplans, and requires V2.0.2 of the arduino-lmic library.


## Notes

>  * MCCI and MCCI Catena are registered trademarks of MCCI Corporation. LoRaWAN is a trademark of the LoRa Alliance. All other trademarks are the properties of their respective owners.
>  * Terry Moore of MCCI was the principal author of **arduino-lorawan**.
>  * Many thanks to Bob Fendrick for assistance in preparing initial test units.
>  * **MCCI** and **Catena** are registered trademarks of MCCI Corporation.
>  * This document composed with [StackEdit](https://stackedit.io/).

