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
 * m2c-token.h
 *
 * Public interface of M2C token type.
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

#ifndef M2C_TOKEN_H
#define M2C_TOKEN_H

#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_token_t
 * --------------------------------------------------------------------------
 * Enumerated token values representing Modula-2 terminal symbols.
 * --------------------------------------------------------------------------
 */

typedef enum {
  /* Null Token */
  
  TOKEN_INVALID,              /* 0 */
  
  /* Reserved Words */

  TOKEN_ALIAS,                /* 1 */
  TOKEN_AND,                  /* 2 */   /* operator */
  TOKEN_ARGLIST,              /* 3 */
  TOKEN_ARRAY,                /* 4 */
  TOKEN_BEGIN,                /* 5 */
  TOKEN_CASE,                 /* 6 */
  TOKEN_CONST,                /* 7 */
  TOKEN_COPY,                 /* 8 */
  TOKEN_DEFINITION,           /* 9 */
  TOKEN_DIV,                 /* 10 */   /* operator */
  TOKEN_DO,                  /* 11 */
  TOKEN_ELSE,                /* 12 */
  TOKEN_ELSIF,               /* 13 */
  TOKEN_END,                 /* 14 */
  TOKEN_EXIT,                /* 15 */
  TOKEN_FOR,                 /* 16 */
  TOKEN_IF,                  /* 17 */
  TOKEN_IMPLEMENTATION,      /* 18 */
  TOKEN_IMPORT,              /* 19 */
  TOKEN_IN,                  /* 20 */   /* operator */
  TOKEN_LOOP,                /* 21 */
  TOKEN_MOD,                 /* 22 */   /* operator */
  TOKEN_MODULE,              /* 23 */
  TOKEN_NEW,                 /* 24 */
  TOKEN_NOP,                 /* 25 */
  TOKEN_NOT,                 /* 26 */   /* operator */
  TOKEN_OCTETSEQ,            /* 27 */
  TOKEN_OF,                  /* 28 */
  TOKEN_OPAQUE,              /* 29 */
  TOKEN_OR,                  /* 30 */   /* operator */
  TOKEN_POINTER,             /* 31 */
  TOKEN_PROCEDURE,           /* 32 */
  TOKEN_READ,                /* 33 */
  TOKEN_RECORD,              /* 34 */
  TOKEN_RELEASE,             /* 35 */
  TOKEN_REPEAT,              /* 36 */
  TOKEN_RETAIN,              /* 37 */
  TOKEN_RETURN,              /* 38 */
  TOKEN_SET,                 /* 39 */
  TOKEN_THEN,                /* 40 */
  TOKEN_TO,                  /* 41 */
  TOKEN_TYPE,                /* 42 */
  TOKEN_UNQUALIFIED,         /* 43 */
  TOKEN_UNTIL,               /* 44 */
  TOKEN_VAR,                 /* 45 */
  TOKEN_WHILE,               /* 46 */
  TOKEN_WRITE,               /* 47 */
  
  /* Identifiers */
  
  TOKEN_STDIDENT,            /* 48 */
  TOKEN_PRIMITIVE,           /* 49 */
  
  /* Literals */
  
  TOKEN_WHOLE_NUMBER,        /* 50 */
  TOKEN_REAL_NUMBER,         /* 51 */
  TOKEN_CHAR_CODE,           /* 52 */
  TOKEN_QUOTED_STRING,       /* 53 */

  /* Punctuation */
  
  TOKEN_COMMA,               /* 54 */
  TOKEN_COLON,               /* 55 */
  TOKEN_SEMICOLON,           /* 56 */
  TOKEN_AT_SIGN,             /* 57 */
  TOKEN_VERTICAL_BAR,        /* 58 */
  TOKEN_DOT_DOT,             /* 59 */
  TOKEN_ASSIGN,              /* 60 */
  TOKEN_PLUS_PLUS,           /* 61 */
  TOKEN_MINUS_MINUS,         /* 62 */
  TOKEN_DOT_STAR,            /* 63 */

  /* Paired Delimiters */
  
  TOKEN_LPAREN,              /* 64 */
  TOKEN_RPAREN,              /* 65 */
  TOKEN_LBRACKET,            /* 66 */
  TOKEN_RPRACKET,            /* 67 */
  TOKEN_LBRACE,              /* 68 */
  TOKEN_RBRACE,              /* 69 */

  /* Operators */

  /* Non-Resword Level-1 Operators */

  TOKEN_EQUAL,               /* 70 */   /* also used as punctuation */
  TOKEN_NOT_EQUAL,           /* 71 */
  TOKEN_LESS,                /* 72 */
  TOKEN_LESS_OR_EQ,          /* 73 */
  TOKEN_GREATER,             /* 74 */
  TOKEN_GREATER_OR_EQ,       /* 75 */
  TOKEN_IDENTITY,            /* 76 */

  /* Non-Resword Level-2 Operators */
  
  TOKEN_PLUS,                /* 77 */   /* also used as punctuation */
  TOKEN_MINUS,               /* 78 */
  TOKEN_CONCAT,              /* 79 */
  TOKEN_SET_DIFF,            /* 80 */

  /* Non-Resword Level-3 Operators */

  TOKEN_ASTERISK,            /* 81 */   /* also used as punctuation */
  TOKEN_REAL_DIV,            /* 82 */

  /* Non-Resword Level-4 Operators */

  /* none */

  /* Non-Resword Level-5 Operators */

  TOKEN_TYPE_CONV,           /* 83 */

  /* Non-Resword Level-6 Operators */

  TOKEN_DOT,                 /* 84 */   /* also used as punctuation */
  TOKEN_DEREF,               /* 85 */

  /* End Of File Marker */

  TOKEN_EOF,                 /* 86 */
  
  /* Non-Semantic Symbols */

  /* Comments and Pragmas */
  
  TOKEN_LINE_COMMENT,        /* 87 */
  TOKEN_BLOCK_COMMENT,       /* 88 */
  TOKEN_PRAGMA,              /* 89 */
  
  /* Malformed Symbols */
  
  TOKEN_MALFORMED_STRING,    /* 90 */   /* string without closing delimiter */
  TOKEN_MALFORMED_INTEGER,   /* 91 */   /* base-16 number without suffix H */
  TOKEN_MALFORMED_REAL,      /* 92 */   /* real number with E but no exponent */
  
  /* Enumeration Terminator */
  
  TOKEN_END_MARK             /* 93 */   /* marks the end of the enumeration */
} m2c_token_t;


