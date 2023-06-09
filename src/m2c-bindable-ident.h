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
 * m2c-bindable-ident.h                                                      *
 *                                                                           *
 * Public interface for bindable identifier lookup module.                   *
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

#ifndef M2C_BINDABLE_IDENT_H
#define M2C_BINDABLE_IDENT_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "interned-strings.h"


/* --------------------------------------------------------------------------
 * type m2c_bindable_t
 * --------------------------------------------------------------------------
 * Enumerated token values representing bindable identifiers.
 * ----------------------------------------------------------------------- */

typedef enum {

  /* Invalid Sentinel */
  
  BINDABLE_INVALID,

  /* Constant Bindable Identifiers */
  
  BINDABLE_COLLATION,         /* COLLATION */
  BINDABLE_TLIMIT,            /* TLIMIT */
  
  /* Procedure Bindable Identifiers */
  
  BINDABLE_ALLOC,             /* ALLOC */
  BINDABLE_APPEND,            /* APPEND */
  BINDABLE_ATINSERT,          /* ATINSERT */
  BINDABLE_ATREMOVE,          /* ATREMOVE */
  BINDABLE_ATSTORE,           /* ATSTORE */
  BINDABLE_ATVALUE,           /* ATVALUE */
  BINDABLE_COUNT,             /* COUNT */
  BINDABLE_DEALLOC,           /* DEALLOC */
  BINDABLE_FIRST,             /* FIRST */
  BINDABLE_LAST,              /* LAST */
  BINDABLE_LENGTH,            /* LENGTH */
  BINDABLE_NEXT,              /* NEXT */
  BINDABLE_PREV,              /* PREV */
  BINDABLE_REMOVE,            /* REMOVE */
  BINDABLE_STDIN,             /* STDIN */
  BINDABLE_STDOUT,            /* STDOUT */
  BINDABLE_STORE,             /* STORE */
  BINDABLE_VALUE,             /* VALUE */
  
  /* Enumeration Terminator */
  
  BINDABLE_END_MARK           /* marks the end of the enumeration */
} m2c_bindable_t;


/* --------------------------------------------------------------------------
 * bindable identifier count
 * ----------------------------------------------------------------------- */

#define BINDABLE_IDENT_COUNT (BINDABLE_END_MARK-2)


/* --------------------------------------------------------------------------
 * bindable group boundaries
 * ----------------------------------------------------------------------- */

#define FIRST_BINDABLE (BINDABLE_INVALID+1)
#define LAST_BINDABLE (BINDABLE_END_MARK-1)

#define FIRST_CONST_BINDABLE BINDABLE_COLLATION
#define LAST_CONST_BINDABLE BINDABLE_TLIMIT

#define FIRST_PROC_BINDABLE BINDABLE_ALLOC
#define LAST_PROC_BINDABLE BINDABLE_VALUE


/* --------------------------------------------------------------------------
 * macro M2C_IS_VALID_BINDABLE(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a bindable, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_VALID_BINDABLE(_value) \
  ((_value >= FIRST_BINDABLE) && (_value <= LAST_BINDABLE))


/* --------------------------------------------------------------------------
 * macro M2C_IS_CONST_BINDABLE(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a const bindable symbol, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_CONST_BINDABLE(_value) \
  ((_value >= FIRST_CONST_BINDABLE) && (_value <= LAST_CONST_BINDABLE))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PROC_BINDABLE(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a proc bindable symbol, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PROC_BINDABLE(_value) \
  ((_value >= FIRST_PROC_BINDABLE) && (_value <= LAST_PROC_BINDABLE))


/* --------------------------------------------------------------------------
 * function m2c_bindable_for_lexeme(lexeme)
 * --------------------------------------------------------------------------
 * Returns the  bindable token  for lexeme,  or BINDABLE_INVALID if lexstr is
 * not a bindable identifier.
 * ----------------------------------------------------------------------- */

m2c_bindable_t m2c_bindable_for_lexeme (intstr_t lexeme);


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_bindable(value)
 * --------------------------------------------------------------------------
 * Returns  the interned string  with the lexeme  of the  bindable identifier
 * represented by value,  or NULL if value is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexeme_for_bindable (m2c_bindable_t value);


#endif /* M2C_BINDABLE_IDENT_H */

/* END OF FILE */