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
 * m2c-reswords.h                                                            *
 *                                                                           *
 * Public interface of Modula-2 reserved word matching module.               *
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

#ifndef M2C_RESWORDS_H
#define M2C_RESWORDS_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "interned-strings.h"
#include "m2c-token.h"


/* --------------------------------------------------------------------------
 * function m2c_resword_token_for_lexeme(lexeme, default_token)
 * --------------------------------------------------------------------------
 * Tests  if lexeme matches  a reserved word  and  returns  its corresponding
 * token.  Returns default_token if lexeme does not match a reserved word.
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_resword_token_for_lexeme
  (intstr_t lexeme, m2c_token_t default_token);


/* --------------------------------------------------------------------------
 * function m2c_resword_lexeme_for_token(token)
 * --------------------------------------------------------------------------
 * Tests  if token represents  a reserved word  and returns its corresponding
 * lexeme.  Returns NULL if token does not represent a reserved word.
 * ----------------------------------------------------------------------- */

intstr_t m2c_resword_lexeme_for_token (m2c_token_t token);


#endif M2C_RESWORDS_H

/* END OF FILE */