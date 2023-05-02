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
 * m2c-parser.h                                                              *
 *                                                                           *
 * Public interface of Modula-2 parser module.                               *
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

#ifndef M2C_PARSER_H
#define M2C_PARSER_H

#include "m2c-common.h"

#include "m2c-ast.h"
#include "m2c-stats.h"


/* --------------------------------------------------------------------------
 * type m2c_sourcetype_t
 * --------------------------------------------------------------------------
 * Enumeration representing type of input source.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_ANY_SOURCE,
  M2C_DEF_SOURCE,
  M2C_MOD_SOURCE
} m2c_sourcetype_t;

#define M2C_FIRST_SOURCETYPE M2C_ANY_SOURCE
#define M2C_LAST_SOURCETYPE M2C_MOD_SOURCE


/* --------------------------------------------------------------------------
 * type m2c_parser_status_t
 * --------------------------------------------------------------------------
 * Status codes for operations on type m2c_parser_t.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_PARSER_STATUS_SUCCESS,
  M2C_PARSER_STATUS_INVALID_REFERENCE,
  M2C_PARSER_STATUS_INVALID_SOURCETYPE,
  M2C_PARSER_STATUS_ALLOCATION_FAILED
} m2c_parser_status_t;


/* --------------------------------------------------------------------------
 * function m2c_parse_file(srctype, srcpath, ast, stats, status)
 * --------------------------------------------------------------------------
 * Parses a Modula-2 source file represented by srcpath and returns status.
 * Builds an abstract syntax tree and passes it back in ast, or NULL upon
 * failure.  Collects simple statistics and passes them back in stats.
 * ----------------------------------------------------------------------- */
 
 void m2c_parse_file
   (m2c_sourcetype_t srctype,      /* in */
    const char *srcpath,           /* in */
    m2c_ast_t *ast,                /* out */
    m2c_stats_t *stats,            /* out */
    m2c_parser_status_t *status);  /* out */

#endif /* M2C_PARSER_H */

/* END OF FILE */
