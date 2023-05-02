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
  * m2c-pathnames.c                                                           *
  *                                                                           *
  * Aggregator to select platform specific implementation.                    *
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


/* --------------------------------------------------------------------------
 * Select implementation for AmigaOS host platform
 * ----------------------------------------------------------------------- */

#if defined(__amigaos__)
#include "m2-pathnames-amiga.c"


/* --------------------------------------------------------------------------
 * Select implementation for POSIX and Unix-like host platforms
 * ----------------------------------------------------------------------- */

#elif (defined(__MACH__)) || (defined(__unix)) || \
      ((defined(__unix__)) && (!defined(_WIN32)))
#include "m2-pathnames-posix.c"


/* --------------------------------------------------------------------------
 * Select implementation for VAX/VMS and OpenVMS host platforms
 * ----------------------------------------------------------------------- */

#elif defined(VMS) || defined(__VMS)
#include "m2-pathnames-vms.c"


/* --------------------------------------------------------------------------
 * Select implementation for Windows, MS-DOS and OS/2 host platforms
 * ----------------------------------------------------------------------- */

#elif (defined(_WIN32)) || (defined(_WIN64)) || \
      (defined(MSDOS)) || defined(OS2)
#include "m2-pathnames-win.c"


/* --------------------------------------------------------------------------
 * Emit error and abort compilation for unsupported host platforms
 * ----------------------------------------------------------------------- */

#else
#error "Unsupported host platform."
#endif


/* END OF FILE */
