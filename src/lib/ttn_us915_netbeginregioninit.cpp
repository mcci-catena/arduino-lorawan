/*

Module:  ttn_us915_netbeginregioninit.cpp

Function:
	Arduino_LoRaWAN_ttn_us915::NetBeginRegionInit()

Copyright notice:
        See LICENSE file accompanying this project.

Author:
	Terry Moore, MCCI Corporation	November 2016

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
#ifdef CFG_us915
void Arduino_LoRaWAN_ttn_us915::NetBeginRegionInit()
    {
    // Set data rate and transmit power
    // DR_SF7 is US DR3; 21 means 21 dBm

    LMIC_setDrTxpow(US915_DR_SF7, 21);

    // Select SubBand prejoin -- saves power for joining
    // This is specific to the US915 bandplan.
    cLMIC::SelectSubBand(
        cLMIC::SubBand::SubBand_2 // must align with subband on gateway.
        );
    }
#endif
