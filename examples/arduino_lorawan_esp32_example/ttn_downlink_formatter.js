///////////////////////////////////////////////////////////////////////////////
// ttn_downlink_formatter.js
// 
// LoRaWAN node example based on ESP32 and RFM95W - 
// sends data to a LoRaWAN network (e.g. The Things Network)
//
// This script allows to send downlink data to the The Things Network with
// commands, parameters and responses are encoded in JSON format.
// Commands shall be sent via FPort=1.
//
// Commands:
// ----------
// {"cmd": "CMD_SET_SLEEP_INTERVAL", "interval": <interval_in_seconds>}
// {"cmd": "CMD_SET_SLEEP_INTERVAL_LONG", "interval": <interval_in_seconds>}
// {"cmd": "CMD_GET_DATETIME"}
// {"cmd": "CMD_SET_DATETIME", "epoch": <epoch>}
// {"cmd": "CMD_GET_CONFIG"}
//
// Responses:
// -----------
// CMD_GET_CONFIG   -> FPort=3: {"reserved": 0, 
//                               "sleep_interval": <interval_in_seconds>,
//                               "sleep_interval_long": <interval_in_seconds>}
// 
// CMD_GET_DATETIME -> FPort=2: {"epoch": <unix_epoch_time>, "rtc_source":<rtc_source>}
//
// <interval>           : 0...65535
// <epoch>              : unix epoch time, see https://www.epochconverter.com/
// <flags>              : 0...15 (1: hourly / 2: daily / 4: weekly / 8: monthly)
// <rtc_source>         : 0x00: GPS / 0x01: RTC / 0x02: LORA / 0x03: unsynched / 0x04: set (source unknown)
//
//
// Based on:
// ---------
// https://www.thethingsindustries.com/docs/integrations/payload-formatters/javascript/downlink/
//
// created: 08/2023
//
//
// MIT License
//
// Copyright (c) 2023 Matthias Prinke
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
// 20230821 Created
//
// ToDo:
// -  
//
///////////////////////////////////////////////////////////////////////////////

// Commands
const cmd_code = new Map([
    ["CMD_SET_SLEEP_INTERVAL", 0xA8],
    ["CMD_SET_SLEEP_INTERVAL_LONG", 0xA9],
    ["CMD_GET_DATETIME", 0x86],
    ["CMD_SET_DATETIME", 0x88],
    ["CMD_GET_CONFIG", 0xB1],
]);

// Source of Real Time Clock setting
var rtc_source_code = {
    0x00: "GPS",
    0x01: "RTC",
    0x02: "LORA",
    0x03: "unsynched",
    0x04: "set (source unknown)"
};

function bytesToInt(bytes) {
    var i = 0;
    for (var x = 0; x < bytes.length; x++) {
        i |= +(bytes[x] << (x * 8));
    }
    return i;
}

// Big Endian
function bytesToIntBE(bytes) {
    var i = 0;
    for (var x = 0; x < bytes.length; x++) {
        i |= +(bytes[x] << ((bytes.length - 1 - x) * 8));
    }
    return i;
}

function uint8(bytes) {
    if (bytes.length !== 1) {
        throw new Error('uint8 must have exactly 1 byte');
    }
    return bytesToInt(bytes);
}

function uint16BE(bytes) {
    if (bytes.length !== 2) {
        throw new Error('uint16BE must have exactly 2 bytes');
    }
    return bytesToIntBE(bytes);
}

function uint32BE(bytes) {
    if (bytes.length !== 4) {
        throw new Error('uint32BE must have exactly 4 bytes');
    }
    return bytesToIntBE(bytes);
}

// Encode Downlink from JSON to bytes
function encodeDownlink(input) {
    if ((input.data.cmd === "CMD_SET_SLEEP_INTERVAL") ||
        (input.data.cmd === "CMD_SET_SLEEP_INTERVAL_LONG")) {

        return {
            bytes: [cmd_code.get(input.data.cmd),
            input.data.interval >> 8,
            input.data.interval & 0xFF
            ],
            fPort: 1,
            warnings: [],
            errors: []
        };
    }
    else if (input.data.cmd === "CMD_SET_DATETIME") {
        return {
            bytes: [cmd_code.get(input.data.cmd),
            input.data.epoch >> 24,
            (input.data.epoch >> 16) & 0xFF,
            (input.data.epoch >> 8) & 0xFF,
            (input.data.epoch & 0xFF)],
            fPort: 1,
            warnings: [],
            errors: []
        };
    }
    else if ((input.data.cmd === "CMD_GET_CONFIG") ||
        (input.data.cmd === "CMD_GET_DATETIME")) {
        return {
            bytes: [cmd_code.get(input.data.cmd)],
            fPort: 1,
            warnings: [],
            errors: []
        };
    } else {
        return {
            bytes: [],
            errors: ["unknown command"],
            fPort: 1,
            warnings: []
        };
    }
}

// Decode Downlink from bytes to JSON
function decodeDownlink(input) {
    switch (input.fPort) {
        case 1:
            for (const x of cmd_code.keys()) {
                if (input.bytes[0] == cmd_code.get(x)) {
                    return {
                        cmd: x
                    };
                }
            }
            return {
                cmd: [],
                errors: ["unknown command"]
            };
        case 2:
            return {
                data: {
                    unixtime: uint32BE(input.bytes.slice(0, 3)),
                    rtc_source: rtc_source_code[input.bytes[4]]
                }
            };
        case 3:
            return {
                data: {
                    reserved: uint8(input.bytes[0]),
                    sleep_interval: uint16BE(input.bytes.slice(1, 2)),
                    sleep_interval_long: uint16BE(input.bytes.slice(3, 4))
                }
            };
        default:
            return {
                errors: ["unknown FPort"]
            };
    }
}
