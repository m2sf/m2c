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
 * snake-case-conv.h                                                         *
 *                                                                           *
 * Public interface of identifier snake-case translation dictionary.         *
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

#ifndef SNAKE_CASE_CONV_H
#define SNAKE_CASE_CONV_H


/* --------------------------------------------------------------------------
 * type snake_status_t
 * --------------------------------------------------------------------------
 * Status codes for snake-case translation dictionary operations.
 * ----------------------------------------------------------------------- */

typedef enum {
  SNAKE_STATUS_SUCCESS,
  SNAKE_STATUS_NOT_INITIALIZED,
  SNAKE_STATUS_ALREADY_INITIALIZED,
  SNAKE_STATUS_INVALID_REFERENCE,
  SNAKE_STATUS_ALLOCATION_FAILED,
  SNAKE_STATUS_SIZE_LIMIT_EXCEEDED
} snake_status_t;


/* --------------------------------------------------------------------------
 * procedure snake_init_dictionary()
 * --------------------------------------------------------------------------
 * Allocates and initialises the snake-case translation dictionary. 
 * ----------------------------------------------------------------------- */

void snake_init_dictionary (unsigned size, snake_status_t *status);


/* --------------------------------------------------------------------------
 * function snake_case_for_ident(ident)
 * --------------------------------------------------------------------------
 * Returns the snake-case translation of ident,  or NULL if malformed.
 * ----------------------------------------------------------------------- */

const char* snake_case_for_ident (const char* ident);


/* --------------------------------------------------------------------------
 * function snake_entry_count()
 * --------------------------------------------------------------------------
 * Returns the number of identifiers stored in the dictionary.
 * ----------------------------------------------------------------------- */

uint_t snake_entry_count (void);


/* --------------------------------------------------------------------------
 * procedure snake_retain_entry(ident)
 * --------------------------------------------------------------------------
 * Prevents the dictionary entry for ident from deallocation.
 * ----------------------------------------------------------------------- */

void snake_retain_entry (const char *ident);


/* --------------------------------------------------------------------------
 * procedure snake_release_entry()
 * --------------------------------------------------------------------------
 * Cancels an  outstanding retain  for ident,  or if there are no outstanding
 * retains, deallocates the entry for ident.
 * ----------------------------------------------------------------------- */

void snake_release_entry (const char *ident);


/* --------------------------------------------------------------------------
 * function snake_last_status()
 * --------------------------------------------------------------------------
 * Returns the status of the last operation. 
 * ----------------------------------------------------------------------- */

snake_status_t snake_last_status (void);


/* --------------------------------------------------------------------------
 * procedure snake_dealloc_dictionary()
 * --------------------------------------------------------------------------
 * Deallocates the dictionary. 
 * ----------------------------------------------------------------------- */

void snake_dealloc_dictionary (snake_status_t *status);


#endif /* SNAKE_CASE_CONV_H */

/* END OF FILE */