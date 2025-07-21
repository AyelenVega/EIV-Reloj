/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */
#include "digital.h"
#include "clock.h"
#include "bsp.h"
#include <stdbool.h>
#include <stddef.h>

/* === Macros definitions ====================================================================== */
#define POSTPONE_MINUTES 5
#define TICKS_PER_SECOND 1000
#define INACTIVITY_COUNT 30000 // ms
#define DELAY_COUNT_SET  3000  // ms

/* === Private data type declarations ========================================================== */
typedef enum {
    UNSET_TIME,
    SHOW_TIME,
    SET_TIME_MINUTE,
    SET_TIME_HOUR,
    SET_ALARM_MINUTE,
    SET_ALARM_HOUR,
} mode_t;

typedef struct {
    bool pressed;
    bool already_pressed;
    uint32_t start_time;
} button_state_t;

/* === Private variable declarations =========================================================== */
static board_t board;
static mode_t mode;
static clock_t clock;
static const uint8_t MINUTE_LIMIT[] = {6, 0};
static const uint8_t HOUR_LIMIT[] = {2, 4};
static uint32_t system_ticks = 0;
static uint32_t inactivity_count = 0;

/* === Private function declarations =========================================================== */
//! Funcion para simular la activacion de la alarma
static void AlarmActivate(clock_t clock);

//! Funcion para simular la desactivacion de la alarma
static void AlarmDeactivate(clock_t clock);

/* === Public variable definitions ============================================================= */
static const struct clock_alarm_driver_s driver_alarm = {
    .AlarmActivate = AlarmActivate,
    .AlarmDeactivate = AlarmDeactivate,
};

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
bool WasButtonPressed(digital_input_t button, button_state_t * state, uint32_t hold_time) {
    if (!DigitalInputGetIsActive(button)) { // botón presionado
        if (!state->pressed) {
            state->pressed = true;
            state->start_time = system_ticks;
        } else {
            // Ya está presionado, verificar duración
            if ((system_ticks - state->start_time) >= hold_time && !state->already_pressed) {
                state->already_pressed = true;
                return true;
            }
        }
    } else {
        // Botón soltado → resetear estado
        state->pressed = false;
        state->already_pressed = false;
    }

    return false;
}

bool ResetButton(button_state_t * button) {
    if (button != NULL) {
        button->pressed = false;
        button->already_pressed = false;
        button->start_time = 0;
        return true;
    } else {
        return false;
    }
}

void ChangeMode(mode_t value) {
    mode = value;
    inactivity_count = 0;

    switch (mode) {
    case UNSET_TIME:
        DisplayFlashDigits(board->display, 0, 3, 200);
        DisplayFlashPoint(board->display, 0b00000010, 200);
        DisplaySetPoint(board->display, 0, false);
        DisplaySetPoint(board->display, 2, false);
        DisplaySetPoint(board->display, 3, false);
        break;
    case SHOW_TIME:
        DisplayFlashDigits(board->display, 0, 0, 0);
        DisplaySetPoint(board->display, 2, false);
        break;
    case SET_TIME_MINUTE:
        DisplayFlashDigits(board->display, 2, 3, 200);
        DisplayFlashPoint(board->display, 0b00001111, 0);
        break;
    case SET_TIME_HOUR:
        DisplayFlashDigits(board->display, 0, 1, 200);
        DisplayFlashPoint(board->display, 0b00001111, 0);
        break;
    case SET_ALARM_MINUTE:
        DisplayFlashDigits(board->display, 2, 3, 200);
        DisplaySetPoint(board->display, 0, true);
        DisplaySetPoint(board->display, 1, true);
        DisplaySetPoint(board->display, 2, true);
        DisplaySetPoint(board->display, 3, true);
        break;
    case SET_ALARM_HOUR:
        DisplayFlashDigits(board->display, 0, 1, 200);
        DisplaySetPoint(board->display, 0, true);
        DisplaySetPoint(board->display, 1, true);
        DisplaySetPoint(board->display, 2, true);
        DisplaySetPoint(board->display, 3, true);
        break;
    default:
        break;
    }
}

