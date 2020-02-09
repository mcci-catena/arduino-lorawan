/*

Module:  helium_us915_netjoin.cpp

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

#if defined(CFG_us915)
void Arduino_LoRaWAN_Helium_us915::NetJoin()
    {
    // do the common work.
    this->Super::NetJoin();

    // Helium is bog standard, and our DNW2 value is already right.
    }
#endif // defined(CFG_us915)
