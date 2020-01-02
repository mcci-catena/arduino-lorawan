/* ttn_base_netjoin.cpp	Tue Nov  1 2016 06:19:25 tmm */

/*

Module:  ttn_base_netjoin.cpp

Function:
	Arduino_LoRaWAN_ttn_base::NetJoin()

Version:
	V0.2.0	Tue Nov  1 2016 06:19:25 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2016 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.
 
Author:
	Terry Moore, MCCI Corporation	November 2016

Revision history:
   0.2.0  Tue Nov  1 2016 06:19:25  tmm
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


void Arduino_LoRaWAN_ttn_base::NetJoin()
	{
	// we don't want to disable link-check mode on
	// current TTN networks with the current LMIC.
	// LMIC_setLinkCheckMode(0);
	}
