/* Arduino_LoRaWAN_ttn.h	Fri May 19 2017 23:58:34 tmm */

/*

Module:  Arduino_LoRaWAN_ttn.h

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

#ifndef _ARDUINO_LORAWAN_TTN_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_TTN_H_

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

class Arduino_LoRaWAN_ttn_base :  public Arduino_LoRaWAN
	{
public:
	Arduino_LoRaWAN_ttn_base() {};
        using Super = Arduino_LoRaWAN;

        virtual const char *GetNetworkName() const
                {
                return "The Things Network";
                };

protected:
        // the NetBegin() function does specific work when starting
        // up; this does the common work for all TTN lorawan
        // variants
        virtual bool NetBegin();

	// the netjoin function does any post-join work -- at present
	// this can be shared by all networks.
	virtual void NetJoin();

        // every derivative must have a NetBeginInit.
        virtual void NetBeginRegionInit() = 0;

private:
	};

class Arduino_LoRaWAN_ttn_eu868 : public Arduino_LoRaWAN_ttn_base
	{
public:
        using Super = Arduino_LoRaWAN_ttn_base;
	Arduino_LoRaWAN_ttn_eu868() {};

protected:
        // the NetBeginInit() function does specific work for the common code
        // when starting up.
        virtual void NetBeginRegionInit();

        // Implement the NetJoin() operations for eu868
        virtual void NetJoin();

private:
        };

class Arduino_LoRaWAN_ttn_us915 : public Arduino_LoRaWAN_ttn_base
	{
public:
        using Super = Arduino_LoRaWAN_ttn_base;
        Arduino_LoRaWAN_ttn_us915() {};

protected:
	// the NetBeginInit() function does specific work when starting
	// up. For us915, we need to turn off the link check mode, and
	// select the subband.
        virtual void NetBeginRegionInit();

	// Implement the NetJoin() operations for US915
	virtual void NetJoin();

private:
	};

class Arduino_LoRaWAN_ttn_au921 : public Arduino_LoRaWAN_ttn_base
	{
public:
        using Super = Arduino_LoRaWAN_ttn_base;
        Arduino_LoRaWAN_ttn_au921() {};

protected:
	// the NetBeginInit() function does specific work when starting
	// up. For au921, we need to turn off the link check mode, and
	// select the subband.
        virtual void NetBeginRegionInit();

	// Implement the NetJoin() operations for US915
	virtual void NetJoin();

private:
	};


class Arduino_LoRaWAN_ttn_as923 : public Arduino_LoRaWAN_ttn_base
	{
public:
        using Super = Arduino_LoRaWAN_ttn_base;
        Arduino_LoRaWAN_ttn_as923() {};

protected:
	// the NetBeginInit() function does specific work when starting
	// up. 
        virtual void NetBeginRegionInit();

	// Implement the NetJoin() operations for as923
	virtual void NetJoin();

private:
	};

class Arduino_LoRaWAN_ttn_in866 : public Arduino_LoRaWAN_ttn_base
	{
public:
        using Super = Arduino_LoRaWAN_ttn_base;
        Arduino_LoRaWAN_ttn_in866() {};

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
#elif defined(CFG_au921)
# define Arduino_LoRaWAN_REGION_TAG au921
#elif defined(CFG_as923)
# define Arduino_LoRaWAN_REGION_TAG as923
#elif defined(CFG_in866)
# define Arduino_LoRaWAN_REGION_TAG in866
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
        using Super = Arduino_LoRaWAN_ttn_LOCAL(Arduino_LoRaWAN_REGION_TAG);
        Arduino_LoRaWAN_ttn() {};

private:
	};


/**** end of Arduino_LoRaWAN_ttn.h ****/
#endif /* _ARDUINO_LORAWAN_TTN_H_ */
