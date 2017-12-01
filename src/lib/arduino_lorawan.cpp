/* arduino_lorawan.cpp	Tue Oct 25 2016 03:59:08 tmm */

/*

Module:  arduino_lorawan.cpp

Function:
	Constructor: Arduino_LoRaWAN::Arduino_LoRaWAN()

Version:
	V0.1.0	Tue Oct 25 2016 03:59:08 tmm	Edit level 1

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
   0.1.0  Tue Oct 25 2016 03:59:08  tmm
	Module created.

*/

#include <Arduino_LoRaWAN.h>

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

Arduino_LoRaWAN::Arduino_LoRaWAN()
        {
	// initialize the pin structure, which mostly needs to be 
	// lmic_pinmap::LMIC_UNUSED_PIN
        memset(&this->m_lmic_pins, lmic_pinmap::LMIC_UNUSED_PIN, sizeof(this->m_lmic_pins));
	
	// However, a few fields need to be different.
	// By default, we want the RX/TX pin to be high for TX.
        this->m_lmic_pins.rxtx_rx_active = 0;
	
	// By default, use the SPI frequency that comes from the LMIC
	// configuration file.
        this->m_lmic_pins.spi_freq = 0;	/* use default */
        }
