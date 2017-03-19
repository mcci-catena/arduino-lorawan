/* arduino_lorawan_loop.cpp	Sat Mar 18 2017 23:39:27 tmm */

/*

Module:  arduino_lorawan_loop.cpp

Function:
	Handle processing for top-level loop() in sketch.

Version:
	V0.2.2	Sat Mar 18 2017 23:39:27 tmm	Edit level 2

Copyright notice:
	This file copyright (C) 2016-2017 by

		MCCI Corporation
		3520 Krums Corners Road
                Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.
 
Author:
	Terry Moore, MCCI Corporation	October 2016

Revision history:
   0.1.0  Tue Oct 25 2016 17:05:50  tmm
	Module created.

   0.2.2  Sat Mar 18 2017 23:39:27  tmm
	Renamed to match standards.

*/

#include <Arduino_LoRaWAN.h>
#include <Arduino_LoRaWAN_lmic.h>

void Arduino_LoRaWAN::loop()
        {
        os_runloop_once();
        }
