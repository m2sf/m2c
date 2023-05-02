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
 * m2c-exl-parser.h                                                          *
 *                                                                           *
 * Public interface of export list file parser module.                       *
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

#ifndef M2C_EXL_PARSER_H
#define M2C_EXL_PARSER_H

#include "m2c-common.h"


/* --------------------------------------------------------------------------
 * type m2c_exl_parser_status_t
 * --------------------------------------------------------------------------
 * Status codes for operations on type m2c_exl_parser_t.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_EXL_PARSER_STATUS_SUCCESS,
  M2C_EXL_PARSER_STATUS_INVALID_REFERENCE,
  M2C_EXL_PARSER_STATUS_ALLOCATION_FAILED
} m2c_exl_parser_status_t;


/* --------------------------------------------------------------------------
 * function m2c_exl_parse_file(exlpath, list, status)
 * --------------------------------------------------------------------------
 * Parses the  export list file  referenced by exlpath,  constructs a list of
 * exported identifiers  by kind  and  passes it back in  list,  or NULL upon
 * failure.  The status of the operation is passed back in status.
 * ----------------------------------------------------------------------- */
 
 void m2c_exl_parse_file
   (const char *exlpath,                /* in */
    m2c_exl_list_t *list,               /* out */
    m2c_exl_parser_status_t *status);   /* out */

#endif /* M2C_EXL_PARSER_H */

/* END OF FILE */