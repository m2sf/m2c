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
 * m2c-token.h                                                               *
 *                                                                           *
 * Public interface of M2C token module.                                     *
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

#ifndef M2C_TOKEN_H
#define M2C_TOKEN_H


/* --------------------------------------------------------------------------
 * type m2c_token_t
 * --------------------------------------------------------------------------
 * Enumerated token values representing Modula-2 terminal symbols.
 * --------------------------------------------------------------------------
 */

typedef enum {
  /* Null Token */
  
  TOKEN_UNKNOWN,              /* 0 */
  
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
  
  TOKEN_IDENT,               /* 48 */
  
  /* Literals */
  
  TOKEN_WHOLE_NUMBER,        /* 49 */
  TOKEN_REAL_NUMBER,         /* 50 */
  TOKEN_CHAR_CODE,           /* 51 */
  TOKEN_QUOTED_STRING,       /* 52 */

  /* Punctuation */
  
  TOKEN_COMMA,               /* 53 */
  TOKEN_COLON,               /* 54 */
  TOKEN_SEMICOLON,           /* 55 */
  TOKEN_AT_SIGN,             /* 56 */
  TOKEN_BAR,                 /* 57 */
  TOKEN_DOT_DOT,             /* 58 */
  TOKEN_ASSIGN,              /* 59 */
  TOKEN_PLUS_PLUS,           /* 60 */
  TOKEN_MINUS_MINUS,         /* 61 */
  TOKEN_WILDCARD,            /* 62 */

  /* Paired Delimiters */
  
  TOKEN_LPAREN,              /* 63 */
  TOKEN_RPAREN,              /* 64 */
  TOKEN_LBRACKET,            /* 65 */
  TOKEN_RPRACKET,            /* 66 */
  TOKEN_LBRACE,              /* 67 */
  TOKEN_RBRACE,              /* 68 */

  /* Operators */

  /* Non-Resword Level-1 Operators */

  TOKEN_EQUAL,               /* 69 */   /* also used as punctuation */
  TOKEN_NOT_EQUAL,           /* 70 */
  TOKEN_LESS,                /* 71 */
  TOKEN_LESS_OR_EQ,          /* 72 */
  TOKEN_GREATER,             /* 73 */
  TOKEN_GREATER_OR_EQ,       /* 74 */
  TOKEN_IDENTITY,            /* 75 */

  /* Non-Resword Level-2 Operators */
  
  TOKEN_PLUS,                /* 76 */   /* also used as punctuation */
  TOKEN_MINUS,               /* 77 */
  TOKEN_CONCAT,              /* 78 */
  TOKEN_SET_DIFF,            /* 79 */

  /* Non-Resword Level-3 Operators */

  TOKEN_ASTERISK,            /* 80 */   /* also used as punctuation */
  TOKEN_SOLIDUS,             /* 81 */

  /* Non-Resword Level-4 Operators */

  /* none */

  /* Non-Resword Level-5 Operators */

  TOKEN_TYPE_CONV,           /* 82 */

  /* Non-Resword Level-6 Operators */

  TOKEN_DOT,                 /* 83 */   /* also used as punctuation */
  TOKEN_DEREF,               /* 84 */

  /* End Of File Marker */

  TOKEN_EOF,                 /* 85 */
  
  /* Non-Semantic Symbols */

  /* Comments and Pragmas */
  
  TOKEN_LINE_COMMENT,        /* 86 */
  TOKEN_BLOCK_COMMENT,       /* 87 */
  TOKEN_PRAGMA,              /* 88 */
  
  /* Malformed Symbols */
  
  TOKEN_MALFORMED_INTEGER,   /* 89 */
  TOKEN_MALFORMED_REAL,      /* 90 */
  TOKEN_MALFORMED_CHAR_CODE, /* 91 */
  TOKEN_MALFORMED_STRING,    /* 92 */
  
  TOKEN_MALFORMED_COMMENT,   /* 93 */
  TOKEN_MALFORMED_PRAGMA,    /* 94 */
  
  /* Enumeration Terminator */
  
  TOKEN_END_MARK             /* 95 */   /* marks the end of the enumeration */
} m2c_token_t;


