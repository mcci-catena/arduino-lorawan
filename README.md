# MCCI Arduino LoRaWAN Library

**User-friendly library for using the Arduino LMIC library with The Things Network and LoRaWAN&reg; networks.**

[![GitHub release](https://img.shields.io/github/release/mcci-catena/arduino-lorawan.svg)](https://github.com/mcci-catena/arduino-lorawan/releases/latest) [![GitHub commits](https://img.shields.io/github/commits-since/mcci-catena/arduino-lorawan/latest.svg)](https://github.com/mcci-catena/arduino-lorawan/compare/v0.8.0...master) [![Build Status](https://travis-ci.com/mcci-catena/arduino-lorawan.svg?branch=master)](https://travis-ci.com/mcci-catena/arduino-lorawan)

**Contents:**

<!-- markdownlint-capture -->
<!-- markdownlint-disable -->
<!-- TOC depthFrom:2 updateOnSave:true -->

- [Overview](#overview)
- [Required libraries](#required-libraries)
- [Compile-time Configuration](#compile-time-configuration)
        - [Region Selection](#region-selection)
        - [Network selection](#network-selection)
        - [Join Subband Selection](#join-subband-selection)
- [Writing Code With This Library](#writing-code-with-this-library)
        - [Using the LMIC's pre-configured pin-maps](#using-the-lmics-pre-configured-pin-maps)
        - [Supplying a pin-map](#supplying-a-pin-map)
        - [Details on use](#details-on-use)
- [APIs](#apis)
        - [Starting operation](#starting-operation)
        - [Poll and update the LMIC](#poll-and-update-the-lmic)
        - [Reset the LMIC](#reset-the-lmic)
        - [Shut down the LMIC](#shut-down-the-lmic)
        - [Register an event listener](#register-an-event-listener)
        - [Send an event to all listeners](#send-an-event-to-all-listeners)
        - [Manipulate the Debug Mask](#manipulate-the-debug-mask)
        - [Output a formatted log message](#output-a-formatted-log-message)
        - [Get the configured LoRaWAN region, country code, and network name](#get-the-configured-lorawan-region-country-code-and-network-name)
        - [Set link-check mode](#set-link-check-mode)
        - [Send a buffer](#send-a-buffer)
        - [Register a Receive-Buffer Callback](#register-a-receive-buffer-callback)
        - [Get DevEUI, AppEUI, AppKey](#get-deveui-appeui-appkey)
        - [Test provisioning state](#test-provisioning-state)
- [Release History](#release-history)
- [Notes](#notes)

<!-- /TOC -->
<!-- markdownlint-restore -->
<!-- Due to a bug in Markdown TOC, the table is formatted incorrectly if tab indentation is set other than 4. Due to another bug, this comment must be *after* the TOC entry. -->

## Overview

The **`arduino-lorawan`** library provides a structured way of using the [`arduino-lmic` library][0] to send sensor data over The Things Network or a similar LoRaWAN-based data network.

This version targets v2.3.0 or later of the `arduino-lmic` library.

It targets devices that are reasonably capable, consisting of:

1. A 32-bit processor (ARM, XTENSA, etc.);
2. An SX1276-based LoRa radio; and
3. An Arduino run-time environment.

The reference target for SAMD21G deployments is [Adafruit Feather M0 LoRa][1].
In addition to the basic Feather M0 LoRa, other products are supported. The [MCCI][3] [Catena 4450][4], [Catena 4460][5], and [Catena 4470][6] products are upward compatible with the Feather M0 LoRa and therefore also can be used with this library.

The reference target for STM32L0 deployments is the Murata CMWX1ZZABZ-078, as deployed in the MCCI [Catena 4610][7], [Catena 4612][9], [Catena 4801][12], [Catena 4617][10], [Catena 4618][11], [Catena 4630][13] etc., with the MCCI Arduino [board support package][8]. Note that for proper TCXO control, you must use v2.3.0 or later of the `arduino-lmic` library.

[0]: https://github.com/mcci-catena/arduino-lmic
[1]: https://www.adafruit.com/products/3178
[2]: https://thethings.nyc/
[3]: https://mcci.com
[4]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4450-lorawan-iot-device
[5]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/catena-4460-sensor-wing-w-bme680
[6]: https://store.mcci.com/collections/lorawan-iot-and-the-things-network/products/mcci-catena-4470-modbus-node-for-lorawan-technology
[7]: https://store.mcci.com/products/mcci-catena-4610-integrated-node-for-lorawan-technology
[8]: https://github.com/mcci-catena/arduino-boards
[9]: https://store.mcci.com/products/catena-4612-integrated-lorawan-node
[10]: https://store.mcci.com/products/mcci-catena-4617
[11]: https://store.mcci.com/products/mcci-catena-4618
[12]: https://store.mcci.com/products/catena-4801
[13]: https://store.mcci.com/products/mcci-catena-4630

**`arduino-lorawan`** attempts to solve three problems.

1. It separates network maintenance code from your application.
2. It separates the common logic of your sensor app from the details about each individual device, allowing you to have a common source base that's used for all sensors.
3. It provides a simple framework for doing low-power programming.
4. It includes a framework for managing non-volatile storage (particularly FRAM) in a stable and atomic way. (However, you have to provide the non-volatile storage handling).

The resulting programming environment is just a little more complicated than basic Arduino, but we intend that it will be almost as easy to use for prototyping, and not too tedious to use when moving to small pilot runs.

MCCI tends to use the this library wrapped by the [Catena Arduino Platform](https://github.com/mcci-catena/Catena-Arduino-Platform) library, but it can be used stand-alone as described below.

## Required libraries

| Library | Version | Comments |
|---------|:-------:|----------|
| [`arduino-lmic`](https://github.com/mcci-catena/arduino-lmic) | 2.3.2.60 | Earlier versions will fail to compile but should give compile-time asserts. |
| [`Catena-mcciadk`](https://github.com/mcci-catena/Catena-mcciadk) | 0.1.1 | Needed for miscellaneous definitions |

## Compile-time Configuration

### Region Selection

This library uses the same configuration variables as the Arduino LMIC for selecting the target region.

### Network selection

The following compile-time defines select the network that will be used. Exactly one should be defined. The value shall be 1.

| Symbol                                | Network               | Regions Supported
|---------------------------------------|-----------------------|--------------
| `ARDUINO_LMIC_CFG_NETWORK_TTN`        | The Things Network    | EU868, US915 (subband 1), AU915 (subband 1), AS923, AS923 Japan, KR920, IN866
| `ARDUINO_LMIC_CFG_NETWORK_ACTILITY`   | Actility              | EU868, US915, AU915, AS923, AS923 Japan, KR920, IN866
| `ARDUINO_LMIC_CFG_NETWORK_HELIUM`     | Helium                | US915 (subband 1)
| `ARDUINO_LMIC_CFG_NETWORK_MACHINEQ`   | machineQ              | US915
| `ARDUINO_LMIC_CFG_NETWORK_SENET`      | Senet                 | US915
| `ARDUINO_LMIC_CFG_NETWORK_SENRA`      | Senra                 | IN866
| `ARDUINO_LMIC_CFG_NETWORK_SWISSCOM`   | Swisscom              | EU868
| `ARDUINO_LMIC_CFG_NETWORK_CHIRPSTACK` | ChirpStack.io         | EU868, US915, AU915, AS923, AS923 Japan, KR920, IN866
| `ARDUINO_LMIC_CFG_NETWORK_GENERIC`    | Generic               | EU868, US915, AU915, AS923, AS923 Japan, KR920, IN866

### Join Subband Selection

Three regional plans (US915, AU915 and CN470) have fixed channel frequencies, and many more channels than are supported by most gateways. In these regions, it's common to reduce the OTAA join channels to a subset of the available channels -- networks often configure gateways to support a maximum of 8 channels. The exact choice of 8 channels is called the subband.

The symbol `ARDUINO_LMIC_CFG_SUBBAND`, if defined, configures the subband to be used. If set to -1, no subband choice is made; all channels are used for joining. Otherwise, the value is multiplied by 8 and used as the base channel for joining. Channels `ARDUINO_LMIC_CFG_SUBBAND` through `ARDUINO_LMIC_CFG_SUBBAND + 7` are used for the join process.  In the US915 and AU915 regions `ARDUINO_LMIC_CFG_SUBBAND` can be any value between 0 and 7, inclusive.  In the CN470 region, `ARDUINO_LMIC_CFG_SUBBAND` can be any value between 0 and 11, inclusive.

Some networks (The Things Network, Helium) pre-define the subband to be used; in that case, `ARDUINO_LMIC_CFG_SUBBAND` is not used.

## Writing Code With This Library

The classes in this library are normally intended to be used inside a class that overrides one or more of the virtual methods.

### Using the LMIC's pre-configured pin-maps

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

### Supplying a pin-map

If the LMIC library doesn't have a pin-map for your board, and you don't want to add one to the library, you can supply your own.  Simply prepare a pin-map, and pass it to the `begin()` method.

A full example can be found in the `example/simple_feather/simple_feather.ino` sketch, but here are the relevant differences.

```c++
// The pin map. This form is convenient if the LMIC library
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

3. Determine whether you need a pin-map, or whether your `arduino-lmic` library already directly supports your board. If directly supported, you can call `myLoRaWAN.begin()` without any arguments, and the LMIC default pin-map for your board will be used. Otherwise, you can allocate a pin-map. If you name it `myPinmap`, you can call `myLoRaWAN.begin(myPinmap);`, as in the example.

4. Implement the required methods.

## APIs

### Starting operation

The `begin()` APIs are used to start the LMIC engine. There are three forms.  See ["Details on use,"](#details-on-use) above.

### Poll and update the LMIC

```c++
void Arduino_LoRaWAN::loop(void);
```

This method must be called periodically in order to keep the LMIC operating. For class-A devices, this need only be called while actively pushing an uplink, or while a task is pending in the LMIC's time-driven queue.

### Reset the LMIC

```c++
void Arduino_LoRaWAN::reset(void);
```

Cancel any pending operations and reinitialize all internal state.

### Shut down the LMIC

```c++
void Arduino_LoRaWAN::Shutdown(void);
```

Shut down the LMIC. Any attempt to transmit while shut down will fail.

### Register an event listener

```c++
typedef void ARDUINO_LORAWAN_EVENT_FN(
                        void *pUserData,
                        uint32_t eventCode
                        );

bool Arduino_LoRaWAN::RegisterListener(
    ARDUINO_LORAWAN_EVENT_FN *pEvent,
    void *pUserData
    );
```

Clients may register event functions using `RegisterListener`. The event function is called on each event from the LMIC. Up to four listeners may be registered. There's no way to cancel a registration.

### Send an event to all listeners

```c++
void Arduino_LoRaWAN::DispatchEvent(uint32_t eventCode);
```

This routine causes each registered event listener to be called with the specified event code. This is mostly for internal use, and may become `protected` in future releases.

### Manipulate the Debug Mask

_To be documented. This feature is currently only in the header files and not used._

### Output a formatted log message

_To be documented. This feature is currently only used by the macro `ARDUINO_LORAWAN_PRINTF`, which in turn is only used in one place._

### Get the configured LoRaWAN region, country code, and network name

```c++
const char *Arduino_LoRaWAN::GetRegionString(char *pBuf, size_t size) const;
```

Set the buffer at `*pBuf` to the configured network region.  At most `size-1` characters will be copied to the target buffer.

The result is guaranteed to be non-NULL, and is a pointer to a string. If `pBuf` is `nullptr` or `size` is zero, then the result is a constant string `"<<null>>"`. Otherwise, the result is `pBuf`. Since the result might be an immutable string, the result is declared as `const char *`.  The result is guaranteed to be a well-formed string. If the buffer is too small to contain the region string, the region string will be truncated to the right as needed.

```c++
Arduino_LoRaWAN::Region Arduino_LoRaWAN::GetRegion() const;
```

Return the region code. `Arduino_LoRaWAN::Region` contains the following values: `unknown`, `eu868`, `us915`, `cn783`, `eu433`, `au915`, `cn490`, `as923`, `kr920`, and `in866`.

```c++
Arduino_LoRaWAN::CountryCode Arduino_LoRaWAN::GetCountryCode() const;
```

Return the country code, which might be relevant to the region definition. The defined values are `none` (in case there are no relevant country-specific variations), and `JP` (which means we must follow Japan listen-before-talk rules).

```c++
enum class Arduino_LoRaWAN::NetworkID_t : std::uint32_t {
    TheThingsNetwork, Actility, Helium, machineQ, Senet, Senra, Swisscom,
    ChirpStack, Generic
};

Arduino_LoRaWAN::NetworkID_t Arduino_LoRaWAN::GetNetworkID() const;
```

Return the network ID, indicating the network for which the stack is currently configured.

```c++
const char *GetNetworkName() const;
```

Return the network name. Values are `"The Things Network"`, `"Actility"`, `"Helium"`, `"machineQ"`, `"Senet"`, `"Senra"`, `"Swisscom"`, `"ChirpStack"`, and `"Generic"`.

### Set link-check mode

```c++
bool Arduino_LoRaWAN::SetLinkCheckMode(
    bool fEnable
    );
```

Enable (or disable) link check mode, based on the value of `fEnabled`. If disabled, the device will not try to maintain a connection to the network. If enabled, the device watches for downlinks. If no downlink is seen for 64 messages, the device starts setting the network ADR request in uplinks. If there's no response after 32 messages, the device reduces the data rate and increases power, and continues this loop until there are no more data rates to try. At that point, the device will start inserting join attempts after every uplink without a downlink.

If disabled, the device doesn't try to reduce data rate automatically and won't ever automatically detect network loss or change.

Disabled was formerly the preferred mode of operation, but as of early 2019, it is clear that enabled is the preferred mode for The Things Network.

### Send a buffer

```c++
typedef void Arduino_LoRaWAN::SendBufferCbFn(
    void *pClientData,
    bool fSuccess
    );

bool Arduino_LoRaWAN::SendBuffer(
    const uint8_t *pBuffer,
    size_t nBuffer,
    SendBufferCbFn *pDoneFn = nullptr,
    void *pClientData = nullptr,
    bool fConfirmed = false,
    uint8_t port = 1
    );
```

Send message from `pBuffer`; call `pDoneFn(pClientData, status)` when the message has either been transmitted or abandoned.

### Register a Receive-Buffer Callback

```c++
typedef void
Arduino_LoRaWAN::ReceivePortBufferCbFn(
    void *pClientData,
    uint8_t uPort,
    const uint8_t *pBuffer,
    size_t nBuffer
    );

void Arduino_LoRaWAN::SetReceiveBufferCb(
    Arduino_LoRaWAN::ReceivePortBufferCbFn *pReceiveBufferFn,
    void *pUserData = nullptr
    );
```

The specified function is called whenever a downlink message is received. `nBuffer` might be zero, and `uPort` might be zero for MAC messages.

### Get DevEUI, AppEUI, AppKey

```c++
bool Arduino_LoRaWAN::GetDevEUI(uint8_t *pBuf);
bool Arduino_LoRaWAN::GetAppEUI(uint8_t *pBuf);
bool Arduino_LoRaWAN::GetAppKey(uint8_t *pBuf);
```

These three routines fetch the provisioned DevEUI, AppEUI, and AppKey.  `pBuf` points to an 8-byte (DevEUI and AppEUI) or 16-byte (AppKey) buffer. The values are returned in network byte order: DevEUI and AppEUI are returned in little-endian byte order, and AppKey is returned in big-endian byte order.

### Test provisioning state

```c++
bool Arduino_LoRaWAN::IsProvisioned(void);
```

Return `true` if the LoRaWAN stack seems to be properly provisioned (provided with a valid Device EUI, Application EUI and Application Key for OTAA; or provided with valid Device Address, Application Session Key and Network Session Key for ABP). Returns `false` otherwise.

## Release History

- v0.8.0 has the following change.

  - [#139](https://github.com/mcci-catena/arduino-lorawan/issues/139) changes the subband for Helium, who move to channels 8-15/65 (subband 2) on [May 12, 2020](https://engineering.helium.com/2020/05/05/lorawan-network-upgrade-potential-disruption-mitigation.html).

- v0.7.0 has the following changes.

  - `library.properties` updated to refer to the required libraries for the Arduino 1.8.10 IDE.
  - [#136](https://github.com/mcci-catena/arduino-lorawan/issues/136) adds support for UI control of target network and subband, for the following networks: The Things Network, Actility, Helium, machineQ, Senet, Senra, Swisscom, ChirpStack, and Generic.
  - [#100](https://github.com/mcci-catena/arduino-lorawan/issues/100), [#121](https://github.com/mcci-catena/arduino-lorawan/issues/121) introduce `Arduino_LoRaWAN_machineQ`, a type that maps onto the selected target network and region.
  - [#120](https://github.com/mcci-catena/arduino-lorawan/issues/120) fixes the setting of RX2 DR9 in EU868 for TTN -- was incorrectly put in US when refactoring.
  - [#116](https://github.com/mcci-catena/arduino-lorawan/issues/110) adds KR920 support. Vestigial / unused uses of `KR921` were changed to match the official `KR920` name. Cleanup typos in this file. This requires `arduino-lmic` library version 2.3.2.60 or greater.

- v0.6.0 has the following changes.

  - [#110](https://github.com/mcci-catena/arduino-lorawan/issues/110) tweak initial power settings for US.
  - [#106](https://github.com/mcci-catena/arduino-lorawan/issues/106), [#107](https://github.com/mcci-catena/arduino-lorawan/issues/107), [#108](https://github.com/mcci-catena/arduino-lorawan/issues/108), [#104](https://github.com/mcci-catena/arduino-lorawan/issues/104) CI improvements
  - [#88](https://github.com/mcci-catena/arduino-lorawan/issues/88) use new LMIC APIs for SendBuffer
  - [#97](https://github.com/mcci-catena/arduino-lorawan/issues/97) add `ARDUINO_LORAWAN_VERSION` macro.
  - [#98](https://github.com/mcci-catena/arduino-lorawan/issues/98) check LMIC version at compile time.
  - [#96](https://github.com/mcci-catena/arduino-lorawan/issues/96) properly restores the NetID from a saved session.
  - [#93](https://github.com/mcci-catena/arduino-lorawan/issues/93) adds `EV_TXCANCELED` support.
  - [#92](https://github.com/mcci-catena/arduino-lorawan/issues/92), [#84](https://github.com/mcci-catena/arduino-lorawan/issues/84), [#85](https://github.com/mcci-catena/arduino-lorawan/issues/85), [#87](https://github.com/mcci-catena/arduino-lorawan/issues/87) handles transmit completion status correctly.
  - [#91](https://github.com/mcci-catena/arduino-lorawan/issues/91) removes a redundant call to `UpdateFCntDown()`.
  - [#89](https://github.com/mcci-catena/arduino-lorawan/issues/89) adds new LMIC event codes added as part of the certification push.
  - [#5](https://github.com/mcci-catena/arduino-lorawan/issues/5) enables link-check-mode by default.
  - [#83](https://github.com/mcci-catena/arduino-lorawan/issues/83) add `SetLinkCheckMode() method.
  - [#81](https://github.com/mcci-catena/arduino-lorawan/issues/81) allows uplinks to arbitrary ports.

- v0.5.3 is a patch release. It fixes a PlatformIO compile warning, and also fixes another missing return for `Arduino_LoRaWAN::begin()` (this time in an overload in the header file.)

- v0.5.2 incorporates the fix for issue [#68](https://github.com/mcci-catena/arduino-lorawan/issues/68), missing return in `Arduino_LoRaWAN::begin()`.

- v0.5.1 fixes compilation errors when the library manager installs `arduino-lmic` in a renamed directory (issue [#65](https://github.com/mcci-catena/arduino-lorawan/issues/65)).

- v0.5.0 has necessary changes to support the LMIC built-in pin-maps, while retaining support for user-supplied pin-maps. We moved the pin-map parameter from compile-time initialization to an argument to Arduino_LoRaWAN::begin().  This is, unfortunately, a breaking change. Either do as we did in the example -- move the pin-map to the `begin()` call -- or add an `m_pinmap` field in your concrete `cMyLoRaWAN`, and initialize it in your `cMyLoRaWAN::cMyLoRaWAN()` constructor. In addition, we added a few example programs (issue [#49](https://github.com/mcci-catena/arduino-lorawan/issues/49)), and fixed handling of downlink messages with port numbers but no payloads (issue [#50](https://github.com/mcci-catena/arduino-lorawan/issues/50)).

- v0.4.0 adds preliminary machineQ support, continuous integration for SAMD and STM32 L0, better PlatformIO support, improved as923jp support, and fixes a defect in the receive-message API.

- v0.3.4 adds a few simple compile tests, improves the library name in `library.properties`, and further improves documentation.

- v0.3.3 adds PlatformIO support and fixes `library.properties`.

- v0.3.2 is just documentation changes.

- v0.3.1 adds documentation (in this file, in the [Required Libraries](#required-libraries) section) describing the need for the [`catena-mcciadk`](https://github.com/mcci-catena/Catena-mcciadk) library. No code changes.

- v0.3.0 adds support for the Murata module. It requires V2.1.0 of the `arduino-lmic` library.

- v0.2.5 added support for the extended band plans, and requires V2.0.2 of the `arduino-lmic` library.

## Notes

> - Terry Moore of MCCI was the principal author of **`arduino-lorawan`**.
> - Many thanks to Bob Fendrick for assistance in preparing initial test units.
> - **MCCI** and **Catena** are registered trademarks of MCCI Corporation. **LoRaWAN** is a registered trademark of the LoRa Alliance. All other trademarks are the properties of their respective owners.
> - This document initially composed with [StackEdit](https://stackedit.io/); now maintained using Visual Studio Code.
