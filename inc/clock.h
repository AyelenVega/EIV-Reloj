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

#ifndef CLOCK_H_
#define CLOCK_H_

/** @file clock.h
 ** @brief
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdbool.h>
#include <stdint.h>
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */
//! Estructura que define el tiempo en BCD
typedef union {
    struct {
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    } time;
    uint8_t bcd[6];
} clock_time_t;

typedef struct clock_s * clock_t;

//! Puntero a una funcion que activa la alarma
typedef void (*clock_alarm_activate_t)(void);

//! Puntero a una funcion que desactiva la alarma
typedef void (*clock_alarm_deactivate_t)(void);

//! Driver
typedef struct clock_alarm_driver_s {
    clock_alarm_activate_t AlarmActivate;
    clock_alarm_deactivate_t AlarmDeactivate;
} const * clock_alarm_driver_t;
/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief Crea el objeto reloj
 * *
 * @param ticks_per_second Cantidad de ticks por segundo
 * @param alarm_postponed_minutes Cantidad de minutos que se puedo posponer la alarma
 * @param driver_alarm Driver de la alarma
 * @return clock_t Puntero al objeto creado
 */
clock_t ClockCreate(uint32_t ticks_per_second, uint32_t alarm_postponed_minutes, clock_alarm_driver_t driver_alarm);

/**
 * @brief Obtiene la hora actual
 *
 * @param self Puntero al objeto reloj
 * @param result Hora actual
 * @return true Si la hora es válida
 * @return false Si la hora es invalida
 */
bool ClockGetTime(clock_t self, clock_time_t * result);
/**
 * @brief Fija la hora del reloj a una hora dada
 *
 * @param self Puntero al objeto reloj
 * @param new_time Hora a la que se setara el reloj
 * @return true Si la hora que se intento fijar es válida
 * @return false Si la hora que se intento fijar NO es válida
 */
bool ClockSetTime(clock_t self, const clock_time_t * new_time);

/**
 * @brief Genera los ticks del reloj
 *
 * @param self  Puntero al objeto reloj
 */
bool ClockNewTick(clock_t self);

/**
 * @brief Setea la alarma a una hora dada
 *
 * @param self Puntero al objeto reloj
 * @param new_alarm Hora a la que se seteará la alarma
 * @return true Si la alarma que se intentó fijar es válida
 * @return false Si la alarma que se intentó fijar NO es válida
 */
bool ClockSetAlarm(clock_t self, const clock_time_t * new_alarm);

/**
 * @brief Lee el valor de la hora de la alarma
 *
 * @param self Puntero al objeto reloj
 * @param alarm_time Hora a la que esta seteada la alarma
 * @return true Si la alarma leida es válida
 * @return false Si la alarma leida no es válida
 */
bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time);
/**
 * @brief Verifica si la alarma esta activada
 *
 * @param self Puntero al objeto reloj
 * @return true Si esta activada
 * @return false Si NO esta activada
 */
bool ClockIsAlarmActive(clock_t self);

/**
 * @brief Activa o desactiva la alarma
 *
 * @param self Puntero al objeto reloj
 * @param activate True si se quiere activar la alarma, false si se la quiere desactivar
 * @return true Si pudo activarse o desactivarse
 * @return false Si no se pudo activar o desactivar
 */
bool ClockActivateAlarm(clock_t self, bool activate);

/**
 * @brief Habilita o deshabilita la alarma
 *
 * @param self Puntero al objeto reloj
 * @param enable True si se quiere habiliatar la alarma, false si se la quiere desactivar
 * @return true Si se pudo activar o desactivar la alarma
 * @return false Si no se pudo activar o desactivar la alarma
 */
bool ClockAlarmEnable(clock_t self, bool enable);

/**
 * @brief Verifica si la alarma esta habilitada
 *
 * @param self Puntero al objeto reloj
 * @return true Si esta habilitada
 * @return false Si esta deshabilitada
 */
bool ClockIsAlarmEnabled(clock_t self);
/**
 * @brief Pospone la alarma una cantidad fija de minutos
 *
 * @param self Puntero a objeto relon
 * @return true Si se pudo posponer la alarma
 * @return false Si no se pudo posponer la alarma
 */
bool ClockPostponeAlarm(clock_t self);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H_ */
