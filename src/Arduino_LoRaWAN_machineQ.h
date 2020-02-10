/*

Module:  Arduino_LoRaWAN_machineQ.h

Function:
        LoRaWAN network object for machineQ.

Copyright notice:
        This file copyright (C) 2016-2017, 2020. See accompanying
        LICENSE file for license information.

Author:
        Terry Moore, MCCI Corporation	October 2016

*/

#ifndef _ARDUINO_LORAWAN_MACHINEQ_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_MACHINEQ_H_

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

class Arduino_LoRaWAN_machineQ_base :  public Arduino_LoRaWAN
        {
public:
        Arduino_LoRaWAN_machineQ_base() {};
        using Super = Arduino_LoRaWAN;
        static constexpr NetworkID_t NetworkID = NetworkID_t::machineQ;

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

class Arduino_LoRaWAN_machineQ_us915 : public Arduino_LoRaWAN_machineQ_base
        {
public:
        using Super = Arduino_LoRaWAN_machineQ_base;
        Arduino_LoRaWAN_machineQ_us915() {};

protected:
        // the NetBeginRegionInit() function allows us to override the LMIC
        // defaults when initalizing for a region. Only provide code if LMIC
        // is doing something wrong.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for US915
        virtual void NetJoin();

private:
        };


#if ARDUINO_LMIC_CFG_NETWORK_MACHINEQ && defined(CFG_us915)
# if defined(ARDUINO_LMIC_CFG_SUBBAND) && ! (ARDUINO_LMIC_CFG_SUBBAND == -1)
#  error "machineQ is a 64-channel network; selecting a subband is not supported"
# endif
# define Arduino_LoRaWAN_REGION_TAG us915
#elif ARDUINO_LMIC_CFG_NETWORK_MACHINEQ
# error "Configured region not supported for machineQ: can't define Arduino_LoRaWAN_REGION_TAG"
#else
// just be silent if we don't think we're targeting MachineQ
#endif

#define Arduino_LoRaWAN_machineQ_LOCAL_(Region)		\
        Arduino_LoRaWAN_machineQ_ ## Region

#define Arduino_LoRaWAN_machineQ_LOCAL(Region)		\
        Arduino_LoRaWAN_machineQ_LOCAL_(Region)

//
// This header file might get compiled all the time ... and it's not
// an error to have a region other than US, unless we're actually targeting
// machineQ. So we just don't define Arduino_LoRaWAN_machineQ unless we
// have a valid region tag.
//
#if defined(Arduino_LoRaWAN_REGION_TAG)

class Arduino_LoRaWAN_machineQ : public Arduino_LoRaWAN_machineQ_LOCAL(Arduino_LoRaWAN_REGION_TAG)
        {
public:
        using Super = Arduino_LoRaWAN_machineQ_LOCAL(Arduino_LoRaWAN_REGION_TAG);
        Arduino_LoRaWAN_machineQ() {};

private:
        };

#endif // defined(Arduino_LoRaWAN_REGION_TAG)

/**** end of Arduino_LoRaWAN_machineQ.h ****/
#endif /* _ARDUINO_LORAWAN_MACHINEQ_H_ */
