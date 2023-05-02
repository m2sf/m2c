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
  * m2c-pathnames.h                                                           *
  *                                                                           *
  * Platform independent public interface to pathname and filename handling.  *
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

#ifndef M2C_PATHNAMES_H
#define M2C_PATHNAMES_H

#include "m2-common.h"
#include <stdbool.h>

/* --------------------------------------------------------------------------
 * Pathname grammar
 * --------------------------------------------------------------------------
 * Pathnames are host platform specific.  EBNF grammars are provided in
 * platform specific implementation files under the platform directory.
 * Reserved platform names are listed in file m2c-host-platforms.txt
 * ----------------------------------------------------------------------- */


/* --------------------------------------------------------------------------
 * Status
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_PATHNAME_STATUS_SUCCESS,
  M2C_PATHNAME_STATUS_INVALID_PATH,
  M2C_PATHNAME_STATUS_INVALID_FILENAME,
  M2C_PATHNAME_STATUS_INVALID_REFERENCE,
  M2C_PATHNAME_STATUS_ALLOCATION_FAILED
} m2c_pathname_status_t;


/* --------------------------------------------------------------------------
 * function split_pathname(path, dirpath, filename, chars_processed)
 * --------------------------------------------------------------------------
 * Verifies path against the host system's prevailing pathname grammar and
 * returns a status code.  If path is valid, its directory path component and
 * filename component are copied to newly allocated NUL terminated C strings
 * and passed back in out-parameters dirpath and filename respectively.  If
 * path does not contain a directory path component, NULL is passed back in
 * dirpath.  If path does not contain a filename component, NULL is passed
 * back in filename.  However, if NULL is passed in for an out-parameter,
 * the out-parameter is ignored and no value is passed back in it.  The index
 * of the last processed character is passed back in chars_processed.  Upon
 * success it represents the length of path.  Upon failure, it represents
 * the index of the first offending character found in path.
 * ----------------------------------------------------------------------- */

m2c_pathname_status_t split_pathname
  (const char *path,         /* in, may not be NULL */
   char_ptr_t *dirpath,      /* out, pass in NULL to ignore */
   char_ptr_t *filename,     /* out, pass in NULL to ignore */
   uint_t *chars_processed); /* out, pass in NULL to ignore */


/* --------------------------------------------------------------------------
 * function is_valid_pathname(path)
 * --------------------------------------------------------------------------
 * Returns true if pathname is a valid pathname, otherwise false.
 * ----------------------------------------------------------------------- */

bool is_valid_pathname (const char *path);


/* --------------------------------------------------------------------------
 * function split_filename(filename, basename, suffix, chars_processed)
 * --------------------------------------------------------------------------
 * Verifies filename against the host system's prevailing pathname grammar
 * and returns a status code.  If filename is valid, its basename and suffix
 * components are copied to newly allocated NUL terminated C strings and
 * passed back in out-parameters basename and suffix respectively.  If
 * filename does not contain a suffix component, NULL is passed back in
 * suffix.  However, if NULL is passed in for an out-parameter, the out-
 * parameter is ignored and no value is passed back in it.  The index of the
 * last processed character is passed back in chars_processed.  Upon success
 * it represents the length of filename.  Upon failure, it represents the
 * index of the first offending character found in filename.
 * ----------------------------------------------------------------------- */

m2c_pathname_status_t split_filename
  (const char *filename,     /* in, may not be NULL */
   char_ptr_t *basename,     /* out, pass in NULL to ignore */
   char_ptr_t *suffix,       /* out, pass in NULL to ignore */
   uint_t *chars_processed); /* out, pass in NULL to ignore */


/* --------------------------------------------------------------------------
 * function is_valid_filename(filename)
 * --------------------------------------------------------------------------
 * Returns true if filename is a valid filename, otherwise false.
 * ----------------------------------------------------------------------- */

bool is_valid_filename (const char *filename);


/* --------------------------------------------------------------------------
 * function is_def_suffix(suffix)
 * --------------------------------------------------------------------------
 * Returns true if suffix is ".def" or ".DEF", otherwise false.
 * ----------------------------------------------------------------------- */

bool is_def_suffix (const char *suffix);


/* --------------------------------------------------------------------------
 * function is_mod_suffix(suffix)
 * --------------------------------------------------------------------------
 * Returns true if suffix is ".mod" or ".MOD", otherwise false.
 * ----------------------------------------------------------------------- */

bool is_mod_suffix (const char *suffix);


/* --------------------------------------------------------------------------
 * function new_path_w_components(dirpath, basename, suffix)
 * --------------------------------------------------------------------------
 * Returns a newly allocated NUL terminated C string containing a pathname
 * composed of components dirpath, basename and file suffix.  Returns NULL
 * if any of dirpath or basename is NULL or empty or if allocation failed.
 * ----------------------------------------------------------------------- */

const char *new_path_w_components
  (const char *dirpath, const char *basename, const char *suffix);


#endif /* M2C_PATHNAMES_H */

/* END OF FILE */