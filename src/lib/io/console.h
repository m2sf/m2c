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
 * console.h
 *
 * Public interface for console output.
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

#ifndef CONSOLE_H
#define CONSOLE_H


#include "interned-strings.h"

#include <stdbool>


/* Read operations */

/* --------------------------------------------------------------------------
 * procedure console_read_char(ch)
 * --------------------------------------------------------------------------
 * Reads one character from the console and passes it back in ch.
 * ----------------------------------------------------------------------- */

void console_read_char (char *ch);


/* --------------------------------------------------------------------------
 * procedure console_read_chars(n, chars)
 * --------------------------------------------------------------------------
 * Reads up to n characters from the console and passes them back
 * in chars.  NEWLINE terminates input and will not be copied to chars.
 * ----------------------------------------------------------------------- */

void console_read_chars (uint_t n, char *ch);


/* Write operations */

/* --------------------------------------------------------------------------
 * procedure console_write_char(ch)
 * --------------------------------------------------------------------------
 * Writes character ch to the console.
 * ----------------------------------------------------------------------- */

void console_write_char (char ch);


/* --------------------------------------------------------------------------
 * procedure console_write_chars(chars)
 * --------------------------------------------------------------------------
 * Writes a NUL terminated char pointer to the console.
 * ----------------------------------------------------------------------- */

void console_write_chars (const char *chars);


/* --------------------------------------------------------------------------
 * procedure console_write_string(string)
 * --------------------------------------------------------------------------
 * Writes an interned string to the console.
 * ----------------------------------------------------------------------- */

void console_write_string (intstr_t *string);


/* --------------------------------------------------------------------------
 * procedure console_write_chars_and_string(chars, string)
 * --------------------------------------------------------------------------
 * Writes a NUL terminated char pointer and string to the console.
 * ----------------------------------------------------------------------- */

void console_write_chars_and_string (const char *chars, intstr_t *string);


/* --------------------------------------------------------------------------
 * procedure console_write_newline()
 * --------------------------------------------------------------------------
 * Writes newline to the console.
 * ----------------------------------------------------------------------- */

void console_write_newline (void);


#endif /* CONSOLE_H */

/* END OF FILE */