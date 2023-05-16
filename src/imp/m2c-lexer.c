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
 * m2c-lexer.c                                                               *
 *                                                                           *
 * Implementation of Modula-2 lexer module.                                  *
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

#include "m2c-lexer.h"
#include "m2c-error.h"
#include "m2c-filereader.h"
#include "m2c-compiler-options.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * language defined lexical limits
 * ----------------------------------------------------------------------- */

#define COMMENT_NESTING_LIMIT 10


/* --------------------------------------------------------------------------
 * private type m2c_symbol_struct_t
 * --------------------------------------------------------------------------
 * record type holding symbol details.
 * ----------------------------------------------------------------------- */

typedef struct {
  /* token */ m2c_token_t token;
  /* line */ uint_t line;
  /* column */ uint_t column;
  /* lexeme */ m2c_string_t lexeme;
} m2c_symbol_struct_t;


/* --------------------------------------------------------------------------
 * null symbol for initialisation.
 * ----------------------------------------------------------------------- */

static const m2c_symbol_struct_t null_symbol = {
  /* token */ TOKEN_UNKNOWN,
  /* line */ 0,
  /* column */ 0,
  /* lexeme */ NULL
}; /* null_symbol */


/* --------------------------------------------------------------------------
 * hidden type m2c_lexer_struct_t
 * --------------------------------------------------------------------------
 * record type representing a Modula-2 lexer object.
 * ----------------------------------------------------------------------- */

struct m2c_lexer_struct_t {
  m2c_infile_t infile;
  m2c_symbol_struct_t current;
  /* lookahead */ m2c_symbol_struct_t lookahead;
  /* status */ m2c_lexer_status_t status;
  /* error_count */ uint_t error_count;
}; /* m2c_lexer_struct_t */

typedef struct m2c_lexer_struct_t m2c_lexer_struct_t;


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
 * o  M2C_LEXER_STATUS_SUCCESS is passed back in status, unless NULL
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
  (m2c_lexer_t *lexer, intstr_t filename, m2c_lexer_status_t *status) {
   
   m2c_infile_t infile;
   m2c_lexer_t new_lexer;
   m2c_infile_status_t infile_status;
   
   /* check pre-conditions */
   if ((lexer == NULL) || (filename == NULL)) {
     SET_STATUS(status, M2C_LEXER_STATUS_INVALID_REFERENCE);
     return;
   } /* end if */
   
   new_lexer = malloc(sizeof(m2c_lexer_struct_t));
   
   if (new_lexer == NULL) {
     SET_STATUS(status, M2C_LEXER_STATUS_ALLOCATION_FAILED);
     return;
   } /* end if */
   
   /* open source file */
   infile = infile_open(filename, &infile_status);
   
  if (infile_status != FILEIO_STATUS_SUCCESS) {
    switch (infile_status) {
      case FILEIO_INVALID_FILENAME :
        SET_STATUS(status, M2C_LEXER_STATUS_INVALID_FILENAME);
        break;
      case FILEIO_STATUS_FILE_NOT_FOUND :
        SET_STATUS(status, M2C_LEXER_STATUS_FILE_NOT_FOUND);
        break;
      case FILEIO_STATUS_ACCESS_DENIED :
        SET_STATUS(status, M2C_LEXER_STATUS_FILE_ACCESS_DENIED);
        break;
      case FILEIO_STATUS_DEVICE_ERROR :
        SET_STATUS(status, M2C_LEXER_STATUS_DEVICE_ERROR);
        break;
    } /* end switch */
    free(new_lexer);
    return;
  } /* end if */
   
   /* initialise lexer object */
   new_lexer->infile = infile;
   new_lexer->current = null_symbol;
   new_lexer->lookahead = null_symbol;
   new_lexer->error_count = 0;
      
   /* read first symbol */
   get_new_lookahead_sym(new_lexer);
   
   *lexer = new_lexer;
   return;
} /* end m2c_new_lexer */


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
 * o  lookahead symbol's token is returned
 * o  current reading position and line and column counters are updated
 * o  file status is set to M2C_LEXER_STATUS_SUCCESS
 *
 * error-conditions:
 * o  if lexer is NULL upon entry, no operation is carried out
 *    and status M2C_LEXER_STATUS_INVALID_REFERENCE is returned
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_read_sym (m2c_lexer_t lexer) {
  
  /* release the lexeme of the current symbol */
  m2c_string_release(lexer->current.lexeme);
  
  /* lookahead symbol becomes current symbol */
  lexer->current = lexer->lookahead;
  
  /* read new lookahead symbol */
  get_new_lookahead_sym(lexer);
  
  /* return current token */
  return lexer->current.token;
  
} /* end m2c_read_sym */


