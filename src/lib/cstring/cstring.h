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
  * cstring.h                                                                 *
  *                                                                           *
  * Public interface of basic C string library.                               *
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

#ifndef CSTRING_H
#define CSTRING_H

#include <stdbool.h>


/* --------------------------------------------------------------------------
 * Collation mode for string comparison.
 * --------------------------------------------------------------------------
 * Collation modes are defined as follows:
 *
 * o  COLLATION_MODE_ASCII uses the 7-bit ASCII table as collation sequence.
 * o  COLLATION_MODE_DICT_LOWER uses the following collation sequence:
 *      #$%*+0123456789aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ
 * o  COLLATION_MODE_DICT_UPPER uses the following collation sequence:
 *      #$%*+0123456789AaBbCcDdEeFfGgHhIiJiKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz
 *
 * In dictionary mode, whitespace and minus rank before all other characters
 * and any characters not listed in the collation sequence are ignored.
 * ----------------------------------------------------------------------- */

typedef enum {
  COLLATION_MODE_ASCII,      /* ASCII mode */
  COLLATION_MODE_DICT_LOWER, /* Dictionary mode, lower- before uppercase */
  COLLATION_MODE_DICT_UPPER  /* Dictionary mode, upper- before lowercase */
} cstr_collation_mode_t;


/* --------------------------------------------------------------------------
 * Collation status.
 * ----------------------------------------------------------------------- */

typedef enum {
  COLLATION_PREDECESSOR,
  COLLATION_SUCCESSOR,
  COLLATION_EQUIVALENT,
  COLLATION_FAILURE
} cstr_collation_status_t;


/* --------------------------------------------------------------------------
 * function cstr_length(cstr)
 * --------------------------------------------------------------------------
 * Returns the length of C string cstr.  Returns 0 if cstr is NULL.
 * ----------------------------------------------------------------------- */

unsigned cstr_length (const char *cstr);


/* --------------------------------------------------------------------------
 * function macro CSTR_FIRST_CHAR(cstr)
 * --------------------------------------------------------------------------
 * Returns first character of cstr.  Causes runtime fault if cstr is NULL.
 * ----------------------------------------------------------------------- */

#define CSTR_FIRST_CHAR(_cstr) (_cstr[0])


/* --------------------------------------------------------------------------
 * function cstr_last_char(cstr)
 * --------------------------------------------------------------------------
 * Returns last character of cstr, or ASCII NUL if cstr is NULL or empty.
 * ----------------------------------------------------------------------- */

char cstr_last_char (const char *cstr);


/* --------------------------------------------------------------------------
 * function cstr_contains_char(cstr, ch)
 * --------------------------------------------------------------------------
 * Returns true if cstr contains ch, otherwise false.
 * ----------------------------------------------------------------------- */

bool cstr_contains_char(const char *cstr, char ch);


/* --------------------------------------------------------------------------
 * function cstr_match(cstr1, cstr2)
 * --------------------------------------------------------------------------
 * Returns true if cstr1 and cstr2 match, otherwise false.
 * If any argument is NULL, false is returned.
 * ----------------------------------------------------------------------- */

bool cstr_match (const char *cstr1, const char *cstr2);


/* --------------------------------------------------------------------------
 * function cstr_collation(cstr1, cstr2, mode)
 * --------------------------------------------------------------------------
 * Compares cstr1 and cstr2 using collation mode and returns a result code:
 *
 * o  COLLATION_PREDECESSOR if cstr1 precedes cstr2,
 * o  COLLATION_SUCCESSOR if cstr1 succeeds cstr2,
 * o  COLLATION_EQUIVALENT if cstr1 is equivalent to cstr2,
 * o  COLLATION_FAILURE if cstr1 or cstr2 is NULL.
 * ----------------------------------------------------------------------- */

cstr_collation_status_t cstr_collation
  (const char *cstr1, const char *cstr2, cstr_collation_mode_t mode);


/* --------------------------------------------------------------------------
 * function new_cstr_from_slice(source, start_index, length)
 * --------------------------------------------------------------------------
 * Returns a newly allocated and NUL terminated C string containing length
 * characters from C string source starting at start_index.  Returns NULL
 * if source is NULL, if length is 0, if the length of source is less than
 * the required length (start_index + length) or if allocation fails.
 * ----------------------------------------------------------------------- */

const char *new_cstr_from_slice
  (const char *source, unsigned start_index, unsigned length);


/* --------------------------------------------------------------------------
 * function new_cstr_by_concat(first, cstr1, cstr2, ... cstrN)
 * --------------------------------------------------------------------------
 * Returns a newly allocated and NUL terminated C string containing the
 * concatenation of all its arguments in left-to-right order. The list of
 * arguments must be terminated by NULL.  Returns NULL if first is NULL.
 * ----------------------------------------------------------------------- */

const char *new_cstr_by_concat (const char *first, ...);


#endif /* CSTRING_H */

/* END OF FILE */
