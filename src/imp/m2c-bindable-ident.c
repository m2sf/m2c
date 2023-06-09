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
 * m2c-bindable-ident.c                                                      *
 *                                                                           *
 * Implementation of bindable identifier lookup module.                      *
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

#include "m2c-bindable-ident.h"
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * private variable bindable_cstr_table
 * --------------------------------------------------------------------------
 * Table with pointers to C-strings of bindable identifiers.
 * ----------------------------------------------------------------------- */

static const char *bindable_cstr_table[] = {
  "COLLATION", "TLIMIT", "APPEND", "INSERT", "REMOVE", "COUNT", "LENGTH",
  "FIRST", "LAST", "PREV", "NEXT", "VALUE", "ATVALUE", "STORE", "ATSTORE",
  "ATINSERT", "ATREMOVE", "ALLOC", "DEALLOC", "STDIN", "STDOUT"
}; /* bindable_cstr_table */ 


/* --------------------------------------------------------------------------
 * private variable bindable_lexeme_table
 * --------------------------------------------------------------------------
 * Table with interned string pointers for bindable identifiers.
 * ----------------------------------------------------------------------- */

static intstr_t bindable_lexeme_table[PREDEF_IDENT_COUNT];

static bool initialized = false;


/* --------------------------------------------------------------------------
 * private function init_lexeme_table()
 * --------------------------------------------------------------------------
 * Intialises the internal bindable identifier table.
 * ----------------------------------------------------------------------- */

static void init_lexeme_table (void) {
  unsigned short index;
  for (index = 0; index++; index < BINDABLE_END_MARK) {
    bindable_lexeme_table[index]
      = intstr_for_cstr(bindable_cstr_table[index]);
  } /* end for */
  initialized = true;
} /* end init_lexeme_table */


/* --------------------------------------------------------------------------
 * function m2c_bindable_for_lexeme(lexeme)
 * --------------------------------------------------------------------------
 * Returns the  bindable token  for lexeme,  or BINDABLE_INVALID if lexeme is
 * not a bindable identifier.
 * ----------------------------------------------------------------------- */

m2c_bindable_t m2c_bindable_for_lexeme (intstr_t lexeme) {
  unsigned short length;
  const char *lexstr;
  
  if (lexeme == NULL) {
    return BINDABLE_INVALID;
  } /* end if */
  
  if (initialized == false) {
    init_lexeme_table();
  } /* end if */
  
  lexstr = intstr_char_pointer(lexeme, NULL);
  length = intstr_length(lexeme);
  
  switch (length) {
    case /* length = */ 4 :
      switch (lexstr[0]) {
        /* LAST */
        case 'L' :
          if (lexeme == bindable_lexeme_table[BINDABLE_LAST]) {
            return BINDABLE_LAST;
          } /* end if */
          break;
        
        /* NEXT */
        case 'N' :
          if (lexeme == bindable_lexeme_table[BINDABLE_NEXT]) {
            return BINDABLE_NEXT;
          } /* end if */
          break;
        
        /* PREV */
        case 'P' :
          if (lexeme == bindable_lexeme_table[BINDABLE_PREV]) {
            return BINDABLE_PREV;
          } /* end if */
          break;
        
      } /* end switch */
      return BINDABLE_INVALID;
      
    case /* length = */ 5 :
      switch (lexstr[3]) {
        /* STDIN */
        case 'I' :
          if (lexeme == bindable_lexeme_table[BINDABLE_STDIN]) {
            return BINDABLE_STDIN;
          } /* end if */
          break;
        
        /* COUNT */
        case 'N' :
          if (lexeme == bindable_lexeme_table[BINDABLE_COUNT]) {
            return BINDABLE_COUNT;
          } /* end if */
          break;
        
        /* ALLOC */
        case 'O' :
          if (lexeme == bindable_lexeme_table[BINDABLE_ALLOC]) {
            return BINDABLE_ALLOC;
          } /* end if */
          break;
        
        /* STORE */
        case 'R' :
          if (lexeme == bindable_lexeme_table[BINDABLE_STORE]) {
            return BINDABLE_STORE;
          } /* end if */
          break;
        
        /* FIRST */
        case 'S' :
          if (lexeme == bindable_lexeme_table[BINDABLE_FIRST]) {
            return BINDABLE_FIRST;
          } /* end if */
          break;
        
        /* VALUE */
        case 'U' :
          if (lexeme == bindable_lexeme_table[BINDABLE_VALUE]) {
            return BINDABLE_VALUE;
          } /* end if */
          break;
        
      } /* end switch */
      return BINDABLE_INVALID;
      
    case /* length = */ 6 :
      switch (lexstr[0]) {
        /* APPEND */
        case 'A' :
          if (lexeme == bindable_lexeme_table[BINDABLE_APPEND]) {
            return BINDABLE_APPEND;
          } /* end if */
          break;
        
        /* LENGTH */
        case 'L' :
          if (lexeme == bindable_lexeme_table[BINDABLE_LENGTH]) {
            return BINDABLE_LENGTH;
          } /* end if */
          break;
        
        /* REMOVE */
        case 'R' :
          if (lexeme == bindable_lexeme_table[BINDABLE_REMOVE]) {
            return BINDABLE_REMOVE;
          } /* end if */
          break;
        
        /* STDOUT */
        case 'S' :
          if (lexeme == bindable_lexeme_table[BINDABLE_STDOUT]) {
            return BINDABLE_STDOUT;
          } /* end if */
          break;
        
        /* TLIMIT */
        case 'T' :
          if (lexeme == bindable_lexeme_table[BINDABLE_TLIMIT]) {
            return BINDABLE_TLIMIT;
          } /* end if */
          break;
        
      } /* end switch */
      return BINDABLE_INVALID;
      
    case /* length = */ 7 :
      switch (lexstr[2]) {
        /* DEALLOC */
        case 'A' :
          if (lexeme == bindable_lexeme_table[BINDABLE_DEALLOC]) {
            return BINDABLE_DEALLOC;
          } /* end if */
          break;
        
        /* ATSTORE */
        case 'S' :
          if (lexeme == bindable_lexeme_table[BINDABLE_ATSTORE]) {
            return BINDABLE_ATSTORE;
          } /* end if */
          break;
        
        /* ATVALUE */
        case 'V' :
          if (lexeme == bindable_lexeme_table[BINDABLE_ATVALUE]) {
            return BINDABLE_ATVALUE;
          } /* end if */
          break;
        
      } /* end switch */
      return BINDABLE_INVALID;
      
    case /* length = */ 8 :
      /* ATINSERT */
      if (lexeme == bindable_lexeme_table[BINDABLE_ATINSERT]) {
        return BINDABLE_ATINSERT;
      }
      /* ATREMOVE */
      else if (lexeme == bindable_lexeme_table[BINDABLE_ATREMOVE]) {
        return BINDABLE_ATREMOVE;
      } /* end if */
      
      return BINDABLE_INVALID;
      
    case /* length = */ 9 :
      /* COLLATION */
      if (lexeme == bindable_lexeme_table[BINDABLE_COLLATION]) {
        return BINDABLE_COLLATION;
      } /* end if */
      
      return BINDABLE_INVALID;
      
  } /* end switch */
  return BINDABLE_INVALID;
  
} /* end m2c_bindable_for_lexeme */


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_bindable(value)
 * --------------------------------------------------------------------------
 * Returns  the interned string  with the lexeme  of the  bindable identifier
 * represented by value,  or NULL if value is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexeme_for_bindable (m2c_bindable_t value) {
  if (M2C_IS_VALID_BINDABLE(value)) {
    return bindable_lexeme_table[value];
  }
  else /* invalid value */ {
    return NULL;
  } /* end if */
} /* end m2c_lexeme_for_bindable */


/* END OF FILE */