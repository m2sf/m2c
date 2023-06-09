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
 * m2c-productions.h                                                         *
 *                                                                           *
 * Public interface for grammar productions module.                          *
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

#ifndef M2C_PRODUCTIONS_H
#define M2C_PRODUCTIONS_H


/* --------------------------------------------------------------------------
 * type m2c_production_t
 * --------------------------------------------------------------------------
 * Enumerated values representing Modula-2 grammar productions.
 * ----------------------------------------------------------------------- */

#define PROD(_caps, _id, _first, _follow) P_##_caps

typedef enum {  
  P_INVALID,
  #include "production-data.h"  
, P_END_MARK /* marks the end of the enumeration */
} m2c_production_t;

#undef PROD


/* --------------------------------------------------------------------------
 * constant PRODUCTION_COUNT
 * ----------------------------------------------------------------------- */

#define PRODUCTION_COUNT P_END_MARK


/* --------------------------------------------------------------------------
 * macro IS_VALID_PRODUCTION(p)
 * --------------------------------------------------------------------------
 * Returns true it p is a valid production, otherwise false.
 * ----------------------------------------------------------------------- */

#define IS_VALID_PRODUCTION(_p) \
  ((_p > P_INVALID) && (_p < PRODUCTION_COUNT))
  

#endif /* M2C_PRODUCTIONS_H */

/* END OF FILE */