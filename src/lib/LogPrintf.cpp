/* LogPrintf.cpp	Tue Nov  1 2016 12:55:06 tmm */

/*

Module:  LogPrintf.cpp

Function:
	Arduino_LoRaWAN::LogPrintf()

Version:
	V0.2.0	Tue Nov  1 2016 12:55:06 tmm	Edit level 1

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
   0.2.0  Tue Nov  1 2016 12:55:06  tmm
	Module created.

*/

#include <Arduino_LoRaWAN.h>

#include <Arduino.h>
#include <stdio.h>
#include <stdarg.h>

void
Arduino_LoRaWAN::LogPrintf(
	const char *fmt, 
	...
	)
	{
	if (! Serial.dtr())
		return;

	char buf[128];
	va_list ap;

	va_start(ap, fmt);
	(void) vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
	va_end(ap);

	// in case we overflowed:
	buf[sizeof(buf) - 1] = '\0';
	if (Serial.dtr()) Serial.print(buf);
	}
