/* machineq_us915_netjoin.cpp	Sat Mar 11 2017 18:42:29 tmm */

/*

Module:  machineq_us915_netjoin.cpp

Function:
	Arduino_LoRaWAN_machineQ_us915::NetJoin()

Version:
	V0.2.2	Sat Mar 11 2017 18:42:29 tmm	Edit level 2

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
   0.2.0  Tue Nov  1 2016 06:19:25  tmm
	Module created.

*/

#include <Arduino_LoRaWAN_machineQ.h>
#include <Arduino_LoRaWAN_lmic.h>

/****************************************************************************\
|
|		Manifest constants & typedefs.
|
|	This is strictly for private types and constants which will not 
|	be exported.
|
\****************************************************************************/



/****************************************************************************\
|
|	Read-only data.
|
|	If program is to be ROM-able, these must all be tagged read-only 
|	using the ROM storage class; they may be global.
|
\****************************************************************************/



/****************************************************************************\
|
|	VARIABLES:
|
|	If program is to be ROM-able, these must be initialized
|	using the BSS keyword.  (This allows for compilers that require
|	every variable to have an initializer.)  Note that only those 
|	variables owned by this module should be declared here, using the BSS
|	keyword; this allows for linkers that dislike multiple declarations
|	of objects.
|
\****************************************************************************/

#if defined(CFG_us915)
void Arduino_LoRaWAN_machineQ_us915::NetJoin()
	{
	// do the common work.
	this->Super::NetJoin();

	// machineQ is bog standard, and our DNW2 value is already rignt.
	}
#endif // defined(CFG_us915)
