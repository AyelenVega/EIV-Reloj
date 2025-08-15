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

#ifndef DISPLAY_TASKS_H_
#define DISPLAY_TASKS_H_

/** @file digital_tasks.h
 ** @brief Declaraciones para el modulo de gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "bsp.h"
#include "clock.h"
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#define REFRESH_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)
#define TICKS_EVENTS_6     (1 << 6) // Evento para controlar paso de medio segundo
#define TICKS_EVENTS_7     (1 << 7) // Evento para detectar tiempo de inactividad
#define TICKS_EVENTS_8     (1 << 8) // evento para reiniciar contador de inactividad

/* === Public data type declarations =============================================================================== */
/**
 * @brief Estructura con los argumentos que se deben pasar a las tareas realacionadas con el refresco de pantalla
 *
 */
typedef struct refresh_task_args_s {
    SemaphoreHandle_t display_mutex;
    EventGroupHandle_t clock_events;
    board_t board;
    clock_t clock;
} * refresh_task_args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief Tarea para refrescar la pantalla
 *
 * @param args
 */
void RefreshDisplay(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_TASKS_H_ */
