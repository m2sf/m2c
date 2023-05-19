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
 * m2c-cli-lexer.c                                                           *
 *                                                                           *
 * Implementation of command line lexer module.                              *
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
#include "pathnames.h"
#include "iso646.h"

#include <stddef.h>
#include <stdbool.h>


/* ---------------------------------------------------------------------------
 * private type cli_args_t
 * ---------------------------------------------------------------------------
 * Record to store command line arguments and index.
 * ------------------------------------------------------------------------ */

typedef struct {
  unsigned index;
  unsigned count;
  char **lexeme;
} cli_args_t;


/* ---------------------------------------------------------------------------
 * private variable initialized
 * ---------------------------------------------------------------------------
 * Stores initialisation status.
 * ------------------------------------------------------------------------ */

static bool initialized = false;


/* ---------------------------------------------------------------------------
 * private variable cli_arg
 * ---------------------------------------------------------------------------
 * Stores command line arguments and index.
 * ------------------------------------------------------------------------ */

static cli_args_t cli_arg = { 0, 0, NULL };


/* Public Functions */

/* ---------------------------------------------------------------------------
 * function cli_init(argc, argv)
 * ---------------------------------------------------------------------------
 * Initialises the command line lexer.
 * ------------------------------------------------------------------------ */

void cli_init (int argc, char **argv) {
  cli_arg.index = 1;
  cli_arg.count = argc;
  cli_arg.argstr = argv;
  initialized = true;
} /* end cli_init */


/* ---------------------------------------------------------------------------
 * function cli_next_token()
 * ---------------------------------------------------------------------------
 * Reads and consumes the next commmand line argument and returns its token.
 * ------------------------------------------------------------------------ */

static cli_token_t match_compiler_switch (const char *argstr);

cli_token_t cli_next_token (void) {
  cli_token_t token;
  unsigned length;
  char *argstr;
  
  if ((initialized == false) || (cli_arg.index >= cli_arg.count)) {
    return CLI_TOKEN_END_OF_INPUT;
  } /* end if */
  
  argstr = cli_arg.argstr[cli_arg.index];
  length = strlen(argstr);
  
  if (argstr[0] == '-') {
    token = match_compiler_switch(argstr);
  }
  else {
    /* tentatively parse as pathname */
    if (is_valid_pathname(argstr)) {
      token = CLI_TOKEN_SOURCE_FILE;
    }
    else /* not a valid pathname */ {
      /* TO DO : report error */
      token = CLI_TOKEN_INVALID;
    } /* end if */
  } /* end if */
    
  index++;
  return token;
} /* end cli_next_token */


/* ---------------------------------------------------------------------------
 * function cli_last_arg()
 * ---------------------------------------------------------------------------
 * Returns the argument string of the last consumed argument or NULL if the
 * end of input token has been returned by a prior call to cli_next_token().
 * ------------------------------------------------------------------------ */

const char *cli_last_arg (void) {
  if ((initialized == false)
    return NULL;
  } /* end if */
  
  if ((cli_arg.index >= 2) && (cli_arg.index <= cli_arg.count))) {
    return cli_arg.lexeme[cli_arg.index-1];
  }
  else {
    return NULL;
  } /* end if */
} /* cli_last_arg */


/* Private Functions */

/* ---------------------------------------------------------------------------
 * private function match_compiler_switch()
 * ---------------------------------------------------------------------------
 * Matches argstr to a compiler switch and returns its token.  Returns token
 * CLI_TOKEN_INVALID if argstr does not represent a valid compiler switch.
 * ------------------------------------------------------------------------ */

