/* begin.cpp	Tue Oct 25 2016 03:38:36 tmm */

/*

Module:  begin.cpp

Function:
	Arduino_LoRaWAN::begin();

Version:
	V1.00a	Tue Oct 25 2016 03:38:36 tmm	Edit level 1

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
   1.00a  Tue Oct 25 2016 03:38:36  tmm
	Module created.

*/

#include <Arduino_LoRaWAN.h>
#include <Arduino_LoRaWAN_lmic.h>
#include <mcciadk_baselib.h>

/* the global instance pointer */
Arduino_LoRaWAN *Arduino_LoRaWAN::pLoRaWAN = NULL;

bool Arduino_LoRaWAN::begin()
    {
    // record self in a static so that we can dispatch events
    ASSERT(Arduino_LoRaWAN::pLoRaWAN == this ||
           Arduino_LoRaWAN::pLoRaWAN == NULL);

    Arduino_LoRaWAN::pLoRaWAN = this;

    // LMIC init
    os_init();

    // Reset the MAC state. Session and pending data transfers will be 
    // discarded.
    LMIC_reset();

    // Set data rate and transmit power (note: txpow seems to be ignored by 
    // the library)
    LMIC_setDrTxpow(DR_SF7,14);

    // Select SubBand
    cLMIC::SelectSubBand(
        cLMIC::SubBand::SubBand_2 // must align with subband on gateway.
        ); 

    // Start job
    // do_send(&sendjob);

    return true;
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

#if 0
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
            break;

        case EV_JOINED:
            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            // TODO: move this to a TTN module; perhaps virtualize?
            LMIC_setLinkCheckMode(0);
            break;

        case EV_RFU1:
            break;

        case EV_JOIN_FAILED:
            break;

        case EV_REJOIN_FAILED:
            break;

        case EV_TXCOMPLETE:
            // TODO: call the completion function for the current send,
            // and advance the queue.
            //
            // Logger may well want to run on its own schedule.
            // TODO: enforce access requriements here (if logger is running
            // too fast, discard?)
            //
            // EV_TXCOMPLETE (includes waiting for RX windows)
            // TODO: if LMIC.dataLen, report a message to the listener.
            //    Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
            // os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);

            break;

        case EV_LOST_TSYNC:
            break;

        case EV_RESET:
            break;

        case EV_RXCOMPLETE:
            // data received in ping slot
            // see TXCOMPLETE.
            break;

        case EV_LINK_DEAD:
            break;

        case EV_LINK_ALIVE:
            break;

        case EV_SCAN_FOUND:
            break;

        case EV_TXSTART:
            break;

        default:
            if (Serial) Serial.println(F("Unknown event"));
            break;
    }
}
#endif

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
    if (this->m_ProvisioningInfo.type != ProvisioningStyle::kOTAA)
        return false;

    memcpy(
        pBuf, 
        this->m_ProvisioningInfo.otaa.AppEUI,
        sizeof(this->m_ProvisioningInfo.otaa.AppEUI)
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
    if (this->m_ProvisioningInfo.type != ProvisioningStyle::kOTAA)
        return false;

    memcpy(
        pBuf, 
        this->m_ProvisioningInfo.otaa.DevEUI,
        sizeof(this->m_ProvisioningInfo.otaa.DevEUI)
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
    if (this->m_ProvisioningInfo.type != ProvisioningStyle::kOTAA)
        return false;

    memcpy(
        pBuf, 
        this->m_ProvisioningInfo.otaa.AppKey,
        sizeof(this->m_ProvisioningInfo.otaa.AppKey)
        );

    return true;
    }

