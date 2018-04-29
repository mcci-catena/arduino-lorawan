/* Arduino_LoRaWAN_lmic.h	Tue Oct 25 2016 06:22:40 tmm */

/*

Module:  Arduino_LoRaWAN_lmic.h

Function:
	Wrapper header file for LMIC internals.

Version:
	V0.1.0	Tue Oct 25 2016 06:22:40 tmm	Edit level 1

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
   0.1.0  Tue Oct 25 2016 06:22:40  tmm
	Module created.

*/

#ifndef _ARDUINO_LORAWAN_LMIC_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_LMIC_H_

#pragma once

#ifndef _ARDUINO_LORAWAN_H_
# include <Arduino_LoRaWAN.h>
#endif

#include <lmic.h>

class Arduino_LoRaWAN::cLMIC
	{
public:
	enum class SubBand
		{
		SubBand_1 = 0,
		SubBand_2,
		SubBand_3,
		SubBand_4,
		SubBand_5,
		SubBand_6,
		SubBand_7,
		SubBand_8
		};

	static inline void SelectSubBand(SubBand subBand)
		{
#if CFG_LMIC_US_like
		LMIC_selectSubBand(
			static_cast<u1_t>(subBand)
			);
#else
		/* nothing */
#endif
		}

	static const char *GetEventName(uint32_t ev);
	};

/* the usual macro-based table for the strings. should be in lmic.h */
#define	ARDUINO_LORAWAN_LMIC_EV_NAMES__BASE	EV_SCAN_TIMEOUT

#define	ARDUINO_LORAWAN_LMIC_EV_NAMES_MZ__INIT			\
	 "SCAN_TIMEOUT\0" "BEACON_FOUND\0" "BEACON_MISSED\0"	\
	 "BEACON_TRACKED\0" "JOINING\0" "JOINED\0" "RFU1\0"	\
	 "JOIN_FAILED\0" "REJOIN_FAILED\0" "TXCOMPLETE\0"	\
	 "LOST_TSYNC\0" "RESET\0" "RXCOMPLETE\0" "LINK_DEAD\0"	\
	 "LINK_ALIVE\0" "SCAN_FOUND\0" "TXSTART\0"

/**** end of Arduino_LoRaWAN_lmic.h ****/
#endif /* _ARDUINO_LORAWAN_LMIC_H_ */
