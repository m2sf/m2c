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
  m2c_fifo_t list;
  m2c_dep_list_status_t last_status;
} m2c_dep_list_s;



/* --------------------------------------------------------------------------
 * procedure m2c_new_dep_list(module_id, list, status)
 * --------------------------------------------------------------------------
 * Creates  a new dependency list for module_id,  and passes it back in list,
 * or NULL on failure.  Passes the status of the operation back in status.
 * ----------------------------------------------------------------------- */

void m2c_new_dep_list
  (intstr_t module_id, m2c_dep_list_t *list, m2c_dep_list_status_t *status) {

  m2c_dep_list_t new_dep_list;
  
  if (module_id == NULL) {
    *status = M2C_DEP_LIST_STATUS_INVALID_REFERENCE;
    *list = NULL;
    return;
  } /* end if */
  
  new_list = malloc(sizeof(m2c_dep_list_s));

  if (new_list == NULL) {
    *status = M2C_DEP_LIST_STATUS_ALLOCATION_FAILED;
    *list = NULL;
    return;
  } /* end if */
  
  new_list->last_status = M2C_DEP_LIST_STATUS_SUCCESS;
  new_list->module_id = module_id;
  new_list->list = NULL;
  
  *status = M2C_DEP_LIST_STATUS_SUCCESS;
  *list = new_list;
  return;
} /* end m2c_new_dep_list */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_module(dep_list)
 * --------------------------------------------------------------------------
 * Returns  the identifier  of the module  whose dependencies  are stored  in
 * dependency list dep_list.  Returns NULL if dep_list is invalid.
 * ----------------------------------------------------------------------- */

intstr_t m2c_dep_list_module (m2c_dep_list_t dep_list) {

  if (dep_list == NULL) {
    return NULL;
  } /* end if */
  
  dep_list->last_status = M2C_DEP_LIST_STATUS_SUCCESS;
  return dep_list->module_id;
} /* end m2c_dep_list_module */


/* --------------------------------------------------------------------------
 * procedure m2c_dep_list_add_item(dep_list, module_id)
 * --------------------------------------------------------------------------
 * Adds dependency module_id to dependency list dep_list.
 * ----------------------------------------------------------------------- */

void m2c_dep_list_add_item (m2c_dep_list_t dep_list, intstr_t module_id) {

  if (dep_list == NULL) {
    return;
  } /* end if */
  
  if (dep_list->list == NULL) {
    dep_list->list = m2c_fifo_new_queue(module_id);
  }
  else {
    m2c_fifo_enqueue_unique(module_id);
  } /* end if */

  dep_list->last_status = M2C_DEP_LIST_STATUS_SUCCESS;
  return;
} /* end m2c_dep_list_add_item */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_item_count(dep_list)
 * --------------------------------------------------------------------------
 * Returns the number of dependencies stored in dependency list dep_list.
 * ----------------------------------------------------------------------- */

uint_t m2c_dep_list_item_count (m2c_dep_list_t dep_list) {

  if (dep_list == NULL) {
    return 0;
  } /* end if */
  
  dep_list->last_status = M2C_DEP_LIST_STATUS_SUCCESS;

  if (dep_list->list == NULL) {
    return 0;
  } /* end if */

  return m2c_fifo_entry_count(dep_list->list);
} /* end m2c_dep_list_item_count */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_item_at_index(dep_list, index)
 * --------------------------------------------------------------------------
 * Returns the dependency stored at index in dep_list,  or NULL on failure.
 * ----------------------------------------------------------------------- */

intstr_t m2c_dep_list_item_at_index (m2c_dep_list_t dep_list, uint_t index) {

/* TO DO */

} /* end m2c_dep_list_item_at_index */


/* --------------------------------------------------------------------------
 * function m2c_dep_list_last_status(dep_list)
 * --------------------------------------------------------------------------
 * Returns the status of the last operation for dep_list.
 * ----------------------------------------------------------------------- */

m2c_dep_list_status_t m2c_dep_list_last_status (m2c_dep_list_t dep_list) {

  if (dep_list == NULL) {
    return M2C_DEP_LIST_STATUS_INVALID_REFERENCE;
  } /* end if */
  
  return dep_list->last_status;
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