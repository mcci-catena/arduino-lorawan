///////////////////////////////////////////////////////////////////////////////
// pico_rtc_utils.h
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
#include <Arduino.h>
#include <time.h>
#include <pico/stdlib.h>
#include <hardware/rtc.h>
#include "pico_sleep.h"
#include "pico_rosc.h"
#include "../logging.h"

#ifndef PICO_RTC_UTILS_H
#define PICO_RTC_UTILS_H

struct tm *datetime_to_tm(datetime_t *dt, struct tm *ti);
datetime_t *tm_to_datetime(struct tm *ti, datetime_t *dt);
time_t datetime_to_epoch(datetime_t *dt, time_t *epoch);
datetime_t *epoch_to_datetime(time_t *epoch, datetime_t *dt);

void print_dt(datetime_t dt);
void print_tm(struct tm ti);

void pico_sleep(unsigned duration);

#endif // PICO_RTC_UTILS_H
#endif // defined(ARDUINO_ARCH_RP2040)
