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
 * m2c-bindables.h                                                           *
 *                                                                           *
 * Public interface of bindables module.                                     *
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

#ifndef M2C_BINDABLES_H
#define M2C_BINDABLES_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "intstr.h"
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_bindable_t
 * --------------------------------------------------------------------------
 * Enumerated values representing bindable symbols.
 * ----------------------------------------------------------------------- */

typedef enum {

  /* Invalid Bindable Sentinel */
  BINDABLE_INVALID,

  /* Constant Bindables */
  
  BINDABLE_COLLATION,         /* COLLATION */
  BINDABLE_TLIMIT,            /* TLIMIT */
  
  /* Procedure Bindables */
  
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
  BINDABLE_NEW,               /* NEW */
  BINDABLE_NEWARGS,           /* NEW+ */
  BINDABLE_NEWCAP,            /* NEW# */
  BINDABLE_NEXT,              /* NEXT */
  BINDABLE_PREV,              /* PREV */
  BINDABLE_READ,              /* READ */
  BINDABLE_READNEW,           /* READ* */
  BINDABLE_RELEASE,           /* RELEASE */
  BINDABLE_RETAIN,            /* RETAIN */
  BINDABLE_REMOVE,            /* REMOVE */
  BINDABLE_STDIN,             /* STDIN */
  BINDABLE_STDOUT,            /* STDOUT */
  BINDABLE_STORE,             /* STORE */
  BINDABLE_SUCC,              /* SUCC */
  BINDABLE_VALUE,             /* VALUE */
  BINDABLE_WRITE,             /* WRITE */
  BINDABLE_WRITEF,            /* WRITE# */
  
  /* Enumeration Terminator */
  
  BINDABLE_END_MARK           /* marks the end of the enumeration */
} m2c_bindable_t;


/* --------------------------------------------------------------------------
 * bindable group boundaries
 * ----------------------------------------------------------------------- */

#define FIRST_BINDABLE (BINDABLE_INVALID+1)
#define LAST_BINDABLE (BINDABLE_END_MARK-1)

#define FIRST_CONST_BINDABLE BINDABLE_COLLATION
#define LAST_CONST_BINDABLE BINDABLE_TLIMIT

#define FIRST_PROC_BINDABLE BINDABLE_ALLOC
#define LAST_PROC_BINDABLE BINDABLE_WRITEF


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
 * function m2c_bindable_for_lexeme(lexstr)
 * --------------------------------------------------------------------------
 * Returns the bindable for lexstr if it represents a bindable symbol, other-
 * wise returns the invalid bindable sentinel BINDABLE_INVALID.
 * ----------------------------------------------------------------------- */

bool m2c_bindable_for_lexeme (intstr_t lexstr);


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_bindable(value)
 * --------------------------------------------------------------------------
 * Returns the  interned lexeme  of the bindable symbol represented by value,
 * or NULL if value is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexeme_for_bindable (m2c_bindable_t value);


#endif /* M2C_BINDABLES_H */

/* END OF FILE */