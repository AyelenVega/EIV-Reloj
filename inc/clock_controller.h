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

#ifndef CLOCK_CONTROLLER_H_
#define CLOCK_CONTROLLER_H_

/** @file clock_controller.h
 ** @brief
 **/

/* === Headers files inclusions ==================================================================================== */
#include "digital.h"
#include "clock.h"
#include "bsp.h"
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

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

/* === Public variable declarations ================================================================================ */
extern board_t board;
extern clock_t clock;
extern mode_t mode;
extern const struct clock_alarm_driver_s driver_alarm;
extern uint32_t system_ticks;
extern uint32_t inactivity_count;
extern bool half_second_flag;

/* === Public function declarations ================================================================================ */
/**
 * @brief Cambia de modo de funcionamiento del reloj.
 *
 * @param value Nuevo modo a establecer.
 */
void ChangeMode(mode_t value);

/**
 * @brief Máquina de Estados Finitos (MEF) del reloj.
 */
void Clock_MEF(void);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_CONTROLLER_H_ */
