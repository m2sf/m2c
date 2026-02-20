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
 * m2c-cli-parser.c                                                          *
 *                                                                           *
 * Implementation of command line parser module.                             *
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

/* ---------------------------------------------------------------------------
 * imports
 * ------------------------------------------------------------------------ */

#include "m2c-cli-lexer.h"
#include "m2c-compiler-options.h"
#include "cstring.h"

#include <stdio.h>


/* ---------------------------------------------------------------------------
 * properties
 * ------------------------------------------------------------------------ */

static unsigned err_count = 0;
static unsigned option_set = 0;
static const char *source_file = NULL;


/* ---------------------------------------------------------------------------
 * macros
 * ------------------------------------------------------------------------ */

#define IS_IN_SET(_set, _elem) \
  (_set & (1 << _elem))
#define INSERT_INTO_SET(_set, _elem) \
  _set = _set | (1 << _elem)
#define REMOVE_FROM_SET(_set, _elem) \
  _set = _set & ~(1 << _elem)


/* Public Operations */

/* ---------------------------------------------------------------------------
 * function cli_parse_args()
 * ---------------------------------------------------------------------------
 * Parses command line arguments and sets compiler options accordingly.
 *
 * args :
 *   infoRequest | compilationRequest
 *   ;
 * ------------------------------------------------------------------------ */

cli_parser_status_t cli_parse_args (void) {
  cli_token_t token;

  token = cli_next_token();
  
  if (CLI_IS_INFO_REQUEST(token)) {
    token = parse_info_request(token);
  }
  else if (CLI_IS_COMPILATION_REQUEST(token)) {
    token = parse_compilation_request(token);
  }
  else if (token == CLI_TOKEN_END_OF_INPUT) {
    report_missing_source_file();
  } /* end if */
  
  while (token != CLI_TOKEN_END_OF_INPUT) {
    report_excess_argument(cli_last_arg());
    token = cli_next_token();
  } /* end while */
  
  if (err_count > 0) {
    status = CLI_PARSER_STATUS_ERRORS_ENCOUNTERED;
  } /* end if */
  
  return status;
/* end cli_parse_args */


(* ---------------------------------------------------------------------------
 * function cli_source_file()
 * ---------------------------------------------------------------------------
 * Returns a string with the source file argument.
 * ------------------------------------------------------------------------ *)

const char *cli_source_file (void) {
  return source_file;
} /* end cli_source_file */


/* ---------------------------------------------------------------------------
 * function cli_error_count()
 * ---------------------------------------------------------------------------
 * Returns the count of errors encountered while parsing the arguments.
 * ------------------------------------------------------------------------ */

unsigned cli_error_count (void) {
  return err_count;
} /* end cli_error_count */


/* Private Operations */

(* ---------------------------------------------------------------------------
 * function parse_info_request(token)
 * ---------------------------------------------------------------------------
 * infoRequest :
 *   --help | -h | --version | -V | --license
 *   ;
 * ------------------------------------------------------------------------ *)

cli_token_t parse_info_request (cli_token_t token) {

  switch (token) {
  /* --help, -h */
    case CLI_TOKEN_HELP :
      status = CLI_PARSER_STATUS_HELP_REQUESTED;
      break;
  
  /* --version, -V */  
    case CLI_TOKEN_VERSION :
      status = CLI_PARSER_STATUS_VERSION_REQUESTED;
      break;
  
  /* --license */
    case CLI_TOKEN_LICENSE :
      status = CLI_PARSER_STATUS_LICENSE_REQUESTED;
      break;
  } /* end switch */
  
  return cli_next_token();
} /* end parse_info_request */


/* ---------------------------------------------------------------------------
 * function parse_compilation_request(token)
 * ---------------------------------------------------------------------------
 * compilationRequest :
 *   products? capabilities? sourceFile diagnostics?
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_compilation_request (cli_token_t token) {

  /* products? */
  if (CLI_IS_PRODUCT_OPTION(token)) {
    token = parse_products(token);
  } /* end if */
  
  /* capabilities? */
  if (CLI_IS_CAPABILITY_OPTION(token)) {
    token = parse_capabilities(token);
  } /* end if */
  
  /* sourceFile */
  if (token == CLI_TOKEN_SOURCE_FILE) {
    token = parse_source_file(token);
  }
  else {
    report_missing_source_file();
  } /* end if */
  
  /* diagnostics? */
  if (CLI_IS_DIAGNOSTICS_OPTION(token)) {
    token = parse_diagnostics(token);
  } /* end if */
  
  return token;
} /* end parse_compilation_request */


