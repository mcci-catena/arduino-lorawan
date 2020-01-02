/* machineq_us915_netbeginregioninit.cpp	Fri May 19 2017 23:58:34 tmm */

/*

Module:  machineq_us915_netbegin.cpp

Function:
	Arduino_LoRaWAN_machineQ_us915::NetBeginRegionInit()

Copyright notice:
        See LICENSE file accompanying this project.

Author:
	Terry Moore, MCCI Corporation	June 2018

*/

#include <Arduino_LoRaWAN_machineQ.h>
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
#ifdef CFG_us915
void Arduino_LoRaWAN_machineQ_us915::NetBeginRegionInit()
        {
        //
        // machineQ is a 64-channel network. So we don't select
        // a subband.
        //
        // Set data rate and transmit power -- these ought to
        // be the defaults in lmic, but it's quicker to just
        // set them here.
        LMIC_setDrTxpow(US915_DR_SF10, 21);
        }
#endif
