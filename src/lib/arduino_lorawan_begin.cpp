/*

Module:  arduino_lorawan_begin.cpp

Function:
        Arduino_LoRaWAN::begin();

Copyright notice:
        See LICENSE file accompanying this project.

Author:
        Terry Moore, MCCI Corporation	October 2016

*/

#include <Arduino_LoRaWAN.h>

#include <arduino_lmic_hal_boards.h>
#include <Arduino_LoRaWAN_lmic.h>
#include <mcciadk_baselib.h>
#include <hal/hal.h>

/* the global instance pointer */
Arduino_LoRaWAN *Arduino_LoRaWAN::pLoRaWAN = NULL;

// if called with no arguments, ask the library
bool Arduino_LoRaWAN::begin()
    {
    return this->begin(Arduino_LMIC::GetPinmap_ThisBoard());
    }

bool Arduino_LoRaWAN::begin(
    const Arduino_LMIC::HalPinmap_t * pPinmap
    )
    {
    // record self in a static so that we can dispatch events
    ASSERT(Arduino_LoRaWAN::pLoRaWAN == this ||
           Arduino_LoRaWAN::pLoRaWAN == NULL);

    Arduino_LoRaWAN::pLoRaWAN = this;

    // LMIC init -- need to pass a void* pointer through to the hal,
    // but it must point to an instance of ArduinoLMIC::HalConfiguration_t.
    if (! os_init_ex(pPinmap))
        return false;

    // Reset the MAC state. Session and pending data transfers will be
    // discarded.
    LMIC_reset();

    // Set data rate and transmit power, based on regional considerations.
    // this is redundant if LMIC_reset() sends EV_RESET, but ... this
    // hasn't been changed yet, and might not be possible to change.
    this->NetBeginRegionInit();

    //
    // If no provisining info, return false.
    //
    if (this->GetProvisioningStyle() == ProvisioningStyle::kNone)
        return false;

    //
    // this will succeed either if provisioned for Abp, or if Otaa and we
    // have successfully joined.  Note that ABP is just exactly the same
    // as what happends after a join, so we use this for fetching all the
    // required information.
    //
    AbpProvisioningInfo abpInfo;

    if (this->GetAbpProvisioningInfo(&abpInfo))
        {
        LMIC_setSession(
                abpInfo.NetID,
                abpInfo.DevAddr,
                abpInfo.NwkSKey,
                abpInfo.AppSKey
                );

        // set the seqnoUp and seqnoDown
        // presumably if non-zero, somebody is stashing these
        // in NVR
        LMIC.seqnoUp = abpInfo.FCntUp;
        LMIC.seqnoDn = abpInfo.FCntDown;

        // because it's ABP, we need to set up the parameters we'd set
        // after an OTAA join.
        this->NetJoin();
        }

    return true;
    }

/****************************************************************************\
|
|   The event dispatcher.
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
    // update our idea of the downlink counter.
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
            // notify framework that RX may be available (because this happens
            // after every transmit).
            this->NetRxComplete();

            // notify framework that tx is complete
            this->NetTxComplete();
            break;

        case EV_LOST_TSYNC:
            // only for class-B or class-C: we lost beacon time synch.
            break;

        case EV_RESET:
            // the LoRaWAN MAC just got reset due to a pending frame rollover
            // on FCntDn or actual rollover on FCntUp.

            // Set data rate, channels, and transmit power, based on regional considerations.
            this->NetBeginRegionInit();
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

        case EV_TXCANCELED:
            break;

        case EV_JOIN_TXCOMPLETE:
            break;

        default:
            break;
        }
    }

void Arduino_LoRaWAN::NetRxComplete(void)
        {
        // notify client that RX is available
        if (LMIC.dataLen  != 0 || LMIC.dataBeg != 0)
                {
                uint8_t port;
                port = 0;
                if (LMIC.txrxFlags & TXRX_PORT)
                        port = LMIC.frame[LMIC.dataBeg - 1];

                if (this->m_pReceiveBufferFn)
                        {
                        this->m_pReceiveBufferFn(
                                this->m_pReceiveBufferCtx,
                                port,
                                LMIC.frame + LMIC.dataBeg,
                                LMIC.dataLen
                                );
                        }
                }
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

