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
 * m2c-compiler-options.h
 *
 * Public interface of M2C compiler options module.
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

#ifndef M2C_COMPILER_OPTIONS_H
#define M2C_COMPILER_OPTIONS_H

#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_token_t
 * --------------------------------------------------------------------------
 * Enumerated token values representing Modula-2 terminal symbols.
 * ----------------------------------------------------------------------- */

typedef enum {
  
  /* Diagnostic Options */

  M2C_COMPILER_OPTION_VERBOSE,             /* --verbose */
  M2C_COMPILER_OPTION_LEXER_DEBUG,         /* --lexer-debug */
  M2C_COMPILER_OPTION_PARSER_DEBUG,        /* --parser-debug */
  M2C_COMPILER_OPTION_SHOW_SETTINGS,       /* --show-settings */
  M2C_COMPILER_OPTION_ERRANT_SEMICOLONS,   /* --errant-semicolons */

  /* Build Product Options */
  
  M2C_COMPILER_OPTION_AST_REQURED,         /* --ast, --no-ast */
  M2C_COMPILER_OPTION_GRAPH_REQUIRED,      /* --graph, --no-graph */
  M2C_COMPILER_OPTION_XLAT_REQUIRED,       /* --xlat, --no-xlat */
  M2C_COMPILER_OPTION_OBJ_REQUIRED,        /* --obj, --no-obj */

  /* Comment Options */
  
  /* --preserve-comments, --strip-comments */
  M2C_COMPILER_OPTION_PRESERVE_COMMENTS,

  /* Capability Options */
  
  /* --dollar-identifiers, --no-dollar-identifiers */
  M2C_COMPILER_OPTION_DOLLAR_IDENTIFIERS,

  /* --lowline-identifiers, --no-lowline-identifiers */
  M2C_COMPILER_OPTION_LOWLINE_IDENTIFIERS,

  /* Enumeration Terminator */

  M2C_COMPILER_OPTION_END_MARK

} m2c_compiler_option_t;


/* --------------------------------------------------------------------------
 * procedure m2c_compiler_option_set(option, value)
 * ---------------------------------------------------------------------------
 * Sets the given option to the given boolean value.
 * ----------------------------------------------------------------------- */

void m2c_compiler_option_set (m2c_compiler_option_t option, bool value);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_verbose()
 * ---------------------------------------------------------------------------
 * Returns true if option --verbose is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_verbose (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_lexer_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --lexer-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_lexer_debug (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_parser_debug()
 * ---------------------------------------------------------------------------
 * Returns true if option --parser-debug is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_parser_debug (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_show_settings()
 * ---------------------------------------------------------------------------
 * Returns true if option --show-settings is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_show_settings (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_errant_semicolons()
 * ---------------------------------------------------------------------------
 * Returns true if option --errant-semicolons is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_errant_semicolons (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_ast_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --ast is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_ast_required (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_graph_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --graph is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_graph_required (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_xlat_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --xlat is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_xlat_required (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_obj_required()
 * ---------------------------------------------------------------------------
 * Returns true if option --obj is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_obj_required (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_preserve_comments()
 * ---------------------------------------------------------------------------
 * Returns true if option --preserve-comments is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_preserve_comments (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_dollar_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --dollar-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_dollar_identifiers (void);


/* --------------------------------------------------------------------------
 * function m2c_compiler_option_lowline_identifiers()
 * ---------------------------------------------------------------------------
 * Returns true if option --lowline-identifiers is turned on, else false.
 * ----------------------------------------------------------------------- */

bool m2c_compiler_option_lowline_identifiers (void);


/* --------------------------------------------------------------------------
 * procedure m2c_compiler_option_print_settings()
 * ---------------------------------------------------------------------------
 * Prints the current settings to the console.
 * ----------------------------------------------------------------------- */

void m2c_compiler_option_print_settings (void);


#endif M2C_COMPILER_OPTIONS_H

/* END OF FILE */