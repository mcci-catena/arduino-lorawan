/* ttn_us915_netbegin.cpp	Fri May 19 2017 23:58:34 tmm */

/*

Module:  ttn_us915_netbegin.cpp

Function:
	Arduino_LoRaWAN_ttn_us915::NetBegin()

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
#ifdef CFG_us915
void Arduino_LoRaWAN_ttn_us915::NetBeginRegionInit()
    {
    // Set data rate and transmit power
    // DR_SF7 is US DR3; 14 means 14 dBm

    // XXX (tmm@mcci.com) although LMIC.adrTxpow is set to 14, it's
    // never used inside the LMIC library. This is because LMIC's radio.c uses
    // LMIC.txpow, and in US915, lmic.c::updatetx() sets LMIC.txpow to 30
    // for 125kHz channels, and  26 for 500kHz channels, ignoring
    // LMIC.adrTxpow.  Then radio.c limits to the value for 10 dBm, and
    // apparendly doesn't even turn on the +20 dBm option if over 10 dBm.

    LMIC_setDrTxpow(US915_DR_SF7, 14);

    // Select SubBand prejoin -- saves power for joining
    // This is specific to the US915 bandplan.
    cLMIC::SelectSubBand(
        cLMIC::SubBand::SubBand_2 // must align with subband on gateway.
        );
    }
#endif
