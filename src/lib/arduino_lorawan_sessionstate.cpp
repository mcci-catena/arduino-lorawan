/*

Module:	arduino_lorawan_sessionstate.cpp

Function:
	Methods to construct and apply saved session state.

Copyright and License:
	This file copyright (C) 2021 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	See accompanying LICENSE file for copyright and license information.

Author:
	Terry Moore, MCCI Corporation	March 2021

*/

#include <Arduino_LoRaWAN.h>

#include <arduino_lmic_hal_boards.h>
#include <Arduino_LoRaWAN_lmic.h>

/*

Name:	Arduino_LoRaWAN::BuildSessionState()

Function:
	Build a SessionState structure based on current LMIC state.

Definition:
	void Arduino_LoRaWAN::BuildSessionState(
		Arduino_LoRaWAN::SessionState &State
		);

Description:
	Session state is extracted from the LMIC data structure and
	placed into `State`.

Returns:
	No explicit result.

Notes:


*/

#define FUNCTION "Arduino_LoRaWAN::BuildSessionState"

void
Arduino_LoRaWAN::BuildSessionState(
	Arduino_LoRaWAN::SessionState &State
	) const
	{
    const auto tNow = os_getTime();

    memset(&State, 0, sizeof(State));

    State.Header.Tag = kSessionStateTag_V1;
    State.Header.Size = sizeof(State);
    State.V1.Region = uint8_t(this->GetRegion());
    State.V1.LinkDR = LMIC.datarate;

    State.V1.FCntUp = LMIC.seqnoUp;
    State.V1.FCntDown = LMIC.seqnoDn;
    // TODO(tmm@mcci.com): set GPS time to our best idea
    // State.V1.gpsTime = 0; -- already try via memset().
    State.V1.globalAvail = LMIC.globalDutyAvail - tNow;
    State.V1.Rx2Frequency = LMIC.dn2Freq;

#if !defined(DISABLE_PING)
    State.V1.PingFrequency = LMIC.ping.freq;
#endif

    State.V1.Country = uint16_t(this->GetCountryCode());
    State.V1.LinkIntegrity = LMIC.adrAckReq;
    State.V1.TxPower = LMIC.adrTxPow;
    State.V1.Redundancy = LMIC.upRepeat;
    State.V1.DutyCycle = LMIC.globalDutyRate;
    State.V1.Rx1DRoffset = LMIC.rx1DrOffset;
    State.V1.Rx2DataRate = LMIC.dn2Dr;
    State.V1.RxDelay = LMIC.rxDelay;
#if LMIC_ENABLE_TxParamSetupReq
    State.V1.TxParam = LMIC.txParam;
#endif
#if !defined(DISABLE_BEACONS)
    State.V1.BeaconChannel = LMIC.bcnChnl;
#endif
#if !defined(DISABLE_PING)
    State.V1.PingDr = LMIC.ping.dr;
#endif
    State.V1.MacRxParamAns = LMIC.dn2Ans;
    State.V1.MacDlChannelAns = LMIC.macDlChannelAns;
    State.V1.MacRxTimingSetupAns = LMIC.macRxTimingSetupAns;

    // handle EU like regions
#if CFG_LMIC_EU_like
    State.V1.Channels.Header.Tag = State.V1.Channels.Header.kEUlike;
    State.V1.Channels.Header.Size = sizeof(State.V1.Channels.EUlike);
    State.V1.Channels.EUlike.clearAll();
    constexpr unsigned maxCh = MAX_CHANNELS < State.V1.Channels.EUlike.nCh ? MAX_CHANNELS : State.V1.Channels.EUlike.nCh;
    State.V1.Channels.EUlike.ChannelMap = LMIC.channelMap;
#if ARDUINO_LMIC_VERSION_COMPARE_GE(ARDUINO_LMIC_VERSION, ARDUINO_LMIC_VERSION_CALC(3,99,0,1))
    State.V1.Channels.EUlike.ChannelShuffleMap = LMIC.channelShuffleMap;
#endif

    // EU: save channel settings
    for (unsigned ch = 0; ch < maxCh; ++ch)
        {
        auto const freq = LMIC.channelFreq[ch];
        State.V1.Channels.EUlike.setFrequency(State.V1.Channels.EUlike.UplinkFreq, ch, freq & ~3);
        State.V1.Channels.EUlike.setBand(ch, freq & 3);
        State.V1.Channels.EUlike.ChannelDrMap[ch] = LMIC.channelDrMap[ch];

#if !defined(DISABLE_MCMD_DlChannelReq)
        State.V1.Channels.EUlike.setFrequency(State.V1.Channels.EUlike.DownlinkFreq, ch, LMIC.channelDlFreq[ch]);
#endif
        }

    // EU: save band state
    for (unsigned iBand = 0; iBand < State.V1.Channels.EUlike.nBands; ++iBand)
        {
        auto & band = State.V1.Channels.EUlike.Bands[iBand];

        band.txDutyDenom = LMIC.bands[iBand].txpow;
        band.txPower = LMIC.bands[iBand].txpow;
        band.lastChannel = LMIC.bands[iBand].lastchnl;

        // don't record tAval from the past, and only record into the future.
        auto const tDelta = LMIC.bands[iBand].avail - tNow;
        band.ostimeAvail = tDelta > 0 ? tDelta : 0;
        }

#elif CFG_LMIC_US_like
    State.V1.Channels.Header.Tag = State.V1.Channels.Header.kUSlike;
    State.V1.Channels.Header.Size = sizeof(State.V1.Channels.USlike);

#if ARDUINO_LMIC_VERSION_COMPARE_GE(ARDUINO_LMIC_VERSION, ARDUINO_LMIC_VERSION_CALC(3,99,0,1))
    static_assert(
        sizeof(State.V1.Channels.USlike.ChannelShuffleMap) == sizeof(LMIC.channelShuffleMap),
        "USlink.ChannelShuffleMap size is wrong"
        );
    memcpy(State.V1.Channels.USlike.ChannelShuffleMap, LMIC.channelShuffleMap, sizeof(State.V1.Channels.USlike.ChannelShuffleMap));
#endif

    State.V1.Channels.USlike.clearAll();
    for (unsigned ch = 0; ch < State.V1.Channels.USlike.nCh; ++ch)
        {
        bool state = ENABLED_CHANNEL(ch);
        State.V1.Channels.USlike.enable(ch, state);
        }
#endif
	}

