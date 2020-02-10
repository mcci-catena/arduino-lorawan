/*

Module:  chirpstack_au915_netbeginregioninit.cpp

Function:
    Arduino_LoRaWAN_ChirpStack_au915::NetBeginRegionInit()

Copyright notice:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	February 2020

*/

#include <Arduino_LoRaWAN_ChirpStack.h>
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
void Arduino_LoRaWAN_ChirpStack_au915::NetBeginRegionInit()
    {
    // Select SubBand prejoin -- saves power for joining
    // This is specific to the AU915 bandplan.
#if defined(ARDUINO_LMIC_CFG_SUBBAND) && ARDUINO_LMIC_CFG_SUBBAND != -1
# if 0 <= ARDUINO_LMIC_CFG_SUBBAND && ARDUINO_LMIC_CFG_SUBBAND < 8
    LMIC_selectSubBand(ARDUINO_LMIC_CFG_SUBBAND);
# elif ARDUINO_LMIC_CFG_NETWORK_CHIRPSTACK && defined(CFG_au915)
#   error "Subband out of range for AU915"
# endif
#endif // defined(ARDUINO_LMIC_CFG_SUBBAND) && ARDUINO_LMIC_CFG_SUBBAND != -1
    }
