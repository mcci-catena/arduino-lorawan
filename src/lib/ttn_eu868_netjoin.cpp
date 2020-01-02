/* ttn_eu868_netjoin.cpp	Fri May 19 2017 23:58:34 tmm */

/*

Module:  ttn_eu868_netjoin.cpp

Function:
	Arduino_LoRaWAN_ttn_eu868::NetJoin()

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


void Arduino_LoRaWAN_ttn_eu868::NetJoin()
	{
	// do the common work.
	this->Super::NetJoin();

	// then, for EU, if ABP, set the RX2 datarate, which is non-standard.
	// for OTAA, just do what we're told.
	if (this->GetProvisioningStyle() == this->ProvisioningStyle::kABP)
		LMIC.dn2Dr = LORAWAN_DR3;	// this is SF9.
	}
