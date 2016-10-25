/* Arduino_LoRaWAN.h	Tue Oct 25 2016 00:10:57 tmm */

/*

Module:  Arduino_LoRaWAN.h

Function:
	The base class for arduino-lmic-based LoRaWAN nodes.

Version:
	V0.1.0	Tue Oct 25 2016 00:10:57 tmm	Edit level 1

Copyright notice:
	This file copyright (C) 2016 by

		MCCI Corporation
		3520 Krums Corners Road
                Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of MCCI Corporation.
 
Author:
	Terry Moore, MCCI Corporation	October 2016

Revision history:
   0.1.0  Tue Oct 25 2016 00:10:57  tmm
	Module created.

*/

#ifndef _ARDUINO_LORAWAN_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_H_

#include <stdint.h>
#include <Arduino.h>
#include <lmic.h>

class Arduino_LoRaWAN;


class Arduino_LoRaWAN
        {
public:
        /*
        || You can use this for declaring event functions...
        || or use a lambda if you're bold; but remember, no
        || captures in that case.
        */
        typedef void EVENT_FN(void *, ev_t);

        /*
        || the constructor.
        */
        Arduino_LoRaWAN();

        /*
        || the begin function. Call this to start things (the constructor
        || does not!
        */
        bool begin(void);

        /*
        || Registering listeners... returns true for
        || success.
        */
        bool RegisterListener(EVENT_FN *, void *);

private:
        };



/**** end of Arduino_LoRaWAN.h ****/
#endif /* _ARDUINO_LORAWAN_H_ */
