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
 * lowline-ident-strings.c                                                   *
 *                                                                           *
 * Implementation of identifier lowline translation dictionary.              *
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

#include "lowline-ident-strings.h"


/* --------------------------------------------------------------------------
 * private type lettercase_t
 * --------------------------------------------------------------------------
 * Enumeration values for lower- and uppercase mode.
 * ----------------------------------------------------------------------- */

typedef enum {
    LC_LOWERCASE,
    LC_UPPERCASE
} lettercase_t;


/* --------------------------------------------------------------------------
 * private type case_transform_f
 * --------------------------------------------------------------------------
 * function pointer type for function to return case transformed character.
 * ----------------------------------------------------------------------- */

typedef char (case_transform_f) (char);


/* --------------------------------------------------------------------------
 * Highest possible number of words within an identifier
 * ----------------------------------------------------------------------- */

#define MAX_IDENT_WORDS ((M2C_MAX_IDENT_LENGTH / 2) + 1)


/* --------------------------------------------------------------------------
 * private type word_entry_t
 * --------------------------------------------------------------------------
 * Length and position of a word within an identifier
 * ----------------------------------------------------------------------- */

typedef struct {
    uint8_t len;
    uint8_t pos;
} word_entry_t;


/* --------------------------------------------------------------------------
 * private type word_map_t
 * --------------------------------------------------------------------------
 * Lengths and positions of all words within an identifier
 * ----------------------------------------------------------------------- */

typedef struct {
  uint8_t word_count;
  word_entry_t word[MAX_IDENT_WORDS];
} word_map_t;


