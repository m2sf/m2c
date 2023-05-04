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
 * m2c-ident-xlat.c                                                          *
 *                                                                           *
 * Implementation of identifier translation module.                          *
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

#include "m2c-ident-xlat.h"
#include "m2c-common.h"


/* --------------------------------------------------------------------------
 * function match_lowercase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident  starting at index  to a lowercase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  lowercaseWord := LowercaseLetter (LowercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static uint_t match_lowercase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* LowercaseLetter */
  if (IS_LOWER(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not lowercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (LowercaseLetter | Digit)* */
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_lowercase_word */


/* --------------------------------------------------------------------------
 * function match_titlecase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident  starting at index  to a titlecase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  titlecaseWord := UppercaseLetter (LowercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static uint_t match_titlecase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* UppercaseLetter */
  if (IS_UPPER(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not uppercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (LowercaseLetter | Digit)* */
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_titlecase_word */


/* --------------------------------------------------------------------------
 * function match_uppercase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident starting at index  to an uppercase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  uppercaseWord :=
 *          UppercaseLetter (UppercaseLetter !LowercaseLetter | Digit)*;
 *
 * !!! An uppercase letter before a lowercase letter must NOT be consumed !!!
 * !!! because it is the first character of a following titlecase word.   !!!
 * ----------------------------------------------------------------------- */

static uint_t match_uppercase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* UppercaseLetter */
  if (IS_UPPER(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not uppercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (UppercaseLetter !LowercaseLetter | Digit)* */
  while (((IS_UPPER(ch) && (NOT(IS_LOWER(ident[index+1])))) || IS_DIGIT(ch))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_uppercase_word */


/* --------------------------------------------------------------------------
 * function m2c_ident_xlat_import_guard(module_id)
 * --------------------------------------------------------------------------
 * Returns an import guard C macro identifier for module_id. 
 * ----------------------------------------------------------------------- */

const char* m2c_ident_xlat_import_guard (intstr_t module_id) {
  
  /* TO DO */
  
}; /* end m2c_ident_xlat_import_guard */


/* --------------------------------------------------------------------------
 * function m2c_ident_xlat_for_exported_name(type, module_id, enum_id, ident)
 * --------------------------------------------------------------------------
 * Returns a fully qualified C identifier for module_id and ident. 
 * ----------------------------------------------------------------------- */

const char* m2c_ident_xlat_for_exported_name
  (m2c_ident_xlat_kind_t kind,
   intstr_t module_id, /* required */
   intstr_t enum_id,   /* may be NULL */
   intstr_t ident)     /* required */ {
  
  /* TO DO */

}; /* end m2c_ident_xlat_for_exported_name */


/* --------------------------------------------------------------------------
 * function m2c_ident_xlat_for_hidden_name(type, enum_id, ident)
 * --------------------------------------------------------------------------
 * Returns a file level C identifier for ident. 
 * ----------------------------------------------------------------------- */

const char* m2c_ident_xlat_for_hidden_name
  (m2c_ident_xlat_kind_t kind,
   intstr_t enum_id,   /* may be NULL */
   intstr_t ident)     /* required */ {
  
  /* TO DO */
  
}; /* end m2c_ident_xlat_for_hidden_name */


/* --------------------------------------------------------------------------
 * function m2c_ident_xlat_for_local_name(type, proc_id, enum_id, ident)
 * --------------------------------------------------------------------------
 * Returns a local C identifier from proc_id for ident.
 * ----------------------------------------------------------------------- */

const char* m2c_ident_xlat_for_local_name
  (m2c_ident_xlat_kind_t kind,
   intstr_t proc_id,   /* required */
   intstr_t enum_id,   /* may be NULL */
   intstr_t ident)    /* required */ {
  
  /* TO DO */
  
}; /* end m2c_ident_xlat_for_local_name */


#endif /* M2C_IDENT_XLAT_H */

/* END OF FILE */