static void AlarmActivate(clock_t clock) {
    (void)clock;
    DigitalOutputActivate(board->buzzer);
    DigitalOutputActivate(board->led1);
    DigitalOutputActivate(board->led2);
    DigitalOutputActivate(board->led3);
}

static void AlarmDeactivate(clock_t clock) {
    DigitalOutputDeactivate(board->buzzer);
    DigitalOutputDeactivate(board->led1);
    DigitalOutputDeactivate(board->led2);
    DigitalOutputDeactivate(board->led3);
    (void)clock;
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
int main(void) {
    clock_time_t time;
    uint8_t hour[2] = {0};
    uint8_t minute[2] = {0};
    uint8_t digits[4] = {0};

    button_state_t button_set_time = {false, false, 0};
    button_state_t button_set_alarm = {false, false, 0};

    clock = ClockCreate(1000, 5, &driver_alarm);
    board = BoardCreate();
    SysTickInit(1000);
    ChangeMode(UNSET_TIME);

    while (true) {
        switch (mode) {
        case UNSET_TIME:
            ClockGetTime(clock, &time);
            ClockTimeToBCD(&time, digits);
            BCDtoHourAndMinute(hour, minute, digits);
            DisplayWrite(board->display, digits, sizeof(digits));

            if (WasButtonPressed(board->set_time, &button_set_time, 3000)) {
                ChangeMode(SET_TIME_MINUTE);
                ResetButton(&button_set_time);
            }
            break;

        case SHOW_TIME:
            ClockGetTime(clock, &time);
            ClockTimeToBCD(&time, digits);
            BCDtoHourAndMinute(hour, minute, digits);
            DisplayWrite(board->display, digits, sizeof(digits));

            if (WasButtonPressed(board->set_time, &button_set_time, 3000)) {
                ChangeMode(SET_TIME_MINUTE);
                ResetButton(&button_set_time);
            }

            if (WasButtonPressed(board->set_alarm, &button_set_alarm, 3000)) {
                if (ClockGetAlarm(clock, &time)) {
                    ClockTimeToBCD(&time, digits);
                    BCDtoHourAndMinute(hour, minute, digits);
                }
                ChangeMode(SET_ALARM_MINUTE);
                ResetButton(&button_set_alarm);
                DisplayWrite(board->display, digits, sizeof(digits));
            }

            if (!ClockIsAlarmActive(clock)) {
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
            if (DigitalInputWasActivated(board->cancel) || inactivity_count >= 30000) {
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
            if (DigitalInputWasActivated(board->cancel) || inactivity_count >= 30000) {
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
            if (DigitalInputWasActivated(board->cancel) || inactivity_count >= 30000) {
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
                HourAndMinuteToBCD(hour, minute, digits);
                BCDToClockTime(&time, digits);
                ClockSetAlarm(clock, &time);
                ChangeMode(SHOW_TIME);
            }
            if (DigitalInputWasActivated(board->cancel) || inactivity_count >= 30000) {
                inactivity_count = 0;
                ChangeMode(SHOW_TIME);
            }
            HourAndMinuteToBCD(hour, minute, digits);
            DisplayWrite(board->display, digits, sizeof(digits));
            break;
        }

        for (volatile int i = 0; i < 25000; i++) {
            __asm("NOP");
        }
    }
}

void SysTick_Handler(void) {
    static uint32_t count;
    clock_time_t time;
    uint8_t bcd[4];

    DisplayRefresh(board->display);
    ClockNewTick(clock);
    count = (count + 1) % 1000;
    system_ticks++;
    inactivity_count++;

    if (mode == SHOW_TIME) {
        ClockGetTime(clock, &time);
        ClockTimeToBCD(&time, bcd);
        DisplayWrite(board->display, bcd, sizeof(bcd));
        DisplaySetPoint(board->display, 0, ClockIsAlarmActive(clock));
        DisplaySetPoint(board->display, 3, ClockIsAlarmEnabled(clock));
        if (count > 500) {
            DisplayFlashPoint(board->display, 0b00000010, 1);
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
