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
 * m2c-token.c                                                               *
 *                                                                           *
 * Implementation of M2C token module.                                       *
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

#include "m2c-token.h"

#include <stddef.h> /* NULL */


/* --------------------------------------------------------------------------
 * hidden array token_name_table
 * --------------------------------------------------------------------------
 * Human readable names for tokens.
 * ----------------------------------------------------------------------- */

static const char *token_name_table[] = {
  /* Null Token */
  
  "UNKNOWN",
  
  /* Reserved Words */

  "RW-ALIAS\0",
  "RW-AND\0",
  "RW-ARGLIST\0",
  "RW-ARRAY\0",
  "RW-BEGIN\0",
  "RW-CASE\0",
  "RW-CONST\0",
  "RW-COPY\0",
  "RW-DEFINITION\0",
  "RW-DIV\0",
  "RW-DO\0",
  "RW-ELSE\0",
  "RW-ELSIF\0",
  "RW-END\0",
  "RW-EXIT\0",
  "RW-FOR\0",
  "RW-IF\0",
  "RW-IMPLEMENTATION\0",
  "RW-IMPORT\0",
  "RW-IN\0",
  "RW-LOOP\0",
  "RW-MOD\0",
  "RW-MODULE\0",
  "RW-NEW\0",
  "RW-NOP\0",
  "RW-NOT\0",
  "RW-OCTETSEQ\0",
  "RW-OF\0",
  "RW-OPAQUE\0",
  "RW-OR\0",
  "RW-POINTER\0",
  "RW-PROCEDURE\0",
  "RW-READ\0",
  "RW-RECORD\0",
  "RW-RELEASE\0",
  "RW-REPEAT\0",
  "RW-RETAIN\0",
  "RW-RETURN\0",
  "RW-SET\0",
  "RW-THEN\0",
  "RW-TO\0",
  "RW-TYPE\0",
  "RW-UNQUALIFIED\0",
  "RW-UNTIL\0",
  "RW-VAR\0",
  "RW-WHILE\0",
  "RW-WRITE\0",
  
  /* Identifiers */
  
  "IDENTIFIER\0",
  
  /* Literals */
  
  "INTEGER-LITERAL\0",
  "REAL-LITERAL\0",
  "CHAR-CODE-LITERAL\0",
  "STRING-LITERAL\0",
  
  /* Punctuation */
  
  "COMMA\0",
  "COLON\0",
  "SEMICOLON\0",
  "AT-SIGN\0",
  "BAR\0",
  "DOT-DOT\0",
  "ASSIGN\0",
  "PLUS-PLUS\0",
  "MINUS-MINUS\0",
  "WILDCARD\0",
  
  /* Paired Delimiters */

  "LEFT-PAREN\0",
  "RIGHT-PAREN\0",
  "LEFT-BRACKET\0",
  "RIGHT-BRACKET\0",
  "LEFT-BRACE\0",
  "RIGHT-BRACE\0",
  
  /* Operators */
  
  /* Non-Resword Level-1 Operators */
  
  "EQUAL\0",
  "NOT-EQUAL\0",
  "LESS\0",
  "LESS-OR-EQUAL\0",
  "GREATER\0",
  "GREATER-OR-EQUAL\0",
  "IDENTITY\0",
  
  /* Non-Resword Level-2 Operators */
  
  "PLUS\0",
  "MINUS\0",
  "CONCAT\0",
  "SET-DIFF\0",

  /* Non-Resword Level-3 Operators */
  
  "ASTERISK\0",
  "SOLIDUS\0",
  
  /* Non-Resword Level-4 Operators */
  
  /* none */
  
  /* Non-Resword Level-5 Operators */
  
  "TYPE-CONV\0",
  
  /* Non-Resword Level-6 Operators */
  
  "DOT\0",
  "DEREF\0",
  
  /* End Of File Marker */
  
  "END-OF-FILE\0",
  
  /* Non-Semantic Symbols */
  
  /* Comments and Pragmas */
  
  "LINE-COMMENT\0",
  "BLOCK-COMMENT\0",
  "PRAGMA\0",
  
  /* Malformed Symbols */
    
  "MALFORMED-INTEGER\0",
  "MALFORMED-REAL\0",
  "MALFORMED-CHAR-CODE\0",
  "MALFORMED-STRING\0",
  
  "MALFORMED-COMMENT\0",
  "MALFORMED-PRAGMA\0",
  
  /* out-of-range guard */
  
  "\0"

}; /* end token_name_table */


