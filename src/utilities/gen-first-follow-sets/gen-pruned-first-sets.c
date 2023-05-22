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
 * gen-pruned-first-sets.c                                                   *
 *                                                                           *
 * Utility program to generate a pruned first set database.                  *
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

#import "m2c-tokenset.h"
#import <stdbool.h>
#import <stdio.h>


/* --------------------------------------------------------------------------
 * enum type production_t
 * ----------------------------------------------------------------------- */

#define PROD(_caps, _id, _first, _follow) P_ ## _caps

typedef enum {
  #include "production-data.h"
  , P_END_MARKER
} production_t;

#define PRODUCTION_COUNT P_END_MARKER

#undef PROD


/* --------------------------------------------------------------------------
 * string table for production names
 * ----------------------------------------------------------------------- */

#define PROD(_caps, _id, _first, _follow) \
  #_caps

static const char *prod_name[] = {
  #include "production-data.h"
  , "\0"
}; /* enum_name */

#undef PROD


/* --------------------------------------------------------------------------
 * complete first set table
 * ----------------------------------------------------------------------- */

static m2c_tokenset_t first_set[PRODUCTION_COUNT];


/* --------------------------------------------------------------------------
 * function init_first_set_table()
 * --------------------------------------------------------------------------
 * Declares and initialises a complete first set table from production-data.h
 * ----------------------------------------------------------------------- */

