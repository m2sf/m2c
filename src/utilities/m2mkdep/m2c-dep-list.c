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
 * m2c-dep-list.c                                                            *
 *                                                                           *
 * Implementation of module dependency list.                                 *
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

#include "m2c-dep-list.h"


/* --------------------------------------------------------------------------
 * hidden type m2c_dep_list_s
 * --------------------------------------------------------------------------
 * Module dependency list.
 * ----------------------------------------------------------------------- */

typedef struct {
  intstr_t module_id;
  uint entry_count;
  intstr_t entry[];
} m2c_dep_list_s;


/* --------------------------------------------------------------------------
 * status of last operation
 * ----------------------------------------------------------------------- */

static m2c_dep_list_status_t last_status = M2C_DEP_LIST_STATUS_SUCCESS;


/* --------------------------------------------------------------------------
 * function m2c_new_dep_list(module_id, item_list)
 * --------------------------------------------------------------------------
 * Creates a new dependency list for module_id from item_list and returns it,
 * or NULL on failure.  The status of the operation is stored in last_status.
 * ----------------------------------------------------------------------- */

m2c_dep_list_t m2c_new_dep_list (intstr_t module_id, m2c_fifo_t item_list) {

  m2c_dep_list_t new_dep_list;
  uint_t index;
  
  if (module_id == NULL) {
    last_status = M2C_DEP_LIST_STATUS_INVALID_REFERENCE;
    return NULL;
  } /* end if */
  
  entry_count = m2c_fifo_entry_count(item_list);

  new_list =
    malloc(sizeof(m2c_dep_list_s) + entry_count * sizeof(intstr_t));

  if (new_list == NULL) {
    last_status = M2C_DEP_LIST_STATUS_ALLOCATION_FAILED;
    return NULL;
  } /* end if */
  
  new_list->module_id = module_id;
  
  index = 0;
  while (index < entry_count) {
    new_list->entry[index] = m2c_fifo_dequeue(item_list);
    index++;
  } /* end while */

  new_list->entry_count = entry_count;
  
  last_status = M2C_DEP_LIST_STATUS_SUCCESS;
  return new_list;
} /* end m2c_new_dep_list */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_module(dep_list)
 * --------------------------------------------------------------------------
 * Returns  the identifier  of the module  whose dependencies  are stored  in
 * dependency list dep_list.  Returns NULL if dep_list is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_dep_list_module (m2c_dep_list_t dep_list) {

  if (dep_list == NULL) {
    last_status = M2C_DEP_LIST_STATUS_INVALID_REFERENCE;
    return NULL;
  }
  else {
    last_status = M2C_DEP_LIST_STATUS_SUCCESS;
    return dep_list->module_id;
  } /* end if */
} /* end m2c_dep_list_module */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_item_count(dep_list)
 * --------------------------------------------------------------------------
 * Returns the number of dependencies stored in dependency list dep_list.
 * ----------------------------------------------------------------------- */

uint_t m2c_dep_list_item_count (m2c_dep_list_t dep_list) {

  if (dep_list == NULL) {
    last_status = M2C_DEP_LIST_STATUS_INVALID_REFERENCE;
    return 0;
  }
  else {
    last_status = M2C_DEP_LIST_STATUS_SUCCESS;
    return dep_list->entry_count;
  } /* end if */
} /* end m2c_dep_list_item_count */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_item_at_index(dep_list, index)
 * --------------------------------------------------------------------------
 * Returns the dependency stored at index in dep_list,  or NULL on failure.
 * ----------------------------------------------------------------------- */

intstr_t m2c_dep_list_item_at_index (m2c_dep_list_t dep_list, uint_t index) {

  if (dep_list == NULL) {
    last_status = M2C_DEP_LIST_STATUS_INVALID_REFERENCE;
    return NULL;
  }
  else (index >= dep_list->entry_count) {
    last_status = M2C_DEP_LIST_STATUS_INVALID_INDEX;
    return NULL;
  }
  else {
    last_status = M2C_DEP_LIST_STATUS_SUCCESS;
    return new_list->entry[index];
  } /* end if */
} /* end m2c_dep_list_item_at_index */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_last_status()
 * --------------------------------------------------------------------------
 * Returns the status of the last operation.
 * ----------------------------------------------------------------------- */

m2c_dep_list_status_t m2c_dep_list_last_status (void) {

  return last_status;
} /* end m2c_dep_list_last_status */


/* --------------------------------------------------------------------------
 * procedure m2c_dep_list_release(dep_list)
 * --------------------------------------------------------------------------
 * Deallocates dep_list and passes NULL in dep_list.
 * ----------------------------------------------------------------------- */

void m2c_dep_list_release (m2c_dep_list_t *dep_list) {

/* TO DO */

} /* end m2c_dep_list_release */

/* END OF FILE */