# MCCI Arduino LoRaWAN Library

**User-friendly library for using the Arduino LMIC library with The Things Network and LoRaWAN™ networks.**

[![GitHub release](https://img.shields.io/github/release/mcci-catena/arduino-lorawan.svg)](https://github.com/mcci-catena/arduino-lorawan/releases/latest) [![GitHub commits](https://img.shields.io/github/commits-since/mcci-catena/arduino-lorawan/latest.svg)](https://github.com/mcci-catena/arduino-lorawan/compare/v0.5.3...master) [![Build Status](https://travis-ci.org/mcci-catena/arduino-lorawan.svg?branch=master)](https://travis-ci.org/mcci-catena/arduino-lorawan)

**Contents**
<!-- TOC depthFrom:2 updateOnSave:true -->

- [Overview](#overview)
- [Required libraries](#required-libraries)
- [How To Use](#how-to-use)
	- [Using the LMIC's pre-configured pinmaps](#using-the-lmics-pre-configured-pinmaps)
	- [Supplying a pinmap](#supplying-a-pinmap)
	- [Details on use](#details-on-use)
- [Release History](#release-history)
- [Notes](#notes)

<!-- /TOC -->

## Overview

The **arduino-lorawan** library provides a structured way of using the [arduino-lmic library][0] to send sensor data over The Things Network or a simlar LoRaWAN-based data network.

This version targets v2.3.0 or later of the arduino-lmic library.

It targets devices that are reasonably capabile, consisting of:

1. A 32-bit processor (ARM, XTENSA, etc.);
2. An SX1276-based LoRa radio; and
3. An Arduino run-time environment.

The reference target for SAMD21G deployments is [Adafruit Feather M0 LoRa][1].
In addition to the basic Feather M0 LoRa, other products are supported. The [MCCI][3] [Catena 4450][4], [Catena 4460][5], and [Catena 4470][6] products are upward compatible with the Feather M0 LoRa and therefore also can be used with this library.

The reference target for STM32L0 deployments is the Murata CMWX1ZZABZ-078, as deployed in the MCCI [Catena 4551][7], etc., with the MCCI Arduino [board support package][7]. Note that for proper TCXO control, you must use v2.3.0 or later of the arduino-lmic library.

[0]: https://github.com/mcci-catena/arduino-lmic
[1]: https://www.adafruit.com/products/3178
[2]: https://thethings.nyc/
[3]: http://www.mcci.com
[4]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4450-lorawan-iot-device
[5]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4460-sensor-wing-w-bme680
[6]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/mcci-catena-4470-modbus-node-for-lorawan-technology
[7]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4551-integrated-lorawan-node
[8]: https://github.com/mcci-catena/arduino-boards

**arduino-lorawan** attempts to solve three problems.

1. It separates network maintenance code from your application.
2. It separates the common logic of your sensor app from the details about each individual device, allowing you to have a common source base that's used for all sensors.
3. It provides a simple framework for doing low-power programming.
4. It includes a framework for managing non-volatile storage (particularly FRAM) in a stable and atomic way. (However, you have to provide the non-volatile storage handling).

The resulting programming environment is just a little more complicated than basic Arduino, but we intend that it will be almost as easy to use for prototyping, and not too tedious to use when moving to small pilot runs.

MCCI tends to use the this library wrapped by the [Catena Arduino Platform](https://github.com/mcci-catena/Catena-Arduino-Platform) library, but it can be used stand-alone as described below.

## Required libraries

| Library | Version | Comments |
|---------|:-------:|----------|
| [arduino-lmic](https://github.com/mcci-catena/arduino-lmic) | 2.3.0 | Earlier versions will fail to compile due to missing `arduino_lmic_hal_boards.h` and `arduino_lmic_hal_configuration.h` |
| [Catena-mcciadk](https://github.com/mcci-catena/Catena-mcciadk) | 0.1.1 | Needed for miscellaneous definitions |

## How To Use

The classes in this library are normally intended to be used inside a class that overrides one or more of the virtual methods.

### Using the LMIC's pre-configured pinmaps

The stand-alone use pattern is as follows, targeting The Things Network V2.  This code can be found in the `example/simple_feather/simple.ino` sketch.  Note that this isn't complete, as you have to add code in the indicated places.

```c++
#include <Arduino_LoRaWAN_ttn.h>

class cMyLoRaWAN : public Arduino_LoRaWAN_ttn {
public:
    cMyLoRaWAN() {};

protected:
    // you'll need to provide implementations for each of the following.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    virtual void NetSaveFCntUp(uint32_t uFCntUp) override;
    virtual void NetSaveFCntDown(uint32_t uFCntDown) override;
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
};

// set up the data structures.
cMyLoRaWAN myLoRaWAN {};

void setup() {
    myLoRaWAN.begin();
}

void loop() {
    myLoRaWAN.loop();
}

// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool
cMyLoRaWAN::GetOtaaProvisioningInfo(
    OtaaProvisioningInfo *pInfo
    ) {
    return false;
}

void
cMyLoRaWAN::NetSaveFCntDown(uint32_t uFCntDown) {
    // save uFcntDown somwwhere
}

void
cMyLoRaWAN::NetSaveFCntUp(uint32_t uFCntUp) {
    // save uFCntUp somewhere
}

void
cMyLoRaWAN::NetSaveSessionInfo(
    const SessionInfo &Info,
    const uint8_t *pExtraInfo,
    size_t nExtraInfo
    ) {
    // save Info somewhere.
}
```

### Supplying a pinmap

If the LMIC library doesn't have a pinmap for your board, and you don't want to add one to the library, you can supply your own.  Simply prepare a pinmap, and pass it to the `begin()` method.

A full example can be found in the `example/simple_feather/simple_feather.ino` sketch, but here are the relevant differences.

```c++
// The pinmap. This form is convenient if the LMIC library
// doesn't support your board and you don't want to add the
// configuration to the library (perhaps you're just testing).
// This pinmap matches the FeatherM0 LoRa. See the arduino-lmic
// docs for more info on how to set this up.
const cMyLoRaWAN::lmic_pinmap myPinMap = {
     .nss = 8,
     .rxtx = cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN,
     .rst = 4,
     .dio = { 3, 6, cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN },
     .rxtx_rx_active = 0,
     .rssi_cal = 0,
     .spi_freq = 8000000,
};

void setup() {
    // simply pass the pinmap to the begin() method.
    myLoRaWAN.begin(myPinMap);
}
```

### Details on use

1. Define a class to define your concrete LoRaWAN instance. This is how you'll provide the out-calls, by defining virtual method functions. We'll call this `cMyLoRaWAN`, beginning with `c` to indicate that this is a class name.

2. Create an instance of your class. We'll call this `myLoRaWAN`.

3. Determine whether you need a pinmap, or whether your arduino-lmic library already directly supports your board. If directly supported, you can call `myLoRaWAN.begin()` without any arguments, and the LMIC default pinmap for your board will be used. Otherwise, you can allocate a pinmap. If you name it `myPinmap`, you can call `myLoRaWAN.begin(myPinmap);`, as in the example.

4. Implement the required methods.

## Release History

- v0.5.3 is a patch release. It fixes a platformio compile warning, and also fixes another missing return for `Arduino_LoRaWAN::begin()` (this time in an overload in the header file.)

- v0.5.2 incorporates the fix for issue [#68](https://github.com/mcci-catena/arduino-lorawan/issues/68), missing return in `Arduino_LoRaWAN::begin()`.

- v0.5.1 fixes compilation errors when the library manager installs arduino-lmic in a renamed directory (issue [#65](https://github.com/mcci-catena/arduino-lorawan/issues/65)).

- v0.5.0 has necessary changes to support the LMIC built-in pinmaps, while retaining support for user-supplied pinmaps. We moved the pinmap parameter from compile-time initialization to an argument to Arduino_LoRaWAN::begin().  This is, unfortunately, a breaking change. Either do as we did in the example -- move the pinmap to the `begin()` call -- or add an `m_pinmap` field in your concrete `cMyLoRaWAN`, and initialize it in your `cMyLoRaWAN::cMyLoRaWAN()` constructor. In addition, we added a few example programs (issue [#49](https://github.com/mcci-catena/arduino-lorawan/issues/49)), and fixed handling of downlink messages with port numbers but no payloads (issue [#50](https://github.com/mcci-catena/arduino-lorawan/issues/50)).

- v0.4.0 adds preliminary machineQ support, continuous integration for SAMD and STM32 L0, better PlatformIO support, improved as923jp support, and fixes a defect in the receive-message API.

- v0.3.4 adds a few simple compile tests, improves the library name in `library.properties`, and further improves documentation.

- v0.3.3 adds PlatformIO support and fixes `library.properties`.

- v0.3.2 is just documentation changes.

- v0.3.1 adds documentation (in this file, in the [Required Libraries](#required-libraries) section) describing the need for the [catena-mcciadk](https://github.com/mcci-catena/Catena-mcciadk) library. No code changes.

- v0.3.0 adds support for the Murata module. It requires V2.1.0 of the arduino-lmic library.

- v0.2.5 added support for the extended bandplans, and requires V2.0.2 of the arduino-lmic library.

## Notes

>  * Terry Moore of MCCI was the principal author of **arduino-lorawan**.
>  * Many thanks to Bob Fendrick for assistance in preparing initial test units.
>  * **MCCI** and **Catena** are registered trademarks of MCCI Corporation. **LoRaWAN** is a trademark of the LoRa Alliance. All other trademarks are the properties of their respective owners.
>  * This document initially composed with [StackEdit](https://stackedit.io/); now maintained using Visual Studio Code.
