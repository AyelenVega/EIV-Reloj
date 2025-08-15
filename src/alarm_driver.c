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

/** @file  alarm_driver.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "alarm_driver.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */
static board_t board_local;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */
/**
 * @brief Activa la alarma
 *
 */
static void AlarmActivate(void);

/**
 * @brief Desactiva la alarma
 *
 */
static void AlarmDeactivate(void);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
static void AlarmActivate(void) {
    DigitalOutputActivate(board_local->buzzer);
    DigitalOutputActivate(board_local->led1);
    DigitalOutputActivate(board_local->led2);
    DigitalOutputActivate(board_local->led3);
}

static void AlarmDeactivate(void) {
    DigitalOutputDeactivate(board_local->buzzer);
    DigitalOutputDeactivate(board_local->led1);
    DigitalOutputDeactivate(board_local->led2);
    DigitalOutputDeactivate(board_local->led3);
}

/* === Public function implementation ========================================================= */
clock_alarm_driver_t AlarmDriverCreate(board_t board) {
    static const struct clock_alarm_driver_s driver = {
        .AlarmActivate = AlarmActivate,
        .AlarmDeactivate = AlarmDeactivate,
    };
    board_local = board;
    return &driver;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */