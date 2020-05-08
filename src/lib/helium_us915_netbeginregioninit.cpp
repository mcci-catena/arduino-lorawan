/*

Module:  helium_us915_netbegin.cpp

Function:
    Arduino_LoRaWAN_Helium_us915::NetBeginRegionInit()

Copyright notice:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	February 2020

*/

#include <Arduino_LoRaWAN_Helium.h>
#include <Arduino_LoRaWAN_lmic.h>

/****************************************************************************\
|
|   Manifest constants & typedefs.
|
\****************************************************************************/



/****************************************************************************\
|
|   Read-only data.
|
\****************************************************************************/



/****************************************************************************\
|
|   Variables.
|
\****************************************************************************/

// protected virtual
#ifdef CFG_us915
void Arduino_LoRaWAN_Helium_us915::NetBeginRegionInit()
    {
    //
    // Helium is an 8-channel network on subband 1 of [0..7]. Pre-join
    // we therefore want to limit to probing channels 8~15 / 65.
    //
    cLMIC::SelectSubBand(cLMIC::SubBand::SubBand_2);

    //
    // Set data rate and transmit power -- these ought to
    // be the defaults in lmic, but it's quicker to just
    // set them here. All this gets overridden post join, anyway.
    //
    LMIC_setDrTxpow(US915_DR_SF10, 21);
    }
#endif
