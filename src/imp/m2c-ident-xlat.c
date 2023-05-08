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
#include "snake-case-conv.h"
#include "c-reswords.h"
#include "base36.h"

#include <stdio.h> /* sprintf */
#include <stdlib.h> /* NULL, malloc */


/* --------------------------------------------------------------------------
 * private function convert_to_upper(str)
 * --------------------------------------------------------------------------
 * Converts all lowercase characters in str to uppercase in situ. 
 * ----------------------------------------------------------------------- */

static void convert_to_upper (char *str) {
  
  uint_t index;
  char ch;

  index = 0;
  ch = str[0];

  while (ch != ASCII_NUL) {
    if (IS_LOWER(ch)) {
      str[index] = ch - 32;
    }
    index++;
    ch = str[index];
  } /* end while*/
} /* convert_to_upper */


/* --------------------------------------------------------------------------
 * function m2c_ident_xlat_import_guard(module_id)
 * --------------------------------------------------------------------------
 * Returns an import guard C macro identifier for module_id. 
 * ----------------------------------------------------------------------- */

const char* m2c_ident_xlat_import_guard (intstr_t module_id) {
  
  char *ll_module_id, *import_guard;
  uint_t len;

  ll_module_id = snake_case_for_ident(intstr_char_ptr(module_id));
  
  len = strlen(ll_module_id) + 2;
  import_guard = malloc(len * sizeof(char) + 1);

  if (import_guard == NULL) {
    return NULL;
  }; /* end if */
  
  sprintf(import_guard, "%s _H", ll_module_id);
  convert_to_upper(import_guard);
  
  return import_guard;
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
  
  char *ll_module_id, *ll_enum_id, *ll_ident, *xlat;
  uint_t len;

  switch (kind) {

    case M2C_IDENT_XLAT_KIND_CONST :

    /* enumerated value */
      if (enum_id != NULL) {
        ll_module_id = snake_case_for_ident(intstr_char_ptr(module_id));
        ll_enum_id = snake_case_for_ident(intstr_char_ptr(enum_id));
        ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_module_id)+strlen(ll_enum_id)+strlen(ll_ident)+3;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s__%s_%s", ll_module_id, ll_enum_id, ll_ident);
      }
    /* other constant */
      else {
        ll_module_id = snake_case_for_ident(intstr_char_ptr(module_id));
        ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_module_id) + strlen(ll_ident) + 2;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s__%s", ll_ident);
      }; /* end if */
      
      /* ALL-CAPS */
      convert_to_upper(xlat);
      break;

    /* type */
    case M2C_IDENT_XLAT_KIND_TYPE  :
      ll_module_id = snake_case_for_ident(intstr_char_ptr(module_id));
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_module_id) + strlen(ll_ident) + 3;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s__%s_t", ll_ident);
      break;

    /* variable or function */
    case M2C_IDENT_XLAT_KIND_VAR   :
    case M2C_IDENT_XLAT_KIND_FUNC  :
      ll_module_id = snake_case_for_ident(intstr_char_ptr(module_id));
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_module_id) + strlen(ll_ident) + 2;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s__%s", ll_module_id, ll_ident);
      break;

    /* procedure */
    case M2C_IDENT_XLAT_KIND_PROC  :
      ll_module_id = snake_case_for_ident(intstr_char_ptr(module_id));
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_module_id) + strlen(ll_ident) + 3;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s__do_%s", ll_module_id, ll_ident);
    
  }; /* end switch */
  
  return xlat;
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
  
  char *ll_enum_id, *ll_ident, *xlat;
  uint_t len;

  switch (kind) {

    case M2C_IDENT_XLAT_KIND_CONST :

    /* enumerated value */
      if (enum_id != NULL) {
        ll_enum_id = snake_case_for_ident(intstr_char_ptr(enum_id));
        ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_enum_id) + strlen(ll_ident) + 1;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s_%s", ll_enum_id, ll_ident);
      }
    /* other constant */
      else {
        ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_ident);
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s", ll_ident);
      }; /* end if */

      /* ALL-CAPS */
      convert_to_upper(xlat);
      break;

    /* type */
    case M2C_IDENT_XLAT_KIND_TYPE  :
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident) + 2;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s_t", ll_ident);
      break;

    /* variable or function */
    case M2C_IDENT_XLAT_KIND_VAR   :
    case M2C_IDENT_XLAT_KIND_FUNC  :
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident);
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s", ll_ident);
      if (is_c_resword(xlat)) {
        /* convert first char to upper */
        xlat[0] = xlat[0] - 32;
      } /* end if */
      break;

    /* procedure */
    case M2C_IDENT_XLAT_KIND_PROC  :
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident) + 3;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "do_%s", ll_ident);
    
  }; /* end switch */
  
  return xlat;
}; /* end m2c_ident_xlat_for_hidden_name */


/* --------------------------------------------------------------------------
 * private function get_base36_hash_str_for_ident(ident, hash_str)
 * --------------------------------------------------------------------------
 * Calculates a base-36 hash string from ident, passes it back in hash_str.
 * ----------------------------------------------------------------------- */

void get_base36_hash_str_for_ident (const char *ident, char *hash_str) {
  
  /* TO DO */

}; /* end get_base36_hash_str_for_ident */


/* --------------------------------------------------------------------------
 * function m2c_ident_xlat_for_local_name(type, proc_id, enum_id, ident)
 * --------------------------------------------------------------------------
 * Returns a local C identifier from proc_id for ident.
 * ----------------------------------------------------------------------- */

const char* m2c_ident_xlat_for_local_name
  (m2c_ident_xlat_kind_t kind,
   intstr_t proc_id,   /* required */
   intstr_t enum_id,   /* may be NULL */
   intstr_t ident)     /* required */ {
  
  char *ll_enum_id, *ll_ident, *xlat;
  base36_str_t suffix;
  uint_t len;
  
  get_base36_hash_str_for_ident(intstr_char_ptr(ident), &suffix);

  switch (kind) {

    case M2C_IDENT_XLAT_KIND_CONST :

    /* enumerated value */
      if (enum_id != NULL) {
        ll_enum_id = snake_case_for_ident(intstr_char_ptr(enum_id));
        ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_enum_id) + strlen(ll_ident) + strlen(suffix) + 4;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s_%s__0%s", ll_enum_id, ll_ident, suffix);
      }
    /* other constant */
      else {
        ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_ident) + strlen(suffix) + 3;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s__0%s", ll_ident, suffix);
      }; /* end if */

      /* ALL-CAPS */
      convert_to_upper(xlat);
      break;

    /* type */
    case M2C_IDENT_XLAT_KIND_TYPE  :
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident) + strlen(suffix) + 5;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s_t__0%s", ll_ident, suffix);
      break;

    /* variable */
    case M2C_IDENT_XLAT_KIND_VAR   :
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident);
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s", ll_ident);
      if (is_c_resword(xlat)) {
        /* convert first char to upper */
        xlat[0] = xlat[0] - 32;
      } /* end if */
      break;
    
    /* function */
    case M2C_IDENT_XLAT_KIND_FUNC  :
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident) + strlen(suffix) + 3;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s__0%s", ll_ident, suffix);
      break;
      
    /* procedure */
    case M2C_IDENT_XLAT_KIND_PROC  :
      ll_ident = snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident) + strlen(suffix) + 6;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "do_%s__0%s", ll_ident, suffix);
    
  }; /* end switch */
  
  return xlat;  
}; /* end m2c_ident_xlat_for_local_name */


#endif /* M2C_IDENT_XLAT_H */

/* END OF FILE */