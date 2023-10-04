///////////////////////////////////////////////////////////////////////////////
// arduino_lorawan_esp32_example.ino
// 
// Example sketch showing how to periodically poll a sensor and send the data
// to a LoRaWAN network.
//
// Based on simple_sensor_bme280.ino with the following modifications: 
// - reading the sensor data is replaced by a stub
// - implements power saving by using the ESP32 deep sleep mode
// - implements fast re-joining after sleep by storing network session data
//   in the ESP32 RTC RAM
// - LoRa_Serialization is used for encoding various data types into bytes
//
//
// Based on:
// ---------
// MCCI LoRaWAN LMIC library by Thomas Telkamp and Matthijs Kooijman / Terry Moore, MCCI
// (https://github.com/mcci-catena/arduino-lmic)

// MCCI Arduino LoRaWAN Library by Terry Moore, MCCI 
// (https://github.com/mcci-catena/arduino-lorawan)
//
//
// Library dependencies (tested versions):
// ---------------------------------------
// MCCI Arduino Development Kit ADK     0.2.2
// MCCI LoRaWAN LMIC library            4.1.1
// MCCI Arduino LoRaWAN Library         0.10.0
// LoRa_Serialization                   3.2.1
// ESP32Time                            2.0.4
// ESP32AnalogRead                      0.2.1 (optional)
//
//
// created: 07/2022
//
//
// MIT License
//
// Copyright (c) 2022 Matthias Prinke
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// History:
//
// 20220729 Created
// 20230307 Changed cMyLoRaWAN to inherit from Arduino_LoRaWAN_network
//          instead of Arduino_LoRaWAN_ttn
//          Added Pin mappings for some common ESP32 LoRaWAN boards
// 20230903 Added time keeping with RTC and synchronization to network time
//          Changed cMyLoRaWAN to inherit from Arduino_LoRaWAN_network
//          instead of Arduino_LoRaWAN_ttn
//          Changed LoRaWAN message size to actual payload size
//          Implemented wake-up to fixed time scheme
//          Added energy saving modes
//          Modified DEBUG_PRINTF/DEBUG_PRINTF_TS macros to use
//          Arduino logging functions
//          Added setting of RTC via downlink
//          Added remote configuration via LoRaWAN downlink
//          Added configuration for several ESP32 boards
//          Implemented downlink commands CMD_GET_DATETIME & CMD_GET_CONFIG
// 20230907 Added missing code for energy saving modes & generic ADC usage
// 20230910 Added pin definitions for FIREBEETLE_COVER_LORA
//
//
// Notes:
// - After a successful transmission, the controller can go into deep sleep
//   (option SLEEP_EN)
// - Sleep time is defined in SLEEP_INTERVAL
// - If joining the network or transmitting uplink data fails,
//   the controller can go into deep sleep
//   (option FORCE_SLEEP)
// - Timeout is defined in SLEEP_TIMEOUT_INITIAL and SLEEP_TIMEOUT_JOINED
// - The ESP32's RTC RAM is used to store information about the LoRaWAN 
//   network session; this speeds up the connection after a restart
//   significantly
// - To enable Network Time Requests:
//   #define LMIC_ENABLE_DeviceTimeReq 1
// - settimeofday()/gettimeofday() must be used to access the ESP32's RTC time
// - Arduino ESP32 package has built-in time zone handling, see 
//   https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino
// - Apply fixes if using Arduino ESP32 board package v2.0.x
//     - mcci-catena/arduino-lorawan#204
//       (https://github.com/mcci-catena/arduino-lorawan/pull/204)
//       --> fixed in mcci-catena/arduino-lorawan v0.10.0
//     - mcci-catena/arduino-lmic#714 
//       (https://github.com/mcci-catena/arduino-lmic/issues/714#issuecomment-822051171)
//
///////////////////////////////////////////////////////////////////////////////

//--- Select LoRaWAN Network ---
// The Things Network
#define ARDUINO_LMIC_CFG_NETWORK_TTN 1

// Helium Network
// see mcci-cathena/arduino-lorawan issue #185 "Add Helium EU868 support"
// (https://github.com/mcci-catena/arduino-lorawan/issues/185)
#define ARDUINO_LMIC_CFG_NETWORK_GENERIC 0

// (Add other networks here)


#include <Arduino_LoRaWAN_network.h>
#include <Arduino_LoRaWAN_EventLog.h>
#include <arduino_lmic.h>
#include <Preferences.h>
#include <ESP32Time.h>

//-----------------------------------------------------------------------------
//
// User Configuration
//

// Enable debug mode (debug messages via serial port)
//#define _DEBUG_MODE_
// NOTE: Add #define LMIC_ENABLE_DeviceTimeReq 1
//        in ~/Arduino/libraries/MCCI_LoRaWAN_LMIC_library/project_config/lmic_project_config.h
#if (not(LMIC_ENABLE_DeviceTimeReq))
    #warning "LMIC_ENABLE_DeviceTimeReq is not set - will not be able to retrieve network time!"
#endif

// Battery voltage thresholds for energy saving

// If SLEEP_EN is defined and battery voltage is below BATTERY_WEAK [mV], MCU will sleep for SLEEP_INTERVAL_LONG
#define BATTERY_WEAK 0 // example: 3500

// Go to sleep mode immediately after start if battery voltage is below BATTERY_LOW [mV]
#define BATTERY_LOW 0 // example: 3200

/// Enable sleep mode - sleep after successful transmission to TTN (recommended!)
#define SLEEP_EN

/// If SLEEP_EN is defined, MCU will sleep for SLEEP_INTERVAL seconds after succesful transmission
#define SLEEP_INTERVAL 360

/// Long sleep interval, MCU will sleep for SLEEP_INTERVAL_LONG seconds if battery voltage is below BATTERY_WEAK
#define SLEEP_INTERVAL_LONG 900

/// RTC to network time sync interval (in minutes)
#define CLOCK_SYNC_INTERVAL 24 * 60

/// Force deep sleep after a certain time, even if transmission was not completed
#define FORCE_SLEEP

// Force a new join procedure (instead of re-join) after encountering sleep timeout
#define FORCE_JOIN_AFTER_SLEEP_TIMEOUT

