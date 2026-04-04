# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

MCCI Arduino LoRaWAN Library -- a high-level C++ wrapper around the MCCI LMIC library for building LoRaWAN end-devices on Arduino-compatible platforms. Supports 9 network operators (TTN, Actility, Helium, ChirpStack, Generic, machineQ, Senet, Senra, Swisscom) across 8 LoRaWAN regions (EU868, US915, AU915, AS923, AS923-JP, KR920, IN866, CN470).

**Dependencies:** MCCI LoRaWAN LMIC library (v4.0.0+), MCCI Arduino Development Kit ADK.

## Building

This is an Arduino library -- there is no standalone build. It compiles via the Arduino IDE or PlatformIO as part of a sketch that includes it.

**CI (Travis):** Compile-tests all example sketches across SAMD21 (Catena 4450) and STM32L0 (Catena 4551/4610/etc.) boards for each supported region. No unit test framework; the `examples/header_test*` sketches serve as compilation regression tests.

To verify a build locally with Arduino CLI:
```
arduino-cli compile --fqbn mcci:samd:mcci_catena_4450:lorawan_region=us915 examples/header_test/header_test.ino
```

Region must be set via board option (`lorawan_region=us915|eu868|au915|as923|as923jp|kr920|in866`) or PlatformIO build flag (`-DCFG_us915`).

## Architecture

### Class Hierarchy

Two-level inheritance: **Network** (provisioning behavior) x **Region** (channel configuration).

```
Arduino_LoRaWAN                        (abstract base, src/Arduino_LoRaWAN.h)
  └── Arduino_LoRaWAN_<Network>_base   (e.g., _ttn_base, _Actility_base)
        └── Arduino_LoRaWAN_<Network>_<Region>  (e.g., _ttn_us915)
```

Each network+region combination has its own `_netbeginregioninit.cpp` and `_netjoin.cpp` in `src/lib/`. These are small files (~50 lines) that override virtual methods for region-specific LMIC initialization.

### Key Virtual Methods (User Overrides)

Users subclass a concrete network+region class and override:
- `GetProvisioningStyle()` / `GetOtaaProvisioningInfo()` / `GetAbpProvisioningInfo()` -- supply credentials
- `NetSaveSessionInfo()` / `NetSaveSessionState()` / `NetGetSessionState()` -- persistence (storage-agnostic)
- `NetRxComplete()` / `NetTxComplete()` -- application callbacks

### Event Dispatch

LMIC's C callback `onEvent(ev_t)` routes through a singleton (`Arduino_LoRaWAN::GetInstance()`) to `DispatchEvent()`, which calls `StandardEventProcessor()` and up to 4 registered listeners.

### Session State

`SessionState` (defined in Arduino_LoRaWAN.h) captures the full LMIC state including frame counters, channel masks (EU-like 16ch, US-like 72ch, CN-like 96ch), data rates, and band config. V2 format (v0.9+) separates join response from mutable state. Serialization uses tagged headers for forward compatibility.

## Code Conventions

- **Copyright:** `MCCI Corporation` with current year, MIT license, reference to LICENSE file.
- **File headers:** Module name, Function description, Copyright notice, Author block.
- **Include guards:** `#ifndef _ARDUINO_LORAWAN_H_` style (some files also have `#pragma once`).
- **Naming:** Classes use `Arduino_LoRaWAN_<Network>_<Region>`. Member variables use `m_` prefix. Enum values use `k` prefix (e.g., `kOTAA`, `kNetworkID`). Constants/macros use `UPPERCASE`.
- **Section markers:** `/**** Manifest constants & typedefs ****/`, `/**** Read-only data ****/`, etc.
- **C compatibility:** `MCCIADK_BEGIN_DECLS` / `MCCIADK_END_DECLS` wrappers where needed.
- **Source file naming in src/lib/:** `<network>_<region>_<function>.cpp` pattern, e.g., `ttn_us915_netbeginregioninit.cpp`.

## Version Scheme

Semantic versioning encoded as a 32-bit integer via `ARDUINO_LORAWAN_VERSION_CALC(major, minor, patch, local)`. Local version 0 is greater than any non-zero local (release > pre-release). Use the `_COMPARE_LT` / `_COMPARE_GT` macros for correct comparison.
