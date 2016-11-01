/* ttn_us915_netbegin.cpp	Tue Nov  1 2016 05:29:19 tmm */

/*

Module:  ttn_us915_netbegin.cpp

Function:
	Arduino_LoRaWAN_ttn_us915::NetBegin()

Version:
	V0.2.0	Tue Nov  1 2016 05:29:19 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2016 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.
 
Author:
	Terry Moore, MCCI Corporation	November 2016

Revision history:
   0.2.0  Tue Nov  1 2016 05:29:19  tmm
	Module created.

*/

#include <Arduino_LoRaWAN_ttn.h>
#include <Arduino_LoRaWAN_lmic.h>

/****************************************************************************\
|
|		Manifest constants & typedefs.
|
\****************************************************************************/



/****************************************************************************\
|
|	Read-only data.
|
\****************************************************************************/



/****************************************************************************\
|
|	VARIABLES:
|
\****************************************************************************/


bool Arduino_LoRaWAN_ttn_us915::NetBegin()
    {
    // Set data rate and transmit power (note: txpow seems to be ignored by 
    // the library)
    LMIC_setDrTxpow(DR_SF7, 14);

    // Select SubBand prejoin -- saves power for joining
    cLMIC::SelectSubBand(
        cLMIC::SubBand::SubBand_2 // must align with subband on gateway.
        ); 

    // set up the keys for ABP mode.
    AbpProvisioningInfo abpInfo;
    if (this->GetAbpProvisioningInfo(&abpInfo))
        {
        LMIC_setSession(/* port */ 1,
                abpInfo.DevAddr,
                abpInfo.NwkSkey,
                abpInfo.AppSkey
                );

        // becasue it's ABP, we need to set up the paramaters we'd set
        // after an OTAA join.

        // for US, already set SB2 for fast join
        
        // TTN doesn't support link-check
        LMIC_setLinkCheckMode(0);

        // TTN uses SF9 for its RX2 window.
        LMIC.dn2Dr = DR_SF9;
        }

    return true;
    }
