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
 * fileio_status.h
 *
 * Public status codes for file IO operations.
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


#ifndef FILEIO_STATUS_H
#define FILEIO_STATUS_H

/* --------------------------------------------------------------------------
 * type fileio_status_t
 * --------------------------------------------------------------------------
 * Status codes for file IO operations.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_FILEIO_STATUS_SUCCESS,
  M2C_FILEIO_INVALID_FILENAME,
  M2C_FILEIO_FILE_NOT_FOUND,
  M2C_FILEIO_ACCESS_DENIED,
  M2C_FILEIO_DEVICE_ERROR,
  /* ... */
} fileio_status_t;


#endif /* FILEIO_STATUS_H */

/* END OF FILE */