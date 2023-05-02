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
 * tabulator.h
 *
 * Public interface of tabulator management.
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

#ifndef TABULATOR_H
#define TABULATOR_H

/* --------------------------------------------------------------------------
 * Default and maximum tabulator settings
 * ----------------------------------------------------------------------- */

#define TAB_DEFAULT 0
#define TAB_MAX_WIDTH 8


/* --------------------------------------------------------------------------
 * procedure tab_set_width(value)
 * --------------------------------------------------------------------------
 * Sets the tab width to the given value. Zero leaves setting unchanged.
 * ----------------------------------------------------------------------- */

void tab_set_width (uint_t value);


/* --------------------------------------------------------------------------
 * function tab_get_width()
 * --------------------------------------------------------------------------
 * Returns the tab width.
 * ----------------------------------------------------------------------- */

uint_t tab_get_width (void);

#endif /* TABULATOR_H */

/* END OF FILE */