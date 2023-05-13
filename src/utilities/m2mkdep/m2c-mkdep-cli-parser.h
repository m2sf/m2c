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
 * m2c-mkdep-cli-parser.h                                                    *
 *                                                                           *
 * Public interface of m2mkdep command line interface parser module.         *
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

#ifndef M2C_MKDEP_CLI_PARSER_H
#define M2C_MKDEP_CLI_PARSER_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "string.h"
#include "m2c-common.h"


/* ---------------------------------------------------------------------------
 * type mkdep_cli_parser_status_t
 * ---------------------------------------------------------------------------
 * Enumerated token values representing m2c CLI parser status codes.
 * ------------------------------------------------------------------------ */

typedef enum {
    MKDEP_CLI_PARSER_STATUS_SUCCESS,
    MKDEP_CLI_PARSER_STATUS_HELP_REQUESTED,
    MKDEP_CLI_PARSER_STATUS_VERSION_REQUESTED,
    MKDEP_CLI_PARSER_STATUS_LICENSE_REQUESTED,
    MKDEP_CLI_PARSER_STATUS_ERRORS_ENCOUNTERED  
} mkdep_cli_parser_status_t;


/* ---------------------------------------------------------------------------
 * function mkdep_cli_parse_args()
 * ---------------------------------------------------------------------------
 * Parses command line arguments and sets m2mkdep utility options accordingly.
 * ------------------------------------------------------------------------ */

mkdep_cli_parser_status_t mkdep_cli_parse_args (void);


/* ---------------------------------------------------------------------------
 * function mkdep_cli_source_file()
 * ---------------------------------------------------------------------------
 * Returns a string with the source file argument.
 * ------------------------------------------------------------------------ */

m2c_string_t mkdep_cli_source_file (void);


/* ---------------------------------------------------------------------------
 * function mkdep_cli_error_count()
 * ---------------------------------------------------------------------------
 * Returns the count of errors encountered while parsing the arguments.
 * ------------------------------------------------------------------------ */

uint_t mkdep_cli_error_count (void);


#endif /* M2C_MKDEP_CLI_PARSER_H */

/* END OF FILE */