/* --------------------------------------------------------------------------
 * function m2c_next_sym(lexer)
 * --------------------------------------------------------------------------
 * Returns the lookahead symbol without consuming it.
 * ----------------------------------------------------------------------- */

inline m2c_token_t m2c_next_sym (m2c_lexer_t lexer) {
  
  return lexer->lookahead.token;

} /* end m2c_next_sym */


/* --------------------------------------------------------------------------
 * function m2c_consume_sym(lexer)
 * --------------------------------------------------------------------------
 * Consumes the lookahead symbol and returns the new lookahead symbol.
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_consume_sym (m2c_lexer_t lexer) {
  
  /* release the lexeme of the current symbol */
  m2c_string_release(lexer->current.lexeme);
  
  /* lookahead symbol becomes current symbol */
  lexer->current = lexer->lookahead;
  
  /* read new lookahead symbol and return it */
  get_lookahead_sym(lexer);
  return lexer->lookahead.token;
  
} /* end m2c_consume_sym */


/* --------------------------------------------------------------------------
 * function m2c_lexer_filename(lexer)
 * --------------------------------------------------------------------------
 * Returns the filename associated with lexer.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_lexer_filename (m2c_lexer_t lexer) {
  
  return infile_filename(lexer->infile);
  
} /* end m2c_lexer_filename */


/* --------------------------------------------------------------------------
 * function m2c_lexer_status(lexer)
 * --------------------------------------------------------------------------
 * Returns the status of the last operation on lexer.
 * ----------------------------------------------------------------------- */

m2c_lexer_status_t m2c_lexer_status (m2c_lexer_t lexer) {
  
  return lexer->status;
  
} /* end m2c_lexer_status */


/* --------------------------------------------------------------------------
 * function m2c_lexer_lookahead_lexeme(lexer)
 * --------------------------------------------------------------------------
 * Returns the lexeme of the lookahead symbol.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_lexer_lookahead_lexeme (m2c_lexer_t lexer) {
  
  m2c_string_retain(lexer->lookahead.lexeme);
  
  return lexer->lookahead.lexeme;
  
} /* end m2c_lexer_lookahead_lexeme */


/* --------------------------------------------------------------------------
 * function m2c_lexer_current_lexeme(lexer)
 * --------------------------------------------------------------------------
 * Returns the lexeme of the most recently consumed symbol.
 * ----------------------------------------------------------------------- */

m2c_string_t m2c_lexer_current_lexeme (m2c_lexer_t lexer) {
  
  m2c_string_retain(lexer->current.lexeme);
  
  return lexer->current.lexeme;
  
} /* end m2c_lexer_current_lexeme */


/* --------------------------------------------------------------------------
 * function m2c_lexer_lookahead_line(lexer)
 * --------------------------------------------------------------------------
 * Returns the line counter of the lookahead symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_lookahead_line (m2c_lexer_t lexer) {
  
  return lexer->lookahead.line;
  
} /* end m2c_lexer_lookahead_line */


/* --------------------------------------------------------------------------
 * function m2c_lexer_current_line(lexer)
 * --------------------------------------------------------------------------
 * Returns the line counter of the most recently consumed symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_current_line (m2c_lexer_t lexer) {
  
  return lexer->current.line;
  
} /* end m2c_lexer_current_line */


