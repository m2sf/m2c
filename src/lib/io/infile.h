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
 * basic-fileio.h                                                            *
 *                                                                           *
 * Public interface of basic file IO module.                                 *
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

#ifndef INFILE_H
#define INFILE_H


#include "fileio-status.h"
#include "interned-strings.h"
#include "m2c-build-params.h"

#include <stdbool>


/* --------------------------------------------------------------------------
 * Maximum line length
 * ----------------------------------------------------------------------- */

#define INFILE_MAX_LINE_LENGTH M2C_MAX_INFILE_COLUMNS


/* --------------------------------------------------------------------------
 * type infile_status_t
 * ----------------------------------------------------------------------- */

typedef file_io_status_t infile_status_t;


/* --------------------------------------------------------------------------
 * opaque type infile_t
 * --------------------------------------------------------------------------
 * Opaque pointer type representing an input file.
 * ----------------------------------------------------------------------- */

typedef struct infile_struct_t *infile_t;


/* --------------------------------------------------------------------------
 * procedure infile_open(infile, path, status)
 * --------------------------------------------------------------------------
 * Opens the file at path and passes a newly allocated and initialised infile
 * object back in out-parameter infile. Passes NULL on failure.
 * ----------------------------------------------------------------------- */

void infile_open (infile_t *infile, char *path, infile_status_t *status);


/* --------------------------------------------------------------------------
 * procedure infile_close(infile)
 * --------------------------------------------------------------------------
 * Closes the file associated with infile and passes NULL in infile.
 * ----------------------------------------------------------------------- */

void infile_close (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_consume_char(infile)
 * --------------------------------------------------------------------------
 * Consumes the current lookahead character  in infile,  updating the rolling
 * hash value  used for file digest calculation,  and  returns the  resulting
 * new lookahead character without consuming it.
 * ----------------------------------------------------------------------- */

char infile_consume_char (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_skip_char(infile)
 * --------------------------------------------------------------------------
 * Consumes the current lookahead character in infile  WITHOUT  updating  the
 * rolling hash value  used for file digest calculation,  and returns the re-
 * sulting  new lookahead character without consuming it.
 * ----------------------------------------------------------------------- */

char infile_skip_char (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_lookahead_char(infile)
 * --------------------------------------------------------------------------
 * Returns the current lookahead char in infile without consuming any char.
 * ----------------------------------------------------------------------- */

char infile_lookahead_char (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_la2_char(infile)
 * --------------------------------------------------------------------------
 * Returns the 2nd lookahead char in infile without consuming any char.
 * ----------------------------------------------------------------------- */

char infile_la2_char (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_status(infile)
 * --------------------------------------------------------------------------
 * Returns status of the last operation.
 * ----------------------------------------------------------------------- */

char infile_status (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_eof(infile)
 * --------------------------------------------------------------------------
 * Returns true if infile has reached the end of the file, else false.
 * ----------------------------------------------------------------------- */

bool infile_eof (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_line(infile)
 * --------------------------------------------------------------------------
 * Returns the line number of the current reading position of infile.
 * ----------------------------------------------------------------------- */

uint_t infile_line (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_column(infile)
 * --------------------------------------------------------------------------
 * Returns the column number of the current reading position of infile.
 * ----------------------------------------------------------------------- */

uint_t infile_column (infile_t *infile);


/* --------------------------------------------------------------------------
 * procedure infile_mark_lexeme(infile)
 * --------------------------------------------------------------------------
 * Marks the current lookahead character as the start of a lexeme.
 * ----------------------------------------------------------------------- */

void infile_mark_lexeme (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_lexeme(infile)
 * --------------------------------------------------------------------------
 * Returns the current lexeme.  Returns NULL if no lexeme has been marked, or
 * if no chars have been consumed since infile_mark_lexeme() has been called.
 * ----------------------------------------------------------------------- */

intstr_t *infile_lexeme (infile_t *infile);


/* --------------------------------------------------------------------------
 * function infile_digest(infile)
 * --------------------------------------------------------------------------
 * Returns a newly allocated NUL terminated character string with the digest
 * of infile.  Returns NULL if the end of infile has not been reached yet.
 * ----------------------------------------------------------------------- */

const char *infile_digest (infile_t *infile);


/* --------------------------------------------------------------------------
 * type print_handler_t
 * --------------------------------------------------------------------------
 * Function pointer to print char function used by infile_print_line.
 * ----------------------------------------------------------------------- */

typedef void (*print_handler_t) (char ch);


/* --------------------------------------------------------------------------
 * function infile_print_handler_installed()
 * --------------------------------------------------------------------------
 * Returns true if a print handler has been installed, else false.
 * ----------------------------------------------------------------------- */

bool infile_print_handler_installed (void);


/* --------------------------------------------------------------------------
 * procedure infile_install_print_handler(infile, handler)
 * --------------------------------------------------------------------------
 * Installs a  print handler  for use by procedure inline_print_line.  A call
 * infile_install_print_handler(&console_write_char) will  install  procedure
 * console_write_char as print handler to be used by inline_print_line.
 * ----------------------------------------------------------------------- */

void infile_install_print_handler (print_handler_t handler);


/* --------------------------------------------------------------------------
 * procedure infile_print_line(infile, line_no)
 * --------------------------------------------------------------------------
 * Prints the line with the given line number  within infile  using the print
 * handler installed by procedure  infile_install_print_handler.  Will return
 * without action if no print handler has been installed prior to the call. 
 * ----------------------------------------------------------------------- */

void infile_print_line (infile_t *infile, uint_t line_no);


#endif /* INFILE_H */

/* END OF FILE */