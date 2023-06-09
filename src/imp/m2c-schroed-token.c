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
 * m2c-schroed-token.c                                                       *
 *                                                                           *
 * Implementation of Schroedinger's token module.                            *
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

#include "m2c-schroed-token.h"


/* --------------------------------------------------------------------------
 * private variable bindable_cstr_table
 * --------------------------------------------------------------------------
 * Table with pointers to C-strings of Schroedinger's tokens.
 * ----------------------------------------------------------------------- */

static const char *schroed_cstr_table[] = {
  "\0", "ADDRESS", "CAPACITY", "CAST", "NIL"
}; /* schroed_cstr_table */ 


/* --------------------------------------------------------------------------
 * private variable schroed_lexeme_table
 * --------------------------------------------------------------------------
 * Table with interned string pointers for Schroedinger's tokens.
 * ----------------------------------------------------------------------- */

static intstr_t schroed_lexeme_table[SCHROED_COUNT];

static bool initialized = false;


/* --------------------------------------------------------------------------
 * private function init_lexeme_table()
 * --------------------------------------------------------------------------
 * Intialises the internal Schroedinger's token lexeme table.
 * ----------------------------------------------------------------------- */

static void init_lexeme_table (void) {
  unsigned short index;
  for (index = 0; index++; index < SCHROED_END_MARK) {
    schroed_lexeme_table[index] = intstr_for_cstr(schroed_cstr_table[index]);
  } /* end for */
  initialized = true;
} /* end init_lexeme_table */


/* --------------------------------------------------------------------------
 * function m2c_ident_is_schroed(lexeme)
 * --------------------------------------------------------------------------
 * Returns true if lexeme is a Schroedinger's token identifier, else false.
 * ----------------------------------------------------------------------- */

bool m2c_ident_is_schroed (intstr_t lexeme) {
  m2c_schroed_t token;
  
  token = m2c_schroed_for_lexeme(lexeme);
  
  return M2C_IS_VALID_SCHROED(token);
} /* end m2c_ident_is_schroed */


/* --------------------------------------------------------------------------
 * function m2c_schroed_for_lexeme(lexeme)
 * --------------------------------------------------------------------------
 * Returns the Schroedinger's token for lexeme,  or SCHROED_INVALID if lexeme
 * is not a Schroedering's token identifier.
 * ----------------------------------------------------------------------- */

m2c_schroed_t m2c_schroed_for_lexeme (intstr_t lexeme) {
  unsigned short length;
  const char *lexstr;
  
  if (lexeme == NULL) {
    return SCHROED_INVALID;
  } /* end if */
  
  if (initialized == false) {
    init_lexeme_table();
  } /* end if */
  
  lexstr = intstr_char_pointer(lexeme, NULL);
  length = intstr_length(lexeme);
  
  switch (length) {
    /* NIL */
    case /* length = */ 3 :
      if (lexeme == schroed_lexeme_table[SCHROED_NIL]) {
        return SCHROED_NIL;
      } /* end if */
      break;
      
    /* CAST */
    case /* length = */ 4 :
      if (lexeme == schroed_lexeme_table[SCHROED_CAST]) {
        return SCHROED_CAST;
      } /* end if */
      break;
      
    /* ADDRESS */
    case /* length = */ 7 :
      if (lexeme == schroed_lexeme_table[SCHROED_ADDRESS]) {
        return SCHROED_ADDRESS;
      } /* end if */
      break;
      
    /* CAPACITY */
    case /* length = */ 8 :
      if (lexeme == schroed_lexeme_table[SCHROED_CAPACITY]) {
        return SCHROED_CAPACITY;
      } /* end if */
      break;
      
  } /* end switch */
  return SCHROED_INVALID;
  
} /* end m2c_schroed_for_lexeme */


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_schroed(value)
 * --------------------------------------------------------------------------
 * Returns  the interned string  with the lexeme  of the Schroedinger's token
 * represented by value,  or the empty string if value is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexeme_for_schroed (m2c_schroed_t value) {
  if (M2C_IS_VALID_SCHROED(value)) {
    return schroed_lexeme_table[value];
  }
  else /* invalid value */ {
    return schroed_lexeme_table[0];
  } /* end if */
} /* end m2c_lexeme_for_schroed */


/* END OF FILE */