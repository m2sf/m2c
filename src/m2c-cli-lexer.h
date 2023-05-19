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
 * m2c-cli-lexer.h                                                           *
 *                                                                           *
 * Public interface of command line lexer module.                            *
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

#ifndef M2C_CLI_LEXER_H
#define M2C_CLI_LEXER_H

/* ---------------------------------------------------------------------------
 * type cli_token_t
 * ---------------------------------------------------------------------------
 * Enumerated token values representing m2c command line arguments.
 * ------------------------------------------------------------------------ */

typedef enum {
  
  /* invalid argument */
  
  CLI_TOKEN_INVALID,
  
  /* information options */
  
  CLI_TOKEN_HELP,                    /* --help, -h */
  CLI_TOKEN_VERSION,                 /* --version, -V */
  CLI_TOKEN_LICENSE,                 /* --license */
  
  /* single product options */
  
  CLI_TOKEN_SYNTAX_ONLY,             /* --syntax-only */
  CLI_TOKEN_AST_ONLY,                /* --ast-only */
  CLI_TOKEN_GRAPH_ONLY,              /* --graph-only */
  CLI_TOKEN_XLAT_ONLY,               /* --xlat-only */
  CLI_TOKEN_OBJ_ONLY,                /* --obj-only */
  
  /* multiple product options */

  CLI_TOKEN_AST,                     /* --ast */
  CLI_TOKEN_NO_AST,                  /* --no-ast */
  CLI_TOKEN_GRAPH,                   /* --graph */
  CLI_TOKEN_NO_GRAPH,                /* --no-graph */
  CLI_TOKEN_XLAT,                    /* --xlat */
  CLI_TOKEN_NO_XLAT,                 /* --no-xlat */
  CLI_TOKEN_OBJ,                     /* --obj */
  CLI_TOKEN_NO_OBJ,                  /* --no-obj */

  /* comment option */

  CLI_TOKEN_PRESERVE_COMMENTS,       /* --preserve-comments */
  CLI_TOKEN_STRIP_COMMENTS,          /* --strip-comments */

  /* capability options */

  CLI_TOKEN_DOLLAR_IDENTIFIERS,      /* --dollar-identifiers */
  CLI_TOKEN_NO_DOLLAR_IDENTIFIERS,   /* --no-dollar-identifiers */
  CLI_TOKEN_LOWLINE_IDENTIFIERS,     /* --lowline-identifiers */
  CLI_TOKEN_NO_LOWLINE_IDENTIFIERS,  /* --no-lowline-identifiers */
  
  /* source file argument */
  
  CLI_TOKEN_SOURCE_FILE,
  
  /* diagnostic options */

  CLI_TOKEN_VERBOSE,                 /* --verbose */
  CLI_TOKEN_LEXER_DEBUG,             /* --lexer-debug */
  CLI_TOKEN_PARSER_DEBUG,            /* --parser-debug */
  CLI_TOKEN_SHOW_SETTINGS,           /* --show-settings */
  CLI_TOKEN_ERRANT_SEMICOLONS,       /* --errant-semicolons */
  
  /* end of input sentinel */
  
  CLI_TOKEN_END_OF_INPUT
  
} cli_token_t;


/* ---------------------------------------------------------------------------
 * Token group boundaries
 * ------------------------------------------------------------------------ */

#define CLI_FIRST_INFO_REQUEST_TOKEN CLI_TOKEN_HELP
#define CLI_LAST_INFO_REQUEST_TOKEN CLI_TOKEN_LICENSE

#define CLI_FIRST_COMPILATION_REQUEST_TOKEN CLI_TOKEN_SYNTAX_ONLY
#define CLI_LAST_COMPILATION_REQUEST_TOKEN CLI_TOKEN_SOURCE_FILE

#define CLI_FIRST_PRODUCT_OPTION_TOKEN CLI_TOKEN_SYNTAX_ONLY
#define CLI_LAST_PRODUCT_OPTION_TOKEN CLI_TOKEN_NO_OBJ

#define CLI_FIRST_SINGLE_PRODUCT_OPTION_TOKEN CLI_TOKEN_SYNTAX_ONLY
#define CLI_LAST_SINGLE_PRODUCT_OPTION_TOKEN CLI_TOKEN_OBJ_ONLY

#define CLI_FIRST_MULTIPLE_PRODUCT_OPTION_TOKEN CLI_TOKEN_AST
#define CLI_LAST_MULTIPLE_PRODUCT_OPTION_TOKEN CLI_TOKEN_NO_OBJ

