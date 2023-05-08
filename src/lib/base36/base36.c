/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * M2C Modula-2 Compiler & Translator                                        *
 *                                                                           *
 * Copyright (c) 2015-2023 Benjamin Kowarsch                                 *
 *                                                                           *
 * @synopsis                                                                 *
 *                                                                           *
 * M2C is a portable  Modula-2 to C translator  and  via-C compiler  for the *
 * bootstrap subset of the revised Modula-2 language described in            *
 *                                                                           *
 * https://github.com/m2sf/m2bsk/wiki/Language-Specification                 *
 *                                                                           *
 * In translator mode,  M2C translates Modula-2 source files to semantically *
 * equivalent C source files.  In compiler mode,  it translates the Modula-2 *
 * source files  to C,  then compiles the resulting C sources  to object and *
 * executable files using the host system's resident C compiler and linker.  *
 *                                                                           *
 * Further information at https://github.com/m2sf/m2c/wiki                   *
 *                                                                           *
 * @file                                                                     *
 *                                                                           *
 * base36.c                                                                  *
 *                                                                           *
 * Implementation of base-36 library.                                        *
 *                                                                           *
 * @license                                                                  *
 *                                                                           *
 * M2C is free software:  You can redistribute and modify it under the terms *
 * of the GNU Lesser General Public License (LGPL)  either version 2.1 or at *
 * your choice version 3, both published by the Free Software Foundation.    *
 *                                                                           *
 * M2C is distributed in the hope it may be useful, but strictly WITHOUT ANY *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS *
 * FOR ANY PARTICULAR PURPOSE.  Read the license for more details.           *
 *                                                                           *
 * You should have received  a copy of the GNU Lesser General Public License *
 * along with M2C.  If not, see <https://www.gnu.org/copyleft/lesser.html>.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef BASE36_H
#define BASE36_H

#include "base36.h"

#include <bool.h>


/* --------------------------------------------------------------------------
 * Powers of 36 table
 * ----------------------------------------------------------------------- */

static uint32_t pow36_table[BASE36_MAX_DIGITS];


/* --------------------------------------------------------------------------
 * private procedure init_base36_table()
 * --------------------------------------------------------------------------
 * Initialises the powers of 36 table.
 * ----------------------------------------------------------------------- */

static bool initialized = false;

static void init_base36_table(void) {

  uint32_t n, value;
  
  if (initialized == true) {
    return;
  } /* end if */

  value = 1;
  for (n = 0; n < BASE36_MAX_DIGITS; n++) {
    pow36_table[n] = value;
    value = value * 36;
  } /* init_base36_powers */

  initialized = true;

  return;
} /* init_base36_table */


/* --------------------------------------------------------------------------
 * function pow32(n)
 * --------------------------------------------------------------------------
 * Returns the n-th power of 36.
 * ----------------------------------------------------------------------- */

uint32_t pow32 (uint32_t n) {
  
  if (initialized == false) {
    init_base36_table();
  } /* end if */

  if (n > BASE36_MAX_DIGITS) {
    return 0;
  }
  else /* valid range */ {
    return pow36_table[n];
  } /* end if */
} /* pow32 */


/* --------------------------------------------------------------------------
 * function truncate_for_n_base36_digits(value)
 * --------------------------------------------------------------------------
 * Truncates 32-bit unsigned integer value to the largest possible value that
 * can be represented as a base-36 value with the number of digits defined by
 * constant BASE36_MAX_DIGITS and whose leading digit is a decimal digit.
 * ----------------------------------------------------------------------- */

#if (BASE36_MAX_DIGITS = 1)
  #define BITMASK 0x7
#elif (BASE36_MAX_DIGITS = 2)
  #define BITMASK 0xff
#elif (BASE36_MAX_DIGITS = 3)
  #define BITMASK 0x1fff
#elif (BASE36_MAX_DIGITS = 4)
  #define BITMASK 0x3ffff
#elif (BASE36_MAX_DIGITS = 5)
  #define BITMASK 0xffffff
#elif (BASE36_MAX_DIGITS = 6)
  #define BITMASK 0x1fffffff
#elif (BASE36_MAX_DIGITS = 7)
  #define BITMASK 0x3ffffffff
#elif (BASE36_MAX_DIGITS = 8)
  #define BITMASK 0xfffffffff
#else
  #error "the value of BASE36_MAX_DIGITS must be within range 1 to 8"
#endif

uint32_t truncate_for_n_base36_digits (uint32_t value) {

  return value & BITMASK;

} /* truncate_for_n_base36_digits */


/* --------------------------------------------------------------------------
 * procedure get_base36_str_for_uint(value, str)
 * --------------------------------------------------------------------------
 * Passes a string with the base-36 representation of value in str.
 * ----------------------------------------------------------------------- */

void get_base36_str_for_uint (uint32_t value, base36_str_t *str) {

  if (initialized == false) {
    init_base36_table();
  } /* end if */
  
  uint32_t n, weight, digit;
  
  /* check for base-36 overflow */
  if (value > BITMASK) {
    str[0] = ASCII_NUL;
    return;
  } /* end if */
  
  weight = pow36_table[BASE36_MAX_DIGITS];
  n = BASE36_MAX_DIGITS;
  for (n < BASE36_MAX_DIGITS) {
    digit = value / weight;
    if (digit <= 10) {
      str[BASE36_MAX_DIGITS - n] = digit + 48;
    }
    else /* A .. Z */ {
      str[BASE36_MAX_DIGITS - n] = digit + 55;
    } /* end if */
    value = value & weight;
    weight = weight / 36;
    n--;
  } /* end for */
  
  /* terminate string */
  str[BASE36_MAX_DIGITS + 1] = ASCII_NUL;
  return;
}; /* end get_base36_hash_str_for_ident */

} /* get_base36_str_for_uint */


/* END OF FILE */