/* ---------------------------------------------------------------------------
 * function parse_products(token)
 * ---------------------------------------------------------------------------
 * products :
 *   ( singleProduct | multipleProducts ) commentOption?
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_products (cli_token_t token) {

  /* ( singleProduct | multipleProducts ) */
  if (CLI_IS_SINGLE_PRODUCT_OPTION(token)) {
    token = parse_single_product(token);
  }
  else {
    token = parse_multiple_products(token);
  } /* end if */
  
  /* commentOption? */
  if (CLI_IS_COMMENT_OPTION(token)) {
    if (m2c_compiler_option_xlat_required()) {
      token = parse_comment_option(token);
    }
    else /*xlat option not set */ {
      report_missing_dependency_for(cli_last_arg(), "--xlat");
      token = cli_next_token();
    } /* end if */
  } /* end if */
  
  return token;
} /* end parse_products */


/* ---------------------------------------------------------------------------
 * function parse_single_product(token)
 * ---------------------------------------------------------------------------
 * singleProduct :
 *   --syntax-only | --ast-only | --graph-only | --xlat-only | --obj-only
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_single_product (cli_token_t token) {

  switch (token) {
  /* --syntax-only | */
    case CLI_TOKEN_SYNTAX_ONLY :
      set_option(M2C_COMPILER_OPTION_AST_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_GRAPH_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_XLAT_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_OBJ_REQUIRED, false);
      break;
  
  /* --ast-only | */
    case CLI_TOKEN_AST_ONLY :
      set_option(M2C_COMPILER_OPTION_AST_REQUIRED, true);
      set_option(M2C_COMPILER_OPTION_GRAPH_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_XLAT_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_OBJ_REQUIRED, false);
      break;
  
  /* --graph-only | */
    case CLI_TOKEN_GRAPH_ONLY :
      set_option(M2C_COMPILER_OPTION_AST_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_GRAPH_REQUIRED, true);
      set_option(M2C_COMPILER_OPTION_XLAT_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_OBJ_REQUIRED, false);
      break;
  
  /* --xlat-only | */
    case CLI_TOKEN_XLAT_ONLY :
      set_option(M2C_COMPILER_OPTION_AST_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_GRAPH_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_XLAT_REQUIRED, true);
      set_option(M2C_COMPILER_OPTION_OBJ_REQUIRED, false);
      break;
  
  /* --obj-only */
    case CLI_TOKEN_OBJ_ONLY :
      set_option(M2C_COMPILER_OPTION_AST_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_GRAPH_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_XLAT_REQUIRED, false);
      set_option(M2C_COMPILER_OPTION_OBJ_REQUIRED, true);
      break;
  } /* end switch */
  
  return cli_next_token();
} /* end parse_single_product */


