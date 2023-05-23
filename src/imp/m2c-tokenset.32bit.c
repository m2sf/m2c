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
 * m2c-tokenset.32bit.c                                                      *
 *                                                                           *
 * Implementation of M2C tokenset type for 32-bit platforms.                 *
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

#include "m2c-tokenset.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


/* --------------------------------------------------------------------------
 * private type segment_t
 * --------------------------------------------------------------------------
 * Type representing bitmap segments in a tokenset.
 * ----------------------------------------------------------------------- */

typedef unsigned long segment_t;


/* --------------------------------------------------------------------------
 * constant SEGMENT_BITWIDTH
 * --------------------------------------------------------------------------
 * Size of a segment in bits.
 * ----------------------------------------------------------------------- */

#define SEGMENT_BITWIDTH 32
#define SEGMENT_MASK 0xFFFFFFFF


/* --------------------------------------------------------------------------
 * constant SEGMENT_COUNT
 * --------------------------------------------------------------------------
 * Number of segments in a tokenset.
 * ----------------------------------------------------------------------- */

#define SEGMENT_COUNT \
  ((TOKEN_END_MARK / SEGMENT_BITWIDTH) + 1)


/* --------------------------------------------------------------------------
 * hidden type m2c_tokenset_opaque_t
 * --------------------------------------------------------------------------
 * Opaque record type representing a tokenset object.
 * ----------------------------------------------------------------------- */

struct m2c_tokenset_opaque_t {
   /* segment */ segment_t segment[SEGMENT_COUNT];
   /* elem_count */ unsigned elem_count;
};

typedef struct m2c_tokenset_opaque_t m2c_tokenset_opaque_t;


/* --------------------------------------------------------------------------
 * function m2c_new_tokenset_from_list(token_list)
 * --------------------------------------------------------------------------
 * Returns a newly allocated tokenset object that includes the tokens passed
 * as arguments of a non-empty variadic argument list.  The argument list
 * must be explicitly terminated with 0.
 * ----------------------------------------------------------------------- */

unsigned count_bits_in_set (m2c_tokenset_t set);

m2c_tokenset_t m2c_new_tokenset_from_list (m2c_token_t first_token, ...) {
  m2c_tokenset_t new_set;
  unsigned bit, seg_index;
  m2c_token_t token;
  
  va_list token_list;
  va_start(token_list, first_token);
  
  /* allocate new set */
  new_set = malloc(sizeof(m2c_tokenset_opaque_t));
  
  /* bail out if allocation failed */
  if (new_set == NULL) {
    return NULL;
  } /* end if */
  
  /* initialise */
  seg_index = 0;
  while (seg_index < SEGMENT_COUNT) {
    new_set->segment[seg_index] = 0;
    seg_index++;
  } /* end while */
  
  /* store tokens from list */
  token = first_token;
  while (token != 0) {
  
    /* store token in set if in range */
    if (token < TOKEN_END_MARK) {
      seg_index = token / SEGMENT_BITWIDTH;
      bit = token % SEGMENT_BITWIDTH;
      new_set->segment[seg_index] =
        SEGMENT_MASK & (new_set->segment[seg_index] | (1 << bit));
    } /* end if */
    
    /* get next token in list */
    token = va_arg(token_list, m2c_token_t);
  } /* end while */
  
  /* update element counter */
  new_set->elem_count = count_bits_in_set(new_set);
  
  return new_set;
} /* end m2c_new_tokenset_from_list */


/* --------------------------------------------------------------------------
 * function m2c_new_tokenset_from_union(set_list)
 * --------------------------------------------------------------------------
 * Returns a newly allocated tokenset object that represents the set union of
 * the tokensets passed as arguments of a non-empty variadic argument list.
 * The argument list must be explicitly terminated with NULL.
 * ----------------------------------------------------------------------- */