/* --------------------------------------------------------------------------
 * function m2c_lexer_lookahead_column(lexer)
 * --------------------------------------------------------------------------
 * Returns the column counter of the lookahead symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_lookahead_column (m2c_lexer_t lexer) {
  
  return lexer->lookahead.column;
  
} /* end m2c_lexer_lookahead_column */


/* --------------------------------------------------------------------------
 * function m2c_lexer_current_column(lexer)
 * --------------------------------------------------------------------------
 * Returns the column counter of the most recently consumed symbol.
 * ----------------------------------------------------------------------- */

uint_t m2c_lexer_current_column (m2c_lexer_t lexer) {
  
  return lexer->current.column;
  
} /* end m2c_lexer_current_column */


/* --------------------------------------------------------------------------
 * procedure m2c_print_line_and_mark_column(lexer, line, column)
 * --------------------------------------------------------------------------
 * Prints the given source line of the current symbol to the console and
 * marks the given coloumn with a caret '^'.
 * ----------------------------------------------------------------------- */

void m2c_print_line_and_mark_column
  (m2c_lexer_t lexer, uint_t line, uint_t column) {
  
  uint_t n;
  
  /* print the line */
  infile_print_line(lexer->infile, line);
  
  /* advance to column */
  n = 1;
  while (n < column) {
    console_write_char(" ");
    n++;
  } /* end while */
  
  /* mark the column with a caret */
  console_write_chars("^\n\n");
  
  return;
} /* end m2c_print_line_and_mark_column */


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

void m2c_release_lexer (m2c_lexer_t *lexptr, m2c_lexer_status_t *status) {
  
  m2c_lexer_t lexer;
  
  /* check pre-conditions */
  if ((lexptr == NULL) || (*lexptr == NULL)) {
    SET_STATUS(status, M2C_LEXER_STATUS_INVALID_REFERENCE);
    return;
  } /* end if */
  
  lexer = *lexptr;
  
  m2c_close_infile(&lexer->infile, NULL);
  m2c_string_release(lexer->current.lexeme);
  m2c_string_release(lexer->lookahead.lexeme);
  
  free(lexer);
  *lexptr = NULL;
  
  return;
} /* end m2c_release_lexer */


/* --------------------------------------------------------------------------
 * procedure report_error_w_offending_pos(error, lexer, line, column)
 * ----------------------------------------------------------------------- */

static void report_error_w_offending_pos
  (m2c_error_t error, m2c_lexer_t lexer, uint_t line, uint_t column) {
    
  m2c_emit_error_w_pos(error, line, column);
  
  if (m2c_option_verbose()) {
    m2c_print_line_and_mark_column(lexer, line, column);
  } /* end if */
  
  lexer->error_count++;
  
  return;
} /* end report_error_w_offending_pos */
  

/* --------------------------------------------------------------------------
 * procedure report_error_w_offending_char(error, lexer, line, column, char)
 * ----------------------------------------------------------------------- */

static void report_error_w_offending_char
  (m2c_error_t error,
   m2c_lexer_t lexer, uint_t line, uint_t column, char offending_char) {
    
  m2c_emit_error_w_chr(error, line, column, offending_char);
  
  if (m2c_option_verbose() && (!IS_CONTROL_CHAR(offending_char))) {
    m2c_print_line_and_mark_column(lexer, line, column);
  } /* end if */
  
  lexer->error_count++;
  
  return;
} /* end report_error_w_offending_char */
  

/* --------------------------------------------------------------------------
 * private procedure get_new_lookahead_sym(lexer)
 * ----------------------------------------------------------------------- */

