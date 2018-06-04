# MCCI Arduino LoRaWAN Library

**User-friendly library for using the Arduino LMIC library with The Things Network and LoRaWAN™ networks.**

[![GitHub release](https://img.shields.io/github/release/mcci-catena/arduino-lorawan.svg)](https://github.com/mcci-catena/arduino-lorawan/releases/latest) [![GitHub commits](https://img.shields.io/github/commits-since/mcci-catena/arduino-lorawan/latest.svg)](https://github.com/mcci-catena/arduino-lorawan/compare/v0.3.4...master) [![Build Status](https://travis-ci.org/mcci-catena/arduino-lorawan.svg?branch=master)](https://travis-ci.org/mcci-catena/arduino-lorawan)

**Contents**
<!-- TOC depthFrom:2 -->

- [Overview](#overview)
- [Required libraries](#required-libraries)
- [How To Use](#how-to-use)
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

MCCI tends to use the this library wrapped by the [Catena Arduino Platform](https://github.com/mcci-catena/Catena-Arduino-Platform) library, but it can be used stand-alone as described below.

## Required libraries

| Library | Version | Comments |
|---------|:-------:|----------|
| [arduino-lmic](https://github.com/mcci-catena/arduino-lmic) | 2.1.0 | Earlier versions will fail to compile due to missing `lmic_pinmap::rxtx_rx_polarity` and `lmic_pinmap::spi_freq` fields. |
| [Catena-mcciadk](https://github.com/mcci-catena/Catena-mcciadk) | 0.1.1 | Needed for miscellaneous definitions |

## How To Use

The classes in this library are normally intended to be used inside a class that overrides one or more of the virtual methods.

The stand-alone use pattern is as follows, targeting The Things Network V2.

```c++
#include <Arduino_LoRaWAN_ttn.h>

class cMyLoRaWAN : public Arduino_LoRaWAN_ttn {
    // ... see below for typical contents
publc:
    myLoRaWAN() {};
    myLoRaWAN(const lmic_pinmap& pinmap) Arduino_LoRaWAN_ttn(pinmap) {};
protected:
    // you'll need to provide implementations for each of the following.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    virtual void NetSaveFCntUp(uint32_t uFCntUp) override;
    virtual void NetSaveFCntDown(uint32_t uFCntDown) override;
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
};

// example pinmap - this is for Feather M0 LoRa
const lmic_pinmap myPinmap = {
     .nss = 8,
     .nss = 8,
     .rxtx = LMIC_UNUSED_PIN,
     .rst = 4,
     .dio = { 3, 6, LMIC_UNUSED_PIN },
     .rxtx_rx_active = 0,
     .spi_freq = 8000000,
};

// set up the data structures.
cMyLoRaWAN myLoRaWAN(myPinMap);

void setup() {
    myLoRaWAN.begin();
}

void loop() {
    myLoRaWAN.loop();
}
```

## Release History

- V0.3.4 adds a few simple compile tests, improves the library name in `library.properties`, and further improves documentation.

- V0.3.3 adds PlatformIO support and fixes `library.properties`.

- V0.3.2 is just documentation changes.

- V0.3.1 adds documentation (in this file, in the [Required Libraries](#required-libraries) section) describing the need for the [catena-mcciadk](https://github.com/mcci-catena/Catena-mcciadk) library. No code changes.

- V0.3.0 adds support for the Murata module. It requires V2.1.0 of the arduino-lmic library.

- V0.2.5 added support for the extended bandplans, and requires V2.0.2 of the arduino-lmic library.


## Notes

>  * Terry Moore of MCCI was the principal author of **arduino-lorawan**.
>  * Many thanks to Bob Fendrick for assistance in preparing initial test units.
>  * **MCCI** and **Catena** are registered trademarks of MCCI Corporation. **LoRaWAN** is a trademark of the LoRa Alliance. All other trademarks are the properties of their respective owners.
>  * This document initially composed with [StackEdit](https://stackedit.io/); now maintained using Visual Studio Code.
