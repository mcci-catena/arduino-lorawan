/* ttn_base_netbegin.cpp	Fri May 19 2017 23:58:34 tmm */

/*

Module:  ttn_base_netbegin.cpp

Function:
	Arduino_LoRaWAN_ttn_base::NetBegin()

Version:
	V0.2.3	Fri May 19 2017 23:58:34 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.

	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.

Author:
	Terry Moore, MCCI Corporation	May 2017

Revision history:
   0.2.3  Fri May 19 2017 23:58:34  tmm
	Module created.

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

bool Arduino_LoRaWAN_ttn_base::NetBegin()
    {
    //
    // If no provisining info, return false.
    //
    if (this->GetProvisioningStyle() == ProvisioningStyle::kNone)
	return false;

    // Set data rate and transmit power, based on regional considerations.
    this->NetBeginRegionInit();

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
