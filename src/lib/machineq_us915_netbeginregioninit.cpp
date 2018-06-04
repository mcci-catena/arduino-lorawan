/* machineq_us915_netbeginregioninit.cpp	Fri May 19 2017 23:58:34 tmm */

/*

Module:  machineq_us915_netbegin.cpp

Function:
	Arduino_LoRaWAN_machineQ_us915::NetBeginRegionInit()

Version:
	V0.2.3	Fri May 19 2017 23:58:34 tmm	Edit level 3

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

   0.2.3  Fri May 19 2017 23:58:34  tmm
        Refactor for eu868 support as well as us915.

*/

#include <Arduino_LoRaWAN_machineQ.h>
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
#ifdef CFG_us915
void Arduino_LoRaWAN_machineQ_us915::NetBeginRegionInit()
        {
        //
        // machineQ is a 64-channel network. So we don't select
        // a subband.
        //
        // Set data rate and transmit power -- these ought to
        // be the defaults in lmic, but it's quicker to just
        // set them here.
        LMIC_setDrTxpow(US915_DR_SF10, 20);
        }
#endif
