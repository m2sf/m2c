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
 * iso646.h                                                                  *
 *                                                                           *
 * Public interface of error reporting module.                               *
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

#ifndef ISO646_H
#define ISO646_H

/* --------------------------------------------------------------------------
 * constants
 * ----------------------------------------------------------------------- */

#define ASCII_NUL 0

#define ASCII_EOT 4

#define ASCII_TAB 9

#define ASCII_LF 10

#define ASCII_CR 13

#define ASCII_SPACE 32


/* --------------------------------------------------------------------------
 * macros
 * ----------------------------------------------------------------------- */

#define IS_DIGIT(_ch) \
  ((_ch >= '0') && (_ch <= '9'))

#define IS_NOT_DIGIT(_ch) \
  ((_ch < '0') || (_ch > '9'))

#define IS_LOWER_LETTER(_ch) \
  ((_ch >= 'a') && (_ch <= 'z'))

#define IS_NOT_LOWER_LETTER(_ch) \
  ((_ch < 'a') || (_ch > 'z'))

#define IS_UPPER_LETTER(_ch) \
  ((_ch >= 'A') && (_ch <= 'Z'))

#define IS_NOT_UPPER_LETTER(_ch) \
  ((_ch < 'A') || (_ch > 'Z'))

#define IS_A_TO_F(_ch) \
  ((_ch >= 'A') && (_ch <= 'F'))

#define IS_PRINTABLE_CHAR(_ch) \
  (_ch >= 32) && (_ch < 127))

#define IS_CTRL_CHAR(_ch) \
  ((_ch < 32) || (_ch == 127))

#define IS_LEGAL_CTRL_CHAR(_ch) \
  ((_ch == ASCII_LF) || (_ch == ASCII_TAB))
  
#define IS_ILLEGAL_CTRL_CHAR(_ch) \
  (((_ch < 32) && (_ch != ASCII_LF) && (_ch != ASCII_TAB)) || (_ch == 127))


#endif ISO646_H

/* END OF FILE*/