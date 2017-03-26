/* ttn_us915_netbegin.cpp	Sun Mar 12 2017 16:21:31 tmm */

/*

Module:  ttn_us915_netbegin.cpp

Function:
	Arduino_LoRaWAN_ttn_us915::NetBegin()

Version:
	V0.2.2	Sun Mar 12 2017 16:21:31 tmm	Edit level 2

Copyright notice:
	This file copyright (C) 2016-2017 by

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

   0.2.2  Sun Mar 12 2017 16:21:31  tmm
	Clarify documentation.

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

/* 
|| TODO(tmm@mcci.com): refactor to move common eu868/us915/etc logic to
|| Arduino_LoRaWAN_ttn::NetBegin(). 
*/
bool Arduino_LoRaWAN_ttn_us915::NetBegin()
    {
    //
    // If no provisining info, return false. 
    //
    if (this->GetProvisioningStyle() == ProvisioningStyle::kNone)
	return false;

    // Set data rate and transmit power
    // DR_SF7 is US DR3; 14 means 14 dBm
    
    // XXX (tmm@mcci.com) although LMIC.adrTxpow is set to 14, it's
    // never used inside the LMIC library. This is because LMIC's radio.c uses
    // LMIC.txpow, and in US915, lmic.c::updatetx() sets LMIC.txpow to 30
    // for 125kHz channels, and  26 for 500kHz channels, ignoring
    // LMIC.adrTxpow.  Then radio.c limits to the value for 10 dBm, and
    // apparendly doesn't even turn on the +20 dBm option if over 10 dBm.

    LMIC_setDrTxpow(DR_SF7, 14);

    // Select SubBand prejoin -- saves power for joining
    // This is specific to the US915 bandplan.
    cLMIC::SelectSubBand(
        cLMIC::SubBand::SubBand_2 // must align with subband on gateway.
        ); 

    //
    // this will succeed either if provisioned for Abp, or if Otaa and we
    // have successfully joined.  Note that ABP is just exactly the same
    // as what happends after a join, so we use this for fetching all the
    // required information.
    //
    AbpProvisioningInfo abpInfo;

    if (this->GetAbpProvisioningInfo(&abpInfo))
        {
        LMIC_setSession(/* port */ 1,
                abpInfo.DevAddr,
                abpInfo.NwkSKey,
                abpInfo.AppSKey
                );

	// set the seqnoUp and seqnoDown
	// presumably if non-zero, somebody is stashing these
	// in NVR
	LMIC.seqnoUp = abpInfo.FCntUp;
	LMIC.seqnoDn = abpInfo.FCntDown;

        // because it's ABP, we need to set up the parameters we'd set
        // after an OTAA join.
	this->NetJoin();
        }

    return true;
    }
