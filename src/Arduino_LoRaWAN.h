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

// Arduino LoRaWAN version
#define ARDUINO_LORAWAN_VERSION_CALC(major, minor, patch, local)        \
        (((major) << 24u) | ((minor) << 16u) | ((patch) << 8u) | (local))

#define ARDUINO_LORAWAN_VERSION ARDUINO_LORAWAN_VERSION_CALC(0, 8, 0, 0)        /* v0.7.0.0 */

#define ARDUINO_LORAWAN_VERSION_GET_MAJOR(v)    \
        (((v) >> 24u) & 0xFFu)

#define ARDUINO_LORAWAN_VERSION_GET_MINOR(v)    \
        (((v) >> 16u) & 0xFFu)

#define ARDUINO_LORAWAN_VERSION_GET_PATCH(v)    \
        (((v) >> 8u) & 0xFFu)

#define ARDUINO_LORAWAN_VERSION_GET_LOCAL(v)    \
        ((v) & 0xFFu)



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
                enum eMaskKind : uint8_t { kEUlike = 0, kUSlike = 1 };

                uint8_t         Tag;
                uint8_t         Size;
                };

        struct SessionChannelMask_US_like
                {
                static constexpr uint32_t nCh = 64 + 8;

                // the fields
                SessionChannelMask_Header       Header;
                uint8_t                         ChannelMask[nCh / 8];

                // the methods
                bool isEnabled(unsigned iCh) const
                        {
                        if (iCh < nCh)
                                {
                                return this->ChannelMask[iCh / 8] & (1 << (iCh & 7));
                                }
                        else
                                return false;
                        }

                void clearAll()
                        {
                        for (auto i = 0u; i < nCh / 8; ++i)
                                this->ChannelMask[i] = 0;
                        }

                // change enable state of indicated channel
                // and return previous state.
                bool enable(unsigned iCh, bool fEnable)
                        {
                        if (iCh < nCh)
                                {
                                auto pByte = this->ChannelMask + iCh/8;
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

        struct SessionChannelMask_EU_like
                {
                static constexpr uint32_t nCh = 16;

                // the fields
                SessionChannelMask_Header       Header;
                uint8_t                         ChannelFreq[nCh * 3];

                // useful methods

                // fetch the recorded frequency of a given channel.
                uint32_t getFrequency(unsigned iCh) const
                        {
                        if (iCh > nCh)
                                return 0;
                        else
                                {
                                auto const chPtr = this->ChannelFreq + iCh * 3;
                                return (uint32_t(chPtr[0] << 16) |
                                        uint32_t(chPtr[1] << 8) |
                                        uint32_t(chPtr[2])) * 100;
                                }
                        }

                // record the frequency of a given channel.
                bool setFrequency(unsigned iCh, uint32_t frequency)
                        {
                        if (iCh > nCh)
                                return false;
                        const uint32_t reducedFreq = frequency / 100;
                        if (reducedFreq > 0xFFFFFFu)
                                return false;

                        auto const chPtr = this->ChannelFreq + iCh * 3;
                        chPtr[0] = uint8_t(reducedFreq >> 16);
                        chPtr[1] = uint8_t(reducedFreq >> 8);
                        chPtr[2] = uint8_t(reducedFreq);
                        }

                // clear all frequencies
                void clearAll()
                        {
                        std::memset(this->ChannelFreq, 0, sizeof(this->ChannelFreq));
                        }
                };

        union SessionChannelMask
                {
                SessionChannelMask_Header       Header;
                SessionChannelMask_EU_like      EUlike;
                SessionChannelMask_US_like      USlike;
                };

        // discriminate SessionInfo variants
        enum SessionInfoTag : uint8_t
                {
                kSessionInfoTag_Null = 0x00,    // indicates that there's no info.
                kSessionInfoTag_V1 = 0x01,      // indicates the V1 structure, which was written but never read.
                kSessionInfoTag_V2 = 0x02,      // indicates the V2 structure, which is first version written and read.
                };

        struct SessionInfoHeader
                {
                uint8_t Tag;                    // the discriminator
                uint8_t Size;                   // size of the overall structure
                };

        // Version 1 of session info.
        struct SessionInfoV1
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
                uint32_t        FCntUp;         // uplink frame count
                uint32_t        FCntDown;       // downlink frame count
                };

        // Version 2 of session info.
        struct SessionInfoV2
                {
                // to ensure packing, we just repeat the header.
                uint8_t         Tag;            // kSessionInfoTag_V1
                uint8_t         Size;           // sizeof(SessionInfoV1)
                uint8_t         Region;         // selected region.
                uint8_t         LinkADR;        // Current link ADR (per [1.0.2] 5.2)
                uint32_t        NetID;          // the network ID
                uint32_t        DevAddr;        // device address
                uint8_t         NwkSKey[16];    // network session key
                uint8_t         AppSKey[16];    // app session key
                uint32_t        FCntUp;         // uplink frame count
                uint32_t        FCntDown;       // downlink frame count
                SessionChannelMask Channels;    // info about the enabled
                                                // channels.
                uint16_t        Country;        // Country code
                int16_t         LinkIntegrity;  // the link-integrity counter.
                uint8_t         Redundancy;     // NbTrans (in bits 3:0)
                uint8_t         DutyCycle;      // Duty cycle (per [1.0.2] 5.3)
                // TODO(tmm@mcci.com) complete
                };


        // information about the curent session, stored persistenly if
        // possible. We allow for versioning, primarily so that (if we
        // choose) we can accommodate older versions and very simple
        // storage schemes. However, this is just future-proofing.
        union SessionInfo
                {
                // the header, same for all versions
                SessionInfoHeader       Header;

                // the V1 version was never used by MCCI, but is
                // maintained for reference, since it shipped and
                // probably has been written to NVRAM
                SessionInfoV1   V1;

                // SessionInfo::V2 is used as of June 2018 for MCCI
                // products to save session info in NVRAM. Layout is
                // extended from SessionInfo::V1.
                SessionInfoV2   V2;
                };

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
        // NetBeginRegionInit is called after LMIC reset.
        virtual void NetBeginRegionInit() = 0;


        // you may have a NetJoin() function.
        // if not, the base function does nothing.
        virtual void NetJoin(void)
                { /* NOTHING */ };

        // You may have a NetRxComplete() function; this is called
        // when receive data *may* be available.
        // If not, the base class function calls this->m_pReceiveBufferFn,
        // and then calls this->NetSaveFCntDown().
        virtual void NetRxComplete(void);

        // you may have a NetTxComplete() function.
        // if not, the base function does nothing.
        virtual void NetTxComplete(void)
                { /* NOTHING */ };

        // you should provide a function that returns the provisioning
        // style from stable storage; if you don't yet have provisioning
        // info, return ProvisioningStyle::kNone
        virtual ProvisioningStyle GetProvisioningStyle(void)
                {
                return ProvisioningStyle::kOTAA;
                }

        // you should provide a function that returns provisioning info from
        // persistent storage. Called during initialization. If this returns
        // false, OTAA will be forced. If this returns true (as it should for
        // a saved session), then a call with a non-null pointer will get teh
        // filled-in provisioning info.
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

        // you should provide a function that returns
        // OTAA provisioning info from persistent storage. Only called
        // if you return ProvisioningStyle::kOtaa to GetProvisioningStyle().
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

        // if you have persistent storage, you should provide a function
        // that gets the saved session info from persistent storage, or
        // indicate that there isn't a valid saved session. Note that
        // the saved info is opaque to the higher level. The number of
        // bytes actually stored into pSessionInfo is returned. FCntUp
        // and FCntDown are stored separately.
        virtual bool GetSavedSessionInfo(
                        SessionInfo *pSessionInfo,
                        uint8_t *pExtraSessionInfo,
                        size_t nExtraSessionInfo,
                        size_t *pnExtraSessionActual
                        )
                {
                // if not provided, default zeros buf and returns false.
                if (pExtraSessionInfo)
                        {
                        memset(pExtraSessionInfo, 0, nExtraSessionInfo);
                        }
                if (pSessionInfo)
                        {
                        memset(pSessionInfo, 0, sizeof(*pSessionInfo));
                        }
                if (pnExtraSessionActual)
                        {
                        *pnExtraSessionActual = 0;
                        }
                return false;
                }

        // if you have persistent storage, you shold provide a function that
        // saves session info to persistent storage. This will be called
        // after a successful join or a MAC message update.
        virtual void NetSaveSessionInfo(
                        const SessionInfo &SessionInfo,
                        const uint8_t *pExtraSessionInfo,
                        size_t nExtraSessionInfo
                        )
                {
                // default: do nothing.
                }

        // Save FCntUp value (the uplink frame counter) (spelling matches
        // LoRaWAN spec).
        // If you have persistent storage, you should provide this function
        virtual void NetSaveFCntUp(
                        uint32_t uFcntUp
                        )
                {
                // default: do nothing.
                }

        // save FCntDown value (the downlink frame counter) (spelling matches
        // LoRaWAN spec).  If you have persistent storage, you should provide
        // this function.
        virtual void NetSaveFCntDown(
                        uint32_t uFcntDown
                        )
                {
                // default: do nothing.
                }

        // return true if verbose logging is enabled.
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

        // since the LMIC code is not really obvious as to which events
        // update the downlink count, we simply watch for changes.
        uint32_t m_savedFCntDown;

        void UpdateFCntDown(uint32_t newFCntDown)
                {
                if (this->m_savedFCntDown != newFCntDown)
                        {
                        this->m_savedFCntDown = newFCntDown;
                        this->NetSaveFCntDown(newFCntDown);
                        }
                }
        };

/****************************************************************************\
|
|       Eventually this will get removed for "free" builds. But if you build
|       in the Arduino enfironment, this is going to get hard to override.
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