/* ---------------------------------------------------------------------------
 * function parse_multiple_products(token)
 * ---------------------------------------------------------------------------
 * multipleProducts :
 *   ( ast | graph | xlat | obj )+
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_multiple_products (cli_token_t token) {

  /* ( ast | graph | xlat | obj )+ */
  while (CLI_IS_MULTIPLE_PRODUCT_OPTION(token)) {
    switch (token) {
      /* --ast | */
      case CLI_TOKEN_AST :
        set_option(M2C_COMPILER_OPTION_AST_REQUIRED, true);
        break;
      
      /* --no-ast | */
      case CLI_TOKEN_NO_AST :
        set_option(M2C_COMPILER_OPTION_AST_REQUIRED, false);
        break;
    
      /* --graph | */
      case CLI_TOKEN_GRAPH :
        set_option(M2C_COMPILER_OPTION_GRAPH_REQUIRED, true);
        break;
      
      /* --no-graph | */
      case CLI_TOKEN_NO_GRAPH :
        set_option(M2C_COMPILER_OPTION_GRAPH_REQUIRED, false);
        break;
    
      /* --xlat | */
      case CLI_TOKEN_XLAT :
        set_option(M2C_COMPILER_OPTION_XLAT_REQUIRED, true);
        break;
      
      /* --no-xlat | */
      case CLI_TOKEN_NO_XLAT :
        set_option(M2C_COMPILER_OPTION_XLAT_REQUIRED, false);
        break;
    
      /* --obj | */
      case CLI_TOKEN_OBJ :
        set_option(M2C_COMPILER_OPTION_OBJ_REQUIRED, true);
        break;
      
      /* --no-obj | */
      case CLI_TOKEN_NO_OBJ :
        set_option(M2C_COMPILER_OPTION_OBJ_REQUIRED, false);
        break;
    } /* end switch */
    
    token = cli_next_token();
  } /* end while */
  
  return token;
} /* end parse_multiple_products */


/* ---------------------------------------------------------------------------
 * function parse_comment_option(token)
 * ---------------------------------------------------------------------------
 * commentOption :
 *   --preserve-comments | --strip-comments
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_comment_option (cli_token_t token) {

  /* --preserve-comments | */
  if (token == CLI_TOKEN_PRESERVE_COMMENTS) {
    set_option(M2C_COMPILER_OPTION_PRESERVE_COMMENTS, true);
  }
  /* --strip-comments */
  else {
    set_option(M2C_COMPILER_OPTION_PRESERVE_COMMENTS, false);
  } /* end if */
  
  return cli_next_token();
} /* end parse_comment_option */


/* ---------------------------------------------------------------------------
 * function parse_capabilities(token)
 * ---------------------------------------------------------------------------
 * capabilities :
 *   ( dollarIdentifiers | lowlineIdentifiers )+
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_capabilities (cli_token_t token) {

  /* ( dollarIdentifiers | lowlineIdentifiers )+ */
  while (CLI_IS_CAPABILITY_OPTION(token)) {
    switch (token) {
      /* --dollar-identifiers */
      case CLI_TOKEN_DOLLAR_IDENTIFIERS :
        set_option(M2C_COMPILER_OPTION_DOLLAR_IDENTIFIERS, true);
        break;
        
    /* --no-dollar-identifiers */
      case CLI_TOKEN_NO_DOLLAR_IDENTIFIERS :
        set_option(M2C_COMPILER_OPTION_DOLLAR_IDENTIFIERS, false);
        break;
    
    /* --lowline-identifiers */
      case CLI_TOKEN_LOWLINE_IDENTIFIERS :
        set_option(M2C_COMPILER_OPTION_LOWLINE_IDENTIFIERS, true);
        break;
    
    /* --no-lowline-identifiers */
      case CLI_TOKEN_NO_LOWLINE_IDENTIFIERS :
        set_option(M2C_COMPILER_OPTION_LOWLINE_IDENTIFIERS, false);
        break;
    } /* end switch */
    
    token = cli_next_token();
  } /* end while */
  
  return token;
} /* end parse_capabilities */


/* ---------------------------------------------------------------------------
 * function parse_source_file(token)
 * ---------------------------------------------------------------------------
 * sourceFile :
 *   <platform dependent path/filename>
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_source_file (cli_token_t token) {
  
  source_file = cli_last_arg();
  
  return cli_next_token();
} /* end parse_source_file */


/* ---------------------------------------------------------------------------
 * function parse_diagnostics(token)
 * ---------------------------------------------------------------------------
 * diagnostics :
 *   ( --verbose | -v | --lexer-debug | --parser-debug | --print-settings |
 *     --errant-semicolons )+
 *   ;
 * ------------------------------------------------------------------------ */

