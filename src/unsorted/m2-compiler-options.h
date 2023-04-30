/* M2C Modula-2 Compiler & Translator
 * Copyright (c) 2015-2023 Benjamin Kowarsch
 *
 * @synopsis
 *
 * M2C is a Modula-2 to C translator for a bootstrap subset of the revised
 * Modula-2 language described in
 *
 * github.com/m2sf/PDFs/blob/master/M2BSK%20Language%20Description.pdf
 *
 * In compiler mode, M2C compiles Modula-2 source via C to object files or
 * executables using the host system's resident C compiler and linker.
 * In translator mode, it translates Modula-2 source to C source.
 *
 * Further information at https://github.com/m2sf/m2c/wiki
 *
 * @file
 *
 * m2-compiler-options.h
 *
 * Public interface for M2C compiler options.
 *
 * @license
 *
 * M2C is free software: you can redistribute and/or modify it under the
 * terms of the GNU Lesser General Public License (LGPL) either version 2.1
 * or at your choice version 3 as published by the Free Software Foundation.
 *
 * M2C is distributed in the hope that it will be useful,  but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  Read the license for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with m2c.  If not, see <https://www.gnu.org/copyleft/lesser.html>.
 */

#ifndef M2C_COMPILER_OPTIONS_H
#define M2C_COMPILER_OPTIONS_H

#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_option_status_t
 * --------------------------------------------------------------------------
 * Status codes for operation m2c_get_cli_args.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_OPTION_STATUS_SUCCESS,
  M2C_OPTION_STATUS_HELP_REQUESTED,
  M2C_OPTION_STATUS_VERSION_REQUESTED,
  M2C_OPTION_STATUS_FAILURE
} m2c_option_status_t;


/* --------------------------------------------------------------------------
 * function m2c_get_cli_args(argc, argv, status)
 * --------------------------------------------------------------------------
 * Processes command line arguments passed in argv, sets option flags and
 * returns a pointer to the argument that contains the input filename.
 * ----------------------------------------------------------------------- */

const char *m2c_get_cli_args
  (int argc, char *argv[], m2c_option_status_t *status);


/* --------------------------------------------------------------------------
 * procedure m2c_print_options()
 * --------------------------------------------------------------------------
 * Prints the current option values to the console.
 * ----------------------------------------------------------------------- */

void m2c_print_options(void);


/* --------------------------------------------------------------------------
 * procedure m2c_print_option_help()
 * --------------------------------------------------------------------------
 * Prints the option help to the console.
 * ----------------------------------------------------------------------- */

void m2c_print_option_help(void);


/* --------------------------------------------------------------------------
 * function m2c_option_verbose()
 * --------------------------------------------------------------------------
 * Returns true if option flag verbose is set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_option_verbose (void);


/* --------------------------------------------------------------------------
 * function m2c_option_lowline_identifiers()
 * --------------------------------------------------------------------------
 * Returns true if option flag lowline_identifiers is set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_option_lowline_identifiers (void);


/* --------------------------------------------------------------------------
 * function m2c_option_lexer_debug()
 * --------------------------------------------------------------------------
 * Returns true if option flag lexer_debug is set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_option_lexer_debug (void);


/* --------------------------------------------------------------------------
 * function m2c_option_parser_debug()
 * --------------------------------------------------------------------------
 * Returns true if option flag parser_debug is set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_option_parser_debug (void);


#endif /* M2C_COMPILER_OPTIONS_H */

/* END OF FILE */
