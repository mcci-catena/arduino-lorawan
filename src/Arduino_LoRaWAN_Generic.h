/*

Module:  Arduino_LoRaWAN_Generic.h

Function:
        LoRaWAN network object for Generic.

Copyright notice:
        See accompanying LICENSE file for copyright and license information.

Author:
        Terry Moore, MCCI Corporation	February 2020

*/

#ifndef _ARDUINO_LORAWAN_GENERIC_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_GENERIC_H_

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

class Arduino_LoRaWAN_Generic_base :  public Arduino_LoRaWAN
        {
public:
        Arduino_LoRaWAN_Generic_base() {};
        using Super = Arduino_LoRaWAN;
        static constexpr NetworkID_t NetworkID = NetworkID_t::Generic;

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

class Arduino_LoRaWAN_Generic_eu868 : public Arduino_LoRaWAN_Generic_base
        {
public:
        using Super = Arduino_LoRaWAN_Generic_base;
        Arduino_LoRaWAN_Generic_eu868() {};

protected:
        // the NetBeginInit() function does specific work for the common code
        // when starting up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for eu868
        virtual void NetJoin();

private:
        };

class Arduino_LoRaWAN_Generic_us915 : public Arduino_LoRaWAN_Generic_base
        {
public:
        using Super = Arduino_LoRaWAN_Generic_base;
        Arduino_LoRaWAN_Generic_us915() {};

protected:
        // the NetBeginInit() function does specific work when starting
        // up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for US915
        virtual void NetJoin();

private:
        };

class Arduino_LoRaWAN_Generic_au915 : public Arduino_LoRaWAN_Generic_base
        {
public:
        using Super = Arduino_LoRaWAN_Generic_base;
        Arduino_LoRaWAN_Generic_au915() {};

protected:
        // the NetBeginInit() function does specific work when starting
        // up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for US915
        virtual void NetJoin();

private:
        };


class Arduino_LoRaWAN_Generic_as923 : public Arduino_LoRaWAN_Generic_base
        {
public:
        using Super = Arduino_LoRaWAN_Generic_base;
        Arduino_LoRaWAN_Generic_as923() {};

protected:
        // the NetBeginInit() function does specific work when starting
        // up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for as923
        virtual void NetJoin();

private:
        };

class Arduino_LoRaWAN_Generic_kr920 : public Arduino_LoRaWAN_Generic_base
        {
public:
        using Super = Arduino_LoRaWAN_Generic_base;
        Arduino_LoRaWAN_Generic_kr920() {};

protected:
        // the NetBeginInit() function does specific work when starting
        // up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for as923
        virtual void NetJoin();

private:
        };

class Arduino_LoRaWAN_Generic_in866 : public Arduino_LoRaWAN_Generic_base
        {
public:
        using Super = Arduino_LoRaWAN_Generic_base;
        Arduino_LoRaWAN_Generic_in866() {};

protected:
        // the NetBeginInit() function does specific work when starting
        // up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for as923
        virtual void NetJoin();

private:
        };


#if defined(CFG_eu868)
# define Arduino_LoRaWAN_REGION_TAG eu868
#elif defined(CFG_us915)
# define Arduino_LoRaWAN_REGION_TAG us915
#elif defined(CFG_au915)
# define Arduino_LoRaWAN_REGION_TAG au915
#elif defined(CFG_as923)
# define Arduino_LoRaWAN_REGION_TAG as923
#elif defined(CFG_in866)
# define Arduino_LoRaWAN_REGION_TAG in866
#elif defined(CFG_kr920)
# define Arduino_LoRaWAN_REGION_TAG kr920
#else
# error "Can't define Arduino_LoRaWAN_REGION_TAG"
#endif

#define Arduino_LoRaWAN_Generic_LOCAL_(Region)		\
        Arduino_LoRaWAN_Generic_ ## Region

#define Arduino_LoRaWAN_Generic_LOCAL(Region)		\
        Arduino_LoRaWAN_Generic_LOCAL_(Region)

#if defined(Arduino_LoRaWAN_REGION_TAG)

class Arduino_LoRaWAN_Generic : public Arduino_LoRaWAN_Generic_LOCAL(Arduino_LoRaWAN_REGION_TAG)
        {
public:
        using Super = Arduino_LoRaWAN_Generic_LOCAL(Arduino_LoRaWAN_REGION_TAG);
        Arduino_LoRaWAN_Generic() {};

private:
        };

#endif // defined(Arduino_LoRaWAN_REGION_TAG)

/**** end of Arduino_LoRaWAN_Generic.h ****/
#endif /* _ARDUINO_LORAWAN_GENERIC_H_ */
