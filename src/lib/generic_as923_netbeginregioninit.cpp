/*

Module:  generic_as923_netbeginregioninit.cpp

Function:
    Arduino_LoRaWAN_Generic_as923::NetBeginRegionInit()

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

// protected virtual
void Arduino_LoRaWAN_Generic_as923::NetBeginRegionInit()
    {
    //
    // for as923, we don't need to do any special setup.
    //
    }
