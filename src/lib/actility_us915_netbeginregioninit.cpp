/*

Module:  actility_us915_netbeginregioninit.cpp

Function:
    Arduino_LoRaWAN_Actility_us915::NetBeginRegionInit()

Copyright notice:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	February 2020

*/

#include <Arduino_LoRaWAN_Actility.h>
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
void Arduino_LoRaWAN_Actility_us915::NetBeginRegionInit()
    {
    // Select SubBand prejoin -- saves power for joining
    // This is specific to the US915 bandplan.
# if defined(ARDUINO_LMIC_CFG_SUBBAND) && ARDUINO_LMIC_CFG_SUBBAND != -1
    LMIC_selectSubBand(ARDUINO_LMIC_CFG_SUBBAND);
# endif // defined(ARDUINO_LMIC_CFG_SUBBAND) && ARDUINO_LMIC_CFG_SUBBAND != -1
    }
