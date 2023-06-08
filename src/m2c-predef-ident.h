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
 * m2c-predef-ident.h                                                        *
 *                                                                           *
 * Public interface of reserved predefined identifier lookup module.         *
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

#ifndef M2C_PREDEF_IDENT_H
#define M2C_PREDEF_IDENT_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "intstr.h"


/* --------------------------------------------------------------------------
 * type m2c_predef_ident_t
 * --------------------------------------------------------------------------
 * Enumerated values representing reserved predefined identifiers.
 * ----------------------------------------------------------------------- */

typedef enum {

  /* Invalid Sentinel */
  PREDEF_INVALID,

  /* Predefined Constant Identifiers */
  
  PREDEF_TRUE,             /* TRUE */
  PREDEF_FALSE,            /* FALSE */
  PREDEF_NIL,              /* NIL */
  
  PREDEF_ARCH,             /* ASSEMBLER.ARCH */
  
  /* Predefined Type Identifiers */
  
  PREDEF_BOOLEAN,          /* BOOLEAN */
  PREDEF_CHAR,             /* CHAR */
  PREDEF_UNICHAR,          /* UNICHAR */
  PREDEF_OCTET,            /* OCTET */
  PREDEF_CARDINAL,         /* CARDINAL */
  PREDEF_INTEGER,          /* INTEGER */
  PREDEF_LONGCARD,         /* LONGCARD */
  PREDEF_LONGINT,          /* LONGINT */
  PREDEF_REAL,             /* REAL */
  PREDEF_LONGREAL,         /* LONGREAL */
  
  PREDEF_BYTE,             /* UNSAFE.BYTE */
  PREDEF_WORD,             /* UNSAFE.WORD */
  PREDEF_LONGWORD,         /* UNSAFE.LONGWORD */
  PREDEF_ADDRESS,          /* UNSAFE.ADDRESS */
  
  PREDEF_REGISTER,         /* ASSEMBLER.REGISTER */
  
  Predefined Procedure Identifiers */
  
  PREDEF_APPEND,           /* APPEND */
  PREDEF_INSERT,           /* INSERT */
  PREDEF_REMOVE,           /* REMOVE */
  
  PREDEF_HALT,             /* UNSAFE.HALT */
  PREDEF_CODE,             /* ASSEMBLER.CODE */
  PREDEF_GETREG,           /* ASSEMBLER.GETREG */
  PREDEF_SETREG,           /* ASSEMBLER.SETREG */
  
  /* Predefined Function Identifiers */
  
  PREDEF_CHR,              /* CHR */
  PREDEF_UCHR,             /* UCHR */
  PREDEF_COLLATION,        /* COLLATION */
  PREDEF_ORD,              /* ORD */
  PREDEF_ODD,              /* ODD */
  PREDEF_ABS,              /* ABS */
  PREDEF_SGN,              /* SGN */
  PREDEF_MIN,              /* MIN */
  PREDEF_MAX,              /* MAX */
  PREDEF_LOG2,             /* LOG2 */
  PREDEF_POW2,             /* POW2 */
  PREDEF_ENTIER,           /* ENTIER */
  PREDEF_PRED,             /* PRED */
  PREDEF_SUCC,             /* SUCC */
  PREDEF_PTR,              /* PTR */
  PREDEF_CAPACITY,         /* CAPACITY */
  PREDEF_COUNT,            /* COUNT */
  PREDEF_LENGTH,           /* LENGTH */
  PREDEF_FIRST,            /* FIRST */
  PREDEF_LAST,             /* LAST */
  PREDEF_PREV,             /* PREV */
  PREDEF_NEXT,             /* NEXT */
  
  PREDEF_CAST,             /* UNSAFE.CAST */
  
  /* Predefined Macro Identifiers */
  
  PREDEF_TMIN,             /* TMIN */
  PREDEF_TMAX,             /* TMAX */
  PREDEF_TSIZE,            /* TSIZE */
  PREDEF_TLIMIT,           /* TLIMIT */
  
  /* Predefined Primitive Identifiers */
  
  PREDEF_VALUE,            /* VALUE */
  PREDEF_ATVALUE,          /* ATVALUE */
  PREDEF_STORE,            /* ATSTORE */
  PREDEF_ATINSERT,         /* ATINSERT */
  PREDEF_ATREMOVE,         /* ATREMOVE */
  PREDEF_ALLOC,            /* ALLOC */
  PREDEF_DEALLOC,          /* DEALLOC */
  PREDEF_STDIN,            /* STDIN */
  PREDEF_STDOUT,           /* STDOUT */
  
  /* Predefined Module Identifiers */
  
  PREDEF_UNSAFE,           /* UNSAFE */
  PREDEF_ATOMIC,           /* ATOMIC */
  PREDEF_ASSEMBLER,        /* ASSEMBLER */
    
  PREDEF_END_MARK          /* marks the end of the enumeration */
} m2c_predef_ident_t;


