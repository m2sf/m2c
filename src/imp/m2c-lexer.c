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

#include "iso646.h"
#include "infile.h"

#include "m2c-lexer.h"
#include "m2c-error.h"
#include "m2c-digest.h"
#include "m2c-match-lex.h"
#include "m2c-compiler-options.h"

#include <stdlib.h>
#include <stddef.h>


/* --------------------------------------------------------------------------
 * private type m2c_symbol_struct_t
 * --------------------------------------------------------------------------
 * record type holding symbol details.
 * ----------------------------------------------------------------------- */

typedef struct {
  intstr_t lexeme;
  m2c_token_t token;
  unsigned int line;
  unsigned int column;
} m2c_symbol_struct_t;


/* --------------------------------------------------------------------------
 * null symbol for initialisation.
 * ----------------------------------------------------------------------- */

static const m2c_symbol_struct_t nullsym = {
  /* lexeme */ NULL,
  /* token */ TOKEN_UNKNOWN,
  /* line */ 0,
  /* column */ 0
}; /* null_symbol */


/* --------------------------------------------------------------------------
 * private type match_handler_t
 * --------------------------------------------------------------------------
 * function type for lexical matching function.
 * ----------------------------------------------------------------------- */

typedef char (*match_handler_t) (infile_t, *m2c_token_t, *intstr_t);


/* --------------------------------------------------------------------------
 * hidden type m2c_lexer_struct_t
 * --------------------------------------------------------------------------
 * record type representing a Modula-2 lexer object.
 * ----------------------------------------------------------------------- */

struct m2c_lexer_struct_t {
  m2c_infile_t infile;
  m2c_symbol_struct_t current;
  m2c_symbol_struct_t lookahead;
  m2c_lexer_status_t status;
  m2c_digest_context_t digest;
  m2c_digest_mode_t digest_mode;
  match_handler_t match_ident;
  match_handler_t match_ident_or_resword;
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
   
   infile_t infile;
   m2c_lexer_t new_lexer;
   infile_status_t infile_status;
   
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
   new_lexer->current = nullsym;
   new_lexer->lookahead = nullsym;
   new_lexer->digest = m2c_digest_init();
   new_lexer->digest_mode = M2C_DIGEST_DONT_PREPEND_SPACER;
   
   if (m2c_compiler_option_dollar_identifiers()) {
    new_lexer->match_ident = m2c_match_lowline_ident;
    new_lexer->match_ident_or_resword = m2c_match_lowline_ident_or_resword;
   }
   else /* no dollar identifiers */ {
    new_lexer->match_ident = m2c_match_ident;
    new_lexer->match_ident_or_resword = m2c_match_ident_or_resword;
   } /* end if */
      
   /* read first symbol */
   get_new_lookahead_sym(new_lexer);
   new_lexer->digest_mode = M2C_DIGEST_PREPEND_SPACER;
   
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
  intstr_release(lexer->current.lexeme);
  
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
 * function m2c_lexer_digest(lexer)
 * --------------------------------------------------------------------------
 * Returns the digest value of the file associated with lexer.
 * ----------------------------------------------------------------------- */

m2c_digest_value_t m2c_lexer_digest (m2c_lexer_t lexer) {
  
  return m2c_digest_value(lexer->digest);
  
} /* end m2c_lexer_digest */


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


/* Private Functions */

/* --------------------------------------------------------------------------
 * private procedure get_new_lookahead_sym(lexer)
 * --------------------------------------------------------------------------
 * Skips any whitespace, newlines, disabled code sections, illegal characters
 * and if compiler switch preserve-comments is off, any comments; matches the
 * remaining input  to a valid symbol,  and  reads it  into lexer's lookahead
 * symbol buffer.  Any errors are reported to the console via error-reporter. 
 * ----------------------------------------------------------------------- */

static void get_new_lookahead_sym (m2c_lexer_t lexer) {
  
  unsigned int line, column;
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
      next_char = infile_consume_char(lexer->infile);
    } /* end while */
    
    /* get line and column of lookahead */
    line = m2c_infile_current_line(lexer->infile);
    column = m2c_infile_current_column(lexer->infile);
    
    /* identifier */
    if (IS_LOWER_LETTER(next_char)) {
      next_char = lexer->match_ident(lexer->infile, &token, &lexeme);
    }
    /* identifier or reserved word */
    else if (IS_UPPER_LETTER(next_char)) {
      next_char =
        lexer->match_ident_or_resword(lexer->infile, &token, &lexeme);
    }
    /* numeric literal */
    else if (IS_DIGIT(next_char)) {
      infile_mark_lexeme(lexer->infile);
      next_char = m2c_match_numeric_literal(lexer->infile, &token, &lexeme);
    }
    else {
      switch (next_char) {
          
        case '!' :
          /* line comment */        
          next_char =
            m2c_match_line_comment(lexer->infile, &token, &lexeme);
          break;
        
        case '\"' :
          /* double quoted literal */
          next_char =
            m2c_match_quoted_literal(lexer->infile, &token, &lexeme);
          break;
        
        case '#' :
          /* not-equal operator */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_NOTEQUAL;
          break;
        
        case '&' :
          /* concatenation operator */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_CONCAT;
          break;
        
        case '\'' :
          /* single quoted literal */
          next_char =
            m2c_match_quoted_literal(lexer->infile, &token, &lexeme);
          break;
        
        case '(' :
          /* left parenthesis */
          if (m2c_la2_char(lexer->infile) != '*') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_LEFT_PARENTHESIS;
          }
          else /* block comment */ {
            next_char =
              m2c_match_block_comment(lexer->infile, &token, &lexeme);
          } /* end if */
          break;
        
        case ')' :
          /* right parenthesis */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_RIGHT_PARENTHESIS;
          break;
        
        case '*' :
          /* asterisk operator */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_ASTERISK;
          break;
        
        case '+' :
          /* increment or plus */
          next_char = infile_consume_char(lexer->infile);
        
          /* increment suffix */
          if (next_char == '+') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_PLUS_PLUS;
          }
          /* plus operator */
          else {
            token = TOKEN_PLUS;
          } /* end if */
          break;
        
