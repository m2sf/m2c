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
  * interned-strings.c                                                        *
  *                                                                           *
  * Implementation of interned strings library.                               *
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
 * Imports
 * ----------------------------------------------------------------------- */

#include "interned-strings.h"
#include "hash.h"


/* --------------------------------------------------------------------------
 * Defaults
 * ----------------------------------------------------------------------- */

#define INTSTR_REPO_DEFAULT_BUCKET_COUNT 2011


/* --------------------------------------------------------------------------
 * private type intstr_hash_t
 * --------------------------------------------------------------------------
 * unsigned integer type representing a 32-bit hash value.
 * ----------------------------------------------------------------------- */

typedef uint32_t intstr_hash_t;


/* --------------------------------------------------------------------------
 * hidden type intstr_struct_t
 * --------------------------------------------------------------------------
 * record type representing a dynamic string object.
 * ----------------------------------------------------------------------- */

struct intstr_struct_t {
  uint_t ref_count;
  uint_t length;
  char char_array[];
};

typedef struct intstr_struct_t intstr_struct_t;


/* --------------------------------------------------------------------------
 * private type intstr_repo_entry_t
 * --------------------------------------------------------------------------
 * pointer to record representing a string repository entry.
 * ----------------------------------------------------------------------- */

typedef struct intstr_repo_entry_s *intstr_repo_entry_t;

struct intstr_repo_entry_s {
  intstr_hash_t key;
  intstr_t str;
  intstr_repo_entry_t next;
};

typedef struct intstr_repo_entry_s intstr_repo_entry_s;


/* --------------------------------------------------------------------------
 * private type intstr_repo_t
 * --------------------------------------------------------------------------
 * pointer to record representing the string repository.
 * ----------------------------------------------------------------------- */

typedef struct intstr_repo_s *intstr_repo_t;

struct intstr_repo_s {
  uint_t entry_count;
  uint_t bucket_count;
  intstr_repo_entry_t bucket[];
};

typedef struct intstr_repo_s intstr_repo_s;


/* --------------------------------------------------------------------------
 * private variable repository
 * --------------------------------------------------------------------------
 * pointer to global string repository.
 * ----------------------------------------------------------------------- */

static intstr_repo_t repository = NULL;


/* --------------------------------------------------------------------------
 * procedure intstr_init_repo(size, status)
 * --------------------------------------------------------------------------
 * Allocates and initialises global string repository.  Parameter size deter-
 * mines the  number of buckets  of the repository's internal hash table.  If
 * size is zero, value STRING_REPO_DEFAULT_BUCKET_COUNT is used.
 *
 * pre-conditions:
 * o  global repository must be uninitialised upon entry
 * o  parameter size may be zero upon entry
 * o  parameter status may be NULL upon entry
 *
 * post-conditions:
 * o  global string repository is allocated and intialised.
 * o  M2C_STRING_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if repository has already been initialised upon entry,
 *    no operation is carried out and M2C_STRING_STATUS_ALREADY_INITIALIZED
 *    is passed back in status unless status is NULL
 * o  if repository allocation failed, M2C_STRING_STATUS_ALLOCATION_FAILED
 *    is passed back in status unless status is NULL
 * ----------------------------------------------------------------------- */

void intstr_init_repo (uint_t size, intstr_status_t *status) {
  
  uint_t index, bucket_count, allocation_size;
  
  /* check pre-conditions */
  if (repository != NULL) {
    SET_STATUS(status, INTSTR_STATUS_ALREADY_INITIALIZED);
    return;
  } /* end if */
  
  /* determine bucket count */
  if (size == 0) {
    bucket_count = INTSTR_REPO_DEFAULT_BUCKET_COUNT;
  }
  else /* size != 0 */ {
    bucket_count = size;
  } /* end if */
  
  /* allocate repository */
  allocation_size = sizeof(intstr_repo_s) +
    bucket_count * sizeof(intstr_repo_entry_t);
  repository = malloc(allocation_size);
  
  /* bail out if allocation failed */
  if (repository == NULL) {
    SET_STATUS(status, INTSTR_STATUS_ALLOCATION_FAILED);
    return;
  } /* end if */
  
  /* set entry and bucket count */
  repository->entry_count = 0;
  repository->bucket_count = bucket_count;
  
  /* initialise buckets */
  index = 0;
  while (index < bucket_count) {
    repository->bucket[index] = NULL;
    index++;
  } /* end while */
  
  SET_STATUS(status, INTSTR_STATUS_SUCCESS);
  return;
} /* end intstr_init_repo */


/* --------------------------------------------------------------------------
 * function intstr_for_cstr(str, status)
 * --------------------------------------------------------------------------
 * Returns an interned string object for str which must be a pointer to a NUL
 * terminated character string.  
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  if a string object for str is present in the internal repository,
 *    that string object is retrieved, retained and returned.
 * o  if no string object for str is present in the repository,
 *    a new string object with a copy of str is created, stored and returned.
 * o  INTSTR_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if str is NULL upon entry, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_INVALID_REFERENCE is
 *    passed back in status unless status is NULL
 * o  if string object allocation failed, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_ALLOCATION_FAILED is
 *    passed back in status unless status is NULL
 * ----------------------------------------------------------------------- */

static intstr_t new_string_from_string (char *str, uint_t length);
static intstr_repo_entry_t new_repo_entry (intstr_t str, intstr_hash_t key);
static bool matches_str_and_len (intstr_t str, const char *cmpstr, uint_t length);

intstr_t intstr_for_cstr (const char *str, intstr_status_t *status) {
  intstr_repo_entry_t new_entry, this_entry;
  intstr_t new_string, this_string;
  uint_t index, length;
  intstr_hash_t key;
  char ch;
  
  /* check repository */
  if (repository == NULL) {
    SET_STATUS(status, INTSTR_STATUS_NOT_INITIALIZED);
    return NULL;
  } /* end if */
  
  /* check str */
  if (str == NULL) {
    SET_STATUS(status, INTSTR_STATUS_INVALID_REFERENCE);
    return NULL;
  } /* end if */
    
  /* determine length and key */
  index = 0;
  ch = str[index];
  key = HASH_INITIAL;
  while (ch != ASCII_NUL) {
    key = HASH_NEXT_CHAR(key, ch);
    index++;
    ch = str[index];
  } /* end while */
  
  length = index + 1;
  key = HASH_FINAL(key);
  
  /* determine bucket index */
  index = key % repository->bucket_count;
  
  /* check if str is already in repository */
  if /* bucket empty */ (repository->bucket[index] == NULL) {
    
    /* create a new string object */
    new_string = new_string_from_string(str, length);
    
    /* create a new repository entry */
    new_entry = new_repo_entry(new_string, key);
    
    /* link the bucket to the new entry */
    repository->bucket[index] = new_entry;
    
    /* update the entry counter */
    repository->entry_count++;
    
    /* set status and return string object */
    SET_STATUS(status, INTSTR_STATUS_SUCCESS);
    return new_string;
  }
  else /* bucket not empty */ {
    
    /* first entry in bucket is starting point */
    this_entry = repository->bucket[index];
    
    /* search bucket for matching string */
    while (true) {
      if /* match found in current entry */
        ((this_entry->key == key) &&
          matches_str_and_len(this_entry->str, str, length)) {
        
        /* get string object of matching entry and retain it */
        this_string = this_entry->str;
        intstr_retain(this_string);
        
        /* set status and return string object */
        SET_STATUS(status, INTSTR_STATUS_SUCCESS);
        return this_string;
      }
      else if /* last entry reached without match */
        (this_entry->next == NULL) {
        
        /* create a new string object */
        new_string = new_string_from_string(str, length);
        
        /* create a new repository entry */
        new_entry = new_repo_entry(new_string, key);
        
        /* link last entry to the new entry */
        this_entry->next = new_entry;
        
        /* update the entry counter */
        repository->entry_count++;
        
        /* set status and return string object */
        SET_STATUS(status, INTSTR_STATUS_SUCCESS);
        return new_string;
      }
      else /* not last entry yet, move to next entry */ {
        this_entry = this_entry->next;
      } /* end if */
    } /* end while */      
  } /* end if */  
} /* end intstr_for_cstr */


/* --------------------------------------------------------------------------
 * function intstr_for_slice(str, offset, length, status)
 * --------------------------------------------------------------------------
 * Returns an interned string object for a given slice of str.  Parameter str
 * must be a pointer to a NUL terminated character string.  The position of
 * the first character of the slice is given by parameter offset and the
 * length of the slice is given by parameter length.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 * o  slice must be within range of str (NOT GUARDED),
 * o  slice must not contain any characters with code points < 32 or > 126 
 * 
 *
 * post-conditions:
 * o  if a string object for the slice is present in the internal repository,
 *    that string object is retrieved, retained and returned.
 * o  if no string object for the slice is present in the repository, a new
 *    string object with a copy of the slice is created, stored and returned.
 * o  INTSTR_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if precondition #1 is not met, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_INVALID_REFERENCE is
 *    passed back in status, unless status is NULL
 * o  if precondition #3 is not met, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_INVALID_INDICES is
 *    passed back in status, unless status is NULL
 * o  if string object allocation failed, no operation is carried out,
 *    NULL is returned and INTSTR_STATUS_ALLOCATION_FAILED is
 *    passed back in status unless status is NULL
 * ----------------------------------------------------------------------- */

static intstr_t new_string_from_slice (char *str, uint_t offset, uint_t length);

intstr_t intstr_for_slice
  (const char *str, uint_t offset, uint_t length, intstr_status_t *status) {
  
  intstr_repo_entry_t new_entry, this_entry;
  intstr_t new_string, this_string;
  uint_t index;
  intstr_hash_t key;
  char ch;
  
  /* check repository */
  if (repository == NULL) {
    SET_STATUS(status, INTSTR_STATUS_NOT_INITIALIZED);
    return NULL;
  } /* end if */
  
  /* check str */
  if (str == NULL) {
    SET_STATUS(status, INTSTR_STATUS_INVALID_REFERENCE);
    return NULL;
  } /* end if */
  
  /* determine key for slice */
  index = offset;
  key = HASH_INITIAL;
  while (index < length) {
    ch = str[index];
    /* bail out if any control codes are found */
    if (IS_CONTROL_CHAR(ch)) {
      SET_STATUS(status, INTSTR_STATUS_INVALID_INDICES);
      return NULL;
    } /* end if */
    key = HASH_NEXT_CHAR(key, ch);
    index++;
  } /* end while */
  
  key = HASH_FINAL(key);
  
  /* determine bucket index */
  index = key % repository->bucket_count;
  
  /* check if str is already in repository */
  if /* bucket empty */ (repository->bucket[index] == NULL) {
    
    /* create a new string object */
    new_string = new_string_from_slice(str, offset, length);
    
    /* create a new repository entry */
    new_entry = new_repo_entry(new_string, key);
    
    /* link the bucket to the new entry */
    repository->bucket[index] = new_entry;
    
    /* update the entry counter */
    repository->entry_count++;
    
    /* set status and return string object */
    SET_STATUS(status, INTSTR_STATUS_SUCCESS);
    return new_string;
  }
  else /* bucket not empty */ {
    
    /* first entry in bucket is starting point */
    this_entry = repository->bucket[index];
    
    /* search bucket for matching string */
    while (true) {
      if /* match found in current entry */
        ((this_entry->key == key) &&
          matches_str_and_len(this_entry->str, str, length)) {
        
        /* get string object of matching entry and retain it */
        this_string = this_entry->str;
        intstr_retain(this_string);
        
        /* set status and return string object */
        SET_STATUS(status, INTSTR_STATUS_SUCCESS);
        return this_string;
      }
      else if /* last entry reached without match */
        (this_entry->next == NULL) {
        
        /* create a new string object */
        new_string = new_string_from_slice(str, offset, length);
        
        /* create a new repository entry */
        new_entry = new_repo_entry(new_string, key);
        
        /* link last entry to the new entry */
        this_entry->next = new_entry;
        
        /* update the entry counter */
        repository->entry_count++;
        
        /* set status and return string object */
        SET_STATUS(status, INTSTR_STATUS_SUCCESS);
        return new_string;
      }
      else /* not last entry yet, move to next entry */ {
        this_entry = this_entry->next;
      } /* end if */
    } /* end while */      
  } /* end if */  
} /* end intstr_for_slice */



/* --------------------------------------------------------------------------
 * function intstr_for_concatenation(str, append_str, status)
 * --------------------------------------------------------------------------
 * Returns an interned string object for the character string  resulting from
 * concatenation of str  and  append_str.  Parameters str and append_str must
 * be pointers to NUL terminated character strings.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 * o  parameter append_str must not be NULL upon entry
 *
 * post-conditions:
 * o  if a string object for the resulting concatenation string is present in
 *    the internal repository, that string object is retrieved, retained and
 *    returned.
 * o  if no string object for the resulting concatenation is present in the
 *    repository, a new string object with a copy of the concatenation is
 *    created, stored and returned.
 * o  INTSTR_STATUS_SUCCESS is passed back in status, unless NULL
 *
 * error-conditions:
 * o  if any of str or append_str is NULL upon entry, no operation is
 *    carried out, NULL is returned and INTSTR_STATUS_INVALID_REFERENCE
 *    is passed back in status, unless NULL
 * o  if no dynamic string object could be allocated, NULL is returned,
 *    and INTSTR_STATUS_ALLOCATION_FAILED is passed back in status,
 *    unless NULL
 * ----------------------------------------------------------------------- */

static intstr_t new_string_by_appending
  (char *str, char *append_str, uint_t str_len, uint_t append_str_len);

intstr_t intstr_for_concatenation
  (const char *str, const char *append_str, intstr_status_t *status) {
  
  intstr_repo_entry_t new_entry, this_entry;
  intstr_t new_string, this_string;
  uint_t index, length, str_len, append_str_len;
  intstr_hash_t key;
  char ch;
  
  /* check repository */
  if (repository == NULL) {
    SET_STATUS(status, INTSTR_STATUS_NOT_INITIALIZED);
    return NULL;
  } /* end if */
  
  /* check str and append_str */
  if ((str == NULL) || (append_str == NULL)) {
    SET_STATUS(status, INTSTR_STATUS_INVALID_REFERENCE);
    return NULL;
  } /* end if */
  
  /* determine key and length of str */
  index = 0;
  ch = str[index];
  key = HASH_INITIAL;
  while (ch != ASCII_NUL) {
    key = HASH_NEXT_CHAR(key, ch);
    index++;
    ch = str[index];
  } /* end while */
  
  str_len = index + 1;
  
  /* determine key and length of append_str */
  index = 0;
  ch = append_str[index];
  key = HASH_INITIAL;
  while (ch != ASCII_NUL) {
    key = HASH_NEXT_CHAR(key, ch);
    index++;
    ch = append_str[index];
  } /* end while */
  
  append_str_len = index + 1;
  
  /* finalise length and key of concatenation */
  length = str_len + append_str_len;
  key = HASH_FINAL(key);
  
  /* determine bucket index */
  index = key % repository->bucket_count;
  
  /* check if str is already in repository */
  if /* bucket empty */ (repository->bucket[index] == NULL) {
    
    /* create a new string object */
    new_string =
      new_string_by_appending(str, append_str, str_len, append_str_len);
    
    /* create a new repository entry */
    new_entry = new_repo_entry(new_string, key);
    
    /* link the bucket to the new entry */
    repository->bucket[index] = new_entry;
    
    /* update the entry counter */
    repository->entry_count++;
    
    /* set status and return string object */
    SET_STATUS(status, INTSTR_STATUS_SUCCESS);
    return new_string;
  }
  else /* bucket not empty */ {
    
    /* first entry in bucket is starting point */
    this_entry = repository->bucket[index];
    
    /* search bucket for matching string */
    while (true) {
      if /* match found in current entry */
        ((this_entry->key == key) &&
          matches_str_and_len(this_entry->str, str, length)) {
        
        /* get string object of matching entry and retain it */
        this_string = this_entry->str;
        intstr_retain(this_string);
        
        /* set status and return string object */
        SET_STATUS(status, INTSTR_STATUS_SUCCESS);
        return this_string;
      }
      else if /* last entry reached without match */
        (this_entry->next == NULL) {
        
        /* create a new string object */
        new_string =
          new_string_by_appending(str, append_str, str_len, append_str_len);
        
        /* create a new repository entry */
        new_entry = new_repo_entry(new_string, key);
        
        /* link last entry to the new entry */
        this_entry->next = new_entry;
        
        /* update the entry counter */
        repository->entry_count++;
        
        /* set status and return string object */
        SET_STATUS(status, INTSTR_STATUS_SUCCESS);
        return new_string;
      }
      else /* not last entry yet, move to next entry */ {
        this_entry = this_entry->next;
      } /* end if */
    } /* end while */      
  } /* end if */  
} /* end intstr_for_concatenation */



/* --------------------------------------------------------------------------
 * function intstr_length(str)
 * --------------------------------------------------------------------------
 * Returns the length of the character string of str.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  the length of str's character string is returned
 *
 * error-conditions:
 * o  if str is NULL upon entry, zero is returned
 * ----------------------------------------------------------------------- */

uint_t intstr_length (intstr_t str) {
  
  if (str == NULL) {
    return 0;
  } /* end if */
  
  return str->length;
} /* end intstr_length */


/* --------------------------------------------------------------------------
 * function intstr_char_ptr(str)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to the character string of str.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  an immutable pointer to str's character string is returned
 *
 * error-conditions:
 * o  if str is NULL upon entry, NULL is returned
 * ----------------------------------------------------------------------- */

const char *intstr_char_ptr (intstr_t str) {
  
  if (str == NULL) {
    return NULL;
  } /* end if */
  
  return (const char *) &(str->char_array);
} /* end intstr_char_ptr */


/* --------------------------------------------------------------------------
 * function intstr_count()
 * --------------------------------------------------------------------------
 * Returns the number of interned strings stored in the string repository.
 *
 * pre-conditions:
 * o  none
 *
 * post-conditions:
 * o  none
 *
 * error-conditions:
 * o  none
 * ----------------------------------------------------------------------- */

uint_t intstr_count (void) {
  
  if (repository == NULL) {
    return 0;
  } /* end if */
  
  return repository->entry_count;
} /* end intstr_count */


/* --------------------------------------------------------------------------
 * function intstr_retain(str)
 * --------------------------------------------------------------------------
 * Prevents str from deallocation.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  str's reference count is incremented.
 *
 * error-conditions:
 * o  if str is not NULL upon entry, no operation is carried out
 * ----------------------------------------------------------------------- */

void intstr_retain (intstr_t str) {
  
  if ((str != NULL) && (str->ref_count > 0)) {
    str->ref_count++;
  } /* end if */
  
} /* end intstr_retain */


/* --------------------------------------------------------------------------
 * function intstr_release(str)
 * --------------------------------------------------------------------------
 * Cancels an outstanding retain, or deallocates str if there are no
 * outstanding retains.
 *
 * pre-conditions:
 * o  parameter str must not be NULL upon entry
 *
 * post-conditions:
 * o  if str's reference count is zero upon entry, str is deallocated
 * o  if str's reference count is not zero upon entry, it is decremented
 *
 * error-conditions:
 * o  if str is not NULL upon entry, no operation is carried out
 * ----------------------------------------------------------------------- */

static m2c_hash_t key_for_string (intstr_t str);

void intstr_release (intstr_t str) {
  
  m2c_hash_t key;
  
  if (str == NULL) {
    return;
  } /* end if */
  
  /* printf("  release called on '%s', refcount %u\n",
    str->char_array, str->ref_count); */
  
  if (str->ref_count > 1) {
    str->ref_count--;
    /* printf("  new refcount %u\n", str->ref_count); */
  }
  else if (str->ref_count == 1) {
    /* remove and deallocate */
    printf("* deallocating string '%s' (%p)\n", intstr_char_ptr(str), str);
    
    /* remove from repo */
    key = key_for_string(str);
    remove_repo_entry(str, key);
    
    /* reset */
    str->length = 0;
    str->ref_count = 0;
    str->char_array[0] = ASCII_NUL;
    
    /* deallocate */
    free(str);
  } /* end if */
} /* end intstr_release */


/* *********************************************************************** *
 * Private Functions                                                       *
 * *********************************************************************** */


/* TO DO */


#endif /* INTSTR_H */

/* END OF FILE */