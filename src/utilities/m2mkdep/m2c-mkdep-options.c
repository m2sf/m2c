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
 * m2c-mkdep-opt-flags.c                                                     *
 *                                                                           *
 * Implementation of m2mkdep option flags module.                            *
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

#ifndef M2C_MKDEP_OPTION_FLAGS_H
#define M2C_MKDEP_OPTION_FLAGS_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "m2c-mkdep-options.h"


/* --------------------------------------------------------------------------
 * default option flags
 * ----------------------------------------------------------------------- */

#define M2C_MKDEP_DEFAULT_OPTIONS { \
  /* verbose */ false, \
  /* lexer-debug */ false, \
  /* parser-debug */ false, \
  /* graph-required */ false, \
  /* dollar-identifiers */ false, \
  /* lowline-identifiers */ false \
} /* default_options */


/* --------------------------------------------------------------------------
 * hidden variable options
 * --------------------------------------------------------------------------
 * Option flags for mkdep
 * ----------------------------------------------------------------------- */

static m2c_mkdep_option_t
  options[M2C_MKDEP_OPTION_END_MARK] = M2C_MKDEP_DEFAULT_OPTIONS;


/* --------------------------------------------------------------------------
 * procedure m2c_mkdep_option_set(option, value)
 * ---------------------------------------------------------------------------
 * Sets the given option to the given boolean value.
 * ----------------------------------------------------------------------- */

void m2c_mkdep_option_set (m2c_mkdep_option_t option, bool value) {

  if (value < M2C_MKDEP_OPTION_END_MARK) {
    options[option] = value;
  } /* end if */

  return;
} /* end m2c_mkdep_option_set */


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_verbose()
 * ---------------------------------------------------------------------------
 * Returns true if option --verbose is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_verbose (void) {

  return options[M2C_COMPILER_OPTION_VERBOSE];

} /* end m2c_mkdep_option_verbose */


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_lexer_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --lexer-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_lexer_debug (void) {

  return options[M2C_COMPILER_OPTION_LEXER_DEBUG];

} /* end m2c_mkdep_option_lexer_debug */


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_parser_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --parser-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_parser_debug (void) {

  return options[M2C_COMPILER_OPTION_PARSER_DEBUG];

} /* end m2c_mkdep_option_parser_debug */


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_graph_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --graph is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_graph_required (void) {

  return options[M2C_COMPILER_OPTION_GRAPH_REQUIRED];

} /* end m2c_mkdep_option_graph_required */


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_dollar_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --dollar-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_dollar_identifiers (void) {

  return options[M2C_MKDEP_OPTION_DOLLAR_IDENTIFIERS];

} /* end m2c_mkdep_option_dollar_identifiers */


/* --------------------------------------------------------------------------
 * function m2c_mkdep_option_lowline_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --lowline-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_mkdep_option_lowline_identifiers (void) {

  return options[M2C_MKDEP_OPTION_LOWLINE_IDENTIFIERS];

} /* end m2c_mkdep_option_dollar_identifiers */


#endif /* M2C_MKDEP_OPTION_FLAGS_H */

/* END OF FILE */