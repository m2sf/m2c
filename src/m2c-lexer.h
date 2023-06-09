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
 * m2c-lexer.h                                                               *
 *                                                                           *
 * Public interface of Modula-2 lexer module.                                *
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

#ifndef M2C_LEXER_H
#define M2C_LEXER_H

#include "m2c-token.h"
#include "m2c-common.h"
#include "m2c-digest.h"
#include "interned-strings.h"


/* --------------------------------------------------------------------------
 * opaque type m2c_lexer_t
 * --------------------------------------------------------------------------
 * Opaque pointer type representing a Modula-2 lexer object.
 * ----------------------------------------------------------------------- */

typedef struct m2c_lexer_struct_t *m2c_lexer_t;


/* --------------------------------------------------------------------------
 * type m2c_lexer_status_t
 * --------------------------------------------------------------------------
 * Status codes for operations on type m2c_lexer_t.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_LEXER_STATUS_SUCCESS,
  M2C_LEXER_STATUS_INVALID_REFERENCE,
  M2C_LEXER_STATUS_INVALID_FILENAME,
  M2C_LEXER_STATUS_FILE_NOT_FOUND,
  M2C_LEXER_STATUS_FILE_ACCESS_DENIED,
  M2C_LEXER_STATUS_DEVICE_ERROR,
  M2C_LEXER_STATUS_ALLOCATION_FAILED
} m2c_lexer_status_t;


/* --------------------------------------------------------------------------
 * procedure m2c_new_lexer(lexer, filename, status)
 * --------------------------------------------------------------------------
 * Allocates a new object of type m2c_lexer_t, opens an input file and
 * associates the opened file with the newly created lexer object.
 *
 * pre-conditions:
 * o  parameter lexer must be NULL upon entry
 * o  parameter status may be NULL
 *
 * post-conditions:
 * o  pointer to newly allocated and opened lexer is passed back in lexer
 * o  M2C_INFILE_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if lexer is not NULL upon entry, no operation is carried out
 *    and status M2C_LEXER_STATUS_INVALID_REFERENCE is returned
 * o  if the file represented by filename cannot be found
 *    status M2C_LEXER_STATUS_FILE_NOT_FOUND is returned
 * o  if the file represented by filename cannot be accessed
 *    status M2C_LEXER_STATUS_FILE_ACCESS_DENIED is returned
 * o  if no infile object could be allocated
 *    status M2C_LEXER_STATUS_ALLOCATION_FAILED is returned
 * ----------------------------------------------------------------------- */

void m2c_new_lexer
  (m2c_lexer_t *lexer, intstr_t filename, m2c_lexer_status_t *status);


/* --------------------------------------------------------------------------
 * function m2c_read_sym(lexer)
 * --------------------------------------------------------------------------
 * Reads the lookahead symbol from the source file associated with lexer and
 * consumes it, thus advancing the current reading position, then returns
 * the symbol's token.
 *
 * pre-conditions:
 * o  parameter infile must not be NULL upon entry
 *
 * post-conditions:
 * o  character code of lookahead character or EOF is returned
 * o  current reading position and line and column counters are updated
 * o  file status is set to M2C_INFILE_STATUC_SUCCESS
 *
 * error-conditions:
 * o  if infile is NULL upon entry, no operation is carried out
 *    and status M2C_INFILE_STATUS_INVALID_REFERENCE is returned
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_read_sym (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_next_sym(lexer)
 * --------------------------------------------------------------------------
 * Reads the lookahead symbol from the source file associated with lexer but
 * does not consume it, thus not advancing the current reading position,
 * then returns the symbol's token.
 *
 * pre-conditions:
 * o  parameter infile must not be NULL upon entry
 *
 * post-conditions:
 * o  character code of lookahead character or EOF is returned
 * o  current reading position and line and column counters are NOT updated
 * o  file status is set to M2C_LEXER_STATUS_SUCCESS
 *
 * error-conditions:
 * o  if infile is NULL upon entry, no operation is carried out
 *    and status M2C_LEXER_STATUS_INVALID_REFERENCE is returned
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_next_sym (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_consume_sym(lexer)
 * --------------------------------------------------------------------------
 * Consumes the lookahead symbol and returns the new lookahead symbol.
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_consume_sym (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_filename(lexer)
 * --------------------------------------------------------------------------
 * Returns the filename associated with lexer.
 * ----------------------------------------------------------------------- */

intstr_t m2c_lexer_filename (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_status(lexer)
 * --------------------------------------------------------------------------
 * Returns the status of the last operation on lexer.
 * ----------------------------------------------------------------------- */

m2c_lexer_status_t m2c_lexer_status (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_lookahead_lexeme(lexer)
 * --------------------------------------------------------------------------
 * Returns the lexeme of the lookahead symbol.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_lexer_lookahead_lexeme (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_current_lexeme(lexer)
 * --------------------------------------------------------------------------
 * Returns the lexeme of the most recently consumed symbol.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_lexer_current_lexeme (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_lookahead_line(lexer)
 * --------------------------------------------------------------------------
 * Returns the line counter of the lookahead symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_lookahead_line (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_current_line(lexer)
 * --------------------------------------------------------------------------
 * Returns the line counter of the most recently consumed symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_current_line (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_lookahead_column(lexer)
 * --------------------------------------------------------------------------
 * Returns the column counter of the lookahead symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_lookahead_column (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_current_column(lexer)
 * --------------------------------------------------------------------------
 * Returns the column counter of the most recently consumed symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_current_column (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_lexer_digest(lexer)
 * --------------------------------------------------------------------------
 * Returns the digest value of the file associated with lexer.
 * ----------------------------------------------------------------------- */

m2c_digest_value_t m2c_lexer_digest (m2c_lexer_t lexer);


/* --------------------------------------------------------------------------
 * procedure m2c_release_lexer(lexer, status)
 * --------------------------------------------------------------------------
 * Closes the file associated with lexer, deallocates its file object,
 * deallocates the lexer object and returns NULL in lexer.
 *
 * pre-conditions:
 * o  parameter lexer must not be NULL upon entry
 * o  parameter status may be NULL
 *
 * post-conditions:
 * o  file object is deallocated
 * o  NULL is passed back in lexer
 * o  M2C_LEXER_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if lexer is NULL upon entry, no operation is carried out
 *    and status M2C_LEXER_STATUS_INVALID_REFERENCE is returned
 * ----------------------------------------------------------------------- */

void m2c_release_lexer (m2c_lexer_t *lexer, m2c_lexer_status_t *status);


#endif /* M2C_LEXER_H */

/* END OF FILE */