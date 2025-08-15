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

#ifndef CLOCK_TASKS_H_
#define CLOCK_TASKS_H_

/** @file clock_tasks.h
 ** @brief Declaraciones para el modulo de gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "display.h"
#include "bsp.h"
#include "clock.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#define CLOCK_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */
/**
 * @brief Modos de funcionamiento del reloj
 *
 */
typedef enum {
    UNSET_TIME,       ///< La hora todavía no fue configurada
    SHOW_TIME,        ///< Modo de visualización del tiempo
    SET_TIME_MINUTE,  ///< Modo de configuración de los minutos del tiempo
    SET_TIME_HOUR,    ///< Modo de configuración de las horas del tiempo
    SET_ALARM_MINUTE, ///< Modo de configuración de los minutos de la alarma
    SET_ALARM_HOUR,   ///< Modo de configuración de las horas de la alarma
} mode_t;

/**
 * @brief Estructura con los argumentos que se deben enviar a las tareas relacionadas con el reloj
 *
 */
typedef struct clock_task_args_s {
    board_t board;
    clock_t clock;
    mode_t current_mode;
    EventGroupHandle_t clock_events;
    SemaphoreHandle_t display_mutex;
} * clock_task_args_t;
/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Tarea que maneja el funcionamiento del reloj
 *
 * @param pointer
 */
void ClockTask(void * pointer);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_TASKS_H_ */
