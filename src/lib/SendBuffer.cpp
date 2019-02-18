/* SendBuffer.cpp	Tue Oct 25 2016 17:25:44 tmm */

/*

Module:  SendBuffer.cpp

Function:
	Arduino_LoRaWAN::SendBuffer()

Version:
	V0.1.0	Tue Oct 25 2016 17:25:44 tmm	Edit level 1

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
   0.1.0  Tue Oct 25 2016 17:25:44  tmm
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
|	Variables
|
\****************************************************************************/

bool Arduino_LoRaWAN::SendBuffer(
        const uint8_t *pBuffer,
        size_t nBuffer,
        SendBufferCbFn *pDoneFn,
        void *pDoneCtx,
	bool fConfirmed,
	uint8_t port
        )
        {
        if (this->m_fTxPending || LMIC.opmode & OP_TXRXPEND)
                {
                if (pDoneFn)
                        (*pDoneFn)(pDoneCtx, false);
                return false;
                }

        const int iResult = LMIC_setTxData2(
                                /* port: */ port != 0 ? port : 1,
                                const_cast<xref2u1_t>(pBuffer),
                                nBuffer,
                                /* confirmed? */ fConfirmed
                                );

        if (iResult == 0)
                {
                this->m_fTxPending = true;
                this->m_pSendBufferDoneFn = pDoneFn;
                this->m_pSendBufferDoneCtx = pDoneCtx;
                return true;
                }
        else
                {
                if (pDoneFn)
                        (*pDoneFn)(pDoneCtx, false);
                return false;
                }
        }
