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

PRUEBAS A REALIZAR
Hora actual
- Al inicializar el reloj esta en 00:00 y la hora es invalida
- Tratar de setear el reloj con una hora válida
- Probar que avance de 0 a 1 segundo
- Probar que avance de 9 a 10 segundos
- Probar que avance de 59 a 1 minuto
- Probar que avance de 9:59 a 10 minutos
- Probar que avance de 59:59 a 1 hora
- Probar que avance de 9:59:59 a 10 horas
- Probar que avance de 23:59:59 a 00:00:00
- Probar GetTime con NULL como argumento
- Tratar de setear la hora con valores invalidos y verificar que los rechace


Alarma
-Probar que al setear la alarma esta suene
-Probar que al deshabilitar la alarma no suene
-Probar que al posponer alarma suene despues de x tiempo
-Hacer sonar la alarma y cancelarla hasta el dia siguiente
-Setear alarma con hora invalida

*********************************************************************************************************************/

/** @file  test_reloj.c
 ** @brief
 **/

/* === Headers files inclusions ==================================================================================== */
#include "unity.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */
#define CLOCK_TICK_PER_SECONDS 5 //!< Cantidad de ticks por segundo
#define TEST_ASSERT_TIME(hours_tens, hours_units, minutes_tens, minutes_units, seconds_tens, seconds_units,            \
                         current_time)                                                                                 \
    clock_time_t current_time = {0};                                                                                   \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");                            \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, current_time.bcd[0], "Diference in unit seconds");                  \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens, current_time.bcd[1], "Diference in tens seconds");                   \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, current_time.bcd[2], "Diference in unit minutes");                  \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens, current_time.bcd[3], "Diference in tens minutes");                   \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_units, current_time.bcd[4], "Diference in unit hours");                      \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_tens, current_time.bcd[5], "Diference in tens hours")

/* === Private data type declarations ==============================================================================
 */

/* === Private function declarations ===============================================================================*/
/**
 * @brief Simula el paso de n cantidad de segundos
 *
 * @param clock Puntero al objeto reloj
 * @param seconds Cantidad de segundos que se quieren avanzar
 */
static void SimulateSeconds(clock_t clock, uint32_t seconds);

//! Funcion para simular la activacion de la alarma
static void AlarmActivate(clock_t clock);

//! Funcion para simular la desactivacion de la alarma
static void AlarmDeactivate(clock_t clock);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */
clock_t clock;
static const struct clock_alarm_driver_s driver_alarm = {
    .AlarmActivate = AlarmActivate,
    .AlarmDeactivate = AlarmDeactivate,
};

/* === Private function definitions ================================================================================ */
static void SimulateSeconds(clock_t clock, uint32_t seconds) {
    for (uint32_t i = 0; i < CLOCK_TICK_PER_SECONDS * seconds; i++) {
        ClockNewTick(clock);
    }
}

static void AlarmActivate(clock_t clock) {
    (void)clock;
}

static void AlarmDeactivate(clock_t clock) {
    (void)clock;
}

/* === Public function implementation ============================================================================== */
void setUp() {
    clock = ClockCreate(CLOCK_TICK_PER_SECONDS, &driver_alarm);
}

// Al inicializar el reloj está en 00:00 y con hora invalida.
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {.bcd = {1, 2, 3, 4, 5, 6}};
    clock_t clock = ClockCreate(CLOCK_TICK_PER_SECONDS, &driver_alarm);
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}
// Al ajustar la hora el reloj queda en hora y es válida.
void test_set_up_with_valid_time(void) {
    static const clock_time_t new_time = {.time = {.hours = {1, 2}, .minutes = {3, 0}, .seconds = {4, 5}}};
    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TIME(2, 1, 0, 3, 5, 4, current_time);
}

// Poner una hora invalida al reloj
void test_set_unvalid_time(void) {
    static const clock_time_t new_time = {.time = {.hours = {4, 2}, .minutes = {3, 0}, .seconds = {4, 5}}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &new_time));
}

// Después de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 1);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1, current_time);
}

// Después de n ciclos de reloj la hora avanza diez segundos
void test_clock_advance_ten_seconds(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 10);
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0, current_time);
}

// Después de n ciclos de reloj la hora avanza un minuto
void test_clock_advance_one_minute(void) {
    ClockSetTime(clock, &(clock_time_t){.time = {.hours = {0, 0}, .minutes = {0, 0}, .seconds = {5, 5}}});
    SimulateSeconds(clock, 5);
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0, current_time);
}

// Después de n ciclos de reloj la hora avanza diez minutos
void test_clock_advance_ten_minutes(void) {
    ClockSetTime(clock, &(clock_time_t){.time = {.hours = {0, 0}, .minutes = {9, 0}, .seconds = {5, 5}}});
    SimulateSeconds(clock, 5);
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0, current_time);
}