/// During initialization (not joined), force deep sleep after SLEEP_TIMEOUT_INITIAL (if enabled)
#define SLEEP_TIMEOUT_INITIAL 1800

/// If already joined, force deep sleep after SLEEP_TIMEOUT_JOINED seconds (if enabled)
#define SLEEP_TIMEOUT_JOINED 600

/// Additional timeout to be applied after joining if Network Time Request pending
#define SLEEP_TIMEOUT_EXTRA 300

// Enable battery / supply voltage measurement
#define ADC_EN

// ADC for supply/battery voltage measurement
// default: on-board connection to VB on FireBeetle ESP32 (with R10+R11 assembled)
//          on-board connection to VBAT on TTGO LoRa32
//          on-board connection to VBAT on Adafruit Feather ESP32
//          no VBAT input circuit on Adafruit Feather ESP32-S2
#ifdef ADC_EN
    #if defined(ARDUINO_TTGO_LoRa32_V1) || defined(ARDUINO_TTGO_LoRa32_V2) || defined(ARDUINO_TTGO_LoRa32_v21new)
        #define PIN_ADC_IN        35
    #elif defined(ARDUINO_FEATHER_ESP32)
        #define PIN_ADC_IN        A13
    #elif defined(LORAWAN_NODE)
        #define PIN_ADC_IN        A0
    #else
        #define PIN_ADC_IN        34
    #endif
#endif


// Additional ADC pins (default: FireBeetle ESP32) 
//#define PIN_ADC0_IN         A0
//#define PIN_ADC1_IN         A1
//#define PIN_ADC2_IN         A2
#ifdef LORAWAN_NODE
  #define PIN_ADC3_IN         A3
#endif

#ifdef PIN_ADC0_IN
    // Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
    const float ADC0_DIV         = 0.5;       
    const uint8_t ADC0_SAMPLES   = 10;
#endif

#ifdef PIN_ADC1_IN
    // Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
    const float ADC1_DIV         = 0.5;       
    const uint8_t ADC1_SAMPLES   = 10;
#endif

#ifdef ADC_EN
    // Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
    const float UBATT_DIV         = 0.5;       
    const uint8_t UBATT_SAMPLES   = 10;
#endif

#ifdef PIN_ADC2_IN
    // Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
    const float ADC2_DIV         = 0.5;       
    const uint8_t ADC2_SAMPLES   = 10;
#endif

#ifdef PIN_ADC3_IN
    // Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
    const float ADC3_DIV         = 0.5;       
    const uint8_t ADC3_SAMPLES   = 10;
#endif

/// Preferences namespace
#define PREFS_NAMESPACE "MY_PREFS"

//-----------------------------------------------------------------------------

// LoRa_Serialization
#include <LoraMessage.h>

#ifdef ADC_EN
    // ESP32 calibrated Analog Input Reading
    #include <ESP32AnalogRead.h>
#endif

// Pin mappings for some common ESP32 LoRaWAN boards.
// The ARDUINO_* defines are set by selecting the appropriate board (and borad variant, if applicable) in the Arduino IDE.
// The default SPI port of the specific board will be used.
#if defined(ARDUINO_TTGO_LoRa32_V1)
    // https://github.com/espressif/arduino-esp32/blob/master/variants/ttgo-lora32-v1/pins_arduino.h
    // http://www.lilygo.cn/prod_view.aspx?TypeId=50003&Id=1130&FId=t3:50003:3
    // https://github.com/Xinyuan-LilyGo/TTGO-LoRa-Series
    // https://github.com/LilyGO/TTGO-LORA32/blob/master/schematic1in6.pdf
    #define PIN_LMIC_NSS      LORA_CS
    #define PIN_LMIC_RST      LORA_RST
    #define PIN_LMIC_DIO0     LORA_IRQ
    #define PIN_LMIC_DIO1     33
    #define PIN_LMIC_DIO2     cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN

#elif defined(ARDUINO_TTGO_LoRa32_V2)
    // https://github.com/espressif/arduino-esp32/blob/master/variants/ttgo-lora32-v2/pins_arduino.h
    #define PIN_LMIC_NSS      LORA_CS
    #define PIN_LMIC_RST      LORA_RST
    #define PIN_LMIC_DIO0     LORA_IRQ
    #define PIN_LMIC_DIO1     33
    #define PIN_LMIC_DIO2     cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN
    #pragma message("LoRa DIO1 must be wired to GPIO33 manually!")

#elif defined(ARDUINO_TTGO_LoRa32_v21new)
    // https://github.com/espressif/arduino-esp32/blob/master/variants/ttgo-lora32-v21new/pins_arduino.h
    #define PIN_LMIC_NSS      LORA_CS
    #define PIN_LMIC_RST      LORA_RST
    #define PIN_LMIC_DIO0     LORA_IRQ
    #define PIN_LMIC_DIO1     LORA_D1
    #define PIN_LMIC_DIO2     LORA_D2

#elif defined(ARDUINO_heltec_wireless_stick) || defined(ARDUINO_heltec_wifi_lora_32_V2)
    // https://github.com/espressif/arduino-esp32/blob/master/variants/heltec_wireless_stick/pins_arduino.h
    // https://github.com/espressif/arduino-esp32/tree/master/variants/heltec_wifi_lora_32_V2/pins_ardiono.h
    #define PIN_LMIC_NSS      SS
    #define PIN_LMIC_RST      RST_LoRa
    #define PIN_LMIC_DIO0     DIO0
    #define PIN_LMIC_DIO1     DIO1
    #define PIN_LMIC_DIO2     DIO2

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
    #define PIN_LMIC_NSS      6
    #define PIN_LMIC_RST      9
    #define PIN_LMIC_DIO0     5
    #define PIN_LMIC_DIO1     11
    #define PIN_LMIC_DIO2     cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN
    #pragma message("ARDUINO_ADAFRUIT_FEATHER_ESP32S2 defined; assuming RFM95W FeatherWing will be used")
    #pragma message("Required wiring: E to IRQ, D to CS, C to RST, A to DI01")
    #pragma message("BLE is not available!")

