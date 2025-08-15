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

/** @file  display_tasks.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "display_tasks.h"
#include <stdint.h>

/* === Macros definitions ====================================================================== */
#define INACTIVITY_COUNT 30000 ///< Tiempo maximo de inactividad en ms

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================= */
void RefreshDisplay(void * pointer) {
    static uint32_t half_second_count = 0;
    static uint32_t thirty_seconds_count = 0;
    refresh_task_args_t args = pointer;
    TickType_t last_value = xTaskGetTickCount();
    TickType_t ticks = pdMS_TO_TICKS(1);

    while (true) {
        if (xEventGroupWaitBits(args->clock_events, TICKS_EVENTS_8, pdTRUE, pdFALSE, 0)) {
            xEventGroupClearBits(args->clock_events, TICKS_EVENTS_8);
            thirty_seconds_count = 0;
        }
        if (xSemaphoreTake(args->display_mutex, portMAX_DELAY)) {
            DisplayRefresh(args->board->display);
            xSemaphoreGive(args->display_mutex);
        }
        ClockNewTick(args->clock);
        half_second_count++;
        thirty_seconds_count++;

        if (half_second_count >= 500) {
            half_second_count = 0;
            xEventGroupSetBits(args->clock_events, TICKS_EVENTS_6);
        }
        if (thirty_seconds_count >= INACTIVITY_COUNT) {
            thirty_seconds_count = 0;
            xEventGroupSetBits(args->clock_events, TICKS_EVENTS_7);
        }
        xTaskDelayUntil(&last_value, ticks);
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */