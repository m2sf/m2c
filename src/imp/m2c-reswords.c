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
      switch (lexeme[0]) {
      
	    case 'B' :
	      /* BY */
	      if (lexeme[1] == 'Y') {
	        return TOKEN_BY;
	      }
	    
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	
	    case 'D' :
	      /* DO */
	      if (lexeme[1] == 'O') {
	        return TOKEN_DO;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	
	    case 'I' :
	      /* IF */
	      if (lexeme[1] == 'F') {
	        return TOKEN_IF;
	      }
	  	    
	      /* IN */
	      else if (lexeme[1] == 'N') {
	        return TOKEN_IN;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	    
	    case 'O' :
	      /* OF */
	      if (lexeme[1] == 'F') {
	        return TOKEN_OF;
	      }
	      
	      /* OR */
	      else if (lexeme[1] == 'R') {
	        return TOKEN_OR;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	    
	    case 'T' :
	      /* TO */
	      if (lexeme[1] == 'O') {
	        return TOKEN_TO;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	    
	    default :
	      return TOKEN_UNKNOWN;
	  } /* end switch */
	
    case /* length = 3 */ 3 :
      switch (lexeme[0]) {
      
	    case 'A' :
	      /* AND */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_AND])) {
	        return TOKEN_AND;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
        	
	    case 'D' :
	      /* DIV */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_DIV])) {
	        return TOKEN_DIV;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	
	    case 'E' :
	      /* END */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_END])) {
	        return TOKEN_END;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'F' :
	      /* FOR */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_FOR])) {
	        return TOKEN_FOR;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'M' :
	      /* MOD */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_MOD])) {
	        return TOKEN_MOD;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'N' :
	      /* NOT */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_NOT])) {
	        return TOKEN_NOT;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'S' :
	      /* SET */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_SET])) {
	        return TOKEN_SET;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'V' :
	      /* VAR */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_VAR])) {
	        return TOKEN_VAR;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    default :
	      return TOKEN_UNKNOWN;
	  } /* end switch */
	
    case /* length = 4 */ 4 :
      switch (lexeme[1]) {
      
	    case 'A' :
	      /* CASE */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_CASE])) {
	        return TOKEN_CASE;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'H' :
	      /* THEN */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_THEN])) {
	        return TOKEN_THEN;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'I' :
	      /* WITH */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_WITH])) {
	        return TOKEN_WITH;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'L' :
	      /* ELSE */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_ELSE])) {
	        return TOKEN_ELSE;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'O' :
	      /* LOOP */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_LOOP])) {
	        return TOKEN_LOOP;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'R' :
	      /* FROM */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_FROM])) {
	        return TOKEN_FROM;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'X' :
	      /* EXIT */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_EXIT])) {
	        return TOKEN_EXIT;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
        case 'Y' :
          /* TYPE */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_TYPE])) {
	        return TOKEN_TYPE;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    default :
	      return TOKEN_UNKNOWN;
	  } /* end switch */
	
    case /* length = 5 */ 5 :
      switch (lexeme[0]) {
      
	    case 'A' :
	      /* ARRAY */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_ARRAY])) {
	        return TOKEN_ARRAY;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'B' :
	      /* BEGIN */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_BEGIN])) {
	        return TOKEN_BEGIN;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'C' :
	      /* CONST */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_CONST])) {
	        return TOKEN_CONST;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'E' :
	      /* ELSIF */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_ELSIF])) {
	        return TOKEN_ELSIF;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'U' :
	      /* UNTIL */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_UNTIL])) {
	        return TOKEN_UNTIL;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'W' :
	      /* WHILE */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_WHILE])) {
	        return TOKEN_WHILE;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    default :
	      return TOKEN_UNKNOWN;
	  } /* end switch */
	
    case /* length 6 */ 6 :
      switch (lexeme[5]) {
      
	    case 'E' :
	      /* MODULE */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_MODULE])) {
	        return TOKEN_MODULE;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'D' :
	      /* RECORD */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_RECORD])) {
	        return TOKEN_RECORD;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'N' :
	      /* RETURN */
	      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_RETURN])) {
	        return TOKEN_RETURN;
	      }
	      
	      else {
	        return TOKEN_UNKNOWN;
	      } /* end if */
	      
	    case 'T' :
	      switch (lexeme[0]) {
	      
	        case 'E' :
	          /* EXPORT */
	          if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_EXPORT])) {
	            return TOKEN_EXPORT;
	          }
	          
	          else {
	            return TOKEN_UNKNOWN;
	          } /* end if */
	        
	        case 'I' :
	          /* IMPORT */
	          if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_IMPORT])) {
	            return TOKEN_IMPORT;
	          }
	          
	          else {
	            return TOKEN_UNKNOWN;
	          } /* end if */
	        
	        case 'R' :
	          /* REPEAT */
	          if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_REPEAT])) {
	            return TOKEN_REPEAT;
	          }
	          
	          else {
	            return TOKEN_UNKNOWN;
	          } /* end if */
	          
	        default :
	          return TOKEN_UNKNOWN;
	      } /* end switch */
	      
	    default :
	      return TOKEN_UNKNOWN;
	  } /* end switch */
	
    case /* length = 7 */ 7 :
      /* POINTER */
      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_POINTER])) {
	    return TOKEN_POINTER;
      }
      
      else {
	    return TOKEN_UNKNOWN;
      } /* end if */
      
    case /* length = 9 */ 9 :
      switch (lexeme[0]) {
      
        case 'P' :
          /* PROCEDURE */
          if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_PROCEDURE])) {
	        return TOKEN_PROCEDURE;
          }
          
          else {
	        return TOKEN_UNKNOWN;
          } /* end if */
        
        case 'Q' :
          /* QUALIFIED */
          if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_QUALIFIED])) {
	        return TOKEN_QUALIFIED;
          }
          
          else {
	        return TOKEN_UNKNOWN;
          } /* end if */
        
        default :
	      return TOKEN_UNKNOWN;
      } /* end switch */
	
    case /* length = 10 */ 10 :
      /* DEFINITION */
      if (str_match(lexeme, m2c_resword_lexeme_table[TOKEN_DEFINITION])) {
	    return TOKEN_DEFINITION;
      }
      
      else {
	    return TOKEN_UNKNOWN;
      } /* end if */
    
    case /* length = 14 */ 14 :
      /* IMPLEMENTATION */
      if (str_match(lexeme,
          m2c_resword_lexeme_table[TOKEN_IMPLEMENTATION])) {
	    return TOKEN_IMPLEMENTATION;
      }
      
      else {
	    return TOKEN_UNKNOWN;
      } /* end if */
    
    default :
      return TOKEN_UNKNOWN;
  } /* end switch (length) */

} /* end m2c_reword_token_for_lexeme */


/* --------------------------------------------------------------------------
 * function m2c_resword_lexeme_for_token(token)
 * --------------------------------------------------------------------------
 * Tests  if token represents  a reserved word  and returns its corresponding
 * lexeme.  Returns NULL if token does not represent a reserved word.
 * ----------------------------------------------------------------------- */

intstr_t m2c_resword_lexeme_for_token (m2c_token_t token);


#endif M2C_RESWORDS_H

/* END OF FILE */