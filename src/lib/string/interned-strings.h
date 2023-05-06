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
  * interned-strings.h                                                        *
  *                                                                           *
  * Public interface of interned strings library.                             *
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

#ifndef INTSTR_H
#define INTSTR_H

#include "m2-common.h"


/* --------------------------------------------------------------------------
 * Dynamic string length limit
 * ----------------------------------------------------------------------- */

#define INTSTR_SIZE_LIMIT 2000


/* --------------------------------------------------------------------------
 * opaque type intstr_t
 * --------------------------------------------------------------------------
 * Opaque pointer type representing an interned string.
 * ----------------------------------------------------------------------- */

typedef struct intstr_struct_t *intstr_t;


/* --------------------------------------------------------------------------
 * type intstr_status_t
 * --------------------------------------------------------------------------
 * Status codes for operations on type intstr_t.
 * ----------------------------------------------------------------------- */

typedef enum {
  INTSTR_STATUS_SUCCESS,
  INTSTR_STATUS_NOT_INITIALIZED,
  INTSTR_STATUS_ALREADY_INITIALIZED,
  INTSTR_STATUS_INVALID_REFERENCE,
  INTSTR_STATUS_INVALID_INDICES,
  INTSTR_STATUS_ALLOCATION_FAILED,
  INTSTR_STATUS_SIZE_LIMIT_EXCEEDED,
} intstr_status_t;


/* --------------------------------------------------------------------------
 * procedure intstr_init_repo(size, status)
 * --------------------------------------------------------------------------
 * Allocates and initialises global string repository.  Parameter size deter-
 * mines the  number of buckets  of the repository's internal hash table.  If
 * size is zero, value STRING_REPO_DEFAULT_BUCKET_COUNT is used.
 *
 * pre-conditions:
 * o  global repository must be uninitialised upon entry
 * o  parameter size may be zero upon entry
 * o  parameter status may be NULL upon entry
 *
 * post-conditions:
 * o  global string repository is allocated and intialised.
 * o  M2C_STRING_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if repository has already been initialised upon entry,
 *    no operation is carried out and M2C_STRING_STATUS_ALREADY_INITIALIZED
 *    is passed back in status unless status is NULL
 * o  if repository allocation failed, M2C_STRING_STATUS_ALLOCATION_FAILED
 *    is passed back in status unless status is NULL
 * ----------------------------------------------------------------------- */

void intstr_init_repo (uint_t size, intstr_status_t *status);


/* --------------------------------------------------------------------------
 * function intstr_for_cstr(str, status)
 * --------------------------------------------------------------------------
 * Returns an interned string object for str which must be a pointer to a NUL
 * terminated character string.  
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  if a string object for str is present in the internal repository,
 *    that string object is retrieved, retained and returned.
 * o  if no string object for str is present in the repository,
 *    a new string object with a copy of str is created, stored and returned.
 * o  INTSTR_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if str is NULL upon entry, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_INVALID_REFERENCE is
 *    passed back in status unless status is NULL
 * o  if string object allocation failed, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_ALLOCATION_FAILED is
 *    passed back in status unless status is NULL
 * ----------------------------------------------------------------------- */

intstr_t intstr_for_cstr (const char *str, intstr_status_t *status);


/* --------------------------------------------------------------------------
 * function intstr_for_slice(str, offset, length, status)
 * --------------------------------------------------------------------------
 * Returns an interned string object for a given slice of str.  Parameter str
 * must be a pointer to a NUL terminated character string.  The position of
 * the first character of the slice is given by parameter offset and the
 * length of the slice is given by parameter length.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 * o  slice must be within range of str (NOT GUARDED),
 * o  slice must not contain any characters with code points < 32 or > 126 
 * 
 *
 * post-conditions:
 * o  if a string object for the slice is present in the internal repository,
 *    that string object is retrieved, retained and returned.
 * o  if no string object for the slice is present in the repository, a new
 *    string object with a copy of the slice is created, stored and returned.
 * o  INTSTR_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if precondition #1 is not met, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_INVALID_REFERENCE is
 *    passed back in status, unless status is NULL
 * o  if precondition #3 is not met, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_INVALID_INDICES is
 *    passed back in status, unless status is NULL
 * o  if string object allocation failed, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_ALLOCATION_FAILED is
 *    passed back in status unless status is NULL
 * ----------------------------------------------------------------------- */

intstr_t intstr_for_slice
  (const char *str, uint_t offset, uint_t length, intstr_status_t *status);


/* --------------------------------------------------------------------------
 * function intstr_for_concatenation(str, append_str, status)
 * --------------------------------------------------------------------------
 * Returns an interned string object for the character string  resulting from
 * concatenation of str  and  append_str.  Parameters str and append_str must
 * be pointers to NUL terminated character strings.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 * o  parameter append_str must not be NULL upon entry
 *
 * post-conditions:
 * o  if a string object for the resulting concatenation string is present in
 *    the internal repository, that string object is retrieved, retained and
 *    returned.
 * o  if no string object for the resulting concatenation is present in the
 *    repository, a new string object with a copy of the concatenation is
 *    created, stored and returned.
 * o  INTSTR_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if any of str or append_str is NULL upon entry, no operation is
 *    carried out, NULL is returned and INTSTR_STATUS_INVALID_REFERENCE
 *    is passed back in status, unless NULL
 * o  if no dynamic string object could be allocated, NULL is returned,
 *    and INTSTR_STATUS_ALLOCATION_FAILED is passed back in status,
 *    unless NULL
 * ----------------------------------------------------------------------- */

intstr_t intstr_for_concatenation
  (const char *str, const char *append_str, intstr_status_t *status);


/* --------------------------------------------------------------------------
 * function intstr_length(str)
 * --------------------------------------------------------------------------
 * Returns the length of the character string of str.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  the length of str's character string is returned
 *
 * error-conditions:
 * o  if str is NULL upon entry, zero is returned
 * ----------------------------------------------------------------------- */

uint_t intstr_length (intstr_t str);


/* --------------------------------------------------------------------------
 * function intstr_char_ptr(str)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to the character string of str.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  an immutable pointer to str's character string is returned
 *
 * error-conditions:
 * o  if str is NULL upon entry, NULL is returned
 * ----------------------------------------------------------------------- */

const char *intstr_char_ptr (intstr_t str);


/* --------------------------------------------------------------------------
 * function intstr_count()
 * --------------------------------------------------------------------------
 * Returns the number of interned strings stored in the string repository.
 *
 * pre-conditions:
 * o  none
 *
 * post-conditions:
 * o  none
 *
 * error-conditions:
 * o  none
 * ----------------------------------------------------------------------- */

uint_t intstr_count (void);


/* --------------------------------------------------------------------------
 * function intstr_retain(str)
 * --------------------------------------------------------------------------
 * Prevents str from deallocation.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  str's reference count is incremented.
 *
 * error-conditions:
 * o  if str is not NULL upon entry, no operation is carried out
 * ----------------------------------------------------------------------- */

void intstr_retain (intstr_t str);


/* --------------------------------------------------------------------------
 * function intstr_release(str)
 * --------------------------------------------------------------------------
 * Cancels an outstanding retain, or deallocates str if there are no
 * outstanding retains.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  if str's reference count is zero upon entry, str is deallocated
 * o  if str's reference count is not zero upon entry, it is decremented
 *
 * error-conditions:
 * o  if str is not NULL upon entry, no operation is carried out
 * ----------------------------------------------------------------------- */

void intstr_release (intstr_t str);


#endif /* INTSTR_H */

/* END OF FILE */