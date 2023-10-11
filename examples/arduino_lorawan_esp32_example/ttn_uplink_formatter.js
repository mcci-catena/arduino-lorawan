///////////////////////////////////////////////////////////////////////////////
// ttn_uplink_formatter.js
// 
// Bresser 5-in-1/6-in-1 868 MHz Weather Sensor Radio Receiver 
// based on ESP32 and RFM95W - 
// sends data to a LoRaWAN network (e.g. The Things Network)
//
// This script allows to decode payload received from The Things Network -
// data (sent at fixed intervals) and responses (sent upon request) -
// from bytes to JSON.
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
// ttn_decoder_fp.js
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

function ttn_decoder(bytes, port) {
    // bytes is of type Buffer
    
    var rtc_source_code = {
        0x00: "GPS",
        0x01: "RTC",
        0x02: "LORA",
        0x03: "unsynched",
        0x04: "set (source unknown)"
    };

    var rtc_source = function (bytes) {
        if (bytes.length !== rtc_source.BYTES) {
            throw new Error('rtc_source must have exactly 1 byte');
        }
        return rtc_source_code[bytes[0]];
    };
    rtc_source.BYTES = 1;
    
    var bytesToInt = function(bytes) {
    var i = 0;
    for (var x = 0; x < bytes.length; x++) {
        i |= +(bytes[x] << (x * 8));
    }
    return i;
    };

    // Big Endian
    var bytesToIntBE = function(bytes) {
    var i = 0;
    for (var x = 0; x < bytes.length; x++) {
        i |= +(bytes[x] << ((bytes.length-1-x) * 8));
    }
    return i;
    };
    
    var unixtime = function(bytes) {
    if (bytes.length !== unixtime.BYTES) {
        throw new Error('Unix time must have exactly 4 bytes');
    }
        //return bytesToInt(bytes);
        dateObj = new Date(bytesToInt(bytes) * 1000);
        return dateObj.toISOString();
    };
    unixtime.BYTES = 4;

    var uint8 = function(bytes) {
    if (bytes.length !== uint8.BYTES) {
        throw new Error('int must have exactly 1 byte');
    }
    return bytesToInt(bytes);
    };
    uint8.BYTES = 1;

    var uint16 = function(bytes) {
    if (bytes.length !== uint16.BYTES) {
        throw new Error('int must have exactly 2 bytes');
    }
    return bytesToInt(bytes);
    };
    uint16.BYTES = 2;

    var uint16fp1 = function(bytes) {
    if (bytes.length !== uint16.BYTES) {
        throw new Error('int must have exactly 2 bytes');
    }
    var res = bytesToInt(bytes) * 0.1;
    return res.toFixed(1);
    };
    uint16fp1.BYTES = 2;

    var uint32 = function(bytes) {
    if (bytes.length !== uint32.BYTES) {
        throw new Error('int must have exactly 4 bytes');
    }
    return bytesToInt(bytes);
    };
    uint32.BYTES = 4;

    var uint16BE = function(bytes) {
    if (bytes.length !== uint16BE.BYTES) {
        throw new Error('int must have exactly 2 bytes');
    }
    return bytesToIntBE(bytes);
    };
    uint16BE.BYTES = 2;
    
    var uint32BE = function(bytes) {
    if (bytes.length !== uint32BE.BYTES) {
        throw new Error('int must have exactly 4 bytes');
    }
    return bytesToIntBE(bytes);
    };
    uint32BE.BYTES = 4;
    
    var latLng = function(bytes) {
    if (bytes.length !== latLng.BYTES) {
        throw new Error('Lat/Long must have exactly 8 bytes');
    }

    var lat = bytesToInt(bytes.slice(0, latLng.BYTES / 2));
    var lng = bytesToInt(bytes.slice(latLng.BYTES / 2, latLng.BYTES));

    return [lat / 1e6, lng / 1e6];
    };
    latLng.BYTES = 8;

    var temperature = function(bytes) {
    if (bytes.length !== temperature.BYTES) {
        throw new Error('Temperature must have exactly 2 bytes');
    }
    var isNegative = bytes[0] & 0x80;
    var b = ('00000000' + Number(bytes[0]).toString(2)).slice(-8)
            + ('00000000' + Number(bytes[1]).toString(2)).slice(-8);
    if (isNegative) {
        var arr = b.split('').map(function(x) { return !Number(x); });
        for (var i = arr.length - 1; i > 0; i--) {
        arr[i] = !arr[i];
        if (arr[i]) {
            break;
        }
        }
        b = arr.map(Number).join('');
    }
    var t = parseInt(b, 2);
    if (isNegative) {
        t = -t;
    }
    t = t / 1e2;
    return t.toFixed(1);
    };
    temperature.BYTES = 2;

    var humidity = function(bytes) {
    if (bytes.length !== humidity.BYTES) {
        throw new Error('Humidity must have exactly 2 bytes');
    }

    var h = bytesToInt(bytes);
    return h / 1e2;
    };
    humidity.BYTES = 2;

    // Based on https://stackoverflow.com/a/37471538 by Ilya Bursov
    // quoted by Arjan here https://www.thethingsnetwork.org/forum/t/decode-float-sent-by-lopy-as-node/8757
    function rawfloat(bytes) {
    if (bytes.length !== rawfloat.BYTES) {
        throw new Error('Float must have exactly 4 bytes');
    }
    // JavaScript bitwise operators yield a 32 bits integer, not a float.
    // Assume LSB (least significant byte first).
    var bits = bytes[3]<<24 | bytes[2]<<16 | bytes[1]<<8 | bytes[0];
    var sign = (bits>>>31 === 0) ? 1.0 : -1.0;
    var e = bits>>>23 & 0xff;
    var m = (e === 0) ? (bits & 0x7fffff)<<1 : (bits & 0x7fffff) | 0x800000;
    var f = sign * m * Math.pow(2, e - 150);
    return f.toFixed(1);
    }
    rawfloat.BYTES = 4;

    var bitmap_node = function (byte) {
        if (byte.length !== bitmap_node.BYTES) {
            throw new Error('Bitmap must have exactly 1 byte');
        }
        var i = bytesToInt(byte);
        var bm = ('00000000' + Number(i).toString(2)).substr(-8).split('').map(Number).map(Boolean);

        return ['res7', 'res6', 'res5', 'res4', 'res3', 'runtime_exp', 'data_ok', 'battery_ok']
            .reduce(function (obj, pos, index) {
                obj[pos] = bm[index];
                return obj;
            }, {});
    };
    bitmap_node.BYTES = 1;

    var bitmap_sensors = function (byte) {
        if (byte.length !== bitmap_sensors.BYTES) {
            throw new Error('Bitmap must have exactly 1 byte');
        }
        var i = bytesToInt(byte);
        var bm = ('00000000' + Number(i).toString(2)).substr(-8).split('').map(Number).map(Boolean);
        return ['res0', 'res1', 'res2', 'res3', 'res4', 'res5', 'res6', 'res7']
            .reduce(function (obj, pos, index) {
                obj[pos] = bm[index];
                return obj;
            }, {});
    };
    bitmap_sensors.BYTES = 1;

    var decode = function(bytes, mask, names) {

    var maskLength = mask.reduce(function(prev, cur) {
        return prev + cur.BYTES;
    }, 0);
    if (bytes.length < maskLength) {
        throw new Error('Mask length is ' + maskLength + ' whereas input is ' + bytes.length);
    }

    names = names || [];
    var offset = 0;
    return mask
        .map(function(decodeFn) {
        var current = bytes.slice(offset, offset += decodeFn.BYTES);
        return decodeFn(current);
        })
        .reduce(function(prev, cur, idx) {
        prev[names[idx] || idx] = cur;
        return prev;
        }, {});
    };

    if (typeof module === 'object' && typeof module.exports !== 'undefined') {
        module.exports = {
            unixtime: unixtime,
            uint8: uint8,
            uint16: uint16,
            uint32: uint32,
            uint16BE: uint16BE,
            uint32BE: uint32BE,
            temperature: temperature,
            humidity: humidity,
            latLng: latLng,
            bitmap_node: bitmap_node,
            bitmap_sensors: bitmap_sensors,
            rawfloat: rawfloat,
            uint16fp1: uint16fp1,
            rtc_source: rtc_source,
            decode: decode
        };
    }
    
    if (port === 1) {
        return decode(
            bytes,
        [   bitmap_node,        temperature,        uint8,          uint16,         uint16
        ], // types
        [   'status_node',      'air_temp_c',       'humidity',     'supply_v',     'battery_v'
        ] // JSON elements
        );
    } else if  (port === 2) {
        return decode(
            bytes,
            [uint32BE, rtc_source
            ],
            ['unixtime', 'rtc_source'
            ]
        );
    } else if (port === 3) {
        return decode(
            bytes,
            [uint16BE, uint16BE 
            ],
            ['sleep_interval', 'sleep_interval_long'
            ]
        );
    }

}


function decodeUplink(input) {
  return {
    data: {
      bytes: ttn_decoder(input.bytes, input.fPort)
    },
    warnings: [],
    errors: []
  };
}
