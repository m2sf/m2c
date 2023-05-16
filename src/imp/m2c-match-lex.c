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
 * m2c-match-lex.c                                                           *
 *                                                                           *
 * Implementation of lexical matching module.                                *
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

#include "infile.h"
#include "m2c-token.h"
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * macros
 * ----------------------------------------------------------------------- */

#define IS_DIGIT(_ch) \
  ((_ch >= '0') && (_ch <= '9'))
#define IS_NOT_DIGIT(_ch) \
  ((_ch < '0') || (_ch > '9'))
#define IS_LOWER_LETTER(_ch) \
  ((_ch >= 'a') && (_ch <= 'z'))
#define IS_NOT_LOWER_LETTER(_ch) \
  ((_ch < 'a') || (_ch > 'z'))
#define IS_UPPER_LETTER(_ch) \
  ((_ch >= 'A') && (_ch <= 'Z'))
#define IS_NOT_UPPER_LETTER(_ch) \
  ((_ch < 'A') || (_ch > 'Z'))
#define IS_A_TO_F(_ch) \
  ((_ch >= 'A') && (_ch <= 'F'))
#define IS_PRINTABLE_CHAR(_ch) \
  (_ch >= 32) && (_ch < 127))
#define IS_CTRL_CHAR(_ch) \
  ((_ch < 32) || (_ch == 127))
#define IS_LEGAL_CTRL_CHAR(_ch) \
  ((_ch == ASCII_LF) || (_ch == ASCII_TAB))
#define IS_ILLEGAL_CTRL_CHAR(_ch) \
  (((_ch < 32) && (_ch != ASCII_LF) && (_ch != ASCII_TAB)) || (_ch == 127))


/* Semantic Symbols */

/* --------------------------------------------------------------------------
 * function m2c_match_ident(infile, token)
 * --------------------------------------------------------------------------
 * Matches  the input  at  the  current  reading position  of  infile  to  an
 * identifier and consumes it.  Passes the associated token back in out para-
 * meter token.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_ident (infile_t infile, m2c_token_t *token) {
  char next_char;
  
  next_char := infile_lookahead_char(infile);
  while (IS_LOWER_LETTER(next_char)
    || IS_UPPER_LETTER(next_char)
    || IS_DIGIT(next_char)) {
    next_char = infile_consume_char(infile);
  } /* end while */

  *token = TOKEN_STDIDENT;
  
  return next_char;
} /* end m2c_match_ident */


/* --------------------------------------------------------------------------
 * function m2c_match_ident_or_resword(infile, allcaps)
 * --------------------------------------------------------------------------
 * Matches  the input  at  the  current  reading position  of  infile  to  an
 * identifier or reserved word  and consumes it.  Passes true back in allcaps
 * if the matched input consists exclusively of uppercase characters,  passes
 * back false otherwise.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_ident_or_resword (infile_t infile, bool *allcaps) {
  char next_char;
  
  /* collect all uppercase letters */
  next_char = infile_lookahead_char(infile);
  while (IS_UPPER_LETTER(next_char)) {
    next_char = infile_consume_char(infile);
  } /* end while */
  
  /* check if followed by lowercase letter or digit */
  if (IS_LOWER_LETTER(next_char) || IS_DIGIT(next_char)) {
    next_char = infile_consume_char(infile);
    *allcaps = false;

    /* collect any remaining letters and digits */
    while (IS_LOWER_LETTER(next_char)
      || IS_UPPER_LETTER(next_char)
      || IS_DIGIT(next_char)) {
      next_char = infile_consume_char(infile);
    } /* end while */
  }
  else {
    *allcaps = true;
  } /* end if*/
  
  return next_char;
} /* end m2c_match_ident_or_resword */


/* --------------------------------------------------------------------------
 * function m2c_match_numeric_literal(infile, token)
 * --------------------------------------------------------------------------
 * Matches  the  input  at  the  current  reading position  of  infile  to  a
 * numeric literal and consumes it.  Passes the associated token back in  out
 * parameter token.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

static char match_decimal_number_tail(infile_t infile, m2c_token_t *token);
static char match_real_number_tail(infile_t infile, m2c_token_t *token);
static char match_base2_digit_seq(infile_t infile, m2c_token_t *token);
static char match_base16_digit_seq(infile_t infile, m2c_token_t *token);

char m2c_match_numeric_literal (infile_t infile, m2c_token_t *token) {
  char next_char;
  
  next_char = infile_lookahead_char(infile);

  if (next_char == '0') {
    next_char = infile_consume_char(infile);
    
    switch (next_char) {
      /* real number */
      case '.' :
        next_char = match_real_number_tail(infile, token);
        break;

      /* base-2 integer */
      case 'b' :
        next_char = match_base2_digit_seq(infile, token);
        break;

      /* character code */
      case 'u' :
        next_char = match_base16_digit_seq(infile, token);
        break;
        
      /* base-16 integer */
      case 'x' :
        next_char = match_base16_digit_seq(infile, token);
        break;

      default :
        /* single zero */
        if (IS_NOT_DIGIT(next_char)
          && IS_NOT_UPPER_LETTER(next_char)
          && IS_NOT_LOWER_LETTER(next_char)) {
            *token = TOKEN_WHOLE_NUMBER;
        }
        /* malformed literal */
        else {
          /* emit error - illegal char in number literal */
          m2c_emit_lex_error_in_token
            (M2C_ERROR_ILLEGAL_CHAR_IN_TOKEN, infile, TOKEN_WHOLE_NUMBER,
             next_char, infile_line(infile), infile_column(infile));
          
          /* collect all illegal chars */
          while (IS_LOWER_LETTER(next_char)
            || IS_UPPER_LETTER(next_char)
            || IS_DIGIT(next_char)) {
            next_char = infile_consume_char(infile);
            *token = TOKEN_MALFORMED_INTEGER;
          } /* end while */
        } /* end if */
    } /* end switch */
  }
  else if ((next_char >= '1') && (next_char <= '9')) {

    /* decimal integer or real number */
    next_char = match_decimal_number_tail(infile, token);
  } /* end if */

  return next_char;
} /* end m2c_match_numeric_literal */


/* --------------------------------------------------------------------------
 * function m2c_match_quoted_literal(infile, token)
 * --------------------------------------------------------------------------
 * Matches  the  input  at  the  current  reading position  of  infile  to  a
 * quoted literal  and consumes it.  Passes the associated token back in  out
 * parameter token.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_quoted_literal (infile_t infile, m2c_token_t *token) {
  char next_char, delimiter;
  bool malformed = false;
  
  /* consume opening string delimiter */
  delimiter = infile_lookahead_char(infile);
  next_char = infile_consume_char(infile);

  while (next_char != delimiter) {
    /* EOF */
    if (infile_eof(infile)) {
      /* emit error -- unexpected EOF in string literal */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_EOF_IN_TOKEN, infile, TOKEN_QUOTED_STRING,
         next_char, infile_line(infile), infile_column(infile));
        *token = TOKEN_MALFORMED_STRING;
        return next_char;
      } /* end if */

    /* check for control characters */
    if (IS_CTRL_CHAR(next_char)) {
      malformed = true;
      
      /* emit error -- control character in string literal */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_ILLEGAL_CHAR_IN_TOKEN, infile, TOKEN_QUOTED_STRING,
         next_char, infile_line(infile), infile_column(infile));
       } /* end if */
    } /* end if */
    
    if (next_char == '\\') {
      next_char - infile_consume_char(infile);

      if ((next_char != 'n') && (next_char != 't') && (next_char != '//')) {
        malformed = true;
        /* emit error -- invalid escape sequence */
        m2c_emit_lex_error_in_token
          (M2C_ERROR_INVALID_ESCAPE_SEQUENCE, infile, TOKEN_QUOTED_STRING,
           next_char, infile_line(infile), infile_column(infile));
       } /* end if */
    } /* end if */
    next_char = infile_consume_char(infile);
  } /* end while */
  
  /* consume closing string delimiter */
  if (next_char = delimiter) {
    next_char = infile_consume_char(infile);
  } /* end if */
  
  /* pass token */
  if (malformed) {
    *token = TOKEN_MALFORMED_STRING;
  }
  else {
    *token = TOKEN_QUOTED_STRING;
  } /* end if */

  return next_char;
} /* end m2c_match_quoted_literal */


/* Non-Semantic Symbols */

/* --------------------------------------------------------------------------
 * function m2c_match_line_comment(infile, token)
 * --------------------------------------------------------------------------
 * Matches the input  at the  current reading position  of  infile  to a line
 * comment  and consumes it.  Passes the associated token  back in  out para-
 * meter token.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_line_comment (infile_t infile, m2c_token_t *token) {
  char next_char;
  
  next_char = infile_consume_char(infile);

  while (infile_eof(infile) == false) {

    /* end of line terminates line comment */
    if (next_char == ASCII_LF) {
      next_char = infile_consume_char(infile);
      exit;
    }
    /* illegal control char */
    else if (IS_CTRL_CHAR(next_char) && (next_char != ASCII_TAB)) {
      /* emit error - illegal control char in comment */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_ILLEGAL_CHAR_IN_TOKEN, infile, TOKEN_LINE_COMMENT,
         next_char, infile_line(infile), infile_column(infile));
     } /* end if */
    
    next_char = infile_consume_char(infile);
  } /* end while */
  
  *token = TOKEN_LINE_COMMENT;

  return next_char;
} /* end m2c_match_line_comment */


/* --------------------------------------------------------------------------
 * function m2c_match_block_comment(infile, token)
 * --------------------------------------------------------------------------
 * Matches the input  at the  current reading position  of infile  to a block
 * comment  and consumes it.  Passes the associated token  back in  out para-
 * meter token.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_block_comment (infile_t infile, m2c_token_t *token) {
  char next_char;
  uint_t nest_level;
  
  /* consume opening comment delimiter */
  next_char = infile_consume_char(infile);
  next_char = infile_consume_char(infile);
  nest_level = 1;

  while (nest_level > 0) {
    /* check for '*)' */
    if ((next_char == '*') && (infile_la2_char(infile) == ')')) {
      next_char = infile_consume_char(infile);
      next_char = infile_consume_char(infile);
      nest_level--;
    }
    /* check for '(*' */
    else if ((next_char == '(') && (infile_la2_char(infile) == '*')) {
      next_char = infile_consume_char(infile);
      next_char = infile_consume_char(infile);
      nest_level++;
    }
    /* premature EOF */
    else if (infile_eof(infile)) {
      /* emit error - premature EOF in comment */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_EOF_IN_TOKEN, infile, TOKEN_BLOCK_COMMENT,
         next_char, infile_line(infile), infile_column(infile));
      *token = TOKEN_MALFORMED_COMMENT;
      return next_char;
    }
    /* legal char */
    else if (IS_PRINTABLE_CHAR(next_char)
      || IS_LEGAL_CTRL_CHAR(next_char)) {
      next_char = infile_consume_char(infile);
    }
    /* illegal control char */
    else {
      next_char = infile_consume_char(infile);
      /* emit error - illegal control char in comment */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_ILLEGAL_CHAR_IN_TOKEN, infile, TOKEN_BLOCK_COMMENT,
         next_char, infile_line(infile), infile_column(infile));
    } /* end if */
  } /* end while */

  *token = TOKEN_BLOCK_COMMENT;

  return next_char;
} /* end m2c_match_block_comment */