static void get_new_lookahead_sym (m2c_lexer_t lexer) {
  
  uint_t line, column;
  m2c_token_t token;
  char next_char;
  
  /* no token yet */
  token = TOKEN_UNKNOWN;
  
  /* get the lookahead character */
  next_char = m2c_next_char(lexer->infile);
  
  while (token == TOKEN_UNKNOWN) {
  
    /* skip all whitespace and line feeds */
    while ((next_char == ASCII_SPACE) ||
           (next_char == ASCII_TAB) ||
           (next_char == ASCII_LF)) {
      
      /* consume the character and get new lookahead */
      next_char = m2c_consume_char(lexer->infile);
    } /* end while */
    
    /* get line and column of lookahead */
    line = m2c_infile_current_line(lexer->infile);
    column = m2c_infile_current_column(lexer->infile);
    
    /* identifier or reserved word */
   if (next_char >= 'a') && (next_char <= 'z') {
      infile_mark_lexeme(lexer->infile);
      next_char = m2c_match_ident(lexer->infile, &token);
      lexeme = infile_lexeme(lexer->infile);
    }
    else if (next_char >= 'A') && (next_char <= 'Z') {
      infile_mark_lexeme(lexer->infile);
      next_char = m2c_match_ident_or_resword(lexer->infile, &allcaps);
      lexeme = infile_lexeme(lexer->infile);
      
      /* set default token */
      token = TOKEN_STDIDENT;

      /* determine if resword */
      if (allcaps) {
        token = m2c_reword_token_for_lexeme(lexeme);
      } /* end if */
    }
    else if (next_char >= '0') && (next_char <= '9') {
      infile_mark_lexeme(lexer->infile);
      next_char = m2c_match_numeric_literal(lexer->infile, &token);
      lexeme = infile_lexeme(lexer->infile);
    }
    else {
      switch (next_char) {
          
        case '!' :
          /* line comment */        
          next_char = skip_line_comment(lexer);
          token = TOKEN_LINE_COMMENT;
          break;
        
        case '\"' :
          /* double quoted literal */
          next_char = get_string_literal(lexer, &token);
          if (token == TOKEN_MALFORMED_STRING) {
            m2c_emit_error_w_pos
              (M2C_ERROR_MISSING_STRING_DELIMITER, line, column);
            lexer->error_count++;
          } /* end if */
          break;
        
        case '#' :
          /* not-equal operator */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_NOTEQUAL;
          break;
        
        case '&' :
          /* concatenation operator */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_CONCAT;
          break;
        
        case '\'' :
          /* single quoted literal */
          next_char = get_string_literal(lexer, &token);
          if (token == TOKEN_MALFORMED_STRING) {
            m2c_emit_error_w_pos
              (M2C_ERROR_MISSING_STRING_DELIMITER, line, column);
            lexer->error_count++;
          } /* end if */
          break;
        
        case '(' :
          /* left parenthesis */
          if (m2c_la2_char(lexer->infile) != '*') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_LEFT_PARENTHESIS;
          }
          else /* block comment */ {
            next_char = skip_block_comment(lexer);
            token = TOKEN_BLOCK_COMMENT;
          } /* end if */
          break;
        
        case ')' :
          /* right parenthesis */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_RIGHT_PARENTHESIS;
          break;
        
        case '*' :
          /* asterisk operator */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_ASTERISK;
          break;
        
        case '+' :
          /* increment or plus */
          next_char = m2c_consume_char(lexer->infile);
        
          /* increment suffix */
          if (next_char == '+') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_PLUS_PLUS;
          }
          /* plus operator */
          else {
            token = TOKEN_PLUS;
          } /* end if */
          break;
        
        case ',' :
          /* comma */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_COMMA;
          break;
        
        case '-' :
          /* decrement or minus */
          next_char = m2c_consume_char(lexer->infile);
          
          /* decrement suffix */
          if (next_char == '-') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_MINUS_MINUS;
          }
          /* minus operator */
          else {
            token = TOKEN_MINUS;
          } /* end if */
          break;
        
        case '.' :
          /* range or wildcard or period */
          next_char = m2c_consume_char(lexer->infile);
          
          /* range */
          if (next_char == '.') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_RANGE;
          }
          /* wildcard */
          else if (next_char == '*') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_WILDCARD;
          }
          /* period */
          else {
            token = TOKEN_PERIOD;
          } /* end if */
          break;
        
        case '/' :
          /* solidus */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_SOLIDUS;
          break;
              
        case ':' :
          /* converstion or assignment or colon */
          next_char = m2c_consume_char(lexer->infile);
          
          /* conversion */
          if (next_char == ':') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_CONVERSION;
          }
          /* assignment */ 
          else if (next_char == '=') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_ASSIGNMENT;
          }
          /* colon */
          else {
            token = TOKEN_COLON;
          } /* end if */
          break;
        
        case ';' :
          /* semicolon */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_SEMICOLON;
          break;
        
        case '<' :
          /* pragma */
          if (m2c_la2_char(lexer->infile) == '*') {
            next_char = get_pragma(lexer);
            token = TOKEN_PRAGMA;
            break;
          }
          /* less-or-equal or less-than */
          next_char = m2c_consume_char(lexer->infile);
          
          /* less-or-equal */
          if (next_char == '=') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_LESS_THAN_OR_EQUAL;
          }
          /* less-than */
          else {
            token = TOKEN_LESS_THAN;
          } /* end if */
          break;
        
        case '=' :
          /* identity or equal */
          next_char = m2c_consume_char(lexer->infile);
          
          if (next_char == '=') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_IDENTITY;
          }
          /* equal */
          else {
            token = TOKEN_EQUAL;
          } /* end if */
          break;
        
        case '>' :
          /* greater-or-equal or equal operator */
          next_char = m2c_consume_char(lexer->infile);
          
          /* greater-or-equal */
          if (next_char == '=') {
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_GREATER_THAN_OR_EQUAL;
          }
          /* greater */
          else {
            token = TOKEN_GREATER_THAN;
          } /* end if */
          break;
        
        case '?' :
          /* disabled code section */
          if ((column == 1) && (infile_la2_char(lexer->infile) == '<')) {
            next_char = skip_code_section(lexer);
          }
          else /* invalid character */ {
            report_error_w_offending_char
              (M2C_ERROR_INVALID_INPUT_CHAR, lexer, line, column, next_char);
            next_char = m2c_consume_char(lexer->infile);
          } /* end if */
          token = TOKEN_UNKNOWN;
          break;
        
        case '@' :
          /* at sign */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_AT_SIGN;
          break;
        
        case '[' :
          /* left bracket */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_LEFT_BRACKET;
          break;
        
        case '\\' :
          /* backslash */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_BACKSLASH;
          break;
        
        case ']' :
          /* right bracket */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_RIGHT_BRACKET;
          break;
        
        case '^' :
          /* caret */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_DEREF;
          break;
              
        case '{' :
          /* left brace */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_LEFT_BRACE;
          break;
        
        case '|' :
          /* vertical bar */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_BAR;
          break;
        
        case '}' :
          /* right brace */
          next_char = m2c_consume_char(lexer->infile);
          token = TOKEN_RIGHT_BRACE;
          break;
                      
        default :
          /* End-of-File marker */        
          if (m2c_infile_eof(lexer->infile)) {
            token = TOKEN_END_OF_FILE;
          }
          else /* invalid char */ {
            report_error_w_offending_char
              (M2C_ERROR_INVALID_INPUT_CHAR, lexer, line, column, next_char);
            next_char = m2c_consume_char(lexer->infile);
            token = TOKEN_UNKNOWN;
          } /* end if */
      } /* end switch */
    } /* end if */
  } /* end while */
  
  /* update lexer's lookahead symbol */
  lexer->lexeme = lexeme;
  lexer->lookahead.token = token;
  lexer->lookahead.line = line;
  lexer->lookahead.column = column;
  
  return;
} /* end get_new_lookahead_sym */


/* END OF FILE */