#elif defined(ARDUINO_FEATHER_ESP32)
    #define PIN_LMIC_NSS      14
    #define PIN_LMIC_RST      27
    #define PIN_LMIC_DIO0     32
    #define PIN_LMIC_DIO1     33
    #define PIN_LMIC_DIO2     cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN
    #pragma message("NOT TESTED!!!")
    #pragma message("ARDUINO_ADAFRUIT_FEATHER_ESP32 defined; assuming RFM95W FeatherWing will be used")
    #pragma message("Required wiring: A to RST, B to DIO1, D to DIO0, E to CS")

#elif defined(FIREBEETLE_COVER_LORA)
    // https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478
    // https://wiki.dfrobot.com/FireBeetle_Covers_LoRa_Radio_868MHz_SKU_TEL0125
    #define PIN_LMIC_NSS      27 // D4
    #define PIN_LMIC_RST      25 // D2
    #define PIN_LMIC_DIO0     26 // D3
    #define PIN_LMIC_DIO1      9 // D5
    #define PIN_LMIC_DIO2     cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN
    #pragma message("FIREBEETLE_COVER_LORA defined; assuming FireBeetle ESP32 with FireBeetle Cover LoRa will be used")
    #pragma message("Required wiring: D2 to RESET, D3 to DIO0, D4 to CS, D5 to DIO1")

#else
    // LoRaWAN_Node board
    // https://github.com/matthias-bs/LoRaWAN_Node
#define PIN_LMIC_NSS      14
#define PIN_LMIC_RST      12
#define PIN_LMIC_DIO0     4
#define PIN_LMIC_DIO1     16
#define PIN_LMIC_DIO2     17

#endif

/// Enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";  



// Uplink message payload size
// The maximum allowed for all data rates is 51 bytes.
const uint8_t PAYLOAD_SIZE = 51;

// RTC Memory Handling
#define MAGIC1 (('m' << 24) | ('g' < 16) | ('c' << 8) | '1')
#define MAGIC2 (('m' << 24) | ('g' < 16) | ('c' << 8) | '2')
#define EXTRA_INFO_MEM_SIZE 64

// Debug printing
// To enable debug mode (debug messages via serial port):
// Arduino IDE: Tools->Core Debug Level: "Debug|Verbose"
// or
// set CORE_DEBUG_LEVEL in BresserWeatherSensorTTNCfg.h
    #define DEBUG_PRINTF(...) { log_d(__VA_ARGS__); }
    #define DEBUG_PRINTF_TS(...) { log_d(__VA_ARGS__); }

// Downlink messages
// ------------------
//
// CMD_SET_SLEEP_INTERVAL
// (seconds)
// byte0: 0xA8
// byte1: sleep_interval[15:8]
// byte2: sleep_interval[ 7:0]

// CMD_SET_SLEEP_INTERVAL_LONG
// (seconds)
// byte0: 0xA9
// byte1: sleep_interval_long[15:8]
// byte2: sleep_interval_long[ 7:0]
//
// CMD_GET_CONFIG
// byte0: 0xB1
//
// CMD_GET_DATETIME
// byte0: 0x86
//
// CMD_SET_DATETIME
// byte0: 0x88
// byte1: unixtime[31:24]
// byte2: unixtime[23:16]
// byte3: unixtime[15: 8]
// byte4: unixtime[ 7: 0]
//
// Response uplink messages
// -------------------------
//
// CMD_GET_DATETIME -> FPort=2
// byte0: unixtime[31:24]
// byte1: unixtime[23:16]
// byte2: unixtime[15: 8]
// byte3: unixtime[ 7: 0]
// byte4: rtc_source[ 7: 0]
//
// CMD_GET_CONFIG -> FPort=3
// byte0: sleep_interval[15:8]
// byte1: sleep_interval[ 7:0]
// byte2: sleep_interval_long[15:8]
// byte3: sleep_interval_long[ 7:0]

#define CMD_SET_SLEEP_INTERVAL          0xA8
#define CMD_SET_SLEEP_INTERVAL_LONG     0xA9
#define CMD_GET_CONFIG                  0xB1
#define CMD_GET_DATETIME                0x86
#define CMD_SET_DATETIME                0x88

void printDateTime(void);

/****************************************************************************\
|
|	The LoRaWAN object
|
\****************************************************************************/

/*!
 * \class cMyLoRaWAN
 * 
 * \brief The LoRaWAN object - LoRaWAN protocol and session handling
 */ 
class cMyLoRaWAN : public Arduino_LoRaWAN_network {
public:
    cMyLoRaWAN() {};
    using Super = Arduino_LoRaWAN_network;
    
    /*!
     * \fn setup
     * 
     * \brief Initialize cMyLoRaWAN object
     */
    void setup();
    

    /*!
    * \fn requestNetworkTime
    * 
    * \brief Wrapper function for LMIC_requestNetworkTime()
    */
    void requestNetworkTime(void);
    
    
    
    /*!
     * \fn printSessionInfo
     * 
     * \brief Print contents of session info data structure for debugging
     * 
     * \param Info Session information data structure
     */
    void printSessionInfo(const SessionInfo &Info);
    
    
    /*!
     * \fn printSessionState
     * 
     * \brief Print contents of session state data structure for debugging
     * 
     * \param State Session state data structure
     */
    void printSessionState(const SessionState &State);

    /*!
     * \fn doCfgUplink
     *
     * \brief Uplink configuration/status
     */
    void doCfgUplink(void);

    bool isBusy(void) {
      return m_fBusy;
    }
    
private:
    bool m_fBusy;                       // set true while sending an uplink
    
protected:
    // you'll need to provide implementation for this.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;

    // NetTxComplete() activates deep sleep mode (if enabled)
    virtual void NetTxComplete(void) override;

    // NetJoin() changes <sleepTimeout>
    virtual void NetJoin(void) override;
    
    // Used to store/load data to/from persistent (at least during deep sleep) memory 
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
    // Note: GetSavedSessionInfo provided in simple_sensor_bme280.ino is not used anywhere
    //virtual bool GetSavedSessionInfo(SessionInfo &Info, uint8_t*, size_t, size_t*) override;
    virtual void NetSaveSessionState(const SessionState &State) override;
    virtual bool NetGetSessionState(SessionState &State) override;
    virtual bool GetAbpProvisioningInfo(AbpProvisioningInfo *pAbpInfo) override;  
};


/****************************************************************************\
|
|	The sensor object
|
\****************************************************************************/

