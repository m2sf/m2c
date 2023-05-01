/* M2C Modula-2 Compiler & Translator
 *
 * Copyright (c) 2015-2023 Benjamin Kowarsch
 *
 * @synopsis
 *
 * M2C  is a  Modula-2 to C translator  and  via-C compiler  for the bootstrap
 * subset of the revised Modula-2 language described in
 *
 * https://github.com/m2sf/PDFs/blob/master/M2BSK%20Language%20Description.pdf
 *
 * In compiler mode,  M2C compiles Modula-2 source files via C to object files
 * or executables  using the host system's resident C compiler and linker.  In
 * translator mode, it translates Modula-2 source files to C source files.
 *
 * Further information at https://github.com/m2sf/m2c/wiki
 *
 * @file
 *
 * m2c-common.h
 *
 * Project wide common definitions.
 *
 * @license
 *
 * M2C is free software:  You can redistribute  and modify it  under the terms
 * of the  GNU Lesser General Public License (LGPL)  either version 2.1  or at
 * your choice version 3, both as published by the Free Software Foundation.
 *
 * M2C  is distributed  in the hope  that it will be useful,  but  WITHOUT ANY
 * WARRANTY;  without even  the implied warranty of MERCHANTABILITY or FITNESS
 * FOR ANY PARTICULAR PURPOSE.  Read the license for more details.
 *
 * You should have  received  a copy of the  GNU Lesser General Public License
 * along with M2C.  If not, see <https://www.gnu.org/copyleft/lesser.html>.
 */

#ifndef M2C_COMMON_H
#define M2C_COMMON_H

/* --------------------------------------------------------------------------
 * Import integer types
 * ----------------------------------------------------------------------- */

#if defined(VMS) || defined(__VMS)
#include <inttypes.h>
#else
#include <stdint.h>
#endif

/* --------------------------------------------------------------------------
 * Identification
 * ----------------------------------------------------------------------- */

#define M2C_IDENTIFICATION "M2C Modula-2 Compiler & Translator"

#define M2C_VERSION "1.00"


/* --------------------------------------------------------------------------
 * Common constants
 * ----------------------------------------------------------------------- */

#define ASCII_NUL 0

#define ASCII_EOT 4

#define ASCII_TAB 9

#define ASCII_LF 10

#define ASCII_CR 13

#define ASCII_SPACE 32

#define EMPTY_STRING "\0"


/* --------------------------------------------------------------------------
 * Common macros
 * ----------------------------------------------------------------------- */

#define NOT(_expr) \
  (!(_expr))

#define CAST(_type,_var) \
  ((_type) _var)

#define SET_STATUS(_status_ptr,_value) \
  { if (_status_ptr != NULL) {*_status_ptr = _value; }; }

#define WRITE_OUTPARAM(_outparam_ptr,_value) \
  { if (_outparam_ptr != NULL) {*_outparam_ptr = _value; }; }

#define IS_DIGIT(_ch) \
  (((_ch) >= '0') && ((_ch) <= '9'))

#define IS_A_TO_F(_ch) \
  (((_ch) >= 'A') && ((_ch) <= 'F'))

#define IS_UPPER(_ch) \
  (((_ch) >= 'A') && ((_ch) <= 'Z'))

#define IS_LOWER(_ch) \
  (((_ch) >= 'a') && ((_ch) <= 'z'))

#define IS_LETTER(_ch) \
  (IS_LOWER(_ch) || IS_UPPER(_ch))

#define IS_ALPHANUMERIC(_ch) \
  (IS_LETTER(_ch) || IS_DIGIT(_ch))

#define IS_CONTROL_CHAR(_ch) \
  ((((_ch) >= 0) && ((_ch) <= 0x1f)) || ((_ch) == 0x7f))


/* --------------------------------------------------------------------------
 * Common C types
 * ----------------------------------------------------------------------- */

typedef unsigned int uint_t;

typedef char *char_ptr_t;

typedef void *address_t;


/* --------------------------------------------------------------------------
 * Common M2 types
 * ----------------------------------------------------------------------- */

#define m2c_byte_t m2c_octet_t

#define m2c_address_t address_t

typedef uint8_t m2c_octet_t;

typedef unsigned char m2c_char_t;

typedef unsigned int m2c_cardinal_t;

typedef long unsigned int m2c_longcard_t;

typedef int m2c_integer_t;

typedef long int m2c_longint_t;

typedef float m2c_real_t;

typedef double m2c_longreal_t;


/* ==========================================================================
 * Verify build parameters
 * ======================================================================= */

#include "m2c-build-params.h"


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_INFILE_SIZE
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_INFILE_SIZE)
#error "no value defined for M2C_MAX_INFILE_SIZE"
#endif


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_INFILE_LINES
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_INFILE_LINES)
#error "no value defined for M2C_MAX_INFILE_LINES"
#endif


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_INFILE_COLUMNS
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_INFILE_COLUMNS)
#error "no value defined for M2C_MAX_INFILE_COLUMNS"
#endif


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_IDENT_LENGTH
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_IDENT_LENGTH)
#error "no value defined for M2C_MAX_IDENT_LENGTH"
#endif


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_NUMBER_LENGTH
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_NUMBER_LENGTH)
#error "no value defined for M2C_MAX_NUMBER_LENGTH"
#endif


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_STRING_LENGTH
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_STRING_LENGTH)
#error "no value defined for M2C_MAX_STRING_LENGTH"
#endif


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_C_MACRO_LENGTH
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_C_MACRO_LENGTH)
#error "no value defined for M2C_MAX_C_MACRO_LENGTH"
#endif


/* --------------------------------------------------------------------------
 * Verify M2C_MAX_C_IDENT_LENGTH
 * ----------------------------------------------------------------------- */

#if !defined(M2C_MAX_C_IDENT_LENGTH)
#error "no value defined for M2C_MAX_C_IDENT_LENGTH"
#endif


#endif /* M2C_COMMON_H */

/* END OF FILE */