#undef FUNCTION

/*

Name:	Arduino_LoRaWAN::SaveSessionState()

Function:
    Internal: build and save session state

Definition:
    void Arduino_LoRaWAN::SaveSessionState(
        void
        );

Description:
    This is just a convenience wrapper: first build, then
    save the session state. The client must have supplied
    a virtual override for NetSaveSessionState() to actually
    do the save.

Returns:
    No explicit result.

Notes:


*/

void Arduino_LoRaWAN::SaveSessionState()
    {
    SessionState State;

    this->BuildSessionState(State);
    if (memcmp(&State, &this->m_savedSessionState, sizeof(State)) != 0)
        {
        this->m_savedSessionState = State;
        this->NetSaveSessionState(State);
        }
    }

/*

Name:	Arduino_LoRaWAN::SaveSessionState()

Function:
    Internal: fetch and apply session state

Definition:
    bool Arduino_LoRaWAN::RestoreSessionState(
        void
        );

Description:
    This is just a convenience wrapper: first fetch, then
    apply the session state. The client must have supplied
    a virtual override for NetSaveSessionState() to actually
    do the save. The restored state must generally match
    the configured region, or it will be ignored.

Returns:
    No explicit result.

Notes:


*/

bool Arduino_LoRaWAN::RestoreSessionState()
    {
    SessionState State;

    if (! this->NetGetSessionState(State))
        return false;

    return this->ApplySessionState(State);
    }

/*

Name:	Arduino_LoRaWAN::ApplySessionState()

Function:
    Apply SessionState data to current LMIC session.

Definition:
    bool Arduino_LoRaWAN::ApplySessionState(
        Arduino_LoRaWAN::SessionState const &State
        );

Description:
    If the contents of the State structure are useful, they are applied
    to the current LMIC session, to bring things back to the point
    where the were at moment it was saved.

Returns:
    `true` if the state could be applied, `false` otherwise.

Notes:


*/

bool Arduino_LoRaWAN::IsValidState(const Arduino_LoRaWAN::SessionState &state) const
    {
    // do not apply the session state unless it roughly matches our configuration.
    if (! state.isValid())
        return false;

    // make sure region and country match. TODO: make sure network matches.
    if (! (Arduino_LoRaWAN::Region(state.V1.Region) == this->GetRegion() &&
           state.V1.Country == uint16_t(this->GetCountryCode())))
        return false;

    // it matches!
    return true;
    }

bool Arduino_LoRaWAN::SessionState::isValid() const
    {
    if (! (this->Header.Tag == kSessionStateTag_V1 &&
           this->Header.Size == sizeof(*this)))
          return false;

    switch (this->V1.Channels.Header.Tag)
        {
    case Arduino_LoRaWAN::SessionChannelMask_Header::eMaskKind::kEUlike:
        return this->V1.Channels.Header.Size == sizeof(this->V1.Channels.EUlike);

    case Arduino_LoRaWAN::SessionChannelMask_Header::eMaskKind::kUSlike:
        return this->V1.Channels.Header.Size == sizeof(this->V1.Channels.USlike);

    case Arduino_LoRaWAN::SessionChannelMask_Header::eMaskKind::kCNlike:
        return this->V1.Channels.Header.Size == sizeof(this->V1.Channels.CNlike);

    default:
        return false;
        }
    }

#define FUNCTION "Arduino_LoRaWAN::ApplySessionState"

