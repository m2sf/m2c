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

/* --------------------------------------------------------------------------
 * error codes
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_ERROR_ILLEGAL_CHAR,
  M2C_ERROR_ILLEGAL_CHAR_IN_TOKEN,
  M2C_ERROR_ILLEGAL_ESCAPE_SEQUENCE,
  M2C_ERROR_MISSING_DIGIT_AFTER_DP,
  M2C_ERROR_MISSING_DIGIT_AFTER_DSEP,
  M2C_ERROR_MISSING_EXPONENT_AFTER_E,
} m2c_error_t;


/* Lexical Errors */

/* --------------------------------------------------------------------------
 * procedure m2c_emit_lex_error(code, infile, ch, line, col)
 * --------------------------------------------------------------------------
 * Reports a lexical error of type code in infile, without token context, for
 * the given offending character at the given line and column. 
 * ----------------------------------------------------------------------- */

void m2c_emit_lex_error
  (m2c_error_t code, infile_t infile, char ch, uint_t line, uint_t col);


/* --------------------------------------------------------------------------
 * procedure m2c_emit_lex_error_in_token(code, infile, token, ch, line, col)
 * --------------------------------------------------------------------------
 * Reports a lexical error  of type code  in infile  with given token context
 * for the given offending character at the given line and column. 
 * ----------------------------------------------------------------------- */

void m2c_emit_lex_error_in_token
  (m2c_error_t code, infile_t infile,
   m2c_token_t token, char ch, uint_t line, uint_t col);


/* Syntax Errors */

/* TO DO */


#endif M2C_ERROR_REPORTER_H

/* END OF FILE */