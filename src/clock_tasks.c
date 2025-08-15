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

/** @file  clock_tasks.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "clock_tasks.h"
#include "button_tasks.h"
#include "display_tasks.h"
/* === Macros definitions ====================================================================== */
#define FLASH_FREQUENCY 200 ///< Cantidad de veces que se quiere que el digito este prendido

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */
static const uint8_t MINUTE_LIMIT[] = {6, 0};
static const uint8_t HOUR_LIMIT[] = {2, 4};

/* === Private function declarations =========================================================== */

/**
 * @brief Incrementa el valor de un numero en BCD respetando un limite maximo
 *
 * @param number Valor actual del número en BCD
 * @param limit Valor maximo que puede tomar el número
 */
void BCDIncrement(uint8_t number[2], const uint8_t limit[2]);

/**
 * @brief Decrementa el valor de un numero en BCD
 *
 * @param number Valor actual del número en BCD
 * @param limit Valor maximo que puede tomar el número
 */
void BCDDecrement(uint8_t number[2], const uint8_t limit[2]);

/**
 * @brief Convierte un tiempo en formato clock_time_t a un arreglo BCD de 4 dígitos.
 *  El arreglo resultante tiene el siguiente formato:
 * `BCD[0] = decena de la hora`
 * `BCD[1] = unidad de la hora`
 * `BCD[2] = decena del minuto`
 * `BCD[3] = unidad del minuto`
 *
 * @param time  Puntero a la estructura de tiempo  clock_time_t
 * @param BCD Puntero a un arreglo de 4 elementos donde se almacenará el tiempo
 */
void ClockTimeToBCD(clock_time_t * time, uint8_t * BCD);
/**
 * @brief Convierte un tiempo de un arreglo BCD de 4 dígitos a clock_time_t
 *
 * @param time Puntero a la estructura de tiempo  clock_time_t
 * @param BCD Puntero a un arreglo de 4 elementos donde se almacena el tiempo en formato [decena_hora, unidad_hora,
 * decena_minuto, unidad_minuto]
 */
void BCDToClockTime(clock_time_t * time, uint8_t * BCD);

/**
 * @brief  Extrae horas y minutos desde un arreglo BCD de 4 elementos
 *
 * @param hour Arreglo de 2 elementos donde se almacenarán las horas [decena, unidad]
 * @param minute Arreglo de 2 elementos donde se almacenarán los minutos [decena, unidad]
 * @param BCD Arreglo donde se guarda el tiempo en formato [decena_hora, unidad_hora, decena_minuto, unidad_minuto]
 */
void BCDtoHourAndMinute(uint8_t hour[], uint8_t minute[], uint8_t BCD[]);

/**
 * @brief Convierte horas y minutos en arreglos separados a un arreglo BCD de 4 dígitos
 *
 * @param hour Arreglo de 2 elementos con la hora [decena, unidad]
 * @param minute Arreglo de 2 elementos con los minutos [decena, unidad]
 * @param BCD Arreglo donde se almacenará el resultado en formato [hh, hh, mm, mm]
 */
void HourAndMinuteToBCD(uint8_t hour[], uint8_t minute[], uint8_t BCD[]);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
void BCDIncrement(uint8_t number[2], const uint8_t limit[2]) {
    number[1]++;
    if (number[1] > 9) {
        number[1] = 0;
        number[0]++;
    }
    if (number[0] >= limit[0] && number[1] >= limit[1]) {
        number[1] = 0;
        number[0] = 0;
    }
}

void BCDDecrement(uint8_t number[2], const uint8_t limit[2]) {
    number[1]--;
    if (number[1] > 9) {
        number[1] = 9;
        number[0]--;
    }
    if (number[0] > limit[0] && number[1] > limit[1]) {
        if (limit[0] == 2 && limit[1] == 4) {
            number[0] = 2;
            number[1] = 3;
        }
        if (limit[0] == 6 && limit[1] == 0) {
            number[0] = 5;
            number[1] = 9;
        }
    }
}

void ClockTimeToBCD(clock_time_t * time, uint8_t * BCD) {
    BCD[0] = time->time.hours[1];
    BCD[1] = time->time.hours[0];
    BCD[2] = time->time.minutes[1];
    BCD[3] = time->time.minutes[0];
}

void BCDToClockTime(clock_time_t * time, uint8_t * BCD) {
    time->time.hours[0] = BCD[1];
    time->time.hours[1] = BCD[0];
    time->time.minutes[0] = BCD[3];
    time->time.minutes[1] = BCD[2];
    time->time.seconds[0] = 0;
    time->time.seconds[1] = 0;
}

void BCDtoHourAndMinute(uint8_t hour[], uint8_t minute[], uint8_t BCD[]) {
    hour[0] = BCD[0];
    hour[1] = BCD[1];
    minute[0] = BCD[2];
    minute[1] = BCD[3];
}

void HourAndMinuteToBCD(uint8_t hour[], uint8_t minute[], uint8_t BCD[]) {
    BCD[0] = hour[0];
    BCD[1] = hour[1];
    BCD[2] = minute[0];
    BCD[3] = minute[1];
}

void ChangeMode(mode_t value, clock_task_args_t args) {
    args->current_mode = value;
    if (xSemaphoreTake(args->display_mutex, portMAX_DELAY)) {
        switch (args->current_mode) {
        case UNSET_TIME:
            DisplayFlashDigits(args->board->display, 0, 3, FLASH_FREQUENCY);
            DisplayFlashPoint(args->board->display, 0b00000010, FLASH_FREQUENCY);
            DisplaySetPoint(args->board->display, 0, false);
            DisplaySetPoint(args->board->display, 2, false);
            DisplaySetPoint(args->board->display, 3, false);
            break;
        case SHOW_TIME:
            DisplayFlashDigits(args->board->display, 0, 0, 0);
            DisplaySetPoint(args->board->display, 2, false);
            break;
        case SET_TIME_MINUTE:
            DisplayFlashDigits(args->board->display, 2, 3, FLASH_FREQUENCY);
            DisplayFlashPoint(args->board->display, 0b00001111, 0);
            DisplaySetPoint(args->board->display, 0, false);
            DisplaySetPoint(args->board->display, 1, false);
            DisplaySetPoint(args->board->display, 2, false);
            DisplaySetPoint(args->board->display, 3, false);
            break;
        case SET_TIME_HOUR:
            DisplayFlashDigits(args->board->display, 0, 1, FLASH_FREQUENCY);
            DisplayFlashPoint(args->board->display, 0b00001111, 0);
            DisplaySetPoint(args->board->display, 0, false);
            DisplaySetPoint(args->board->display, 1, false);
            DisplaySetPoint(args->board->display, 2, false);
            DisplaySetPoint(args->board->display, 3, false);
            break;
        case SET_ALARM_MINUTE:
            DisplayFlashDigits(args->board->display, 2, 3, FLASH_FREQUENCY);
            DisplaySetPoint(args->board->display, 0, true);
            DisplaySetPoint(args->board->display, 1, true);
            DisplaySetPoint(args->board->display, 2, true);
            DisplaySetPoint(args->board->display, 3, true);
            break;
        case SET_ALARM_HOUR:
            DisplayFlashDigits(args->board->display, 0, 1, FLASH_FREQUENCY);
            DisplaySetPoint(args->board->display, 0, true);
            DisplaySetPoint(args->board->display, 1, true);
            DisplaySetPoint(args->board->display, 2, true);
            DisplaySetPoint(args->board->display, 3, true);
            break;
        default:
            break;
        }
        xSemaphoreGive(args->display_mutex);
    }
}

/* === Public function implementation ========================================================= */
/*
 */
