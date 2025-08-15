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

/** @file  main.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "button_tasks.h"
#include "clock_tasks.h"
#include "display_tasks.h"
#include <stdlib.h>

/* === Macros definitions ====================================================================== */
#define BUTTON_ACCEPT          BUTTON_EVENT_0
#define BUTTON_CANCEL          BUTTON_EVENT_1
#define BUTTON_INCREMENT       BUTTON_EVENT_2
#define BUTTON_DECREMENT       BUTTON_EVENT_3
#define BUTTON_SET_TIME        BUTTON_EVENT_4
#define BUTTON_SET_ALARM       BUTTON_EVENT_5
#define HALF_SECOND            TICKS_EVENTS_6
#define INACTIVITY_TIME        TICKS_EVENTS_7
#define RESET_THIRTY_SECONDS   TICKS_EVENT_8

#define ALARM_POSTPONE_MINUTES 5    ///< Cantidad de minutos que se pospone la alarma
#define TICKS_PER_SECOND       1000 ///< Cantidad de ticks por segundo
#define DELAY_SET_TIME         3000 ///< Cantidad de tiempo que tiene que presionarse el boton de setear tiempo en ms
#define DELAY_SET_ALARM        3000 ///< Cantidad de tiempo que tiene que presionarse el boton de setear alarma en ms

/* === Private data type declarations ========================================================== */
board_t board;
clock_t clock;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */
/**
 * @brief Funcion para señalizar en caso de que ocurra un error al crear las tareas
 *
 * @param parameters
 */

void Blinking(void * parameters);
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
 * @brief Inicializa todas las tareas
 *
 * @param clock Puntero al objeto reloj
 * @param board Puntero al objeto board
 */
void TasksInit(clock_t clock, board_t board);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */
/**
 * @brief Estructura que representa el driver de la alarma
 *
 */
const struct clock_alarm_driver_s driver_alarm = {
    .AlarmActivate = AlarmActivate,
    .AlarmDeactivate = AlarmDeactivate,
};

/* === Private function implementation ========================================================= */
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

void TasksInit(clock_t clock, board_t board) {
    EventGroupHandle_t clock_events;
    SemaphoreHandle_t display_mutex;
    BaseType_t result;

    display_mutex = xSemaphoreCreateMutex();
    clock_events = xEventGroupCreate();

    static button_state_t button_set_time = {false, false, 0, DELAY_SET_TIME};
    static button_state_t button_set_alarm = {false, false, 0, DELAY_SET_ALARM};

    if (clock_events && display_mutex) {
        button_task_args_t button_args = malloc(sizeof(*button_args));
        button_args->clock_events = clock_events;
        button_args->event_bit = BUTTON_ACCEPT;
        button_args->button = board->accept;
        result =
            xTaskCreate(ButtonTask, "Button_Accept", BUTTON_TASK_STACK_SIZE, button_args, tskIDLE_PRIORITY + 1, NULL);
    }
    if (result == pdPASS) {
        button_task_args_t button_args = malloc(sizeof(*button_args));
        button_args->clock_events = clock_events;
        button_args->event_bit = BUTTON_CANCEL;
        button_args->button = board->cancel;
        result =
            xTaskCreate(ButtonTask, "Button_Cancel", BUTTON_TASK_STACK_SIZE, button_args, tskIDLE_PRIORITY + 1, NULL);
    }

    if (result == pdPASS) {
        button_task_args_t button_args = malloc(sizeof(*button_args));
        button_args->clock_events = clock_events;
        button_args->event_bit = BUTTON_INCREMENT;
        button_args->button = board->increment;
        result = xTaskCreate(ButtonTask, "Button_Increment", BUTTON_TASK_STACK_SIZE, button_args, tskIDLE_PRIORITY + 1,
                             NULL);
    }
    if (result == pdPASS) {
        button_task_args_t button_args = malloc(sizeof(*button_args));
        button_args->clock_events = clock_events;
        button_args->event_bit = BUTTON_DECREMENT;
        button_args->button = board->decrement;
        result = xTaskCreate(ButtonTask, "Button_Decrement", BUTTON_TASK_STACK_SIZE, button_args, tskIDLE_PRIORITY + 1,
                             NULL);
    }
    if (result == pdPASS) {
        button_task_args_t button_args = malloc(sizeof(*button_args));
        button_args->clock_events = clock_events;
        button_args->event_bit = BUTTON_SET_TIME;
        button_args->button = board->set_time;
        button_args->state = &button_set_time;
        result = xTaskCreate(ButtonPressedForLongTimeTask, "Button_Set_Time", BUTTON_TASK_STACK_SIZE, button_args,
                             tskIDLE_PRIORITY + 1, NULL);
    }
    if (result == pdPASS) {
        button_task_args_t button_args = malloc(sizeof(*button_args));
        button_args->clock_events = clock_events;
        button_args->event_bit = BUTTON_SET_ALARM;
        button_args->button = board->set_alarm;
        button_args->state = &button_set_alarm;
        result = xTaskCreate(ButtonPressedForLongTimeTask, "Button_Set_Alarm", BUTTON_TASK_STACK_SIZE, button_args,
                             tskIDLE_PRIORITY + 1, NULL);
    }
    if (result == pdPASS) {
        clock_task_args_t clock_args = malloc(sizeof(*clock_args));
        clock_args->clock_events = clock_events;
        clock_args->display_mutex = display_mutex;
        clock_args->board = board;
        clock_args->clock = clock;
        result = xTaskCreate(ClockTask, "ClockTask", CLOCK_TASK_STACK_SIZE, clock_args, tskIDLE_PRIORITY + 3, NULL);
    }
    if (result == pdPASS) {
        refresh_task_args_t refresh_args = malloc(sizeof(*refresh_args));
        refresh_args->display_mutex = display_mutex;
        refresh_args->board = board;
        refresh_args->clock = clock;
        refresh_args->clock_events = clock_events;
        result = xTaskCreate(RefreshDisplay, "Refresh", REFRESH_STACK_SIZE, refresh_args, tskIDLE_PRIORITY + 2, NULL);
    }

    if (result != pdPASS) {
        xTaskCreate(Blinking, "Baliza", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    }
}
void Blinking(void * parameters) {
    while (true) {
        DigitalOutputToggle(board->buzzer);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* === Public function implementation ========================================================= */
int main(void) {
    clock = ClockCreate(TICKS_PER_SECOND, ALARM_POSTPONE_MINUTES, &driver_alarm);
    board = BoardCreate();
    TasksInit(clock, board);
    vTaskStartScheduler();
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
