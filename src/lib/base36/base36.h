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
 * base36.h                                                                  *
 *                                                                           *
 * Public interface of base-36 library.                                      *
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

/* --------------------------------------------------------------------------
 * Maximum number of digits in a base-36 string
 * ----------------------------------------------------------------------- */

#define BASE36_MAX_DIGITS 5


/* --------------------------------------------------------------------------
 * type base36_str_t
 * --------------------------------------------------------------------------
 * Character array type to hold base-36 representations of values.
 * ----------------------------------------------------------------------- */

typedef base36_str_t char[BASE36_MAX_DIGITS + 1];


/* --------------------------------------------------------------------------
 * function pow32(n)
 * --------------------------------------------------------------------------
 * Returns the n-th power of 36.
 * ----------------------------------------------------------------------- */

uint32_t pow32 (uint32_t n);


/* --------------------------------------------------------------------------
 * function truncate_for_n_base36_digits(value)
 * --------------------------------------------------------------------------
 * Truncates 32-bit unsigned integer value to the largest possible value that
 * can be represented as a base-36 value with the number of digits defined by
 * constant BASE36_MAX_DIGITS and whose leading digit is a decimal digit.
 * ----------------------------------------------------------------------- */

uint32_t truncate_for_n_base36_digits (uint32_t value);


/* --------------------------------------------------------------------------
 * procedure get_base36_str_for_uint(value, str)
 * --------------------------------------------------------------------------
 * Passes a string with the base-36 representation of value in str.
 * ----------------------------------------------------------------------- */

void get_base36_str_for_uint (uint32_t value, base36_str_t *str);


#endif BASE36_H

/* END OF FILE */