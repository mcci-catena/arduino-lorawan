/* Arduino_LoRaWAN_machineQ.h	Fri May 19 2017 23:58:34 tmm */

/*

Module:  Arduino_LoRaWAN_machineQ.h

Function:
	LoRaWAN-variants for The Things Network.

Version:
	V0.2.3	Fri May 19 2017 23:58:34 tmm	Edit level 2

Copyright notice:
	This file copyright (C) 2016-2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.

	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.

Author:
	Terry Moore, MCCI Corporation	October 2016

Revision history:
   0.2.0  Mon Oct 31 2016 15:44:49  tmm
	Module created.

   0.2.3  Fri May 19 2017 23:58:34  tmm
        Support eu868.

*/

#ifndef _ARDUINO_LORAWAN_MACHINEQ_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_TTN_H_

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

        virtual const char *GetNetworkName() const
                {
                return "machineQ";
                };

protected:
        // the NetBegin() function does specific work when starting
        // up; this does the common work for all machineQ
        // variants -- but there's only one; we're just trying to
        // save typing later.
        virtual bool NetBegin();

	// The netjoin function does any post-join work -- at present
	// this can be shared by all networks and is empty for machineQ.
        // To make this like other subclasses, we use an exernal function
        // body.
	virtual void NetJoin();

        // every derivative must have a NetBeginRegionInit.
        virtual void NetBeginRegionInit() = 0;

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


#if defined(CFG_us915)
# define Arduino_LoRaWAN_REGION_TAG us915
#elif defined(ARDUINO_LORAWAN_NETWORK_MACHINEQ)
# warning "Configured region not supported for machineQ: can't define Arduino_LoRaWAN_REGION_TAG"
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
#endif /* _ARDUINO_LORAWAN_TTN_H_ */
