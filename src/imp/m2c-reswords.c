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


/* --------------------------------------------------------------------------
 * function m2c_reword_token_for_lexeme(lexeme, default_token)
 * --------------------------------------------------------------------------
 * Tests  if lexeme matches  a reserved word  and  returns  its corresponding
 * token.  Returns default_token if lexeme does not match a reserved word.
 * ----------------------------------------------------------------------- */

m2c_token_t m2c_reword_token_for_lexeme
  (intstr_t lexeme, m2c_token_t default_token) {
  const char *lexstr;
  uint_t length;
  
  if (lexeme == NULL) {
    return default_token;
  } /* end if */

  lexstr = intstr_char_ptr(lexeme);
  length = intstr_length(lexeme);

  if ((length < 2) || (length > 14)) {
    return default_token;
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
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_AND])) {
	        return TOKEN_AND;
	      } /* end if */

          return default_token;
        	
	    case 'D' :
	      /* DIV */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_DIV])) {
	        return TOKEN_DIV;
	      } /* end if */
	      
          return default_token;
	
	    case 'E' :
	      /* END */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_END])) {
	        return TOKEN_END;
	      } /* end if */
	      
          return default_token;
	      
	    case 'F' :
	      /* FOR */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_FOR])) {
	        return TOKEN_FOR;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'M' :
	      /* MOD */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_MOD])) {
	        return TOKEN_MOD;
	      } /* end if */
          
          return default_token;
	      
	    case 'N' :

          switch (lexstr[2]) {
	        case 'P' :
              /* NOP */
	          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_NOP])) {
	            return TOKEN_NOP;
              } /* end if */
              
              return default_token;
            
            case 'T' :
              /* NOT */
	          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_NOT])) {
	            return TOKEN_NOT;
              } /* end if */
              
              return default_token;

            case 'W' :
              /* NEW */
	          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_NEW])) {
	            return TOKEN_NEW;
              } /* end if */
              
              return default_token;
          
	      } /* end switch */
	      
          return default_token;
	      
	    case 'S' :
	      /* SET */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_SET])) {
	        return TOKEN_SET;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'V' :
	      /* VAR */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_VAR])) {
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
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_CASE])) {
	        return TOKEN_CASE;
	      } /* end if */
	      
	      return default_token;

        case 'E' :
          /* READ */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_READ])) {
	        return TOKEN_READ;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'H' :
	      /* THEN */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_THEN])) {
	        return TOKEN_THEN;
	      } /* end if */
          
          return default_token;
	      
	    case 'L' :
	      /* ELSE */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_ELSE])) {
	        return TOKEN_ELSE;
	      } /* end if */
	      
	      return default_token;
	      
	    case 'O' :
          switch (lexstr[3]) {

            case 'P' :
	          /* LOOP */
	          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_LOOP])) {
	            return TOKEN_LOOP;
	          } /* end if */
              
              return default_token;
            
            case 'Y' :
              /* COPY */
	          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_LOOP])) {
	            return TOKEN_LOOP;
	          } /* end if */
              
              return default_token;
          } /* end switch */

          return default_token;

	    case 'X' :
	      /* EXIT */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_EXIT])) {
	        return TOKEN_EXIT;
	      } /* end if */	      
	      
	      return default_token;
	      
        case 'Y' :
          /* TYPE */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_TYPE])) {
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
	          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_WHILE])) {
	            return TOKEN_WHILE;
	          } /* end if */
              
              return default_token;
              
            case 'T' :
              /* WRITE */
	          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_WRITE])) {
	            return TOKEN_WRITE;
	          } /* end if */

              return default_token;
            
	        default :
	          return default_token;
          } /* end switch */
          
        case 'F' :
          /* ELSIF */
          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_ELSIF])) {
            return TOKEN_ELSIF;
          } /* end if */

          return default_token;

        case 'L' :
          /* UNTIL */
          if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_UNTIL])) {
            return TOKEN_UNTIL;
          } /* end if */

          return default_token;

	    case 'N' :
	      /* BEGIN */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_BEGIN])) {
	        return TOKEN_BEGIN;
	      } /* end if */
          
          return default_token;
	      
	    case 'S' :
	      /* ALIAS */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_ALIAS])) {
	        return TOKEN_ALIAS;
	      } /* end if */
          
          return default_token;

	    case 'T' :
	      /* CONST */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_CONST])) {
	        return TOKEN_CONST;
	      } /* end if */
          
          return default_token;

	    case 'Y' :
	      /* ARRAY */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_ARRAY])) {
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
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_OPAQUE])) {
	        return TOKEN_OPAQUE;
	      } /* end if */
          
          return default_token;

        case 'C' :
          /* RECORD */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_RECORD])) {
	        return TOKEN_RECORD;
	      } /* end if */
          
          return default_token;

        case 'D' :
          /* MODULE */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_MODULE])) {
	        return TOKEN_MODULE;
	      } /* end if */
          
          return default_token;

        case 'P' :
          switch (lexstr[0]) {
            case 'I' :
              /* IMPORT */
              if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_IMPORT])) {
                return TOKEN_IMPORT;
              } /* end if */

              return default_token;

            case 'R' :
              /* REPEAT */
              if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_REPEAT])) {
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
              if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_RETAIN])) {
                return TOKEN_RETAIN;
              } /* end if */

              return default_token;

            case 'U' :
              /* RETURN */
              if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_RETURN])) {
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
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_ARGLIST])) {
	        return TOKEN_ARGLIST;
	      } /* end if */
          
          return default_token;

        case 'P' :
          /* POINTER */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_POINTER])) {
	        return TOKEN_POINTER;
	      } /* end if */
          
          return default_token;

        case 'R' :
          /* RELEASE */
	      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_RELEASE])) {
	        return TOKEN_RELEASE;
	      } /* end if */
          
          return default_token;

	    default :
	      return default_token;
      } /* end switch */
      
    case /* length = 8 */ 8 :
      /* OCTETSEQ */
	  if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_OCTETSEQ])) {
	    return TOKEN_OCTETSEQ;
	  } /* end if */
          
      return default_token;
      
    case /* length = 9 */ 9 :
      /* PROCEDURE */
	  if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_PROCEDURE])) {
	    return TOKEN_PROCEDURE;
	  } /* end if */
          
      return default_token;
	
    case /* length = 11 */ 11 :
      /* UNQUALIFIED */
      if (str_match(lexstr, m2c_resword_lexeme_table[TOKEN_UNQUALIFIED])) {
	    return TOKEN_UNQUALIFIED;
      } /* end if */
      
      return default_token;
    
    case /* length = 14 */ 14 :
      /* IMPLEMENTATION */
      if (str_match(lexstr,
          m2c_resword_lexeme_table[TOKEN_IMPLEMENTATION])) {
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

} /* end m2c_resword_lexeme_for_token */


/* END OF FILE */