#define CLI_FIRST_COMMENT_OPTION_TOKEN CLI_TOKEN_PRESERVE_COMMENTS
#define CLI_LAST_COMMENT_OPTION_TOKEN CLI_TOKEN_STRIP_COMMENTS

#define CLI_FIRST_CAPABILITY_OPTION_TOKEN CLI_TOKEN_DOLLAR_IDENTIFIERS
#define CLI_LAST_CAPABILITY_OPTION_TOKEN CLI_TOKEN_NO_LOWLINE_IDENTIFIERS

#define CLI_FIRST_DIAGNOSTICS_OPTION_TOKEN CLI_TOKEN_VERBOSE
#define CLI_LAST_DIAGNOSTICS_OPTION_TOKEN CLI_TOKEN_ERRANT_SEMICOLONS


/* ---------------------------------------------------------------------------
 * macro CLI_IS_INFO_REQUEST(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents an information request, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_INFO_REQUEST(_token) \
  ((_token >= CLI_FIRST_INFO_REQUEST_TOKEN) \
  && (_token <= CLI_LAST_INFO_REQUEST_TOKEN))


/* ---------------------------------------------------------------------------
 * macro CLI_IS_COMPILATION_REQUEST(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a compilation request, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_COMPILATION_REQUEST(_token) \
  ((_token >= CLI_FIRST_COMPILATION_REQUEST_TOKEN) \
  && (_token <= CLI_LAST_COMPILATION_REQUEST_TOKEN))


/* ---------------------------------------------------------------------------
 * macro CLI_IS_PRODUCT_OPTION(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a product option, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_PRODUCT_OPTION(_token) \
  ((_token >= CLI_FIRST_PRODUCT_OPTION_TOKEN) \
  && (_token <= CLI_LAST_PRODUCT_OPTION_TOKEN))


/* ---------------------------------------------------------------------------
 * macro CLI_IS_SINGLE_PRODUCT_OPTION(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a single product option, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_SINGLE_PRODUCT_OPTION(_token) \
  ((_token >= CLI_FIRST_SINGLE_PRODUCT_OPTION_TOKEN) \
  && (_token <= CLI_LAST_SINGLE_PRODUCT_OPTION_TOKEN))


/* ---------------------------------------------------------------------------
 * macro CLI_IS_MULTIPLE_PRODUCT_OPTION(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a multiple product option, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_MULTIPLE_PRODUCT_OPTION(_token) \
  ((_token >= CLI_FIRST_MULTIPLE_PRODUCT_OPTION_TOKEN) \
  && (_token <= CLI_LAST_MULTIPLE_PRODUCT_OPTION_TOKEN))


/* ---------------------------------------------------------------------------
 * macro CLI_IS_COMMENT_OPTION(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a comment option, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_COMMENT_OPTION(_token) \
  ((_token >= CLI_FIRST_COMMENT_OPTION_TOKEN) \
  && (_token <= CLI_LAST_COMMENT_OPTION_TOKEN))


/* ---------------------------------------------------------------------------
 * macro CLI_IS_CAPABILITY_OPTION(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a comment option, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_CAPABILITY_OPTION(_token) \
  ((_token >= CLI_FIRST_CAPABILITY_OPTION_TOKEN) \
  && (_token <= CLI_LAST_CAPABILITY_OPTION_TOKEN))


/* ---------------------------------------------------------------------------
 * macro CLI_IS_DIAGNOSTICS_OPTION(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a comment option, else false.
 * ------------------------------------------------------------------------ */

#define CLI_IS_DIAGNOSTICS_OPTION(_token) \
  ((_token >= CLI_FIRST_DIAGNOSTICS_OPTION_TOKEN) \
  && (_token <= CLI_LAST_DIAGNOSTICS_OPTION_TOKEN))


/* ---------------------------------------------------------------------------
 * function cli_init(argc, argv)
 * ---------------------------------------------------------------------------
 * Initialises the command line lexer.
 * ------------------------------------------------------------------------ */

void cli_init (int argc, char **argv);


/* ---------------------------------------------------------------------------
 * function cli_next_token()
 * ---------------------------------------------------------------------------
 * Reads and consumes the next commmand line argument and returns its token.
 * ------------------------------------------------------------------------ */

cli_token_t cli_next_token (void);


/* ---------------------------------------------------------------------------
 * function cli_last_arg()
 * ---------------------------------------------------------------------------
 * Returns the argument string of the last consumed argument or NULL if the
 * end of input token has been returned by a prior call to cli_next_token().
 * ------------------------------------------------------------------------ */

const char *cli_last_arg (void);


#endif /* M2C_CLI_LEXER_H */

/* END OF FILE */