/*!
 * \class cSensor
 * 
 * \brief The sensor object - collect sensor data and schedule uplink
 */
class cSensor {
public:
    /// \brief the constructor. Deliberately does very little.
    cSensor() {};

    // Sensor data function stubs
    float    getTemperature(void);
    uint8_t  getHumidity(void);
    uint16_t getVoltageBattery(void);
    uint16_t getVoltageSupply(void);
    
#ifdef ADC_EN
        /*!
         * \fn getVoltage
         * 
         * \brief Get supply voltage (fixed ADC input circuit on FireBeetle ESP32 board)
         * 
         * \returns Voltage [mV]
         */
        uint16_t getVoltage(void);
        
        /*
         * \fn getVoltage
         * 
         * \brief Get ADC voltage from specified port with averaging and application of divider
         * 
         * \param adc ADC port
         * 
         * \param samples No. of samples used in averaging
         * 
         * \param divider Voltage divider
         * 
         * \returns Voltage [mV]
         */
        uint16_t getVoltage(ESP32AnalogRead &adc, uint8_t samples, float divider);
    #endif
        
  /*!
     * \fn uplinkRequest
     * 
     * \brief Request uplink to LoRaWAN
     */
    void uplinkRequest(void) {
        m_fUplinkRequest = true;
    };
    
    /*!
     * \brief set up the sensor object
    *
     * \param uplinkPeriodMs optional uplink interval. If not specified,
    *                       transmit every six minutes.
    */
    void setup(std::uint32_t uplinkPeriodMs = 6 * 60 * 1000);

    /*!
     * \brief update sensor loop.
    *
     * \details
    *     This should be called from the global loop(); it periodically
    *     gathers and transmits sensor data.
    */
    void loop();

    bool isBusy(void) {
      return m_fBusy;
    }
    
private:
    void doUplink();

    bool m_fUplinkRequest;              //!< set true when uplink is requested
    bool m_fBusy;                       //!< set true while sending an uplink
    std::uint32_t m_uplinkPeriodMs;     //!< uplink period in milliseconds
    std::uint32_t m_tReference;         //!< time of last uplink
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

// The pin map. This form is convenient if the LMIC library
// doesn't support your board and you don't want to add the
// configuration to the library (perhaps you're just testing).
// This pinmap matches the FeatherM0 LoRa. See the arduino-lmic
// docs for more info on how to set this up.
const cMyLoRaWAN::lmic_pinmap myPinMap = {
     .nss = PIN_LMIC_NSS,
     .rxtx = cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN,
     .rst = PIN_LMIC_RST,
     .dio = { PIN_LMIC_DIO0, PIN_LMIC_DIO1, PIN_LMIC_DIO2 },
     .rxtx_rx_active = 0,
     .rssi_cal = 0,
     .spi_freq = 8000000,
    .pConfig = NULL
};

// The following variables are stored in the ESP32's RTC RAM -
// their value is retained after a Sleep Reset.
RTC_DATA_ATTR uint32_t                        magicFlag1;               //!< flag for validating Session State in RTC RAM
RTC_DATA_ATTR Arduino_LoRaWAN::SessionState   rtcSavedSessionState;     //!< Session State saved in RTC RAM
RTC_DATA_ATTR uint32_t                        magicFlag2;               //!< flag for validating Session Info in RTC RAM
RTC_DATA_ATTR Arduino_LoRaWAN::SessionInfo    rtcSavedSessionInfo;      //!< Session Info saved in RTC RAM
RTC_DATA_ATTR size_t                          rtcSavedNExtraInfo;       //!< size of extra Session Info data
RTC_DATA_ATTR uint8_t                         rtcSavedExtraInfo[EXTRA_INFO_MEM_SIZE]; //!< extra Session Info data
RTC_DATA_ATTR bool                            runtimeExpired = false;   //!< flag indicating if runtime has expired at least once
RTC_DATA_ATTR bool                            longSleep;                //!< last sleep interval; 0 - normal / 1 - long
RTC_DATA_ATTR time_t                          rtcLastClockSync = 0;     //!< timestamp of last RTC synchonization to network time

/// Uplink payload buffer
static uint8_t loraData[PAYLOAD_SIZE];

/// Force sleep mode after <sleepTimeout> has been reached (if FORCE_SLEEP is defined) 

#ifdef ADC_EN
    // ESP32 ADC with calibration
    ESP32AnalogRead adc; //!< ADC object for supply voltage measurement
#endif

// ESP32 ADC with calibration
#if defined(ADC_EN) && defined(PIN_ADC0_IN)
    ESP32AnalogRead adc0; //!< ADC object
#endif
#if defined(ADC_EN) && defined(PIN_ADC1_IN)
    ESP32AnalogRead adc1; //!< ADC object
#endif
#if defined(ADC_EN) && defined(PIN_ADC2_IN)
    ESP32AnalogRead adc2; //!< ADC object
#endif
#if defined(ADC_EN) && defined(PIN_ADC3_IN)
    ESP32AnalogRead adc3; //!< ADC object
#endif
/// ESP32 preferences (stored in flash memory)
Preferences preferences;

struct sPrefs {
    uint16_t  sleep_interval;       //!< preferences: sleep interval
    uint16_t  sleep_interval_long;  //!< preferences: sleep interval long
} prefs;

/// Sleep request
bool sleepReq = false;

/// Uplink request - command received via downlink
uint8_t uplinkReq = 0;

/// Force sleep mode after <code>sleepTimeout</code> has been reached (if FORCE_SLEEP is defined) 
ostime_t sleepTimeout; //!

/// Seconds since the UTC epoch
uint32_t userUTCTime;

/// RTC sync request flag - set (if due) in setup() / cleared in UserRequestNetworkTimeCb()
bool rtcSyncReq = false;

/// Real time clock
ESP32Time rtc;

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

// APPEUI, DEVEUI and APPKEY
#include "secrets.h"

#ifndef SECRETS
    #define SECRETS
    
    // The following constants should be copied to secrets.h and configured appropriately
    // according to the settings from TTN Console
    