/* --------------------------------------------------------------------------
 * hidden array special_symbol_lexeme_table
 * --------------------------------------------------------------------------
 * Lexeme strings for special symbol tokens.
 * ----------------------------------------------------------------------- */

static const char *special_symbol_lexeme_table[] = {
  
  /* Punctuation */
  
  ",\0",   /* COMMA */
  ":\0",   /* COLON */
  ";\0",   /* SEMICOLON */
  "@\0",   /* AT-SIGN */
  "|\0",   /* BAR */
  "..\0",  /* DOT-DOT */
  ":=\0",  /* ASSIGN */
  "++\0",  /* PLUS-PLUS */
  "--\0",  /* MINUS-MINUS */
  ".*\0",  /* WILDCARD */
  
  /* Paired Delimiters */
  
  "(\0",   /* LEFT-PAREN */
  ")\0",   /* RIGHT-PAREN */
  "[\0",   /* LEFT-BRACKET */
  "]\0",   /* RIGHT-BRACKET */
  "{\0",   /* LEFT-BRACE */
  "}\0",   /* RIGHT-BRACE */
  
  /* Operators */
  
  /* Non-Resword Level-1 Operators */
  
  "=\0",   /* EQUAL */
  "#\0",   /* NOT-EQUAL */
  "<\0",   /* LESS */
  "<=\0",  /* LESS-OR-EQUAL */
  ">\0",   /* GREATER */
  ">=\0",  /* GREATER-OR-EQUAL */
  "==\0",  /* IDENTITY */
  
  /* Non-Resword Level-2 Operators */
  
  "+\0",   /* PLUS */
  "-\0",   /* MINUS */
  "&\0",   /* CONCAT */
  "\\\0",  /* SET-DIFF */
  
  /* Non-Resword Level-3 Operators */

  "*\0",   /* ASTERISK */
  "/\0",   /* SOLIDUS */
  
  /* Non-Resword Level-4 Operators */
  
  /* none */
  
  /* Non-Resword Level-5 Operators */
  
  "::\0",  /* TYPE-CONV */
  
  /* Non-Resword Level-6 Operators */
  
  ".\0",   /* DOT */
  "^\0"    /* DEREF */
  
}; /* end special_symbol_lexeme_table */


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_resword(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with the
 * lexeme of the resword represented by token, or NULL if token is not valid.
 * ----------------------------------------------------------------------- */

const char *m2c_lexeme_for_resword (m2c_token_t token) {
  const char *lexeme;
  
  if (M2C_IS_RESWORD_TOKEN(token)) {
    lexeme = token_name_table[token];
    lexeme++; lexeme++; lexeme++;
    return lexeme;
  }
  else /* invalid token */ {
    return NULL;
  } /* end if */
} /* end m2c_name_for_token */


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_special_symbol(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with the
 * lexeme for the special symbol represented by token.  Returns NULL if token
 * does not represent a special symbol.
 * ----------------------------------------------------------------------- */

const char *m2c_lexeme_for_special_symbol (m2c_token_t token) {
  if (M2C_IS_SPECIAL_SYMBOL_TOKEN(token)) {
    return special_symbol_lexeme_table[token];
  }
  else /* not a special symbol token */ {
    return NULL;
  } /* end if */
} /* end m2c_lexeme_for_special_symbol */


/* --------------------------------------------------------------------------
 * function m2c_name_for_token(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with
 * a human readable name for token.  Returns NULL token is not a valid token.
 * ----------------------------------------------------------------------- */

const char *m2c_name_for_token (m2c_token_t token) {
  if (token < TOKEN_END_MARK) {
    return token_name_table[token];
  }
  else /* invalid token */ {
    return NULL;
  } /* end if */
} /* end m2c_name_for_token */


/* END OF FILE */