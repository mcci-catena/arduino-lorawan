/*

Module:  SetLinkCheckMode.cpp

Function:
	Arduino_LoRaWAN::SetLinkCheckMode()

Copyright notice:
	See accompanying LICENSE file.
 
Author:
	Terry Moore, MCCI Corporation	June 2018

*/

#include <Arduino_LoRaWAN.h>

#include <Arduino_LoRaWAN_lmic.h>

bool
Arduino_LoRaWAN::SetLinkCheckMode(bool fEnable)
        {
        // remember current state.
        const bool result = LMIC.adrAckReq != LINK_CHECK_OFF;

        // set nuew state.
        LMIC_setLinkCheckMode(fEnable);
        return result;
        }
