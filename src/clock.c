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

/** @file  clock.c
 ** @brief
 **/

/* === Headers files inclusions ==================================================================================== */
#include "clock.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */
//! Estructura que define a un reloj
struct clock_s {
    uint32_t clock_ticks;
    uint32_t ticks_per_second;
    uint32_t alarm_postponed_times;
    uint32_t postponed_minutes;
    clock_time_t current_time;
    clock_time_t alarm_time;
    bool valid;
    bool valid_alarm;
    bool alarm_active;
    bool alarm_enable;
    clock_alarm_driver_t driver;
};

/* === Private function declarations =============================================================================== */
/**
 * @brief Verifica si la hora dada en BCD es válida
 *
 * @param time hora en BCD
 * @return true Si la hora es válida
 * @return false Si la hora es inválida
 */
bool ClockIsValidTime(const clock_time_t * time);

/**
 * @brief Convierte segundos a BCD
 *
 * @param time tiempo en BCD
 * @param seconds cantidad de segundos
 */
void SecondsToBCD(clock_time_t * time, uint32_t seconds);

/**
 * @brief Convierte BCD a segundos
 *
 * @param time tiempo en BCD
 * @return uint32_t tiempo en segundos
 */
uint32_t BCDToSeconds(const clock_time_t * time);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */
bool ClockIsValidTime(const clock_time_t * time) {
    bool result;
    if (time->time.hours[1] > 2 || (time->time.hours[1] == 2 && time->time.hours[0] > 3) || time->time.hours[0] > 9 ||
        time->time.minutes[1] > 5 || time->time.minutes[0] > 9 || time->time.seconds[1] > 5 ||
        time->time.seconds[0] > 9) {
        result = false;
    } else {
        result = true;
    }
    return result;
}
uint32_t BCDToSeconds(const clock_time_t * time) {
    uint32_t hours = 10 * time->time.hours[1] + time->time.hours[0];
    uint32_t minutes = 10 * time->time.minutes[1] + time->time.minutes[0];
    uint32_t seconds = 10 * time->time.seconds[1] + time->time.seconds[0];

    return hours * 3600 + minutes * 60 + seconds;
}

void SecondsToBCD(clock_time_t * time, uint32_t total_seconds) {
    uint32_t hours = (total_seconds / 3600) % 24;
    total_seconds %= 3600;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;

    time->time.hours[1] = hours / 10;
    time->time.hours[0] = hours % 10;
    time->time.minutes[1] = minutes / 10;
    time->time.minutes[0] = minutes % 10;
    time->time.seconds[1] = seconds / 10;
    time->time.seconds[0] = seconds % 10;
}

/* === Public function implementation ============================================================================== */

clock_t ClockCreate(uint32_t ticks_per_second, uint32_t alarm_postponed_minutes, clock_alarm_driver_t driver_alarm) {
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s));
    self->valid = false;
    self->ticks_per_second = ticks_per_second;
    self->valid_alarm = false;
    self->alarm_enable = false;
    self->alarm_active = false;
    self->alarm_postponed_times = 0;
    self->postponed_minutes = alarm_postponed_minutes;
    self->driver = driver_alarm;
    return self;
}
bool ClockGetTime(clock_t self, clock_time_t * result) {
    if (self == NULL || result == NULL) {
        return false;
    }

    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid;
}
bool ClockSetTime(clock_t self, const clock_time_t * new_time) {
    if (new_time == NULL || self == NULL) {
        return false;
    }

    if (ClockIsValidTime(new_time)) {
        self->valid = true;
        memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    } else {
        self->valid = false;
    }

    return self->valid;
}

bool ClockSetAlarm(clock_t self, const clock_time_t * new_alarm) {
    if (self == NULL || new_alarm == NULL) {
        return false;
    }

    if (ClockIsValidTime(new_alarm)) {
        self->valid_alarm = true;
        self->alarm_enable = true;
        self->alarm_active = false;
        memcpy(&self->alarm_time, new_alarm, sizeof(clock_time_t));
    } else {
        self->valid_alarm = false;
    }

    return self->valid_alarm;
}
bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time) {
    if (self == NULL || alarm_time == NULL) {
        return false;
    }
    memcpy(alarm_time, &self->alarm_time, sizeof(clock_time_t));
    return self->valid_alarm;
}

bool ClockIsAlarmActive(clock_t self) {
    return self->alarm_active;
}

bool ClockActivateAlarm(clock_t self, bool activate) {
    if (self == NULL) {
        return false;
    }

    uint32_t alarm_seconds = BCDToSeconds(&self->alarm_time);
    uint32_t postpone_seconds = 60 * self->postponed_minutes * self->alarm_postponed_times;

    if (activate) {
        if (memcmp(self->current_time.bcd, self->alarm_time.bcd, sizeof(clock_time_t)) == 0 &&
            self->alarm_enable == true) {
            self->alarm_active = true;

            self->driver->AlarmActivate(self);
        }
    } else {
        self->alarm_active = false;
        self->driver->AlarmDeactivate(self);
        alarm_seconds = (alarm_seconds - postpone_seconds) % (24 * 3600);
        SecondsToBCD(&self->alarm_time, alarm_seconds);
        self->alarm_postponed_times = 0;
    }

    return true;
}

bool ClockAlarmEnable(clock_t self, bool enable) {
    if (!self) {
        return false;
    }
    self->alarm_enable = enable;
    return true;
}
bool ClockIsAlarmEnabled(clock_t self) {
    return self->alarm_enable;
}

bool ClockPostponeAlarm(clock_t self) {
    if (!self) {
        return false;
    }

    uint32_t postpone_seconds = 60 * self->postponed_minutes;

    self->alarm_postponed_times++;
    self->alarm_active = false;
    uint32_t alarm_seconds = BCDToSeconds(&self->alarm_time);
    alarm_seconds = (alarm_seconds + postpone_seconds) % (24 * 3600);
    SecondsToBCD(&self->alarm_time, alarm_seconds);
    return true;
}
bool ClockNewTick(clock_t self) {
    if (!self) {
        return false;
    }

    self->clock_ticks++;
    if (self->clock_ticks == self->ticks_per_second) {
        self->clock_ticks = 0;

        uint32_t total_seconds = BCDToSeconds(&self->current_time);
        total_seconds = (total_seconds + 1) % (24 * 3600);
        SecondsToBCD(&self->current_time, total_seconds);
        ClockActivateAlarm(self, true);
    }
    return true;
}

/* === End of documentation ========================================================================================
 */
