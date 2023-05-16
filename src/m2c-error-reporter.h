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
 * m2c-error-reporter.h                                                      *
 *                                                                           *
 * Public interface of error reporting module.                               *
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

#ifndef M2C_ERROR_REPORTER_H
#define M2C_ERROR_REPORTER_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "infile.h"
#include "m2c-common.h"
#include "m2c-token.h"


/* Lexical Errors */

/* --------------------------------------------------------------------------
 * procedure m2c_emit_error_ill_char(infile, char, line, col)
 * --------------------------------------------------------------------------
 * Reports illegal character in infile at line and column to the console.
 * ----------------------------------------------------------------------- */

void m2c_emit_error_ill_char
  (infile_t infile, char ch, uint_t line, uint_t col);


/* --------------------------------------------------------------------------
 * procedure m2c_emit_error_ill_char_in_token(infile, token, ch, line, col)
 * --------------------------------------------------------------------------
 * Reports illegal character in infile within symbol token  at the given line
 * and column to the console.
 * ----------------------------------------------------------------------- */

void m2c_emit_error_ill_char_in_token
  (infile_t infile, m2c_token_t token, char ch, uint_t line, uint_t col);


/* Syntax Errors */

/* TO DO */


#endif M2C_ERROR_REPORTER_H

/* END OF FILE */