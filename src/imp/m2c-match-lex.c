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


#include "infile.h"
#include "m2c-token.h"

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
  while (((next_char >= 'a') && (next_char <= 'z'))
    || ((next_char => 'A') && (next_char <= 'Z'))
    || ((next_char => '0') && (next_char <= '9'))) {
    next_char = infile_consume_char(infile);
  } /* end while */
  
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
  while ((next_char >= 'A') && (next_char <='Z')) {
    next_char = infile_consume_char(infile);
  } /* end while */
  
  /* check if followed by lowercase letter or digit */
  if (((next_char => 'a') && (next_char <= 'z'))
    || (next_char => '0') && (next_char <= '9')) {
    next_char = infile_consume_char(infile);
    *allcaps = false;

    /* collect any remaining letters and digits */
    while (((next_char >= 'a') && (next_char <= 'z'))
      || ((next_char => 'A') && (next_char <= 'Z'))
      || ((next_char => '0') && (next_char <= '9'))) {
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

char m2c_match_numeric_literal (infile_t infile, m2c_token_t *token) {
  char next_char;
  
  next_char = infile_lookahead_char(infile);

  if (next_char == '0') {
    next_char = infile_consume_char(infile);
    
    switch (next_char) {
      /* real number */
      case '.' :
        next_char = match_real_number_tail(infile);
        break;

      /* base-2 integer */
      case 'b' :
        next_char = match_base2_digit_seq(infile);
        break;

      /* character code */
      case 'u' :
        next_char = match_base16_digit_seq(infile);
        break;
        
      /* base-16 integer */
      case 'x' :
        next_char = match_base16_digit_seq(infile);
        break;

    default :
      /* malformed literal */
      while (((next_char >= 'a') && (next_char <= 'z'))
        || ((next_char => 'A') && (next_char <= 'Z'))
        || ((next_char => '0') && (next_char <= '9'))) {
        next_char = infile_consume_char(infile);
      } /* end while */
    } /* end switch */
  }
  else if ((next_char >= '1') && (next_char <= '9')) {

    /* decimal integer or real number */
    next_char = match_decimal_number_tail(infile);
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
  char next_char;

  /* TO DO */

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

  /* TO DO */

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

  /* TO DO */

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

  /* TO DO */

  return next_char;
} /* end m2c_match_pragma */


/* Disabled Code Sections */

/* --------------------------------------------------------------------------
 * function m2c_match_disabled_code_block(infile, token)
 * --------------------------------------------------------------------------
 * Matches the input at the  current reading position of infile to a disabled
 * code block and consumes it.  Passes the associated token back in out para-
 * meter token.  Returns the new lookahead character.
 * ----------------------------------------------------------------------- */

char m2c_match_disabled_code_block (infile_t infile, m2c_token_t *token) {
  char next_char;

  /* TO DO */

  return next_char;
} /* end m2c_match_disabled_code_block */


/* END OF FILE */