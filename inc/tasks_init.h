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

#ifndef TASKS_INIT_H_
#define TASKS_INIT_H_

/** @file tasks_init.h
 ** @brief Declaraciones para el modulo de gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include "button_tasks.h"
#include "clock_tasks.h"
#include "display_tasks.h"
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#define BUTTON_ACCEPT        BUTTON_EVENT_0
#define BUTTON_CANCEL        BUTTON_EVENT_1
#define BUTTON_INCREMENT     BUTTON_EVENT_2
#define BUTTON_DECREMENT     BUTTON_EVENT_3
#define BUTTON_SET_TIME      BUTTON_EVENT_4
#define BUTTON_SET_ALARM     BUTTON_EVENT_5
#define HALF_SECOND          TICKS_EVENTS_6
#define INACTIVITY_TIME      TICKS_EVENTS_7
#define RESET_THIRTY_SECONDS TICKS_EVENT_8

#define DELAY_SET_TIME       3000 ///< Cantidad de tiempo que tiene que presionarse el boton de setear tiempo en ms
#define DELAY_SET_ALARM      3000 ///< Cantidad de tiempo que tiene que presionarse el boton de setear alarma en ms

/* === Public data type declarations =============================================================================== */

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief Inicializa todas las tareas
 *
 * @param clock Puntero al objeto reloj
 * @param board Puntero al objeto board
 */
void TasksInit(clock_t clock, board_t board);
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* TASKS_INIT_H_ */
