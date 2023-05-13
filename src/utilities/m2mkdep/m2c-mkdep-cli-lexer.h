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
 * m2c-mkdep-cli-lexer.h                                                     *
 *                                                                           *
 * Public interface of m2mkdep command line interface lexer module.          *
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

#ifndef M2C_MKDEP_CLI_LEXER_H
#define M2C_MKDEP_CLI_LEXER_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "string.h"
#include <stdbool.h>


/* ---------------------------------------------------------------------------
 * type mkdep_cli_token_t
 * ---------------------------------------------------------------------------
 * Enumerated token values representing m2mkdep command line arguments.
 * ------------------------------------------------------------------------ */

typedef enum {
  
  /* invalid argument */
  
  MKDEP_CLI_TOKEN_INVALID,
  
  /* information options */
  
  MKDEP_CLI_TOKEN_HELP,                    /* --help, -h */
  MKDEP_CLI_TOKEN_VERSION,                 /* --version, -V */
  MKDEP_CLI_TOKEN_LICENSE,                 /* --license */
  
  /* capability options */

  MKDEP_CLI_TOKEN_DOLLAR_IDENTIFIERS,      /* --dollar-identifiers */
  MKDEP_CLI_TOKEN_NO_DOLLAR_IDENTIFIERS,   /* --no-dollar-identifiers */
  MKDEP_CLI_TOKEN_LOWLINE_IDENTIFIERS,     /* --lowline-identifiers */
  MKDEP_CLI_TOKEN_NO_LOWLINE_IDENTIFIERS,  /* --no-lowline-identifiers */
  
  /* source file argument */
  
  MKDEP_CLI_TOKEN_SOURCE_FILE,
  
  /* diagnostic options */

  MKDEP_CLI_TOKEN_VERBOSE,                 /* --verbose */
  MKDEP_CLI_TOKEN_LEXER_DEBUG,             /* --lexer-debug */
  MKDEP_CLI_TOKEN_PARSER_DEBUG,            /* --parser-debug */
  
  /* end of input sentinel */
  
  MKDEP_CLI_TOKEN_END_OF_INPUT
  
} mkdep_cli_token_t;


/* ---------------------------------------------------------------------------
 * function mkdep_cli_next_token()
 * ---------------------------------------------------------------------------
 * Reads and consumes the next commmand line argument and returns its token.
 * ------------------------------------------------------------------------ */

mkdep_cli_token_t mkdep_cli_next_token (void);


/* ---------------------------------------------------------------------------
 * function mkdep_cli_last_arg()
 * ---------------------------------------------------------------------------
 * Returns the argument string of the last consumed argument or NULL if the
 * end of input token has been returned by a prior call to cli_next_token().
 * ------------------------------------------------------------------------ */

m2c_string_t mkdep_cli_last_arg (void);


/* ---------------------------------------------------------------------------
 * function mkdep_cli_is_info_request(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents an information request, else false.
 * ------------------------------------------------------------------------ */

bool mkdep_cli_is_info_request (mkdep_cli_token_t token);


/* ---------------------------------------------------------------------------
 * function mkdep_cli_is_compilation_request(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a compilation request, else false.
 * ------------------------------------------------------------------------ */

bool mkdep_cli_is_compilation_request (mkdep_cli_token_t token);


/* ---------------------------------------------------------------------------
 * function mkdep_cli_is_capability_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a capability option, else false.
 * ------------------------------------------------------------------------ */

bool mkdep_cli_is_capability_option (mkdep_cli_token_t token);


/* ---------------------------------------------------------------------------
 * function mkdep_cli_is_diagnostics_option(token)
 * ---------------------------------------------------------------------------
 * Returns true if token represents a diagnostics option, else false.
 * ------------------------------------------------------------------------ */

bool mkdep_cli_is_diagnostics_option (mkdep_cli_token_t token);


#endif /* M2C_MKDEP_CLI_LEXER_H */

/* END OF FILE */