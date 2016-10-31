/* Arduino_LoRaWAN_ttn.h	Mon Oct 31 2016 15:44:49 tmm */

/*

Module:  Arduino_LoRaWAN_ttn.h

Function:
	LoRaWAN-variants for The Things Network.

Version:
	V0.2.0	Mon Oct 31 2016 15:44:49 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2016 by

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

*/

#ifndef _ARDUINO_LORAWAN_TTN_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_TTN_H_

#ifndef _ARDUNIO_LORAWAN_H_
# include <Arduino_LoRaWAN.h>
#endif

#ifndef _MCCIADK_ENV_H_
# include <mcciadk_env.h>
#endif

/* this is a bit of a hack, but... */
/* make sure we have the config variables in scope */
#include "../../arduino-lmic/project_config/lmic_project_config.h"

class Arduino_LoRaWAN_ttn_base :  public Arduino_LoRaWAN
	{
public:
	Arduino_LoRaWAN_ttn_base() {};

private:
	};

class Arduino_LoRaWAN_ttn_eu868 : public Arduino_LoRaWAN_ttn_base
	{
public:
	Arduino_LoRaWAN_ttn_eu868() {};

private:
	};

class Arduino_LoRaWAN_ttn_us915 : public Arduino_LoRaWAN_ttn_base
	{
public:
	Arduino_LoRaWAN_ttn_us915() {};

private:
	};

class Arduino_LoRaWAN_ttn_as923 : public Arduino_LoRaWAN_ttn_base
	{
public:
	Arduino_LoRaWAN_ttn_as923() {};

private:
	};

#if CFG_eu868
# define Arduino_LoRaWAN_REGION_TAG eu868
#elif CFG_us915
# define Arduino_LoRaWAN_REGION_TAG us915
#elif defined(CFG_as923) && CFG_as923
# define Arduino_LoRaWAN_REGION_TAG as923
#else
# error "Can't define Arduino_LoRaWAN_REGION_TAG"
#endif

#define Arduino_LoRaWAN_ttn_LOCAL_(Region)		\
	Arduino_LoRaWAN_ttn_ ## Region

#define Arduino_LoRaWAN_ttn_LOCAL(Region)		\
	Arduino_LoRaWAN_ttn_LOCAL_(Region)

class Arduino_LoRaWAN_ttn : public Arduino_LoRaWAN_ttn_LOCAL(Arduino_LoRaWAN_REGION_TAG)
	{
public:
	Arduino_LoRaWAN_ttn() {};

private:
	};


/**** end of Arduino_LoRaWAN_ttn.h ****/
#endif /* _ARDUINO_LORAWAN_TTN_H_ */
