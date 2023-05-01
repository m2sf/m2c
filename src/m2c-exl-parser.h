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
 * m2c-exl-parser.h
 *
 * Public interface of M2C export list file parser module.
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