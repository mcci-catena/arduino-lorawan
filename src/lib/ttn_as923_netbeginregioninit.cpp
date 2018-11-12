/* ttn_as923_netbegin.cpp	Mon Aug 07 2017 00:41:05 tmm */

/*

Module:  ttn_as923_netbegin.cpp

Function:
	Arduino_LoRaWAN_ttn_as923::NetBegin()

Version:
	V0.2.4	Mon Aug 07 2017 00:41:051 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2016-2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.
 
Author:
	Terry Moore, MCCI Corporation	November 2016

Revision history:
   0.2.4  Mon Aug 07 2017 00:41:05  tmm
	Module created.

*/

#include <Arduino_LoRaWAN_ttn.h>
#include <Arduino_LoRaWAN_lmic.h>

/****************************************************************************\
|
|		Manifest constants & typedefs.
|
\****************************************************************************/



/****************************************************************************\
|
|	Read-only data.
|
\****************************************************************************/



/****************************************************************************\
|
|	VARIABLES:
|
\****************************************************************************/

// protected virtual
void Arduino_LoRaWAN_ttn_as923::NetBeginRegionInit()
    {
    //
    // for as923, we don't need to do any special setup. 
    //
    }
