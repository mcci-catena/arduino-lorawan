/* Arduino_LoRaWAN.h	Tue Oct 25 2016 00:10:57 tmm */

/*

Module:  Arduino_LoRaWAN.h

Function:
	The base class for arduino-lmic-based LoRaWAN nodes.

Version:
	V0.1.0	Tue Oct 25 2016 00:10:57 tmm	Edit level 1

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
   0.1.0  Tue Oct 25 2016 00:10:57  tmm
	Module created.

*/

#ifndef _ARDUINO_LORAWAN_H_		/* prevent multiple includes */
#define _ARDUINO_LORAWAN_H_

#include <stdint.h>
#include <Arduino.h>
#include <mcciadk_env.h>

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

	enum class ProvisioningStyle
		{
		kNone, kABP, kOTAA
		};

	struct AbpProvisioningInfo
		{
		uint8_t		NwkSkey[16];
		uint8_t		AppSkey[16];
		uint32_t	DevAddr;
                uint32_t        InitialSeqnoUp;
                uint32_t        InitialSeqnoDown;
		};

	struct OtaaProvisioningInfo
		{
		uint8_t		AppKey[16];
		uint8_t		DevEUI[8];
		uint8_t		AppEUI[8];
		};

	struct ProvisioningInfo
		{
		ProvisioningStyle	Style;
		AbpProvisioningInfo     AbpInfo;
		OtaaProvisioningInfo    OtaaInfo;
		};

        struct ProvisioningTable
                {
                const ProvisioningInfo  *pInfo;
                unsigned                nInfo;
                };

        /*
        || the constructor.
        */
        Arduino_LoRaWAN();

        /*
        || the begin function. Call this to start things (the constructor
        || does not!
        */
        bool begin(void);

	/*
	|| the function to call from your loop()
	*/
	void loop(void);

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

	bool GetTxReady();

        typedef void SendBufferCbFn(void *pCtx, bool fSuccess);

	bool SendBuffer(
		const uint8_t *pBuffer,
		size_t nBuffer,
                SendBufferCbFn pDoneFn = nullptr,
                void *pCtx = nullptr
		);

	bool GetDevEUI(
		uint8_t *pBuf
		);

	bool GetAppEUI(
	    uint8_t *pBuf
	    );

	bool GetAppKey(
	    uint8_t *pBuf
	    );

protected:
        // you must have a NetBegin() function or things won't work.
        virtual bool NetBegin(void) = 0;

        // you may have a NetJoin() function.
        // if not, the base function does nothing.
        virtual void NetJoin(void) 
                { /* NOTHING */ };

        // you may have a NetRxComplete() function. 
        // if not, the base function does nothing.
        virtual void NetRxComplete(void) 
                { /* NOTHING */ };

        // you may have a NetTxComplete() function. 
        // if not, the base function does nothing.
        virtual void NetTxComplete(void) 
                { /* NOTHING */ };

	virtual ProvisioningStyle GetProvisioningStyle(void)
		{
		return ProvisioningStyle::kNone;
		}
	virtual bool GetAbpProvisioningInfo(
			AbpProvisioningInfo *pProvisioningInfo
			)
		{
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
	virtual bool GetOtaaProvisioningInfo(
			OtaaProvisioningInfo *pProvisioningInfo
			)
		{
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

	bool LogVerbose()
		{
		return (this->m_ulDebugMask & LOG_VERBOSE) != 0;
		}
	
	uint32_t m_ulDebugMask;
        bool m_fTxPending;

        SendBufferCbFn *m_pSendBufferDoneFn;
        void *m_pSendBufferDoneCtx;

private:
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
	
        };

/****************************************************************************\
|
|	Eventually this will get removed for "free" builds. But if you build
|	in the Arduino enfironment, this is going to get hard to override.
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
# define ARDUINO_LORAWAN_PRINTF(a_check, a_fmt, ...)	\
    do { ; } while (0)
#endif

/**** end of Arduino_LoRaWAN.h ****/
#endif /* _ARDUINO_LORAWAN_H_ */
