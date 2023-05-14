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
 * m2c-mkdep-opt-flags.h                                                     *
 *                                                                           *
 * Public interface of m2mkdep options module.                               *
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

#ifndef M2C_MKDEP_OPTIONS_H
#define M2C_MKDEP_OPTIONS_H

#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_mkdep_option_t
 * --------------------------------------------------------------------------
 * Enumerated token values representing mkdep CLI option symbols.
 * ----------------------------------------------------------------------- */

typedef enum {
  
  /* Diagnostic Options */

  M2C_COMPILER_OPTION_VERBOSE,             /* --verbose */
  M2C_COMPILER_OPTION_LEXER_DEBUG,         /* --lexer-debug */
  M2C_COMPILER_OPTION_PARSER_DEBUG,        /* --parser-debug */

  /* Output Options */
  
  M2C_MKDEP_OPTION_GRAPH_REQUIRED,         /* --graph, --no-graph */

  /* Capability Options */
  
  /* --dollar-identifiers, --no-dollar-identifiers */
  M2C_MKDEP_OPTION_DOLLAR_IDENTIFIERS,

  /* --lowline-identifiers, --no-lowline-identifiers */
  M2C_MKDEP_OPTION_LOWLINE_IDENTIFIERS,

  /* Enumeration Terminator */

  M2C_MKDEP_OPTION_END_MARK

} m2c_mkdep_option_t;


/* --------------------------------------------------------------------------
 * procedure m2c_mkdep_option_set(option, value)
 * ---------------------------------------------------------------------------
 * Sets the given option to the given boolean value.
 * ----------------------------------------------------------------------- */

void m2c_mkdep_option_set (m2c_mkdep_option_t option, bool value);


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_verbose()
 * ---------------------------------------------------------------------------
 * Returns true if option --verbose is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_verbose (void);


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_lexer_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --lexer-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_lexer_debug (void);


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_parser_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --parser-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_parser_debug (void);


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_graph_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --graph is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_graph_required (void);


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_dollar_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --dollar-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_dollar_identifiers (void);


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_lowline_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --lowline-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_lowline_identifiers (void);


#endif /* M2C_MKDEP_OPTIONS_H */

/* END OF FILE */