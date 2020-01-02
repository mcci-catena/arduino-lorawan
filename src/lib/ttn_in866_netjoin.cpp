/* ttn_in866_netjoin.cpp	Mon Aug 07 2017 00:41:05 tmm */

/*

Module:  ttn_in866_netjoin.cpp

Function:
	Arduino_LoRaWAN_ttn_in866::NetJoin()

Version:
	V0.2.4	Mon Aug 07 2017 00:41:05 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.

	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.

Author:
	Terry Moore, MCCI Corporation	August 2017

Revision history:
   0.2.4  Mon Aug 07 2017 00:41:05  tmm
	Module created.

*/

#include <Arduino_LoRaWAN_ttn.h>
#include <Arduino_LoRaWAN_lmic.h>

/****************************************************************************\
|
|	Manifest constants & typedefs.
|
\****************************************************************************/



/****************************************************************************\
|
|	Read-only data.
|
\****************************************************************************/



/****************************************************************************\
|
|	Variables.
|
\****************************************************************************/


void Arduino_LoRaWAN_ttn_in866::NetJoin()
	{
	// do the common work.
	this->Super::NetJoin();
	}
