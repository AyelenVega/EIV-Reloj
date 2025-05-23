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

#ifndef DIGITAL_H_
#define DIGITAL_H_

/** @file digital.h
 ** @brief Declaraciones para el modulo de gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Representa los cambios de estado que puede haber tenido una entrada digital
typedef enum digital_state_e {
    DIGITAL_INPUT_WAS_DEACTIVATED = -1, //!< La entrada pasó de activa a inactiva
    DIGITAL_INPUT_NO_CHANGE = 0,        //!< La entrada no cambió
    DIGITAL_INPUT_WAS_ACTIVATED = 1     //!< La entrada pasó de inactiva a activa
} digital_state_t;

//! Estructura que representa una salida digital
typedef struct digital_output_s * digital_output_t;

//! Estructura que representa una entrada digital
typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear salida digital
 *
 * @param gpio Número del puerto GPIO
 * @param bit Número del bit
 * @return digital_output_t Referencia a la salida digital creada
 */
digital_output_t DigitalOutputCreate(uint8_t gpio, uint8_t bit);

/**
 * @brief Función para activar una salida digital
 *
 * @param self Referencia a la salida digital que se quiere activar
 */
void DigitalOutputActivate(digital_output_t self);
/**
 * @brief Función para desactivar una salida digital
 *
 * @param self Referencia a la salida digital que se quiere desactivar
 */
void DigitalOutputDeactivate(digital_output_t self);
/**
 * @brief Función para cambiar el estado del pin digital
 *
 * @param self Referencia a la salida digital cuyo estado se quiere cambiar
 */
void DigitalOutputToggle(digital_output_t self);

/**
 * @brief Función para crear una entrada digital
 *
 * @param gpio Número del puerto GPIO
 * @param bit Número del bit
 * @param inverted True si es de logica inversa, false en caso contrario
 * @return digital_input_t  Referencia a la entrada digital creada
 */
digital_input_t DigitalInputCreate(uint8_t gpio, uint8_t bit, bool inverted);

/**
 * @brief Indica si la entrada digital esta activa
 *
 * @param self Referencia a la entrada digital
 * @retval true Si la entrada esta activa
 * @retval false Si la entrada esta inactiva
 */
bool DigitalInputGetIsActive(digital_input_t self);

/**
 * @brief Indica si la entrada digital fue activada desde la última lectura
 *
 * @param self Referencia a la entrada digital
 * @retval true Si la entrada fue activada
 * @retval false Si la entrada NO fue activada
 */
bool DigitalInputWasActivated(digital_input_t self);

/**
 * @brief Indica si la entrada digital fue desactivada desde la última lectura
 *
 * @param self Referencia a la entrada digital
 * @retval true Si la entrada fue desactivada
 * @retval false Si la entrada NO fue desactivada
 */
bool DigitalInputWasDeactivated(digital_input_t self);

/**
 * @brief Indica si cambió el estado de la entrada digital
 *
 * @param self Referencia a la entrada digital
 * @retval DIGITAL_INPUT_WAS_ACTIVATED    Si la entrada fue activada
 * @retval DIGITAL_INPUT_WAS_DEACTIVATED  Si la entrada fue desactivada
 * @retval DIGITAL_INPUT_NO_CHANGE        Si la entrada no cambió
 */
digital_state_t DigitalInputWasChanged(digital_input_t self);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_H_ */
