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
 * m2c-import-parser.h                                                       *
 *                                                                           *
 * Public interface of import section parser for dependency graph.           *
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

#ifndef M2C_IMPORT_PARSER_H
#define M2C_IMPORT_PARSER_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "../lib/strings/interned-strings.h"

#include "m2c-dep-list.h"
#include "m2c-parser-status.h"


/* --------------------------------------------------------------------------
 * function m2c_parse_imports(srcpath, dep_list, status)
 * --------------------------------------------------------------------------
 * Parses  the import section of the Modula-2 source file located at src_path
 * and  passes a newly allocated dependency list with identifiers of imported
 * modules back in dep_list, or NULL on failure.  The status of the operation
 * is passed back in status.
 * ----------------------------------------------------------------------- */
 
 void m2c_parse_imports
   (const intstr_t src_path,        /* in */
    m2c_dep_list_t *dep_list,       /* out */
    m2c_parser_status_t *status);   /* out */


#endif /* M2C_IMPORT_PARSER_H */

/* END OF FILE */