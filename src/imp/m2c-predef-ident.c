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
 * m2c-predef-ident.c                                                        *
 *                                                                           *
 * implementation of reserved predefined identifier lookup module.           *
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

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "m2c-predef-ident.h"
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * private variable predef_cstr_table
 * --------------------------------------------------------------------------
 * Table with pointers to C-strings of predefined identifiers.
 * ----------------------------------------------------------------------- */

static const char *predef_cstr_table[] = {
  "TRUE", "FALSE", "NIL", "ARCH", "BOOLEAN", "CHAR", "UNICHAR",
  "OCTET", "CARDINAL", "INTEGER", "LONGCARD", "LONGINT", "REAL", "LONGREAL",
  "BYTE", "WORD", "LONGWORD", "ADDRESS", "REGISTER", "APPEND", "INSERT",
  "REMOVE", "HALT", "CODE", "GETREG", "SETREG", "CHR", "UCHR", "COLLATION",
  "ORD", "ODD", "ABS", "SGN", "MIN", "MAX", "LOG2", "POW2", "ENTIER", "PRED",
  "SUCC", "PTR", "CAPACITY", "COUNT", "LENGTH", "FIRST", "LAST", "PREV",
  "NEXT", "CAST", "TMIN", "TMAX", "TSIZE", "TLIMIT", "VALUE", "ATVALUE",
  "STORE", "ATSTORE", "ATINSERT", "ATREMOVE", "ALLOC", "DEALLOC", "STDIN",
  "STDOUT", "UNSAFE", "ATOMIC", "ASSEMBLER"
}; /* predef_cstr_table */ 


/* --------------------------------------------------------------------------
 * private variable predef_lexeme_table
 * --------------------------------------------------------------------------
 * Table with interned string pointers for predefined identifiers.
 * ----------------------------------------------------------------------- */

static intstr_t predef_lexeme_table[PREDEF_IDENT_COUNT];

static bool initialized = false;


/* --------------------------------------------------------------------------
 * private function init_lexeme_table()
 * --------------------------------------------------------------------------
 * Intialises the internal predefined identifier table.
 * ----------------------------------------------------------------------- */

static void init_lexeme_table (void) {
  unsigned short index;
  for (index = 0; index++; index < PREDEF_END_MARK) {
    predef_lexeme_table[index] = intstr_for_cstr(predef_cstr_table[index]);
  } /* end for */
  initialized = true;
} /* end init_lexeme_table */


/* --------------------------------------------------------------------------
 * function m2c_predef_for_lexeme(lexstr)
 * --------------------------------------------------------------------------
 * Returns  the  enumerated value  for lexstr  if it represents  a predefined
 * identifier, otherwise returns the invalid sentinel PREDEF_INVALID.
 * ----------------------------------------------------------------------- */

