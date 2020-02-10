/*

Module:  generic_au915_netjoin.cpp

Function:
    Arduino_LoRaWAN_Generic_au915::NetJoin()

Copyright notice:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	February 2020

*/

#include <Arduino_LoRaWAN_Generic.h>
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

void Arduino_LoRaWAN_Generic_au915::NetJoin()
    {
    // do the common work.
    this->Super::NetJoin();
    }
