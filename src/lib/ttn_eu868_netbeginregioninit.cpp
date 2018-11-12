/* ttn_eu868_netbegin.cpp	Sun Mar 12 2017 16:21:31 tmm */

/*

Module:  ttn_eu868_netbegin.cpp

Function:
	Arduino_LoRaWAN_ttn_eu868::NetBegin()

Version:
	V0.2.2	Sun Mar 12 2017 16:21:31 tmm	Edit level 2

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
   0.2.0  Tue Nov  1 2016 05:29:19  tmm
	Module created.

   0.2.2  Sun Mar 12 2017 16:21:31  tmm
	Clarify documentation.

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
void Arduino_LoRaWAN_ttn_eu868::NetBeginRegionInit()
    {
    //
    // for eu868, we don't need to do any special setup. 
    //
    }
