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

#include "m2c-import-parser.h"

static void m2c_parse_file ();
static void parse_start_symbol ();


/* --------------------------------------------------------------------------
 * private function module_header_and_import()
 * --------------------------------------------------------------------------
 * moduleHeaderAndImport :=
 *   defModHdrAndImport | impOrPgmModHdrAndImport
 *   ;
 * ----------------------------------------------------------------------- */

static m2c_token_t module_header_and_import () {
  m2c_token_t lookahead;

  lookahead = m2c_next_sym();
   
  switch (lookahead) {
    case TOKEN_DEFINITION :
      lookahead = def_mod_hdr_and_import();
      break;
      
    case TOKEN_IMPLEMENTATION :
    case TOKEN_MODULE:
      lookahead = imp_or_pgm_mod_hdr_and_import();
      break;
  } /* end switch */
  
  return lookahead;
} /* end module_header_and_import */


/* --------------------------------------------------------------------------
 * private function def_mod_hdr_and_import()
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

static m2c_token_t def_mod_hdr_and_import () {
  m2c_token_t lookahead;
  
  /* DEFINITION */
  lookahead = m2c_consume_sym();
  
  /* MODULE */
  if (lookahead == TOKEN_MODULE) {
    lookahead = m2c_consume_sym();
    
    /* moduleIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IMPORT_OR_DEFINITION_OR_END))) {
      lookahead = m2c_consume_sym();

      module_id = m2c_lexer_current_lexeme();
      
      /* ';' */
      if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_DEFINITION_OR_END))) {
        lookahead = m2c_consume_sym();
      }
      else /* resync */ {
        lookahead = m2c_next_sym();
      } /* end if */
    }
    else /* resync */ {
      lookahead = m2c_next_sym();
    } /* end if */
  }
  else /* resync */ {
    lookahead = m2c_next_sym();
  } /* end if */
  
  import_list = m2c_fifo_new_queue(NULL);

  /* import* */
  while (lookahead == TOKEN_IMPORT) {
    lookahead = import();
    m2c_fifo_enqueue(import_list, imported_id);
  } /* end while */
  
  /* endOfDefModImport */
  if (match_set(FIRST(END_OF_DEF_IMPORT), RESYNC(END_OR_EOF))) {
    m2c_consume_sym();
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

static m2c_token_t import () {
  m2c_token_t lookahead;
  
  /* IMPORT */
  lookahead = m2c_consume_sym();
  
  /* libIdent */
  if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
    lookahead = m2c_consume_sym();
    
    library_id = m2c_lexer_current_lexeme();
    m2c_fifo_enqueue(import_list, library_id);

    /* '+' */
    if (match_token(TOKEN_PLUS, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
      lookahead = m2c_consume_sym();
    }
    else /* resync */ {
      lookahead = m2c_next_sym();
  }
  else /* resync */ {
    lookahead = m2c_next_sym();
  } /* end if */

  /* ( ',' libIdent reExport? )* */
  while (lookahead == TOKEN_COMMA) {
    lookahead = m2c_consume_sym();

    /* libIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
      lookahead = m2c_consume_sym();
      
      library_id = m2c_lexer_current_lexeme();
      m2c_fifo_enqueue(import_list, library_id);

      /* '+' */
      if (match_token(TOKEN_PLUS, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
        lookahead = m2c_consume_sym();
      }
      else /* resync */ {
        lookahead = m2c_next_sym();
    }
    else /* resync */ {
      lookahead = m2c_next_sym();
    } /* end if */
  } /* end while */

  /* ';' */
  if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_OR_DEFINITION_OR_END))) {
    lookahead = m2c_consume_sym();
  }
  else /* resync */ {
    lookahead = m2c_next_sym();
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

static m2c_token_t imp_or_pgm_mod_hdr_and_import () {
  m2c_token_t lookahead;
  
  lookahead = m2c_lookahead_sym();
  
  /* IMPLEMENTATION? */
  if (lookahead == TOKEN_IMPLEMENTATION) {
    lookahead = m2c_consume_sym();
  } /* end if */
  
  /* MODULE */
  if (lookahead == TOKEN_MODULE) {
    lookahead = m2c_consume_sym();
    
    /* moduleIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IMPORT_OR_DECLARATION_OR_END))) {
      lookahead = m2c_consume_sym();

      module_id = m2c_lexer_current_lexeme();
      
      /* ';' */
      if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_OR_DECLARATION_OR_END))) {
        lookahead = m2c_consume_sym();
      }
      else /* resync */ {
        lookahead = m2c_next_sym();
      } /* end if */
    }
    else /* resync */ {
      lookahead = m2c_next_sym();
    } /* end if */
  }
  else /* resync */ {
    lookahead = m2c_next_sym();
  } /* end if */
  
  import_list = m2c_fifo_new_queue(NULL);

  /* privateImport* */
  while (lookahead == TOKEN_IMPORT) {
    lookahead = private_import();
    m2c_fifo_enqueue(import_list, imported_id);
  } /* end while */
  
  /* endOfImpAndPgmModImport */
  if (match_set(FIRST(END_OF_MOD_IMPORT), RESYNC(END_OR_EOF))) {
    m2c_consume_sym();
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

static m2c_token_t private_import () {
  m2c_token_t lookahead;
  
  /* IMPORT */
  lookahead = m2c_consume_sym();
  
  /* moduleList */

  /* Ident */
  if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
    lookahead = m2c_consume_sym();
    
    library_id = m2c_lexer_current_lexeme();
    m2c_fifo_enqueue(import_list, library_id);
  }
  else /* resync */ {
    lookahead = m2c_next_sym();
  } /* end if */

  /* ( ',' Ident )* */
  while (lookahead == TOKEN_COMMA) {
    lookahead = m2c_consume_sym();

    /* libIdent */
    if (match_token(TOKEN_IDENTIFIER, RESYNC(IDENT_OR_COMMA_OR_SEMICOLON))) {
      lookahead = m2c_consume_sym();
      
      library_id = m2c_lexer_current_lexeme();
      m2c_fifo_enqueue(import_list, library_id);
   }
    else /* resync */ {
      lookahead = m2c_next_sym();
    } /* end if */
  } /* end while */

  /* ';' */
  if (match_token(TOKEN_SEMICOLON, RESYNC(IMPORT_OR_DEFINITION_OR_END))) {
    lookahead = m2c_consume_sym();
  }
  else /* resync */ {
    lookahead = m2c_next_sym();
  } /* end if */
  
  return lookahead;
} /* end private_import */


/* --------------------------------------------------------------------------
 * function m2c_parse_imports(srctype, srcpath, list, status)
 * --------------------------------------------------------------------------
 * Parses  the import section of the Modula-2 source file located at srcpath,
 * passes a list of identifiers of imported modules back in list  on success,
 * or NULL on failure.  Passes the status of the operation back in status.
 * ----------------------------------------------------------------------- */
 
 void m2c_parse_imports
   (m2c_sourcetype_t srctype,       /* in */
    const char *srcpath,            /* in */
    m2c_import_list_t *list,        /* out */
    m2c_parser_status_t *status) {  /* out */

  /* TO DO */

} /* end m2c_parse_imports */

/* END OF FILE */