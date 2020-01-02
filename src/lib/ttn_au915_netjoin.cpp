/*

Module:  ttn_au915_netjoin.cpp

Function:
	Arduino_LoRaWAN_ttn_au915::NetJoin()

Copyright notice:
        See LICENSE file accompanying this project.

Author:
	Terry Moore, MCCI Corporation	August 2017

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

#if defined(CFG_au915)
void Arduino_LoRaWAN_ttn_au915::NetJoin()
	{
	// do the common work.
	this->Super::NetJoin();
	}
#endif // defined(CFG_au915)
