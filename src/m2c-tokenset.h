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
 * m2c-tokenset.h                                                            *
 *                                                                           *
 * Public interface for M2C tokenset type.                                   *
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

#ifndef M2C_TOKENSET_H
#define M2C_TOKENSET_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "m2c-token.h"
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_tokenset_t
 * --------------------------------------------------------------------------
 * Opaque pointer type representing a Modula-2 token-set object.
 * ----------------------------------------------------------------------- */

typedef struct m2c_tokenset_opaque_t *m2c_tokenset_t;


/* --------------------------------------------------------------------------
 * function m2c_new_tokenset_from_list(token_list)
 * --------------------------------------------------------------------------
 * Returns a newly allocated tokenset object that includes the tokens passed
 * as arguments of a non-empty variadic argument list.  The argument list
 * must be explicitly terminated with 0.
 * ----------------------------------------------------------------------- */

m2c_tokenset_t m2c_new_tokenset_from_list (m2c_token_t first_token, ...);


/* --------------------------------------------------------------------------
 * function m2c_new_tokenset_from_union(set_list)
 * --------------------------------------------------------------------------
 * Returns a newly allocated tokenset object that represents the set union of
 * the tokensets passed as arguments of a non-empty variadic argument list.
 * The argument list must be explicitly terminated with NULL.
 * ----------------------------------------------------------------------- */

m2c_tokenset_t m2c_new_tokenset_from_union (m2c_tokenset_t first_set, ...);


/* --------------------------------------------------------------------------
 * function m2c_tokenset_element(set, token)
 * --------------------------------------------------------------------------
 * Returns true if token is an element of set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_tokenset_element (m2c_tokenset_t set, m2c_token_t token);


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_element_count(set)
 * --------------------------------------------------------------------------
 * Returns the number of elements in set.
 * ----------------------------------------------------------------------- */

unsigned m2c_tokenset_element_count (m2c_tokenset_t set);


/* --------------------------------------------------------------------------
 * function m2c_tokenset_subset(set, subset)
 * --------------------------------------------------------------------------
 * Returns true if each element in subset is also in set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_tokenset_subset (m2c_tokenset_t set, m2c_tokenset_t subset);


/* --------------------------------------------------------------------------
 * function m2c_tokenset_disjunct(set1, set2)
 * --------------------------------------------------------------------------
 * Returns true if set1 and set2 have no common elements, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_tokenset_disjunct (m2c_tokenset_t set1, m2c_tokenset_t set2);


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_set(set_name, set)
 * --------------------------------------------------------------------------
 * Prints a human readable representation of set.
 * Format: set_name = { comma-separated list of tokens };
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_set (const char *set_name, m2c_tokenset_t set);


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_list(set)
 * --------------------------------------------------------------------------
 * Prints a human readable list of symbols in set.
 * Format: first, second, third, ..., secondToLast or last
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_list (m2c_tokenset_t set);


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_literal_struct(ident)
 * --------------------------------------------------------------------------
 * Prints a struct definition for tokenset literals.
 * Format: struct ident { <largest unsigned> s0, s1, ...; short unsigned n };
 * where type <largest unsigned> depends on platform and implementation file.
 *
 *  #  |  platform  | implementation file  |  type used
 * ----+------------+---------------------------------------------
 *  1  |   16-bit   |   tokenset.16bit.c   |   unsigned
 *  2  |   32-bit   |   tokenset.32bit.c   |   unsigned long
 *  3  |   64-bit   |   tokenset.64bit.c   |   unsigned long long
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_literal_struct (const char *ident);


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_literal(set)
 * --------------------------------------------------------------------------
 * Prints a sequence of hex values representing the bit pattern of set.
 * Format: { 0xHHHHHHHH, 0xHHHHHHHH, ..., count };
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_literal (m2c_tokenset_t set);


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_release(set)
 * --------------------------------------------------------------------------
 * Deallocates tokenset set.
 * ----------------------------------------------------------------------- */

void m2c_tokenset_release (m2c_tokenset_t set);


#endif /* M2C_TOKENSET_H */

/* END OF FILE */