// Después de n ciclos de reloj la hora avanza una hora
void test_clock_advance_one_hour(void) {
    ClockSetTime(clock, &(clock_time_t){.time = {.hours = {0, 0}, .minutes = {9, 5}, .seconds = {5, 5}}});
    SimulateSeconds(clock, 5);
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0, current_time);
}

// Después de n ciclos de reloj la hora avanza diez horas
void test_clock_advance_ten_hours(void) {
    ClockSetTime(clock, &(clock_time_t){.time = {.hours = {9, 0}, .minutes = {9, 5}, .seconds = {5, 5}}});
    SimulateSeconds(clock, 5);
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0, current_time);
}

// Después de n ciclos de reloj la hora avanza un dia
void test_clock_advance_one_day(void) {
    ClockSetTime(clock, &(clock_time_t){.time = {.hours = {3, 2}, .minutes = {9, 5}, .seconds = {5, 5}}});
    SimulateSeconds(clock, 5);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0, current_time);
}

// Alarma
// Fijar la hora de la alarma y consultarla.
void test_set_up_alarm_with_valid_time(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    clock_time_t alarm_time = {0};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &new_alarm));
    ClockGetAlarm(clock, &alarm_time);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(new_alarm.bcd, alarm_time.bcd, 6);
}

// Fijar la hora de la alarma en un tiempo no válido
void test_set_up_alarm_with_unvalid_time(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {4, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    TEST_ASSERT_FALSE(ClockSetAlarm(clock, &new_alarm));
}
// Fijar la alarma y avanzar el reloj para que suene.
void test_set_alarm_advance_clock_and_expect_ring(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {2, 0}}};
    static const clock_time_t current_time = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    ClockSetTime(clock, &current_time);
    ClockSetAlarm(clock, &new_alarm);
    SimulateSeconds(clock, 2);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
}

// Fijar la alarma, avanzar el reloj pero antes de que tenga que sonar
void test_set_alarm_advance_clock_and_expect_not_to_ring(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {3, 0}}};
    static const clock_time_t current_time = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    ClockSetTime(clock, &current_time);
    ClockSetAlarm(clock, &new_alarm);
    SimulateSeconds(clock, 2);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
}

// Fijar la alarma, deshabilitarla y avanzar el reloj para no suene
void test_set_alarm_and_disable_alarm(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {2, 0}}};
    static const clock_time_t current_time = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    ClockSetTime(clock, &current_time);
    ClockSetAlarm(clock, &new_alarm);
    TEST_ASSERT_TRUE(ClockAlarmEnable(clock, false));
    TEST_ASSERT_FALSE(ClockIsAlarmEnabled(clock));
    SimulateSeconds(clock, 2);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
}
// Hacer sonar la alarma y posponerla.
void test_postpone_alarm(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    static const clock_time_t current_time = {.time = {.hours = {1, 2}, .minutes = {9, 2}, .seconds = {8, 5}}};
    static const clock_time_t postpone_alarm = {.time = {.hours = {1, 2}, .minutes = {5, 3}, .seconds = {0, 0}}};
    clock_time_t alarm_time = {0};
    uint32_t postpone_minutes = 5;
    ClockSetTime(clock, &current_time);
    ClockSetAlarm(clock, &new_alarm);
    SimulateSeconds(clock, 2);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
    TEST_ASSERT_TRUE(ClockPostponeAlarm(clock, postpone_minutes));
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
    ClockGetAlarm(clock, &alarm_time);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(postpone_alarm.bcd, alarm_time.bcd, 6);
    SimulateSeconds(clock, postpone_minutes * 60);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
}
// Hacer sonar la alarma y cancelarla hasta el otro dia
void test_cancel_and_wait_until_next_day(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    static const clock_time_t current_time = {.time = {.hours = {1, 2}, .minutes = {9, 2}, .seconds = {8, 5}}};
    ClockSetTime(clock, &current_time);
    ClockSetAlarm(clock, &new_alarm);
    SimulateSeconds(clock, 2);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
    ClockActivateAlarm(clock, false);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
    SimulateSeconds(clock, 86400);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
}

// Hacer sonar la alarma y cancelarla hasta el otro dia, pero esperar hasta un segundo antes de que tenga que sonar
void test_cancel_and_wait_until_next_day_but_not_ring(void) {
    static const clock_time_t new_alarm = {.time = {.hours = {1, 2}, .minutes = {0, 3}, .seconds = {0, 0}}};
    static const clock_time_t current_time = {.time = {.hours = {1, 2}, .minutes = {9, 2}, .seconds = {8, 5}}};
    ClockSetTime(clock, &current_time);
    ClockSetAlarm(clock, &new_alarm);
    SimulateSeconds(clock, 2);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
    ClockActivateAlarm(clock, false);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
    SimulateSeconds(clock, 86399);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
}

/* === End of documentation ======================================================================================== */
