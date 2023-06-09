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
 * m2c-schroed-token.h                                                       *
 *                                                                           *
 * Public interface for Schroedinger's token module.                         *
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

#ifndef M2C_SCHROED_TOKEN_H
#define M2C_SCHROED_TOKEN_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "interned-strings.h"
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_schroed_t
 * --------------------------------------------------------------------------
 * Enumerated token values for Schroedinger's tokens: symbols that may either
 * be reserved words or predefined identifiers depending on context.
 * ----------------------------------------------------------------------- */

typedef enum {

  /* Invalid Sentinel */
  
  SCHROED_INVALID,

  /*  */
  
  SCHROED_ADDRESS,            /* ADDRESS */
  SCHROED_CAPACITY,           /* CAPACITY */
  SCHROED_CAST,               /* CAST */
  SCHROED_NIL,                /* NIL */
  
  /* Enumeration Terminator */
  
  SCHROED_END_MARK           /* marks the end of the enumeration */
} m2c_schroed_t;


/* --------------------------------------------------------------------------
 * Schroedinger's token count
 * ----------------------------------------------------------------------- */

#define SCHROED_COUNT (SCHROED_END_MARK-1)


/* --------------------------------------------------------------------------
 * boundaries
 * ----------------------------------------------------------------------- */

#define FIRST_SCHROED (SCHROED_INVALID+1)
#define LAST_SCHROED (SCHROED_END_MARK-1)


/* --------------------------------------------------------------------------
 * macro M2C_IS_VALID_SCHROED(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a Schroedinger's token, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_VALID_SCHROED(_value) \
  ((_value >= FIRST_SCHROED) && (_value <= LAST_SCHROED))


/* --------------------------------------------------------------------------
 * function m2c_ident_is_schroed(lexeme)
 * --------------------------------------------------------------------------
 * Returns true if lexeme is a Schroedinger's token identifier, else false.
 * ----------------------------------------------------------------------- */

bool m2c_ident_is_schroed (intstr_t lexeme);


/* --------------------------------------------------------------------------
 * function m2c_schroed_for_lexeme(lexeme)
 * --------------------------------------------------------------------------
 * Returns the Schroedinger's token for lexeme,  or SCHROED_INVALID if lexeme
 * is not a Schroedering's token identifier.
 * ----------------------------------------------------------------------- */

m2c_schroed_t m2c_schroed_for_lexeme (intstr_t lexeme);


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_schroed(value)
 * --------------------------------------------------------------------------
 * Returns  the interned string  with the lexeme  of the Schroedinger's token
 * represented by value,  or NULL if value is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexeme_for_schroed (m2c_schroed_t value);


#endif /* M2C_SCHROED_TOKEN_H */

/* END OF FILE */