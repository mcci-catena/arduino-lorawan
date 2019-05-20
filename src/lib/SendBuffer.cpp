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

static lmic_txmessage_cb_t localSendBufferCb;

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

/*

Name:   Arduino_LoRaWAN::SendBuffer()

Function:
        Send a message via the LMIC.

Definition:
        public typedef void Arduino_LoRaWAN::SendBufferCbFn(
                                void *pDoneCtx,
                                bool fSuccess
                                );

        public bool Arduino_LoRaWAN::SendBuffer(
                const uint8_t *pBuffer,
                size_t nBuffer,
                SendBufferCbFn *pDoneFn,
                void *pDoneCtx,
                bool fConfirmed = false,
                uint8_t port = 1
                );

Description:
        The message is submitted to the LMIC for transmission to the network,
        if possible. When processing is finished, pDoneFn is called as follows:

                (*pDoneFn)(pDoneCtx, fSuccess);

        fSuccess will be true if (as far as we can tell) the message was successfully
        transmitted. For unconfirmed uplinks, as long as we sent the message, we
        return true. (Uplinks might be canceled due to link tracking or other issues.)

        If the LMIC is already processing a message, then the request
        is immediately completed, and this routine returns false.

        We guarantee that pDoneFn will be called once, when message
        processing is complete.

Returns:
        The result is not very useful, because of the guarantees around
        completion processing. However, if false, then the transmit failed,
        and the completion routine has already been called. If true, then
        the transmit was started; the completion routine might already have
        been called.  We don't recommend using this result.

*/


bool Arduino_LoRaWAN::SendBuffer(
        const uint8_t *pBuffer,
        size_t nBuffer,
        SendBufferCbFn *pDoneFn,
        void *pDoneCtx,
	bool fConfirmed,
	uint8_t port
        )
        {
        if (! this->GetTxReady())
                {
                if (pDoneFn)
                        (*pDoneFn)(pDoneCtx, false);
                return false;
                }

        this->m_SendBufferData.pDoneFn = pDoneFn;
        this->m_SendBufferData.pDoneCtx = pDoneCtx;
        this->m_SendBufferData.fTxPending = true;
        this->m_SendBufferData.pSelf = this;

        const int iResult = LMIC_sendWithCallback(
                                /* port: */ port != 0 ? port : 1,
                                const_cast<xref2u1_t>(pBuffer),
                                nBuffer,
                                /* confirmed? */ fConfirmed,
                                localSendBufferCb,
                                (void *)&this->m_SendBufferData
                                );

        if (iResult == 0)
                {
                // transmit was queued, will complete.
                return true;
                }
        else
                {
                this->m_SendBufferData.fTxPending = false;
                if (pDoneFn)
                        (*pDoneFn)(pDoneCtx, false);
                return false;
                }
        }

static void
localSendBufferCb(
        void *pUserData,
        int fSuccess
        )
        {
        auto const pSendBufferData =
                (Arduino_LoRaWAN::SendBufferData_t *)pUserData;
        auto const pDoneFn = pSendBufferData->pDoneFn;

        pSendBufferData->fTxPending = false;

        if (pDoneFn != nullptr)
                {
                pSendBufferData->pDoneFn = nullptr;
                pDoneFn(pSendBufferData->pDoneCtx, fSuccess);
                }
        }
