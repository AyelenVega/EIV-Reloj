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

#ifndef BUTTON_TASKS_H_
#define BUTTON_TASKS_H_

/** @file button_tasks.h
 ** @brief Declaraciones del módulo para la gestion de una pantalla multiplexada de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */
#include "FreeRTOS.h"
#include "digital.h"
#include "event_groups.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#define BUTTON_EVENT_0         (1 << 0)
#define BUTTON_EVENT_1         (1 << 1)
#define BUTTON_EVENT_2         (1 << 2)
#define BUTTON_EVENT_3         (1 << 3)
#define BUTTON_EVENT_4         (1 << 4)
#define BUTTON_EVENT_5         (1 << 5)

#define ANY_EVENT              0xFF

#define BUTTON_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)
/* === Public data type declarations =============================================================================== */
typedef struct button_task_args_s {
    EventGroupHandle_t clock_events;
    uint8_t event_bit;
    digital_input_t button;
} * button_task_args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
void ButtonTask(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_TASKS_H_ */