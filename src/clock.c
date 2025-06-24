/*********************************************************************************************************************
Copyright (c) 2025, María Ayelén Vega Caro <ayelenvegacaro@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file  reloj.c
 ** @brief
 **/

/* === Headers files inclusions ==================================================================================== */
#include "clock.h"
#include <stddef.h>
#include <string.h>
/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */
struct clock_s {
    uint16_t clock_ticks;
    uint16_t ticks_per_second;
    clock_time_t current_time;
    clock_time_t alarm_time;
    bool valid;
    bool valid_alarm;
    bool alarm_active;
    bool alarm_enable;
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */
clock_t ClockCreate(uint16_t ticks_per_second) {
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s));
    self->valid = false;
    self->ticks_per_second = ticks_per_second;
    return self;
}
bool ClockGetTime(clock_t self, clock_time_t * result) {
    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid;
}
bool ClockSetTime(clock_t self, const clock_time_t * new_time) {
    self->valid = true;
    memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    return self->valid;
}

bool ClockSetAlarm(clock_t self, const clock_time_t * new_alarm) {
    self->valid_alarm = true;
    self->alarm_enable = true;
    self->alarm_active = false;
    memcpy(&self->alarm_time, new_alarm, sizeof(clock_time_t));
    return true;
}
bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time) {
    memcpy(alarm_time, &self->alarm_time, sizeof(clock_time_t));
    return self->valid_alarm;
}

bool ClockIsAlarmActive(clock_t self) {
    return self->alarm_active;
}

bool ClockActivateAlarm(clock_t self) {
    if (memcmp(self->current_time.bcd, self->alarm_time.bcd, sizeof(clock_time_t)) == 0 && self->alarm_enable == true) {
        self->alarm_active = true;
    }
    return true;
}

bool ClockAlarmEnable(clock_t self, bool enable) {
    self->alarm_enable = enable;
    return true;
}
bool ClockIsAlarmEnabled(clock_t self) {
    return self->alarm_enable;
}

bool ClockPostponeAlarm(clock_t self, uint8_t postpone_seconds) {
    self->alarm_active = false;
    self->alarm_time.time.seconds[0] += postpone_seconds % 10;
    self->alarm_time.time.seconds[1] += postpone_seconds / 10;

    if (self->alarm_time.time.seconds[0] > 9) {
        self->alarm_time.time.seconds[0] -= 10;
        self->alarm_time.time.seconds[1]++;
    }
    if (self->alarm_time.time.seconds[1] > 5) {
        self->alarm_time.time.seconds[1] -= 6;
        self->alarm_time.time.minutes[0]++;
    }
    if (self->alarm_time.time.minutes[0] > 9) {
        self->alarm_time.time.minutes[0] -= 10;
        self->alarm_time.time.minutes[1]++;
    }
    if (self->alarm_time.time.minutes[1] > 5) {
        self->alarm_time.time.minutes[1] -= 6;
        self->alarm_time.time.hours[0]++;
        if ((self->alarm_time.time.hours[1] >= 2 && self->alarm_time.time.hours[0] > 3) ||
            self->alarm_time.time.hours[0] > 9) {
            self->alarm_time.time.hours[0] = 0;
            self->alarm_time.time.hours[1]++;
            if (self->alarm_time.time.hours[1] > 2 ||
                (self->alarm_time.time.hours[1] == 2 && self->alarm_time.time.hours[0] > 3)) {
                self->alarm_time.time.hours[1] = 0;
                self->alarm_time.time.hours[0] = 0;
            }
        }
    }

    return true;
}

void ClockNewTick(clock_t self) {
    self->clock_ticks++;
    if (self->clock_ticks == self->ticks_per_second) {
        self->clock_ticks = 0;
        self->current_time.time.seconds[0]++;
        if (self->current_time.time.seconds[0] > 9) {
            self->current_time.time.seconds[0] = 0;
            self->current_time.time.seconds[1]++;
            if (self->current_time.time.seconds[1] > 5) {
                self->current_time.time.seconds[1] = 0;
                self->current_time.time.minutes[0]++;
                if (self->current_time.time.minutes[0] > 9) {
                    self->current_time.time.minutes[0] = 0;
                    self->current_time.time.minutes[1]++;
                    if (self->current_time.time.minutes[1] > 5) {
                        self->current_time.time.minutes[1] = 0;
                        self->current_time.time.hours[0]++;
                        if ((self->current_time.time.hours[1] == 2 && self->current_time.time.hours[0] > 3) ||
                            self->current_time.time.hours[0] > 9) {
                            self->current_time.time.hours[0] = 0;
                            self->current_time.time.hours[1]++;
                            if (self->current_time.time.hours[1] > 2) {
                                self->current_time.time.hours[1] = 0;
                                self->current_time.time.hours[0] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    ClockActivateAlarm(self);
}
/* === End of documentation ========================================================================================
 */