m2c_tokenset_t m2c_new_tokenset_from_union (m2c_tokenset_t first_set, ...) {
  m2c_tokenset_t new_set;
  unsigned seg_index;
  m2c_tokenset_t set;
  
  va_list set_list;
  va_start(set_list, first_set);
  
  /* allocate new set */
  new_set = malloc(sizeof(m2c_tokenset_opaque_t));
  
  /* bail out if allocation failed */
  if (new_set == NULL) {
    return NULL;
  } /* end if */
  
  /* initialise */
  seg_index = 0;
  while (seg_index < SEGMENT_COUNT) {
    new_set->segment[seg_index] = 0;
    seg_index++;
  } /* end while */
  
  set = first_set;
  /* calculate union with each set in list */
  while (set != NULL) {
    /* for each segment ... */
    while (seg_index < SEGMENT_COUNT) {
      /* ... store union of corresponding segments */
      new_set->segment[seg_index] =
        new_set->segment[seg_index] | set->segment[seg_index];
      
      /* next segment */
      seg_index++;
    } /* end while */
    
    /* get next set in list */
    set = va_arg(set_list, m2c_tokenset_t);
  } /* end while */
  
  /* update element counter */
  new_set->elem_count = count_bits_in_set(new_set);
  
  return new_set;
} /* m2c_new_tokenset_from_union */


/* --------------------------------------------------------------------------
 * function m2c_tokenset_element(set, token)
 * --------------------------------------------------------------------------
 * Returns true if token is an element of set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_tokenset_element (m2c_tokenset_t set, m2c_token_t token) {
  unsigned bit, seg_index;
  
  if (token >= TOKEN_END_MARK) {
    return false;
  } /* end if */
  
  seg_index = token / SEGMENT_BITWIDTH;
  bit = token % SEGMENT_BITWIDTH;
  
  return ((set->segment[seg_index] & (1 << bit)) != 0);
} /* end m2c_tokenset_element */


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_element_count(set)
 * --------------------------------------------------------------------------
 * Returns the number of elements in set.
 * ----------------------------------------------------------------------- */

unsigned m2c_tokenset_element_count (m2c_tokenset_t set) {  
  if (set == NULL) {
    return 0;
  } /* end if */
  
  return set->elem_count;
} /* end m2c_tokenset_element_count */


/* --------------------------------------------------------------------------
 * function m2c_tokenset_subset (set, subset)
 * --------------------------------------------------------------------------
 * Returns true if each element in subset is also in set, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_tokenset_subset (m2c_tokenset_t set, m2c_tokenset_t subset) {
  unsigned seg_index;
  
  seg_index = 0;
  while (seg_index < SEGMENT_COUNT) {
    if (((set->segment[seg_index] & subset->segment[seg_index]) ^
        subset->segment[seg_index]) == 0) {
      seg_index++;
    }
    else {
      return false;
    } /* end if */
  } /* end while */
  
  return true;
} /* end m2c_tokenset_subset */


/* --------------------------------------------------------------------------
 * function m2c_tokenset_disjunct(set1, set2)
 * --------------------------------------------------------------------------
 * Returns true if set1 and set2 have no common elements, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_tokenset_disjunct (m2c_tokenset_t set1, m2c_tokenset_t set2) {
  unsigned seg_index;
  
  seg_index = 0;
  while (seg_index < SEGMENT_COUNT) {
    if ((set1->segment[seg_index] & set2->segment[seg_index]) == 0) {
      seg_index++;
    }
    else {
      return false;
    } /* end if */
  } /* end while */
  
  return true;
} /* end m2c_tokenset_disjunct */


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_set(set_name, set)
 * --------------------------------------------------------------------------
 * Prints a human readable representation of set.
 * Format: set_name = { comma-separated list of tokens };
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_set (const char *set_name, m2c_tokenset_t set) {
  unsigned bit, seg_index, count;
  m2c_token_t token;
  
  printf("%s = {", set_name);
  
  if (set->elem_count == 0) {
    printf(" ");
  } /* end if */
  
  count = 0; token = 0;
  while ((count <= set->elem_count) && (token < TOKEN_END_MARK)) {
    seg_index = token / SEGMENT_BITWIDTH;
    bit = token % SEGMENT_BITWIDTH;
    if ((set->segment[seg_index] & (1 << bit)) != 0) {
      count++;
      if (count < set->elem_count) {
        printf("\n  %s,", m2c_name_for_token(token));
      }
      else {
        printf("\n  %s\n", m2c_name_for_token(token));
      } /* end if */
    } /* end if */
    token++;
  } /* end while */
  
  printf("};\n");
} /* m2c_tokenset_print_set */


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_list(set)
 * --------------------------------------------------------------------------
 * Prints a human readable list of symbols in set.
 * Format: first, second, third, ..., secondToLast or last
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_list (m2c_tokenset_t set) {
  unsigned bit, seg_index, count;
  m2c_token_t token;
  
  if (set->elem_count == 0) {
    printf("(nil)");
  } /* end if */
  
  count = 0; token = 0;
  while ((count <= set->elem_count) && (token < TOKEN_END_MARK)) {
    seg_index = token / SEGMENT_BITWIDTH;
    bit = token % SEGMENT_BITWIDTH;
    
    if ((set->segment[seg_index] & (1 << bit)) != 0) {
      count++;
      if (count > 1) {
        if (count < set->elem_count) {
          printf(", ");
        }
        else {
          printf(" or ");
        } /* end if */
      } /* end if */
      
      if (token == TOKEN_IDENT) {
        printf("identifier");
      }
      else if (token == TOKEN_QUOTED_STRING) {
        printf("string");
      }
      else if (token == TOKEN_WHOLE_NUMBER) {
        printf("whole number");
      }
      else if (token == TOKEN_REAL_NUMBER) {
        printf("real number");
      }
      else if (token == TOKEN_CHAR_CODE) {
        printf("character code");
      }
      else if (M2C_IS_RESWORD_TOKEN(token)) {
        printf("%s", m2c_lexeme_for_resword(token));
      }
      else if (M2C_IS_SPECIAL_SYMBOL_TOKEN(token)) {
        printf("'%s'", m2c_lexeme_for_special_symbol(token));
      }
      else if (token == TOKEN_EOF) {
        printf("<EOF>");
      } /* end if */
    } /* end if */
    token++;
  } /* end while */
  
  printf(".\n");
} /* m2c_tokenset_print_list */


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_literal_struct(ident)
 * --------------------------------------------------------------------------
 * Prints a struct definition for tokenset literals.
 * Format: struct ident { unsigned s0, s1, s2, ...; unsigned short n };
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_literal_struct (const char *ident) {
  unsigned seg_index;
  
  printf("struct %s { unsigned long s0", ident);
  
  seg_index = 1;
  while (seg_index < SEGMENT_COUNT) {
    printf(", s%u", seg_index);
    seg_index++;
  } /* end while */
  
  printf("; unsigned short n; };\n");
  
  printf("typedef struct %s %s;\n", ident, ident);
} /* m2c_tokenset_print_literal_struct */


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_print_literal(set)
 * --------------------------------------------------------------------------
 * Prints a sequence of hex values representing the bit pattern of set.
 * Format: { 0xHHHHHHHH, 0xHHHHHHHH, ..., count };
 * ----------------------------------------------------------------------- */

void m2c_tokenset_print_literal (m2c_tokenset_t set) {
  unsigned seg_index;
  
  /* print list head and first segment */
  printf("{ /* bits: */ 0x%08lX", set->segment[0]);
  
  /* print remaining segments */
  seg_index = 1;
  while (seg_index < SEGMENT_COUNT) {  
    printf(", 0x%08lX", set->segment[seg_index]);
    seg_index++;
  } /* end while */
  
/* print counter and list tail */
  printf(", /* counter: */ %u }\n", set->elem_count);
} /* m2c_tokenset_print_literal */


/* --------------------------------------------------------------------------
 * procedure m2c_tokenset_release(set)
 * --------------------------------------------------------------------------
 * Deallocates tokenset set.
 * ----------------------------------------------------------------------- */

void m2c_tokenset_release (m2c_tokenset_t set) {
  if (set != NULL) {
    free(set);
  } /* end if */
} /* end m2c_tokenset_release */


/* --------------------------------------------------------------------------
 * private function count_bits_in_set(set)
 * --------------------------------------------------------------------------
 * Returns the number of set bits in set.
 * ----------------------------------------------------------------------- */

unsigned count_bits_in_set (m2c_tokenset_t set) {
  unsigned bit, seg_index, bit_count;
  
  bit_count = 0;
  seg_index = 0;
  
  while (seg_index < SEGMENT_COUNT) {
    bit = 0;
    while (bit < SEGMENT_BITWIDTH) {
      if ((set->segment[seg_index] & (1 << bit)) != 0) {
        bit_count++;
      } /* end if */
      bit++;
    } /* end while */
    seg_index++;
  } /* end while */
  
  return bit_count;
} /* count_bits_in_set */

/* END OF FILE */