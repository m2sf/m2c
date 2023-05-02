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
  * m2c-pathname-policy.h                                                     *
  *                                                                           *
  * Pathname policy settings, required by module m2c-pathnames.               *
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

#ifndef M2C_PATHNAME_POLICY_H
#define M2C_PATHNAME_POLICY_H

/* --------------------------------------------------------------------------
 * All settings are applied at compile time and cannot be changed at runtime!
 * ----------------------------------------------------------------------- */


/* --------------------------------------------------------------------------
 * Use of special characters in pathname components
 * --------------------------------------------------------------------------
 * The following constants define whether certain special characters may or
 * may not occur legally within the basename of a filename or directory:
 *
 * M2C_PATHCOMP_MAY_CONTAIN_PERIOD
 *   enable (1) or disable (0) use of period ('.') in a pathname component.
 *
 *   Restrictions
 *   - a period may not be trailing nor consecutive,
 *   - a period must be escaped with a caret ('^') on OpenVMS.
 *
 * M2C_PATHCOMP_MAY_CONTAIN_SPACE
 *   enable (1) or disable (0) use of space (' ') in a pathname component.
 *
 *   Restrictions
 *   - a space may not be leading nor trailing nor consecutive,
 *   - a space must be escaped with a caret ('^') on OpenVMS.
 *
 * M2C_PATHCOMP_MAY_CONTAIN_MINUS
 *   enable (1) or disable (0) use of minus ('-') in a pathname component.
 *
 *   Restrictions
 *   - a minus may not be leading.
 *
 * M2C_PATHCOMP_MAY_CONTAIN_TILDE
 *   enable (1) or disable (0) use of tilde ('~') in a pathname component.
 *
 *   Restrictions
 *   - a tilde may not be leading.
 * ----------------------------------------------------------------------- */


/* --------------------------------------------------------------------------
 * Settings for MacOS X
 * ----------------------------------------------------------------------- */

#if defined(__APPLE__)
#define M2C_PATHCOMP_MAY_CONTAIN_PERIOD 1
#define M2C_PATHCOMP_MAY_CONTAIN_SPACE 1
#define M2C_PATHCOMP_MAY_CONTAIN_MINUS 1
#define M2C_PATHCOMP_MAY_CONTAIN_TILDE 0


/* --------------------------------------------------------------------------
 * Settings for AmigaOS
 * ----------------------------------------------------------------------- */

#elif defined(__amigaos)
#define M2C_PATHCOMP_MAY_CONTAIN_PERIOD 1
#define M2C_PATHCOMP_MAY_CONTAIN_SPACE 1
#define M2C_PATHCOMP_MAY_CONTAIN_MINUS 1
#define M2C_PATHCOMP_MAY_CONTAIN_TILDE 0


/* --------------------------------------------------------------------------
 * Settings for MS-DOS and OS/2
 * ----------------------------------------------------------------------- */

#elif defined(MSDOS) || defined(OS2)
#define M2C_PATHCOMP_MAY_CONTAIN_PERIOD 0
#define M2C_PATHCOMP_MAY_CONTAIN_SPACE 0
#define M2C_PATHCOMP_MAY_CONTAIN_MINUS 1
#define M2C_PATHCOMP_MAY_CONTAIN_TILDE 1


/* --------------------------------------------------------------------------
 * Settings for OpenVMS
 * ----------------------------------------------------------------------- */

#elif defined(VMS) || defined(__VMS)
#define M2C_PATHCOMP_MAY_CONTAIN_PERIOD 0
#define M2C_PATHCOMP_MAY_CONTAIN_SPACE 0
#define M2C_PATHCOMP_MAY_CONTAIN_MINUS 1
#define M2C_PATHCOMP_MAY_CONTAIN_TILDE 1


/* --------------------------------------------------------------------------
 * Settings for Microsoft Windows
 * ----------------------------------------------------------------------- */

#elif defined(_WIN32) || defined(_WIN64)
#define M2C_PATHCOMP_MAY_CONTAIN_PERIOD 0
#define M2C_PATHCOMP_MAY_CONTAIN_SPACE 1
#define M2C_PATHCOMP_MAY_CONTAIN_MINUS 1
#define M2C_PATHCOMP_MAY_CONTAIN_TILDE 1


/* --------------------------------------------------------------------------
 * Default settings for Unix or Unix-like systems
 * ----------------------------------------------------------------------- */

#elif defined(unix) || defined(__unix__)
#define M2C_PATHCOMP_MAY_CONTAIN_PERIOD 1
#define M2C_PATHCOMP_MAY_CONTAIN_SPACE 0
#define M2C_PATHCOMP_MAY_CONTAIN_MINUS 1
#define M2C_PATHCOMP_MAY_CONTAIN_TILDE 0


/* --------------------------------------------------------------------------
 * Default settings for any other systems
 * ----------------------------------------------------------------------- */

#else /* D E F A U L T */
#define M2C_PATHCOMP_MAY_CONTAIN_PERIOD 0
#define M2C_PATHCOMP_MAY_CONTAIN_SPACE 0
#define M2C_PATHCOMP_MAY_CONTAIN_MINUS 1
#define M2C_PATHCOMP_MAY_CONTAIN_TILDE 0
#endif

#endif /* M2C_PATHNAME_POLICY_H */

/* END OF FILE */
