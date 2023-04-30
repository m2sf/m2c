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
 * m2c-cli-parser.h
 *
 * Public interface of M2C command line parser module.
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

#ifndef M2C_CLI_PARSER_H
#define M2C_CLI_PARSER_H

#include "string.h"


/* ---------------------------------------------------------------------------
 * type cli_parser_status_t
 * ---------------------------------------------------------------------------
 * Enumerated token values representing CLI parser status codes.
 * ------------------------------------------------------------------------ */

typedef enum {
    CLI_PARSER_STATUS_SUCCESS,
    CLI_PARSER_STATUS_HELP_REQUESTED,
    CLI_PARSER_STATUS_VERSION_REQUESTED,
    CLI_PARSER_STATUS_LICENSE_REQUESTED,
    CLI_PARSER_STATUS_ERRORS_ENCOUNTERED  
} cli_parser_status_t;


/* ---------------------------------------------------------------------------
 * function cli_parse_args()
 * ---------------------------------------------------------------------------
 * Parses command line arguments and sets compiler options accordingly.
 * ------------------------------------------------------------------------ */

cli_parser_status_t cli_parse_args (void);


/* ---------------------------------------------------------------------------
 * function cli_source_file()
 * ---------------------------------------------------------------------------
 * Returns a string with the source file argument.
 * ------------------------------------------------------------------------ */

m2c_string_t cli_source_file (void);


/* ---------------------------------------------------------------------------
 * function cli_error_count()
 * ---------------------------------------------------------------------------
 * Returns the count of errors encountered while parsing the arguments.
 * ------------------------------------------------------------------------ */

uint_t cli_error_count (void);


#endif /* M2C_CLI_PARSER_H */

/* END OF FILE */