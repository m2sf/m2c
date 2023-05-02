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
 * source files to C,  then compiles the resulting C source files  to object *
 * and executable files using the host system's resident C compiler.         *
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

#ifndef FILEIO_H
#define FILEIO_H


#include "fileio-status.h"

#include <stdbool>


/* --------------------------------------------------------------------------
 * opaque type file_t
 * --------------------------------------------------------------------------
 * Opaque pointer type representing a file.
 * ----------------------------------------------------------------------- */

typedef struct file_struct_t *file_t;


/* --------------------------------------------------------------------------
 * type fileio_mode_t
 * --------------------------------------------------------------------------
 * File access modes.
 * ----------------------------------------------------------------------- */

typedef enum {
  FILEIO_MODE_READ,
  FILEIO_MODE_WRITE,
  FILEIO_MODE_APPEND
} fileio_mode_t;


/* Operations */

/* --------------------------------------------------------------------------
 * Support for an operation depends on the mode in which the file has
 * been opened. For details, see the table below.
 *
 * operation                   | supported in file mode | sets
 *                             | Read    Write   Append | status
 * ----------------------------+------------------------+-------
 * fileio_open                 | yes     yes     yes    | yes
 * fileio_close                | yes     yes     yes    | n/a
 * ----------------------------+------------------------+-------
 * fileio_get_mode             | yes     yes     yes    | no
 * fileio_status               | yes     yes     yes    | no
 * fileio_insert_buffer_full   | yes     no*     no*    | no
 * fileio_eof                  | yes     no*     no*    | no
 * ----------------------------+------------------------+-------
 * fileio_read_char            | yes     no      no     | yes
 * fileio_insert_char          | yes     no      no     | yes
 * fileio_read_chars           | yes     no      no     | yes
 * fileio_read_octet           | yes     no      no     | yes
 * fileio_insert_octet         | yes     no      no     | yes
 * fileio_read_octets          | yes     no      no     | yes
 * ----------------------------+------------------------+-------
 * fileio_write_char           | no      yes     yes    | yes
 * fileio_write_chars          | no      yes     yes    | yes
 * fileio_write_octet          | no      yes     yes    | yes
 * fileio_write_octets         | no      yes     yes    | yes
 * ----------------------------+------------------------+-------
 * key: trailing * = always returns FALSE, result is meaningless.
 * ----------------------------------------------------------------------- */


/* Open and close */

/* --------------------------------------------------------------------------
 * procedure fileio_open(file, path, mode, status)
 * --------------------------------------------------------------------------
 * Opens the file at path in the given mode. Passes a new file object in file
 * and the status in status.  If the file does not exist, it will be created
 * when opened in write mode, otherwise FileNotFound is passed back in status.
 * When opening an already existing file in write mode, all of its current
 * contents will be overwritten.
 * ----------------------------------------------------------------------- */

void fileio_open
  (file_t *file, char *path, fileio_mode_t mode, fileio_status_t *status);


/* --------------------------------------------------------------------------
 * procedure fileio_close(infile)
 * --------------------------------------------------------------------------
 * Closes the file associated with outfile and passes NULL in outfile.
 * ----------------------------------------------------------------------- */

void fileio_close (file_t *file);


/* Introspection */

/* --------------------------------------------------------------------------
 * procedure fileio_get_mode(file, mode)
 * --------------------------------------------------------------------------
 * Passes the mode of file in mode.
 * ----------------------------------------------------------------------- */

void fileio_get_mode (file_t *file, fileio_mode_t *mode);


/* --------------------------------------------------------------------------
 * function fileio_status(file)
 * --------------------------------------------------------------------------
 * Returns the status of the last operation on file.
 * ----------------------------------------------------------------------- */

fileio_status_t fileio_status (file_t *file);


/* --------------------------------------------------------------------------
 * function fileio_insert_buffer_full(file)
 * --------------------------------------------------------------------------
 * Returns true if the internal insert buffer of file is full, else false.
 * ----------------------------------------------------------------------- */

bool fileio_insert_buffer_full (file_t *file);


/* --------------------------------------------------------------------------
 * function fileio_eof(infile)
 * --------------------------------------------------------------------------
 * Returns true if file has reached the end of the file, else false.
 * ----------------------------------------------------------------------- */

bool fileio_eof (file_t *file);


/* Read and unread operations */

/* --------------------------------------------------------------------------
 * procedure fileio_read_char(file, ch)
 * --------------------------------------------------------------------------
 * If the internal insert buffer of file is not empty, removes the first
 * character from the buffer and returns it in out-parameter ch. Otherwise,
 * if the internal insert buffer of file is empty, reads one character at
 * the current reading position of file and passes it in ch, or ASCII EOT
 * if the end of file had already been reached upon entry into ReadChar.
 * ----------------------------------------------------------------------- */

void fileio_read_char (file_t *file, char *ch);


/* --------------------------------------------------------------------------
 * procedure InsertChar(file, ch)
 * --------------------------------------------------------------------------
 * Inserts character ch into the internal insert buffer of file unless the
 * buffer is full. Sets file's status to FILEIO_INSERT_BUFFER_FULL if full.
 * ----------------------------------------------------------------------- */

void fileio_insert_char (file_t *file, char ch);


/* --------------------------------------------------------------------------
 * procedure fileio_read_chars(file, bufsize, buffer, chars_read)
 * --------------------------------------------------------------------------
 * If the internal insert buffer of file is not empty, removes as many chars
 * from the insert buffer as will fit into out-parameter buffer   and copies
 * them to out-parameter buffer.  If and once  the internal insert buffer is
 * empty,  reads contents  starting at  the current reading position of file
 * into out-parameter buffer  until either  the pen-ultimate index of buffer
 * is written  or eof  is reached.  Out-parameter buffer  is then terminated
 * with ASCII NUL.  The number of characters copied is passed in chars_read.
 * ----------------------------------------------------------------------- */

void fileio_read_chars
  (file_t *file, uint_t bufsize, char *buffer, uint_t *chars_read);


/* Write operations */

/* --------------------------------------------------------------------------
 * procedure fileio_write_char(file, ch)
 * --------------------------------------------------------------------------
 * Writes character ch to file at the current writing position.
 * ----------------------------------------------------------------------- */

void fileio_write_char (file_t *file, char ch);


/* ---------------------------------------------------------------------------
 * procedure fileio_write_chars(file, buffer, chars_written)
 * ---------------------------------------------------------------------------
 * Writes the contents of buffer up to and excluding the first ASCII NUL
 * character code to file at the current writing position. The number of
 * characters actually written is passed in chars_written.
 * ------------------------------------------------------------------------ */

void fileio_write_chars
  (file_t *file, const char *buffer, uint_t *chars_written);


#endif /* FILEIO_H */

/* END OF FILE */