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

/** @file  tasks_init.c
 ** @brief
 **/

/* === Headers files inclusions =============================================================== */
#include "tasks_init.h"
#include <stdlib.h>

/* === Macros definitions ====================================================================== */
#define BUTTON_SCAN_DELAY 100

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */
/**
 * @brief Funcion para mostrar que ocurrio un error
 *
 * @param parameters
 */
void Blinking(void * parameters);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */
typedef struct error_task_args_s {
    board_t board;
} * error_task_args_t;

/* === Private function implementation ========================================================= */
void Blinking(void * parameters) {
    error_task_args_t args = (error_task_args_t)parameters;
    while (true) {
        DigitalOutputToggle(args->board->buzzer);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* === Public function implementation ========================================================= */
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
        error_task_args_t error_args = malloc(sizeof(*error_args));
        error_args->board = board;
        xTaskCreate(Blinking, "Baliza", configMINIMAL_STACK_SIZE, error_args, tskIDLE_PRIORITY + 1, NULL);
    }
}
/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */