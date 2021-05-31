/*

Module:	Arduino_LoRaWAN_EventLog.h

Function:
	EventLog for Arduino_LoRaWAN

Copyright and License:
	This file copyright (C) 2021 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	See accompanying LICENSE file for copyright and license information.

Author:
	Terry Moore, MCCI Corporation	May 2021

*/

#ifndef _Arduino_LoRaWAN_EventLog_h_
#define _Arduino_LoRaWAN_EventLog_h_	/* prevent multiple includes */

#pragma once

#include <Arduino_LoRaWAN.h>
#include <cstdint>

/****************************************************************************\
|
|	The event log object
|
\****************************************************************************/

class Arduino_LoRaWAN::cEventLog
    {
public:
    cEventLog() {};
    ~cEventLog() {};

    /// \brief do EventLog processing for Arduino \c setup().
    void setup();

    /// \brief do EventLog processig for Arduino \c loop().
    void loop();

    class EventNode_t;

    /// \brief abstract type for logging callbacks.
    typedef void (LogCallback_t)(const EventNode_t *);

    ///
    /// \brief LoRaWAN log entry
    ///
    /// \details generally, we try to hide the details from the deloggers;
    ///     so the callback functions have to use the methods to get at the
    ///     contents of this object.
    ///
    class EventNode_t
        {
        // allow cEventLog methods access to private fields.
        friend class cEventLog;

    private:
        std::uint32_t time;             ///< timestamp for event -- really an ostime_t, but
                                        ///  don't want to have the LMIC in scope.
        LogCallback_t *pCallBack;       ///< callback function for delogging.
        void *pClientData;              ///< client data for callback function
        std::uintptr_t data[3];         ///< arbitrary data

    public:
        /// \brief the constructor
        EventNode_t() {}

        /// \brief get timestamp from event node (ostime_t ticks)
        std::uint32_t getTime() const { return this->time; }

        /// \brief get client data pointer from event node
        void *getClientData() const { return this->pClientData; }

        /// \brief get a given data element from event node.
        std::uintptr_t getData(unsigned i) const { return this->data[i]; }
        };

    ///
    /// \brief make a standard entry in the event log.
    ///
    /// \return
    ///     If this function successfully allocates a log entry, returns a pointer to the
    ///     entry. Otherwise returns \c nullptr.
    ///
    EventNode_t *logEvent(void *pClientData, std::uintptr_t arg1, std::uintptr_t arg2, std::uintptr_t arg3, LogCallback_t *pFn);

    /* convenience routines */

    /// \brief print a channel number
    void printCh(std::uint8_t channel) const;

    /// \brief decode and display an LMIC \c rps_t value.
    void printRps(std::uint8_t rps) const;

    /// \brief given an LMIC \c rps_t, return the spreading-factor name
    const char *getSfName(std::uint8_t rps) const;
    /// \brief given an LMIC \c rps_t, return the bandwidth text
    const char *getBwName(std::uint8_t rps) const;
    /// \brief given an LMIC \c rps_t, return the coding scheme name
    const char *getCrName(std::uint8_t rps) const;
    /// \brief given an LMIC \c rps_t, return string saying whether CRC is enabled or disabled.
    const char *getCrcName(std::uint8_t rps) const;

    /// \brief print a value as a 2-digit hex number
    void printHex2(unsigned v) const;

    /// \brief print a value as a 4-digit hex number
    void printHex4(unsigned v) const;

    /// \brief print a space
    void printSpace(void) const;

    /// \brief print a frequency (given in Hz, but displayed in MHz)
    void printFreq(std::uint32_t freq) const;

private:
    unsigned m_head;                ///< index of first node in queue; queue is empty if head == tail
    unsigned m_tail;                ///< index of last node in queue
    EventNode_t m_queue[8];         ///< queue of entries

    /// \brief called to despool and print a single event.
    bool processSingleEvent();
    };

#endif /* _Arduino_LoRaWAN_EventLog_h_ */
