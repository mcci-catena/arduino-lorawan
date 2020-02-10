/*

Module:  senet_us915_netjoin.cpp

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

void Arduino_LoRaWAN_Senet_us915::NetJoin()
    {
    // do the common work.
    this->Super::NetJoin();
    }
