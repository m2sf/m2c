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
 * m2c-dep-list.h                                                            *
 *                                                                           *
 * Public interface of module dependency list.                               *
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


#ifndef M2C_DEP_LIST_H
#define M2C_DEP_LIST_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "interned-strings.h"
#include "m2c-common.h"


/* --------------------------------------------------------------------------
 * opaque type m2c_dep_list_t
 * --------------------------------------------------------------------------
 * Module dependency list.
 * ----------------------------------------------------------------------- */

typedef m2c_dep_list_s *m2c_dep_list_t;


/* --------------------------------------------------------------------------
 * status
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_DEP_LIST_STATUS_SUCCESS,
  M2C_DEP_LIST_STATUS_INVALID_REFERENCE,
  M2C_DEP_LIST_STATUS_ALLOCATION_FAILED
  M2C_DEP_LIST_STATUS_INVALID_INDEX,
} m2c_dep_list_status_t;


/* --------------------------------------------------------------------------
 * function m2c_new_dep_list(module_id, item_list)
 * --------------------------------------------------------------------------
 * Creates a new dependency list for module_id from item_list and returns it,
 * or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_dep_list_t m2c_new_dep_list (intstr_t module_id, m2c_fifo_t item_list);


/* --------------------------------------------------------------------------
 * function m2c_dep_list_module(dep_list)
 * --------------------------------------------------------------------------
 * Returns  the identifier  of the module  whose dependencies  are stored  in
 * dependency list dep_list.  Returns NULL if dep_list is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_dep_list_module (m2c_dep_list_t dep_list);


/* --------------------------------------------------------------------------
 * function m2c_dep_list_item_count(dep_list)
 * --------------------------------------------------------------------------
 * Returns the number of dependencies stored in dependency list dep_list.
 * ----------------------------------------------------------------------- */

uint_t m2c_dep_list_item_count (m2c_dep_list_t dep_list);


/* --------------------------------------------------------------------------
 * function m2c_dep_list_item_at_index(dep_list, index)
 * --------------------------------------------------------------------------
 * Returns the dependency stored at index in dep_list,  or NULL on failure.
 * ----------------------------------------------------------------------- */

intstr_t m2c_dep_list_item_at_index (m2c_dep_list_t dep_list, uint_t index);


/* --------------------------------------------------------------------------
 * function m2c_dep_list_last_status()
 * --------------------------------------------------------------------------
 * Returns the status of the last operation.
 * ----------------------------------------------------------------------- */

m2c_dep_list_status_t m2c_dep_list_last_status (void);


/* --------------------------------------------------------------------------
 * procedure m2c_dep_list_dispose(dep_list)
 * --------------------------------------------------------------------------
 * Deallocates dep_list and passes NULL in dep_list.
 * ----------------------------------------------------------------------- */

void m2c_dep_list_dispose (m2c_dep_list_t *dep_list);


#endif /* M2C_DEP_LIST_H */

/* END OF FILE */