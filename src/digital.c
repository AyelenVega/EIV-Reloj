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

/** @file  digital.c
 ** @brief Codigo fuente para el modulo de gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "digital.h"
#include "config.h"
#include "chip.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

struct digital_output_s {
    uint8_t gpio; //!< Numero de puerto
    uint8_t bit;  //!< Numero de pin
};

struct digital_input_s {
    uint8_t gpio;
    uint8_t bit;
    bool inverted;
    bool lastState;
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

digital_output_t DigitalOutputCreate(uint8_t gpio, uint8_t bit) {
    digital_output_t self = malloc(sizeof(struct digital_output_s));
    if (self != NULL) {
        self->gpio = gpio;
        self->bit = bit;
    }
    DigitalOutputDeactivate(self);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->gpio, self->bit, true);
    return self;
}

void DigitalOutputActivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio, self->bit, true);
}

void DigitalOutputDeactivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio, self->bit, false);
}

void DigitalOutputToggle(digital_output_t self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->gpio, self->bit);
}

digital_input_t DigitalInputCreate(uint8_t gpio, uint8_t bit, bool inverted) {
    digital_input_t self = malloc(sizeof(struct digital_input_s));
    if (self != NULL) {
        self->gpio = gpio;
        self->bit = bit;
        self->inverted = inverted;
        self->lastState = DigitalInputGetIsActive(self);
    }
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->gpio, self->bit, false);
    return self;
}

bool DigitalInputGetIsActive(digital_input_t self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->gpio, self->bit);
    if (self->inverted) {
        state = !state;
    }
    return state;
}

bool DigitalInputWasActivated(digital_input_t self) {
    return DIGITAL_INPUT_WAS_ACTIVATED == DigitalInputWasChanged(self);
}
bool DigitalInputWasDeactivated(digital_input_t self) {
    return DIGITAL_INPUT_WAS_DEACTIVATED == DigitalInputWasChanged(self);
}

digital_state_t DigitalInputWasChanged(digital_input_t self) {
    bool state = DigitalInputGetIsActive(self);
    digital_state_t result = DIGITAL_INPUT_NOT_CHANGED;
    if (state && !self->lastState) {
        result = DIGITAL_INPUT_WAS_ACTIVATED;
    } else if (!state && self->lastState) {
        result = DIGITAL_INPUT_WAS_DEACTIVATED;
    }
    self->lastState = state;
    return result;
}

/* === End of documentation ======================================================================================== */
