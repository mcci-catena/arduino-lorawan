/* arduino_lorawan_begin.cpp	Sat Mar 18 2017 23:41:11 tmm */

/*

Module:  arduino_lorawan_begin.cpp

Function:
	Arduino_LoRaWAN::begin();

Version:
	V0.2.2	Sat Mar 18 2017 23:41:11 tmm	Edit level 4

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
   0.1.0  Tue Oct 25 2016 03:38:36  tmm
	Module created.

   0.2.2  Sun Mar 12 2017 15:05:33  tmm
	Revise to support stable storage of join parameters.

   0.2.2  Sat Mar 18 2017 23:41:11  tmm
	Rename to follow project standards.

*/

#include <Arduino_LoRaWAN.h>
#include <Arduino_LoRaWAN_lmic.h>
#include <mcciadk_baselib.h>
#include <hal/hal.h>

/* the global instance pointer */
Arduino_LoRaWAN *Arduino_LoRaWAN::pLoRaWAN = NULL;

/* we have to provide the LMIC's instance of the lmic_pins structure */
static struct ::lmic_pinmap s_lmic_pins;

bool Arduino_LoRaWAN::begin()
    {
    // record self in a static so that we can dispatch events
    ASSERT(Arduino_LoRaWAN::pLoRaWAN == this ||
           Arduino_LoRaWAN::pLoRaWAN == NULL);

    Arduino_LoRaWAN::pLoRaWAN = this;

    // set up the LMIC pinmap.
    // this is a little awkward because we don't want the LMIC
    // header in scope everywhere, so we have a duplicate C++
    // structure that's part of the class. Therefore we have to
    // copy element by element.
    s_lmic_pins.nss = this->m_lmic_pins.nss;
    s_lmic_pins.rxtx = this->m_lmic_pins.rxtx;
    s_lmic_pins.rst = this->m_lmic_pins.rst;
    memcpy(s_lmic_pins.dio, this->m_lmic_pins.dio, sizeof(s_lmic_pins.dio));
    s_lmic_pins.rxtx_rx_active = this->m_lmic_pins.rxtx_rx_active;
    s_lmic_pins.spi_freq = this->m_lmic_pins.spi_freq;

    // LMIC init
    if (! os_init_ex(&s_lmic_pins))
        return false;

    // Reset the MAC state. Session and pending data transfers will be
    // discarded.
    LMIC_reset();

    return this->NetBegin();
    }

/****************************************************************************\
|
|	The event dispatcher
|
\****************************************************************************/

void onEvent(ev_t ev)
    {
    Arduino_LoRaWAN * const pLoRaWAN = Arduino_LoRaWAN::GetInstance();

    ASSERT(pLoRaWAN != NULL);

    pLoRaWAN->DispatchEvent(ev);
    }

void Arduino_LoRaWAN::DispatchEvent(
    uint32_t ev
    )
    {
    ARDUINO_LORAWAN_PRINTF(
        LogVerbose,
        "EV_%s\n",
        cLMIC::GetEventName(ev)
        );

    // do the usual work in another function, for clarity.
    this->StandardEventProcessor(ev);

    // dispatch to the registered clients
    for (unsigned i = 0; i < this->m_nRegisteredListeners; ++i)
        {
        this->m_RegisteredListeners[i].ReportEvent(ev);
        }
    }

bool Arduino_LoRaWAN::RegisterListener(
    ARDUINO_LORAWAN_EVENT_FN *pEventFn,
    void *pContext
    )
    {
    if (this->m_nRegisteredListeners <
                MCCIADK_LENOF(this->m_RegisteredListeners))
        {
        Arduino_LoRaWAN::Listener * const pListener =
                &this->m_RegisteredListeners[this->m_nRegisteredListeners];
        ++this->m_nRegisteredListeners;

        pListener->pEventFn = pEventFn;
        pListener->pContext = pContext;
        return true;
        }
    else
        {
        return false;
        }
    }

const char *
Arduino_LoRaWAN::cLMIC::GetEventName(uint32_t ev)
    {
    static const char szEventNames[] = ARDUINO_LORAWAN_LMIC_EV_NAMES_MZ__INIT;
    const char *p;

    if (ev < ARDUINO_LORAWAN_LMIC_EV_NAMES__BASE)
        return "<<unknown>>";

    p = McciAdkLib_MultiSzIndex(
        szEventNames,
        ev - ARDUINO_LORAWAN_LMIC_EV_NAMES__BASE
        );

    if (*p == '\0')
        return "<<unknown>>";
    else
        return p;
    }

/*

Name:	Arduino_LoRaWAN::StandardEventProcessor()

Function:
	Handle LMIC events.

Definition:
	private: void Arduino_LoRaWAN::StandardEventProcessor(
			uint32_t ev
			);

Description:
	The simple events emitted from the LMIC core are processed, both
	to arrange for completions and notificatoins for asynchronous events,
	and to generate notifications to cause data to be pushed to the
	platform's persistent storage.

Returns:
	No explicit result.

*/

