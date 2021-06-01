/*

Module:  simple_sensor_bme280.ino

Function:
    Example app showing how to periodically poll a
    sensor.

Copyright notice and License:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	May 2021

Notes:
    This app compiles and runs on an MCCI Catena 4610 board.

*/

#include <Arduino_LoRaWAN_network.h>
#include <Arduino_LoRaWAN_EventLog.h>
#include <arduino_lmic.h>
#include <Adafruit_BME280.h>

/****************************************************************************\
|
|	The LoRaWAN object
|
\****************************************************************************/

class cMyLoRaWAN : public Arduino_LoRaWAN_network {
public:
    cMyLoRaWAN() {};
    using Super = Arduino_LoRaWAN_network;
    void setup();

protected:
    // you'll need to provide implementation for this.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    // if you have persistent storage, you can provide implementations for these:
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
    virtual void NetSaveSessionState(const SessionState &State) override;
    virtual bool NetGetSessionState(SessionState &State) override;
};


/****************************************************************************\
|
|	The sensor object
|
\****************************************************************************/

class cSensor {
public:
    /// \brief the constructor. Deliberately does very little.
    cSensor() {};

    ///
    /// \brief set up the sensor object
    ///
    /// \param uplinkPeriodMs optional uplink interval. If not specified,
    ///         transmit every six minutes.
    ///
    void setup(std::uint32_t uplinkPeriodMs = 6 * 60 * 1000);

    ///
    /// \brief update sensor loop.
    ///
    /// \details
    ///     This should be called from the global loop(); it periodically
    ///     gathers and transmits sensor data.
    ///
    void loop();

private:
    void doUplink();

    bool m_fUplinkRequest;              // set true when uplink is requested
    bool m_fBusy;                       // set true while sending an uplink
    std::uint32_t m_uplinkPeriodMs;     // uplink period in milliseconds
    std::uint32_t m_tReference;         // time of last uplink

    //   The temperature/humidity sensor
    Adafruit_BME280 m_BME280; // The default initalizer creates an I2C connection
};

/****************************************************************************\
|
|	Globals
|
\****************************************************************************/

// the global LoRaWAN instance.
cMyLoRaWAN myLoRaWAN {};

// the global sensor instance
cSensor mySensor {};

// the global event log instance
Arduino_LoRaWAN::cEventLog myEventLog;

/****************************************************************************\
|
|	Provisioning info for LoRaWAN OTAA
|
\****************************************************************************/

//
// For normal use, we require that you edit the sketch to replace FILLMEIN_x
// with values assigned by the your network. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN_x to non-
// working but innocuous values.
//
// #define COMPILE_REGRESSION_TEST 1

#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN_8     1, 0, 0, 0, 0, 0, 0, 0
# define FILLMEIN_16    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN_8 (#dont edit this, edit the lines that use FILLMEIN_8)
# define FILLMEIN_16 (#dont edit this, edit the lines that use FILLMEIN_16)
#endif

// deveui, little-endian
static const std::uint8_t deveui[] = { FILLMEIN_8 };

// appeui, little-endian
static const std::uint8_t appeui[] = { FILLMEIN_8 };

// appkey: just a string of bytes, sometimes referred to as "big endian".
static const std::uint8_t appkey[] = { FILLMEIN_16 };

/****************************************************************************\
|
|	setup()
|
\****************************************************************************/

void setup() {
    // set baud rate, and wait for serial to be ready.
    Serial.begin(115200);
    while (! Serial)
        yield();

    Serial.println(F("simple_sensor_bmi280.ino: setup()"));

    // set up the log; do this fisrt.
    myEventLog.setup();

    // set up lorawan.
    myLoRaWAN.setup();

    // similarly, set up the sensor.
    mySensor.setup();
}

/****************************************************************************\
|
|	loop()
|
\****************************************************************************/

void loop() {
    // the order of these is arbitrary, but you must poll them all.
    myLoRaWAN.loop();
    mySensor.loop();
    myEventLog.loop();
}

/****************************************************************************\
|
|	LoRaWAN methods
|
\****************************************************************************/

// our setup routine does the class setup and then registers an event handler so
// we can see some interesting things
void
cMyLoRaWAN::setup() {
    // simply call begin() w/o parameters, and the LMIC's built-in
    // configuration for this board will be used.
    this->Super::begin();

//    LMIC_selectSubBand(0);
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    this->RegisterListener(
        // use a lambda so we're "inside" the cMyLoRaWAN from public/private perspective
        [](void *pClientInfo, uint32_t event) -> void {
            auto const pThis = (cMyLoRaWAN *)pClientInfo;

            // for tx start, we quickly capture the channel and the RPS
            if (event == EV_TXSTART) {
                // use another lambda to make log prints easy
                myEventLog.logEvent(
                    (void *) pThis,
                    LMIC.txChnl,
                    LMIC.rps,
                    0,
                    // the print-out function
                    [](cEventLog::EventNode_t const *pEvent) -> void {
                        Serial.print(F(" TX:"));
                        myEventLog.printCh(std::uint8_t(pEvent->getData(0)));
                        myEventLog.printRps(rps_t(pEvent->getData(1)));
                    }
                );
            }
            // else if (event == some other), record with print-out function
            else {
                // do nothing.
            }
        },
        (void *) this   // in case we need it.
        );
}

// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool
cMyLoRaWAN::GetOtaaProvisioningInfo(
    OtaaProvisioningInfo *pInfo
    ) {
    // these are the same constants used in the LMIC compliance test script; eases testing
    // with the RedwoodComm RWC5020B/RWC5020M testers.

    // initialize info
    memcpy(pInfo->DevEUI, deveui, sizeof(pInfo->DevEUI));
    memcpy(pInfo->AppEUI, appeui, sizeof(pInfo->AppEUI));
    memcpy(pInfo->AppKey, appkey, sizeof(pInfo->AppKey));

    return true;
}

// save Info somewhere (if possible)
// if not possible, just do nothing and make sure you return false
// from NetGetSessionState().
void
cMyLoRaWAN::NetSaveSessionInfo(
    const SessionInfo &Info,
    const uint8_t *pExtraInfo,
    size_t nExtraInfo
    ) {
    // in this example, we can't save, so we just return.
}

// save State somewhere. Note that it's often the same;
// often only the frame counters change.
// if not possible, just do nothing and make sure you return false
// from NetGetSessionState().
void
cMyLoRaWAN::NetSaveSessionState(const SessionState &State) {
    // in this example, we can't save, so we just return.
}

// either fetch SessionState from somewhere and return true or...
// return false, which forces a re-join.
bool
cMyLoRaWAN::NetGetSessionState(SessionState &State) {
    // we didn't save earlier, so just tell the core we don't have data.
    return false;
}


/****************************************************************************\
|
|	Sensor methods
|
\****************************************************************************/

void
cSensor::setup(std::uint32_t uplinkPeriodMs) {
    if (! this->m_BME280.begin(BME280_ADDRESS, Adafruit_BME280::OPERATING_MODE::Sleep)) {
        while (true) {
            // just loop forever, printing an error occasionally.
            Serial.println("BME280.begin failed");
            delay(2000);
        }
    }

    // set the initial time.
    this->m_uplinkPeriodMs = uplinkPeriodMs;
    this->m_tReference = millis();

    // uplink right away
    this->m_fUplinkRequest = true;
}

void
cSensor::loop(void) {
    auto const tNow = millis();
    auto const deltaT = tNow - this->m_tReference;

    if (deltaT >= this->m_uplinkPeriodMs) {
        // request an uplink
        this->m_fUplinkRequest = true;

        // keep trigger time locked to uplinkPeriod
        auto const advance = deltaT / this->m_uplinkPeriodMs;
        this->m_tReference += advance * this->m_uplinkPeriodMs; 
    }

    // if an uplink was requested, do it.
    if (this->m_fUplinkRequest) {
        this->m_fUplinkRequest = false;
        this->doUplink();
    }
}

void
cSensor::doUplink(void) {
    // if busy uplinking, just skip
    if (this->m_fBusy)
        return;
    // if LMIC is busy, just skip
    if (LMIC.opmode & (OP_POLL | OP_TXDATA | OP_TXRXPEND))
        return;

    // make a measurement on the BME280
    auto const m = this->m_BME280.readTemperaturePressureHumidity();

    Serial.print("Sensor: T="); Serial.print(m.Temperature);
    Serial.print(" degC, P="); Serial.print(m.Pressure);
    Serial.print(" hPa, RH="); Serial.print(m.Humidity);
    Serial.println("%");

    // format the uplink
    // temperature is 2 bytes from -0x80.00 to +0x7F.FF degrees C
    // humidity is 2 bytes, where 0 == 0% and 0xFFFF == 100%.
    // pressure is 3 bytes, Pa.
    // big-endian.

    std::uint8_t uplink[7];
    auto const t = m.Temperature;
    auto const it = std::int16_t(floor(t * 256 + 0.5));
    auto const up = std::uint32_t(floor(m.Pressure + 0.5));
    auto const uh = std::uint16_t(m.Humidity / 100.0 * 65535 + 0.5);

    uplink[0] = std::uint8_t(std::uint16_t(it) >> 8);
    uplink[1] = std::uint8_t(it);
    uplink[2] = std::uint8_t(uh >> 8);
    uplink[3] = std::uint8_t(uh);
    uplink[4] = std::uint8_t(up >> 16);
    uplink[5] = std::uint8_t(up >> 8);
    uplink[6] = std::uint8_t(up);

    this->m_fBusy = true;
    
    if (! myLoRaWAN.SendBuffer(
        uplink, sizeof(uplink),
        // this is the completion function:
        [](void *pClientData, bool fSucccess) -> void {
            auto const pThis = (cSensor *)pClientData;
            pThis->m_fBusy = false;
        },
        (void *)this,
        /* confirmed */ false,
        /* port */ 1
        )) {
        // sending failed; callback has not been called and will not
        // be called. Reset busy flag.
        this->m_fBusy = false;
    }
}
