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
 * m2c-import-parser.c                                                       *
 *                                                                           *
 * Implementation of import section parser for dependency graph.             *
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

#include "m2c-import-parser.h"
#include "m2c-import-lexer.h"


/* --------------------------------------------------------------------------
 * private type parser_context_t
 * ----------------------------------------------------------------------- */

typedef struct {
  char *srcpath;
  m2c_import_lexer_t lexer;
  fifo_t import_list;
  uint_t error_count;
} parser_context_s;

typedef parser_context_s *parser_context_t;


/* --------------------------------------------------------------------------
 * function m2c_parse_imports(srcpath, list, status)
 * --------------------------------------------------------------------------
 * Parses  the import section of the Modula-2 source file located at srcpath,
 * passes a list of identifiers of imported modules back in list  on success,
 * or NULL on failure.  Passes the status of the operation back in status.
 * ----------------------------------------------------------------------- */
 
 void m2c_parse_imports
   (const char *srcpath,            /* in */
    m2c_import_list_t *list,        /* out */
    m2c_parser_status_t *status) {  /* out */

  import_parser_t parser;
  import_lexer_t lexer;
  
  new_import_lexer(lexer, srcpath, lexer_status);

  if ( /*TO DO : check status for errors */ ) {
    *list = NULL;
    /* TO DO : set status */
    return;
  } /* end if */

  parser = malloc(sizeof(import_parser_s));

  if (parser == NULL) {
    *list = NULL;
    *status = M2C_IMPORT_PARSER_STATUS_ALLOCATION_FAILED;
    return;
  } /* end if */
  
  parser->lexer = lexer;
  parser->import_list = NULL;
  parser->error_count = 0;
  
  parse_module_header_and_import(&parser);
  
  /* TO DO : status */

  list = parser->import_list;
  
  return;
} /* end m2c_parse_imports */


/* *********************************************************************** *
 * P R I V A T E   F U N C T I O N S                                       *
 * *********************************************************************** */

/* --------------------------------------------------------------------------
 * private function module_header_and_import(p)
 * --------------------------------------------------------------------------
 * moduleHeaderAndImport :=
 *   defModHdrAndImport | impOrPgmModHdrAndImport
 *   ;
 * ----------------------------------------------------------------------- */

static m2c_token_t def_mod_hdr_and_import (parser_context_t p);
static m2c_token_t imp_or_pgm_mod_hdr_and_import (parser_context_t p);

static m2c_token_t module_header_and_import (parser_context_t p) {
  m2c_token_t lookahead;

  lookahead = m2c_next_sym(p->lexer);
   
  switch (lookahead) {
    case TOKEN_DEFINITION :
      lookahead = def_mod_hdr_and_import(p);
      break;
      
    case TOKEN_IMPLEMENTATION :
    case TOKEN_MODULE:
      lookahead = imp_or_pgm_mod_hdr_and_import(p);
      break;
  } /* end switch */
  
  return lookahead;
} /* end module_header_and_import */


/* --------------------------------------------------------------------------
 * private function def_mod_hdr_and_import(p)
 * --------------------------------------------------------------------------
 * defModHdrAndImport :=
 *   DEFINITION MODULE moduleIdent ';'
 *   import* endOfDefModImport;
 *   ;
 *
 * alias moduleIdent = Ident ;
 *
 * endOfDefModImport := CONST | TYPE | VAR | PROCEDURE | TO | EndOfFile ;
 * ----------------------------------------------------------------------- */

static m2c_token_t import (parser_context_t p);

static m2c_token_t def_mod_hdr_and_import (parser_context_t p) {
  m2c_token_t lookahead;
  
  /* DEFINITION */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* MODULE */
  if (lookahead == TOKEN_MODULE) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* moduleIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IMPORT_OR_DEFINITION_OR_END))) {
      lookahead = m2c_consume_sym(p->lexer);

      module_id = m2c_lexer_current_lexeme(p->lexer);
      
      /* ';' */
      if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_DEFINITION_OR_END))) {
        lookahead = m2c_consume_sym(p->lexer);
      }
      else /* resync */ {
        lookahead = m2c_next_sym(p->lexer);
      } /* end if */
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */
  
  p->import_list = m2c_fifo_new_queue(NULL);

  /* import* */
  while (lookahead == TOKEN_IMPORT) {
    lookahead = import(p->lexer);
  } /* end while */
  
  /* endOfDefModImport */
  if (match_set(FIRST(END_OF_DEF_IMPORT), RESYNC(END_OR_EOF))) {
    m2c_consume_sym(p->lexer);
  } /* end if */
  
  return lookahead;
} /* end def_mod_hdr_and_import */


/* --------------------------------------------------------------------------
 * private function import()
 * --------------------------------------------------------------------------
 * import :=
 *   IMPORT libIdent reExport? ( ',' libIdent reExport? )* ';'
 *   ;
 *
 * alias libIdent = Ident ;
 *
 * alias reExport = '+' ;
 * ----------------------------------------------------------------------- */

static m2c_token_t import (parser_context_t p) {
  m2c_token_t lookahead;
  
  /* IMPORT */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* libIdent */
  if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    library_id = m2c_lexer_current_lexeme(p->lexer);
    m2c_fifo_enqueue(p->import_list, library_id);

    /* '+' */
    if (match_token(TOKEN_PLUS, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */

  /* ( ',' libIdent reExport? )* */
  while (lookahead == TOKEN_COMMA) {
    lookahead = m2c_consume_sym(p->lexer);

    /* libIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
      lookahead = m2c_consume_sym(p->lexer);
      
      library_id = m2c_lexer_current_lexeme();
      m2c_fifo_enqueue(p->import_list, library_id);

      /* '+' */
      if (match_token(TOKEN_PLUS, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
        lookahead = m2c_consume_sym(p->lexer);
      }
      else /* resync */ {
        lookahead = m2c_next_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  } /* end while */

  /* ';' */
  if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_OR_DEFINITION_OR_END))) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */
  
  return lookahead;
} /* end import */


/* --------------------------------------------------------------------------
 * private function imp_or_pgm_mod_hdr_and_import()
 * --------------------------------------------------------------------------
 * impOrPgmModHdrAndImport :=
 *   IMPLEMENTATION? MODULE moduleIdent ';'
 *   privateImport* endOfImpAndPgmModImport;
 *   ;
 *
 * endOfImpAndPgmModImport := BEGIN | endOfDefModImport ;
 * ----------------------------------------------------------------------- */

static m2c_token_t private_import (parser_context_t p);

static m2c_token_t imp_or_pgm_mod_hdr_and_import (parser_context_t p) {
  m2c_token_t lookahead;
  
  lookahead = m2c_lookahead_sym(p->lexer);
  
  /* IMPLEMENTATION? */
  if (lookahead == TOKEN_IMPLEMENTATION) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end if */
  
  /* MODULE */
  if (lookahead == TOKEN_MODULE) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* moduleIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IMPORT_OR_DECLARATION_OR_END))) {
      lookahead = m2c_consume_sym(p->lexer);

      module_id = m2c_lexer_current_lexeme(p->lexer);
      
      /* ';' */
      if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_OR_DECLARATION_OR_END))) {
        lookahead = m2c_consume_sym(p->lexer);
      }
      else /* resync */ {
        lookahead = m2c_next_sym(p->lexer);
      } /* end if */
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */
  
  p->import_list = m2c_fifo_new_queue(NULL);
  
  /* privateImport* */
  while (lookahead == TOKEN_IMPORT) {
    lookahead = private_import(p->lexer);
  } /* end while */
  
  /* endOfImpAndPgmModImport */
  if (match_set(FIRST(END_OF_MOD_IMPORT), RESYNC(END_OR_EOF))) {
    m2c_consume_sym(p->lexer);
  } /* end if */
  
  return lookahead;
} /* end imp_or_pgm_mod_hdr_and_import */


/* --------------------------------------------------------------------------
 * private function private_import()
 * --------------------------------------------------------------------------
 * privateImport :=
 *   IMPORT moduleList ';'
 *   ;
 *
 * alias moduleList = identList;
 *
 * identList :=
 *   Ident ( ',' Ident )*
 *   ;
 * ----------------------------------------------------------------------- */

static m2c_token_t private_import (parser_context_t p) {
  m2c_token_t lookahead;
  
  /* IMPORT */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* moduleList */

  /* Ident */
  if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    library_id = m2c_lexer_current_lexeme(p->lexer);
    m2c_fifo_enqueue(p->import_list, library_id);
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */

  /* ( ',' Ident )* */
  while (lookahead == TOKEN_COMMA) {
    lookahead = m2c_consume_sym(p->lexer);

    /* libIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
      lookahead = m2c_consume_sym(p->lexer);
      
      library_id = m2c_lexer_current_lexeme(p->lexer);
      m2c_fifo_enqueue(p->import_list, library_id);
   }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  } /* end while */

  /* ';' */
  if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_OR_DEFINITION_OR_END))) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */
  
  return lookahead;
} /* end private_import */

/* END OF FILE */