/* --------------------------------------------------------------------------
 * private function match_lowercase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident  starting at index  to a lowercase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  lowercaseWord := LowercaseLetter (LowercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static uint_t match_lowercase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* LowercaseLetter */
  if (IS_LOWER(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not lowercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (LowercaseLetter | Digit)* */
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_lowercase_word */


/* --------------------------------------------------------------------------
 * private function match_titlecase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident  starting at index  to a titlecase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  titlecaseWord := UppercaseLetter (LowercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static uint_t match_titlecase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* UppercaseLetter */
  if (IS_UPPER(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not uppercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (LowercaseLetter | Digit)* */
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_titlecase_word */


/* --------------------------------------------------------------------------
 * private function match_uppercase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident starting at index  to an uppercase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  uppercaseWord :=
 *          UppercaseLetter !LowercaseLetter
 *          (UppercaseLetter !LowercaseLetter | Digit)*;
 *
 * !!! An uppercase letter before a lowercase letter must NOT be consumed !!!
 * !!! because it is the first character of a following titlecase word.   !!!
 * ----------------------------------------------------------------------- */

static uint_t match_uppercase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* UppercaseLetter !LowercaseLetter */
  if (IS_UPPER(ch) && (NOT(IS_LOWER(ident[index+1])))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not uppercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (UppercaseLetter !LowercaseLetter | Digit)* */
  while ((IS_UPPER(ch) && (NOT(IS_LOWER(ident[index+1])))) || IS_DIGIT(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_uppercase_word */


/* --------------------------------------------------------------------------
 * private function get_word_map_for_ident(ident, map)
 * --------------------------------------------------------------------------
 * Calculates a word map for ident, passes it in map  and returns word count.
 * If ident is malformed, passes zero filled map  and returns zero.
 * ----------------------------------------------------------------------- */

static void get_word_map_for_ident (const char *ident, word_map_t *map) {
  
  char ch;
  uint8_t pos, index;
  
  pos = 0;
  index = 0;
  ch = ident[pos];
  while (ch != ASCII_NUL) {
    /* lowercase word */
    if (IS_LOWER(ch)) {
      len = match_lowercase_word(pos, ident);
    }
    else if (IS_UPPER(ch)) {
      /* titlecase word */
      if (IS_LOWER(ident[pos+1])) {
        len = match_titlecase_word(pos, ident);
      }
      /* uppercase word */
      else {
        len = match_uppercase_word(pos, ident);
      }; /* end if */
    }
    /* lowline in identifier */
    else if (m2c_compiler_option_lowline_identifiers() && (ch == '_')) {
      /* skip lowline */
      pos++;
      continue;
    }
    /* no word match and no lowline */
    else /* malformed identifier */ {
      /* fill map with zeroes and zero word count */
      for (index = 0; index < MAX_IDENT_WORDS; index++) {
        map->word[index] = (word_entry_t) { 0, 0 };
      }; /* end for */
      map->word_count = 0;
      return;
    }; /* end if */
    
    /* word matched */
    if (len > 0) {
      /* pass length and position in map entry */
      map->word[index] = (word_entry_t) { len, pos };
      /* next word */
      pos = pos + len;
      /* next char */
      ch = ident[pos];
      /* next index */
      index++;
    }
    /* no match */
    else {
      break;
    }; /* end if */
  }; /* end while */
  
  /* pass word count */
  map->word_count = index;
  return;
}; /* end get_word_map_for_ident */


/* --------------------------------------------------------------------------
 * private function required_length_for_snake_case(map)
 * --------------------------------------------------------------------------
 * Returns the required length for snake case translation from word map.
 * ----------------------------------------------------------------------- */

static uint8_t required_length_for_snake_case (const word_map_t *map) {
  
  uint8_t len, index, word_count;
  
  word_count = map->word_count;

  if (word_count == 0) {
    return 0;
  }; /* end if */
  
  /* add up all the word lengths */
  len = 0;
  index = 0;
  while (index < word_count) {
    len = len + map->word[index].len;
    index++;
  }; /* end while */
  
  /* add lowline count */
  len = len + word_count - 1;
  
  return len;
}; /* end required_length_for_snake_case */


/* --------------------------------------------------------------------------
 * private function lowline_transform(ident, map, lettercase, maxlen)
 * --------------------------------------------------------------------------
 * Returns the lowline and lettercase translation for ident using the given
 * word map.  If the result exceeds maxlen, it is truncated to maxlen.
 * ----------------------------------------------------------------------- */

static const char* lowline_transform
  (const char *ident, const word_map_t *map,
   lettercase_t lettercase, uint_t maxlen) {
  
  uint8_t word_count, xlat_len, index;
  case_transform_t case_transform;
  char *xlat;
  
  word_count = map->word_count;

  if (word_count == 0) {
    return NULL;
  }; /* end if */

  /* install case transformation handler */
  if (lettercase == LC_LOWERCASE) {
    case_transform = to_lower;
  }
  else /* LC_UPPERCASE */ {
    case_transform = to_upper;
  }; /* end if */
  
  xlat_len = required_length_for_snake_case(map);
  if xlat_len > maxlen {
    xlat_len = maxlen
  }; /* end if */
  xlat = malloc(xlat_len * sizeof(char) + 1);

  /* copy first word */
  tgt_index = 0;
  src_index = 0;
  len = map->word[0].len
  while ((src_index < len) && (tgt_index < maxlen)) {
    xlat[tgt_index] = case_transform(ident[src_index]);
    tgt_index++;
    src_index++;
  }; /* end while */
  
  word_index = 0;
  
  while (word_index < word_count) {
    /* append lowline */
    xlat[tgt_index] = '_';
    tgt_index++;
    
    /* copy word at index */
    len = map->word[word_index].len;
    src_index = map->word[word_index].pos;
    while ((src_index < len) && (tgt_index < maxlen)) {
      xlat[tgt_index] = case_transform(ident[src_index]);
      tgt_index++;
      src_index++;
    }; /* end while */
    
    /* next word */
    word_index++;
  }; /* end while */

  /* terminate string */
  xlat[tgt_index] = ASCII_NUL;
  
  return xlat;
}; /* end lowline_transform */


/* --------------------------------------------------------------------------
 * private type llid_xlat_t
 * --------------------------------------------------------------------------
 * pointer to record representing a translation.
 * ----------------------------------------------------------------------- */

typedef llid_xlat_s *llid_xlat_t;

typdef struct {
  uint_t length;
  char ident[];
} llid_xlat_s;


/* --------------------------------------------------------------------------
 * private type llid_ident_t
 * --------------------------------------------------------------------------
 * pointer to record representing an identifier object.
 * ----------------------------------------------------------------------- */

typedef llid_ident_s *llid_ident_t;

typedef struct {
  uint_t ref_count;
  uint_t length;
  char *ident;
  llid_xlat_t snake_case;
  llid_xlat_t macro_case;
} llid_ident_s;


/* --------------------------------------------------------------------------
 * private type llid_dict_entry_t
 * --------------------------------------------------------------------------
 * pointer to record representing a dictionary entry.
 * ----------------------------------------------------------------------- */

typedef struct llid_dict_entry_s *llid_dict_entry_t;

typedef struct {
  llid_hash_t key;
  llid_str_t ident;
  llid_dict_entry_t next;
} llid_dict_entry_s;


/* --------------------------------------------------------------------------
 * private type llid_dict_t
 * --------------------------------------------------------------------------
 * pointer to record representing the dictionary.
 * ----------------------------------------------------------------------- */

typedef struct llid_dict_s *llid_dict_t;

typedef struct {
  uint_t entry_count;
  uint_t bucket_count;
  llid_status_t last_status;
  llid_dict_entry_t bucket[];
} llid_dict_s;


/* --------------------------------------------------------------------------
 * private variable dictionary
 * --------------------------------------------------------------------------
 * pointer to global dictionary.
 * ----------------------------------------------------------------------- */

static llid_dict_t dictionary = NULL;


/* --------------------------------------------------------------------------
 * procedure llid_init_dictionary()
 * --------------------------------------------------------------------------
 * Allocates and initialises the lowline representation dictionary. 
 * ----------------------------------------------------------------------- */

void llid_init_dictionary (unsigned size, llid_status_t *status) {
  
  uint_t index, bucket_count, allocation_size;
  
  /* check pre-conditions */
  if (dictionary != NULL) {
    SET_STATUS(status, LLID_STATUS_ALREADY_INITIALIZED);
    return;
  } /* end if */
  
  /* determine bucket count */
  if (size == 0) {
    bucket_count = LLID_DICT_DEFAULT_BUCKET_COUNT;
  }
  else /* size != 0 */ {
    bucket_count = size;
  } /* end if */
  
  /* allocate dictionary */
  allocation_size = sizeof(llid_dict_s) +
    bucket_count * sizeofllid_dict_entry_t);
  dictionary = malloc(allocation_size);
  
  /* bail out if allocation failed */
  if (dictionary == NULL) {
    SET_STATUS(status, LLID_STATUS_ALLOCATION_FAILED);
    return;
  } /* end if */
  
  /* set entry and bucket count */
  dictionary->entry_count = 0;
  dictionary->bucket_count = bucket_count;
  
  /* initialise buckets */
  index = 0;
  while (index < bucket_count) {
    dictionary->bucket[index] = NULL;
    index++;
  } /* end while */

  SET_STATUS(status, LLID_STATUS_SUCCESS);
  return;
}; /* end llid_init_dictionary */


/* --------------------------------------------------------------------------
 * function llid_snake_case_for_ident(ident)
 * --------------------------------------------------------------------------
 * Returns the snake_case representation of ident,  or NULL if malformed.
 * ----------------------------------------------------------------------- */

const char* llid_snake_case_for_ident (const char* ident) {
  
  llid_dict_entry_t new_entry, this_entry;
  char *new_string, this_string;
  uint_t index, length;
  llid_hash_t key;
  char ch;
  
  /* check dictionary */
  if (dictionary == NULL) {
    return NULL;
  } /* end if */
  
  /* check ident */
  if (ident == NULL) {
    dictionary->last_status = LLID_STATUS_INVALID_REFERENCE;
    return NULL;
  } /* end if */
  
  /* determine length and key */
  index = 0;
  ch = ident[index];
  key = HASH_INITIAL;
  while (ch != ASCII_NUL) {
    key = HASH_NEXT_CHAR(key, ch);
    index++;
    ch = ident[index];
  } /* end while */
  
  length = index + 1;
  key = HASH_FINAL(key);
  
  /* determine bucket index */
  index = key % dictionary->bucket_count;
  
  /* check if ident is already in dictionary */
  if /* bucket empty */ (dictionary->bucket[index] == NULL) {
    
    /* create a new identifier object */
    new_ident = new_ident_entry(ident, length);
    
    /* create a new dictionary entry */
    new_entry = new_dict_entry(new_ident, key);
    
    /* link the bucket to the new entry */
    dictionary->bucket[index] = new_entry;
    
    /* update the entry counter */
    dictionary->entry_count++;
    
    /* set status and return string object */
    dictionary->last_status = LLID_STATUS_SUCCESS;
    return new_ident->snake_case->ident;
  }
  else /* bucket not empty */ {
    
    /* first entry in bucket is starting point */
    this_entry = dictionary->bucket[index];
    
    /* search bucket for matching string */
    while (true) {
      if /* match found in current entry */
        ((this_entry->key == key) &&
          matches_ident_and_len(this_entry->ident, ident, length)) {
        
        /* get identifier object of matching entry and retain it */
        this_ident = this_entry->ident;
        llid_retain(this_ident);
        
        /* set status and return string object */
        dictionary->last_status = LLID_STATUS_SUCCESS;
        return this_ident->snake_case->ident;
      }
      else if /* last entry reached without match */
        (this_entry->next == NULL) {
        
        /* create a new identifier object */
        new_ident = new_ident_entry(ident, length);
        
        /* create a new repository entry */
        new_entry = new_dict_entry(new_ident, key);
        
        /* link last entry to the new entry */
        this_entry->next = new_entry;
        
        /* update the entry counter */
        dictionary->entry_count++;
        
        /* set status and return string object */
        dictionary->last_status = LLID_STATUS_SUCCESS;
        return new_ident->snake_case->ident;
      }
      else /* not last entry yet, move to next entry */ {
        this_entry = this_entry->next;
      } /* end if */
    } /* end while */      
  } /* end if */    
}; /* end llid_snake_case_for_ident */


/* --------------------------------------------------------------------------
 * function llid_macro_case_for_ident(ident)
 * --------------------------------------------------------------------------
 * Returns the MACRO_CASE representation of ident,  or NULL if malformed.
 * ----------------------------------------------------------------------- */

const char* llid_macro_case_for_ident (const char* ident) {
  
  /* TO DO */
  
}; /* end llid_macro_case_for_ident */


/* --------------------------------------------------------------------------
 * function llid_entry_count()
 * --------------------------------------------------------------------------
 * Returns the number of identifiers stored in the dictionary.
 * ----------------------------------------------------------------------- */

uint_t llid_entry_count (void) {
  
  if (dictionary == NULL) {
    return 0;
  } /* end if */
  
  return dictionary->entry_count;
  
}; /* end llid_entry_count */


/* --------------------------------------------------------------------------
 * procedure llid_retain_entry(ident)
 * --------------------------------------------------------------------------
 * Prevents the dictionary entry for ident from deallocation.
 * ----------------------------------------------------------------------- */

void llid_retain_entry (const char *ident) {
  
  /* TO DO */
  
}; /* end llid_retain_entry */


/* --------------------------------------------------------------------------
 * procedure llid_release_entry()
 * --------------------------------------------------------------------------
 * Cancels an  outstanding retain  for ident,  or if there are no outstanding
 * retains, deallocates the entry for ident.
 * ----------------------------------------------------------------------- */

void llid_release_entry (const char *ident) {
  
  /* TO DO */
  
}; /* end llid_release_entry */


/* --------------------------------------------------------------------------
 * function llid_last_status()
 * --------------------------------------------------------------------------
 * Returns the status of the last operation. 
 * ----------------------------------------------------------------------- */

llid_status_t llid_last_status (void) {
  
  if (dictionary == NULL) {
    return LLID_STATUS_NOT_INITIALIZED;
  }
  else {
    return dictionary->last_status;
  } /* end if */
  
}; /* end llid_last_status */


/* --------------------------------------------------------------------------
 * procedure llid_dealloc_dictionary()
 * --------------------------------------------------------------------------
 * Deallocates the dictionary. 
 * ----------------------------------------------------------------------- */

void llid_dealloc_dictionary (llid_status_t *status) {
  
  /* TO DO */
  
}; /* end llid_dealloc_dictionary */


/* END OF FILE */