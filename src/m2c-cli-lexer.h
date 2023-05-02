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
 * cli-lexer.h                                                               *
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

#include "string.h"


/* ---------------------------------------------------------------------------
 * type cli_token_t
 * ---------------------------------------------------------------------------
 * Enumerated token values representing command line arguments.
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

m2c_string_t cli_last_arg (void);


/* ---------------------------------------------------------------------------
 * function cli_is_info_request(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents an information request, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_info_request (cli_token_t token);


/* ---------------------------------------------------------------------------
 * function cli_is_compilation_request(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a compilation request, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_compilation_request (cli_token_t token);


/* ---------------------------------------------------------------------------
 * function cli_is_product_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a product option, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_product_option (cli_token_t token);


/* ---------------------------------------------------------------------------
 * function cli_is_single_product_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a single product option, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_single_product_option (cli_token_t token);


/* ---------------------------------------------------------------------------
 * function cli_is_multiple_product_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a multiple product option, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_multiple_product_option (cli_token_t token);


/* ---------------------------------------------------------------------------
 * function cli_is_comment_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a comment option, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_comment_option (cli_token_t token);


/* ---------------------------------------------------------------------------
 * function cli_is_capability_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a capability option, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_capability_option (cli_token_t token);


/* ---------------------------------------------------------------------------
 * function cli_is_diagnostics_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a diagnostics option, else false.
 * ------------------------------------------------------------------------ */

bool cli_is_diagnostics_option (cli_token_t token);


#endif /* M2C_CLI_LEXER_H */

/* END OF FILE */