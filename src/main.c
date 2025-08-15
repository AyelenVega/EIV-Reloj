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

/** @file  main.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "tasks_init.h"
#include "alarm_driver.h"

/* === Macros definitions ====================================================================== */
#define ALARM_POSTPONE_MINUTES 5    ///< Cantidad de minutos que se pospone la alarma
#define TICKS_PER_SECOND       1000 ///< Cantidad de ticks por segundo

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================= */
int main(void) {
    static board_t board;
    static clock_t clock;
    board = BoardCreate();
    clock = ClockCreate(TICKS_PER_SECOND, ALARM_POSTPONE_MINUTES, AlarmDriverCreate(board));
    TasksInit(clock, board);
    vTaskStartScheduler();
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
