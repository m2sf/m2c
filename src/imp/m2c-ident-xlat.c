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
#include "lowline-ident-strings.h"

#include <stdio.h> /* sprintf */
#include <stdlib.h> /* NULL, malloc */


/* --------------------------------------------------------------------------
 * function m2c_ident_xlat_import_guard(module_id)
 * --------------------------------------------------------------------------
 * Returns an import guard C macro identifier for module_id. 
 * ----------------------------------------------------------------------- */

const char* m2c_ident_xlat_import_guard (intstr_t module_id) {
  
  char *ll_module_id, *import_guard;

  ll_module_id = llid_macro_case_for_ident(intstr_char_ptr(module_id));
  
  len = strlen(ll_module_id) + 2;
  import_guard = malloc(len * sizeof(char) + 1);

  if (import_guard == NULL) {
    return NULL;
  }; /* end if */
  
  sprintf(import_guard, "%s _H", ll_module_id);
  
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

  switch (kind) {

    case M2C_IDENT_XLAT_KIND_CONST :

    /* enumerated value */
      if (enum_id != NULL) {
        ll_module_id = llid_macro_case_for_ident(intstr_char_ptr(module_id));
        ll_enum_id = llid_macro_case_for_ident(intstr_char_ptr(enum_id));
        ll_ident = llid_macro_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_module_id)+strlen(ll_enum_id)+strlen(ll_ident)+3;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s__%s_%s", ll_module_id, ll_enum_id, ll_ident);
      }
    /* other constant */
      else {
        ll_module_id = llid_macro_case_for_ident(intstr_char_ptr(module_id));
        ll_ident = llid_macro_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_module_id) + strlen(ll_ident) + 2;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s__%s", ll_ident);
      }; /* end if */
      break;

    /* type */
    case M2C_IDENT_XLAT_KIND_TYPE  :
      ll_module_id = llid_snake_case_for_ident(intstr_char_ptr(module_id));
      ll_ident = llid_snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_module_id) + strlen(ll_ident) + 3;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s__%s_t", ll_ident);
      break;

    /* variable or function */
    case M2C_IDENT_XLAT_KIND_VAR   :
    case M2C_IDENT_XLAT_KIND_FUNC  :
      ll_module_id = llid_snake_case_for_ident(intstr_char_ptr(module_id));
      ll_ident = llid_snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_module_id) + strlen(ll_ident) + 2;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s__%s", ll_module_id, ll_ident);
      break;

    /* procedure */
    case M2C_IDENT_XLAT_KIND_PROC  :
      ll_module_id = llid_snake_case_for_ident(intstr_char_ptr(module_id));
      ll_ident = llid_snake_case_for_ident(intstr_char_ptr(ident));
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

  switch (kind) {

    case M2C_IDENT_XLAT_KIND_CONST :

    /* enumerated value */
      if (enum_id != NULL) {
        ll_enum_id = llid_macro_case_for_ident(intstr_char_ptr(enum_id));
        ll_ident = llid_macro_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_enum_id) + strlen(ll_ident) + 1;
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s_%s", ll_enum_id, ll_ident);
      }
    /* other constant */
      else {
        ll_ident = llid_macro_case_for_ident(intstr_char_ptr(ident));
        len = strlen(ll_ident);
        xlat = malloc(len * sizeof(char) + 1);
        sprintf(xlat, "%s", ll_ident);
      }; /* end if */
      break;

    /* type */
    case M2C_IDENT_XLAT_KIND_TYPE  :
      ll_ident = llid_snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident) + 2;
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s_t", ll_ident);
      break;

    /* variable or function */
    case M2C_IDENT_XLAT_KIND_VAR   :
    case M2C_IDENT_XLAT_KIND_FUNC  :
      ll_ident = llid_snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident);
      xlat = malloc(len * sizeof(char) + 1);
      sprintf(xlat, "%s", ll_ident);
      break;

    /* procedure */
    case M2C_IDENT_XLAT_KIND_PROC  :
      ll_ident = llid_snake_case_for_ident(intstr_char_ptr(ident));
      len = strlen(ll_ident);
      xlat = malloc(len * sizeof(char) + 3);
      sprintf(xlat, "do_%s", ll_ident);
    
  }; /* end switch */
  
  return xlat;
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