cli_token_t parse_diagnostics (cli_token_t token) {

  while (CLI_IS_DIAGNOSTICS_OPTION(token)) {
    switch (token) {
    /* --verbose | -v | */
      case CLI_TOKEN_VERBOSE :
        set_option(M2C_COMPILER_OPTION_VERBOSE, true);
        break;
    
    /* --lexer-debug | */
      case CLI_TOKEN_LEXER_DEBUG :
        set_option(M2C_COMPILER_OPTION_LEXER_DEBUG, true);
        break;
    
    /* --parser-debug | */
      case CLI_TOKEN_PARSER_DEBUG :
        set_option(M2C_COMPILER_OPTION_PARSER_DEBUG, true);
        break;
    
    /* --print-settings | */
      case CLI_TOKEN_SHOW_SETTINGS :
        set_option(M2C_COMPILER_OPTION_SHOW_SETTINGS, true);
        break;
    
    /* --errant-semicolons | */
      case CLI_TOKEN_ERRANT_SEMICOLONS :
        set_option(M2C_COMPILER_OPTION_ERRANT_SEMICOLONS, true);
        break;
    } /* end switch */
    
    token = cli_next_token();
  } /* end while */
  
  return token;
} /* end parse_diagnostics */


/* ---------------------------------------------------------------------------
 * procedure set_option(option)
 * ---------------------------------------------------------------------------
 * Sets option unless duplicate. Reports duplicate.
 * ------------------------------------------------------------------------ */

static void set_option (m2c_compiler_option_t option, bool value) {

  /* check if duplicate */
  if (IS_IN_SET(option_set, option)) {
    report_duplicate_option(cli_last_arg());
  }
  else /* not a duplicate */ {
    m2c_compiler_option_set(option, value);
    /* remember this option */
    INSERT_INTO_SET(option_set, option);
  } /* end if */
  
} /* end set_option */


/* ---------------------------------------------------------------------------
 * procedure report_invalid_option(argstr)
 * ---------------------------------------------------------------------------
 * Reports argstr as an invalid option to the console.
 * ------------------------------------------------------------------------ */

static void report_invalid_option (const char *argstr) {

  printf("invalid option %s\n", argstr);
  err_count++;
  
} /* end report_invalid_option */


/* ---------------------------------------------------------------------------
 * procedure report_duplicate_option(argstr)
 * ---------------------------------------------------------------------------
 * Reports argstr as a duplicate option to the console.
 * ------------------------------------------------------------------------ */

static void report_duplicate_option (const char *argstr) {

  printf("duplicate option %s\n", argstr);
  err_count++;
  
} /* end report_duplicate_option */


/* ---------------------------------------------------------------------------
 * procedure report_excess_argument(argstr)
 * ---------------------------------------------------------------------------
 * Reports argstr as an excess argument to the console.
 * ------------------------------------------------------------------------ */

static void report_excess_argument (const char *argstr) {

  printf("excess argument %s\n", argstr);
  err_count++;
  
} /* end report_excess_argument */


/* ---------------------------------------------------------------------------
 * procedure report_missing_source_file
 * ---------------------------------------------------------------------------
 * Reports missing sourcefile argument to the console.
 * ------------------------------------------------------------------------ */

static void report_missing_source_file (void) {

  printf("missing sourcefile argument\n");
  err_count++;
  
} /* end report_missing_source_file */


/* ---------------------------------------------------------------------------
 * procedure report_missing_dependency_for(argstr, depstr)
 * ---------------------------------------------------------------------------
 * Reports argstr to have a missing dependency on depstr to the console.
 * ------------------------------------------------------------------------ */

static void report_missing_dependency_for
  (const char *argstr, const char *depstr) {
  
  printf("option %s only available with option %s\n", argstr, depstr);
  err_count++;
  
} /* end report_missing_dependency_for */


/* END OF FILE */