static cli_token_t match_compiler_switch (const char *argstr) {
  unsigned length;
  
  if (argstr == NULL) {
    return CLI_TOKEN_END_OF_INPUT;
  } /* end if */
  
  length = strlen(argstr);

  switch (length) {
    case /* length == */ 2 :
      switch (argstr[1]) {
        /* -h */
        case 'h' :
          return CLI_TOKEN_HELP;
          
        /* -V */
        case 'V' :
          return CLI_TOKEN_VERSION;
          
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
    
    case /* length == */ 5 :
      switch (argstr[2]) {
        /* --ast */
        case 'a' :
          if (str_match(argstr, "--ast")) {
            return CLI_TOKEN_AST;
          } /* end if */
          
        /* --obj */
        case 'o' :
          if (str_match(argstr, "--obj")) {
            return CLI_TOKEN_OBJ;
          } /* end if */
        
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
      
    case /* length == */ 6 :
      switch (argstr[2]) {
        /* --help */
        case 'h' :
          if (str_match(argstr, "--help")) {
            return CLI_TOKEN_HELP;
          } /* end if */
          
        /* --xlat */
        case 'x' :
          if (str_match(argstr, "--xlat")) {
            return CLI_TOKEN_XLAT;
          } /* end if */
        
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
      
    case /* length == */ 7 :
      /* --graph */
      if (str_match(argstr, "--graph")) {
        return CLI_TOKEN_GRAPH;
      } /* end if */
      
    case /* length == */ 8 :
      switch (argstr[5]) {
        /* --no-ast */
        case 'a' :
          if (str_match(argstr, "--no-ast")) {
            return CLI_TOKEN_NO_AST;
          } /* end if */
          
        /* --no-obj */
        case 'o' :
          if (str_match(argstr, "--no-obj")) {
            return CLI_TOKEN_NO_OBJ;
          } /* end if */
        
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
      
    case /* length == */ 9 :
      switch (argstr[5]) {
        /* --verbose */
        case 'b' :
          if (str_match(argstr, "--verbose")) {
            return CLI_TOKEN_VERBOSE;
          } /* end if */
          
        /* --license */
        case 'e' :
          if (str_match(argstr, "--license")) {
            return CLI_TOKEN_LICENSE;
          } /* end if */
        
        /* --version */
        case 's' :
          if (str_match(argstr, "--version")) {
            return CLI_TOKEN_VERSION;
          } /* end if */
        
        /* --no-xlat */
        case 'x' :
          if (str_match(argstr, "--no-xlat")) {
            return CLI_TOKEN_NO_XLAT;
          } /* end if */
        
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
      
    case /* length == */ 10 :
      switch (argstr[2]) {
        /* --ast-only */
        case 'a' :
          if (str_match(argstr, "--ast-only")) {
            return CLI_TOKEN_AST_ONLY;
          } /* end if */
          
        /* --no-graph */
        case 'n' :
          if (str_match(argstr, "--no-graph")) {
            return CLI_TOKEN_NO_GRAPH;
          } /* end if */
          
        /* --obj-only */
        case 'o' :
          if (str_match(argstr, "--obj-only")) {
            return CLI_TOKEN_OBJ_ONLY;
          } /* end if */
        
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
      
    case /* length == */ 11 :
      /* --xlat-only */
      if (str_match(argstr, "--xlat-only")) {
        return CLI_TOKEN_XLAT_ONLY;
      } /* end if */
      
    case /* length == */ 12 :
      /* --graph-only */
      if (str_match(argstr, "--graph-only")) {
        return CLI_TOKEN_GRAPH_ONLY;
      } /* end if */
      
    case /* length == */ 13 :
      switch (argstr[2]) {
        /* --lexer-debug */
        case 'l' :
          if (str_match(argstr, "--lexer-debug")) {
            return CLI_TOKEN_LEXER_DEBUG;
          } /* end if */
          
        /* --syntax-only */
        case 's' :
          if (str_match(argstr, "--syntax-only")) {
            return CLI_TOKEN_SYNTAX_ONLY;
          } /* end if */
        
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
      
    case /* length == */ 14 :
      /* --parser-debug */
      if (str_match(argstr, "--parser-debug")) {
        return CLI_TOKEN_PARSER_DEBUG;
      } /* end if */
      
    case /* length == */ 15 :
      /* --show-settings */
      if (str_match(argstr, "--show-settings")) {
        return CLI_TOKEN_SHOW_SETTINGS;
      } /* end if */
      
    case /* length == */ 16 :
      /* --strip-comments */
      if (str_match(argstr, "--strip-comments")) {
        return CLI_TOKEN_STRIP_COMMENTS;
      } /* end if */
      
    case /* length == */ 19 :
      switch (argstr[2]) {
        /* --errant-semicolons */
        case 'e' :
          if (str_match(argstr, "--errant-semicolons")) {
            return CLI_TOKEN_ERRANT_SEMICOLON;
          } /* end if */
          
        /* --preserve-comments */
        case 'p' :
          if (str_match(argstr, "--preserve-comments")) {
            return CLI_TOKEN_PRESERVE_COMMENTS;
          } /* end if */
        
        default :
          return CLI_TOKEN_INVALID;
      } /* end switch */
      
    case /* length == */ 20 :
      /* --dollar-identifiers */
      if (str_match(argstr, "--dollar-identifiers")) {
        return CLI_TOKEN_DOLLAR_IDENTIFIERS;
      } /* end if */
      
    case /* length == */ 21 :
      /* --lowline-identifiers */
      if (str_match(argstr, "--lowline-identifiers")) {
        return CLI_TOKEN_LOWLINE_IDENTIFIERS;
      } /* end if */
      
    case /* length == */ 23 :
      /* --no-dollar-identifiers */
      if (str_match(argstr, "--no-dollar-identifiers")) {
        return CLI_TOKEN_NO_DOLLAR_IDENTIFIERS;
      } /* end if */
      
    case /* length == */ 24 :
      /* --no-lowline-identifiers */
      if (str_match(argstr, "--no-lowline-identifiers")) {
        return CLI_TOKEN_NO_LOWLINE_IDENTIFIERS;
      } /* end if */
      
    default :
      return CLI_TOKEN_INVALID;
  } /* end switch */
} /* end match_compiler_switch */


#endif /* M2C_CLI_LEXER_H */

/* END OF FILE */