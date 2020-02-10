/*

Module:  senet_us915_netbegin.cpp

Function:
    Arduino_LoRaWAN_Senet_us915::NetBeginRegionInit()

Copyright notice:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	February 2020

*/

#include <Arduino_LoRaWAN_Senet.h>
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
void Arduino_LoRaWAN_Senet_us915::NetBeginRegionInit()
    {
    //
    // Senet suggests either subband 0 for joins or all 64 channels.
    //
#if defined(ARDUINO_LMIC_CFG_SUBBAND) && ! (ARDUINO_LMIC_CFG_SUBBAND == -1)
    LMIC_selectSubBand(ARDUINO_LMIC_CFG_SUBBAND);
#endif
    }
