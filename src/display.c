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

/** @file  display.c
 ** @brief Codigo fuente para el modulo de gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "poncho.h"

/* === Macros definitions ========================================================================================== */
#ifndef DISPLAY_MAX_DIGITS
#define DISPLAY_MAX_DIGITS 8
#endif

/* === Private data type declarations ============================================================================== */

struct display_s {
    uint8_t digits;
    uint8_t current_digit;
    struct {
        uint8_t from;
        uint8_t to;
        uint8_t count;
        uint16_t frecuency;
    } digit_flashing;
    struct {
        uint8_t mask;
        uint8_t count;
        uint16_t frecuency;
    } point_flashing;
    uint8_t point_set_mask;
    display_driver_t driver;
    uint8_t value[DISPLAY_MAX_DIGITS];
};

static const uint8_t DIGIT_MAP[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,             // 9

};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */
display_t DisplayCreate(uint8_t digits, display_driver_t driver) {
    display_t self = malloc(sizeof(struct display_s));
    if (digits > DISPLAY_MAX_DIGITS) {
        digits = DISPLAY_MAX_DIGITS;
    }
    if (self != NULL) {
        self->digits = digits;
        self->driver = driver;
        self->current_digit = 0;
        self->digit_flashing.count = 0;
        self->digit_flashing.frecuency = 0;
        self->point_set_mask = 0;
    }
    return self;
}
void DisplayWrite(display_t self, uint8_t value[], uint8_t size) {
    memset(self->value, 0, sizeof(self->value));
    if (size > self->digits) {
        size = self->digits;
    }
    for (size_t i = 0; i < size; i++) {
        self->value[i] = DIGIT_MAP[value[i]];
    }
}
void DisplayRefresh(display_t self) {
    uint8_t segments;

    self->driver->DigitsTurnOff();
    self->current_digit = (self->current_digit + 1) % self->digits;

    segments = self->value[self->current_digit];
    if (self->digit_flashing.frecuency != 0) {
        if (self->current_digit == 0) {
            self->digit_flashing.count = (self->digit_flashing.count + 1) % (self->digit_flashing.frecuency);
        }
        if (self->digit_flashing.count < (self->digit_flashing.frecuency / 2)) {
            if ((self->current_digit >= self->digit_flashing.from) &&
                (self->current_digit <= self->digit_flashing.to)) {
                segments = 0;
            }
        }
    }

    if (self->point_flashing.frecuency != 0) {
        if (self->current_digit == 0) {
            self->point_flashing.count = (self->point_flashing.count + 1) % self->point_flashing.frecuency;
        }
        if (self->point_flashing.count >= (self->point_flashing.frecuency / 2)) {
            if (((1 << self->current_digit) & self->point_flashing.mask)) {
                segments |= SEGMENT_P;
                DisplaySetPoint(self, self->current_digit, false);
            }
        }
    }

    if ((self->point_set_mask & (1 << self->current_digit)) != 0) {
        segments |= SEGMENT_P;
    }

    self->driver->SegmentsUpdate(segments);
    self->driver->DigitsTurnOn(self->current_digit);
}

int DisplayFlashDigits(display_t self, uint8_t from, uint8_t to, uint16_t time_on) {
    int result = 0;
    if ((from > to) || (from >= DISPLAY_MAX_DIGITS) || (to >= DISPLAY_MAX_DIGITS)) {
        result = -1;
    } else if (!self) {
        result = -1;
    } else {
        self->digit_flashing.from = from;
        self->digit_flashing.to = to;
        self->digit_flashing.frecuency = 2 * time_on;
        self->digit_flashing.count = 0;
    }
    return result;
}
int DisplayFlashPoint(display_t self, uint8_t mask, uint16_t time_on) {
    int result = 0;
    if (!self) {
        result = -1;
    } else {
        self->point_flashing.mask = mask;
        self->point_flashing.frecuency = 2 * time_on;
        self->point_flashing.count = 0;
    }
    return result;
}

int DisplaySetPoint(display_t self, uint8_t digit, bool on) {
    int result = 0;
    if (!self || digit >= self->digits) {
        result = -1;
    }
    if (on) {
        self->point_set_mask |= (1 << digit);
    } else {
        self->point_set_mask &= ~(1 << digit);
    }
    return result;
}

/* === End of documentation ======================================================================================== */
