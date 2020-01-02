/*

Module:  ttn_au915_netbeginregioninit.cpp

Function:
	Arduino_LoRaWAN_ttn_au915::NetBeginRegionInit()

Copyright notice:
        See LICENSE file accompanying this project.

Author:
	Terry Moore, MCCI Corporation	August 2017

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

// protected virtual
#ifdef CFG_au915
void Arduino_LoRaWAN_ttn_au915::NetBeginRegionInit()
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
    LMIC_setDrTxpow(AU915_DR_SF7, 30);

    // Select SubBand prejoin -- saves power for joining
    // This is specific to the AU923 bandplan.
    cLMIC::SelectSubBand(
        cLMIC::SubBand::SubBand_2 // must align with subband on gateway.
        );
    }
#endif