static void init_first_set_table (void) {
  
  #define PROD(_caps, _id, _first, _follow) \
    first_set[P_##_caps] = m2c_new_tokenset_from_list(_first)
  
  #include "production-data.h"
  ;
  
  #undef PROD
} /* init_first_set_table */


/* --------------------------------------------------------------------------
 * pruned first set table
 * ----------------------------------------------------------------------- */

static *m2c_tokenset_t pruned_first_set_table[PRODUCTION_COUNT + 1];
static unsigned pruned_production_count;


/* --------------------------------------------------------------------------
 * pruned index lookup table
 * ----------------------------------------------------------------------- */

static unsigned pruned_index_lookup[PRODUCTION_COUNT];
static unsigned reverse_index_lookup[PRODUCTION_COUNT + 1];


/* --------------------------------------------------------------------------
 * function is_duplicate(set, index, equiv_index)
 * --------------------------------------------------------------------------
 * Searches the  pruned first set table  up to  max_index for a matching set.
 * If a matching entry is found, its index is passed back in equiv_index, and
 * true is returned, otherwise no value is passed and false is returned.
 * ----------------------------------------------------------------------- */

#define SETS_MATCH(_set1, _set2) \
  ((m2c_tokenset_element_count(_set1) == \
      m2c_tokenset_element_count(_set2)) \
      && m2c_tokenset_subset(_set1, _set2)) \

static bool is_duplicate
  (m2c_tokenset_t set, unsigned set_index, unsigned *equiv_index) {
  unsigned index;
  m2c_tokenset_t compare_set;
  
  for (index = 1, index < set_index, index++) {
    compare_set = pruned_first_set_table[index];
    
    if (SETS_MATCH(set, compare_set)) {
      /* match */
      *equiv_index = index;
      return true;
    } /* end if */
  } /* end for */
  
  /* no match */
  return false;
} /* end is_duplicate */


/* --------------------------------------------------------------------------
 * function init_pruned_table()
 * --------------------------------------------------------------------------
 * Initialises the pruned first set table by omitting sets of cardinality one
 * and duplicate sets. Also initialises the pruned index lookup table and the
 * reverse index lookup table.  Indices of sets with cardinality one are map-
 * ped to a sentinel nil entry at index zero in the pruned table  and indices
 * of duplicate sets to the first equivalent entry in the pruned table.
 * ----------------------------------------------------------------------- */

static void init_pruned_table (void) {
  unsigned index, pruned_index, equiv_index;
  m2c_tokenset_t this_set;
  
  pruned_table[0] = NULL;
  index = 0; pruned_index = 1;
  while (index < PRODUCTION_COUNT) {
    this_set = first_set[index];
    
    if (m2c_tokenset_element_count(this_set) <= 1) {
      pruned_index_lookup[index] = 0;
    }
    else if (is_duplicate(this_set, index, &equiv_index)) {
      pruned_index_lookup[index] = equiv_index;
    }
    else {
      pruned_first_set_table[pruned_index] = first_set[index];
      pruned_index_lookup[index] = pruned_index;
      reverse_index_lookup[pruned_index] = index;
      pruned_index++;
    }
    else {
      pruned_index_lookup[index] = 0;
    } /* end if */
    
    index++;
  } /* end while */
  
  pruned_production_count = pruned_index - 1;
} /* print_literals */


/* --------------------------------------------------------------------------
 * function print_set_literals()
 * --------------------------------------------------------------------------
 * Prints the set literals of all sets in the pruned first set table as name/
 * value pairs to the console. Each entry has the format DATA(name, literal),
 * where name is the name of the production and literal is a literal that re-
 * presents the production's first set.
 * ----------------------------------------------------------------------- */

#define PREAMBLE \
  "/* AUTO-GENERATED by utility gen-pruned-first-sets * DO NOT EDIT! */\n\n"

#define EOF_MARKER \
  "\n/* END OF FILE */\n"

static void print_set_literals (void) {
  unsigned index;
  m2c_tokenset_t set;
  
  printf(PREAMBLE);
  
  for (index = 1, index < pruned_production_count, index++ ) {
    
    /* name */
    printf("DATA(%s, ", prod_name[reverse_index_lookup[index]]);
    
    /* set literal */
    set = pruned_first_set_table[index];
    m2c_tokenset_print_literal(set);
    
    /* separator */
    printf("),\n");
  } /* end for */
  
  printf(EOF_MARKER)
} /* end print_set_literals */


/* --------------------------------------------------------------------------
 * function print_lookup_table()
 * --------------------------------------------------------------------------
 * Prints the  lookup table  as  name/value pairs to the console.  Each entry
 * has the format DATA(name, index), where name is the name of the production
 * and index is the production's index in the pruned first set table.
 * ----------------------------------------------------------------------- */

static void print_lookup_table (void) {
  unsigned index;
  
  printf(PREAMBLE);
  
  for (index = 0, index < production_count, index++) {
    printf("DATA(%s, %u),\n", prod_name[index], pruned_index_lookup[index]);
  } /* end for */
  
  printf(EOF_MARKER)
} /* end print_lookup_table */


/* --------------------------------------------------------------------------
 * function str_len(str)
 * ----------------------------------------------------------------------- */

static unsigned str_len(const char *str) {
  unsigned index;
  
  index = 0;
  while (str[index] != '\0') {
    index++;
  } /* end while */
  return index;
} /* end str_len */


/* --------------------------------------------------------------------------
 * function print_usage()
 * --------------------------------------------------------------------------
 * Prints usage info to the console.
 * ----------------------------------------------------------------------- */

static void print_usage (void) {
  printf("usage info:\n\n");
  printf("-h prints this info.\n");
  printf("-s prints first set literals.\n");
  printf("-l prints first set lookup table.\n\n");
  printf("examples:\n\n");
  printf("$ gen-pruned-first-sets -s > pruned-first-set-literals.h\n");
  printf("$ gen-pruned-first-sets -l > pruned-first-set-lookup.h\n\n");
} /* end print_usage */


/* --------------------------------------------------------------------------
 * utility program gen-pruned-first-sets
 * --------------------------------------------------------------------------
 * This utility prints a comma delimited list of FIRST set literals to the
 * console. It should be invoked with output redirection as follows:
 *
 * to generate the set literals
 * $ gen-pruned-first-sets -s > pruned-first-set-literals.h
 *
 * to generate the lookup table
 * $ gen-pruned-first-sets -l > pruned-first-set-lookup.h
 * ----------------------------------------------------------------------- */

int main(int argc, const char *argv[]) {
  char *argstr;
  unsigned len;
  
  init_first_set_table();
  init_pruned_table();
   
  if ((argc == 1) {
    argstr = argv[1];
    len = str_len(argstr);
    
    if ((len == 2) && (argstr[0] == '-')) {
      switch (argstr[1]) {
        case 'h' :
          print_usage();
          break;
        case 'l' :
          print_lookup_table();
          break;
        case 's' :
          print_set_literals();
          break;
        default :
          print_usage();
          return (-1);
      } /* end switch */
    }
    else /* invalid args */ {
    print_usage();
    return (-1);
  } /* end if */
  
  return 0;
} /* end main */

/* END OF FILE */