bool
Arduino_LoRaWAN::ApplySessionState(
    const Arduino_LoRaWAN::SessionState &State
    )
    {
    // do not apply the session state unless it roughly matches our configuration.
    if (! this->IsValidState(State))
        return false;

    auto const tNow = os_getTime();

    // record that we've done it.
    this->m_savedSessionState = State;

    // set FcntUp, FcntDown, and session state
    LMIC.datarate   = State.V1.LinkDR;

    // set the uplink and downlink count
    LMIC.seqnoDn    = State.V1.FCntDown;
    LMIC.seqnoUp    = State.V1.FCntUp;

    //
    // TODO(tmm@mcci.com): State.V1.gpsTime can be used to tweak the saved cycle
    // time and also as a fallback if the system clock is not robust. but right
    // now we ignore it.
    //

    // conservatively set the global avail time.
    LMIC.globalDutyAvail = tNow + State.V1.globalAvail;

    // set the Rx2 frequency
    LMIC.dn2Freq    = State.V1.Rx2Frequency;

#if !defined(DISABLE_PING)
    // set the ping frequency
    LMIC.ping.freq  = State.V1.PingFrequency;
#endif

    LMIC.adrAckReq = State.V1.LinkIntegrity;
    LMIC.adrTxPow = State.V1.TxPower;
    LMIC.upRepeat = State.V1.Redundancy;
    LMIC.globalDutyRate = State.V1.DutyCycle;
    LMIC.rx1DrOffset = State.V1.Rx1DRoffset;
    LMIC.dn2Dr = State.V1.Rx2DataRate;
    LMIC.rxDelay = State.V1.RxDelay;

#if LMIC_ENABLE_TxParamSetupReq
    LMIC.txParam = State.V1.TxParam;
#endif
#if !defined(DISABLE_BEACONS)
    LMIC.bcnChnl = State.V1.BeaconChannel;
#endif
#if !defined(DISABLE_PING)
    LMIC.ping.dr = State.V1.PingDr;
#endif
    LMIC.dn2Ans = State.V1.MacRxParamAns;
    LMIC.macDlChannelAns = State.V1.MacDlChannelAns;
    LMIC.macRxTimingSetupAns = State.V1.MacRxTimingSetupAns;

#if CFG_LMIC_EU_like
    // don't turn off bits: user can't fool us here.
    // we can get the immutable channels from the
    // channelMap value after reset.
    auto const resetMap = LMIC.channelMap;
    auto const & euLike = State.V1.Channels.EUlike;
    LMIC.channelMap |= euLike.ChannelMap;
#if ARDUINO_LMIC_VERSION_COMPARE_GE(ARDUINO_LMIC_VERSION, ARDUINO_LMIC_VERSION_CALC(3,99,0,1))
    LMIC.channelShuffleMap = euLike.ChannelShuffleMap;
#endif
    for (unsigned ch = 0; ch < MAX_CHANNELS; ++ch)
        {
        if ((resetMap & (decltype(resetMap)(1) << ch)) == 0)
            {
            // copy data -- note that the saved band number is encoded
            LMIC_setupChannel(
                ch,
                euLike.getFrequency(euLike.UplinkFreq, ch),
                euLike.ChannelDrMap[ch],
                euLike.getBand(ch)
                );
#if !defined(DISABLE_MCMD_DlChannelReq)
            LMIC.channelDlFreq[ch] = euLike.getFrequency(euLike.DownlinkFreq, ch);
#endif
            }
        }

    for (unsigned band = 0; band < MAX_BANDS; ++band)
        {
        LMIC.bands[band].txcap = euLike.Bands[band].txDutyDenom;
        LMIC.bands[band].txpow = euLike.Bands[band].txPower;
        LMIC.bands[band].lastchnl = euLike.Bands[band].lastChannel;
        // Heuristic; we don't know how long has passed since we saved
        // this, because we don't currently have GPS time available.
        // Conservatively reserve time from now.
        LMIC.bands[band].avail = tNow + euLike.Bands[band].ostimeAvail;
        }

#elif CFG_LMIC_US_like
# if ARDUINO_LMIC_VERSION_COMPARE_GE(ARDUINO_LMIC_VERSION, ARDUINO_LMIC_VERSION_CALC(3,99,0,1))
    static_assert(sizeof(LMIC.channelShuffleMap) == sizeof(State.V1.Channels.USlike.ChannelShuffleMap),
        "shuffle map doesn't match");
    // copy the shuffle map bits
    memcpy(LMIC.channelShuffleMap, State.V1.Channels.USlike.ChannelShuffleMap, sizeof(LMIC.channelShuffleMap));
# endif
    // copy the enabled states
    for (unsigned ch = 0; ch < State.V1.Channels.USlike.nCh; ++ch)
        {
        const bool state = State.V1.Channels.USlike.isEnabled(ch);

        if (state)
            LMIC_enableChannel(ch);
        else
            LMIC_disableChannel(ch);
        }
#endif
    }

#undef FUNCTION
