/*

Module:  Arduino_LoRaWAN.h

Function:
        The base class for arduino-lmic-based LoRaWAN nodes.

Copyright notice:
        See LICENSE file accompanying this project.

Author:
        Terry Moore, MCCI Corporation   October 2016

*/

#ifndef _ARDUINO_LORAWAN_H_             /* prevent multiple includes */
#define _ARDUINO_LORAWAN_H_

#include <cstdint>
#include <Arduino.h>
#ifndef _MCCIADK_ENV_H_
# include <mcciadk_env.h>
#endif

#include <cstring>
#include <arduino_lmic_hal_configuration.h>

/// \brief construct Arduino LoRaWAN semantic version
#define ARDUINO_LORAWAN_VERSION_CALC(major, minor, patch, local)        \
        (((major) << 24u) | ((minor) << 16u) | ((patch) << 8u) | (local))

/// \brief library semantic version
/// \note local "0" is *greater than* any local non-zero; use
///     \ref ARDUINO_LORAWAN_VERSION_COMPARE_LT() to compare relative versions.
///
#define ARDUINO_LORAWAN_VERSION \
        ARDUINO_LORAWAN_VERSION_CALC(0, 9, 1, 0) /* v0.9.1 */

#define ARDUINO_LORAWAN_VERSION_GET_MAJOR(v)    \
        (((v) >> 24u) & 0xFFu)

#define ARDUINO_LORAWAN_VERSION_GET_MINOR(v)    \
        (((v) >> 16u) & 0xFFu)

#define ARDUINO_LORAWAN_VERSION_GET_PATCH(v)    \
        (((v) >> 8u) & 0xFFu)

#define ARDUINO_LORAWAN_VERSION_GET_LOCAL(v)    \
        ((v) & 0xFFu)

/// \brief convert a semantic version to an integer.
#define ARDUINO_LORAWAN_VERSION_TO_INT(v)       \
        (((v) & 0xFFFFFF00u) | (((v) - 1) & 0xFFu))

/// \brief compare two semantic versions
/// \return \c true if \b a is less than \b b (as a semantic version).
#define ARDUINO_LORAWAN_VERSION_COMPARE_LT(a, b)   \
        (ARDUINO_LORAWAN_VERSION_TO_INT(a) < ARDUINO_LORAWAN_VERSION_TO_INT(b))

/// \brief compare two semantic versions
/// \return \c true if \b a is greater than \b b (as a semantic version).
#define ARDUINO_LORAWAN_VERSION_COMPARE_GT(a, b)   \
        (ARDUINO_LORAWAN_VERSION_TO_INT(a) > ARDUINO_LORAWAN_VERSION_TO_INT(b))

class Arduino_LoRaWAN;

/*
|| You can use this for declaring event functions...
|| or use a lambda if you're bold; but remember, no
|| captures in that case.  Yes, we're *not* using one
|| of the several thousand C++ ways of doing this;
|| we're using C-compatible callbacks.
*/
MCCIADK_BEGIN_DECLS
typedef void ARDUINO_LORAWAN_EVENT_FN(void *, uint32_t);
MCCIADK_END_DECLS

namespace Arduino_LMIC {
        // return the country code
        constexpr uint16_t kCountryCode(char c1, char c2)
                {
                return (c1 < 'A' || c1 > 'Z' || c2 < 'A' || c2 > 'Z')
                        ? 0
                        : ((c1 << 8) | c1);
                }
} // namespace Arduino_LMIC