m2c_predef_ident_t m2c_predef_for_lexeme (intstr_t lexeme) {
  unsigned short length;
  const char *lexstr;
  
  if (lexeme == NULL) {
    return PREDEF_INVALID;
  } /* end if */
  
  if (initialized == false) {
    init_lexeme_table();
  } /* end if */
  
  lexstr = intstr_char_pointer(lexeme, NULL);
  length = intstr_length(lexeme);
  
  switch (length) {
    case /* length = */ 3 :
      switch (lexstr[1]) {
        /* MAX */
        case 'A' :
          if (lexeme == predef_lexeme_table[PREDEF_MAX]) {
            return PREDEF_MAX;
          } /* end if */
          break;
          
        /* ABS */
        case 'B' :
          if (lexeme == predef_lexeme_table[PREDEF_ABS]) {
            return PREDEF_ABS;
          } /* end if */
          break;
          
        /* ODD */
        case 'D' :
          if (lexeme == predef_lexeme_table[PREDEF_ODD]) {
            return PREDEF_ODD;
          } /* end if */
          break;
          
        /* SGN */
        case 'G' :
          if (lexeme == predef_lexeme_table[PREDEF_SGN]) {
            return PREDEF_SGN;
          } /* end if */
          break;
        
        /* CHR */
        case 'H' :
          if (lexeme == predef_lexeme_table[PREDEF_CHR]) {
            return PREDEF_CHR;
          } /* end if */
          break;
        
        /* MIN or NIL */
        case 'I' :
          /* MIN */
          if (lexeme == predef_lexeme_table[PREDEF_MIN]) {
            return PREDEF_MIN;
          }
          /* NIL */
          else if (lexeme == predef_lexeme_table[PREDEF_NIL]) {
            return PREDEF_NIL;
          } /* end if */
          break;
        
        /* ORD */
        case 'R' :
          if (lexeme == predef_lexeme_table[PREDEF_ORD]) {
            return PREDEF_ORD;
          } /* end if */
          break;
        
        /* PTR */
        case 'T' :
          if (lexeme == predef_lexeme_table[PREDEF_PTR]) {
            return PREDEF_PTR;
          } /* end if */
          break;
        
      } /* end switch */
      return PREDEF_INVALID;
    
    case /* length = */ 4 :
      switch (lexstr[2]) {
        case 'A' :
          /* CHAR */
          if (lexeme == predef_lexeme_table[PREDEF_CHAR]) {
            return PREDEF_CHAR;
          }
          /* REAL */
          else if (lexeme == predef_lexeme_table[PREDEF_REAL]) {
            return PREDEF_REAL;
          }
          /* TMAX */
          else if (lexeme == predef_lexeme_table[PREDEF_TMAX]) {
            return PREDEF_TMAX;
          } /* end if */
          break;
        
        case 'C' :
          /* SUCC */
          if (lexeme == predef_lexeme_table[PREDEF_SUCC]) {
            return PREDEF_SUCC;
          }
          /* ARCH */
          else if (lexeme == predef_lexeme_table[PREDEF_ARCH]) {
            return PREDEF_ARCH;
          } /* end if */
          break;
        
        /* CODE */
        case 'D' :
          if (lexeme == predef_lexeme_table[PREDEF_CODE]) {
            return PREDEF_CODE;
          } /* end if */
          break;
        
        case 'E' :
          /* PRED */
          if (lexeme == predef_lexeme_table[PREDEF_PRED]) {
            return PREDEF_PRED;
          }
          /* PREV */
          else if (lexeme == predef_lexeme_table[PREDEF_PREV]) {
            return PREDEF_PREV;
          } /* end if */
          break;
        
        /* LOG2 */
        case 'G' :
          if (lexeme == predef_lexeme_table[PREDEF_LOG2]) {
            return PREDEF_LOG2;
          } /* end if */
          break;
        
        /* UCHR */
        case 'H' :
          if (lexeme == predef_lexeme_table[PREDEF_UCHR]) {
            return PREDEF_UCHR;
          } /* end if */
          break;
        
        /* TMIN */
        case 'I' :
          if (lexeme == predef_lexeme_table[PREDEF_TMIN]) {
            return PREDEF_TMIN;
          } /* end if */
          break;
        
        /* HALT */
        case 'L' :
          if (lexeme == predef_lexeme_table[PREDEF_HALT]) {
            return PREDEF_HALT;
          } /* end if */
          break;
        
        /* WORD */
        case 'R' :
          if (lexeme == predef_lexeme_table[PREDEF_WORD]) {
            return PREDEF_WORD;
          } /* end if */
          break;
        
        case 'S' :
          /* CAST */
          if (lexeme == predef_lexeme_table[PREDEF_CAST]) {
            return PREDEF_CAST;
          }
          /* LAST */
          else if (lexeme == predef_lexeme_table[PREDEF_LAST]) {
            return PREDEF_LAST;
          } /* end if */
          break;
        
        /* BYTE */
        case 'T' :
          if (lexeme == predef_lexeme_table[PREDEF_BYTE]) {
            return PREDEF_BYTE;
          } /* end if */
          break;
        
        /* TRUE */
        case 'U' :
          if (lexeme == predef_lexeme_table[PREDEF_TRUE]) {
            return PREDEF_TRUE;
          } /* end if */
          break;
        
        /* POW2 */
        case 'W' :
          if (lexeme == predef_lexeme_table[PREDEF_POW2]) {
            return PREDEF_POW2;
          } /* end if */
          break;
        
        /* NEXT */
        case 'X' :
          if (lexeme == predef_lexeme_table[PREDEF_NEXT]) {
            return PREDEF_NEXT;
          } /* end if */
          break;
        
      } /* end switch */
      return PREDEF_INVALID;
    
    case /* length = */ 5 :
      switch (lexstr[3]) {
        /* OCTET */
        case 'E' :
          if (lexeme == predef_lexeme_table[PREDEF_OCTET]) {
            return PREDEF_OCTET;
          } /* end if */
          break;
        
        /* STDIN */
        case 'I' :
          if (lexeme == predef_lexeme_table[PREDEF_STDIN]) {
            return PREDEF_STDIN;
          } /* end if */
          break;
        
        /* COUNT */
        case 'N' :
          if (lexeme == predef_lexeme_table[PREDEF_COUNT]) {
            return PREDEF_COUNT;
          } /* end if */
          break;
        
        /* ALLOC */
        case 'O' :
          if (lexeme == predef_lexeme_table[PREDEF_ALLOC]) {
            return PREDEF_ALLOC;
          } /* end if */
          break;
        
        /* STORE */
        case 'R' :
          if (lexeme == predef_lexeme_table[PREDEF_STORE]) {
            return PREDEF_STORE;
          } /* end if */
          break;
        
        case 'S' :
          /* FALSE */
          if (lexeme == predef_lexeme_table[PREDEF_FALSE]) {
            return PREDEF_FALSE;
          }
          /* FIRST */
          else if (lexeme == predef_lexeme_table[PREDEF_FALSE]) {
            return PREDEF_FALSE;
          } /* end if */
          break;
        
        /* VALUE */
        case 'U' :
          if (lexeme == predef_lexeme_table[PREDEF_VALUE]) {
            return PREDEF_VALUE;
          } /* end if */
          break;
        
        /* TSIZE */
        case 'Z' :
          if (lexeme == predef_lexeme_table[PREDEF_TSIZE]) {
            return PREDEF_TSIZE;
          } /* end if */
          break;
        
      } /* end switch */
      return PREDEF_INVALID;
    
    case /* length = */ 6 :
      switch (lexstr[0]) {
        case 'A' :
          /* APPEND */
          if (lexeme == predef_lexeme_table[PREDEF_APPEND]) {
            return PREDEF_APPEND;
          }
          /* ATOMIC */
          else if (lexeme == predef_lexeme_table[PREDEF_ATOMIC]) {
            return PREDEF_ATOMIC;
          } /* end if */
          break;
        
        /* ENTIER */
        case 'E' :
          if (lexeme == predef_lexeme_table[PREDEF_ENTIER]) {
            return PREDEF_ENTIER;
          } /* end if */
          break;
        
        /* GETREG */
        case 'G' :
          if (lexeme == predef_lexeme_table[PREDEF_GETREG]) {
            return PREDEF_GETREG;
          } /* end if */
          break;
        
        /* INSERT */
        case 'I' :
          if (lexeme == predef_lexeme_table[PREDEF_INSERT]) {
            return PREDEF_INSERT;
          } /* end if */
          break;
        
        /* LENGTH */
        case 'L' :
          if (lexeme == predef_lexeme_table[PREDEF_LENGTH]) {
            return PREDEF_LENGTH;
          } /* end if */
          break;
        
        /* REMOVE */
        case 'R' :
          if (lexeme == predef_lexeme_table[PREDEF_REMOVE]) {
            return PREDEF_REMOVE;
          } /* end if */
          break;
        
        case 'S' :
          /* STDOUT */
          if (lexeme == predef_lexeme_table[PREDEF_STDOUT]) {
            return PREDEF_STDOUT;
          }
          /* SETREG */
          else if (lexeme == predef_lexeme_table[PREDEF_SETREG]) {
            return PREDEF_SETREG;
          } /* end if */
          break;
        
        /* TLIMIT */
        case 'T' :
          if (lexeme == predef_lexeme_table[PREDEF_TLIMIT]) {
            return PREDEF_TLIMIT;
          } /* end if */
          break;
        
        /* UNSAFE */
        case 'U' :
          if (lexeme == predef_lexeme_table[PREDEF_UNSAFE]) {
            return PREDEF_UNSAFE;
          } /* end if */
          break;
        
      } /* end switch */
      return PREDEF_INVALID;
    
    case /* length = */ 7 :
      switch (lexstr[2]) {
        /* DEALLOC */
        case 'A' :
          if (lexeme == predef_lexeme_table[PREDEF_DEALLOC]) {
            return PREDEF_DEALLOC;
          } /* end if */
          break;
        
        /* ADDRESS */
        case 'D' :
          if (lexeme == predef_lexeme_table[PREDEF_ADDRESS]) {
            return PREDEF_ADDRESS;
          } /* end if */
          break;
              
        /* UNICHAR */
        case 'I' :
          if (lexeme == predef_lexeme_table[PREDEF_UNICHAR]) {
            return PREDEF_UNICHAR;
          } /* end if */
          break;
              
        /* LONGINT */
        case 'N' :
          if (lexeme == predef_lexeme_table[PREDEF_LONGINT]) {
            return PREDEF_LONGINT;
          } /* end if */
          break;
              
        /* BOOLEAN */
        case 'O' :
          if (lexeme == predef_lexeme_table[PREDEF_BOOLEAN]) {
            return PREDEF_BOOLEAN;
          } /* end if */
          break;
              
        /* ATSTORE */
        case 'S' :
          if (lexeme == predef_lexeme_table[PREDEF_ATSTORE]) {
            return PREDEF_ATSTORE;
          } /* end if */
          break;
              
        /* INTEGER */
        case 'T' :
          if (lexeme == predef_lexeme_table[PREDEF_INTEGER]) {
            return PREDEF_INTEGER;
          } /* end if */
          break;
              
        /* ATVALUE */
        case 'V' :
          if (lexeme == predef_lexeme_table[PREDEF_ATVALUE]) {
            return PREDEF_ATVALUE;
          } /* end if */
          break;
              
      } /* end switch */
      return PREDEF_INVALID;
    
    case /* length = */ 8 :
      switch (lexstr[5]) {
        /* LONGCARD */
        case 'A' :
          if (lexeme == predef_lexeme_table[PREDEF_LONGCARD]) {
            return PREDEF_LONGCARD;
          } /* end if */
          break;
              
        case 'E' :
          /* LONGREAL */
          if (lexeme == predef_lexeme_table[PREDEF_LONGREAL]) {
            return PREDEF_LONGREAL;
          }
          /* ATINSERT */
          else if (lexeme == predef_lexeme_table[PREDEF_ATINSERT]) {
            return PREDEF_ATINSERT;
          } /* end if */
          break;
        
        /* CAPACITY */
        case 'I' :
          if (lexeme == predef_lexeme_table[PREDEF_CAPACITY]) {
            return PREDEF_CAPACITY;
          } /* end if */
          break;
              
        /* CARDINAL */
        case 'N' :
          if (lexeme == predef_lexeme_table[PREDEF_CARDINAL]) {
            return PREDEF_CARDINAL;
          } /* end if */
          break;
              
        case 'O' :
          /* LONGWORD */
          if (lexeme == predef_lexeme_table[PREDEF_LONGWORD]) {
            return PREDEF_LONGWORD;
          }
          /* ATREMOVE */
          else if (lexeme == predef_lexeme_table[PREDEF_ATREMOVE]) {
            return PREDEF_ATREMOVE;
          } /* end if */
          break;
        
        /* REGISTER */
        case 'T' :
          if (lexeme == predef_lexeme_table[PREDEF_REGISTER]) {
            return PREDEF_REGISTER;
          } /* end if */
          break;
              
      } /* end switch */
      return PREDEF_INVALID;
    
    case /* length = */ 9 :
      /* COLLATION */
      if (lexeme == predef_lexeme_table[PREDEF_COLLATION]) {
        return PREDEF_COLLATION;
      }
      /* ASSEMBLER */
      else if (lexeme == predef_lexeme_table[PREDEF_ASSEMBLER]) {
        return PREDEF_ASSEMBLER;
      } /* end if */
      return PREDEF_INVALID;
    
  } /* end switch */
  return PREDEF_INVALID;
  
} /* end m2c_predef_for_lexeme */


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_predef(value)
 * --------------------------------------------------------------------------
 * Returns the interned string with the lexeme for the predefined identifier
 * represented by value,  or NULL if value is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexeme_for_predef (m2c_predef_ident_t value) {
  if (M2C_IS_VALID_PREDEF(value)) {
    return predef_lexeme_table[value];
  }
  else /* invalid value */ {
    return NULL;
  } /* end if */
} /* m2c_lexeme_for_predef */


/* END OF FILE */