/* --------------------------------------------------------------------------
 * predefined identifier group boundaries
 * ----------------------------------------------------------------------- */

#define FIRST_PREDEF (PREDEF_INVALID+1)
#define LAST_PREDEF (PREDEF_END_MARK-1)

#define FIRST_PREDEF_CONST PREDEF_TRUE
#define LAST_PREDEF_CONST PREDEF_ARCH

#define FIRST_PREDEF_TYPE PREDEF_BOOLEAN
#define LAST_PREDEF_TYPE PREDEF_REGISTER

#define FIRST_PREDEF_PROC PREDEF_APPEND
#define LAST_PREDEF_PROC PREDEF_SETREG

#define FIRST_PREDEF_FUNC PREDEF_CHR
#define LAST_PREDEF_FUNC PREDEF_CAST

#define FIRST_PREDEF_MACRO PREDEF_TMIN
#define LAST_PREDEF_MACRO PREDEF_TLMIT

#define FIRST_PREDEF_PRIMITIVE PREDEF_VALUE
#define LAST_PREDEF_PRIMITIVE PREDEF_STDOUT

#define FIRST_PREDEF_MODULE PREDEF_UNSAFE
#define LAST_PREDEF_MODULE PREDEF_ASSEMBLER


/* --------------------------------------------------------------------------
 * macro M2C_IS_VALID_PREDEF(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined identifier, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_VALID_PREDEF(_value) \
  ((_value >= FIRST_PREDEF) && (_value <= LAST_PREDEF))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PREDEF_CONST(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined constant, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PREDEF_CONST(_value) \
  ((_value >= FIRST_PREDEF_CONST) && (_value <= LAST_PREDEF_CONST))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PREDEF_TYPE(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined type, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PREDEF_TYPE(_value) \
  ((_value >= FIRST_PREDEF_TYPE) && (_value <= LAST_PREDEF_TYPE))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PREDEF_PROC(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined procedure, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PREDEF_PROC(_value) \
  ((_value >= FIRST_PREDEF_PROC) && (_value <= LAST_PREDEF_PROC))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PREDEF_FUNC(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined function, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PREDEF_FUNC(_value) \
  ((_value >= FIRST_PREDEF_FUNC) && (_value <= LAST_PREDEF_FUNC))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PREDEF_MACRO(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined macro, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PREDEF_MACRO(_value) \
  ((_value >= FIRST_PREDEF_MACRO) && (_value <= LAST_PREDEF_MACRO))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PREDEF_PRIMITIVE(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined primitive, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PREDEF_PRIMITIVE(_value) \
  ((_value >= FIRST_PREDEF_PRIMITIVE) && (_value <= LAST_PREDEF_PRIMITIVE))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PREDEF_MODULE(value)
 * --------------------------------------------------------------------------
 * Returns true if value represents a predefined module, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PREDEF_MODULE(_value) \
  ((_value >= FIRST_PREDEF_MODULE) && (_value <= LAST_PREDEF_MODULE))


/* --------------------------------------------------------------------------
 * function m2c_predef_for_lexeme(lexstr)
 * --------------------------------------------------------------------------
 * Returns  the  enumerated value  for lexstr  if it represents  a predefined
 * identifier, otherwise returns the invalid sentinel PREDEF_INVALID.
 * ----------------------------------------------------------------------- */

m2c_predef_ident_t m2c_predef_for_lexeme (intstr_t lexstr);


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_predef(value)
 * --------------------------------------------------------------------------
 * Returns the  interned lexeme  of the predefined identifier  represented by
 * value,  or NULL if value is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexeme_for_predef (m2c_predef_ident_t value);


#endif /* M2C_PREDEF_IDENT_H */

/* END OF FILE */