void Arduino_LoRaWAN::StandardEventProcessor(
    uint32_t ev
    )
    {
    this->UpdateFCntDown(LMIC.seqnoDn);

    switch(ev)
        {
        case EV_SCAN_TIMEOUT:
            break;
        case EV_BEACON_FOUND:
            break;
        case EV_BEACON_MISSED:
            break;
        case EV_BEACON_TRACKED:
            break;
        case EV_JOINING:
            // if we're joining, try to see if we have OTAA info. If not
            // we need to just reset.
            if (! this->GetOtaaProvisioningInfo(nullptr))
                {
                bool txPending = this->m_fTxPending;
                this->m_fTxPending = false;
                // notify client that TX is complete
                if (txPending && this->m_pSendBufferDoneFn)
                        this->m_pSendBufferDoneFn(this->m_pSendBufferDoneCtx, false);
                }
            break;

        case EV_JOINED:
            {
	    // announce that we have joined; allows for
	    // network-specific fixups, and saving keys.
	    this->NetJoin();
            SessionInfo Info;
            Info.V1.Tag = kSessionInfoTag_V1;
            Info.V1.Size = sizeof(Info);
            Info.V1.Rsv2 = 0;
            Info.V1.Rsv3 = 0;
            LMIC_getSessionKeys(&Info.V1.NetID, &Info.V1.DevAddr, Info.V1.NwkSKey, Info.V1.AppSKey);
            Info.V1.FCntUp = LMIC.seqnoUp;
            Info.V1.FCntDown = LMIC.seqnoDn;

            this->NetSaveSessionInfo(Info, nullptr, 0);
            }
            break;

        case EV_RFU1:
            break;

        case EV_JOIN_FAILED:
	    // we failed the join. But we keep trying; client must
	    // do a reset to stop us.
	    // TODO(tmm@mcci.com): this->NetJoinFailed(), and/or
            // an outcall
            break;

        case EV_REJOIN_FAILED:
	    // after we join, if ABP is enabled (LMIC_setLinkCheck(true)),
	    // if we don't get downlink messages for a while, we'll try to
	    // rejoin. This message indicated that the rejoin failed.
	    // TODO(tmm@mcci.com): this->NetRejoinFailed(), and/or
	    //	an outcall
            break;

        case EV_TXCOMPLETE:
            this->m_fTxPending = false;

	    // notify framework that RX may be available (because this happens
	    // after every transmit).
	    this->NetRxComplete();

	    // notify framework that tx is complete
            this->NetTxComplete();

	    // notify client that TX is complete
            this->completeTx(true);
            break;

        case EV_LOST_TSYNC:
	    // only for class-B or class-C: we lost beacon time synch.
            break;

        case EV_RESET:
	    // the LoRaWAN MAC just got reset due to a pending frame rollover
	    // on FCntDn or actual rollover on FCntUp.
            break;

        case EV_RXCOMPLETE:
            // data received in ping slot
            // see TXCOMPLETE.

	    // follow protocol:
	    this->NetRxComplete();
            break;

        case EV_LINK_DEAD:
            break;

        case EV_LINK_ALIVE:
            break;

        case EV_SCAN_FOUND:
            break;

        case EV_TXSTART:
	    this->NetSaveFCntUp(LMIC.seqnoUp);
            break;

	default:
	    break;
	}
    }

void Arduino_LoRaWAN::NetRxComplete(void)
	{
	// notify client that RX is available
	if (LMIC.dataLen)
		{
		if (this->m_pReceiveBufferFn)
			{
			this->m_pReceiveBufferFn(
				this->m_pReceiveBufferCtx,
				LMIC.frame + LMIC.dataBeg,
				LMIC.dataLen
				);
			}
		}

	// Try to save the Rx sequence number.
	// For efficiency, client should look for changes
	// since last save.
	this->UpdateFCntDown(LMIC.seqnoDn);
	}

/****************************************************************************\
|
|	Getting provisioning info
|
\****************************************************************************/

void os_getArtEui(uint8_t* buf)
    {
    Arduino_LoRaWAN * const pLoRaWAN = Arduino_LoRaWAN::GetInstance();

    ASSERT(pLoRaWAN != NULL);

    pLoRaWAN->GetAppEUI(buf);
    }

bool Arduino_LoRaWAN::GetAppEUI(
    uint8_t *pBuf
    )
    {
    OtaaProvisioningInfo otaaInfo;

    if (! this->GetOtaaProvisioningInfo(&otaaInfo))
        return false;

    memcpy(
        pBuf,
        otaaInfo.AppEUI,
        sizeof(otaaInfo.AppEUI)
        );

    return true;
    }

void os_getDevEui(uint8_t* buf)
    {
    Arduino_LoRaWAN * const pLoRaWAN = Arduino_LoRaWAN::GetInstance();

    ASSERT(pLoRaWAN != NULL);

    pLoRaWAN->GetDevEUI(buf);
    }

bool Arduino_LoRaWAN::GetDevEUI(
    uint8_t *pBuf
    )
    {
    OtaaProvisioningInfo otaaInfo;

    if (! this->GetOtaaProvisioningInfo(&otaaInfo))
        return false;

    memcpy(
        pBuf,
        otaaInfo.DevEUI,
        sizeof(otaaInfo.DevEUI)
        );

    return true;
    }



void os_getDevKey(uint8_t* buf)
    {
    Arduino_LoRaWAN * const pLoRaWAN = Arduino_LoRaWAN::GetInstance();

    ASSERT(pLoRaWAN != NULL);

    pLoRaWAN->GetAppKey(buf);
    }

bool Arduino_LoRaWAN::GetAppKey(
    uint8_t *pBuf
    )
    {
    OtaaProvisioningInfo otaaInfo;

    if (! this->GetOtaaProvisioningInfo(&otaaInfo))
        return false;

    memcpy(
        pBuf,
        otaaInfo.AppKey,
        sizeof(otaaInfo.AppKey)
        );

    return true;
    }