    /// DeviceEUI, little-endian (lsb first)
    static const std::uint8_t deveui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    
    /// AppEUI, little-endian (lsb first)
    static const std::uint8_t appeui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    
    /// AppKey: just a string of bytes, sometimes referred to as "big endian".
    static const std::uint8_t appkey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif

    
/****************************************************************************\
|
|	setup()
|
\****************************************************************************/

/// Arduino setup
void setup() {
    // set baud rate
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    // wait for serial to be ready - or timeout if USB is not connected
    delay(500);

    preferences.begin(PREFS_NAMESPACE, false);
    prefs.sleep_interval      = preferences.getUShort("sleep_interval", SLEEP_INTERVAL);
    log_d("Preferences: sleep_interval:        %u s", prefs.sleep_interval);
    prefs.sleep_interval_long = preferences.getUShort("sleep_interval_long", SLEEP_INTERVAL_LONG);
    log_d("Preferences: sleep_interval_long:   %u s", prefs.sleep_interval_long);
    preferences.end();

    sleepTimeout = sec2osticks(SLEEP_TIMEOUT_INITIAL);

    DEBUG_PRINTF_TS("");
    
    // Set time zone
    setenv("TZ", TZ_INFO, 1);
    printDateTime();
    
    // Check if clock was never synchronized or sync interval has expired 
    if ((rtcLastClockSync == 0) || ((rtc.getLocalEpoch() - rtcLastClockSync) > (CLOCK_SYNC_INTERVAL * 60))) {
        DEBUG_PRINTF("RTC sync required");
        rtcSyncReq = true;
    }

    // set up the log; do this first.
    myEventLog.setup();
    DEBUG_PRINTF("myEventlog.setup() - done");

    // set up the sensors.
    mySensor.setup();
    DEBUG_PRINTF("mySensor.setup() - done");

    // set up lorawan.
    myLoRaWAN.setup();
    DEBUG_PRINTF("myLoRaWAN.setup() - done");

    mySensor.uplinkRequest();
}

/****************************************************************************\
|
|	loop()
|
\****************************************************************************/

/// Arduino execution loop
void loop() {
    // the order of these is arbitrary, but you must poll them all.
    myLoRaWAN.loop();
    mySensor.loop();
    myEventLog.loop();

    if (uplinkReq != 0) {
      myLoRaWAN.doCfgUplink();
    }
    #ifdef SLEEP_EN
        if (sleepReq & !rtcSyncReq) {
            myLoRaWAN.Shutdown();
            prepareSleep();
        }
    #endif

    #ifdef FORCE_SLEEP
        if (os_getTime() > sleepTimeout) {
            runtimeExpired = true;
            myLoRaWAN.Shutdown();
            #ifdef FORCE_JOIN_AFTER_SLEEP_TIMEOUT
                // Force join (instead of re-join)
                magicFlag1 = 0;
                magicFlag2 = 0;
            #endif
            DEBUG_PRINTF_TS("Sleep timer expired!");
            prepareSleep();
        }
    #endif
}

/****************************************************************************\
|
|	LoRaWAN methods
|
\****************************************************************************/

// Receive and process downlink messages
void ReceiveCb(
    void *pCtx,
    uint8_t uPort,
    const uint8_t *pBuffer,
    size_t nBuffer) {
            
    uplinkReq = 0;
    log_v("Port: %d", uPort);
    char buf[255];
    *buf = '\0';

    if (uPort > 0) {
        for (int i = 0; i < nBuffer; i++) {
              sprintf(&buf[strlen(buf)], "%02X ", pBuffer[i]);
        }
        log_v("Data: %s", buf);

        if ((pBuffer[0] == CMD_GET_DATETIME) && (nBuffer == 1)) {
            log_d("Get date/time");
            uplinkReq = CMD_GET_DATETIME;
        }
        if ((pBuffer[0] == CMD_GET_CONFIG) && (nBuffer == 1)) {
            log_d("Get config");
            uplinkReq = CMD_GET_CONFIG; 
        }
        if ((pBuffer[0] == CMD_SET_DATETIME) && (nBuffer == 5)) {
            
            time_t set_time = pBuffer[4] | (pBuffer[3] << 8) | (pBuffer[2] << 16) | (pBuffer[1] << 24);
            rtc.setTime(set_time);
            rtcLastClockSync = rtc.getLocalEpoch();
            #if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
                char tbuf[25];
                struct tm timeinfo;
           
                localtime_r(&set_time, &timeinfo);
                strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
                log_d("Set date/time: %s", tbuf);
            #endif
        }
        if ((pBuffer[0] == CMD_SET_SLEEP_INTERVAL) && (nBuffer == 3)){
            prefs.sleep_interval = pBuffer[2] | (pBuffer[1] << 8);
            log_d("Set sleep_interval: %u s", prefs.sleep_interval);
            preferences.begin(PREFS_NAMESPACE, false);
            preferences.putUShort("sleep_interval", prefs.sleep_interval);
            preferences.end();            
        }
        if ((pBuffer[0] == CMD_SET_SLEEP_INTERVAL_LONG) && (nBuffer == 3)){
            prefs.sleep_interval_long = pBuffer[2] | (pBuffer[1] << 8);
            log_d("Set sleep_interval_long: %u s", prefs.sleep_interval_long);
            preferences.begin(PREFS_NAMESPACE, false);
            preferences.putUShort("sleep_interval_long", prefs.sleep_interval_long);
            preferences.end();            
        }
    }
    if (uplinkReq == 0) {
        sleepReq = true;
    }
}

// our setup routine does the class setup and then registers an event handler so
// we can see some interesting things
void
cMyLoRaWAN::setup() {
    // simply call begin() w/o parameters, and the LMIC's built-in
    // configuration for this board will be used.
    this->Super::begin(myPinMap);
    
//    LMIC_selectSubBand(0);
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);


