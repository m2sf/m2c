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
 * outfile.h
 *
 * Public interface of output file writer.
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

#ifndef OUTFILE_H
#define OUTFILE_H


#include "file-io.h"
#include "interned-strings.h"


/* --------------------------------------------------------------------------
 * type outfile_status_t
 * ----------------------------------------------------------------------- */

typedef file_io_status_t outfile_status_t;


/* --------------------------------------------------------------------------
 * opaque type outfile_t
 * --------------------------------------------------------------------------
 * Opaque pointer type representing an output file.
 * ----------------------------------------------------------------------- */

typedef struct outfile_struct_t *outfile_t;


/* --------------------------------------------------------------------------
 * procedure outfile_open(infile, path, status)
 * --------------------------------------------------------------------------
 * Opens file at path and passes a newly allocated and initialised outfile
 * object back in out-parameter outfile. Passes NULL on failure.
 * ----------------------------------------------------------------------- */

void outfile_open (outfile_t *outfile, char *path, outfile_status_t *status);


/* --------------------------------------------------------------------------
 * procedure outfile_close(infile)
 * --------------------------------------------------------------------------
 * Closes the file associated with outfile and passes NULL in outfile.
 * ----------------------------------------------------------------------- */

void outfile_close (outfile_t *outfile);


/* --------------------------------------------------------------------------
 * procedure outfile_write_char(outfile, ch)
 * --------------------------------------------------------------------------
 * Writes character ch to outfile.
 * ----------------------------------------------------------------------- */

void outfile_write_char (outfile_t *outfile, char ch);


/* --------------------------------------------------------------------------
 * procedure outfile_write_chars(outfile, chars)
 * --------------------------------------------------------------------------
 * Writes a NUL terminated char pointer to outfile.
 * ----------------------------------------------------------------------- */

void outfile_write_chars (outfile_t *outfile, const char *chars);


/* --------------------------------------------------------------------------
 * procedure outfile_write_string(outfile, string)
 * --------------------------------------------------------------------------
 * Writes an interned string to outfile.
 * ----------------------------------------------------------------------- */

void outfile_write_string (outfile_t *outfile, intstr_t *string);


/* --------------------------------------------------------------------------
 * procedure outfile_write_tab(outfile)
 * --------------------------------------------------------------------------
 * Writes tab to outfile. Expands tabs to spaces if tabwidth > 0.
 * ----------------------------------------------------------------------- */

void outfile_write_tab (outfile_t *outfile);


/* --------------------------------------------------------------------------
 * procedure outfile_write_newline(outfile)
 * --------------------------------------------------------------------------
 * Writes newline to outfile.
 * ----------------------------------------------------------------------- */

void outfile_write_newline (outfile_t *outfile);


/* --------------------------------------------------------------------------
 * function outfile_status(outfile)
 * --------------------------------------------------------------------------
 * Returns status of the last operation.
 * ----------------------------------------------------------------------- */

outfile_status_t outfile_status (outfile_t *outfile);


/* --------------------------------------------------------------------------
 * function outfile_line(outfile)
 * --------------------------------------------------------------------------
 * Returns the line number of the current writing position of outfile.
 * ----------------------------------------------------------------------- */

uint_t outfile_line (outfile_t *outfile);


/* --------------------------------------------------------------------------
 * function outfile_column(outfile)
 * --------------------------------------------------------------------------
 * Returns the column number of the current writing position of outfile.
 * ----------------------------------------------------------------------- */

uint_t outfile_column (outfile_t *outfile);


#endif /* OUTFILE_H */

/* END OF FILE */