class Arduino_LoRaWAN
        {
public:
        /*
        || the internal LMIC wrapper
        */
        class cLMIC; /* forward reference, see Arduino_LoRaWAN_lmic.h */

        /*
        || the event logger
        */
        class cEventLog; /* forward reference, see Arduino_LoRaWAN_EventLog.h */

        /*
        || debug things
        */
        enum {
             LOG_BASIC = 1 << 0,
             LOG_ERRORS = 1 << 1,
             LOG_VERBOSE = 1 << 2,
             };

        // We must replicate the C structure from
        // "lmic.h" inside the class. Otherwise we'd
        // need to have all of lmic.h in scope everywhere,
        // which could cause naming clashes.
        using lmic_pinmap = Arduino_LMIC::HalPinmap_t;

        // the networks that we support
        enum class NetworkID_t : std::uint32_t
                {
                TheThingsNetwork,
                Actility,
                Helium,
                machineQ,
                Senet,
                Senra,
                Swisscom,
                ChirpStack,
                Generic
                };

        // change network code to text
        static constexpr const char * NetworkID_t_GetName(NetworkID_t net)
                {
                return  (net == NetworkID_t::TheThingsNetwork) ?    "The Things Network" :
                        (net == NetworkID_t::Actility) ?            "Actility" :
                        (net == NetworkID_t::Helium)   ?            "Helium" :
                        (net == NetworkID_t::machineQ) ?            "machineQ" :
                        (net == NetworkID_t::Senet)    ?            "Senet" :
                        (net == NetworkID_t::Senra)    ?            "Senra" :
                        (net == NetworkID_t::Swisscom) ?            "Swisscom" :
                        (net == NetworkID_t::ChirpStack) ?          "ChirpStack" :
                        (net == NetworkID_t::Generic)  ?            "Generic" :
                        "<<unknown network>>"
                        ;
                }

        /*
        || provisioning things:
        */

        // the provisioning styles.
        enum class ProvisioningStyle
                {
                kNone, kABP, kOTAA
                };

        // information provided for ABP provisioning
        struct AbpProvisioningInfo
                {
                uint8_t         NwkSKey[16];
                uint8_t         AppSKey[16];
                uint32_t        DevAddr;
                uint32_t        NetID;
                uint32_t        FCntUp;
                uint32_t        FCntDown;
                };

        // information provided for OTAA provisioning
        struct OtaaProvisioningInfo
                {
                uint8_t         AppKey[16];
                uint8_t         DevEUI[8];
                uint8_t         AppEUI[8];
                };

        // the provisioning blob.
        struct ProvisioningInfo
                {
                ProvisioningStyle       Style;
                AbpProvisioningInfo     AbpInfo;
                OtaaProvisioningInfo    OtaaInfo;
                };

        struct ProvisioningTable
                {
                const ProvisioningInfo  *pInfo;
                unsigned                nInfo;
                };

        // US-like regions use a 72-bit mask of enabled channels.
        // CN474-like regions use a 96-bit mask of enabled channels.
        // EU-like regions use a a table of 16 frequencies with
        //    100-Hz resolution (at 24 bits, that's 48 bytes)
        //    In this encoding, we use zeros to represent disabled channels

        struct SessionChannelMask_Header
                {
                enum eMaskKind : uint8_t { kEUlike = 0, kUSlike = 1, kCNlike = 2 };

                uint8_t         Tag;    ///< discriminator, eMaskKind.
                uint8_t         Size;   ///< size of actual SessionChannelMask, in bytes
                };

        template <uint32_t a_nCh>
        struct SessionChannelMask_US_like
                {
                static constexpr uint32_t nCh = a_nCh;
                static_assert(
                        1 <= a_nCh && a_nCh <= 96,
                        "number of channels must be in [1..96]"
                        );

                // the fields
                SessionChannelMask_Header Header;                                       ///< the common header
                uint8_t                   ChannelMap[2 * ((nCh + 15) / 16)];            ///< the channel enable mask
                uint8_t                   ChannelShuffleMap[2 * ((nCh + 15) / 16)];     ///< the channel shuffle mask;
                                                                                        ///  a multiple of 16 bits to match
                                                                                        ///  the LMIC.

                // the methods
                bool isEnabled(unsigned iCh) const
                        {
                        if (iCh < nCh)
                                {
                                return this->ChannelMap[iCh / 8] & (1 << (iCh & 7));
                                }
                        else
                                return false;
                        }

                void clearAll()
                        {
                        for (auto i = 0u; i < (nCh + 7) / 8; ++i)
                                {
                                this->ChannelMap[i] = 0;
                                this->ChannelShuffleMap[i] = 0;
                                }
                        }

                // change enable state of indicated channel
                // and return previous state.
                bool enable(unsigned iCh, bool fEnable)
                        {
                        if (iCh < nCh)
                                {
                                auto pByte = this->ChannelMap + iCh/8;
                                uint8_t mask = 1 << (iCh & 7);
                                auto v = *pByte;
                                bool fResult = (v & mask) != 0;

                                if (fEnable)
                                        *pByte = uint8_t(v | mask);
                                else
                                        *pByte = uint8_t(v & ~mask);

                                return fResult;
                                }
                        else
                                return false;
                        }
                };

        ///
        /// \brief "band" data for rate limiting in an EU-like configuration
        ///
        struct SessionChannelBand
                {
                // the fields
                uint16_t        txDutyDenom;    ///< duty cycle limitation: 1/txDutyDenom
                uint8_t         txPower;        ///< maximum TX power, this band
                uint8_t         lastChannel;    ///< last used channel in this band
                uint32_t        ostimeAvail;    ///< when will it be available relative to
                                                ///  GPStime in ostime_t ticks.
                };

        ///
        /// \brief session data for channels in an EU-like configuration
        ///
        /// \param a_nCh specifies the number of channels this class instance
        ///     should handle. Must be in 1..16.
        ///
        /// \param a_nBands specifies the maximum number of bands to be supported.
        ///
        template <uint32_t a_nCh = 16, uint32_t a_nBands = 4>
        struct SessionChannelMask_EU_like
                {
                /// \brief number of channels declared for this class instance
                static constexpr uint32_t nCh = a_nCh;
                static_assert(
                        1 <= a_nCh && a_nCh <= 16,
                        "number of channels must be in [1..16]"
                        );
                /// \brief number of bands declared for this class instance
                static constexpr uint32_t nBands = a_nBands;
                static_assert(
                        1 <= a_nBands && a_nBands <= 4,
                        "number of channels must be in [1..4]"
                        );

                // the fields
                SessionChannelMask_Header       Header;                         ///< the header
                uint32_t                        ChannelBands;                   ///< band number for each channel
                uint16_t                        ChannelMap;                     ///< mask of enabled channels, one bit per channel
                uint16_t                        ChannelShuffleMap;              ///< the channel re-use shuffle bitmap
                uint16_t                        ChannelDrMap[nCh];              ///< data rates for each channel
                uint8_t                         UplinkFreq[nCh * 3];            ///< packed downlink frequencies for each channel (100 Hz units)
                uint8_t                         DownlinkFreq[nCh * 3];          ///< packed uplink frequencies for each channel.
                SessionChannelBand              Bands[nBands];                  ///< band limiting info

                // useful methods
                ///
                /// \brief set the band number for a given channel
                ///
                /// \param [in] ch channel
                /// \param [in] band band (in 0..3)
                ///
                void setBand(unsigned ch, unsigned band)
                        {
                        uint32_t mask = 0x3 << (2 * ch);
                        uint32_t v = band << (2 * ch);

                        this->ChannelBands ^= (this->ChannelBands ^ v) & mask;
                        }

                ///
                /// \brief get the band for a given channel
                ///
                /// \param [in] ch channel
                ///
                /// \return band number, in 0..3.
                ///
                uint32_t getBand(unsigned ch) const
                        {
                        return (this->ChannelBands >> (2 * ch)) & 0x3u;
                        }

                /// \brief return the recorded frequency of a given channel in Hz from a given table
                uint32_t getFrequency(const uint8_t (&freq)[nCh * 3], unsigned iCh) const
                        {
                        if (iCh > nCh)
                                return 0;
                        else
                                {
                                auto const chPtr = freq + iCh * 3;
                                return (uint32_t(chPtr[0] << 16) |
                                        uint32_t(chPtr[1] << 8) |
                                        uint32_t(chPtr[2])) * 100;
                                }
                        }

                ///
                /// \brief record the frequency of a given channel.
                /// \param [in] freq packed table of frequencies
                /// \param [in] iCh channel index
                /// \param [in] freq frequency in Hertz
                ///
                /// \return true if channel number and frequency were valid,
                ///     false otherwise.
                ///
                bool setFrequency(uint8_t (&freq)[nCh * 3], unsigned iCh, uint32_t frequency)
                        {
                        if (iCh > nCh)
                                return false;
                        const uint32_t reducedFreq = frequency / 100;
                        if (reducedFreq > 0xFFFFFFu)
                                return false;

                        auto const chPtr = freq + iCh * 3;
                        chPtr[0] = uint8_t(reducedFreq >> 16);
                        chPtr[1] = uint8_t(reducedFreq >> 8);
                        chPtr[2] = uint8_t(reducedFreq);
                        }

                /// \brief clear all entries in the channel table.
                void clearAll()
                        {
                        this->ChannelMap = 0;
                        this->ChannelShuffleMap = 0;
                        std::memset(this->ChannelDrMap, 0, sizeof(this->ChannelDrMap));
                        std::memset(this->UplinkFreq, 0, sizeof(this->UplinkFreq));
                        std::memset(this->DownlinkFreq, 0, sizeof(this->DownlinkFreq));
                        }
                };

        typedef union SessionChannelMask_u
                {
                SessionChannelMask_Header               Header;
                SessionChannelMask_EU_like<16>          EUlike;
                SessionChannelMask_US_like<64 + 8>      USlike;
                SessionChannelMask_US_like<96>          CNlike;
                } SessionChannelMask;

        /// \brief discriminate SessionInfo variants
        enum SessionInfoTag : uint8_t
                {
                kSessionInfoTag_Null = 0x00,    ///< indicates that there's no info.
                kSessionInfoTag_V1 = 0x01,      ///< indicates the V1 structure
                kSessionInfoTag_V2 = 0x02,      ///< indicates the V1 structure
                };

        /// \brief Header for SessionInfo; allows versioning.
        struct SessionInfoHeader
                {
                SessionInfoTag Tag;             // the discriminator
                uint8_t Size;                   // size of the overall structure
                };

        /// \brief Version 1 of session info.
        /// We send this up after a join.
        struct SessionInfoV1
                {
                // to ensure packing, we just repeat the header.
                SessionInfoTag  Tag;            // kSessionInfoTag_V1
                uint8_t         Size;           // sizeof(SessionInfoV1)
                uint8_t         Rsv2;           // reserved
                uint8_t         Rsv3;           // reserved
                uint32_t        NetID;          // the network ID
                uint32_t        DevAddr;        // device address
                uint8_t         NwkSKey[16];    // network session key
                uint8_t         AppSKey[16];    // app session key
                uint32_t        FCntUp;         // uplink frame count
                uint32_t        FCntDown;       // downlink frame count
                };

        /// \brief Version 2 of session info; used in conjunction with.
        /// `SessionState`. Omits duplicated info.
        struct SessionInfoV2
                {
                // to ensure packing, we just repeat the header.
                uint8_t         Tag;            // kSessionInfoTag_V1
                uint8_t         Size;           // sizeof(SessionInfoV1)
                uint8_t         Rsv2;           // reserved
                uint8_t         Rsv3;           // reserved
                uint32_t        NetID;          // the network ID
                uint32_t        DevAddr;        // device address
                uint8_t         NwkSKey[16];    // network session key
                uint8_t         AppSKey[16];    // app session key
                };

        /// \brief information about the current session.
        ///
        /// \details
        /// This structure is stored persistently if
        /// possible, and represents the result of a join. We allow for
        /// versioning, primarily so that (if we
        /// choose) we can accommodate older versions and very simple
        /// storage schemes.
        ///
        /// Older versions of Arduino_LoRaWAN sent version 1 at join,
        /// including the frame counts. Newer versions send version 2
        /// at join, followed by a SessionState message (which includes
        /// the frame counts).
        ///
        /// \see SessionState
        ///
        typedef union SessionInfo_u
                {
                /// the header, same for all versions
                SessionInfoHeader       Header;

                /// the V1 form used through v0.8 of the Arduino_LoraWAN.
                SessionInfoV1   V1;

                /// SessionInfo::V2 is used as v0.9 of the Arduino_LoRaWAN,
                /// in conjunction with the SessionState message
                SessionInfoV2   V2;
                } SessionInfo;


        /// \brief discriminate SessionState variants
        enum SessionStateTag : uint8_t
                {
                kSessionStateTag_Null = 0x00,   ///< indicates that there's no info.
                kSessionStateTag_V1 = 0x01,     ///< indicates the V1 structure
                };

        ///
        /// \brief Session state information
        ///
        /// \details
        /// Arduino_LoRaWAN sends this whenever the underlying state might
        /// have changed (in particular, after each EV_TXCOMPLETE). The client
        /// may store this in non-volatile storage in order to allow for
        /// power interruption. This is not necessarily a complete image of
        /// MAC state, but is reasonably comprehensive.
        ///
        /// This should be taken as an opaque blob by most clients.
        ///
        struct SessionStateHeader
                {
                SessionStateTag Tag;            ///< versioning info
                uint8_t         Size;           ///< size of embedded data
                };

        ///
        /// \brief the first version of SessionState
        ///
        struct SessionStateV1
                {
                // to ensure packing, we just repeat the header.
                SessionStateTag Tag;            ///< kSessionStateTag_V1
                uint8_t         Size;           ///< sizeof(SessionStateV1)
                uint8_t         Region;         ///< selected region.
                uint8_t         LinkDR;         ///< Current link DR (per [1.0.2] 5.2)
                // above 4 entries make one uint32_t.

                // keep uint32_t values together for alignment
                uint32_t        FCntUp;         ///< uplink frame count
                uint32_t        FCntDown;       ///< downlink frame count
                uint32_t        gpsTime;        ///< if non-zero, "as-of" time.
                uint32_t        globalAvail;    ///< osticks to global avail time.
                uint32_t        Rx2Frequency;   ///< RX2 Frequency (in Hz)
                uint32_t        PingFrequency;  ///< class B: ping frequency

                // next, the uint16_t values, again for alignment
                uint16_t        Country;        ///< Country code
                int16_t         LinkIntegrity;  ///< the link-integrity counter.

                // finally, the uint8_t values
                uint8_t         TxPower;        ///< Current TX power (per LinkADR)
                uint8_t         Redundancy;     ///< NbTrans (in bits 3:0)
                uint8_t         DutyCycle;      ///< Duty cycle (per [1.0.2] 5.3)
                uint8_t         Rx1DRoffset;    ///< RX1 datarate offset

                uint8_t         Rx2DataRate;    ///< RX2 data rate
                uint8_t         RxDelay;        ///< RX window delay
                uint8_t         TxParam;        ///< saved TX param
                uint8_t         BeaconChannel;  ///< class B: beacon channel.

                uint8_t         PingDr;         ///< class B: ping datarate
                uint8_t         MacRxParamAns;  ///< saved LMIC.dn2Ans
                uint8_t         MacDlChannelAns;///< saved LMIC.macDlChannelAns
                uint8_t         MacRxTimingSetupAns;    ///< saved LMIC.macRxTimingSetupAns;

                // at the very end
                SessionChannelMask Channels;    ///< info about the enabled channels
                };

        static_assert(sizeof(SessionStateV1) < 256, "SessionStateV1 is too large");

        typedef union SessionState_u
                {
                SessionStateHeader      Header;
                SessionStateV1          V1;
                bool isValid() const;
                } SessionState;

        /*
        || the constructor.
        */
        Arduino_LoRaWAN();

        /*
        || the begin function. Call this to start things -- the constructor
        || does not!
        */
        bool begin(const Arduino_LMIC::HalPinmap_t *pPinmap);
        bool begin(const Arduino_LMIC::HalPinmap_t &pinmap) { return this->begin(&pinmap); };
        bool begin(void);

        /*
        || the function to call from your loop()
        */
        void loop(void);

        /*
        || Reset the LMIC
        */
        void Reset(void);

        /*
        || Shutdown the LMIC
        */
        void Shutdown(void);

        /*
        || Registering listeners... returns true for
        || success.
        */
        bool RegisterListener(ARDUINO_LORAWAN_EVENT_FN *, void *);

        /*
        || Dispatch an event to all listeners
        */
        void DispatchEvent(uint32_t);

        uint32_t GetDebugMask() { return this->m_ulDebugMask; }
        uint32_t SetDebugMask(uint32_t ulNewMask)
                {
                const uint32_t ulOldMask = this->m_ulDebugMask;

                this->m_ulDebugMask = ulNewMask;

                return ulOldMask;
                }

        void LogPrintf(
                const char *fmt,
                ...
                ) __attribute__((__format__(__printf__, 2, 3)));
                /* format counts start with 2 for non-static C++ member fns */


        /*
        || we only support a single instance, but we don't name it. During
        || begin processing, we register, then we can find it.
        */
        static Arduino_LoRaWAN *GetInstance()
                {
                return Arduino_LoRaWAN::pLoRaWAN;
                }

        // return the region string to the buffer
        const char *GetRegionString(char *pBuf, size_t sizeBuf) const;

        // return the region code
        enum class Region : uint8_t
                {
                unknown = 0,
                eu868 = 1,
                us915,
                cn783,
                eu433,
                au915,
                cn490,
                as923,
                kr920,
                in866,
                };
        Region GetRegion(void) const;

        enum class CountryCode : uint16_t
                {
                none = 0,
                JP = Arduino_LMIC::kCountryCode('J', 'P'),
                };
        CountryCode GetCountryCode() const;

        virtual NetworkID_t GetNetworkID() const = 0;
        virtual const char *GetNetworkName() const = 0;

        bool GetTxReady() const;

        typedef void SendBufferCbFn(void *pCtx, bool fSuccess);

        bool SendBuffer(
                const uint8_t *pBuffer,
                size_t nBuffer,
                SendBufferCbFn *pDoneFn = nullptr,
                void *pCtx = nullptr,
                bool fConfirmed = false,
                uint8_t port = 1
                );

        typedef void ReceivePortBufferCbFn(
                void *pCtx,
                uint8_t uPort,
                const uint8_t *pBuffer,
                size_t nBuffer
                );

        void SetReceiveBufferBufferCb(
                ReceivePortBufferCbFn *pReceivePortBufferFn,
                void *pCtx = nullptr
                )
                {
                this->m_pReceiveBufferFn = pReceivePortBufferFn;
                this->m_pReceiveBufferCtx = pCtx;
                }

        bool GetDevEUI(
                uint8_t *pBuf
                );

        bool GetAppEUI(
            uint8_t *pBuf
            );

        bool GetAppKey(
            uint8_t *pBuf
            );

        bool IsValidState(const SessionState &state) const;

        // return true iff network seems to be provisioned.  Make
        // it virtual so it can be overridden if needed.
        virtual bool IsProvisioned(void)
                {
                switch (this->GetProvisioningStyle())
                        {
                case ProvisioningStyle::kABP:
                        return this->GetAbpProvisioningInfo(nullptr);
                case ProvisioningStyle::kOTAA:
                        return this->GetOtaaProvisioningInfo(nullptr);
                case ProvisioningStyle::kNone:
                default:
                        return false;
                        }
                }

        // Enable (or disable) link-check mode, which generates uplink ADR
        // requests and causes automatic rejoin if the network seems not
        // to be responding.  Without this, downlink ADR settings are
        // honored, but the device will never try to rejoin.
        bool SetLinkCheckMode(bool fEnable);

        // Data about the currently pending transmit.
        struct SendBufferData_t
                {
                Arduino_LoRaWAN *pSelf;
                SendBufferCbFn *pDoneFn;
                void *pDoneCtx;
                bool fTxPending;
                };

protected:
        /// \brief client-provided method for region initialization.
        ///
        /// \details
        /// NetBeginRegionInit is called after LMIC reset.
        /// The client must provide this. Normally it's provided by
        /// the network-specific derived class.
        ///
        virtual void NetBeginRegionInit() = 0;

        /// \brief notify client about network join.
        ///
        /// \details
        /// you may have a \c NetJoin() function.
        /// if not, the base function does nothing.
        virtual void NetJoin(void)
                { /* NOTHING */ };

        /// \brief request client to check whether RX data is available.
        ///
        /// \details
        /// You may have a \c NetRxComplete() function; this is called
        /// when receive data *may* be available.
        /// If not, the base class function calls \c this->m_pReceiveBufferFn
        /// if and only if receive data actually is available.
        ///
        virtual void NetRxComplete(void);

        /// \brief notify client that transmission has completed.
        /// you may have a NetTxComplete() function.
        /// if not, the base function does nothing.
        virtual void NetTxComplete(void)
                { /* NOTHING */ };

        /// \brief return the configured provisioning style.
        ///
        /// you should provide a function that returns the provisioning
        /// style from stable storage; if you don't yet have provisioning
        /// info, return ProvisioningStyle::kNone
        virtual ProvisioningStyle GetProvisioningStyle(void)
                {
                return ProvisioningStyle::kOTAA;
                }

        /// \brief return ABP provisioning info.
        ///
        /// Your sketch (or something outside the Arduino_LoRaWAN library)
        /// should provide a function that returns provisioning info from
        /// persistent storage. Called during initialization. If this returns
        /// false, OTAA will be forced. If this returns true (as it should for
        /// a saved session), then a call with a non-null pointer will get the
        /// filled-in provisioning info.
        virtual bool GetAbpProvisioningInfo(
                        AbpProvisioningInfo *pProvisioningInfo
                        )
                {
                // if not provided, default zeros buf and returns false.
                if (pProvisioningInfo)
                    {
                    memset(
                        pProvisioningInfo,
                        0,
                        sizeof(*pProvisioningInfo)
                        );
                    }
                return false;
                }

        /// \brief return OTAA provisioning info.
        ///
        /// you should provide a function that returns
        /// OTAA provisioning info from persistent storage. Only called
        /// if you return ProvisioningStyle::kOtaa to GetProvisioningStyle().
        ///
        virtual bool GetOtaaProvisioningInfo(
                        OtaaProvisioningInfo *pProvisioningInfo
                        )
                {
                // if not provided, default zeros buf and returns false.
                if (pProvisioningInfo)
                    {
                    memset(
                        pProvisioningInfo,
                        0,
                        sizeof(*pProvisioningInfo)
                        );
                    }
                return false;
                }

        /// \brief Return saved session info (keys)
        ///
        /// if you have persistent storage, you should provide a function
        /// that gets the saved session info from persistent storage, or
        /// indicate that there isn't a valid saved session. Note that
        /// the saved info is opaque to the higher level.
        ///
        /// \return true if \p sessionInfo was filled in, false otherwise.
        ///
        virtual bool GetSavedSessionInfo(
                        SessionInfo &sessionInfo,
                        uint8_t *pExtraSessionInfo,
                        size_t nExtraSessionInfo,
                        size_t *pnExtraSessionActual
                        )
                {
                // if not provided, default zeros buf and returns false.
                memset(&sessionInfo, 0, sizeof(sessionInfo));
                if (pExtraSessionInfo)
                        {
                        memset(pExtraSessionInfo, 0, nExtraSessionInfo);
                        }
                if (pnExtraSessionActual)
                        {
                        *pnExtraSessionActual = 0;
                        }
                return false;
                }

        /// \brief save session info (after join)
        ///
        /// \details
        /// if you have persistent storage, you should provide a function that
        /// saves session info to persistent storage. This will be called
        /// after a successful join.
        ///
        virtual void NetSaveSessionInfo(
                        const SessionInfo &SessionInfo,
                        const uint8_t *pExtraSessionInfo,
                        size_t nExtraSessionInfo
                        )
                {
                // default: do nothing.
                }

        /// \brief get the session state
        ///
        /// \return true if a valid session state was found.
        virtual bool NetGetSessionState(
                SessionState &State
                )
                {
                // default: not implemented.
                return false;
                }

        /// \brief save the session state
        ///
        /// If not provided, the default does nothing.
        ///
        virtual void NetSaveSessionState(
                const SessionState &State
                )
                {
                // default: do nothing.
                }

        /// \brief return true if verbose logging is enabled.
        bool LogVerbose()
                {
                return (this->m_ulDebugMask & LOG_VERBOSE) != 0;
                }

        uint32_t m_ulDebugMask;

private:
        SendBufferData_t m_SendBufferData;

        ReceivePortBufferCbFn *m_pReceiveBufferFn;
        void *m_pReceiveBufferCtx;

        // this is a 'global' -- it gives us a way to bootstrap
        // back into C++ from the LMIC code.
        static Arduino_LoRaWAN *pLoRaWAN;

        void StandardEventProcessor(
            uint32_t ev
            );

        struct Listener
                {
                ARDUINO_LORAWAN_EVENT_FN *pEventFn;
                void *pContext;

                void ReportEvent(uint32_t ev)
                        {
                        this->pEventFn(this->pContext, ev);
                        }
                };

        Listener m_RegisteredListeners[4];
        uint32_t m_nRegisteredListeners;

        ///
        /// \brief Update the downlink frame counter.
        /// \param [in] newFCntDown the most recently observed downlink counter.
        ///
        /// \details
        /// To save effort for the client, we want to avoid upcalls for changes
        /// in the downlink count unless it really seems to have changed.
        /// Since the LMIC code is not really obvious as to which events
        /// update the downlink count, we simply call this on every event, and
        /// watch for changes.
        ///
        void UpdateFCntDown(uint32_t newFCntDown)
                {
                if (this->m_savedSessionState.Header.Tag == kSessionStateTag_V1 &&
                    this->m_savedSessionState.V1.FCntDown == newFCntDown)
                        return;

                this->SaveSessionState();
                }

        /// \brief Internal routine for saving state after join
        void SaveSessionInfo();

        /// \brief Internal routine to save session state as appropriate
        void SaveSessionState();

        ///
        /// \brief build session state object
        ///
        /// \param [in] State reference to the session state object to be initialized.
        ///
        void BuildSessionState(SessionState &State) const;

        ///
        /// \brief apply session state data to current LMIC session
        ///
        /// \param [in] State
        bool ApplySessionState(const SessionState &State);

        ///
        /// \brief get session state and apply to the LMIC
        ///
        /// \return true if valid session state was found and applied.
        ///
        bool RestoreSessionState();

        /// \brief the internal copy of the session state, used to
        ///     reduce the number of saves to a minimum. It's initially
        ///     marked as "not valid".
        SessionState m_savedSessionState { .Header = { .Tag = kSessionStateTag_Null } };
        };

/****************************************************************************\
|
|       Eventually this will get removed for "free" builds. But if you build
|       in the Arduino environment, this is going to get hard to override.
|
\****************************************************************************/

#if MCCIADK_DEBUG || 1
# define ARDUINO_LORAWAN_PRINTF(a_check, a_fmt, ...)    \
    do  {                                               \
        if (this->a_check())                            \
                {                                       \
                this->LogPrintf(a_fmt, ## __VA_ARGS__); \
                }                                       \
        } while (0)
#else
# define ARDUINO_LORAWAN_PRINTF(a_check, a_fmt, ...)    \
    do { ; } while (0)
#endif

/**** end of Arduino_LoRaWAN.h ****/
#endif /* _ARDUINO_LORAWAN_H_ */
