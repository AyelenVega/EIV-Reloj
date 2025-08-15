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

/** @file  button_tasks.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "button_tasks.h"

/* === Macros definitions ====================================================================== */
#define BUTTON_SCAN_DELAY 100

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================= */

void ButtonTask(void * pointer) {
    button_task_args_t args = pointer;

    while (true) {
        while (!DigitalInputGetIsActive(args->button)) {
            vTaskDelay(pdMS_TO_TICKS(BUTTON_SCAN_DELAY));
        };

        xEventGroupSetBits(args->clock_events, args->event_bit);

        while (DigitalInputGetIsActive(args->button)) {
            vTaskDelay(pdMS_TO_TICKS(BUTTON_SCAN_DELAY));
        };
    }
}
void ButtonPressedForLongTimeTask(void * pointer) {
    button_task_args_t args = pointer;

    while (true) {
        if (DigitalInputGetIsActive(args->button)) {
            if (!args->state->pressed) {
                args->state->pressed = true;
                args->state->start_time = xTaskGetTickCount();
            } else {
                if ((xTaskGetTickCount() - args->state->start_time) >= pdMS_TO_TICKS(args->state->delay_ms) &&
                    !args->state->already_pressed) {
                    args->state->already_pressed = true;
                    xEventGroupSetBits(args->clock_events, args->event_bit);
                }
            }
        } else {
            args->state->pressed = false;
            args->state->already_pressed = false;
            args->state->start_time = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(BUTTON_SCAN_DELAY));
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */