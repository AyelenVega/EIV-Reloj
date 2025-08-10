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

/** @file  clock_controller.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "clock_controller.h"
#include <stdbool.h>
#include <stdlib.h>

/* === Macros definitions ====================================================================== */
#define INACTIVITY_COUNT 30000 ///< Tiempo maximo de inactividad en ms
#define DELAY_SET_TIME   3000  ///< Cantidad de tiempo que tiene que presionarse el boton de setear tiempo en ms
#define DELAY_SET_ALARM  3000  ///< Cantidad de tiempo que tiene que presionarse el boton de setear alarma en ms
#define FLASH_FREQUENCY  200   ///< Cantidad de veces que se quiere que el digito este prendido

/* === Private data type declarations ========================================================== */
/**
 * @brief Estructura que representa el estado de un botón
 *
 */
typedef struct {
    bool pressed;         ///< Indica si el botón esta siendo presionado
    bool already_pressed; ///< Indica si ya fue registrado como presionado
    uint32_t start_time;  ///< Momento en que se comenzo a presionar el botón
} button_state_t;

/* === Private variable declarations =========================================================== */
static const uint8_t MINUTE_LIMIT[] = {6, 0};
static const uint8_t HOUR_LIMIT[] = {2, 4};

/* === Private function declarations =========================================================== */
/**
 * @brief Activa la alarma
 *
 */
static void AlarmActivate(void);

/**
 * @brief Desactiva la alarma
 *
 */
static void AlarmDeactivate(void);

/**
 * @brief Resetea el estado de botón
 *
 * @param button_state Estado del botón
 * @return true Si se pudo resetear el estado
 * @return false Si NO se pudo resetear el estado
 */
bool ResetButton(button_state_t * button_state);

/**
 * @brief Verifica si un botón estuvo presionado una cierta cantidad de tiempo
 *
 * @param button Boton que se esta presionando
 * @param button_state Estado del boton
 * @param delay Cantidad de tiempo que debe estar presionado (en ms)
 * @return true Si estuvo presionado el tiempo requerido
 * @return false Si NO estuvo presionado el tiempo requerido
 */
bool WasButtonPressed(digital_input_t button, button_state_t * button_state, uint32_t delay);

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
/**
 * @brief Estructura que representa el driver de la alarma
 *
 */
const struct clock_alarm_driver_s driver_alarm = {
    .AlarmActivate = AlarmActivate,
    .AlarmDeactivate = AlarmDeactivate,
};

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

bool WasButtonPressed(digital_input_t button, button_state_t * button_state, uint32_t delay) {
    if (DigitalInputGetIsActive(button)) {
        if (!button_state->pressed) {
            button_state->pressed = true;
            button_state->start_time = system_ticks;
        } else {
            if ((system_ticks - button_state->start_time) >= delay && !button_state->already_pressed) {
                button_state->already_pressed = true;
                return true;
            }
        }
    } else {
        ResetButton(button_state);
    }
    return false;
}

bool ResetButton(button_state_t * button_state) {
    if (button_state != NULL) {
        button_state->pressed = false;
        button_state->already_pressed = false;
        button_state->start_time = 0;
        return true;
    } else {
        return false;
    }
}

static void AlarmActivate(void) {
    DigitalOutputActivate(board->buzzer);
    DigitalOutputActivate(board->led1);
    DigitalOutputActivate(board->led2);
    DigitalOutputActivate(board->led3);
}

static void AlarmDeactivate(void) {
    DigitalOutputDeactivate(board->buzzer);
    DigitalOutputDeactivate(board->led1);
    DigitalOutputDeactivate(board->led2);
    DigitalOutputDeactivate(board->led3);
}

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

/* === Public function implementation ========================================================= */
void ChangeMode(mode_t value) {
    mode = value;
    inactivity_count = 0;
    switch (mode) {
    case UNSET_TIME:
        DisplayFlashDigits(board->display, 0, 3, FLASH_FREQUENCY);
        DisplayFlashPoint(board->display, 0b00000010, FLASH_FREQUENCY);
        DisplaySetPoint(board->display, 0, false);
        DisplaySetPoint(board->display, 2, false);
        DisplaySetPoint(board->display, 3, false);
        break;
    case SHOW_TIME:
        DisplayFlashDigits(board->display, 0, 0, 0);
        DisplaySetPoint(board->display, 2, false);
        break;
    case SET_TIME_MINUTE:
        DisplayFlashDigits(board->display, 2, 3, FLASH_FREQUENCY);
        DisplayFlashPoint(board->display, 0b00001111, 0);
        DisplaySetPoint(board->display, 0, false);
        DisplaySetPoint(board->display, 1, false);
        DisplaySetPoint(board->display, 2, false);
        DisplaySetPoint(board->display, 3, false);
        break;
    case SET_TIME_HOUR:
        DisplayFlashDigits(board->display, 0, 1, FLASH_FREQUENCY);
        DisplayFlashPoint(board->display, 0b00001111, 0);
        DisplaySetPoint(board->display, 0, false);
        DisplaySetPoint(board->display, 1, false);
        DisplaySetPoint(board->display, 2, false);
        DisplaySetPoint(board->display, 3, false);
        break;
    case SET_ALARM_MINUTE:
        DisplayFlashDigits(board->display, 2, 3, FLASH_FREQUENCY);
        DisplaySetPoint(board->display, 0, true);
        DisplaySetPoint(board->display, 1, true);
        DisplaySetPoint(board->display, 2, true);
        DisplaySetPoint(board->display, 3, true);
        break;
    case SET_ALARM_HOUR:
        DisplayFlashDigits(board->display, 0, 1, FLASH_FREQUENCY);
        DisplaySetPoint(board->display, 0, true);
        DisplaySetPoint(board->display, 1, true);
        DisplaySetPoint(board->display, 2, true);
        DisplaySetPoint(board->display, 3, true);
        break;
    default:
        break;
    }
}

void Clock_MEF(void) {
    static clock_time_t time;
    static uint8_t hour[2] = {0};
    static uint8_t minute[2] = {0};
    static uint8_t digits[4] = {0};
    static button_state_t button_set_time = {false, false, 0};
    static button_state_t button_set_alarm = {false, false, 0};
    static bool alarm_already_set = false;
    static bool point_state_show_time = false;

    switch (mode) {
    case UNSET_TIME:
        ClockGetTime(clock, &time);
        ClockTimeToBCD(&time, digits);
        BCDtoHourAndMinute(hour, minute, digits);
        DisplayWrite(board->display, digits, sizeof(digits));

        if (WasButtonPressed(board->set_time, &button_set_time, DELAY_SET_TIME)) {
            ChangeMode(SET_TIME_MINUTE);
        }
        break;

    case SHOW_TIME:
        ClockGetTime(clock, &time);
        ClockTimeToBCD(&time, digits);
        BCDtoHourAndMinute(hour, minute, digits);
        DisplayWrite(board->display, digits, sizeof(digits));
        DisplaySetPoint(board->display, 0, ClockIsAlarmActive(clock));
        DisplaySetPoint(board->display, 3, ClockIsAlarmEnabled(clock));
        if (half_second_flag) {
            half_second_flag = false;
            point_state_show_time = !point_state_show_time;
            DisplaySetPoint(board->display, 1, point_state_show_time);
        }

        if (WasButtonPressed(board->set_time, &button_set_time, DELAY_SET_TIME)) {
            ChangeMode(SET_TIME_MINUTE);
        }

        if (WasButtonPressed(board->set_alarm, &button_set_alarm, DELAY_SET_ALARM)) {
            if (ClockGetAlarm(clock, &time)) {
                ClockTimeToBCD(&time, digits);
                BCDtoHourAndMinute(hour, minute, digits);
            }
            ChangeMode(SET_ALARM_MINUTE);
            DisplayWrite(board->display, digits, sizeof(digits));
        }

        if (!ClockIsAlarmActive(clock) && alarm_already_set) { // Solamente puedo habilitar y deshabilitar la alarma
                                                               // cuando ya se la seteo por primera vez
            if (DigitalInputWasActivated(board->accept)) {
                ClockAlarmEnable(clock, true);
            }
            if (DigitalInputWasActivated(board->cancel)) {
                ClockAlarmEnable(clock, false);
            }
        } else {
            if (DigitalInputWasActivated(board->accept)) {
                ClockPostponeAlarm(clock);
            }
            if (DigitalInputWasActivated(board->cancel)) {
                ClockActivateAlarm(clock, false);
            }
        }
        break;

    case SET_TIME_MINUTE:
        if (DigitalInputWasActivated(board->increment)) {
            BCDIncrement(minute, MINUTE_LIMIT);
            inactivity_count = 0;
        }
        if (DigitalInputWasActivated(board->decrement)) {
            BCDDecrement(minute, MINUTE_LIMIT);
            inactivity_count = 0;
        }
        if (DigitalInputWasActivated(board->accept)) {
            ChangeMode(SET_TIME_HOUR);
            inactivity_count = 0;
        }
        if (DigitalInputWasActivated(board->cancel) || inactivity_count >= INACTIVITY_COUNT) {
            inactivity_count = 0;
            if (ClockGetTime(clock, &time)) {
                ChangeMode(SHOW_TIME);
            } else {
                ChangeMode(UNSET_TIME);
            }
        }
        HourAndMinuteToBCD(hour, minute, digits);
        DisplayWrite(board->display, digits, sizeof(digits));
        break;

    case SET_TIME_HOUR:
        if (DigitalInputWasActivated(board->increment)) {
            inactivity_count = 0;
            BCDIncrement(hour, HOUR_LIMIT);
        }
        if (DigitalInputWasActivated(board->decrement)) {
            inactivity_count = 0;
            BCDDecrement(hour, HOUR_LIMIT);
        }
        if (DigitalInputWasActivated(board->accept)) {
            inactivity_count = 0;
            HourAndMinuteToBCD(hour, minute, digits);
            BCDToClockTime(&time, digits);
            ClockSetTime(clock, &time);
            ChangeMode(SHOW_TIME);
        }
        if (DigitalInputWasActivated(board->cancel) || inactivity_count >= INACTIVITY_COUNT) {
            inactivity_count = 0;
            if (ClockGetTime(clock, &time)) {
                ChangeMode(SHOW_TIME);
            } else {
                ChangeMode(UNSET_TIME);
            }
        }
        HourAndMinuteToBCD(hour, minute, digits);
        DisplayWrite(board->display, digits, sizeof(digits));
        break;

    case SET_ALARM_MINUTE:
        if (DigitalInputWasActivated(board->increment)) {
            inactivity_count = 0;
            BCDIncrement(minute, MINUTE_LIMIT);
        }
        if (DigitalInputWasActivated(board->decrement)) {
            inactivity_count = 0;
            BCDDecrement(minute, MINUTE_LIMIT);
        }
        if (DigitalInputWasActivated(board->accept)) {
            inactivity_count = 0;
            ChangeMode(SET_ALARM_HOUR);
        }
        if (DigitalInputWasActivated(board->cancel) || inactivity_count >= INACTIVITY_COUNT) {
            inactivity_count = 0;
            ChangeMode(SHOW_TIME);
        }
        HourAndMinuteToBCD(hour, minute, digits);
        DisplayWrite(board->display, digits, sizeof(digits));
        break;

    case SET_ALARM_HOUR:
        if (DigitalInputWasActivated(board->increment)) {
            inactivity_count = 0;
            BCDIncrement(hour, HOUR_LIMIT);
        }
        if (DigitalInputWasActivated(board->decrement)) {
            inactivity_count = 0;
            BCDDecrement(hour, HOUR_LIMIT);
        }
        if (DigitalInputWasActivated(board->accept)) {
            inactivity_count = 0;
            alarm_already_set = true;
            HourAndMinuteToBCD(hour, minute, digits);
            BCDToClockTime(&time, digits);
            ClockSetAlarm(clock, &time);
            ChangeMode(SHOW_TIME);
        }
        if (DigitalInputWasActivated(board->cancel) || inactivity_count >= INACTIVITY_COUNT) {
            inactivity_count = 0;
            ChangeMode(SHOW_TIME);
        }
        HourAndMinuteToBCD(hour, minute, digits);
        DisplayWrite(board->display, digits, sizeof(digits));
        break;
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
