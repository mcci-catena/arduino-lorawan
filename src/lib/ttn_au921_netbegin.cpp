/* ttn_au921_netbegin.cpp	Mon Aug 07 2017 00:41:05 tmm */

/*

Module:  ttn_au921_netbegin.cpp

Function:
	Arduino_LoRaWAN_ttn_au921::NetBegin()

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
   0.2.4  Tue Nov  1 2016 05:29:19  tmm
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
#ifdef CFG_au921
void Arduino_LoRaWAN_ttn_au921::NetBeginRegionInit()
    {
    // Set data rate and transmit power
    // DR_SF7 is US DR3; 14 means 14 dBm

    // XXX (tmm@mcci.com) although LMIC.adrTxpow is set to 30, it's
    // never used inside the LMIC library. This is because LMIC's radio.c uses
    // LMIC.txpow, and in US-like, lmic.c::updatetx() sets LMIC.txpow to 30
    // for 125kHz channels, and  26 for 500kHz channels, ignoring
    // LMIC.adrTxpow.  Then radio.c limits to the value for 10 dBm, and
    // apparently doesn't even turn on the +20 dBm option if over 10 dBm.
    // Really, this step should be eliminated and the library should do the
    // right thing at join time.
    LMIC_setDrTxpow(AU921_DR_SF7, 30);

    // Select SubBand prejoin -- saves power for joining
    // This is specific to the AU923 bandplan.
    cLMIC::SelectSubBand(
        cLMIC::SubBand::SubBand_2 // must align with subband on gateway.
        );
    }
#endif
