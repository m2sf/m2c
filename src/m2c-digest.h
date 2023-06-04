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
  * m2c-digest.h                                                              *
  *                                                                           *
  * Public interface of M2C module digest library.                            *
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

#ifndef M2C_DIGEST_H
#define M2C_DIGEST_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include <stdint.h>


/* --------------------------------------------------------------------------
 * type m2c_digest_value_t
 * --------------------------------------------------------------------------
 * Unsigned integer type to hold a digest value.
 * ----------------------------------------------------------------------- */

typedef m2c_digest_value_t uint32_t;


/* --------------------------------------------------------------------------
 * type m2c_digest_t
 * --------------------------------------------------------------------------
 * Opaque type to hold a digest context.
 * ----------------------------------------------------------------------- */

typedef m2c_digest_s *m2c_digest_t;


/* --------------------------------------------------------------------------
 * function m2c_digest_init()
 * --------------------------------------------------------------------------
 * Returns a newly allocated and initialised digest context.
 * ----------------------------------------------------------------------- */

m2c_digest_t m2c_digest_init (void);


/* --------------------------------------------------------------------------
 * procedure m2c_digest_add_token(context, token)
 * --------------------------------------------------------------------------
 * Updates the rolling digest value of context with the lexeme of token, pre-
 * pending a single whitespace, except for the very first symbol added.
 * ----------------------------------------------------------------------- */

void m2c_digest_add_token (m2c_digest_t digest, m2c_token_t token);

/* --------------------------------------------------------------------------
 * procedure m2c_digest_add_lexeme(context, lexeme)
 * --------------------------------------------------------------------------
 * Updates  the  rolling digest value  of context  with lexeme,  prepending a
 * single whitespace,  except for the very first symbol added.
 * ----------------------------------------------------------------------- */
 
void m2c_digest_add_lexeme (m2c_digest_t digest, intstr_t lexeme);


/* --------------------------------------------------------------------------
 * function m2c_digest_finalize(context)
 * --------------------------------------------------------------------------
 * Finalizes the digest calculation of context.
 * ----------------------------------------------------------------------- */

void m2c_digest_finalize (m2c_digest_t digest);


/* --------------------------------------------------------------------------
 * function m2c_digest_value(context)
 * --------------------------------------------------------------------------
 * Returns the digest value of context.
 * ----------------------------------------------------------------------- */

m2c_digest_value_t m2c_digest_value (m2c_digest_t digest);


/* --------------------------------------------------------------------------
 * function m2c_digest_release(context)
 * --------------------------------------------------------------------------
 * Releases context, returns NULL.
 * ----------------------------------------------------------------------- */

m2c_digest_t m2c_digest_release (m2c_digest_t);


#endif /* M2C_DIGEST_H */

/* END OF FILE */