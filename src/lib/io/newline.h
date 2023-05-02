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
 * newline.h
 *
 * Public interface of newline mode management.
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

#ifndef NEWLINE_H
#define NEWLINE_H


/* --------------------------------------------------------------------------
 * Default newline mode
 * ----------------------------------------------------------------------- */

typedef enum {
    NEWLINE_LF,
    NEWLINE_CR,
    NEWLINE_CRLF
} newline_mode_t;


/* --------------------------------------------------------------------------
 * Default newline mode
 * ----------------------------------------------------------------------- */

#define NEWLINE_DEFAULT NEWLINE_LF


/* --------------------------------------------------------------------------
 * procedure newline_set_mode(value)
 * --------------------------------------------------------------------------
 * Sets the newline mode. Undefined values leave setting unchanged.
 * ----------------------------------------------------------------------- */

void newline_set_mode (newline_mode_t mode);


/* --------------------------------------------------------------------------
 * function newline_mode()
 * --------------------------------------------------------------------------
 * Returns the newline mode.
 * ----------------------------------------------------------------------- */

newline_mode_t newline_mode (void);


#endif /* NEWLINE_H */

/* END OF FILE */