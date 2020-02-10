/*

Module:  Arduino_LoRaWAN_Senra.h

Function:
        LoRaWAN network object for Senra.

Copyright notice:
        See accompanying LICENSE file for copyright and license information.

Author:
        Terry Moore, MCCI Corporation	February 2020

*/

#ifndef _ARDUINO_LORAWAN_SENRA_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_SENRA_H_

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

class Arduino_LoRaWAN_Senra_base :  public Arduino_LoRaWAN
        {
public:
        Arduino_LoRaWAN_Senra_base() {};
        using Super = Arduino_LoRaWAN;
        static constexpr NetworkID_t NetworkID = NetworkID_t::Senra;

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

class Arduino_LoRaWAN_Senra_in866 : public Arduino_LoRaWAN_Senra_base
        {
public:
        using Super = Arduino_LoRaWAN_Senra_base;
        Arduino_LoRaWAN_Senra_in866() {};

protected:
        // the NetBeginInit() function does specific work when starting
        // up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for as923
        virtual void NetJoin();

private:
        };

#if defined(CFG_in866)
#  define Arduino_LoRaWAN_REGION_TAG in866
#elif ARDUINO_LMIC_CFG_NETWORK_SENRA
#  error "Can't define Arduino_LoRaWAN_REGION_TAG: region not supported for Senra"
#else
// just ignore.
#endif

#define Arduino_LoRaWAN_Senra_LOCAL_(Region)		\
        Arduino_LoRaWAN_Senra_ ## Region

#define Arduino_LoRaWAN_Senra_LOCAL(Region)		\
        Arduino_LoRaWAN_Senra_LOCAL_(Region)

//
// This header file might get compiled all the time ... and it's not
// an error to have a region other than IN, unless we're actually targeting
// Senra. So we just don't define Arduino_LoRaWAN_Senra unless we
// have a valid region tag.
//
#if defined(Arduino_LoRaWAN_REGION_TAG)

class Arduino_LoRaWAN_Senra : public Arduino_LoRaWAN_Senra_LOCAL(Arduino_LoRaWAN_REGION_TAG)
        {
public:
        using Super = Arduino_LoRaWAN_Senra_LOCAL(Arduino_LoRaWAN_REGION_TAG);
        Arduino_LoRaWAN_Senra() {};

private:
        };

#endif // defined(Arduino_LoRaWAN_REGION_TAG)

/**** end of Arduino_LoRaWAN_Senra.h ****/
#endif /* _ARDUINO_LORAWAN_SENRA_H_ */
