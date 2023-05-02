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
 * c-resword.h                                                               *
 *                                                                           *
 * Public interface of C reserved word matching module.                      *
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

#ifndef C_RESWORDS_H
#define C_RESWORDS_H

#include <stdbool.h>

/* --------------------------------------------------------------------------
 * compile time parameter C_RESWORDS_INCLUDE_PSEUDO_RESWORDS
 * --------------------------------------------------------------------------
 * If set to 0, function is_c_reswords tests for reserved words only.
 * Otherwise, function is_c_reswords also tests for pseudo reserved words.
 * ----------------------------------------------------------------------- */

#define C_RESWORDS_INCLUDE_PSEUDO_RESWORDS 1


/* --------------------------------------------------------------------------
 * function is_c_resword(cstr)
 * --------------------------------------------------------------------------
 * Tests if cstr matches reserved words and pseudo reserved words depending
 * on macro C_RESWORDS_INCLUDE_PSEUDO_RESWORDS.  Returns true in case of a
 * match, otherwise false.
 *
 * Only reserved words are tested if C_RESWORDS_INCLUDE_PSEUDO_RESWORDS is 0,
 * otherwise both reserved words and pseudo reserved words are tested.
 *
 * Reserved words:
 *   auto, break, case, char, const, continue, default, do, double, else,
 *   enum, extern, float, for, goto, if, inline, int, long, register,
 *   restrict, return, short, signed, sizeof, static, struct, switch,
 *   typedef, union, unsigned, void, volatile, while;
 *
 * Pseudo reserved words:
 *   NULL, alignas, alignof,  bool, complex, exit, false, free, imaginary,
 *   main, malloc, noreturn; 
 * ----------------------------------------------------------------------- */

bool is_c_resword (const char *cstr);


#endif /* C_RESWORDS_H */

/* END OF FILE */
