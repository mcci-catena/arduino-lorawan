///////////////////////////////////////////////////////////////////////////////
// pico_rtc_utils.cpp
// 
// RTC utility functions for RP2040
//
// Sleep/wakeup scheme based on
// https://github.com/lyusupov/SoftRF/tree/master/software/firmware/source/libraries/RP2040_Sleep
// by Linar Yusupov
//
// Using code from pico-extras:
// https://github.com/raspberrypi/pico-extras/blob/master/src/rp2_common/pico_sleep/include/pico/sleep.h
// https://github.com/raspberrypi/pico-extras/blob/master/src/rp2_common/pico_sleep/sleep.c
// https://github.com/raspberrypi/pico-extras/blob/master/src/rp2_common/hardware_rosc/include/hardware/rosc.h
// https://github.com/raspberrypi/pico-extras/blob/master/src/rp2_common/hardware_rosc/rosc.c
//
// created: 10/2023
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
//
// 20231006 Created
//
// ToDo:
// - 
//
///////////////////////////////////////////////////////////////////////////////
#if defined(ARDUINO_ARCH_RP2040)

#include "pico_rtc_utils.h"

struct tm *datetime_to_tm(datetime_t *dt, struct tm *ti)
{
  ti->tm_sec = dt->sec;
  ti->tm_min = dt->min;
  ti->tm_hour = dt->hour;
  ti->tm_mday = dt->day;
  ti->tm_mon = dt->month - 1;
  ti->tm_year = dt->year - 1900;
  ti->tm_wday = dt->dotw;

  return ti;
}

datetime_t *tm_to_datetime(struct tm *ti, datetime_t *dt)
{
  dt->sec = ti->tm_sec;
  dt->min = ti->tm_min;
  dt->hour = ti->tm_hour;
  dt->day = ti->tm_mday;
  dt->month = ti->tm_mon + 1;
  dt->year = ti->tm_year + 1900;
  dt->dotw = ti->tm_wday;

  return dt;
}

void print_dt(datetime_t dt) {
    log_i("%4d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
}

void print_tm(struct tm ti) {
    log_i("%4d-%02d-%02d %02d:%02d:%02d", ti.tm_year+1900, ti.tm_mon+1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
}

time_t datetime_to_epoch(datetime_t *dt, time_t *epoch) {
        struct tm ti;
        datetime_to_tm(dt, &ti);
        
        // Apply daylight saving time according to timezone and date
        ti.tm_isdst = -1;

        // Convert to epoch
        time_t _epoch = mktime(&ti);

        if (epoch) {
          *epoch = _epoch;
        }

        return _epoch;
}

datetime_t *epoch_to_datetime(time_t *epoch, datetime_t *dt) {
    struct tm ti;

    // Apply daylight saving time according to timezone and date
    ti.tm_isdst = -1;

    // Convert epoch to struct tm
    localtime_r(epoch, &ti);

    // Convert struct tm to datetime_t
    tm_to_datetime(&ti, dt);

    return dt;
}

// Sleep for <duration> seconds
void pico_sleep(unsigned duration) {
    datetime_t dt;
    rtc_get_datetime(&dt);
    log_i("RTC time:");
    print_dt(dt);

    time_t now;
    datetime_to_epoch(&dt, &now);
    
    // Add sleep_duration
    time_t wakeup = now + duration;

    epoch_to_datetime(&wakeup, &dt);

    log_i("Wakeup time:");
    print_dt(dt);

    Serial.flush();
    Serial1.end();
    Serial2.end();
    Serial.end();

    // From
    // https://github.com/lyusupov/SoftRF/tree/master/software/firmware/source/libraries/RP2040_Sleep
    // also see src/pico_rtc
    // --8<-----
    #if defined(USE_TINYUSB)
        // Disable USB
        USBDevice.detach();
    #endif /* USE_TINYUSB */

    sleep_run_from_xosc();

    sleep_goto_sleep_until(&dt, NULL);

    // back from dormant state
    rosc_enable();
    clocks_init();
    // --8<-----
}
#endif