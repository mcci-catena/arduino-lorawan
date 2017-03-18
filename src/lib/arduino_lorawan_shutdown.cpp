/* arduino_lorawan_shutdown.cpp	Sat Mar 18 2017 00:50:33 tmm */

/*

Module:  arduino_lorawan_shutdown.cpp

Function:
	Arduino_LoRaWAN::Shutdown()

Version:
	V0.2.2	Sat Mar 18 2017 00:50:33 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2017 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.
 
Author:
	Terry Moore, MCCI Corporation	March 2017

Revision history:
   0.2.2  Sat Mar 18 2017 00:50:33  tmm
	Module created.

*/

#include <Arduino_LoRaWAN.h>

#include <Arduino_LoRaWAN_lmic.h>

void Arduino_LoRaWAN::Shutdown(void)
	{
	LMIC_shutdown();
	}
