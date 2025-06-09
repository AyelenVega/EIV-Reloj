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

#ifndef DISPLAY_H_
#define DISPLAY_H_

/** @file display.h
 ** @brief Declaraciones del módulo para la gestion de una pantalla multiplexada de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#define SEGMENT_A (1 << 0)
#define SEGMENT_B (1 << 1)
#define SEGMENT_C (1 << 2)
#define SEGMENT_D (1 << 3)
#define SEGMENT_E (1 << 4)
#define SEGMENT_F (1 << 5)
#define SEGMENT_G (1 << 6)
#define SEGMENT_P (1 << 7)

/* === Public data type declarations =============================================================================== */
//! Estructura que representa un display
typedef struct display_s * display_t;

//! Punteros a funciones para digitos
typedef void (*digits_turn_off_t)(void);
typedef void (*segments_update_t)(uint8_t);
typedef void (*digits_turn_on_t)(uint8_t);

//! Estructura que representa el driver del display
typedef struct display_driver_s {
    digits_turn_off_t DigitsTurnOff;
    segments_update_t SegmentsUpdate;
    digits_turn_on_t DigitsTurnOn;
} const * display_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief Crea el display de 7 segmentos
 *
 * @param digits Cantidad de digitos
 * @param driver Puntero al driver
 * @return display_t Referencia al display creado
 */
display_t DisplayCreate(uint8_t digits, display_driver_t driver);

/**
 * @brief Escribe en el display
 *
 * @param self Referencia al display
 * @param value Valores que se escribirán en la pantalla
 * @param size Cantidad de pantallas en el display
 */
void DisplayWrite(display_t self, uint8_t value[], uint8_t size);
/**
 * @brief Refresca la pantalla
 *
 * @param self Referencia al display
 */
void DisplayRefresh(display_t self);
/**
 * @brief Hace parpadear los digitos
 *
 * @param self Referencia al display
 * @param from Desde que digito debe parpadear
 * @param to Hasta que digito se quiere que parpadee
 * @param time_on Cantidad de veces que se quiere que el digito este prendido
 * @return int Devuelve -1 si hubo algún error y 0 si no hubieron errores
 */
int DisplayFlashDigits(display_t self, uint8_t from, uint8_t to, uint16_t time_on);
/**
 * @brief Hace parpadear los puntos
 *
 * @param self Referencia al display
 * @param mask Mascara que indica cuales son los puntos que se quieren prender
 * @param time_on Cantidad de veces que se quiere que el punto este prendido
 * @return int Devuelve -1 si hubo algún error y 0 si no hubieron errores
 */
int DisplayFlashPoint(display_t self, uint8_t mask, uint16_t time_on);
/**
 * @brief Setea en 0 o 1 el punto
 *
 * @param self Referencia al display
 * @param digit Punto que se quiere setear
 * @param on True si se lo quiere prender, False si se lo quiere apagar
 * @return int int Devuelve -1 si hubo algún error y 0 si no hubieron errores
 */
int DisplaySetPoint(display_t self, uint8_t digit, bool on);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H_ */