/* GetTxReady.cpp	Tue Oct 25 2016 17:31:33 tmm */

/*

Module:  GetTxReady.cpp

Function:
	Arduino_LoRaWAN::GetTxReady()

Version:
	V0.1.0	Tue Oct 25 2016 17:31:33 tmm	Edit level 1

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
   0.1.0  Tue Oct 25 2016 17:31:33  tmm
	Module created.

*/

#include <Arduino_LoRaWAN.h>

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

/*

Name:   Arduino_LoRaWAN::GetTxReady()

Function:
        Return TRUE if the LMIC can accept a transmit operation.

Definition:
        public bool Arduino_LoRaWAN::GetTxReady() const;

Description:
        If the LMIC is already processing a message, or if there's
        a message in the Arduino_LoRaWAN queue being processed,
        then transmit is not ready. Otherwise, transmit is ready.

Returns:
        true if ready for a message, false otherwise.

*/

bool Arduino_LoRaWAN::GetTxReady() const
        {
        if (LMIC.opmode & OP_TXRXPEND)
                return false;
        else
                return ! this->m_SendBufferData.fTxPending;
        }
