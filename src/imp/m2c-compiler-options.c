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
 * m2c-compiler-options.c                                                    *
 *                                                                           *
 * Implementation of compiler options module.                                *
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

#include "m2c-compiler-options.h"


/* --------------------------------------------------------------------------
 * Default option flags
 * ----------------------------------------------------------------------- */

#define DEFAULT_OPTIONS { \
  /* verbose */ false, \
  /* lexer_debug */ false, \
  /* parser_debug */ false, \
  /* show_settings */ false, \
  /* errant_semicolon */ false, \
  /* ast_required */ false, \
  /* graph_requre */ false, \
  /* xlat_required */ true, \
  /* obj_required */ false, \
  /* preserve_comments */ true, \
  /* lowline_identifiers */ false, \
  /* dollar_identifiers */ false \
} /* DEFAULT_OPTIONS */


/* --------------------------------------------------------------------------
 * hidden variable compiler_option
 * ----------------------------------------------------------------------- */

#define OPTION_COUNT M2C_COMPILER_OPTION_END_MARK

m2t_compiler_option_t compiler_option[OPTION_COUNT] = DEFAULT_OPTIONS;


/* --------------------------------------------------------------------------
 * procedure m2c_compiler_option_set(option, value)
 * ---------------------------------------------------------------------------
 * Sets the given option to the given boolean value.
 * ----------------------------------------------------------------------- */

void m2c_compiler_option_set (m2c_compiler_option_t option, bool value) {
  if (option < OPTION_COUNT) {
    compiler_option[option] = value;
  } /* end if */
} /* end m2c_compiler_option_set */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_verbose()
 * ---------------------------------------------------------------------------
 * Returns true if option --verbose is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_verbose (void) {
  return compiler_option[M2C_COMPILER_OPTION_VERBOSE];
} /* end m2c_compiler_option_verbose */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_lexer_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --lexer-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_lexer_debug (void) {
  return compiler_option[M2C_COMPILER_OPTION_LEXER_DEBUG];
} /* end m2c_compiler_option_lexer_debug */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_parser_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --parser-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_parser_debug (void) {
  return compiler_option[M2C_COMPILER_OPTION_PARSER_DEBUG];
} /* end m2c_compiler_option_parser_debug */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_show_settings()
 * ---------------------------------------------------------------------------
 * Returns true if option --show-settings is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_show_settings (void) {
  return compiler_option[M2C_COMPILER_OPTION_SHOW_SETTINGS];
} /* end m2c_compiler_option_show_settings */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_errant_semicolons()
 * ---------------------------------------------------------------------------
 * Returns true if option --errant-semicolons is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_errant_semicolons (void) {
  return compiler_option[M2C_COMPILER_OPTION_ERRANT_SEMICOLONS];
} /* end m2c_compiler_option_errant_semicolons */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_ast_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --ast is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_ast_required (void) {
  return compiler_option[M2C_COMPILER_OPTION_AST_REQUIRED];
} /* end m2c_compiler_option_ast_required */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_graph_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --graph is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_graph_required (void) {
  return compiler_option[M2C_COMPILER_OPTION_GRAPH_REQUIRED];
} /* end m2c_compiler_option_graph_required */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_xlat_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --xlat is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_xlat_required (void) {
  return compiler_option[M2C_COMPILER_OPTION_XLAT_REQUIRED];
} /* end m2c_compiler_option_xlat_required */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_obj_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --obj is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_obj_required (void) {
  return compiler_option[M2C_COMPILER_OPTION_OBJ_REQUIRED];
} /* end m2c_compiler_option_obj_required */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_preserve_comments()
 * ---------------------------------------------------------------------------
 * Returns true if option --preserve-comments is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_preserve_comments (void) {
  return compiler_option[M2C_COMPILER_OPTION_PRESERVE_COMMENTS];
} /* end m2c_compiler_option_preserve_comments */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_lowline_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --lowline-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_lowline_identifiers (void) {
  return compiler_option[M2C_COMPILER_OPTION_LOWLINE_IDENTIFIERS];
} /* end m2c_compiler_option_lowline_identifiers */


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_dollar_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --dollar-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_dollar_identifiers (void) {
  return compiler_option[M2C_COMPILER_OPTION_DOLLAR_IDENTIFIERS];
} /* end m2c_compiler_option_dollar_identifiers */


/* --------------------------------------------------------------------------
 * procedure m2c_compiler_option_print_settings()
 * ---------------------------------------------------------------------------
 * Prints the current settings to the console.
 * ----------------------------------------------------------------------- */

void m2c_compiler_option_print_settings (void) {

  /* TO DO */

} /* end m2c_compiler_option_print_settings */


/* END OF FILE */