/* --------------------------------------------------------------------------
 * function m2c_match_pragma(infile, token)
 * --------------------------------------------------------------------------
 * Matches the input at the  current reading position  of infile  to a pragma
 * and consumes it.  Passes the associated token back in out parameter token.
 * Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_pragma (infile_t infile, m2c_token_t *token) {
  char next_char;
  
  /* consume '<' and '*' */
  next_char = infile_consume_char(infile);
  next_char = infile_consume_char(infile);
  
  while ((next_char != '*') && (infile_la2_char(infile) != '>')) {
    next_char = infile_consume_char(infile);

    if (infile_eof(infile)) {
      /* emit error : unexpected EOF in pragma */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_EOF_IN_TOKEN, infile, TOKEN_PRAGMA,
         next_char, infile_line(infile), infile_column(infile));
      *token TOKEN_MALFORMED_PRAGMA;
      return next_char;

    /* illegal control char */
    else if (IS_ILLEGAL_CTRL_CHAR(next_char)) {
      /* emit error - illegal control char in pragma */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_ILLEGAL_CHAR_IN_TOKEN, infile, TOKEN_PRAGMA,
         next_char, infile_line(infile), infile_column(infile));
    } /* end if */
  } /* end while */

  /* consume '*' and '>' */
  next_char = infile_consume_char(infile);
  next_char = infile_consume_char(infile);
  *token = TOKEN_PRAGMA;

  return next_char;
} /* end m2c_match_pragma */


/* Disabled Code Section */

/* --------------------------------------------------------------------------
 * function m2c_match_disabled_code_block(infile)
 * --------------------------------------------------------------------------
 * Matches the input at the  current reading position of infile to a disabled
 * code block and consumes it.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_disabled_code_block (infile_t infile) {
  char next_char;

  /* consume '?' and '<' */
  next_char = infile_consume_char(infile);
  next_char = infile_consume_char(infile);
  
  while (NOT((next_char == '>')
    && (infile_column(infile) == 1)
    && (infile_la2_char(infile) == '?'))) {
    
    /* since this is a debugging aid, all input is ignored, legal or not */

    if (infile_eof(infile)) {
      return next_char;
    } /* end if */

    next_char = infile_consume_char(infile);
  } /* end while */
  
  /* consume '>' and '?' */
  next_char = infile_consume_char(infile);
  next_char = infile_consume_char(infile);

  return next_char;
} /* end m2c_match_disabled_code_block */


/* Private Procedures */

/* --------------------------------------------------------------------------
 * private function match_decimal_number_tail(infile)
 * --------------------------------------------------------------------------
 * Matches the input in infile to a decimal number tail, returns lookahead.
 *
 * EBNF
 *
 * DecimalNumberTail :=
 *   DigitSep? DigitSeq RealNumberTail?
 *   ;
 *
 * alias DigitSep = "'" ;
 *
 * pre-conditions:
 *  (1) infile is the current input file and it must not be NIL.
 *  (2) lookahead of infile is a digit between 1 and 9 or a decimal point.
 *
 * post-conditions:
 *  (1) lookahead of infile is the character immediately following the last
 *      digit of the literal whose first digit was the lookahead of infile
 *      upon entry into the procedure.
 *
 * error-conditions:
 *  (1) illegal character encountered
 *       TO DO
 * ----------------------------------------------------------------------- */

#define DIGIT_SEPARATOR '\''

char match_decimal_number_tail (infile_t infile, m2c_token_t *token) {
  char next_char;
  
  next_char = infile_consume_char(infile);
  
  /* DigitSep? */
  if (next == DIGIT_SEPARATOR) {
    next_char = infile_consume_char(infile);
  } /* end if */
  
  /* DigitSeq */
  if (IS_DIGIT(next_char)) {
    next_char := match_digit_seq(infile, token);
  }
  else /* lookahead is not a decimal digit */ {
    /* emit error -- missing digit after digit separator in number */
    m2c_emit_lex_error_in_token
      (M2C_ERROR_MISSING_DIGIT_AFTER_DSEP, infile, TOKEN_WHOLE_NUMBER,
       next_char, infile_line(infile), infile_column(infile));
  } /* end if */
  
  /* RealNumberTail? */
  if (next_char == '.') {
    next_char = match_real_number_tail(infile, token);
  } /* end if */
  
  return next_char;
} /* end match_decimal_number_tail */


/* --------------------------------------------------------------------------
 * private function match_real_number_tail(infile, token)
 * --------------------------------------------------------------------------
 * Matches the input in infile to a real number tail, returns lookahead.
 *
 * EBNF
 *
 * RealNumberTail :=
 *   '.' DigitSeq ( 'e' ( '+' | '-' )? DigitSeq )?
 *   ;
 *
 * pre-conditions:
 *  (1) infile is the current input file and it must not be NIL.
 *  (2) lookahead of infile is a decimal point.
 *
 * post-conditions:
 *  (1) lookahead of infile is the character immediately following the last
 *      digit of the literal whose decimal point was the lookahead of infile
 *      upon entry into the procedure.
 *
 * error-conditions:
 *  (1) illegal character encountered
 *       TO DO
 * ----------------------------------------------------------------------- */

static char match_real_Number_tail (infile_t infile, m2c_token_t *token) {
  char next_char;
    
  /* '.' */
  next_char := infile_consume_char(infile);
  *token = TOKEN_REAL_NUMBER;
  
  /* DigitSeq */
  if (IS_DIGIT(next_char)) {
    next_char = match_digit_seq(infile, token);
  }
  else /* lookahead is not a decimal digit */ {
    /* emit error - missing digit after decimal point in real number */
    m2c_emit_lex_error_in_token
      (M2C_ERROR_MISSING_DIGIT_AFTER_DP, infile, TOKEN_REAL_NUMBER,
       next_char, infile_line(infile), infile_column(infile));
  } /* end if */
  
  /* ('e' ( '+' | '-' )? DigitSeq)? */
  if (next_char == 'e') {
    /* consume 'e' */
    next_char = infile_consume_char(infile);
    
    /* ( '+' | '-' )?  */
    if ((next_char == '+') || (next_char == '-')) {
      /* consume sign */
      next_char = infile_consume_char(infile);
    } /* end if */
    
    /* DigitSeq */
    if (IS_DIGIT(next_char)) {
      next_char = match_digit_seq(infile, token);
    }
    else /* lookahead is not a decimal digit */ {
      /* emit error - missing exponent in real number */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_MISSING_EXPONENT_AFTER_E, infile, TOKEN_REAL_NUMBER,
         next_char, infile_line(infile), infile_column(infile));
    } /* end if */
  } /* end if */
  
  return next_char;
} /* end match_real_Number_tail */


/* --------------------------------------------------------------------------
 * private function match_digit_seq(infile, token)
 * --------------------------------------------------------------------------
 * Matches input in infile to a decimal digit sequence, returns lookahead.
 *
 * EBNF
 *
 * matchDigitSeq :=
 *   Digit+ ( DigitSep Digit+ )*
 *   ;
 *
 * pre-conditions:
 *  (1) infile is the current input file and it must not be NIL.
 *  (2) lookahead of infile is a decimal digit.
 *
 * post-conditions:
 *  (1) lookahead of infile is the character immediately following the last
 *      digit of the literal whose first digit was the lookahead of infile
 *      upon entry into the procedure.
 *
 * error-conditions:
 *  (1) illegal character encountered
 *       TO DO
 * ----------------------------------------------------------------------- */

static char match_digit_seq (infile_t infile, m2c_token_t *token) {
  char next_char;
    
  /* Digit */
  next_char = infile_consume_char(infile);
  
  /* Digit* */
  while (IS_DIGIT(next_char)) {
    next_char = infile_consume_char(infile);
  } /* end while */
  
  /* ( DigitSep Digit+ )? */
  if (next_char = DIGIT_SEPARATOR) {
    next_char = infile_consume_char(infile);
    
    /* Digit */
    if (IS_DIGIT(next_char)) {
      next_char = infile_consume_char(infile);
      
      /* Digit* */
      while (IS_DIGIT(next_char)) {
        next_char = infile_consume_char(infile);
      } /* end while */

    else /* lookahead not a decimal digit */ {
      /* emit error - missing digit after digit separator in number */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_MISSING_DIGIT_AFTER_DSEP, infile, *token,
         next_char, infile_line(infile), infile_column(infile));
    } /* end if */
  } /* end if */
  
  return next_char;
} /* end match_digit_seq */