    this->SetReceiveBufferBufferCb(ReceiveCb);
    
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
                        #if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
                            //rps_t _rps = rps_t(pEvent->getData(1));
                            //Serial.printf("rps (1): %02X\n", _rps);
                            uint8_t rps = pEvent->getData(1);
                            uint32_t tstamp = osticks2ms(pEvent->getTime());
                        #endif
                        // see MCCI_Arduino_LoRaWAN_Library/src/lib/arduino_lorawan_cEventLog.cpp
                        log_i("TX @%u ms: ch=%d rps=0x%02x (%s %s %s %s IH=%d)", 
                            tstamp,
                            std::uint8_t(pEvent->getData(0)),
                            rps,
                            myEventLog.getSfName(rps),
                            myEventLog.getBwName(rps),
                            myEventLog.getCrName(rps),
                            myEventLog.getCrcName(rps),
                            unsigned(getIh(rps)));
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

// This method is called after the node has joined the network.
void
cMyLoRaWAN::NetJoin(
    void) {
    DEBUG_PRINTF_TS("");
    sleepTimeout = os_getTime() + sec2osticks(SLEEP_TIMEOUT_JOINED);
    if (rtcSyncReq) {
        // Allow additional time for completing Network Time Request
        sleepTimeout += os_getTime() + sec2osticks(SLEEP_TIMEOUT_EXTRA);
    }
}

// This method is called after transmission has been completed.
void
cMyLoRaWAN::NetTxComplete(void) {
    DEBUG_PRINTF_TS("");
}

// Print session info for debugging
void 
cMyLoRaWAN::printSessionInfo(const SessionInfo &Info)
{
    log_v("Tag:\t\t%d", Info.V1.Tag);
    log_v("Size:\t\t%d", Info.V1.Size);
    log_v("Rsv2:\t\t%d", Info.V1.Rsv2);
    log_v("Rsv3:\t\t%d", Info.V1.Rsv3);
    log_v("NetID:\t\t0x%08X", Info.V1.NetID);
    log_v("DevAddr:\t\t0x%08X", Info.V1.DevAddr);
    if (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG) {
        char buf[64];
        *buf = '\0';
        for (int i=0; i<15;i++) {
            sprintf(&buf[strlen(buf)], "%02X ", Info.V1.NwkSKey[i]);  
        }
        log_v("NwkSKey:\t\t%s", buf);
    }
    if (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG) {
        char buf[64];
        *buf = '\0';
        for (int i=0; i<15;i++) {
            sprintf(&buf[strlen(buf)], "%02X ", Info.V1.AppSKey[i]);
        }
        log_v("AppSKey:\t\t%s", buf);
    }    
}

// Print session state for debugging
void
cMyLoRaWAN::printSessionState(const SessionState &State)
{
    log_v("Tag:\t\t%d", State.V1.Tag);
    log_v("Size:\t\t%d", State.V1.Size);
    log_v("Region:\t\t%d", State.V1.Region);
    log_v("LinkDR:\t\t%d", State.V1.LinkDR);
    log_v("FCntUp:\t\t%d", State.V1.FCntUp);
    log_v("FCntDown:\t\t%d", State.V1.FCntDown);
    log_v("gpsTime:\t\t%d", State.V1.gpsTime);
    log_v("globalAvail:\t%d", State.V1.globalAvail);
    log_v("Rx2Frequency:\t%d", State.V1.Rx2Frequency);
    log_v("PingFrequency:\t%d", State.V1.PingFrequency);
    log_v("Country:\t\t%d", State.V1.Country);
    log_v("LinkIntegrity:\t%d", State.V1.LinkIntegrity);
    // There is more in it...
}


// Save Info to ESP32's RTC RAM
// if not possible, just do nothing and make sure you return false
// from NetGetSessionState().
void
cMyLoRaWAN::NetSaveSessionInfo(
    const SessionInfo &Info,
    const uint8_t *pExtraInfo,
    size_t nExtraInfo
    ) {
    if (nExtraInfo > EXTRA_INFO_MEM_SIZE)
        return;
    rtcSavedSessionInfo = Info;
    rtcSavedNExtraInfo = nExtraInfo;
    memcpy(rtcSavedExtraInfo, pExtraInfo, nExtraInfo);
    magicFlag2 = MAGIC2;
    DEBUG_PRINTF_TS("");
            printSessionInfo(Info);
    }


// Save State in RTC RAM. Note that it's often the same;
// often only the frame counters change.
// [If not possible, just do nothing and make sure you return false
// from NetGetSessionState().]
void
cMyLoRaWAN::NetSaveSessionState(const SessionState &State) {
    rtcSavedSessionState = State;
    magicFlag1 = MAGIC1;
    DEBUG_PRINTF_TS("");
            printSessionState(State);
}

// Either fetch SessionState from somewhere and return true or...
// return false, which forces a re-join.
bool
cMyLoRaWAN::NetGetSessionState(SessionState &State) {
    if (magicFlag1 == MAGIC1) {
        State = rtcSavedSessionState;
        DEBUG_PRINTF_TS("o.k.");
        printSessionState(State);
        return true;
    } else {
        DEBUG_PRINTF_TS("failed");
        return false;
    }
}

// Get APB provisioning info - this is also used in OTAA after a succesful join.
// If it can be provided in OTAA mode after a restart, no re-join is needed.
bool
cMyLoRaWAN::GetAbpProvisioningInfo(AbpProvisioningInfo *pAbpInfo) {
    SessionState state;

    // ApbInfo:
    // --------
    // uint8_t         NwkSKey[16];
    // uint8_t         AppSKey[16];
    // uint32_t        DevAddr;
    // uint32_t        NetID;
    // uint32_t        FCntUp;
    // uint32_t        FCntDown;
    
    if ((magicFlag1 != MAGIC1) || (magicFlag2 != MAGIC2)) {
         return false;
    }
    DEBUG_PRINTF_TS("");

    pAbpInfo->DevAddr = rtcSavedSessionInfo.V2.DevAddr;
    pAbpInfo->NetID   = rtcSavedSessionInfo.V2.NetID;
    memcpy(pAbpInfo->NwkSKey, rtcSavedSessionInfo.V2.NwkSKey, 16);
    memcpy(pAbpInfo->AppSKey, rtcSavedSessionInfo.V2.AppSKey, 16);
    NetGetSessionState(state);
    pAbpInfo->FCntUp   = state.V1.FCntUp;
    pAbpInfo->FCntDown = state.V1.FCntDown;

    if (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG) {
        char buf[64];
        
        *buf = '\0';
        for (int i=0; i<15;i++) {
          sprintf(&buf[strlen(buf)], "%02X ", pAbpInfo->NwkSKey[i]);  
        }
        log_v("NwkSKey:\t%s", buf);

        *buf = '\0';
        for (int i=0; i<15;i++) {
          sprintf(&buf[strlen(buf)], "%02X ", pAbpInfo->AppSKey[i]);  
        }
        log_v("AppSKey:\t%s", buf);
        log_v("FCntUp:\t%d", state.V1.FCntUp);
    }
    return true;
}

/// Print date and time (i.e. local time)
void printDateTime(void) {
        struct tm timeinfo;
        char tbuf[25];
        
        time_t tnow = rtc.getLocalEpoch();
        localtime_r(&tnow, &timeinfo);
        strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
        DEBUG_PRINTF("%s", tbuf);
}

/// Determine sleep duration and enter Deep Sleep Mode
void prepareSleep(void) {
    uint32_t sleep_interval = prefs.sleep_interval;
    longSleep = false;
    #ifdef ADC_EN
        // Long sleep interval if battery is weak
        if (mySensor.getVoltage() < BATTERY_WEAK) {
            sleep_interval = prefs.sleep_interval_long;
            longSleep = true;
        }
    #endif

    // If the real time is available, align the wake-up time to the
    // to next non-fractional multiple of sleep_interval past the hour
    if (rtcLastClockSync) {
        struct tm timeinfo;
        time_t t_now = rtc.getLocalEpoch();
        localtime_r(&t_now, &timeinfo);

        sleep_interval = sleep_interval - ((timeinfo.tm_min * 60) % sleep_interval + timeinfo.tm_sec);
        sleep_interval += 20; // Added extra 20-secs of sleep to allow for slow ESP32 RTC timers
    }
    
    DEBUG_PRINTF_TS("Shutdown() - sleeping for %d s", sleep_interval);
    ESP.deepSleep(sleep_interval * 1000000LL);
}

/**
 * \fn UserRequestNetworkTimeCb
 * 
 * \brief Callback function for setting RTC from LoRaWAN network time
 * 
 * \param pVoidUserUTCTime user supplied buffer for UTC time
 * 
 * \param flagSuccess flag indicating if network time request was succesful
 */
void UserRequestNetworkTimeCb(void *pVoidUserUTCTime, int flagSuccess) {
    // Explicit conversion from void* to uint32_t* to avoid compiler errors
    uint32_t *pUserUTCTime = (uint32_t *) pVoidUserUTCTime;

    // A struct that will be populated by LMIC_getNetworkTimeReference.
    // It contains the following fields:
    //  - tLocal: the value returned by os_GetTime() when the time
    //            request was sent to the gateway, and
    //  - tNetwork: the seconds between the GPS epoch and the time
    //              the gateway received the time request
    lmic_time_reference_t lmicTimeReference;

    if (flagSuccess != 1) {
        // Most likely the service is not provided by the gateway. No sense in trying again...
        DEBUG_PRINTF_TS("didn't succeed");
        rtcSyncReq = false;
        return;
    }

    // Populate "lmic_time_reference"
    flagSuccess = LMIC_getNetworkTimeReference(&lmicTimeReference);
    if (flagSuccess != 1) {
        DEBUG_PRINTF_TS("LMIC_getNetworkTimeReference didn't succeed");
        return;
    }

    // Update userUTCTime, considering the difference between the GPS and UTC
    // epoch, and the leap seconds
    *pUserUTCTime = lmicTimeReference.tNetwork + 315964800;

    // Add the delay between the instant the time was transmitted and
    // the current time

    // Current time, in ticks
    ostime_t ticksNow = os_getTime();
    // Time when the request was sent, in ticks
    ostime_t ticksRequestSent = lmicTimeReference.tLocal;
    uint32_t requestDelaySec = osticks2ms(ticksNow - ticksRequestSent) / 1000;
    *pUserUTCTime += requestDelaySec;

    // Update the system time with the time read from the network
    rtc.setTime(*pUserUTCTime);
    
    // Save clock sync timestamp and clear flag 
    rtcLastClockSync = rtc.getLocalEpoch();
    rtcSyncReq = false;
    DEBUG_PRINTF_TS("RTC sync completed");
    printDateTime();
}

void
cMyLoRaWAN::requestNetworkTime(void) {
    LMIC_requestNetworkTime(UserRequestNetworkTimeCb, &userUTCTime);
}


void
cMyLoRaWAN::doCfgUplink(void) {
    // if busy uplinking, just skip
    if (this->m_fBusy || mySensor.isBusy()) {
        //log_d("busy");
        return;
    }
    // if LMIC is busy, just skip
    //if (LMIC.opmode & (OP_POLL | OP_TXDATA | OP_TXRXPEND)) {
    //    log_v("LMIC.opmode: 0x%02X", LMIC.opmode);
    //    return;
    //}
    if (!GetTxReady())
        return;

    log_d("--- Uplink Configuration/Status ---");
    
    uint8_t uplink_payload[5];
    uint8_t port;

    //
    // Encode data as byte array for LoRaWAN transmission
    //
    LoraEncoder encoder(uplink_payload);

    if (uplinkReq == CMD_GET_DATETIME) {
        log_d("Date/Time");
        port = 2;
        time_t t_now = rtc.getLocalEpoch();
        encoder.writeUint8((t_now >> 24) & 0xff);
        encoder.writeUint8((t_now >> 16) & 0xff);
        encoder.writeUint8((t_now >>  8) & 0xff);
        encoder.writeUint8( t_now        & 0xff);

        // time source & status, see below
        //
        // bits 0..3 time source
        //    0x00 = GPS
        //    0x01 = RTC
        //    0x02 = LORA
        //    0x03 = unsynched
        //    0x04 = set (source unknown)
        //
        // bits 4..7 esp32 sntp time status (not used)
        // TODO add flags for succesful LORA time sync/manual sync
        encoder.writeUint8((rtcSyncReq) ? 0x03 : 0x02);
    } else if (uplinkReq) {
        log_d("Config");
        port = 3;
        encoder.writeUint8(prefs.sleep_interval >> 8);
        encoder.writeUint8(prefs.sleep_interval & 0xFF);
        encoder.writeUint8(prefs.sleep_interval_long >> 8);
        encoder.writeUint8(prefs.sleep_interval_long & 0xFF);
    } else {
      log_v("");
        return;
    }

    this->m_fBusy = true;
    log_v("Trying SendBuffer: port=%d, size=%d", port, encoder.getLength());

    for (int i=0; i<encoder.getLength(); i++) {
      Serial.printf("%02X ", uplink_payload[i]);
    }
    Serial.println();
    
    // Schedule transmission
    if (! this->SendBuffer(
        uplink_payload,
        encoder.getLength(),
        // this is the completion function:
        [](void *pClientData, bool fSucccess) -> void {
            auto const pThis = (cMyLoRaWAN *)pClientData;
            pThis->m_fBusy = false;
            uplinkReq = 0;
            log_v("Sending successful");
        },
        (void *)this,
        /* confirmed */ true,
        /* port */ port
        )) {
        // sending failed; callback has not been called and will not
        // be called. Reset busy flag.
        this->m_fBusy = false;
        uplinkReq = 0;
        log_v("Sending failed");
    }
}


/****************************************************************************\
|
|	Sensor methods
|
\****************************************************************************/

void
cSensor::setup(std::uint32_t uplinkPeriodMs) {
    // set the initial time.
    this->m_uplinkPeriodMs = uplinkPeriodMs;
    this->m_tReference = millis();
    
    #ifdef ADC_EN
        // Use ADC with PIN_ADC_IN
        adc.attach(PIN_ADC_IN);
        
        if (getVoltage() < BATTERY_LOW) {
          DEBUG_PRINTF("Battery low!");
          prepareSleep();
        }
    #endif

    // Initialize your sensors here...
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

#ifdef ADC_EN
//
// Get supply / battery voltage
//
uint16_t
cSensor::getVoltage(void)
{
    float voltage_raw = 0;
    for (uint8_t i=0; i < UBATT_SAMPLES; i++) {
        voltage_raw += float(adc.readMiliVolts());
    }
    uint16_t voltage = int(voltage_raw / UBATT_SAMPLES / UBATT_DIV);
     
    DEBUG_PRINTF("Voltage = %dmV", voltage);

    return voltage;
}

//
// Get supply / battery voltage
//
uint16_t
cSensor::getVoltage(ESP32AnalogRead &adc, uint8_t samples, float divider)
{
    float voltage_raw = 0;
    for (uint8_t i=0; i < samples; i++) {
        voltage_raw += float(adc.readMiliVolts());
    }
    uint16_t voltage = int(voltage_raw / samples / divider);
     
    DEBUG_PRINTF("Voltage = %dmV", voltage);

    return voltage;
}
#endif

//
// Get battery voltage (Stub)
//
uint16_t
cSensor::getVoltageBattery(void)
{
    const uint16_t voltage = 3850;
     
    DEBUG_PRINTF("Battery Voltage = %dmV", voltage);

    return voltage;
}


//
// Get supply voltage (Stub)
//
uint16_t
cSensor::getVoltageSupply(void)
{
    const uint16_t voltage = 3300;
     
    DEBUG_PRINTF("Supply Voltage = %dmV", voltage);

    return voltage;
}

//
// Get temperature (Stub)
//
float
cSensor::getTemperature(void)
{
    const float temperature = 16.4;
    
    DEBUG_PRINTF("Outdoor Air Temperature = %.1f°C", temperature);
    
    return temperature;
}

//
// Get humidity (Stub)
//
uint8_t
cSensor::getHumidity(void)
{
    const uint8_t humidity = 42;
    
    DEBUG_PRINTF("Outdoor Humidity = %d%%", humidity);
    
    return humidity;
}

    
//
// Prepare uplink data for transmission
//
void
cSensor::doUplink(void) {
    // if busy uplinking, just skip
    if (this->m_fBusy) {
        DEBUG_PRINTF_TS("doUplink(): busy");
        return;
    }
    // if LMIC is busy, just skip
    if (LMIC.opmode & (OP_POLL | OP_TXDATA | OP_TXRXPEND)) {
        DEBUG_PRINTF_TS("doUplink(): other operation in progress");    
        return;
    }
    
    //
    // Request time and date
    //
    if (rtcSyncReq)
        myLoRaWAN.requestNetworkTime();

    // Call sensor data function stubs
    float temperature_deg_c = getTemperature();
    uint8_t humidity_percent  = getHumidity();
    uint32_t battery_voltage_v = getVoltageBattery();
    uint32_t supply_voltage_v  = getVoltageSupply();
    
    // Status flags (Examples)
    bool data_ok    = true; // validation of sensor data
    bool battery_ok = true; // sensor battery status
    
    DEBUG_PRINTF("--- Uplink Data ---");
    DEBUG_PRINTF("Air Temperature:   % 3.1f °C", temperature_deg_c);
    DEBUG_PRINTF("Humidity:           %2d   %%", humidity_percent);
    DEBUG_PRINTF("Supply  Voltage:  %4d   mV",   supply_voltage_v);
    DEBUG_PRINTF("Battery Voltage:  %4d   mV",   battery_voltage_v);
    DEBUG_PRINTF("Status:");
    DEBUG_PRINTF("    battery_ok:     %d",       battery_ok);
    DEBUG_PRINTF("    data_ok:        %d",       data_ok);
    DEBUG_PRINTF("    runtimeExpired: %d",       runtimeExpired);
    DEBUG_PRINTF("");

    
    // Serialize data into byte array
    // NOTE: 
    // For TTN MQTT integration, ttn_decoder.js must be adjusted accordingly 
    LoraEncoder encoder(loraData);
    encoder.writeBitmap(false, false, false, false, false,
                        runtimeExpired,
                        data_ok,
                        battery_ok);              // 1 Byte
    encoder.writeTemperature(temperature_deg_c);  // 2 Bytes
    encoder.writeUint8(humidity_percent);         // 1 Byte
    encoder.writeUint16(supply_voltage_v);        // 2 Bytes
    encoder.writeUint16(battery_voltage_v);       // 2 Bytes


    this->m_fBusy = true;
    
// Schedule transmission
    if (! myLoRaWAN.SendBuffer(
        loraData, encoder.getLength(),
        // this is the completion function:
        [](void *pClientData, bool fSucccess) -> void {
            auto const pThis = (cSensor *)pClientData;
            pThis->m_fBusy = false;
        },
        (void *)this,
        /* confirmed */ true,
        /* port */ 1
        )) {
        // sending failed; callback has not been called and will not
        // be called. Reset busy flag.
        this->m_fBusy = false;
    }
}
