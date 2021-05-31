/*

Module:	arduino_lorawan_cEventLog.cpp

Function:
	Arduino_LoRaWAN::cEventLog methods.

Copyright and License:
	This file copyright (C) 2021 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	See accompanying LICENSE file for copyright and license information.

Author:
	Terry Moore, MCCI Corporation	May 2021

*/

#include <Arduino_LoRaWAN_EventLog.h>
#include <arduino_lmic.h>

/****************************************************************************\
|
|	Log methods
|
\****************************************************************************/

void
Arduino_LoRaWAN::cEventLog::setup()
    {
    // no setup needed.
    }

void
Arduino_LoRaWAN::cEventLog::loop()
    {
    if ((LMIC.opmode & OP_TXRXPEND) != 0)
        return;

    if (os_queryTimeCriticalJobs(ms2osticks(1000)))
        return;

    this->processSingleEvent();
    }

Arduino_LoRaWAN::cEventLog::EventNode_t *
Arduino_LoRaWAN::cEventLog::logEvent(
    void *pClientData,
    std::uintptr_t arg1,
    std::uintptr_t arg2,
    std::uintptr_t arg3,
    LogCallback_t *pFn
    )
    {
    auto i = this->m_tail + 1;

    if (i == sizeof(this->m_queue) / sizeof(this->m_queue[0]))
        {
        i = 0;
        }
    if (i == this->m_head)
        {
        // indicate failure. Callback won't be called!
        return nullptr;
        }

    auto const pEvent = &this->m_queue[this->m_tail];

    // save log data
    pEvent->time = os_getTime();
    pEvent->pCallBack = pFn;
    pEvent->pClientData = pClientData;
    pEvent->data[0] = arg1;
    pEvent->data[1] = arg2;
    pEvent->data[2] = arg3;

    // advance pointer
    this->m_tail = i;

    // indicate success (if anyone cares)
    return pEvent;
    }

bool
Arduino_LoRaWAN::cEventLog::processSingleEvent()
    {
    if (this->m_head == this->m_tail)
        {
        return false;
        }

    auto const pEvent = &this->m_queue[this->m_head];

    Serial.print(osticks2ms(pEvent->time));
    Serial.print(" ms:");
    pEvent->pCallBack(pEvent);
    Serial.println();

    if (++m_head == sizeof(m_queue) / sizeof(m_queue[0]))
        {
        m_head = 0;
        }
    }

void
Arduino_LoRaWAN::cEventLog::printCh(std::uint8_t channel) const
    {
    Serial.print(F(" ch="));
    Serial.print(std::uint32_t(channel));
    }

const char *
Arduino_LoRaWAN::cEventLog::getSfName(std::uint8_t rps) const
    {
    const char * const t[] = { "FSK", "SF7", "SF8", "SF9", "SF10", "SF11", "SF12", "SFrfu" };
    return t[getSf(rps)];
    }

const char *
Arduino_LoRaWAN::cEventLog::getBwName(std::uint8_t rps) const
    {
    const char * const t[] = { "BW125", "BW250", "BW500", "BWrfu" };
    return t[getBw(rps)];
    }

const char *
Arduino_LoRaWAN::cEventLog::getCrName(std::uint8_t rps) const
    {
    const char * const t[] = { "CR 4/5", "CR 4/6", "CR 4/7", "CR 4/8" };
    return t[getCr(rps)];
    }

const char *
Arduino_LoRaWAN::cEventLog::getCrcName(std::uint8_t rps) const
    {
    return getNocrc(rps) ? "NoCrc" : "Crc";
    }

void
Arduino_LoRaWAN::cEventLog::printHex2(unsigned v) const
    {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
    }

void
Arduino_LoRaWAN::cEventLog::printHex4(unsigned v) const
    {
    printHex2(v >> 8u);
    printHex2(v);
    }

void
Arduino_LoRaWAN::cEventLog::printSpace(void) const
    {
    Serial.print(' ');
    }

void
Arduino_LoRaWAN::cEventLog::printFreq(u4_t freq) const
    {
    Serial.print(F(": freq="));
    Serial.print(freq / 1000000);
    Serial.print('.');
    Serial.print((freq % 1000000) / 100000);
    }

void Arduino_LoRaWAN::cEventLog::printRps(std::uint8_t rps) const
    {
    Serial.print(F(" rps=0x")); printHex2(rps);
    Serial.print(F(" (")); Serial.print(getSfName(rps));
    printSpace(); Serial.print(getBwName(rps));
    printSpace(); Serial.print(getCrName(rps));
    printSpace(); Serial.print(getCrcName(rps));
    Serial.print(F(" IH=")); Serial.print(unsigned(getIh(rps)));
    Serial.print(')');
    }
