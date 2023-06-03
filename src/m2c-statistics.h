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
 * m2c-statistics.h                                                          *
 *                                                                           *
 * Public interface for compilation statistics module.                       *
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

#ifndef M2C_STATISTICS_H
#define M2C_STATISTICS_H

/* --------------------------------------------------------------------------
 * type m2c_stats_type_t
 * --------------------------------------------------------------------------
 * Enumerated values representing statistics counters.
 * ----------------------------------------------------------------------- */

typedef enum {
  M2C_STATS_DECL_COUNT,
  M2C_STATS_PROC_COUNT,
  M2C_STATS_STMT_COUNT,
  M2C_STATS_LEX_WARN_COUNT,
  M2C_STATS_LEX_ERROR_COUNT,
  M2C_STATS_SYNTAX_WARN_COUNT,
  M2C_STATS_SYNTAX_ERROR_COUNT,
  M2C_STATS_SEMANTIC_WARN_COUNT,
  M2C_STATS_SEMANTIC_ERROR_COUNT,
  M2C_STATS_END_MARK
} m2c_stats_type_t;


/* --------------------------------------------------------------------------
 * constant STATS_TYPE_COUNT
 * ----------------------------------------------------------------------- */

#define STATS_TYPE_COUNT M2C_STATS_END_MARK


/* --------------------------------------------------------------------------
 * type m2c_stats_t
 * --------------------------------------------------------------------------
 * Type to hold all statistics counters.
 * ----------------------------------------------------------------------- */

typedef *m2c_stats_t;


/* --------------------------------------------------------------------------
 * macro IS_VALID_STAT_TYPE(p)
 * --------------------------------------------------------------------------
 * Returns true it p is a valid statistics type parameter, otherwise false.
 * ----------------------------------------------------------------------- */

#define IS_VALID_STATS_TYPE(_p) \
  ((_p >= 0) && (_p < STATS_TYPE_COUNT))
  

/* --------------------------------------------------------------------------
 * function m2c_stats_new()
 * --------------------------------------------------------------------------
 * Returns a new statistics record.
 * ----------------------------------------------------------------------- */

m2c_stats_t m2c_stats_new (void);

  
/* --------------------------------------------------------------------------
 * function m2c_stats_inc(stats, param)
 * --------------------------------------------------------------------------
 * Increments the counter for statistic param of statistics record stats.
 * ----------------------------------------------------------------------- */

void m2c_stats_inc (m2c_stats_t stats, m2c_stat_type_t param);


/* --------------------------------------------------------------------------
 * function m2c_stats_value(stats)
 * --------------------------------------------------------------------------
 * Returns the counter for statistic param of statistics record stats.
 * ----------------------------------------------------------------------- */

unsigned short m2c_stats_value (m2c_stats_t stats, m2c_stat_type_t param);


/* --------------------------------------------------------------------------
 * function m2c_stats_set_line_count(stats, value)
 * --------------------------------------------------------------------------
 * Sets the line count of statistics record stats to value.
 * ----------------------------------------------------------------------- */

void m2c_stats_set_line_count (m2c_stats_t stats, unsigned short value);


/* --------------------------------------------------------------------------
 * function m2c_stats_line_count(stats)
 * --------------------------------------------------------------------------
 * Returns the line count of statistics record stats.
 * ----------------------------------------------------------------------- */

unsigned short m2c_stats_line_count (m2c_stats_t stats);


/* --------------------------------------------------------------------------
 * function m2c_stats_release(stats)
 * --------------------------------------------------------------------------
 * Deallocates statistics record stats.
 * ----------------------------------------------------------------------- */

void m2c_stats_release (m2c_stats_t stats);


#endif /* M2C_STATISTICS_H */

/* END OF FILE */