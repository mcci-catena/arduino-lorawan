# ESP32 LoRaWAN Sensor Node Example

The provided sensor data function stubs `getTemperature()` and `getHumidity()` can be replaced by real function implementations. External sensors can be integrated via Bluetooth Low Energy (BLE), OneWire, UART, analog/digital inputs etc. 

## Features
* Pin configurations for several ESP32 boards
* Tested with [The Things Network](https://www.thethingsnetwork.org/) and [Helium Network](https://www.helium.com/) (EU868)
* Fast LoRaWAN Joining after Deep Sleep (using ESP32 RTC RAM)
* Low Power Design (using ESP32 Deep Sleep Mode)
* ESP32 Analog Digital Converter Integration (optional)
    * Supply/Battery Voltage Monitoring
    * Analog Sensor Data Aquisition 
* Time Keeping with RTC and Synchronization to Network Time
* Wake-up to fixed Time Scheme (Sleep Time is adjusted to current Up Time)
* Power Saving by Switching to longer Transmit Interval (below BATTERY_WEAK Threshold)
* Power Saving by Entering Deep Sleep Mode immediately after Wake-Up (below BATTERY_LOW Threshold)
* [Remote Configuration via LoRaWAN Downlink](README.md#remote-configuration-via-lorawan-downlink)
* Example Javascript Uplink/Downlink Formatters for The Things Network
* Debug Output using Arduino Logging Functions - using Debug Level set in Arduino IDE

## Hardware
* [LoRaWAN_Node](https://github.com/matthias-bs/LoRaWAN_Node)
* [LILYGO® TTGO LORA32](http://www.lilygo.cn/prod_view.aspx?TypeId=50060&Id=1326&FId=t3:50060:3)
    
    [TTGO LoRa32 V2.1.6 Pinout](https://github.com/lnlp/pinout-diagrams/blob/main/LoRa%20development%20boards/TTGO%20LoRa32%20V2.1.6%20Pinout%20(LLP).pdf) 

* [Heltec Wireless Stick](https://heltec.org/project/wireless-stick/)
* *Resumably* [Heltec WiFi LoRa32 V2](https://heltec.org/project/wifi-lora-32/) (confirmation wanted!!!)
* *Presumably* [Adafruit Feather ESP32-S2](https://www.adafruit.com/product/4769) with [Adafruit LoRa Radio FeatherWing](https://www.adafruit.com/product/3231) (confirmation wanted!!!)

* *Presumably* [Adafruit Feather ESP32](https://www.adafruit.com/product/3405) with [Adafruit LoRa Radio FeatherWing](https://www.adafruit.com/product/3231) (confirmation wanted!!!)

* [Thingpulse ePulse Feather](https://thingpulse.com/product/epulse-feather-low-power-esp32-development-board/) with [Adafruit LoRa Radio FeatherWing](https://www.adafruit.com/product/3231) (**see** [**#56**](https://github.com/matthias-bs/BresserWeatherSensorTTN/issues/56))

* [DFRobot FireBeetle ESP32 IoT Microcontroller](https://www.dfrobot.com/product-1590.html) with [FireBeetle Cover LoRa Radio 868MHz](https://www.dfrobot.com/product-1831.html)

See [The Things Network's](https://www.thethingsnetwork.org) [Big ESP32 + SX127x topic part 2](https://www.thethingsnetwork.org/forum/t/big-esp32-sx127x-topic-part-2/11973) for some hardware options.

See [Leonel Lopes Parente's](https://github.com/lnlp) collection of [LoRa development boards pinout-diagrams](https://github.com/lnlp/pinout-diagrams/tree/main/LoRa%20development%20boards).

### Power Supply
Mains adapter or Li-Ion battery (with or without solar charger) - depending on desired operation time and duty cycle.

## Software Build Setup

* Install the Arduino ESP32 board package in the Arduino IDE -<br>
     **Note:** You have to apply a fix in arduino-lmic (see below)
* Select the desired ESP32 board in the Arduino IDE
* Select the desired debug level in the Arduino IDE
* Install all libraries as listed in the section [Library Dependencies](README.md#library-dependencies) via the Arduino IDE Library Manager
* Configure `Arduino/libraries/MCCI_LoRaWAN_LMIC_library/project_config/lmic_project_config.h`:
   * select you appropriate region
   * `#define CFG_sx1276_radio 1`

* Add the following line to `Arduino/libraries/MCCI_LoRaWAN_LMIC_library/project_config/lmic_project_config.h`:

    `#define LMIC_ENABLE_DeviceTimeReq 1`
    
    (Otherwise requesting the time from the LoRaWAN network will not work, even if supported by the network.)
* Apply fix:
   * https://github.com/mcci-catena/arduino-lmic/issues/714#issuecomment-822051171
* Compile

### Library Dependencies

| Library                            | r: required /<br>o: optional |
| ---------------------------------- | ---------------------------- |
| MCCI Arduino Development Kit ADK   | r                            |
| MCCI LoRaWAN LMIC library          | r                            |
| MCCI Arduino LoRaWAN Library       | r                            |
| LoRa_Serialization                 | r                            |
| ESP32Time                          | r                            |
| ESP32AnalogRead                    | o                            |


## Software Customization

### Configure the LoRaWAN Network settings APPEUI, DEVEUI and APPKEY

* First you have to follow your LoRaWAN Network provider's instructions on how to configure/obtain the settings.
* Then configure the arduino_lorawan_esp32_example software accordingly:
   * Solution 1 (not recommended):
      Configure the section starting with `// APPEUI, DEVEUI and APPKEY` in [arduino_lorawan_esp32_example.ino](arduino_lorawan_esp32_example.ino)
   * Solution 2 (recommended):
   Configure the file `secrets.h` - refer to [secrets.h.template](secrets.h.template) as an example --
      ```
      #define SECRETS
   
      // deveui, little-endian
      static const std::uint8_t deveui[] = { 0xAA, 0xBB, 0xCC, 0x00, 0x00, 0xDD, 0xEE, 0xFF };
   
      // appeui, little-endian
      static const std::uint8_t appeui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

     // appkey: just a string of bytes, sometimes referred to as "big endian".
     static const std::uint8_t appkey[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00 };
     ```

### Configure the ESP32 / RF Transceiver GPIO Wiring

#### Pinout Configuration by selecting a supported Board in the Arduino IDE

By selecting a Board and a Board Revision (if available) in the Arduino IDE, a define is passed to the preprocessor/compiler. For the boards in the table below, the default configuration is assumed based in this define.

If you are not using the Arduino IDE, you can use the defines in the table below with your specific tool chain to get the same result.

If this is not what you need, you have to switch to Manual Configuration.

  | Setup                                                          | Board              | Board Revision               | Define                 | Radio Module | Notes    |
   | -------------------------------------------------------------- | ------------------ | ---------------------------- | ---------------------- | -------- | ------- |
   | [LILYGO®TTGO-LORA32 V1](https://github.com/Xinyuan-LilyGo/TTGO-LoRa-Series) | "TTGO LoRa32-OLED" | "TTGO LoRa32 V1 (No TFCard)" | ARDUINO_TTGO_LORA32_V1 | SX1276 (HPD13A) | -   |
   | [LILYGO®TTGO-LORA32 V2](https://github.com/LilyGO/TTGO-LORA32) | "TTGO LoRa32-OLED" | "TTGO LoRa32 V2"             | ARDUINO_TTGO_LoRa32_V2 | SX1276 (HPD13A) | Wire DIO1 to GPIO33 |
   | [LILYGO®TTGO-LORA32 V2.1](http://www.lilygo.cn/prod_view.aspx?TypeId=50060&Id=1271&FId=t3:50060:3) | "TTGO LoRa32-OLED" | "TTGO LoRa32 V2.1 (1.6.1)" | ARDUINO_TTGO_LoRa32_v21new |  SX1276 (HPD13A) | - |
   | [Heltec Wireless Stick](https://heltec.org/project/wireless-stick/) | "Heltec Wireless Stick" | n.a. | ARDUINO_heltec_wireless_stick |  SX1276  | - |
   | [LoRaWAN_Node](https://github.com/matthias-bs/LoRaWAN_Node)      | "FireBeetle-ESP32" | n.a.                       | ARDUINO_ESP32_DEV -> LORAWAN_NODE     | SX1276 (RFM95W) | -      |
   | [DFRobot FireBeetle ESP32 IoT Microcontroller](https://www.dfrobot.com/product-1590.html) with [FireBeetle Cover LoRa Radio 868MHz](https://www.dfrobot.com/product-1831.html) | "FireBeetle-ESP32" | n.a.                       | ARDUINO_ESP32_DEV & FIREBEETLE_ESP32_COVER_LORA | SX1276 (LoRa1276) | Wiring on the cover: <br>D2 to RESET<br>D3 to DIO0<br>D4 to CS<br>D5 to DIO1 |
   | [Adafruit Feather ESP32S2 with Adafruit LoRa Radio FeatherWing](https://github.com/matthias-bs/BresserWeatherSensorReceiver#adafruit-feather-esp32s2-with-adafruit-lora-radio-featherwing)                                | "Adafruit Feather ESP32-S2" | n.a.               | ARDUINO_<br>ADAFRUIT_FEATHER_ESP32S2   | SX1276 (RFM95W) | **No Bluetooth available!**<br>Wiring on the Featherwing:<br>E to IRQ<br>D to CS<br>C to RST<br>A to DI01 |
| [Thingpulse ePulse Feather](https://thingpulse.com/product/epulse-feather-low-power-esp32-development-board/) with [Adafruit LoRa Radio FeatherWing](https://www.adafruit.com/product/3231)     | "Adafruit ESP32 Feather" | n.a.               | ARDUINO_FEATHER_ESP32   | SX1276 (RFM95W) | Wiring on the Featherwing:<br>E to IRQ<br>D to CS<br>C to RST<br>A to DI01<br><br>**see** [**#55**](https://github.com/matthias-bs/BresserWeatherSensorTTN/issues/55) **and** [**#56**](https://github.com/matthias-bs/BresserWeatherSensorTTN/issues/56) |

If enabled in the Arduino IDE Preferences ("Verbose Output"), the preprosessor will provide some output regarding the selected configuration, e.g.

```
ARDUINO_ADAFRUIT_FEATHER_ESP32S2 defined; assuming RFM95W FeatherWing will be used
[...]
Receiver chip: [SX1276]
Pin config: RST->0 , CS->6 , GD0/G0/IRQ->5 , GDO2/G1/GPIO->11
```

#### Manual Pinout Configuration

Change the configuration in [arduino_lorawan_esp32_example.ino](arduino_lorawan_esp32_example.ino), e.g.:

```
#define PIN_LMIC_NSS      14
#define PIN_LMIC_RST      12
#define PIN_LMIC_DIO0     4
#define PIN_LMIC_DIO1     16
#define PIN_LMIC_DIO2     17
```

### Configure the RF Transceiver SPI Wiring

The board specific default SPI pin definitions (MISO, MOSI and SCK) can be found in 
https://github.com/espressif/arduino-esp32/tree/master/variants

To configure other SPI pins than the default ones... is up to you. I.e. better use the default pins unless you have a really good reason not to do so and then only if you know what you're doing! 

### Other Configuration Options

In [arduino_lorawan_esp32_example.ino](arduino_lorawan_esp32_example.ino):
* Select the desired LoRaWAN network by (un)-commenting `ARDUINO_LMIC_CFG_NETWORK_TTN` or `ARDUINO_LMIC_CFG_NETWORK_GENERIC`
* Disable features which you do not want to use
* Configure the timing parameters (if you think this is needed) 
* Configure your time zone by editing `TZ_INFO`
* Configure the ADC's input pins, dividers and oversampling settings as needed

### Change the LoRaWAN Message Payload/Encoding
In [arduino_lorawan_esp32_example.ino](arduino_lorawan_esp32_example.ino), change the code starting with
```
//
// Encode sensor data as byte array for LoRaWAN transmission
//
LoraEncoder encoder(loraData);
```
Make sure that you do not exceed the size of the LoRaWAN uplink payload buffer `loraData[PAYLOAD_SIZE]`. The payload size is limited to 51 bytes by the LMIC library (for a good reason).

If you are using an integration at the network side (such as an MQTT Integration), make sure you adjust your changes there as well - otherwise decoding the receiving/decoding the messages will fail. 

## Debug Output Configuration

See [Debug Output Configuration in Arduino IDE](DEBUG_OUTPUT.md)

## Remote Configuration via LoRaWAN Downlink

| Command / Response            | Cmd  | Port | Unit    | Data0           | Data1           | Data2           | Data3           |
| ----------------------------- | ---- | ---- | ------- | --------------- | --------------- | --------------- | --------------- |
| CMD_SET_SLEEP_INTERVAL        | 0xA8 |      | seconds | interval[15: 8] | interval[ 7: 0] |                 |                 |
| CMD_SET_SLEEP_INTERVAL_LONG   | 0xA9 |      | seconds | interval[15: 8] | interval[ 7: 0] |                 |                 |
| CMD_GET_CONFIG                | 0xB1 |         |                 |                 |                 |               |                 |                 |
|    response:               |  |  3       | seconds   | sleep_interval[15: 8] | sleep_interval[ 7: 0] | sleep_interval_long[15: 8] | sleep_interval_long[ 7: 0] |
| CMD_GET_DATETIME              | 0x86 |         |                 |                 |                |                 |
|   response:            |      | 2       | epoch   | unixtime[31:24] | unixtime[23:16] | unixtime[15:8] | unixtime[7:0] |
| CMD_SET_DATETIME              | 0x88 |         |epoch   | unixtime[31:24] | unixtime[23:16] | unixtime[15:8] | unixtime[7:0] |

:warning: Confirmed downlinks should not be used! (see [here](https://www.thethingsnetwork.org/forum/t/how-to-purge-a-scheduled-confirmed-downlink/56849/7) for an explanation.)

### Remote Configuration with The Things Network Console
#### With Payload Formatter

* see [ttn_downlink_formatter.js](scripts/ttn_downlink_formatter.js) for syntax of commands and responses
* see [The Things Network MQTT Integration and Payload Formatters](README.md#the-things-network-mqtt-integration-payload-formatters)

##### Example 1: Set SLEEP_INTERVAL to 360 seconds
1. Build command sequence as JSON string: `{"cmd": "CMD_SET_SLEEP_INTERVAL", "interval": 360}`
2. Send command sequence via The Things Network Console
![TTN Downlink as JSON](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/e61ca412-713b-4972-8c4f-f0068068c323)

##### Example 2: Set Date/Time
1. Get epoch (e.g. from https://www.epochconverter.com) (Example: 1692729833); add an offset (estimated) for time until received (Example: + 64 seconds => 16927298**97**) 
2. Build command sequence as JSON string: {"cmd": "CMD_SET_DATETIME", "epoch": 1692729897} 
3. Send command sequence via The Things Network Console

#### Without Payload Formatter
##### Example 1: Set SLEEP_INTERVAL to 360 seconds
1. Convert interval to hex: 360 = 0x0168
2. Build command sequence: "CMD_SET_SLEEP_INTERVAL 360 secs" -> 0xA8 0x01 0x68
3. Send command sequence via The Things Network Console
![TTN Downlink as Hex](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/67544195-c2cd-4118-8de5-1f1c0facacdb)

##### Example 2: Set Date/Time
1. Get epoch (e.g. from https://www.epochconverter.com/hex) (Example: 0x63B2BC32); add an offset (estimated) for time until received (Example: + 64 / 0x40 seconds => 0x63B2BC**7**2) 
2. Build command sequence: "CMD_SET_DATETIME 0x63B2BC72" -> 0x88 0x63 0xB2 0xBC 0x72
3. Send command sequence via The Things Network Console

### Remote Configuration with Helium Console
#### With Payload Formatter
_To be done_

#### Without Payload Formatter 
##### Example 1: Set SLEEP_INTERVAL to 360 seconds
1. Convert interval to hex: 360 = 0x0168
2. Build command sequence: "CMD_SET_SLEEP_INTERVAL 360 secs" -> 0xA8 0x01 0x68
3. Convert command sequence to Base64 encoding: 0xA8 0x01 0x68 -> "qAFo" ([Base64 Guru](https://base64.guru/converter/encode/hex))
4. Send command sequence via Helium Console
    ![Helium_Add_Downlink_Payload](https://user-images.githubusercontent.com/83612361/210183244-a2d109bc-6782-4f83-b406-7f6e0b17eda1.png)

##### Example 2: Set Date/Time

1. Get epoch (e.g. from https://www.epochconverter.com/hex) (Example: 0x63B2BC32); add an offset (estimated) for time until received (Example: + 64 / 0x40 seconds => 0x63B2BC**7**2) 
2. Build command sequence: "CMD_SET_DATETIME 0x63B2BC72" -> 0x88 0x63 0xB2 0xBC 0x72
3. Convert command sequence to Base64 encoding: 0x88 0x63 0xB2 0xBC 0x72 -> "iGOyvHI="
4. Send command sequence e.g. via Helium Console


## MQTT Integration

### The Things Network MQTT Integration and Payload Formatters

#### Uplink Formatter

Decode uplink payload (a sequence of bytes) into data structures which are readable/suitable for further processing.

In The Things Network Console:
1. Go to "Payload formatters" -> "Uplink"
2. Select "Formatter type": "Custom Javascript formatter"
3. "Formatter code": Paste [ttn_uplink_formatter.js](scripts/ttn_uplink_formatter.js)
4. Apply "Save changes"

![TTN Uplink Formatter](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/38b66478-688a-4028-974a-c517cddae662)

#### Downlink Formatter

Encode downlink payload from JSON to a sequence of bytes.

In The Things Network Console:
1. Go to "Payload formatters" -> "Downlink"
2. Select "Formatter type": "Custom Javascript formatter"
3. "Formatter code": Paste [ttn_downlink_formatter.js](ttn_downlink_formatter.js)
4. Apply "Save changes"

**Note:** The actual payload depends on the options selected in the Arduino software - the decoder must be edited accordingly (add or remove data types and JSON identifiers - see [ttn_uplink_formatter.js](httn_uplink_formatter.js) line 316ff).

#### MQTT Integration
TTN provides an MQTT broker.
How to receive and decode the payload with an MQTT client -
see https://www.thethingsnetwork.org/forum/t/some-clarity-on-mqtt-topics/44226/2

V3 topic:

`v3/<ttn app id><at symbol>ttn/devices/<ttn device id>/up`

  
v3 message key field jsonpaths:
  
```
<ttn device id> = .end_device_ids.device_id
<ttn app id> = .end_device_ids.application_ids.application_id  // (not including the <at symbol>ttn in the topic)
<payload> = .uplink_message.frm_payload
```  


JSON-Path with Uplink-Decoder (see [ttn_uplink_formatter.js](ttn_uplink_formatter.js))

`.uplink_message.decoded_payload.bytes.<variable>`

### Helium Network MQTT Integration and Message Decoder

Please refer to https://docs.helium.com/use-the-network/console/integrations/mqtt/.

Add an MQTT integration in the Helium console - the "Endpoint" is in fact an MQTT broker you have to provide:
![Helium_MQTT_Integration](https://user-images.githubusercontent.com/83612361/195050719-8562ad0e-5523-436f-8b61-e4b15b08d6de.png)

Add [scripts/helium_decoder.js](scripts/helium_decoder.js) in the Helium console as custom function:
![Helium_Decoder_Function](https://user-images.githubusercontent.com/83612361/195045593-d6c76e0c-1d87-410a-b941-8636b35d601a.png)

**Note:** The actual payload depends on the options selected in the Arduino software - the decoder must be edited accordingly (add or remove data types and JSON identifiers).

Add your function to the flow:
![Helium_Decoder_Flow](https://user-images.githubusercontent.com/83612361/195047042-6a8d9dfe-61f6-43e3-ac51-b917d01ff237.png)

Example decoder output (JSON):
```
rx = {[...],"decoded":{"payload":{"air_temp_c":"13.5","battery_v":4197,"humidity":72,
"status_node":{"res7":false,"res6":false,"res5":false,"res4":false,"res3":false,"runtime_exp":false,"data_ok":true,"battery_ok":true},
"supply_v":4210},
"status":"success"}, [...]
```
### Datacake Integration

YouTube Video: [Get started for free with LoRaWaN on The Things Network and Datacake IoT Platform](https://youtu.be/WGVFgYp3k3s)

### Decoder Scripts Summary

#### [ttn_uplink_formatter.js](scripts/ttn_uplink_formatter.js)
    
[The Things Network](https://www.thethingsnetwork.org/) uplink formatter for sensor data / node status messages and response messages triggered by uplink command messages.

#### [ttn_downlink_formatter.js](ttn_downlink_formatter.js)

[The Things Network](https://www.thethingsnetwork.org/) downlink formatter for sending commands as JSON strings to the device.


## References

Based on
* [MCCI LoRaWAN LMIC library](https://github.com/mcci-catena/arduino-lmic) by Thomas Telkamp and Matthijs Kooijman / Terry Moore, MCCI
* [MCCI Arduino LoRaWAN Library](https://github.com/mcci-catena/arduino-lorawan) by Terry Moore, MCCI
* [BresserWeatherSensorTTN](https://github.com/matthias-bs/BresserWeatherSensorTTN) by Matthias Prinke
* [Lora-Serialization](https://github.com/thesolarnomad/lora-serialization) by Joscha Feth
* [ESP32Time](https://github.com/fbiego/ESP32Time) by Felix Biego
* [ESP32AnalogRead](https://github.com/madhephaestus/ESP32AnalogRead) by Kevin Harrington (madhephaestus)