/* --------------------------------------------------------------------------
 * private function match_base2_digit_seq(infile, token)
 * --------------------------------------------------------------------------
 * Matches the input in infile to a base-2 digit sequence, returns lookahead.
 *
 * EBNF
 *
 * Base2DigitSeq :=
 *   Base2Digit+ ( DigitSep Base2Digit+ )*
 *   ;
 *
 * pre-conditions:
 *  (1) infile is the current input file and it must not be NIL.
 *  (2) lookahead of infile is a base-2 digit.
 *
 * post-conditions:
 *  (1) lookahead of infile is the character immediately following the last
 *      digit of the literal whose first digit was the lookahead of infile
 *      upon entry into the procedure.
 *
 * error-conditions:
 *  (1) illegal character encountered
 *       TO DO
 * ----------------------------------------------------------------------- */

static char match_base2_digit_seq (infile_t infile, m2c_token_t *token) {
  char next_char;
  
  /* Base2Digit */
  next_char = infile_consume_char(infile);
  
  /* Base2Digit* */
  while ((next_char == '0') || (next_char == '1')) {
    next_char = infile_consume_char(infile);
  } /* end while */
  
  /* ( DigitSep Base2Digit+ )? */
  if (next_char == DIGIT_SEPARATOR) {
    next_char = infile_consume_char(infile);
    
    /* Base2Digit */
    if ((next_char == '0') || (next_char == '1')) {
      next_char = infile_consume_char(infile);
      
      /* Base2Digit* */
      while ((next_char == '0') || (next_char == '1')) {
        next_char = infile_consume_char(infile);
      } /* end while */

    else /* lookahead not a base-2 digit */ {
      /* emit error - missing digit after digit separator in number */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_MISSING_DIGIT_AFTER_DSEP, infile, TOKEN_WHOLE_NUMBER,
         next_char, infile_line(infile), infile_column(infile));
    } /* end if */
  } /* end if */
  
  return next_char;
} /* end match_base2_digit_seq */


/* --------------------------------------------------------------------------
 * private function match_base16_digit_seq(infile, token)
 * --------------------------------------------------------------------------
 * Matches input in infile to a base-16 digit sequence, returns lookahead.
 *
 * EBNF
 *
 * Base16DigitSeq :=
 *   Base16Digit+ ( DigitSep Base16Digit+ )*
 *   ;
 *
 * pre-conditions:
 *  (1) infile is the current input file and it must not be NIL.
 *  (2) lookahead of infile is a base-16 digit.
 *
 * post-conditions:
 *  (1) lookahead of infile is the character immediately following the last
 *      digit of the literal whose first digit was the lookahead of infile
 *      upon entry into the procedure.
 *
 * error-conditions:
 *  (1) illegal character encountered
 *       TO DO
 * ----------------------------------------------------------------------- */

static char match_base16_digit_seq (infile_t infile, m2c_token_t *token) {
  char next_char;
    
  /* Base16Digit */
  next_char = infile_consume_char(infile);
  
  /* Base16Digit* */
  while (IS_DIGIT(next_char) || IS_A_TO_F(next_char)) {
    next_char = infile_consume_char(infile);
  } /* end while */
  
  /* ( DigitSep Base16Digit+ )? */
  if (next_char == DIGIT_SEPARATOR) {
    next_char = infile_consume_char(infile);
    
    /* Base16Digit */
    if (IS_DIGIT(next_char) || IS_A_TO_F(next_char)) {
      next_char = infile_consume_char(infile);
      
      /* Base16Digit* */
      while (IS_DIGIT(next_char) || IS_A_TO_F(next_char)) {
        next_char = infile_consume_char(infile);
        } /* end while */

    else /* lookahead not a base-16 digit */ {
      /* emit error - missing digit after digit separator in number */
      m2c_emit_lex_error_in_token
        (M2C_ERROR_MISSING_DIGIT_AFTER_DSEP, infile, TOKEN_WHOLE_NUMBER,
         next_char, infile_line(infile), infile_column(infile));
    } /* IF */
  } /* IF */
  
  return next_char;
} /* end match_base16_digit_seq */


/* END OF FILE */