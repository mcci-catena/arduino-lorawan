/*

Module:  Arduino_LoRaWAN_Helium.h

Function:
    LoRaWAN network object for Helium.

Copyright notice:
    This file copyright (C) 2020. See accompanying
    LICENSE file for license information.

Author:
    Terry Moore, MCCI Corporation	February 2020

*/

#ifndef _ARDUINO_LORAWAN_HELIUM_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_HELIUM_H_

#pragma once

#ifndef _ARDUNIO_LORAWAN_H_
# include <Arduino_LoRaWAN.h>
#endif

#ifndef _MCCIADK_ENV_H_
# include <mcciadk_env.h>
#endif

#if !defined(PLATFORMIO)
/* this is a bit of a hack, but... */
/* make sure we have the config variables in scope */
/* Under PlatformIO use build_flags = -DCFG_ etc. in platformio.ini */
# include <arduino_lmic_user_configuration.h>
#endif

class Arduino_LoRaWAN_Helium_base :  public Arduino_LoRaWAN
    {
public:
    Arduino_LoRaWAN_Helium_base() {};
    using Super = Arduino_LoRaWAN;
    static constexpr NetworkID_t NetworkID = NetworkID_t::Helium;

    virtual const char *GetNetworkName() const override
        {
        return NetworkID_t_GetName(NetworkID);
        };

    virtual NetworkID_t GetNetworkID() const override
        {
        return NetworkID;
        }

protected:
    // Handle common NetJoin() operations for the network.
    virtual void NetJoin();

private:
    };

class Arduino_LoRaWAN_Helium_us915 : public Arduino_LoRaWAN_Helium_base
    {
public:
    using Super = Arduino_LoRaWAN_Helium_base;
    Arduino_LoRaWAN_Helium_us915() {};

protected:
    // the NetBeginRegionInit() function allows us to override the LMIC
    // defaults when initalizing for a region. Only provide code if LMIC
    // is doing something wrong.
    virtual void NetBeginRegionInit();

    // Implement the NetJoin() operations for US915
    virtual void NetJoin();

private:
    };


#if ARDUINO_LMIC_CFG_NETWORK_HELIUM && defined(CFG_us915)
# if defined(ARDUINO_LMIC_CFG_SUBBAND) && ! (ARDUINO_LMIC_CFG_SUBBAND == -1 || ARRDUINO_LMIC_CFG_SUBBAND == 6)
#   error "Helium network in US915 region is fixed at subband channels 48~55/70"
# endif
# define Arduino_LoRaWAN_REGION_TAG us915
#elif ARDUINO_LMIC_CFG_NETWORK_HELIUM
# error "Configured region not supported for Helium: can't define Arduino_LoRaWAN_REGION_TAG"
#else
// just be silent if we don't think we're targeting Helium
#endif

#define Arduino_LoRaWAN_Helium_LOCAL_(Region)		\
    Arduino_LoRaWAN_Helium_ ## Region

#define Arduino_LoRaWAN_Helium_LOCAL(Region)		\
    Arduino_LoRaWAN_Helium_LOCAL_(Region)

//
// This header file might get compiled all the time ... and it's not
// an error to have a region other than US, unless we're actually targeting
// Helium. So we just don't define Arduino_LoRaWAN_Helium unless we
// have a valid region tag.
//
#if defined(Arduino_LoRaWAN_REGION_TAG)

class Arduino_LoRaWAN_Helium : public Arduino_LoRaWAN_Helium_LOCAL(Arduino_LoRaWAN_REGION_TAG)
    {
public:
    using Super = Arduino_LoRaWAN_Helium_LOCAL(Arduino_LoRaWAN_REGION_TAG);
    Arduino_LoRaWAN_Helium() {};

private:
    };

#endif // defined(Arduino_LoRaWAN_REGION_TAG)

/**** end of Arduino_LoRaWAN_Helium.h ****/
#endif /* _ARDUINO_LORAWAN_HELIUM_H_ */