/* --------------------------------------------------------------------------
 * convenience aliases
 * ----------------------------------------------------------------------- */

#define TOKEN_LESS_OR_EQUAL TOKEN_LESS_OR_EQ
#define TOKEN_GREATER_OR_EQUAL TOKEN_GREATER_OR_EQ


/* --------------------------------------------------------------------------
 * first and last reserved word tokens
 * ----------------------------------------------------------------------- */

#define FIRST_RESERVED_WORD_TOKEN TOKEN_ALIAS
#define LAST_RESERVED_WORD_TOKEN TOKEN_WRITE


/* --------------------------------------------------------------------------
 * first and last semantic tokens
 * ----------------------------------------------------------------------- */

#define FIRST_SEMANTIC_TOKEN TOKEN_FIRST_RESERVED_WORD
#define LAST_SEMANTIC_TOKEN TOKEN_EOF


/* --------------------------------------------------------------------------
 * function m2c_is_valid_token(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a terminal symbol, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_is_valid_token (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_resword(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a reserved word, otherwise true.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_resword (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_identifier(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents an identifier, otherwise true.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_identifier (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_number(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a number literal, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_number (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_const_expr_literal(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a const expression literal, else false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_const_expr_literal (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_oper_l1(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-1 operator, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_oper_l1 (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_oper_l2(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-2 operator, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_oper_l2 (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_oper_l3(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-3 operator, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_oper_l3 (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_comment(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a comment, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_comment (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_pragma(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a pragma, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_pragma (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_is_malformed_literal(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a malformed literal, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_token_is_malformed_literal (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_token_for_resword(lexeme, length)
 * --------------------------------------------------------------------------
 * Tests if the given lexeme represents a reserved word and returns the
 * corresponding token or TOKEN_UNKNOWN if it does not match a reserved word.
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_token_for_resword (const char *lexeme, uint_t length);


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_resword(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with
 * the lexeme for the reserved word represented by token.  Returns NULL
 * if the token does not represent a reserved word.
 * ----------------------------------------------------------------------- */

const char *m2c_lexeme_for_resword (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_special_symbol(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with
 * the lexeme for the special symbol represented by token.  Returns NULL
 * if the token does not represent a special symbol.
 * ----------------------------------------------------------------------- */

const char *m2c_lexeme_for_special_symbol (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_name_for_token(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with
 * a human readable name for token.  Returns NULL token is not a valid token.
 * ----------------------------------------------------------------------- */

const char *m2c_name_for_token (m2c_token_t token);


#endif /* M2C_TOKEN_H */

/* END OF FILE */