/* --------------------------------------------------------------------------
 * convenience aliases
 * ----------------------------------------------------------------------- */

#define TOKEN_PERIOD TOKEN_DOT
#define TOKEN_LESS_OR_EQUAL TOKEN_LESS_OR_EQ
#define TOKEN_GREATER_OR_EQUAL TOKEN_GREATER_OR_EQ


/* --------------------------------------------------------------------------
 * token group boundaries
 * ----------------------------------------------------------------------- */

#define FIRST_SEMANTIC_TOKEN (TOKEN_UNKNOWN + 1)
#define LAST_SEMANTIC_TOKEN TOKEN_EOF

#define FIRST_RESWORD_TOKEN TOKEN_ALIAS
#define LAST_RESWORD_TOKEN TOKEN_WRITE

#define FIRST_LITERAL_TOKEN TOKEN_WHOLE_NUMBER
#define LAST_LITERAL_TOKEN TOKEN_QUOTED_STRING

#define FIRST_NUMBER_LITERAL_TOKEN TOKEN_WHOLE_NUMBER
#define LAST_NUMBER_LITERAL_TOKEN TOKEN_REAL_NUMBER

#define FIRST_SPECIAL_SYMBOL_TOKEN TOKEN_COMMA
#define LAST_SPECIAL_SYMBOL_TOKEN TOKEN_DEREF

#define FIRST_NON_RESWORD_OPER_L1_TOKEN TOKEN_EQUAL
#define LAST_NON_RESWORD_OPER_L1_TOKEN TOKEN_IDENTITY

#define FIRST_NON_RESWORD_OPER_L2_TOKEN TOKEN_PLUS
#define LAST_NON_RESWORD_OPER_L2_TOKEN TOKEN_SET_DIFF

#define FIRST_NON_RESWORD_OPER_L3_TOKEN TOKEN_ASTERISK
#define LAST_NON_RESWORD_OPER_L3_TOKEN TOKEN_SOLIDUS

#define FIRST_NON_RESWORD_OPER_L6_TOKEN TOKEN_DOT
#define LAST_NON_RESWORD_OPER_L6_TOKEN TOKEN_DEREF

#define FIRST_NON_SEMANTIC_TOKEN TOKEN_LINE_COMMENT
#define LAST_NON_SEMANTIC_TOKEN TOKEN_PRAGMA

#define FIRST_ERROR_TOKEN TOKEN_MALFORMED_INTEGER
#define LAST_ERROR_TOKEN TOKEN_MALFORMED_PRAGMA

#define FIRST_MALFORMED_LITERAL_TOKEN TOKEN_MALFORMED_INTEGER
#define LAST_MALFORMED_LITERAL_TOKEN TOKEN_MALFORMED_STRING


