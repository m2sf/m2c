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
 * m2c-statistics.c                                                          *
 *                                                                           *
 * Implementation of compilation statistics module.                          *
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
 * imports
 * ----------------------------------------------------------------------- */

#include "statistics.h"

#include <bool.h>
#include <stdlib.h>


/* --------------------------------------------------------------------------
 * type m2c_stats_t
 * --------------------------------------------------------------------------
 * Type to hold all statistics counters.
 * ----------------------------------------------------------------------- */

typedef struct {
  unsigned short value[STATS_TYPE_COUNT];
  unsigned short line_count;
} m2c_stats_s;

typedef m2c_stats_s *m2c_stats_t;


/* --------------------------------------------------------------------------
 * function m2c_stats_new()
 * --------------------------------------------------------------------------
 * Returns a new statistics record.
 * ----------------------------------------------------------------------- */

m2c_stats_t m2c_stats_new (void) {
  m2c_stat_type_t index;
  
  /* allocate */
  stats = malloc(sizeof(m2c_stats_s));
  if (stats == NULL) {
    return NULL;
  } /* end if */
  
  /* initialise */
  for (index = 0; index++; index < STATS_TYPE_COUNT) {
    stats->value[index] = 0;
  } /* end for */
  stats->line_count = 0;
  
  return stats;
} /* end m2c_stats_new */

  
/* --------------------------------------------------------------------------
 * function m2c_stats_inc(stats, param)
 * --------------------------------------------------------------------------
 * Increments the counter for statistic param of statistics record stats.
 * ----------------------------------------------------------------------- */

void m2c_stats_inc (m2c_stats_t stats, m2c_stat_type_t param) {
  if ((stats != NULL) && (IS_VALID_STATS_TYPE(param))) {
    stats->value[param]++;
  } /* end if */
} /* end m2c_stats_inc */


/* --------------------------------------------------------------------------
 * function m2c_stats_value(stats)
 * --------------------------------------------------------------------------
 * Returns the counter for statistic param of statistics record stats.
 * ----------------------------------------------------------------------- */

unsigned short m2c_stats_value (m2c_stats_t stats, m2c_stat_type_t param) {
  if ((stats == NULL) || (IS_VALID_STATS_TYPE(param) == false)) {
    return 0;
  } /* end if */
  
  return stats->value[param];
} /* end m2c_stats_value */


/* --------------------------------------------------------------------------
 * function m2c_stats_set_line_count(stats, value)
 * --------------------------------------------------------------------------
 * Sets the line count of statistics record stats to value.
 * ----------------------------------------------------------------------- */

void m2c_stats_set_line_count (m2c_stats_t stats, unsigned short value) {
  if (stats != NULL) {
    stats->line_count = value;
  } /* end if */
} /* end m2c_stats_set_line_count */


/* --------------------------------------------------------------------------
 * function m2c_stats_line_count(stats)
 * --------------------------------------------------------------------------
 * Returns the line count of statistics record stats.
 * ----------------------------------------------------------------------- */

unsigned short m2c_stats_line_count (m2c_stats_t stats) {
  if (stats == NULL) {
    return 0;
  } /* end if */
  
  return stats->line_count;
} /* end m2c_stats_line_count */


/* --------------------------------------------------------------------------
 * function m2c_stats_release(stats)
 * --------------------------------------------------------------------------
 * Deallocates statistics record stats.
 * ----------------------------------------------------------------------- */

void m2c_stats_release (m2c_stats_t stats) {
  free(stats);
} /* end m2c_stats_release */


/* END OF FILE */