void ClockTask(void * pointer) {
    static clock_time_t time = {0};
    clock_task_args_t args = pointer;
    EventBits_t clock_events;

    static uint8_t hour[2] = {0};
    static uint8_t minute[2] = {0};
    static uint8_t digits[4] = {0};
    bool alarm_already_set = false;
    static bool point_state_show_time = false;
    args->current_mode = UNSET_TIME;

    ChangeMode(UNSET_TIME, args);

    while (true) {

        xEventGroupClearBits(args->clock_events, ANY_EVENT);

        clock_events = xEventGroupWaitBits(args->clock_events, ANY_EVENT, pdTRUE, pdFALSE, pdMS_TO_TICKS(100));
        if (clock_events &
            (BUTTON_EVENT_0 | BUTTON_EVENT_1 | BUTTON_EVENT_2 | BUTTON_EVENT_3 | BUTTON_EVENT_4 | BUTTON_EVENT_5)) {
            xEventGroupSetBits(args->clock_events, TICKS_EVENTS_8);
        }
        switch (args->current_mode) {
        case UNSET_TIME:
            if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                ClockGetTime(args->clock, &time);
                ClockTimeToBCD(&time, digits);
                BCDtoHourAndMinute(hour, minute, digits);
                DisplayWrite(args->board->display, digits, sizeof(digits));
                xSemaphoreGive(args->display_mutex);
            }
            if (clock_events & BUTTON_EVENT_4) {
                ChangeMode(SET_TIME_MINUTE, args);
            }
            break;
        case SHOW_TIME:
            if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                ClockGetTime(args->clock, &time);
                ClockTimeToBCD(&time, digits);
                BCDtoHourAndMinute(hour, minute, digits);
                DisplayWrite(args->board->display, digits, sizeof(digits));
                DisplaySetPoint(args->board->display, 0, ClockIsAlarmActive(args->clock));
                DisplaySetPoint(args->board->display, 3, ClockIsAlarmEnabled(args->clock));
                if (clock_events & TICKS_EVENTS_6) {
                    point_state_show_time = !point_state_show_time;
                    DisplaySetPoint(args->board->display, 1, point_state_show_time);
                }
                xSemaphoreGive(args->display_mutex);
            }

            if (clock_events & BUTTON_EVENT_4) {
                ChangeMode(SET_TIME_MINUTE, args);
            }
            if (clock_events & BUTTON_EVENT_5) {
                if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                    ClockGetAlarm(args->clock, &time);
                    ClockTimeToBCD(&time, digits);
                    BCDtoHourAndMinute(hour, minute, digits);
                    HourAndMinuteToBCD(hour, minute, digits);
                    DisplayWrite(args->board->display, digits, sizeof(digits));
                    xSemaphoreGive(args->display_mutex);
                }
                ChangeMode(SET_ALARM_MINUTE, args);
            }
            if (!ClockIsAlarmActive(args->clock) && alarm_already_set) { // Solamente puedo habilitar y deshabilitar la
                                                                         // alarma cuando ya se la seteo por primera vez
                if (clock_events & BUTTON_EVENT_0) {                     // Aceptar
                    ClockAlarmEnable(args->clock, true);
                }
                if (clock_events & BUTTON_EVENT_1) { // cancelar
                    ClockAlarmEnable(args->clock, false);
                }
            } else {
                if (clock_events & BUTTON_EVENT_0) {
                    ClockPostponeAlarm(args->clock);
                }
                if (clock_events & BUTTON_EVENT_1) {
                    ClockActivateAlarm(args->clock, false);
                }
            }
            break;

        case SET_TIME_MINUTE:
            if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                HourAndMinuteToBCD(hour, minute, digits);
                DisplayWrite(args->board->display, digits, sizeof(digits));
                xSemaphoreGive(args->display_mutex);
            }
            if (clock_events & BUTTON_EVENT_2) { // Incrementar
                BCDIncrement(minute, MINUTE_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_3) { // Decrementar
                BCDDecrement(minute, MINUTE_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_0) { // Aceptar
                ChangeMode(SET_TIME_HOUR, args);
            }
            if (clock_events & (BUTTON_EVENT_1 | TICKS_EVENTS_7)) { // cancelar
                if (ClockGetTime(args->clock, &time)) {
                    ChangeMode(SHOW_TIME, args);
                } else {
                    ChangeMode(UNSET_TIME, args);
                }
            }

            break;
        case SET_TIME_HOUR:
            if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                HourAndMinuteToBCD(hour, minute, digits);
                DisplayWrite(args->board->display, digits, sizeof(digits));
                xSemaphoreGive(args->display_mutex);
            }
            if (clock_events & BUTTON_EVENT_2) { // Incrementra
                BCDIncrement(hour, HOUR_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_3) { // Decrementar
                BCDDecrement(hour, HOUR_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_0) { // Aceptar
                if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                    HourAndMinuteToBCD(hour, minute, digits);
                    BCDToClockTime(&time, digits);
                    ClockSetTime(args->clock, &time);
                    xSemaphoreGive(args->display_mutex);
                }
                ChangeMode(SHOW_TIME, args);
            }
            if (clock_events & (BUTTON_EVENT_1 | TICKS_EVENTS_7)) { // cancelar
                if (ClockGetTime(args->clock, &time)) {
                    ChangeMode(SHOW_TIME, args);
                } else {
                    ChangeMode(UNSET_TIME, args);
                }
            }

            break;
        case SET_ALARM_MINUTE:
            if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                HourAndMinuteToBCD(hour, minute, digits);
                DisplayWrite(args->board->display, digits, sizeof(digits));
                xSemaphoreGive(args->display_mutex);
            }
            if (clock_events & BUTTON_EVENT_2) { // Incrementra
                BCDIncrement(minute, MINUTE_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_3) { // Decrementar
                BCDDecrement(minute, MINUTE_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_0) { // Aceptar
                ChangeMode(SET_ALARM_HOUR, args);
            }
            if (clock_events & (BUTTON_EVENT_1 | TICKS_EVENTS_7)) { // cancelar
                if (ClockGetTime(args->clock, &time)) {
                    ChangeMode(SHOW_TIME, args);
                } else {
                    ChangeMode(UNSET_TIME, args);
                }
            }

            break;
        case SET_ALARM_HOUR:
            if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                HourAndMinuteToBCD(hour, minute, digits);
                DisplayWrite(args->board->display, digits, sizeof(digits));
                xSemaphoreGive(args->display_mutex);
            }
            if (clock_events & BUTTON_EVENT_2) { // Incrementra
                BCDIncrement(hour, HOUR_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_3) { // Decrementar
                BCDDecrement(hour, HOUR_LIMIT);
            }
            if (clock_events & BUTTON_EVENT_0) { // Aceptar
                if (xSemaphoreTake(args->display_mutex, pdMS_TO_TICKS(100))) {
                    HourAndMinuteToBCD(hour, minute, digits);
                    BCDToClockTime(&time, digits);
                    ClockSetAlarm(args->clock, &time);
                    xSemaphoreGive(args->display_mutex);
                }
                alarm_already_set = true;
                ChangeMode(SHOW_TIME, args);
            }
            if (clock_events & (BUTTON_EVENT_1 | TICKS_EVENTS_7)) { // cancelar

                ChangeMode(SHOW_TIME, args);
            }

            break;
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */