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
  * m2c-digest.c                                                              *
  *                                                                           *
  * Implementation of the M2C module digest library.                          *
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

#ifndef M2C_DIGEST_H
#define M2C_DIGEST_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "m2c-digest.h"

#include <stdint.h>
#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_digest_t
 * --------------------------------------------------------------------------
 * Type to hold a digest context.
 * ----------------------------------------------------------------------- */

typedef struct {
  m2c_digest_value_t c0, c1;
  char remaining_char;
  bool prepend_spacer;
  bool finalized;
} m2c_digest_s;

typedef m2c_digest_s *m2c_digest_t;


/* --------------------------------------------------------------------------
 * function m2c_digest_init()
 * --------------------------------------------------------------------------
 * Returns a newly allocated and initialised digest context.
 * ----------------------------------------------------------------------- */

m2c_digest_t m2c_digest_init (void) {
  m2c_digest_t new_context;
  
  new_context = malloc(sizeof(m2c_digest_s));
  
  if (new_context == NULL) {
    return NULL;
  } /* end if */
  
  new_context->c0 = 0;
  new_context->c1 = 0;
  new_context->remaining_char = ASCII_NUL;
  new_context->prepend_spacer = false;
  new_context->finalized = false;
  
  return new_context;
} /* end m2c_digest_init */


/* --------------------------------------------------------------------------
 * procedure m2c_digest_add_token(context, mode, token)
 * --------------------------------------------------------------------------
 * Updates the rolling digest value of context with the lexeme of token while
 * prepending a single whitespace depending on mode.
 * ----------------------------------------------------------------------- */

static void digest_add_cstr
  (m2c_digest_t digest, m2c_digest_mode_t mode,
   uint16_t len, const char *lexstr);
  
void m2c_digest_add_token
  (m2c_digest_t digest, m2c_digest_mode_t mode, m2c_token_t token) {
  
  const char *lexstr;
  uint16_t len;
  
  lexstr = m2c_lexeme_for_special_symbol(token);
  
  len = 0;
  while (lexstr[len] != ASCII_NUL) {
    len++;
  } /* end while */
  
  digest_add_cstr(digest, mode, len, lexstr);
  
} /* end m2c_digest_add_token */


/* --------------------------------------------------------------------------
 * procedure m2c_digest_add_lexeme(context, mode, lexeme)
 * --------------------------------------------------------------------------
 * Updates the rolling digest value of context with lexeme while prepending
 * a single whitespace depending on mode.
 * ----------------------------------------------------------------------- */

void m2c_digest_add_lexeme
  (m2c_digest_t digest, m2c_digest_mode_t mode, intstr_t lexeme) {
 
  digest_add_cstr
    (digest, mode, intstr_length(lexeme), intstr_char_ptr(lexeme));
  
} /* end m2c_digest_add_lexeme */
  

/* --------------------------------------------------------------------------
 * function m2c_digest_finalize(context)
 * --------------------------------------------------------------------------
 * Finalizes the digest calculation of context.
 * ----------------------------------------------------------------------- */

void m2c_digest_finalize (m2c_digest_t digest) {
  if (digest->remaining_char != ASCII_NUL) {
    word = digest->remaining_char << 8;
    digest->remaining_char = ASCII_NUL;
    
    digest->c0 = digest->c0 + word;
    digest->c1 = digest->c1 + digest->c0;
    
    digest->c0 = digest->c0 % 0xFFFF;
    digest->c1 = digest->c1 % 0xFFFF;
  } /* end if */
  
  digest->finalized = true;
} /* end m2c_digest_finalize */


/* --------------------------------------------------------------------------
 * function m2c_digest_value(context)
 * --------------------------------------------------------------------------
 * Returns the digest value of context.
 * ----------------------------------------------------------------------- */

m2c_digest_value_t m2c_digest_value (m2c_digest_t digest) {
  
  return digest->c1 << 16 | digest->c0;
  
} /* end m2c_digest_value */


/* --------------------------------------------------------------------------
 * function m2c_digest_release(context)
 * --------------------------------------------------------------------------
 * Releases context, returns NULL.
 * ----------------------------------------------------------------------- */

m2c_digest_t m2c_digest_release (m2c_digest_t digest) {

  free(digest);
  return NULL;

} /* end m2c_digest_release */


/* --------------------------------------------------------------------------
 * private procedure digest_add_cstr(context, mode, len, lexstr)
 * --------------------------------------------------------------------------
 * Updates the rolling digest value of context with lexstr while prepending
 * a single whitespace depending on mode.
 * ----------------------------------------------------------------------- */

#define IS_ODD(_n) (_n % 2)

static void digest_add_cstr
  (m2c_digest_t digest, m2c_digest_mode_t mode, 
   uint16_t len, const char *lexstr) {
  
  uint16_t word, index;
  
  if (digest->remaining_char != ASCII_NUL) {
    
    word = digest->remaining_char << 8;
    
    if (prepend_spacer) {
      word = word | 0x20;
      index = 0;
    }
    else /* no spacer */ {
      word = word | lexstr[0];
      index = 1;
    } /* end if */
        
    if (IS_ODD(len-index)) {
      len = len - 1;
      digest->remaining_char = lexstr[len];
    }
    else {
      digest->remaining_char = ASCII_NUL;
    } /* end if */
  }
  else /* no remaining character */ {
    
    if (prepend_spacer) {
      if(IS_ODD(len+1)) {
        len = len - 1;
        digest->remaining_char = lexstr[len];
      } /* end if */
    
      word = 0x2000; /* whitespace << 8 */
      word = word | lexstr[0];
      index = 1;
    }
    else /* no spacer */ {
      if (len == 1) {
        digest->remaining_char = lexstr[0];
        return;
      }
      else if (IS_ODD(len)) {
        len = len - 1;
        digest->remaining_char = lexstr[len];
      } /* end if */
    
      word = lexstr[0] << 8;
      word = word | lexstr[1];
      index = 2;
    } /* end if */
  } /* end if */
  
  digest->c0 = digest->c0 + word;
  digest->c1 = digest->c1 + digest->c0;
  
  while (index < len) {
    word = (lexstr[index] << 8); index++;
    word = word | lexstr[index]; index++;
    digest->c0 = digest->c0 + word;
    digest->c1 = digest->c1 + digest->c0;
  } /* end while */
  
  digest->c0 = digest->c0 % 0xFFFF;
  digest->c1 = digest->c1 % 0xFFFF;
  
  return;
} /* end digest_add_cstr */

/* END OF FILE */