/* --------------------------------------------------------------------------
 * macro M2C_IS_VALID_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a terminal symbol, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_VALID_TOKEN(_token) \
  ((_token > TOKEN_UNKNOWN) && (_token < TOKEN_END_MARK))


/* --------------------------------------------------------------------------
 * macro M2C_IS_SEMANTIC_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a semantic symbol, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_SEMANTIC_TOKEN(_token) \
  ((_token >= FIRST_SEMANTIC_TOKEN) && (_token <= LAST_SEMANTIC_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_RESWORD_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a reserved word, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_RESWORD_TOKEN(_token) \
  ((_token >= FIRST_RESWORD_TOKEN) && (_token <= LAST_RESWORD_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_LITERAL_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a literal, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_LITERAL_TOKEN(_token) \
  ((_token >= FIRST_LITERAL_TOKEN) && (_token <= LAST_LITERAL_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_NUMBER_LITERAL_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a number literal, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_NUMBER_LITERAL_TOKEN(_token) \
  ((_token >= FIRST_NUMBER_LITERAL_TOKEN) \
  && (_token <= LAST_NUMBER_LITERAL_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_SPECIAL_SYMBOL_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a special symbol, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_SPECIAL_SYMBOL_TOKEN(_token) \
  ((_token >= FIRST_SPECIAL_SYMBOL_TOKEN) \
  && (_token <= LAST_SPECIAL_SYMBOL_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_OPER_L1_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-1 operator, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_OPER_L1_TOKEN(_token) \
  (((_token >= FIRST_NON_RESWORD_OPER_L1_TOKEN) \
  && (_token <= LAST_NON_RESWORD_OPER_L1_TOKEN)) \
  || (_token == TOKEN_IN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_OPER_L2_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-2 operator, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_OPER_L2_TOKEN(_token) \
  (((_token >= FIRST_NON_RESWORD_OPER_L2_TOKEN) \
  && (_token <= LAST_NON_RESWORD_OPER_L2_TOKEN)) \
  || (_token == TOKEN_OR))


/* --------------------------------------------------------------------------
 * macro M2C_IS_OPER_L3_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-3 operator, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_OPER_L3_TOKEN(_token) \
  (((_token >= FIRST_NON_RESWORD_OPER_L3_TOKEN) \
  && (_token <= LAST_NON_RESWORD_OPER_L3_TOKEN)) \
  || (_token == TOKEN_AND) || (_token == TOKEN_DIV) || (_token == TOKEN_MOD))


/* --------------------------------------------------------------------------
 * macro M2C_IS_OPER_L4_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-4 operator, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_OPER_L4_TOKEN(_token) \
  (_token == TOKEN_NOT)


/* --------------------------------------------------------------------------
 * macro M2C_IS_OPER_L4_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-4 operator, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_OPER_L5_TOKEN(_token) \
  (_token == TOKEN_TYPE_CONV)


/* --------------------------------------------------------------------------
 * macro M2C_IS_OPER_L6_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a level-6 operator, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_OPER_L6_TOKEN(_token) \
  ((_token >= FIRST_NON_RESWORD_OPER_L6_TOKEN) \
  && (_token <= LAST_NON_RESWORD_OPER_L6_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_NON_SEMANTIC_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a non-semantic token, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_NON_SEMANTIC_TOKEN(_token) \
  ((_token >= FIRST_NON_SEMANTIC_TOKEN) \
  && (_token <= LAST_NON_SEMANTIC_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_COMMENT_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a comment token, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_COMMENT_TOKEN(_token) \
  ((_token == TOKEN_LINE_COMMENT) || (_token == TOKEN_BLOCK_COMMENT))


/* --------------------------------------------------------------------------
 * macro M2C_IS_PRAGMA_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a pragma, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_PRAGMA_TOKEN(_token) \
  (_token == TOKEN_PRAGMA)


/* --------------------------------------------------------------------------
 * macro M2C_IS_ERROR_TOKEN(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a literal, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_ERROR_TOKEN(_token) \
  ((_token >= FIRST_ERROR_TOKEN) && (_token <= LAST_ERROR_TOKEN))


/* --------------------------------------------------------------------------
 * macro M2C_IS_MALFORMED_LITERAL(token)
 * --------------------------------------------------------------------------
 * Returns true if token represents a literal, otherwise false.
 * ----------------------------------------------------------------------- */

#define M2C_IS_MALFORMED_LITERAL(_token) \
  ((_token >= FIRST_MALFORMED_LITERAL_TOKEN) \
  && (_token <= LAST_MALFORMED_LITERAL_TOKEN))


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_resword(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with the
 * lexeme of the resword represented by token, or NULL if token is not valid.
 * ----------------------------------------------------------------------- */

const char *m2c_lexeme_for_resword (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_lexeme_for_special_symbol(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with the
 * lexeme for the special symbol represented by token.  Returns NULL if token
 * does not represent a special symbol.
 * ----------------------------------------------------------------------- */

const char *m2c_lexeme_for_special_symbol (m2c_token_t token);


/* --------------------------------------------------------------------------
 * function m2c_name_for_token(token)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with
 * a human readable name for token,  or NULL if token is not a valid token.
 * ----------------------------------------------------------------------- */

const char *m2c_name_for_token (m2c_token_t token);


#endif /* M2C_TOKEN_H */

/* END OF FILE */