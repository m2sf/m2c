/* M2C Modula-2 Compiler & Translator
 *
 * Copyright (c) 2015-2023 Benjamin Kowarsch
 *
 * @synopsis
 *
 * M2C  is a  Modula-2 to C translator  and  via-C compiler  for the bootstrap
 * subset of the revised Modula-2 language described in
 *
 * https://github.com/m2sf/PDFs/blob/master/M2BSK%20Language%20Description.pdf
 *
 * In compiler mode,  M2C compiles Modula-2 source files via C to object files
 * or executables  using the host system's resident C compiler and linker.  In
 * translator mode, it translates Modula-2 source files to C source files.
 *
 * Further information at https://github.com/m2sf/m2c/wiki
 *
 * @file
 *
 * m2c-exl-lexer.h
 *
 * Public interface of M2C export list file lexer module.
 *
 * @license
 *
 * M2C is free software:  You can redistribute  and modify it  under the terms
 * of the  GNU Lesser General Public License (LGPL)  either version 2.1  or at
 * your choice version 3, both as published by the Free Software Foundation.
 *
 * M2C  is distributed  in the hope  that it will be useful,  but  WITHOUT ANY
 * WARRANTY;  without even  the implied warranty of MERCHANTABILITY or FITNESS
 * FOR ANY PARTICULAR PURPOSE.  Read the license for more details.
 *
 * You should have  received  a copy of the  GNU Lesser General Public License
 * along with M2C.  If not, see <https://www.gnu.org/copyleft/lesser.html>.
 */

#ifndef M2C_EXL_LEXER_H
#define M2C_EXL_LEXER_H

#include "string.h"


/* ---------------------------------------------------------------------------
 * type m2c_exl_token_t
 * ---------------------------------------------------------------------------
 * Enumerated token values representing export list terminal symbols.
 * ------------------------------------------------------------------------ */

typedef enum {
  
  /* Invalid Token */
  
  EXL_TOKEN_INVALID,         /* 0 */
  
  /* Section Prefixes */
  
  EXL_TOKEN_EXPORTER,        /* 1 */
  EXL_TOKEN_IMPORTERS,       /* 2 */
  EXL_TOKEN_TYPES,           /* 3 */
  EXL_TOKEN_CONSTANTS,       /* 4 */
  EXL_TOKEN_VARIABLES,       /* 5 */
  EXL_TOKEN_FUNCTIONS,       /* 6 */
  EXL_TOKEN_PROCEDURES,      /* 7 */
 
  /* Identifiers */
  
  EXL_TOKEN_IDENT,           /* 8 */
  EXL_TOKEN_LOWLINE_IDENT,   /* 9 */
  
  /* Punctuation */

  EXL_TOKEN_COMMA,           /* 10 */
  EXL_TOKEN_SEMICOLON,       /* 11 */
  EXL_TOKEN_ASTERISK,        /* 12 */
   
  /* End Of File Marker */
  
  EXL_TOKEN_EOF,             /* 13 */

  /* Enumeration Terminator */

  EXL_TOKEN_END_MARK         /* 14 */   /* marks the end of the enumeration */

} m2c_exl_token_t;


/* --------------------------------------------------------------------------
 * opaque type m2c_exl_lexer_t
 * --------------------------------------------------------------------------
 * Opaque pointer type representing an export list lexer object.
 * ----------------------------------------------------------------------- */

typedef struct m2c_exl_lexer_struct_t *m2c_exl_lexer_t;


/* --------------------------------------------------------------------------
 * type m2c_exl_lexer_status_t
 * --------------------------------------------------------------------------
 * Status codes for operations on type m2c_exl_lexer_t.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_EXL_LEXER_STATUS_SUCCESS,
  M2C_EXL_LEXER_STATUS_INVALID_REFERENCE,
  M2C_EXL_LEXER_STATUS_ALLOCATION_FAILED,
  /* add more status codes as needed */
} m2c_exl_lexer_status_t;


/* --------------------------------------------------------------------------
 * procedure m2c_exl_new_lexer(lexer, filename, status)
 * --------------------------------------------------------------------------
 * Allocates a new object of type m2c_exl_lexer_t, opens an input file and
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
 *    and status M2C_EXL_LEXER_STATUS_INVALID_REFERENCE is returned
 * o  if the file represented by filename cannot be found
 *    status M2C_EXL_LEXER_STATUS_FILE_NOT_FOUND is returned
 * o  if the file represented by filename cannot be accessed
 *    status M2C_EXL_LEXER_STATUS_FILE_ACCESS_DENIED is returned
 * o  if no infile object could be allocated
 *    status M2C_EXL_LEXER_STATUS_ALLOCATION_FAILED is returned
 * ----------------------------------------------------------------------- */

void m2c_exl_new_lexer
  (m2c_exl_lexer_t *lexer,
   m2c_string_t filename,
   m2c_exl_lexer_status_t *status);


/* --------------------------------------------------------------------------
 * function m2c_exl_read_sym(lexer)
 * --------------------------------------------------------------------------
 * Reads the lookahead symbol from the export list file associated with lexer
 * and consumes it, thus advancing the current reading position, then returns
 * the symbol's token.
 *
 * pre-conditions:
 * o  parameter infile must not be NULL upon entry
 *
 * post-conditions:
 * o  character code of lookahead character or EOF is returned
 * o  current reading position and line and column counters are updated
 * o  file status is set to M2C_INFILE_STATUS_SUCCESS
 *
 * error-conditions:
 * o  if infile is NULL upon entry, no operation is carried out
 *    and status M2C_INFILE_STATUS_INVALID_REFERENCE is returned
 * ----------------------------------------------------------------------- */

m2c_exl_token_t m2c_exl_read_sym (m2c_exl_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_exl_next_sym(lexer)
 * --------------------------------------------------------------------------
 * Reads the lookahead symbol from the export list file associated with lexer
 * but does not consume it, thus not advancing the current reading position,
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

m2c_exl_token_t m2c_exl_next_sym (m2c_exl_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_exl_consume_sym(lexer)
 * --------------------------------------------------------------------------
 * Consumes the lookahead symbol and returns the new lookahead symbol.
 * ----------------------------------------------------------------------- */

m2c_exl_token_t m2c_exl_consume_sym (m2c_exl_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_exl_lexer_filename(lexer)
 * --------------------------------------------------------------------------
 * Returns the filename associated with lexer.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_exl_lexer_filename (m2c_exl_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_exl_lexer_status(lexer)
 * --------------------------------------------------------------------------
 * Returns the status of the last operation on lexer.
 * ----------------------------------------------------------------------- */

m2c_exl_lexer_status_t m2c_exl_lexer_status (m2c_exl_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_exl_lexer_lookahead_lexeme(lexer)
 * --------------------------------------------------------------------------
 * Returns the lexeme of the lookahead symbol.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_exl_lexer_lookahead_lexeme (m2c_exl_lexer_t lexer);


/* --------------------------------------------------------------------------
 * function m2c_exl_lexer_current_lexeme(lexer)
 * --------------------------------------------------------------------------
 * Returns the lexeme of the most recently consumed symbol.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_exl_lexer_current_lexeme (m2c_exl_lexer_t lexer);


/* --------------------------------------------------------------------------
 * procedure m2c_exl_release_lexer(lexer, status)
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

void m2c_exl_release_lexer
  (m2c_exl_lexer_t *lexer, m2c_exl_lexer_status_t *status);


#endif /* M2C_EXL_LEXER_H */

/* END OF FILE */