        case ',' :
          /* comma */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_COMMA;
          break;
        
        case '-' :
          /* decrement or minus */
          next_char = infile_consume_char(lexer->infile);
          
          /* decrement suffix */
          if (next_char == '-') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_MINUS_MINUS;
          }
          /* minus operator */
          else {
            token = TOKEN_MINUS;
          } /* end if */
          break;
        
        case '.' :
          /* range or wildcard or period */
          next_char = infile_consume_char(lexer->infile);
          
          /* range */
          if (next_char == '.') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_RANGE;
          }
          /* wildcard */
          else if (next_char == '*') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_WILDCARD;
          }
          /* period */
          else {
            token = TOKEN_PERIOD;
          } /* end if */
          break;
        
        case '/' :
          /* solidus */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_SOLIDUS;
          break;
              
        case ':' :
          /* converstion or assignment or colon */
          next_char = infile_consume_char(lexer->infile);
          
          /* conversion */
          if (next_char == ':') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_CONVERSION;
          }
          /* assignment */ 
          else if (next_char == '=') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_ASSIGNMENT;
          }
          /* colon */
          else {
            token = TOKEN_COLON;
          } /* end if */
          break;
        
        case ';' :
          /* semicolon */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_SEMICOLON;
          break;
        
        case '<' :
          /* pragma */
          if (m2c_la2_char(lexer->infile) == '*') {
            next_char = m2c_match_pragma(lexer->infile, &token, &lexeme);
            break;
          }
          /* less-or-equal or less-than */
          next_char = infile_consume_char(lexer->infile);
          
          /* less-or-equal */
          if (next_char == '=') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_LESS_THAN_OR_EQUAL;
          }
          /* less-than */
          else {
            token = TOKEN_LESS_THAN;
          } /* end if */
          break;
        
        case '=' :
          /* identity or equal */
          next_char = infile_consume_char(lexer->infile);
          
          if (next_char == '=') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_IDENTITY;
          }
          /* equal */
          else {
            token = TOKEN_EQUAL;
          } /* end if */
          break;
        
        case '>' :
          /* greater-or-equal or equal operator */
          next_char = infile_consume_char(lexer->infile);
          
          /* greater-or-equal */
          if (next_char == '=') {
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_GREATER_THAN_OR_EQUAL;
          }
          /* greater */
          else {
            token = TOKEN_GREATER_THAN;
          } /* end if */
          break;
                
        case '@' :
          /* at sign */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_AT_SIGN;
          break;
        
        case '[' :
          /* left bracket */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_LEFT_BRACKET;
          break;
        
        case '\\' :
          /* backslash */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_BACKSLASH;
          break;
        
        case ']' :
          /* right bracket */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_RIGHT_BRACKET;
          break;
        
        case '^' :
          /* caret */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_DEREF;
          break;
              
        case '{' :
          /* left brace */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_LEFT_BRACE;
          break;
        
        case '|' :
          /* vertical bar */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_BAR;
          break;
        
        case '}' :
          /* right brace */
          next_char = infile_consume_char(lexer->infile);
          token = TOKEN_RIGHT_BRACE;
          break;
                      
        default :
          /* End-of-File marker */        
          if (m2c_infile_eof(lexer->infile)) {
            token = TOKEN_END_OF_FILE;
          }
          /* disabled code section */
          else if ((next_char == '?') && (column == 1)
            && (infile_la2_char(lexer->infile) == '<')) {
            next_char = m2c_match_disabled_code_block(lexer->infile);
            token = TOKEN_UNKNOWN;
          }
          else /* invalid char */ {
            m2c_emit_lex_error
              (M2C_ERROR_ILLEGAL_CHAR, lexer->infile,
               next_char, infile_line(lexer->infile), infile_column(lexer->infile));
            next_char = infile_consume_char(lexer->infile);
            token = TOKEN_UNKNOWN;
          } /* end if */
      } /* end switch */
    } /* end if */
  } /* end while */
  
  /* update module digest */
  if (M2C_IS_SPECIAL_SYMBOL_TOKEN(token)) {
    m2c_digest_add_token(lexer->digest, lexer->digest_mode, token);
  }
  else if ((token == TOKEN_IDENT) || (M2C_IS_RESWORD_TOKEN)
    || (M2C_IS_LITERAL_TOKEN(token)) || (token == TOKEN_PRAGMA)) {
    m2c_digest_add_lexeme(lexer->digest, lexer->digest_mode, lexeme);
  }
  else if (token == TOKEN_EOF) {
    m2c_digest_finalize(lexer->digest);
  } /* end if */
  
  /* update lexer's lookahead symbol */
  lexer->lexeme = lexeme;
  lexer->lookahead.token = token;
  lexer->lookahead.line = line;
  lexer->lookahead.column = column;
  
  return;
} /* end get_new_lookahead_sym */


/* END OF FILE */