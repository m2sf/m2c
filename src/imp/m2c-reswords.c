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
 * m2c-reswords.c                                                            *
 *                                                                           *
 * Implementation of Modula-2 reserved word matching module.                 *
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

#include "m2c-reswords.h"

#include <stdbool>


/* --------------------------------------------------------------------------
 * resword lexeme table
 * ----------------------------------------------------------------------- */

#define RESWORD_COUNT (LAST_RESERVED_WORD_TOKEN + 1)

static intstr_t lextab[RESWORD_COUNT];

static bool lextab_not_initialized = true;

static void init_resword_lextab(void);


/* --------------------------------------------------------------------------
 * function m2c_reword_token_for_lexeme(lexeme, default_token)
 * --------------------------------------------------------------------------
 * Tests  if lexeme matches  a reserved word  and  returns  its corresponding
 * token.  Returns default_token if lexeme does not match a reserved word.
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_reword_token_for_lexeme
  (intstr_t lexeme, m2c_token_t default_token) {
  
  const char *lexstr;
  unsigned int length;
  
  if (lexeme == NULL) {
    return default_token;
  } /* end if */

  lexstr = intstr_char_ptr(lexeme);
  length = intstr_length(lexeme);

  if ((length < 2) || (length > 14)) {
    return default_token;
  } /* end if */

  if (lextab_not_initialized) {
    init_resword_lextab();
  } /* end if */
  
  switch (length) {
    case /* length = 2 */ 2 :
      switch (lexstr[0]) {
      
	    case 'D' :
	      /* DO */
	      if (lexstr[1] == 'O') {
	        return TOKEN_DO;
	      } /* end if */

          return default_token;
	
	    case 'I' :
	      /* IF */
	      if (lexstr[1] == 'F') {
	        return TOKEN_IF;
	      }
          	  	    
	      /* IN */
	      else if (lexstr[1] == 'N') {
	        return TOKEN_IN;
          } /* end if */

	      return default_token;

	    case 'O' :
	      /* OF */
	      if (lexstr[1] == 'F') {
	        return TOKEN_OF;
	      }
	      
	      /* OR */
	      else if (lexstr[1] == 'R') {
	        return TOKEN_OR;
	      } /* end if */

	      return default_token;

	    case 'T' :
	      /* TO */
	      if (lexstr[1] == 'O') {
	        return TOKEN_TO;
	      } /* end if */

	      return default_token;

	    default :
	      return default_token;
	  } /* end switch */
	
    case /* length = 3 */ 3 :
      switch (lexstr[0]) {
      
	    case 'A' :
	      /* AND */
	      if (lexeme == lextab[TOKEN_AND]) {
	        return TOKEN_AND;
	      } /* end if */

          return default_token;
        	
	    case 'D' :
	      /* DIV */
	      if (lexeme == lextab[TOKEN_DIV]) {
	        return TOKEN_DIV;
	      } /* end if */
	      
          return default_token;
	
	    case 'E' :
	      /* END */
	      if (lexeme == lextab[TOKEN_END]) {
	        return TOKEN_END;
	      } /* end if */
	      
          return default_token;
	      
	    case 'F' :
	      /* FOR */
	      if (lexeme == lextab[TOKEN_FOR]) {
	        return TOKEN_FOR;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'M' :
	      /* MOD */
	      if (lexeme == lextab[TOKEN_MOD]) {
	        return TOKEN_MOD;
	      } /* end if */
          
          return default_token;
	      
	    case 'N' :

          switch (lexstr[2]) {
	        case 'P' :
              /* NOP */
	          if (lexeme == lextab[TOKEN_NOP]) {
	            return TOKEN_NOP;
              } /* end if */
              
              return default_token;
            
            case 'T' :
              /* NOT */
	          if (lexeme == lextab[TOKEN_NOT]) {
	            return TOKEN_NOT;
              } /* end if */
              
              return default_token;

            case 'W' :
              /* NEW */
	          if (lexeme == lextab[TOKEN_NEW]) {
	            return TOKEN_NEW;
              } /* end if */
              
              return default_token;
          
	      } /* end switch */
	      
          return default_token;
	      
	    case 'S' :
	      /* SET */
	      if (lexeme == lextab[TOKEN_SET]) {
	        return TOKEN_SET;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'V' :
	      /* VAR */
	      if (lexeme == lextab[TOKEN_VAR]) {
	        return TOKEN_VAR;
	      } /* end if */
	      
	      return default_token;
	      
	    default :
	      return default_token;
	  } /* end switch */
	
    case /* length = 4 */ 4 :
      switch (lexstr[1]) {
      
	    case 'A' :
	      /* CASE */
	      if (lexeme == lextab[TOKEN_CASE]) {
	        return TOKEN_CASE;
	      } /* end if */
	      
	      return default_token;

        case 'E' :
          /* READ */
	      if (lexeme == lextab[TOKEN_READ]) {
	        return TOKEN_READ;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'H' :
	      /* THEN */
	      if (lexeme == lextab[TOKEN_THEN]) {
	        return TOKEN_THEN;
	      } /* end if */
          
          return default_token;
	      
	    case 'L' :
	      /* ELSE */
	      if (lexeme == lextab[TOKEN_ELSE]) {
	        return TOKEN_ELSE;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'O' :
          switch (lexstr[3]) {

            case 'P' :
	          /* LOOP */
	          if (lexeme == lextab[TOKEN_LOOP]) {
	            return TOKEN_LOOP;
	          } /* end if */
              
              return default_token;
            
            case 'Y' :
              /* COPY */
	          if (lexeme == lextab[TOKEN_COPY]) {
	            return TOKEN_LOOP;
	          } /* end if */
              
              return default_token;
          } /* end switch */

          return default_token;

	    case 'X' :
	      /* EXIT */
	      if (lexeme == lextab[TOKEN_EXIT]) {
	        return TOKEN_EXIT;
	      } /* end if */	      
	      
	      return default_token;
	      
        case 'Y' :
          /* TYPE */
	      if (lexeme == lextab[TOKEN_TYPE]) {
	        return TOKEN_TYPE;
	      } /* end if */
          
          return default_token;
	      
	    default :
	      return default_token;
	  } /* end switch */
	
    case /* length = 5 */ 5 :
      switch (lexstr[4]) {
        
        case 'E' :
          switch (lexstr[3]) {
            case 'L' :
              /* WHILE */
	          if (lexeme == lextab[TOKEN_WHILE]) {
	            return TOKEN_WHILE;
	          } /* end if */
              
              return default_token;
              
            case 'T' :
              /* WRITE */
	          if (lexeme == lextab[TOKEN_WRITE]) {
	            return TOKEN_WRITE;
	          } /* end if */

              return default_token;
            
	        default :
	          return default_token;
          } /* end switch */
          
        case 'F' :
          /* ELSIF */
          if (lexeme == lextab[TOKEN_ELSIF]) {
            return TOKEN_ELSIF;
          } /* end if */

          return default_token;

        case 'L' :
          /* UNTIL */
          if (lexeme == lextab[TOKEN_UNTIL]) {
            return TOKEN_UNTIL;
          } /* end if */

          return default_token;

	    case 'N' :
	      /* BEGIN */
	      if (lexeme == lextab[TOKEN_BEGIN]) {
	        return TOKEN_BEGIN;
	      } /* end if */
          
          return default_token;
	      
	    case 'S' :
	      /* ALIAS */
	      if (lexeme == lextab[TOKEN_ALIAS]) {
	        return TOKEN_ALIAS;
	      } /* end if */
          
          return default_token;

	    case 'T' :
	      /* CONST */
	      if (lexeme == lextab[TOKEN_CONST]) {
	        return TOKEN_CONST;
	      } /* end if */
          
          return default_token;

	    case 'Y' :
	      /* ARRAY */
	      if (lexeme == lextab[TOKEN_ARRAY]) {
	        return TOKEN_ARRAY;
	      } /* end if */
          
          return default_token;
	      
	    default :
	      return default_token;
	  } /* end switch */
	
    case /* length 6 */ 6 :
      switch (lexstr[2]) {
        case 'A' :
          /* OPAQUE */
	      if (lexeme == lextab[TOKEN_OPAQUE]) {
	        return TOKEN_OPAQUE;
	      } /* end if */
          
          return default_token;

        case 'C' :
          /* RECORD */
	      if (lexeme == lextab[TOKEN_RECORD]) {
	        return TOKEN_RECORD;
	      } /* end if */
          
          return default_token;

        case 'D' :
          /* MODULE */
	      if (lexeme == lextab[TOKEN_MODULE]) {
	        return TOKEN_MODULE;
	      } /* end if */
          
          return default_token;

        case 'P' :
          switch (lexstr[0]) {
            case 'I' :
              /* IMPORT */
              if (lexeme == lextab[TOKEN_IMPORT]) {
                return TOKEN_IMPORT;
              } /* end if */

              return default_token;

            case 'R' :
              /* REPEAT */
              if (lexeme == lextab[TOKEN_REPEAT]) {
                return TOKEN_REPEAT;
              } /* end if */

              return default_token;

            default :
              return default_token;
          } /* end switch */
        
        case 'T' :
          switch (lexstr[3]) {
            case 'A' :
              /* RETAIN */
              if (lexeme == lextab[TOKEN_RETAIN]) {
                return TOKEN_RETAIN;
              } /* end if */

              return default_token;

            case 'U' :
              /* RETURN */
              if (lexeme == lextab[TOKEN_RETURN]) {
                return TOKEN_RETURN;
              } /* end if */

              return default_token;

            default :
              return default_token;
          } /* end switch */

	    default :
	      return default_token;
      } /* end switch */


    case /* length = 7 */ 7 :
      switch (lexstr[0]) {
        case 'A' :
          /* ARGLIST */
	      if (lexeme == lextab[TOKEN_ARGLIST]) {
	        return TOKEN_ARGLIST;
	      } /* end if */
          
          return default_token;

        case 'P' :
          /* POINTER */
	      if (lexeme == lextab[TOKEN_POINTER]) {
	        return TOKEN_POINTER;
	      } /* end if */
          
          return default_token;

        case 'R' :
          /* RELEASE */
	      if (lexeme == lextab[TOKEN_RELEASE]) {
	        return TOKEN_RELEASE;
	      } /* end if */
          
          return default_token;

	    default :
	      return default_token;
      } /* end switch */
      
    case /* length = 8 */ 8 :
      /* OCTETSEQ */
	  if (lexeme == lextab[TOKEN_OCTETSEQ]) {
	    return TOKEN_OCTETSEQ;
	  } /* end if */
          
      return default_token;
      
    case /* length = 9 */ 9 :
      /* PROCEDURE */
	  if (lexeme == lextab[TOKEN_PROCEDURE]) {
	    return TOKEN_PROCEDURE;
	  } /* end if */
          
      return default_token;
	
    case /* length = 11 */ 11 :
      /* UNQUALIFIED */
      if (lexeme == lextab[TOKEN_UNQUALIFIED]) {
	    return TOKEN_UNQUALIFIED;
      } /* end if */
      
      return default_token;
    
    case /* length = 14 */ 14 :
      /* IMPLEMENTATION */
      if (lexeme == lextab[TOKEN_IMPLEMENTATION]) {
	    return TOKEN_IMPLEMENTATION;
      } /* end if */
      
      return default_token;
    
    default :
      return default_token;
  } /* end switch (length) */
} /* end m2c_reword_token_for_lexeme */


/* --------------------------------------------------------------------------
 * function m2c_resword_lexeme_for_token(token)
 * --------------------------------------------------------------------------
 * Tests  if token represents  a reserved word  and returns its corresponding
 * lexeme.  Returns NULL if token does not represent a reserved word.
 * ----------------------------------------------------------------------- */

intstr_t m2c_resword_lexeme_for_token (m2c_token_t token) {
  if m2c_token_is_resword(token) {

    if (lextab_not_initialized) {
      init_resword_lextab();
    } /* end if */

    return lextab[token];
  }
  else {
    return NULL;
  } /* end if */
} /* end m2c_resword_lexeme_for_token */


/* Private Functions */

/* --------------------------------------------------------------------------
 * procedure init_resword_lextab()
 * --------------------------------------------------------------------------
 * Initialises the resword lexeme table. Clears lextab_not_initialized flag.
 * ----------------------------------------------------------------------- */

static void init_resword_lextab(void) {
  lextab[TOKEN_ALIAS] = intstr_for_cstr("ALIAS", NULL);
  lextab[TOKEN_AND] = intstr_for_cstr("AND", NULL);
  lextab[TOKEN_ARGLIST] = intstr_for_cstr("ARGLIST", NULL);
  lextab[TOKEN_ARRAY] = intstr_for_cstr("ARRAY", NULL);
  lextab[TOKEN_BEGIN] = intstr_for_cstr("BEGIN", NULL);
  lextab[TOKEN_CASE] = intstr_for_cstr("CASE", NULL);
  lextab[TOKEN_CONST] = intstr_for_cstr("CONST", NULL);
  lextab[TOKEN_COPY] = intstr_for_cstr("COPY", NULL);
  lextab[TOKEN_DEFINITION] = intstr_for_cstr("DEFINITION", NULL);
  lextab[TOKEN_DIV] = intstr_for_cstr("DIV", NULL);
  lextab[TOKEN_DO] = intstr_for_cstr("DO", NULL);
  lextab[TOKEN_ELSE] = intstr_for_cstr("ELSE", NULL);
  lextab[TOKEN_ELSIF] = intstr_for_cstr("ELSIF", NULL);
  lextab[TOKEN_END] = intstr_for_cstr("END", NULL);
  lextab[TOKEN_EXIT] = intstr_for_cstr("EXIT", NULL);
  lextab[TOKEN_FOR] = intstr_for_cstr("FOR", NULL);
  lextab[TOKEN_IF] = intstr_for_cstr("IF", NULL);
  lextab[TOKEN_IMPLEMENTATION] = intstr_for_cstr("IMPLEMENTATION", NULL);
  lextab[TOKEN_IMPORT] = intstr_for_cstr("IMPORT", NULL);
  lextab[TOKEN_IN] = intstr_for_cstr("IN", NULL);
  lextab[TOKEN_LOOP] = intstr_for_cstr("LOOP", NULL);
  lextab[TOKEN_MOD] = intstr_for_cstr("MOD", NULL);
  lextab[TOKEN_MODULE] = intstr_for_cstr("MODULE", NULL);
  lextab[TOKEN_NEW] = intstr_for_cstr("NEW", NULL);
  lextab[TOKEN_NOP] = intstr_for_cstr("NOP", NULL);
  lextab[TOKEN_NOT] = intstr_for_cstr("NOT", NULL);
  lextab[TOKEN_OCTETSEQ] = intstr_for_cstr("OCTETSEQ", NULL);
  lextab[TOKEN_OF] = intstr_for_cstr("OF", NULL);
  lextab[TOKEN_OPAQUE] = intstr_for_cstr("OPAQUE", NULL);
  lextab[TOKEN_OR] = intstr_for_cstr("OR", NULL);
  lextab[TOKEN_POINTER] = intstr_for_cstr("POINTER", NULL);
  lextab[TOKEN_PROCEDURE] = intstr_for_cstr("PROCEDURE", NULL);
  lextab[TOKEN_READ] = intstr_for_cstr("READ", NULL);
  lextab[TOKEN_RECORD] = intstr_for_cstr("RECORD", NULL);
  lextab[TOKEN_RELEASE] = intstr_for_cstr("RELEASE", NULL);
  lextab[TOKEN_REPEAT] = intstr_for_cstr("REPEAT", NULL);
  lextab[TOKEN_RETAIN] = intstr_for_cstr("RETAIN", NULL);
  lextab[TOKEN_RETURN] = intstr_for_cstr("RETURN", NULL);
  lextab[TOKEN_SET] = intstr_for_cstr("SET", NULL);
  lextab[TOKEN_THEN] = intstr_for_cstr("THEN", NULL);
  lextab[TOKEN_TO] = intstr_for_cstr("TO", NULL);
  lextab[TOKEN_TYPE] = intstr_for_cstr("TYPE", NULL);
  lextab[TOKEN_UNQUALIFIED] = intstr_for_cstr("UNQUALIFIED", NULL);
  lextab[TOKEN_UNTIL] = intstr_for_cstr("UNTIL", NULL);
  lextab[TOKEN_VAR] = intstr_for_cstr("VAR", NULL);
  lextab[TOKEN_WHILE] = intstr_for_cstr("WHILE", NULL);
  lextab[TOKEN_WRITE] = intstr_for_cstr("WRITE", NULL);
  lextab_not_initialized = false;
  return;
} /* end init_resword_lextab */


/* END OF FILE */