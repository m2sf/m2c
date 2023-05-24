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
 * m2c-first-sets.c                                                          *
 *                                                                           *
 * Implementation of FIRST sets module.                                      *
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

#include "m2c-tokenset.h"
#include "m2c-productions.h"

#include <stddef.h> /* NULL */
#include <stdint.h> /* uint8_t */


/* --------------------------------------------------------------------------
 * FIRST set table, pruned
 * --------------------------------------------------------------------------
 * Table of first sets with cardinality > 1 and without duplicates.
 * ----------------------------------------------------------------------- */

#define DATA(_prod, _set_literal) _set_literal,

static const m2c_tokenset_t *first_set[] = {
  NULL, /* sentinel for pruned entries */
  #include "m2c-first-set-literals.h"
}; /* end first_set */

#undef DATA


/* --------------------------------------------------------------------------
 * FIRST set lookup table
 * --------------------------------------------------------------------------
 * Table to map productions to pruned first set table indices.
 * ----------------------------------------------------------------------- */

typedef uint8_t table_index_t;   /* unsigned char if uint8_t is missing */

#define DATA(_prod, _index) _index,

static const table_index_t index_table[] = {
  #include "m2c_first-set-lookup.h"
}; /* end index_table */

#undef DATA


/* --------------------------------------------------------------------------
 * function FIRST(production)
 * --------------------------------------------------------------------------
 * Returns the FIRST set of p if p is valid and |FIRST(p)| > 1, else NULL.
 * ----------------------------------------------------------------------- */

const m2c_tokenset_t FIRST (m2c_production_t p) {
 
  if (IS_VALID_PRODUCTION(p)) {
    return first_set[index_table[p]];
  }
  else /* invalid production */ {
    return NULL;
  } /* end if */
} /* end FIRST */

/* END OF FILE */