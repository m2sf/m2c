/* M2C Modula-2 Compiler & Translator
 *
 * Copyright (c) 2015-2023 Benjamin Kowarsch
 *
 * @synopsis
 *
 * M2C  is a  Modula-2 to C translator  and  via-C compiler  for the bootstrap
 * subset of the revised Modula-2 language described in
 *
 * https://github.com/m2sf/PDFs/blob/master/M2BSK%20Language%20Description.pdf
 *
 * In compiler mode,  M2C compiles Modula-2 source files via C to object files
 * or executables  using the host system's resident C compiler and linker.  In
 * translator mode, it translates Modula-2 source files to C source files.
 *
 * Further information at https://github.com/m2sf/m2c/wiki
 *
 * @file
 *
 * m2c-parser.c
 *
 * Implementation of M2C parser module.
 *
 * @license
 *
 * M2C is free software:  You can redistribute  and modify it  under the terms
 * of the  GNU Lesser General Public License (LGPL)  either version 2.1  or at
 * your choice version 3, both as published by the Free Software Foundation.
 *
 * M2C  is distributed  in the hope  that it will be useful,  but  WITHOUT ANY
 * WARRANTY;  without even  the implied warranty of MERCHANTABILITY or FITNESS
 * FOR ANY PARTICULAR PURPOSE.  Read the license for more details.
 *
 * You should have  received  a copy of the  GNU Lesser General Public License
 * along with M2C.  If not, see <https://www.gnu.org/copyleft/lesser.html>.
 */

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include "m2c-parser.h"

#include "m2c-lexer.h"
#include "m2c-error.h"
#include "m2c-tokenset.h"
#include "m2c-fileutils.h"
#include "m2c-production.h"
#include "m2c-resync-sets.h"
#include "m2c-compiler-options.h"

#include <stdio.h>
#include <stdlib.h>


#define PARSER_DEBUG_INFO(_str) \
  { if (m2c_option_parser_debug()) \
      printf("*** %s ***\n  @ line: %u, column: %u, lookahead: %s\n", _str, \
        m2c_lexer_lookahead_line(p->lexer), \
        m2c_lexer_lookahead_column(p->lexer), \
        m2c_string_char_ptr(m2c_lexer_lookahead_lexeme(p->lexer))); }


/* --------------------------------------------------------------------------
 * private type m2c_module_context_t
 * --------------------------------------------------------------------------
 * Enumeration type to represent module context.
 * ----------------------------------------------------------------------- */

typedef enum {
  PUBLIC,   /* definition module context */
  PRIVATE   /* implementation and program module context */
} m2c_module_context_t;


/* --------------------------------------------------------------------------
 * private type m2c_parser_context_t
 * --------------------------------------------------------------------------
 * Pointer type to represent parser context.
 * ----------------------------------------------------------------------- */

typedef struct m2c_parser_context_s *m2c_parser_context_t;


/* --------------------------------------------------------------------------
 * private type m2c_parser_context_s
 * --------------------------------------------------------------------------
 * Record type to implement parser context.
 * ----------------------------------------------------------------------- */

struct m2c_parser_context_s {
  /* filename */      const char *filename;
  /* lexer */         m2c_lexer_t lexer;
  /* ast */           m2c_astnode_t ast;
  /* status */        m2c_parser_status_t status;
  /* warning_count */ unsigned short warning_count;
  /* error_count */   unsigned short error_count;
};

typedef struct m2c_parser_context_s m2c_parser_context_s;


/* --------------------------------------------------------------------------
 * function m2c_parse_file(srctype, srcpath, ast, stats, status)
 * --------------------------------------------------------------------------
 * Parses a Modula-2 source file represented by srcpath and returns status.
 * Builds an abstract syntax tree and passes it back in ast, or NULL upon
 * failure.  Collects simple statistics and passes them back in stats.
 * ----------------------------------------------------------------------- */

static void parse_start_symbol
  (m2c_sourcetype_t srctype, m2c_parser_context_t p);
 
void m2c_parse_file
  (m2c_sourcetype_t srctype,
   const char *srcpath,
   m2c_ast_t *ast,
   m2c_stats_t *stats,
   m2c_parser_status_t *status) {
  
  const char *filename;
  m2c_parser_context_t p;
  uint_t line_count;
  
  if ((srctype < M2C_FIRST_SOURCETYPE) || (srctype > M2C_LAST_SOURCETYPE)) {
    SET_STATUS(status, M2C_PARSER_STATUS_INVALID_SOURCETYPE);
    return;
  } /* end if */
  
  if ((srcpath == NULL) || (srcpath[0] == ASCII_NUL)) {
    SET_STATUS(status, M2C_PARSER_STATUS_INVALID_REFERENCE);
    return;
  } /* end if */
  
  /* set up parser context */
  p = malloc(sizeof(m2c_parser_context_s));
  
  if (p == NULL) {
    SET_STATUS(status, M2C_PARSER_STATUS_ALLOCATION_FAILED);
    return;
  } /* end if */
  
  /* create lexer object */
  m2c_new_lexer(&(p->lexer), srcpath, NULL);
  
  if (p->lexer == NULL) {
    SET_STATUS(status, M2C_PARSER_STATUS_ALLOCATION_FAILED);
    free(p);
    return;
  } /* end if */
  
  /* init context */
  p->filename = filename;
  p->ast = NULL;
  p->warning_count = 0;
  p->error_count = 0;
  p->status = 0;
  
  if (m2c_option_variant_records()) {
    /* install function to parse variant records */
    p->record_type = variant_record_type;
  }
  else /* extensible records */ {
    /* install function to parse extensible records */
    p->record_type = extensible_record_type;
  } /* end if */
  
  /* parse and build AST */
  parse_start_symbol(srctype, p);
  line_count = m2c_lexer_lookahead_line(p->lexer);
  
  /* pass back AST, statistics and return status */
  *ast = p->ast;
  *stats = m2c_stats_new(p->warning_count, p->error_count, line_count);
  SET_STATUS(status, p->status);
  
  /* clean up and return */
  m2c_release_lexer(p->lexer, NULL);
  free(p);
  return;
} /* end m2c_parse_file */


/* --------------------------------------------------------------------------
 * private function match_token(p, expected_token, resync_set)
 * --------------------------------------------------------------------------
 * Matches the lookahead symbol to expected_token and returns true if they
 * match.  If they don't match, a syntax error is reported, the error count
 * is incremented, symbols are consumed until the lookahead symbol matches
 * one of the symbols in resync_set and false is returned.
 * ----------------------------------------------------------------------- */

bool match_token
  (m2c_parser_context_t p,
   m2c_token_t expected_token,
   m2c_tokenset_t resync_set) {
  
  m2c_string_t lexeme;
  const char *lexstr;
  uint_t line, column;
  m2c_token_t lookahead;
  
  lookahead = m2c_next_sym(p->lexer);
  
  if (lookahead == expected_token) {
    return true;
  }
  else /* no match */ {
    /* get details for offending lookahead symbol */
    line = m2c_lexer_lookahead_line(p->lexer);
    column = m2c_lexer_lookahead_column(p->lexer);
    lexeme = m2c_lexer_lookahead_lexeme(p->lexer);
    lexstr = m2c_string_char_ptr(lexeme);
    
    /* report error */
    m2c_emit_syntax_error_w_token
      (line, column, lookahead, lexstr, expected_token);
    
    /* print source line */
    if (m2c_option_verbose()) {
      m2c_print_line_and_mark_column(p->lexer, line, column);
    } /* end if */
    
    /* update error count */
    p->error_count++;
    
    /* skip symbols until lookahead matches resync_set */
    while (!m2c_tokenset_element(resync_set, lookahead)) {
      lookahead = m2c_consume_sym(p->lexer);
    } /* end while */
    return false;
  } /* end if */
} /* end match_token */


/* --------------------------------------------------------------------------
 * private function match_set(p, expected_set, resync_set)
 * --------------------------------------------------------------------------
 * Matches the lookahead symbol to set expected_set and returns true if it
 * matches any of the tokens in the set.  If there is no match, a syntax
 * error is reported, the error count is incremented, symbols are consumed
 * until the lookahead symbol matches one of the symbols in resync_set and
 * false is returned.
 * ----------------------------------------------------------------------- */

bool match_set
  (m2c_parser_context_t p,
   m2c_tokenset_t expected_set,
   m2c_tokenset_t resync_set) {
  
  m2c_string_t lexeme;
  const char *lexstr;
  uint_t line, column;
  m2c_token_t lookahead;
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* check if lookahead matches any token in expected_set */
  if (m2c_tokenset_element(expected_set, lookahead)) {
    return true;
  }
  else /* no match */ {
    /* get details for offending lookahead symbol */
    line = m2c_lexer_lookahead_line(p->lexer);
    column = m2c_lexer_lookahead_column(p->lexer);
    lexeme = m2c_lexer_lookahead_lexeme(p->lexer);
    lexstr = m2c_string_char_ptr(lexeme);
    
    /* report error */
    m2c_emit_syntax_error_w_set
      (line, column, lookahead, lexstr, expected_set);
    
    /* print source line */
    if (m2c_option_verbose()) {
      m2c_print_line_and_mark_column(p->lexer, line, column);
    } /* end if */
        
    /* update error count */
    p->error_count++;
    
    /* skip symbols until lookahead matches resync_set */
    while (!m2c_tokenset_element(resync_set, lookahead)) {
      lookahead = m2c_consume_sym(p->lexer);
    } /* end while */
    return false;
  } /* end if */
} /* end match_set */


/* --------------------------------------------------------------------------
 * procedure report_error_w_offending_lexeme(error, lexeme, line, column)
 * ----------------------------------------------------------------------- */

static void report_error_w_offending_lexeme
  (m2c_error_t error,
   m2c_parser_context_t p,
   m2c_string_t lexeme, uint_t line, uint_t column) {
    
  m2c_emit_error_w_lex(error, line, column, m2c_string_char_ptr(lexeme));
  
  if (m2c_option_verbose()) {
    m2c_print_line_and_mark_column(p->lexer, line, column);
  } /* end if */
  
  p->error_count++;
  return;
} /* end report_error_w_offending_lexeme */


/* ************************************************************************ *
 * Syntax Analysis                                                          *
 * ************************************************************************ */

/* --------------------------------------------------------------------------
 * private procedure parse_start_symbol(srctype, p)
 * ----------------------------------------------------------------------- */

static void parse_start_symbol
  (m2c_sourcetype_t srctype, m2c_parser_context_t p) {
  
  m2c_astnode_t id, opt;
  m2c_string_t ident;
  m2c_token_t lookahead;
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (srctype) {
    M2C_ANY_SOURCE :
      if ((lookahead == TOKEN_DEFINITION) ||
          (lookahead == TOKEN_IMPLEMENTATION) ||
          (lookahead == TOKEN_MODULE)) {
        lookahead = compilation_unit(p);
      }
      else /* invalid start symbol */ {
        /* TO DO: report error */
        p->status = M2C_PARSER_STATUS_INVALID_START_SYMBOL;
      } /* end if */
      break;
    
    M2C_DEF_SOURCE :
      if (lookahead == TOKEN_DEFINITION) {
        lookahead = definition_module(p);
      }
      else /* invalid start symbol */ {
        /* TO DO: report error */
        p->status = M2C_PARSER_STATUS_INVALID_START_SYMBOL;
      } /* end if */
      break;
    
    M2C_MOD_SOURCE :
      if (lookahead == TOKEN_IMPLEMENTATION) {
        lookahead = implementation_module(p);
      }
      else if (lookahead == TOKEN_MODULE) {
        lookahead = program_module(p);
      }
      else /* invalid start symbol */ {
        /* TO DO: report error */
        p->status = M2C_PARSER_STATUS_INVALID_START_SYMBOL;
      } /* end if */
      break;
  } /* end switch */
  
  /* build AST node and pass it back in p->ast */
  ident = m2c_get_string(p->filename);
  id = m2c_ast_new_terminal_node(AST_IDENT, ident);
  opt = m2c_ast_empty_node(); /* TO DO : encode options */
  p->ast = m2c_ast_new_node(AST_ROOT, id, opt, p->ast, NULL);
  
  if (lookahead != TOKEN_EOF) {
    /* TO DO: report error -- extra symbols after end of compilation unit */
  } /* end if */
} /* end parse_start_symbol */


/* --------------------------------------------------------------------------
 * private function compilation_unit()
 * --------------------------------------------------------------------------
 * compilationUnit :=
 *   definitionModule | implementationModule | programModule
 *   ;
 *
 * astnode: defModuleNode | impModuleNode
 * ----------------------------------------------------------------------- */

m2c_token_t compilation_unit (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("compilationUnit");
  
  m2c_token_t lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
    case TOKEN_DEFINITION :
      lookahead = definition_module(p);
      break;
      
    case TOKEN_IMPLEMENTATION :
      lookahead = implementation_module(p);
      break;
      
    case TOKEN_MODULE:
      lookahead = program_module(p);
      break;
  } /* end switch */
  
  return lookahead;
} /* end compilation_unit */


/* ************************************************************************ *
 * Definition Module Syntax                                                 *
 * ************************************************************************ */

/* --------------------------------------------------------------------------
 * alias const_expression()
 * ----------------------------------------------------------------------- */

#define const_expression(_p) expression(_p)


/* --------------------------------------------------------------------------
 * private function definition_module()
 * --------------------------------------------------------------------------
 * definitionModule :=
 *   DEFINITION MODULE moduleIdent ';'
 *   import* definition* END moduleIdent '.'
 *   ;
 *
 * moduleIdent := Ident ;
 *
 * astnode: (DEFMOD identNode implist deflist)
 * ----------------------------------------------------------------------- */

m2c_token_t import (m2c_parser_context_t p);
m2c_token_t definition (m2c_parser_context_t p);

m2c_token_t definition_module (m2c_parser_context_t p) {
  intstr_t ident1, ident2;
  m2c_token_t lookahead;
  m2c_fifo_t imp_list, def_list;
  m2c_astnode_t id_node, imp_node, def_node;
  
  PARSER_DEBUG_INFO("definitionModule");
  
  /* DEFINITION */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* MODULE */
  if (match_token(p, TOKEN_MODULE, RESYNC(IMPORT_OR_DEFINITON_OR_END))) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token(p, TOKEN_IDENT, TOKEN_SEMICOLON);
  } /* end if */
  
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = m2c_consume_sym(p->lexer);
    ident1 = m2c_lexer_current_lexeme(p->lexer);
  }
  else /* resync */ {
    lookahead = skipt_to_token_or_set(p, TOKEN_SEMICOLON, FIRST(IMPORT));
  } /* end if */
  
  /* ';' */
  if (match_token(p, TOKEN_SEMICOLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skipt_token_or_set(p, TOKEN_IMPORT, FOLLOW(IMPORT));
  } /* end if */
  
  imp_list = m2c_fifo_new_queue(NULL);
  
  /* import* */
  while (lookahead == TOKEN_IMPORT) {
    lookahead = import(p);
    m2c_fifo_enqueue(imp_list, p->ast);
  } /* end while */
  
  def_list = m2c_fifo_new_queue(NULL);
  
  /* definition* */
  while (m2c_tokenset_element(FIRST(DEFINITION), lookahead)) {
    lookahead = definition(p);
    m2c_fifo_enqueue(def_list, p->ast);
  } /* end while */
  
  /* END */
  if (match_token(p, TOKEN_END)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    skip_to_token(p, TOKEN_IDENT);
  } /* end if */
  
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = m2c_consume_sym(p->lexer);
    ident1 = m2c_lexer_current_lexeme(p->lexer);
    
    if (ident1 != ident2) {
      /* TO DO: error -- module identifiers don't match */
    } /* end if */
  }
  else /* resync */ {
    skip_to_token(p, TOKEN_DOT);
  } /* end if */
  
  /* '.' */
  if (match_token(p, TOKEN_DOT)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    skip_to_token(p, TOKEN_EOF);
  } /* end if */
    
  /* build AST node and pass it back in p->ast */
  id_node = m2c_ast_new_terminal_node(AST_IDENT, ident1);
  imp_node = m2c_ast_new_term_list_node(AST_IMPORT, imp_list);
  def_node = m2c_ast_new_term_list_node(AST_IMPORT, def_list);
  
  p->ast = m2c_ast_new_node(AST_DEFMOD, id_node, imp_node, def_node, NULL);
  
  m2c_fifo_release(imp_list);
  m2c_fifo_release(def_list);
  
  return lookahead;
} /* end definition_module */


/* --------------------------------------------------------------------------
 * private function import()
 * --------------------------------------------------------------------------
 * import :=
 *   IMPORT libIdent reExport? ( ',' libIdent reExport? )* ';'
 *   ;
 *
 * alias libIdent = Ident ;
 * alias reExport = '+' ;
 * ----------------------------------------------------------------------- */

m2c_token_t ident (m2c_parser_context_t p);

m2c_token_t import (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_node_t lib_ident;
  m2c_fifo_t imp_list, rxp_list;
  
  PARSER_DEBUG_INFO("import");
  
  /* IMPORT */
  lookahead = m2c_consume_sym(p->lexer);
  
  imp_list = m2c_fifo_new_queue(NULL);
  rxp_list = m2c_fifo_new_queue(NULL);
  
  /* libIdent reExport? */
  if (match_token(p, TOKEN_IDENT) {
    
    /* libIdent */
    lookahead = ident(p);
    lib_ident = p->ast;
        
    /* reExport? */
    if (lookahead == TOKEN_PLUS) {
      lookahead = m2c_consume_sym(p->lexer);
      
      /* add ident to re-export list */
      rxp_list = m2c_fifo_enqueue(rxp_list, lib_ident);
    }
    else {
      /* add ident to import list */
      imp_list = m2c_fifo_enqueue(imp_list, lib_ident);
    } /* end if */
  }
  else /* resync */ {
    lookahead =
      skip_to_token(p, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_IMPORT, NULL);
  } /* end if */
    
  /* (',' libIdent reExport? )* */
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    if (match_token(p, TOKEN_IDENT) {
    
      /* libIdent */
      lookahead = ident(p);
      lib_ident = p->ast;
      
      /* reExport? */
      if (lookahead == TOKEN_PLUS) {
        lookahead = m2c_consume_sym(p->lexer);
      
        /* add ident to re-export list */
        rxp_list = m2c_fifo_enqueue(rxp_list, lib_ident);
      }
      else {
        /* add ident to import list */
        imp_list = m2c_fifo_enqueue(imp_list, lib_ident);
      } /* end if */
    }
    else /* resync */ {
      lookahead =
        skip_to_token(p, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_IMPORT, NULL);
    } /* end if */
  } /* end while */
  
  /* ';' */
  if (match_token(p, TOKEN_SEMICOLON) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */
    lookahead = skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(IMPORT));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IMPORT, imp_list, rxp_list);
  
  m2c_fifo_release_queue(imp_list);
  m2c_fifo_release_queue(rxp_list);
  
  return lookahead;
} /* end import */


/* --------------------------------------------------------------------------
 * private function definition()
 * --------------------------------------------------------------------------
 * definition :=
 *   CONST ( constDefinition ';' )+ |
 *   TYPE ( typeDefinition ';' )+ |
 *   VAR ( varDefinition ';' )+ |
 *   procedureHeader ';' |
 *   toDoList ';'
 *   ;
 *
 * varDefinition := identList ':' typeIdent ;
 * ----------------------------------------------------------------------- */

m2c_token_t const_definition_list (m2c_parser_context_t p);
m2c_token_t type_definition_list (m2c_parser_context_t p);
m2c_token_t var_definition_list (m2c_parser_context_t p);
m2c_token_t procedure_header (m2c_parser_context_t p);
m2c_token_t to_do_list (m2c_parser_context_t p);

m2c_token_t definition (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("definition");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
    
    /* CONST */
    case TOKEN_CONST :
      /* (constDefinition ';')+ */
      lookahead = const_definition_list(p);
      break;
      
    /* | TYPE */
    case TOKEN_TYPE :
      /* (typeDefinition ';')+ */
      lookahead = type_definition_list(p);
      break;
      
    /* | VAR */
    case TOKEN_VAR :
      /* (varDefinition ';')+ */
      lookahead = var_definition_list(p);
      break;
      
    /* | procedureHeader */
    case TOKEN_PROCEDURE :
      lookahead = procedure_header(p); /* p->ast holds ast-node */
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON)) {
        lookahead = m2c_consume_sym(p->lexer);
      else /* resync */ {
        lookahead = skip_to_set(p, FOLLOW(DEFINITION));
      } /* end if */
      break;
      
    /* | toDoList */
    case TOKEN_TO :
      lookahead = to_do_list(p); /* p->ast holds ast-node */
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON)) {
        lookahead = m2c_consume_sym(p->lexer);
      else /* resync */ {
        lookahead = skip_to_set(p, FOLLOW(DEFINITION));
      } /* end if */
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
  } /* end switch */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end definition */


/* --------------------------------------------------------------------------
 * private function const_definition_list()
 * --------------------------------------------------------------------------
 * constDefinitionList :=
 *   constDefinition ';' (constDefinition ';')*
 *   ;
 *
 * astnode: (CONSTDEFLIST constDefNode1 constDefNode2 ... constDefNodeN)
 * ----------------------------------------------------------------------- */

m2c_token_t const_declaration (m2c_parser_context_t p);

m2c_token_t const_definition_list (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t def_list;
  
  PARSER_DEBUG_INFO("constDefinitionList");

  def_list = m2c_fifo_new_queue(NULL);
  
  /* constDefinition ';' */
  
  /* constDefinition */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = const_definition(p); /* p->ast holds ast-node */
    def_list = m2c_fifo_enqueue(def_list, p->ast);
      
    /* ';' */
    if (match_token(p, TOKEN_SEMICOLON)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(CONST_DEFINITION));
    } /* end if */
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(CONST_DEFINITION));
  } /* end if */
  
  /* (constDefinition ';')* */
  
  /* constDefinition */
  while (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = const_definition(p); /* p-ast holds ast-node */
    def_list = m2c_fifo_enqueue(def_list, p->ast);
    
    /* ';' */
    if (match_token == TOKEN_SEMICOLON) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(CONST_DEFINITION));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_CONSTDEFLIST, def_list);
  
  m2c_fifo_releast(def_list);
  
  return lookahead;
} /* end const_definition_list */


/* --------------------------------------------------------------------------
 * private function constDefinition()
 * --------------------------------------------------------------------------
 * constDefinition :=
 *   constBinding | constDeclaration
 *   ;
 *
 * astNode: (BIND ident expr) | (CONST constId typeId expr)
 * ----------------------------------------------------------------------- */

m2c_token_t const_binding (m2c_parser_context_t p);
m2c_token_t const_declaration (m2c_parser_context_t p);

m2c_token_t const_definition (m2c_parser_context_t p) {
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("constDefinition");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* constBinding | */
  if (lookahead == TOKEN_LBRACKET) {
    lookahead = const_binding(p);
  }
  /* constDeclaration */
  else /* lookahead is identifier */
    lookahead = const_declaration(p);
  } /* end if */
  
  return lookahead;
} /* end const_definition */


/* --------------------------------------------------------------------------
 * private function const_binding()
 * --------------------------------------------------------------------------
 * constBinding :=
 *   '[' ( StdIdent=COLLATION | StdIdent=TLIMIT ) ']' '=' constExpression
 *   ;
 *
 * astNode: (BIND ident expr)
 * ----------------------------------------------------------------------- */

m2c_token_t ident (m2c_parser_context_t p);
m2c_token_t const_expression (m2c_parser_context_t p);

m2c_token_t const_binding (m2c_parser_context_t p) {
  intstr_t ident;
  m2c_token_t lookahead;
  m2c_ast_node_t id_node, expr_node;
    
  PARSER_DEBUG_INFO("constBinding");
  
  /* '[' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* COLLATION | TLIMIT */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = m2c_consume_sym(p->lexer);
    ident = m2c_current_lexeme(p->lexer);
    
    if ((ident == m2c_res_ident(RESIDENT_COLLATION))
      || (ident == m2c_res_ident(RESIDENT_TLIMIT)))
      lookahead = ident(p);
      id_node = p->ast;
    }
    else {
      lookahead = m2c_consume_sym(p);
      id_node = m2c_ast_empty_node();
      
      /* TO DO: error -- invalid binding specifier */
    } /* end if */
  }
  else /* resync */
    lookahead =
      skip_to_token_or_set(p, TOKEN_RBRACKET, FIRST(CONST_EXPRESSION));
  } /* end if */
  
  /* '=' */
  if (match_token(p, TOKEN_EQUAL)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead =
      skip_to_token_or_set(p, TOKEN_RBRACKET, FIRST(CONST_EXPRESSION));
  } /* end if */
  
  /* ']' */
  if (match_token(p, TOKEN_RBRACKET)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead =
      skip_to_set_or_set(p, FIRST(CONST_EXPRESSION), FOLLOW(CONST_BINDING));
  } /* end if */
  
  /* constExpression */
  if (match_set(FIRST(CONST_EXPRESSION)) {
    lookahead = const_expression(p);
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(CONST_BINDING));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = new_ast_new_node(AST_BIND, id_node, expr_node);
  
  return lookahead;
} /* end const_binding */


/* --------------------------------------------------------------------------
 * private function const_declaration()
 * --------------------------------------------------------------------------
 * constDeclaration :=
 *   ident ( ':' typeIdent )? '=' constExpression
 *   ;
 *
 * alias constExpression = expression ;
 *
 * astNode: (CONST constId typeId expr)
 * ----------------------------------------------------------------------- */

m2c_token_t const_declaration (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_node_t const_id, type_id, expr;
  
  PARSER_DEBUG_INFO("constDeclaration");
  
  /* ident */
  lookahead = ident(p);
  const_id = p->ast;
  
  /* (':' typeIdent)? */
  if (lookahead == TOKEN_COLON) {
    /* ':' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* typeIdent */
    if (match_token(p, TOKEN_IDENT)) {
      lookahead = ident(p);
      type_id = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_EQUAL, FIRST(EXPRESSION));
      type_id = NULL;
    } /* end if */
  } /* end if */
  
  /* '=' */
  if (match_token(p, TOKEN_EQUAL)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead =
      skip_to_set_or_set(p, FIRST(EXPRESSION), FOLLOW(CONST_DECLARATION));
  } /* end if */
  
  /* constExpression */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = const_expression(p);
    expr = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(CONST_DECLARATION));
    expr = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_CONSTDECL, const_id, type_id, expr);
  
  return lookahead;
} /* end const_declaration */


/* --------------------------------------------------------------------------
 * private function ident()
 * --------------------------------------------------------------------------
 * alias ident = StdIdent ;
 *
 * astNode: (IDENT "lexeme")
 * ----------------------------------------------------------------------- */

m2c_token_t ident (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("ident");
  
  lookahead = m2c_consume_sym(p->lexer);
  lexeme = m2c_current_lexeme(p->lexer);
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(IDENT, lexeme);
  
  return lookahead;
} /* end ident */


/* --------------------------------------------------------------------------
 * private function type_definition_list()
 * --------------------------------------------------------------------------
 * typeDefinitionList :=
 *   typeDefinition ';' (typeDefinition ';')*
 *   ;
 *
 * astnode: (TYPEDEFLIST typeDefNode1 typeDefNode2 ... typeDefNodeN)
 * ----------------------------------------------------------------------- */

m2c_token_t type_definition (m2c_parser_context_t p);

m2c_token_t type_definition_list (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t def_list;
  
  PARSER_DEBUG_INFO("typeDefinitionList");

  def_list = m2c_fifo_new_queue(NULL);
  
  /* typeDefinition ';' */
  
  /* typeDefinition */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = type_definition(p); /* p->ast holds ast-node */
    def_list = m2c_fifo_enqueue(def_list, p->ast);
      
    /* ';' */
    if (match_token(p, TOKEN_SEMICOLON)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(TYPE_DEFINITION));
    } /* end if */
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(TYPE_DEFINITION));
  } /* end if */
  
  /* (typeDefinition ';')* */
  
  /* typeDefinition */
  while (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = type_definition(p); /* p-ast holds ast-node */
    def_list = m2c_fifo_enqueue(def_list, p->ast);
    
    /* ';' */
    if (match_token == TOKEN_SEMICOLON) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(TYPE_DEFINITION));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_TYPEDEFLIST, def_list);
  
  m2c_fifo_release(def_list);
  
  return lookahead;
} /* end type_definition_list */


/* --------------------------------------------------------------------------
 * private function type_definition()
 * --------------------------------------------------------------------------
 * typeDefinition :=
 *   ident '=' type
 *   ;
 *
 * astNode: (TYPE identNode typeNode)
 * ----------------------------------------------------------------------- */

#define public_type(_p) type(PUBLIC, _p)

m2c_token_t type (m2c_parser_context_t p);

m2c_token_t type_definition (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t ident_node, type_node;
    
  PARSER_DEBUG_INFO("typeDefinition");
  
  /* Ident */
  lookahead = ident(p);
  ident_node = p->ast;
  
  /* '=' */
  if (match_token(p, TOKEN_EQUAL)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(TYPE));
  } /* end if */
  
  /* type */
  if (match_set(p, FIRST(TYPE)) {
    lookahead = public_type(p);
    type_node = p->ast;
  }
  else /* resync */ {
    lokahead = skip_to_set(p, FOLLOW(TYPE_DEFINITION));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_TYPEDEF, ident_node, type_node);
  
  return lookahead;
} /* end type_definition */


/* --------------------------------------------------------------------------
 * private function type()
 * --------------------------------------------------------------------------
 * Parses rule type or privateType depending on moduleContext, constructs its
 * AST node, passes it back in p->ast and returns the new lookahead symbol.
 *
 * type :=
 *   aliasType | derivedType | subrangeType | enumType | setType |
 *   arrayType | recordType | pointerType | opaqueType | procedureType )
 *   ;
 *
 * privateType :=
 *   aliasType | derivedType | subrangeType | enumType | setType |
 *   arrayType | recordType | octetSeqType | privatePointerType |
 *   procedureType )
 *   ;
 *
 * astNode: aliasTypeNode | derivedTypeNode | subrangeTypeNode |
 *   enumTypeNode | setTypeNode | arrayTypeNode | recordTypeNode |
 *   pointerTypeNode | opaqueTypeNode | octetSeqTypeNode |
 *   indeterminateTypeNode | procedureTypeNode
 * ----------------------------------------------------------------------- */

m2c_token_t derived_type (m2c_parser_context_t p);
m2c_token_t subrange_type (m2c_parser_context_t p);
m2c_token_t enum_type (m2c_parser_context_t p);
m2c_token_t set_type (m2c_parser_context_t p);
m2c_token_t array_type (m2c_parser_context_t p);
m2c_token_t pointer_type (m2c_parser_context_t p);
m2c_token_t private_pointer_type (m2c_parser_context_t p);
m2c_token_t procedure_type (m2c_parser_context_t p);

m2c_token_t type (module_context_t module_context, m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("type");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
  
    /* aliasType */
    case TOKEN_ALIAS :
      lookahead = alias_type(p); /* p->ast holds ast-node */
      break;
      
    /* | arrayType */
    case TOKEN_ARRAY :
      lookahead = array_type(p); /* p->ast holds ast-node */
      break;
      
    /* | octetseqType */
    case TOKEN_OCTETSEQ :
      if (module_context == PRIVATE) {
        lookahead = octetseq_type(p); /* p->ast holds ast-node */
      }
      else {
        /* fatal error -- we should never get here */
        exit (-1)
      } /* end if */
      break;
      
    /* | opaqueType */
    case TOKEN_OPAQUE :
      if (module_context == PUBLIC) {
        lookahead = opaque_type(p); /* p->ast holds ast-node */
      }
      else {
        /* fatal error -- we should never get here */
        exit (-1)
      } /* end if */
      break;
    
    /* | pointerType | privatePointerType */
    case TOKEN_POINTER :
      if (module_context == PUBLIC) {
        lookahead = pointer_type(p); /* p->ast holds ast-node */
      }
      else /* PRIVATE */ {
        lookahead = private_pointer_type(p); /* p->ast holds ast-node */
      } /* end if */
      break;
      
    /* | procedureType */
    case TOKEN_PROCEDURE :
      lookahead = procedure_type(p); /* p->ast holds ast-node */
      break;
      
    /* | recordType */
    case TOKEN_RECORD :
      lookahead = p->record_type(p); /* p->ast holds ast-node */
      break;
      
    /* | setType */
    case TOKEN_SET :
      lookahead = set_type(p); /* p->ast holds ast-node */
      break;
      
    /* | derivedType */
    case TOKEN_IDENTIFIER :
      lookahead = derived_type(p); /* p->ast holds ast-node */
      break;
    
    /* | enumType */
    case TOKEN_LEFT_PAREN :
      lookahead = enum_type(p); /* p->ast holds ast-node */
      break;
      
    /* | subrangeType */  
    case TOKEN_LEFT_BRACKET :
      lookahead = subrange_type(p); /* p->ast holds ast-node */
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
    } /* end switch */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end type */


/* --------------------------------------------------------------------------
 * private function alias_type()
 * --------------------------------------------------------------------------
 * aliasType :=
 *   ALIAS OF typeIdent
 *   ;
 *
 * alias typeIdent = qualident ;
 *
 * astNode: (ALIAS baseType)
 * ----------------------------------------------------------------------- */

m2c_token_t qualident (m2c_parser_context_t p);

m2c_token_t alias_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_node_t type_node;
  
  PARSER_DEBUG_INFO("type");
  
  /* ALIAS */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* OF */
  if (match_token(p, TOKEN_OF)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token(p, TOKEN_IDENT);
  } /* end if */
  
  /* typeIdent */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = qualident(p);
    type_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(TYPE_DEFINITION));
  } /* end if */
  
  /* build AST node and pass back in p->ast */
  p->ast = m2c_ast_new_node(AST_ALIAS, type_node);
  
  return lookahead;
} /* end alias_type */


/* --------------------------------------------------------------------------
 * private function qualident()
 * --------------------------------------------------------------------------
 * qualident :=
 *   Ident ( '.' Ident )*
 *   ;
 *
 * astnode: (IDENT ident) | (QUALIDENT q0 q1 q2 ... qN ident)
 * ----------------------------------------------------------------------- */

m2c_token_t qualident (m2c_parser_context_t p) {
  m2c_string_t first_ident, tail_ident;
  m2c_fifo_t lex_list;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("qualident");
  
  /* Ident */
  lookahead = m2c_consume_sym(p->lexer);
  first_ident = m2c_lexer_current_lexeme(p->lexer);
  
  /* add ident to temporary list */
  lex_list = m2c_fifo_new_queue(first_ident);
  
  /* ( '.' Ident )* */
  while (lookahead == TOKEN_PERIOD) {
    /* '.' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* Ident */
    if (match_token(p, TOKEN_IDENTIFIER, FOLLOW(QUALIDENT))) {
      lookahead = m2c_consume_sym(p->lexer);
      tail_ident = m2c_lexer_current_lexeme(p->lexer);
      m2c_fifo_enqueue(lex_list, tail_ident);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(QUALIDENT));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  if (m2c_fifo_entry_count(tmplist) == 1) {
    p->ast = m2c_ast_new_terminal_node(AST_IDENT, first_ident, NULL);
  }
  else {
    p->ast = m2c_ast_new_terminal_list_node(AST_QUALIDENT, lex_list);
  } /* end if */
  
  m2c_fifo_release(lex_list);
  
  return lookahead;
} /* end qualident */


/* --------------------------------------------------------------------------
 * private function subrange_type()
 * --------------------------------------------------------------------------
 * subrangeType :=
 *   constRange OF countableType
 *   ;
 *
 * constRange :=
 *   '[' lowerBound '..' upperBound ']'
 *   ;
 *
 * alias countableType = typeIdent ;
 *
 * alias upperBound, lowerBound = constExpression ;
 *
 * astNode: (SUBR baseType lowerBound upperBound)
 * ----------------------------------------------------------------------- */

m2c_token_t subrange_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node, lower_bound, upper_bound;
  
  PARSER_DEBUG_INFO("subrangeType");
  
  /* '[' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* lowerBound */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    lower_bound = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_DOT_DOT, FIRST(EXPRESSION));
    lower_bound = m2c_ast_empty_node();
  } /* end if */
  
  /* '..' */
  if (match_token(p, TOKEN_DOT_DOT)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(EXPRESSION));
  } /* end if */
  
  /* upperBound */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    upper_bound = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token(p, TOKEN_RBRACKET, TOKEN_OF, NULL);
    upper_bound = m2c_ast_empty_node();
  } /* end if */
  
  /* ']' */
  if (match_token(p, TOKEN_RBRACKET)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token(p, TOKEN_OF, TOKEN_IDENT, NULL);
  } /* end if */
  
  /* OF */
  if (match_token(p, TOKEN_OF)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(SUBRANGE_TYPE));
  } /* end if */
  
  /* countableType */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = qualident(p);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(SUBRANGE_TYPE));
  } /* end if */
    
  /* build AST node and pass back in p->ast */
  p->ast = m2c_ast_new_node(AST_SUBR, type_node, lower_bound, upper_bound);
  
  return lookahead;
} /* end subrange_type */


/* --------------------------------------------------------------------------
 * private function enum_type()
 * --------------------------------------------------------------------------
 * enumType :=
 *   '(' ( '+' enumTypeToExtend ',' )? identList ')'
 *   ;
 *
 * alias enumTypeToExtend = enumTypeIdent ;
 *
 * alias enumTypeIdent = typeIdent ;
 *
 * astNode: (ENUM baseType enumValues)
 * ----------------------------------------------------------------------- */

m2c_token_t ident_list (m2c_parser_context_t p);

m2c_token_t enum_type (m2c_parser_context_t p) {
  m2c_ast_node_t type_node, list_node;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("enumType");
  
  /* '(' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ('+' enumTypeToExtend ',')? */
  if (lookahead == TOKEN_PLUS) {
    /* '+' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* enumTypeToExtend */
    if (match_token(p, TOKEN_IDENT)) {
      lookahead = qualident(p);
      type_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token(p, TOKEN_COMMA, TOKEN_IDENT, NULL);
      type_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ',' */
    if (match_token(p, TOKEN_COMMA)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_token(p, TOKEN_IDENT);
    } /* end if */
  }
  else {
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* identList */
  if (match(p, TOKEN_IDENT)) {
    lookahead = ident_list(p);
    list_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_RPAREN, FOLLOW(ENUM_TYPE));
  } /* end if */
  
  /* ')' */
  if (match_token(p, TOKEN_RPAREN)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(ENUM_TYPE));
  } /* end if */
    
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_ENUM, type_node, list_node, NULL);
  
  return lookahead;
} /* end enum_type */


/* --------------------------------------------------------------------------
 * private function ident_list()
 * --------------------------------------------------------------------------
 * identList :=
 *   Ident ( ',' Ident )*
 *   ;
 *
 * astnode: (IDENTLIST ident0 ident1 ident2 ... identN)
 * ----------------------------------------------------------------------- */

m2c_token_t ident_list (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_fifo_t tmp_list;
  uint_t line, column;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("identList");
  
  /* Ident */
  lookahead = m2c_consume_sym(p->lexer);
  lexeme = m2c_lexer_current_lexeme(p->lexer);
  
  /* add ident to temporary list */
  tmp_list = m2c_fifo_new_queue(lexeme);
  
  /* ( ',' Ident )* */
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* Ident */
    if (match_token(p, TOKEN_IDENTIFIER)) {
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_lexer_current_lexeme(p->lexer);
      
      /* check for duplicate identifier */
      if (m2c_fifo_entry_exists(tmp_list, lexeme)) {
        line = m2c_lexer_current_line(p->lexer);
        column = m2c_lexer_current_column(p->lexer);
        report_error_w_offending_lexeme
          (M2C_ERROR_DUPLICATE_IDENT_IN_IDENT_LIST, p,
           lexeme, line, column);
      }
      else /* not a duplicate */ {
        /* add ident to temporary list */
        m2c_fifo_enqueue(tmp_list, lexeme);
      } /* end if */
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_COMMA, FOLLOW(IDENT_LIST));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IDENTLIST, tmp_list);
  
  m2c_fifo_release_queue(tmp_list);
  
  return lookahead;
} /* end ident_list */


/* --------------------------------------------------------------------------
 * private function set_type()
 * --------------------------------------------------------------------------
 * setType :=
 *   SET OF enumTypeIdent
 *   ;
 *
 * alias enumTypeIdent = typeIdent ;
 *
 * astNode: (SET baseType)
 * ----------------------------------------------------------------------- */

m2c_token_t set_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_node_t type_node;
  
  PARSER_DEBUG_INFO("setType");
  
  /* SET */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* OF */
  if (match_token(p, TOKEN_OF)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(SET_TYPE));
  } /* end if */
  
  /* enumTypeIdent */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = qualident(p);
    type_node = p->ast;  
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(SET_TYPE));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_SET, type_node);
  
  return lookahead;
} /* end set_type */


/* --------------------------------------------------------------------------
 * private function array_type()
 * --------------------------------------------------------------------------
 * arrayType :=
 *   ARRAY valueCount OF typeIdent
 *   ;
 *
 * alias valueCount = constExpression ;
 *
 * astNode: (ARRAY baseType valueCount )
 * ----------------------------------------------------------------------- */

m2c_token_t array_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node, value_node;
  
  PARSER_DEBUG_INFO("arrayType");
  
  /* ARRAY */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* valueCount */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    value_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token(p, TOKEN_OF, TOKEN_IDENT, NULL);
    value_node = m2c_ast_empty_node();
  } /* end if */
  
  /* OF */
  if (match_token(p, TOKEN_OF)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(ARRAY_TYPE));
  } /* end if */
  
  /* typeIdent */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = qualident(p);
    type_node = p->ast;  
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(ARRAY_TYPE));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_ARRAY, type_node, value_node, NULL);
    
  return lookahead;
} /* end array_type */


/* --------------------------------------------------------------------------
 * private function record_type()
 * --------------------------------------------------------------------------
 * recordType :=
 *   RECORD ( '(' recTypeToExtend ')' )?
 *   fieldList ( ';' fieldList )* END
 *   ;
 *
 * recTypeToExtend = typeIdent | NIL ;
 *
 * alias fieldList = varOrFieldDeclaration ;
 *
 * astNode: (RECORD baseType fieldListSeq)
 * ----------------------------------------------------------------------- */

m2c_token_t field_list_sequence (m2c_parser_context_t p);

m2c_token_t record_type (m2c_parser_context_t p) {
  m2c_astnode_t type_node, list_node;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("recordType");
  
  /* RECORD */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ( '(' recTypeToExtend ')' )? */
  if (lookahead == TOKEN_LPAREN) {
    /* '(' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* typeIdent | NIL */
    if (match_token(p, TOKEN_IDENT))
      lookahead = qualident(p);
      type_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_RPAREN, FIRST(FIELD_LIST));
      type_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ')' */
    if (match_token(p, TOKEN_RPAREN)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FIRST(FIELD_LIST));
    } /* end if */
  }
  else /* non-extensible record */ {
    type_node = m2c_ast_empty_node();
  } /* end if */
    
  /* fieldList (';' fieldList)* */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = field_list_sequence(p);
    list_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_or_set(p, TOKEN_END, FOLLOW(FIELD_LIST_SEQUENCE));
    list_node = m2c_ast_empty_node();
  } /* end if */
  
  /* END */
  if (match_token(p, TOKEN_END)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(FIELD_LIST_SEQUENCE));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_RECORD, type_node, list_node, NULL);
  
  return lookahead;
} /* end record_type */


/* --------------------------------------------------------------------------
 * private function field_list_sequence()
 * --------------------------------------------------------------------------
 * fieldListSequence :=
 *   fieldList ( ';' fieldList )*
 *   ;
 *
 * astnode: (FIELDLISTSEQ fieldListNode+)
 * ----------------------------------------------------------------------- */

m2c_token_t field_list (m2c_parser_context_t p);

m2c_token_t field_list_sequence (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t tmp_list;
  
  PARSER_DEBUG_INFO("fieldListSequence");
  
  /* fieldList */
  lookahead = field_list(p);
  
  tmp_list = m2c_fifo_new_queue(p->ast);
  
  /* ( ';' fieldList )* */
  while (lookahead == TOKEN_SEMICOLON) {
    /* ';' */
    lookahead = m2c_consume_sym(p->lexer);
        
    /* fieldList */
    if (match_set(p, FIRST(VARIABLE_DECLARATION))) {
      lookahead = field_list(p);
      m2c_fifo_enqueue(tmp_list, p->ast);
    }
    else /* resync */ {
      lookahead = skip_to_token(p, TOKEN_SEMICOLON, TOKEN_END, NULL);
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_list_node(AST_FIELDLISTSEQ, tmp_list);
  
  m2c_fifo_release(tmp_list);
  
  return lookahead;
} /* end field_list_sequence */


/* --------------------------------------------------------------------------
 * private function field_list()
 * --------------------------------------------------------------------------
 * fieldList :=
 *   identList ':' ( typeIdent | subrangeType | arrayType | procedureType )
 *   ;
 *
 * astNode: (FIELDLIST identListNode typeNode)
 * ----------------------------------------------------------------------- */

m2c_token_t field_list (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node, list_node;
    
  PARSER_DEBUG_INFO("fieldList");
  
  /* IdentList */
  lookahead = ident_list(p);
  list_node = p->ast;
  
  /* ':' */
  if (match_token(p, TOKEN_COLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token(p,
      TOKEN_IDENT, TOKEN_LBRACKET, TOKEN_ARRAY, TOKEN_PROCEDURE, NULL);
  } /* end if */
    
    /* type */
    if (match_set(p, FIRST(TYPE), FOLLOW(VARIABLE_DECLARATION))) {
      lookahead = type(p);
      tc = p->ast;
    } /* end if */
  }
  else /**/ {
    lookahead = skip_to_set(p, FOLLOW(VARIABLE_DECLARATION));
  } /* end if */
  
  /* typeIdent | subrangeType | arrayType | procedureType */
  if (match_token_list(p,
    TOKEN_IDENT, TOKEN_LBRACKET, TOKEN_ARRAY, TOKEN_PROCEDURE, NULL)) {
    
    switch (lookahead) {
      case TOKEN_ARRAY :
        lookahead = array_type(p);
        break;
        
      case TOKEN_PROCEDURE :
        lookahead = procedure_type(p);
        break;
        
      case TOKEN_IDENT :
        lookahead = qualident(p);
        break;
        
      case TOKEN_LBRACKET :
        lookahead = subrange_type(p);
        break;
        
      default :
        /* fatal error -- abort */
        exit (-1);
    } /* end switch */
    type_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(FIELD_LIST));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_FIELDLIST, list_node, type_node, NULL);
  
  return lookahead;
} /* end field_list */


/* --------------------------------------------------------------------------
 * private function pointer_type()
 * --------------------------------------------------------------------------
 * pointerType :=
 *   POINTER TO typeIdent
 *   ;
 *
 * astNode: (POINTER qualidentNode)
 * ----------------------------------------------------------------------- */

m2c_token_t pointer_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node;
  
  PARSER_DEBUG_INFO("pointerType");
  
  /* POINTER */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* TO */
  if (match_token(p, TOKEN_TO)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(POINTER_TYPE));
  } /* end if */
  
  /* typeIdent */
  if (match_(p, FIRST(TYPE))) {
      lookahead = qualident(p);
      type_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(POINTER_TYPE));
    type_node = m2c_ast_empty_type();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_POINTER, type_node, NULL);
  
  return lookahead;
} /* end pointer_type */


/* --------------------------------------------------------------------------
 * private function opaque_type()
 * --------------------------------------------------------------------------
 * opaqueType :=
 *   OPAQUE ('[' allocSize ']')?;
 *
 * alias allocSize = constExpression ;
 *
 * astNode: (OPAQUE size)
 * ----------------------------------------------------------------------- */
 
m2c_token_t opaque_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node;
  
  PARSER_DEBUG_INFO("opaqueType");
  
  /* OPAQUE */
  lookahead = m2c_consume_sym(p);
  
  /* '[' */
  if (lookahead == TOKEN_LBRACKET) {
    lookahead = m2c_consume_sym(p->lexer);
  
    /* allocSize */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = expression(p);
      size_node = p->ast;
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_RBRACKET, FOLLOW(TYPE_DEFINITION));
        size_node = m2c_ast_empty_node();
    } /* end if */
  
  /* ']' */
  if (match_token(p, TOKEN_RBRACKET)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(TYPE_DEFINITION));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_OPAQUE, size_node, NULL);
  
  return lookahead;
} /* end pointer_type */


/* --------------------------------------------------------------------------
 * private function procedure_type()
 * --------------------------------------------------------------------------
 * procedureType :=
 *   PROCEDURE
 *   ( '(' formalType ( ',' formalType )* ')' )? ( ':' returnedType )?
 *   ;
 *
 * alias returnedType = typeIdent ;
 *
 * astNode: (PROCTYPE qualidentNode formalTypeNode+)
 * ----------------------------------------------------------------------- */

m2c_token_t formal_type (m2c_parser_context_t p);

m2c_token_t procedure_type (m2c_parser_context_t p) {
  m2c_astnode_t type_node, list_node;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("procedureType");
  
  /* PROCEDURE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ( '(' formalTypeList ')' )? */
  if (lookahead == TOKEN_LPAREN) {
    /* '(' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* formalTypeList */
    if (match_set(p, FIRST(FORMAL_TYPE))) {
      lookahead = formal_type_list(p);
      list_node = p->ast;
    }
    else /* resync */ {
      lookahead =
        skip_to_token(p, TOKEN_RPAREN, TOKEN_COLON, TOKEN_IDENT, NULL);
      list_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ')' */
    if (match_token(p, TOKEN_RPAREN)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_token(p, TOKEN_COLON, TOKEN_IDENT, NULL);
    } /* end if */
  }
  else /* no formal type list */ {
    list_node = m2c_ast_empty_node();
  } /* end if */
  
  /* (':' returnType)? */
  if (lookahead == TOKEN_COLON)) {
    /* ':' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* returnType */
    if (match_token(p, TOKEN_IDENT)) {
      lookahead = qualident(p);
      type_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(TYPE_DEFINITION));
      type_node = m2c_ast_empty_node();
    } /* end if */
  }
  else /* no return type */ {
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_PROCTYPE, type_node, list_node, NULL);
  
  return lookahead;
} /* end procedure_type */


/* --------------------------------------------------------------------------
 * private function formal_type_list()
 * --------------------------------------------------------------------------
 * formalTypeList :=
 *   formalType ( ',' formalType )*
 *   ;
 *
 * astNode: (FTYPELIST formalTypeNode+)
 * ----------------------------------------------------------------------- */

m2c_token_t formal_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t tmp_list;
  
  PARSER_DEBUG_INFO("formalTypeList");
  
  /* formalType */
  lookahead = formal_type(p);
  tmp_list = m2c_fifo_new_queue(p->ast);
  
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* formalType */
    if (match_set(p, FIRST(FORMAL_TYPE))) {
      lookahead = formal_type(p);
      m2c_fifo_enqueue(tmp_list, p->ast);
    }
    else /* resync */ {
      lookahead = skip_to_token(p, TOKEN_COMMA, TOKEN_RPAREN, NULL);
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_FTYPELIST, tmp_list, NULL);
  
  return lookahead;
} /* end if */


/* --------------------------------------------------------------------------
 * private function formal_type()
 * --------------------------------------------------------------------------
 * formalType :=
 *   ( CONST | VAR )? nonAttrFormalType
 *   ;
 *
 * astNode: formalTypeNode
 * ----------------------------------------------------------------------- */

m2c_token_t non_attr_formal_type (m2c_parser_context_t p);

m2c_token_t formal_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_node_t type_node;
  
  PARSER_DEBUG_INFO("formalType");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* (CONST | VAR)? */
  if ((lookahead == TOKEN_CONST) {
    /* CONST */
    lookahead = m2c_consume_sym(p->lexer);
    node_type = AST_CONSTP;
  }
  else if (lookahead == TOKEN_VAR) {
    /* VAR */
    lookahead = m2c_consume_sym(p->lexer);
    node_type = AST_VARP;
  }
  else /* no attribute */
    node_type = ;
  } /* end if */
  
  /* nonAttrFormalType */
  if (match_set(p, FIRST(NON_ATTR_FORMAL_TYPE))) {
    lookahead = non_attr_formal_type(p);
    type_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(FORMAL_TYPE));
    type_node = m2c_ast_empty_node();
  } /* end if */
    
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(node_type, type_node, NULL);
  
  return lookahead;
} /* end formal_type */


/* --------------------------------------------------------------------------
 * private function non_attr_formal_type()
 * --------------------------------------------------------------------------
 * nonAttrFormalType :=
 *   simpleFormalType | castingFormalType | variadicFormalType
 *   ;
 *
 * astnode:
 * ----------------------------------------------------------------------- */

m2c_token_t non_attr_formal_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("nonAttrFormalType");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* simpleFormalType | castingFormalType | variadicFormalType */
  switch (lookahead) {
    /* ARGLIST */
    case TOKEN_ARGLIST :
      lookahead = variadic_formal_type(p);
      break;
    
    /* ARRAY */
    case TOKEN_ARRAY :
      lookahead = simple_formal_type(p);
      break;
      
    /* CAST | Ident */
    case TOKEN_IDENT :
      lexeme = m2c_lookahead_lexeme(p->lexer);
      
      /* CAST */
      if (lexeme == m2c_res_ident(RESIDENT_CAST)) {
        lookahead = casting_formal_type(p);
      }
      /* Ident */
      else {
        lookahead = simple_formal_type(p);
      } /* end if */
      break;
    
    default :
    /* fatal error -- abort */
    return (-1);
  } /* end switch */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end non_attr_formal_type */


/* --------------------------------------------------------------------------
 * private function simple_formal_type()
 * --------------------------------------------------------------------------
 * simpleFormalType :=
 *   ( ARRAY OF )? typeIdent
 *   ;
 *
 * astnode:
 *  (IDENT ident) | (QUALIDENT q0 q1 q2 ... qN ident) | (OPENARRAY identNode)
 * ----------------------------------------------------------------------- */

m2c_token_t simple_formal_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node;
  bool open_array;
  
  PARSER_DEBUG_INFO("simpleFormalType");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* ( ARRAY OF )? */
  if (lookahead == TOKEN_ARRAY) {
    /* ARRAY */
    lookahead = m2c_consume_sym(p->lexer);
    open_array = true;
    
    /* OF */
    if (match_token(p, TOKEN_OF)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(SIMPLE_FORMAL_TYPE));
    } /* end if */
  }
  else /* not an open array */ {
    open_array = false;
  } /* end if */
    
  /* typeIdent */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = qualident(p);
    type_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(SIMPLE_FORMAL_TYPE));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  
  if (open_array) {
    /* astnode: (OPENARRAY identNode) */
    p->ast = m2c_ast_new_node(AST_OPENARRAY, type_node, NULL);
  }
  else {
    /* astnode: (IDENT ident) | (QUALIDENT q0 q1 q2 ... qN ident) */
    p->ast = type_node;
  } /* end if */
  
  return lookahead;
} /* end simple_formal_type */


/* --------------------------------------------------------------------------
 * private function casting_formal_type()
 * --------------------------------------------------------------------------
 * castingFormalType :=
 *   CAST ( OCTETSEQ | ADDRESS )
 *   ;
 *
 * astNode: (CASTP OctetSeq ) | (CASTP Address)
 * ----------------------------------------------------------------------- */

m2c_token_t casting_formal_type (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  m2c_astnode_t type_node;
  
  PARSER_DEBUG_INFO("castingFormalType");
  
  /* CAST */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* OCTETSEQ | ADDRESS */
  lexeme = m2c_res_ident(RESIDENT_ADDRESS);
  if (match_token_or_lexeme(p, TOKEN_OCTETSEQ, lexeme)) {
    
    /* OCTETSEQ */
    if (lookahead = TOKEN_OCTETSEQ) {
      lookahead = m2c_consume_sym(p->lexer);
      type_node = m2c_ast_new_node(AST_OCTSEQ, NULL);
    }
    /* | ADDRESS */
    else {
      type_node = m2c_ast_new_node(AST_ADDR, NULL);
    } /* end if */
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(CASTING_FORMALTYPE));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_CASTP, type_node, NULL);
  
  return lookahead;
} /* end casting_formal_type */


/* --------------------------------------------------------------------------
 * private function variadic_formal_type()
 * --------------------------------------------------------------------------
 * variadicFormalType :=
 *   ARGLIST OF simpleFormalType
 *   ;
 *
 * astNode: (VARGP formalTypeNode)
 * ----------------------------------------------------------------------- */

m2c_token_t variadic_formal_type (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  m2c_astnode_t type_node;
  
  PARSER_DEBUG_INFO("variadicFormalType");
  
  /* ARGLIST */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* OF */
  if (match_token(p, TOKEN_OF)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(SIMPLE_FORMAL_TYPE));
  } /* end if */
  
  /* simpleFormalType */
  if (match_set(p, FIRST(SIMPLE_FORMAL_TYPE))) {
    lookahead = simple_formal_type(p);
    type_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(VARIADIC_FORMAL_TYPE));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_VARGP, type_node, NULL);
  
  return lookahead;
} /* end variadic_formal_type */


/* --------------------------------------------------------------------------
 * private function var_definition()
 * --------------------------------------------------------------------------
 * varDefinition :=
 *   identList ':' typeIdent
 *   ;
 *
 * astNode: (VARDECL idlist typeId)
 * ----------------------------------------------------------------------- */

m2c_token_t var_definition (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  m2c_astnode_t list_node, type_node;
  
  PARSER_DEBUG_INFO("varDefinition");
  
  /* identList */
  lookahead = ident_list(p);
  list_node = p->ast;
  
  /* ':' */
  if (match_token(p, TOKEN_COLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead =
      skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(VAR_DEFINITION));
  } /* end if */
  
  /* typeIdent */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = qualident(p);
    type_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(VAR_DEFINITION));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_VARDECL, list_node, type_node, NULL);
  
  return lookahead;
} /* end var_definition */


/* --------------------------------------------------------------------------
 * private function procedure_header()
 * --------------------------------------------------------------------------
 * procedureHeader :=
 *   PROCEDURE procedureSignature
 *   ;
 * ----------------------------------------------------------------------- */

m2c_token_t procedure_signature (m2c_parser_context_t p);

m2c_token_t procedure_header (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("procedureHeader");
  
  /* PROCEDURE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* procedureSignature */
  if (match_token(p, TOKEN_IDENTIFIER, FOLLOW(PROCEDURE_HEADER))) {
    lookahead = procedure_signature(p);
  } /* end if */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end procedure_header */


/* --------------------------------------------------------------------------
 * private function procedure_signature()
 * --------------------------------------------------------------------------
 * procedureSignature :=
 *   Ident ( '(' formalParamList? ')' ( ':' returnedType )? )?
 *   ;
 *
 * astnode: (PROCDEF identNode formalParamListNode returnTypeNode)
 * ----------------------------------------------------------------------- */

m2c_token_t formal_param_list (m2c_parser_context_t p);

m2c_token_t procedure_signature (m2c_parser_context_t p) {
  m2c_astnode_t id, fplist, rtype;
  m2c_string_t ident;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("procedureSignature");
  
  /* Ident */
  lookahead = m2c_consume_sym(p->lexer);
  ident = m2c_lexer_current_lexeme(p->lexer);
  id = m2c_ast_new_terminal_node(AST_IDENT, ident);
  
  /* ( '(' formalParamList? ')' ( ':' returnedType )? )? */
  if (lookahead == TOKEN_LEFT_PAREN) {
    
    /* '(' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* formalParamList? */
    if ((lookahead == TOKEN_IDENTIFIER) ||
        (lookahead == TOKEN_VAR)) {
      lookahead = formal_param_list(p);
      fplist = p->ast;
    }
    else {
      fplist = m2c_ast_empty_node();
    } /* end if */
    
    /* ')' */
    if (match_token(p, TOKEN_RIGHT_PAREN, FOLLOW(PROCEDURE_TYPE))) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
    
    /* ( ':' returnedType )? */
    if (lookahead == TOKEN_COLON) {
      /* ':' */
      lookahead = m2c_consume_sym(p->lexer);
    
      /* returnedType */
      if (match_token(p, TOKEN_IDENTIFIER, FOLLOW(PROCEDURE_TYPE))) {
        lookahead = qualident(p);
        rtype = p->ast;
      } /* end if */
    }
    else {
      rtype = m2c_ast_empty_node();
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_PROCDEF, id, fplist, rtype, NULL);
  
  return lookahead;
} /* end procedure_signature */


/* --------------------------------------------------------------------------
 * private function formal_param_list()
 * --------------------------------------------------------------------------
 * formalParamList :=
 *   formalParams ( ';' formalParams )*
 *   ;
 *
 * astnode: (FPARAMLIST fparams+)
 * ----------------------------------------------------------------------- */

m2c_token_t formal_params (m2c_parser_context_t p);

m2c_token_t formal_param_list (m2c_parser_context_t p) {
  m2c_fifo_t tmplist;
  m2c_token_t lookahead;
  uint_t line_of_semicolon, column_of_semicolon;
  
  PARSER_DEBUG_INFO("formalParamList");
  
  /* formalParams */
  lookahead = formal_params(p);
  tmplist = m2c_fifo_new_queue(p->ast);
  
  /* ( ';' formalParams )* */
  while (lookahead == TOKEN_SEMICOLON) {
    /* ';' */
    line_of_semicolon = m2c_lexer_lookahead_line(p->lexer);
    column_of_semicolon = m2c_lexer_lookahead_column(p->lexer);
    lookahead = m2c_consume_sym(p->lexer);
    
    /* check if semicolon occurred at the end of a formal parameter list */
    if (lookahead == TOKEN_RIGHT_PAREN) {
    
      if (m2c_option_errant_semicolon()) {
        /* treat as warning */
        m2c_emit_warning_w_pos
          (M2C_SEMICOLON_AFTER_FORMAL_PARAM_LIST,
           line_of_semicolon, column_of_semicolon);
        p->warning_count++;
      }
      else /* treat as error */ {
        m2c_emit_error_w_pos
          (M2C_SEMICOLON_AFTER_FORMAL_PARAM_LIST,
           line_of_semicolon, column_of_semicolon);
        p->error_count++;
      } /* end if */
      
      /* print source line */
      if (m2c_option_verbose()) {
        m2c_print_line_and_mark_column(p->lexer,
          line_of_semicolon, column_of_semicolon);
      } /* end if */
    
      /* leave field list sequence loop to continue */
      break;
    } /* end if */
    
    /* formalParams */
    if (match_set(p, FIRST(FORMAL_PARAMS), FOLLOW(FORMAL_PARAMS))) {
      lookahead = formal_params(p);
      m2c_fifo_enqueue(tmplist, p->ast);
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_list_node(AST_FPARAMLIST, tmplist);
  m2c_fifo_release(tmplist);
  
  return lookahead;
} /* end formal_param_list */


/* --------------------------------------------------------------------------
 * private function formal_params()
 * --------------------------------------------------------------------------
 * formalParams :=
 *   simpleFormalParams | attribFormalParams
 *   ;
 * ----------------------------------------------------------------------- */

m2c_token_t simple_formal_params (m2c_parser_context_t p);

m2c_token_t attrib_formal_params (m2c_parser_context_t p);

m2c_token_t formal_params (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("formalParams");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* simpleFormalParams */
  if (lookahead == TOKEN_IDENTIFIER) {
    lookahead = simple_formal_params(p);
  }
  /* | attribFormalParams */
  else if ((lookahead == TOKEN_CONST) || (lookahead == TOKEN_VAR)) {
    lookahead = attrib_formal_params(p);
  }
  else /* unreachable code */ {
    /* fatal error -- abort */
      exit(-1);
  } /* end if */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end formal_params */


/* --------------------------------------------------------------------------
 * private function simple_formal_params()
 * --------------------------------------------------------------------------
 * simpleFormalParams :=
 *   identList ':' simpleFormalType
 *   ;
 *
 * astnode: (FPARAMS identListNode simpleFormalTypeNode)
 * ----------------------------------------------------------------------- */

m2c_token_t simple_formal_params (m2c_parser_context_t p) {
  m2c_astnode_t idlist, sftype;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("simpleFormalParams");
  
  /* IdentList */
  lookahead = ident_list(p);
  idlist = p->ast;
  
  /* ':' */
  if (match_token(p, TOKEN_COLON, FOLLOW(SIMPLE_FORMAL_PARAMS))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* formalType */
    if (match_set(p, FIRST(FORMAL_TYPE), FOLLOW(SIMPLE_FORMAL_PARAMS))) {
      lookahead = simple_formal_type(p);
      sftype = p->ast;
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_FPARAMS, idlist, sftype, NULL);
  
  return lookahead;
} /* end simple_formal_params */


/* --------------------------------------------------------------------------
 * private function attrib_formal_params()
 * --------------------------------------------------------------------------
 * attribFormalParams :=
 *   ( CONST | VAR ) simpleFormalParams
 *   ;
 *
 * astnode: (FPARAMS identListNode formalTypeNode)
 * ----------------------------------------------------------------------- */

m2c_token_t attrib_formal_params (m2c_parser_context_t p) {
  m2c_astnode_t aftype, sftype;
  m2c_token_t lookahead;
  bool const_attr = false;
  
  PARSER_DEBUG_INFO("attribFormalParams");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* CONST */
  if (lookahead == TOKEN_CONST) {
    lookahead = m2c_consume_sym(p->lexer);
    const_attr = true;
  }
  /* | VAR */
  else if (lookahead == TOKEN_VAR) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* unreachable code */ {
    /* fatal error -- abort */
    exit(-1);
  } /* end if */
  
  /* simpleFormalParams */
  if (match_set(p, FIRST(SIMPLE_FORMAL_PARAMS),
      FOLLOW(ATTRIB_FORMAL_PARAMS))) {
    lookahead = simple_formal_params(p);
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  sftype = m2c_ast_subnode_for_index(p->ast, 1);
  
  if (const_attr) {
    aftype = m2c_ast_new_node(AST_CONSTP, sftype, NULL);
  }
  else {
    aftype = m2c_ast_new_node(AST_VARP, sftype, NULL);
  } /* end if */
  
  m2c_ast_replace_subnode(p->ast, 1, aftype);
  
  return lookahead;
} /* end attrib_formal_params */


/* ************************************************************************ *
 * Implementation and Program Module Syntax                                 *
 * ************************************************************************ */


/* --------------------------------------------------------------------------
 * private function implementation_module()
 * --------------------------------------------------------------------------
 * implementationModule :=
 *   IMPLEMENTATION programModule
 *   ;
 * ----------------------------------------------------------------------- */

m2c_token_t implementation_module (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("implementationModule");
  
  /* IMPLEMENTATION */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* programModule */
  if (match_token(p, TOKEN_MODULE, FOLLOW(PROGRAM_MODULE))) {
    lookahead = program_module(p);
  } /* end if */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end implementation_module */


/* --------------------------------------------------------------------------
 * private function program_module()
 * --------------------------------------------------------------------------
 * programModule :=
 *   MODULE moduleIdent modulePriority? ';'
 *   import* block moduleIdent '.'
 *   ;
 *
 * moduleIdent := Ident ;
 *
 * astnode: (IMPMOD identNode priorityNode importListNode blockNode)
 * ----------------------------------------------------------------------- */

m2c_token_t block (m2c_parser_context_t p);

m2c_token_t program_module (m2c_parser_context_t p) {
  m2c_astnode_t id, prio, implist, body;
  m2c_string_t ident1, ident2;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("programModule");
  
  /* MODULE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENTIFIER, RESYNC(IMPORT_OR_BLOCK))) {
    lookahead = m2c_consume_sym(p->lexer);
    ident1 = m2c_lexer_current_lexeme(p->lexer);
    
    /* modulePriority? */
    if (lookahead == TOKEN_LEFT_BRACKET) {
      lookahead = module_priority(p);
      prio = p->ast;
    }
    else /* no module priority */ {
      prio = m2c_ast_empty_node();
    } /* end while */
    
    /* ';' */
    if (match_token(p, TOKEN_SEMICOLON, RESYNC(IMPORT_OR_BLOCK))) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */
  
  tmplist = m2c_fifo_new_queue(NULL);
  
  /* import* */
  while ((lookahead == TOKEN_IMPORT) ||
         (lookahead == TOKEN_FROM)) {
    lookahead = import(p);
    m2c_fifo_enqueue(tmplist, p->ast);
  } /* end while */
  
  if (m2c_fifo_entry_count(tmplist) > 0) {
    implist = m2c_ast_new_list_node(AST_IMPLIST, tmplist);
  }
  else /* no import list */ {
    implist = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(tmplist);
  
  /* block */
  if (match_set(p, FIRST(BLOCK), FOLLOW(PROGRAM_MODULE))) {
    lookahead = block(p);
    body = p->ast;
    
    /* moduleIdent */
    if (match_token(p, TOKEN_IDENTIFIER, FOLLOW(PROGRAM_MODULE))) {
      lookahead = m2c_consume_sym(p->lexer);
      ident2 = m2c_lexer_current_lexeme(p->lexer);
      
      if (ident1 != ident2) {
        /* TO DO: report error -- module identifiers don't match */ 
      } /* end if */
      
      if (match_token(p, TOKEN_PERIOD, FOLLOW(PROGRAM_MODULE))) {
        lookahead = m2c_consume_sym(p->lexer);
      } /* end if */
    } /* end if */
  } /* end if */  
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IMPMOD, id, prio, implist, body, NULL);
  
  return lookahead;
} /* end program_module */


/* --------------------------------------------------------------------------
 * private function private_import()
 * --------------------------------------------------------------------------
 * privateImport :=
 *   IMPORT moduleList
 *   ;
 *
 * moduleList := identList ;
 *
 * astnode: (IMPORT identListNode)
 * ----------------------------------------------------------------------- */

m2c_token_t ident_list (m2c_parser_context_t p);

m2c_token_t qualified_import (m2c_parser_context_t p) {
  m2c_astnode_t idlist;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("qualifiedImport");
  
  /* IMPORT */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* moduleList */
  if (match_token(p, TOKEN_IDENTIFIER, FOLLOW(QUALIFIED_IMPORT))) {
    lookahead = ident_list(p);
    idlist = p->ast;
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IMPORT, idlist, NULL);
  
  return lookahead;
} /* end qualified_import */


/* --------------------------------------------------------------------------
 * private function block()
 * --------------------------------------------------------------------------
 * block :=
 *   declaration* ( BEGIN statementSequence )? END
 *   ;
 *
 * astnode: (BLOCK declarationListNode statementSeqNode)
 * ----------------------------------------------------------------------- */

m2c_token_t declaration (m2c_parser_context_t p);

m2c_token_t statement_sequence (m2c_parser_context_t p);

m2c_token_t block (m2c_parser_context_t p) {
  m2c_astnode_t decllist, stmtseq;
  m2c_fifo_t tmplist;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("block");
  
  lookahead = m2c_next_sym(p->lexer);
  
  tmplist = m2c_fifo_new_queue(NULL);
  
  /* declaration* */
  while ((lookahead == TOKEN_CONST) ||
         (lookahead == TOKEN_TYPE) ||
         (lookahead == TOKEN_VAR) ||
         (lookahead == TOKEN_PROCEDURE) ||
         (lookahead == TOKEN_MODULE)) {
    lookahead = declaration(p);
    m2c_fifo_enqueue(tmplist, p->ast);
  } /* end while */
  
  if (m2c_fifo_entry_count(tmplist) > 0) {
    decllist = m2c_ast_new_list_node(AST_DECLLIST, tmplist);
  }
  else /* no declarations */ {
    decllist = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(tmplist);
  
  /* ( BEGIN statementSequence )? */
  if (lookahead == TOKEN_BEGIN) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* check for empty statement sequence */
    if ((m2c_tokenset_element(FOLLOW(STATEMENT_SEQUENCE), lookahead))) {
    
        /* print warning */
        m2c_emit_warning_w_pos
          (M2C_EMPTY_STMT_SEQ,
           m2c_lexer_lookahead_line(p->lexer),
           m2c_lexer_lookahead_column(p->lexer));
        p->warning_count++;
    }
    /* statementSequence */
    else if (match_set(p, FIRST(STATEMENT_SEQUENCE), FOLLOW(STATEMENT))) {
      lookahead = statement_sequence(p);
      stmtseq = p->ast;
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  }
  else /* no statement sequence */ {
    stmtseq = m2c_ast_empty_node();
  } /* end if */
  
  /* END */
  if (match_token(p, TOKEN_END, FOLLOW(BLOCK))) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_BLOCK, decllist_node, stmtseq, NULL);
  
  return lookahead;
} /* end block */


/* --------------------------------------------------------------------------
 * private function declaration()
 * --------------------------------------------------------------------------
 * declaration :=
 *   CONST ( constDeclaration ';' )* |
 *   TYPE ( typeDeclaration ';' )* |
 *   VAR ( variableDeclaration ';' )* |
 *   procedureDeclaration ';'
 *   moduleDeclaration ';'
 *   ;
 * 
 * constDeclaration := constDefinition ;
 * ----------------------------------------------------------------------- */

m2c_token_t type_declaration (m2c_parser_context_t p);
m2c_token_t variable_declaration (m2c_parser_context_t p);
m2c_token_t procedure_declaration (m2c_parser_context_t p);
m2c_token_t module_declaration (m2c_parser_context_t p);

m2c_token_t declaration (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("declaration");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
    
    /* CONST */
    case TOKEN_CONST :
      lookahead = m2c_consume_sym(p->lexer);
      
      /* ( constDeclaration ';' )* */
      while (lookahead == TOKEN_IDENTIFIER) {
        lookahead = const_definition(p);
        
        /* ';' */
        if (match_token(p, TOKEN_SEMICOLON,
            RESYNC(DECLARATION_OR_IDENT_OR_SEMICOLON))) {
          lookahead = m2c_consume_sym(p->lexer);
        } /* end if */
      } /* end while */
      break;
      
    /* | TYPE */
    case TOKEN_TYPE :
      lookahead = m2c_consume_sym(p->lexer);
      
      /* ( typeDeclaration ';' )* */
      while (lookahead == TOKEN_IDENTIFIER) {
        lookahead = type_declaration(p);
        
        /* ';' */
        if (match_token(p, TOKEN_SEMICOLON,
            RESYNC(DECLARATION_OR_IDENT_OR_SEMICOLON))) {
          lookahead = m2c_consume_sym(p->lexer);
        } /* end if */
      } /* end while */
      break;
      
    /* | VAR */
    case TOKEN_VAR :
      lookahead = m2c_consume_sym(p->lexer);
      
      /* ( variableDeclaration ';' )* */
      while (lookahead == TOKEN_IDENTIFIER) {
        lookahead = variable_declaration(p);
        
        /* ';' */
        if (match_token(p, TOKEN_SEMICOLON,
            RESYNC(DECLARATION_OR_IDENT_OR_SEMICOLON))) {
          lookahead = m2c_consume_sym(p->lexer);
        } /* end if */
      } /* end while */
      break;
      
    /* | procedureDeclaration ';' */
    case TOKEN_PROCEDURE :
      lookahead = procedure_declaration(p);
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON,
          RESYNC(DECLARATION_OR_SEMICOLON))) {
        lookahead = m2c_consume_sym(p->lexer);
      } /* end if */
      break;
      
    /* | moduleDeclaration ';' */
    case TOKEN_MODULE :
      lookahead = module_declaration(p);
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON,
          RESYNC(DECLARATION_OR_SEMICOLON))) {
        lookahead = m2c_consume_sym(p->lexer);
      } /* end if */
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
  } /* end switch */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end declaration */


/* --------------------------------------------------------------------------
 * private function type_declaration()
 * --------------------------------------------------------------------------
 * typeDeclaration :=
 *   Ident '=' ( type | varSizeRecordType )
 *   ;
 *
 * astnode: (TYPEDECL identNode typeConstructorNode)
 * ----------------------------------------------------------------------- */

m2c_token_t var_size_record_type (m2c_parser_context_t p);

m2c_token_t type_declaration (m2c_parser_context_t p) {
  m2c_astnode_t id, tc;
  m2c_string_t ident;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("typeDeclaration");
  
  /* Ident */
  lookahead = m2c_consume_sym(p->lexer);
  ident = m2c_lexer_current_lexeme(p->lexer);
  id = m2c_ast_new_terminal_node(AST_IDENT, ident);
  
  /* '=' */
  if (match_token(p, TOKEN_EQUAL, FOLLOW(TYPE_DECLARATION))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* type | varSizeRecordType */
    if (match_set(p, FIRST(TYPE_DECLARATION_TAIL),
        FOLLOW(TYPE_DECLARATION))) {
      
      /* type */
      if (lookahead != TOKEN_VAR) {
        lookahead = type(p);
        tc = p->ast;
      }
      /* | varSizeRecordType */
      else {
        lookahead = var_size_record_type(p);
        tc = p->ast;
      } /* end if */
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_TYPEDECL, id, tc, NULL);
  
  return lookahead;
} /* end type_declaration */


/* --------------------------------------------------------------------------
 * private function var_size_record_type()
 * --------------------------------------------------------------------------
 * varSizeRecordType :=
 *   VAR RECORD fieldListSequence
 *   VAR varSizeFieldIdent ':' ARRAY sizeFieldIdent OF typeIdent
 *   END
 *   ;
 *
 * astnode:
 *  (VSREC fieldListSeqNode (VSFIELD identNode identNode identNode))
 * ----------------------------------------------------------------------- */

m2c_token_t var_size_record_type (m2c_parser_context_t p) {
  m2c_astnode_t flseq, vsfield, vsfieldid, sizeid, typeid;
  m2c_string_t ident;
  uint_t line_of_semicolon, column_of_semicolon;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("varSizeRecordType");
  
  /* VAR */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* RECORD */
  if (match_token(p, TOKEN_RECORD, FOLLOW(VAR_SIZE_RECORD_TYPE))) {
    lookahead = m2c_consume_sym(p->lexer); 
    
    /* check for empty field list sequence */
    if (lookahead == TOKEN_VAR) {

        /* empty field list sequence warning */
        m2c_emit_warning_w_pos
          (M2C_EMPTY_FIELD_LIST_SEQ,
           m2c_lexer_lookahead_line(p->lexer),
           m2c_lexer_lookahead_column(p->lexer));
        p->warning_count++;
    }
    /* fieldListSequence */
    else if (match_set(p, FIRST(FIELD_LIST_SEQUENCE),
             FOLLOW(VAR_SIZE_RECORD_TYPE))) {
      lookahead = field_list_sequence(p);
      flseq = p->ast;
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
    
    /* VAR */
    if (match_token(p, TOKEN_VAR, FOLLOW(VAR_SIZE_RECORD_TYPE))) {
      lookahead = m2c_consume_sym(p->lexer);
      
      if (lookahead == TOKEN_END) {
        m2c_emit_warning_w_pos
          (M2C_EMPTY_FIELD_LIST_SEQ,
           m2c_lexer_lookahead_line(p->lexer),
           m2c_lexer_lookahead_column(p->lexer));
           m2c_consume_sym(p->lexer);
        p->warning_count++;
      }
      /* varSizeFieldIdent */
      else if (match_token(p, TOKEN_IDENTIFIER,
               FOLLOW(VAR_SIZE_RECORD_TYPE))) {
        lookahead = m2c_consume_sym(p->lexer);
        ident = m2c_lexer_current_lexeme(p->lexer);
        vsfieldid = m2c_ast_new_terminal_node(AST_IDENT, ident);
      
        /* ':' */
        if (match_token(p, TOKEN_COLON, FOLLOW(VAR_SIZE_RECORD_TYPE))) {
          lookahead = m2c_consume_sym(p->lexer);
        
          /* ARRAY */
          if (match_token(p, TOKEN_ARRAY, FOLLOW(VAR_SIZE_RECORD_TYPE))) {
            lookahead = m2c_consume_sym(p->lexer);
          
            /* sizeFieldIdent */
            if (match_token(p, TOKEN_IDENTIFIER,
                FOLLOW(VAR_SIZE_RECORD_TYPE))) {
              lookahead = m2c_consume_sym(p->lexer);
              ident = m2c_lexer_current_lexeme(p->lexer);
              sizeid = m2c_ast_new_terminal_node(AST_IDENT, ident);
            
              /* OF */
              if (match_token(p, TOKEN_OF, FOLLOW(VAR_SIZE_RECORD_TYPE))) {
                lookahead = m2c_consume_sym(p->lexer);
              
                /* typeIdent */
                if (match_token(p, TOKEN_IDENTIFIER,
                    FOLLOW(VAR_SIZE_RECORD_TYPE))) {
                  lookahead = qualident(p);
                  typeid = p->ast;
                  
                  /* check for errant semicolon */
                  if (lookahead == TOKEN_SEMICOLON) {
                    line_of_semicolon =
                      m2c_lexer_lookahead_line(p->lexer);
                    column_of_semicolon =
                      m2c_lexer_lookahead_column(p->lexer);
                  
                    if (m2c_option_errant_semicolon()) {
                      /* treat as warning */
                      m2c_emit_warning_w_pos
                        (M2C_SEMICOLON_AFTER_FIELD_LIST_SEQ,
                         line_of_semicolon, column_of_semicolon);
                      p->warning_count++;
                    }
                    else /* treat as error */ {
                      m2c_emit_error_w_pos
                        (M2C_SEMICOLON_AFTER_FIELD_LIST_SEQ,
                         line_of_semicolon, column_of_semicolon);
                      p->error_count++;
                    } /* end if */
                    
                    m2c_consume_sym(p->lexer);
                    
                    /* print source line */
                    if (m2c_option_verbose()) {
                      m2c_print_line_and_mark_column(p->lexer,
                        line_of_semicolon, column_of_semicolon);
                    } /* end if */
                  } /* end if */
                  
                  if (match_token(p, TOKEN_END,
                      FOLLOW(VAR_SIZE_RECORD_TYPE))) {
                    lookahead = m2c_consume_sym(p->lexer);
                  } /* end if */
                } /* end if */
              } /* end if */
            } /* end if */
          } /* end if */
        } /* end if */
      } /* end if */
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  vsfield = m2c_ast_new_node(AST_VSFIELD, vsfieldid, sizeid, typeid, NULL);
  p->ast = m2c_ast_new_node(AST_VSREC, flseq, vsfield, NULL);
  
  return lookahead;
} /* end var_size_record_type */


/* --------------------------------------------------------------------------
 * private function variable_declaration()
 * --------------------------------------------------------------------------
 * variableDeclaration :=
 *   identList ':' type
 *   ;
 *
 * astnode: (VARDECL identListNode typeConstructorNode)
 * ----------------------------------------------------------------------- */

m2c_token_t variable_declaration (m2c_parser_context_t p) {
  m2c_astnode_t idlist, tc;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("variableDeclaration");
  
  /* IdentList */
  lookahead = ident_list(p);
  idlist = p->ast;
  
  /* ':' */
  if (match_token(p, TOKEN_COLON, FOLLOW(VARIABLE_DECLARATION))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* type */
    if (match_set(p, FIRST(TYPE), FOLLOW(VARIABLE_DECLARATION))) {
      lookahead = type(p);
      tc = p->ast;
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_VARDECL, idlist, tc, NULL);
  
  return lookahead;
} /* end variable_declaration */


/* --------------------------------------------------------------------------
 * private function procedure_declaration()
 * --------------------------------------------------------------------------
 * procedureDeclaration :=
 *   procedureHeader ';' block Ident
 *   ;
 *
 * astnode: (PROC procDefinitionNode blockNode)
 * ----------------------------------------------------------------------- */

m2c_token_t procedure_declaration (m2c_parser_context_t p) {
  m2c_astnode_t procdef, body;
  m2c_string_t ident;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("procedureDeclaration");
  
  /* procedureHeader */
  lookahead = procedure_header(p);
  procdef = p->ast;
  
  /* ';' */
  if (match_token(p, TOKEN_SEMICOLON, FOLLOW(PROCEDURE_DECLARATION))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* block */
    if (match_set(p, FIRST(BLOCK), FOLLOW(PROCEDURE_DECLARATION))) {
      lookahead = block(p);
      body = p->ast;
      
      /* Ident */
      if (match_token(p, TOKEN_IDENTIFIER, FOLLOW(PROCEDURE_DECLARATION))) {
        lookahead = m2c_consume_sym(p->lexer);
        ident = m2c_lexer_current_lexeme(p->lexer);
        
        /* TO DO: check if procedure identifiers match */
      } /* end if */
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_PROC, procdef, body, NULL);
  
  return lookahead;
} /* end procedure_declaration */


/* --------------------------------------------------------------------------
 * private function statement_sequence()
 * --------------------------------------------------------------------------
 * statementSequence :=
 *   statement ( ';' statement )*
 *   ;
 *
 * astnode: (STMTSEQ stmtNode+)
 * ----------------------------------------------------------------------- */

m2c_token_t statement (m2c_parser_context_t p);

m2c_token_t statement_sequence (m2c_parser_context_t p) {
  m2c_fifo_t tmplist;
  m2c_token_t lookahead;
  uint_t line_of_semicolon, column_of_semicolon;
  
  PARSER_DEBUG_INFO("statementSequence");
  
  /* statement */
  lookahead = statement(p);
  tmplist = m2c_fifo_new_queue(p->ast);
  
  /* ( ';' statement )* */
  while (lookahead == TOKEN_SEMICOLON) {
    /* ';' */
    line_of_semicolon = m2c_lexer_lookahead_line(p->lexer);
    column_of_semicolon = m2c_lexer_lookahead_column(p->lexer);
    lookahead = m2c_consume_sym(p->lexer);
    
    /* check if semicolon occurred at the end of a statement sequence */
    if (m2c_tokenset_element(FOLLOW(STATEMENT_SEQUENCE), lookahead)) {
    
      if (m2c_option_errant_semicolon()) {
        /* treat as warning */
        m2c_emit_warning_w_pos
          (M2C_SEMICOLON_AFTER_STMT_SEQ,
           line_of_semicolon, column_of_semicolon);
        p->warning_count++;
      }
      else /* treat as error */ {
        m2c_emit_error_w_pos
          (M2C_SEMICOLON_AFTER_STMT_SEQ,
           line_of_semicolon, column_of_semicolon);
        p->error_count++;
      } /* end if */
      
      /* print source line */
      if (m2c_option_verbose()) {
        m2c_print_line_and_mark_column(p->lexer,
          line_of_semicolon, column_of_semicolon);
      } /* end if */
    
      /* leave statement sequence loop to continue */
      break;
    } /* end if */
    
    /* statement */
    if (match_set(p, FIRST(STATEMENT),
        RESYNC(FIRST_OR_FOLLOW_OF_STATEMENT))) {
      lookahead = statement(p);
      m2c_fifo_enqueue(tmplist, p->ast);
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_list_node(AST_STMTSEQ, tmplist);
  m2c_fifo_release(tmplist);
  
  return lookahead;
} /* end statement_sequence */


/* --------------------------------------------------------------------------
 * private function statement()
 * --------------------------------------------------------------------------
 * statement :=
 *   assignmentOrProcCall | returnStatement | withStatement | ifStatement |
 *   caseStatement | loopStatement | whileStatement | repeatStatement |
 *   forStatement | EXIT
 *   ;
 * ----------------------------------------------------------------------- */

m2c_token_t assignment_or_proc_call (m2c_parser_context_t p);
m2c_token_t return_statement (m2c_parser_context_t p);
m2c_token_t if_statement (m2c_parser_context_t p);
m2c_token_t case_statement (m2c_parser_context_t p);
m2c_token_t loop_statement (m2c_parser_context_t p);
m2c_token_t while_statement (m2c_parser_context_t p);
m2c_token_t repeat_statement (m2c_parser_context_t p);
m2c_token_t for_statement (m2c_parser_context_t p);

m2c_token_t statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("statement");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
  
    /* assignmentOrProcCall */
    case TOKEN_IDENTIFIER :
      lookahead = assignment_or_proc_call(p);
      break;
      
    /* | returnStatement */
    case TOKEN_RETURN :
      lookahead = return_statement(p);
      break;
      
    /* | withStatement */
    case TOKEN_WITH :
      lookahead = with_statement(p);
      break;
      
    /* | ifStatement */
    case TOKEN_IF :
      lookahead = if_statement(p);
      break;
      
    /* | caseStatement */
    case TOKEN_CASE :
      lookahead = case_statement(p);
      break;
      
    /* | loopStatement */
    case TOKEN_LOOP :
      lookahead = loop_statement(p);
      break;
      
    /* | whileStatement */
    case TOKEN_WHILE :
      lookahead = while_statement(p);
      break;
      
    /* | repeatStatement */
    case TOKEN_REPEAT :
      lookahead = repeat_statement(p);
      break;
      
    /* | forStatement */
    case TOKEN_FOR :
      lookahead = for_statement(p);
      break;
      
    /* | EXIT */
    case TOKEN_EXIT :
      lookahead = m2c_consume_sym(p->lexer);
      p->ast = m2c_ast_new_node(AST_EXIT, NULL);
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
    } /* end switch */
  
  return lookahead;
} /* end statement */


/* --------------------------------------------------------------------------
 * private function assignment_or_proc_call()
 * --------------------------------------------------------------------------
 * assignmentOrProcCall :=
 *   designator ( ':=' expression | actualParameters )?
 *   ;
 *
 * astnode:
 *  (ASSIGN designatorNode exprNode) | (PCALL designatorNode argsNode)
 * ----------------------------------------------------------------------- */

m2c_token_t designator (m2c_parser_context_t p);

m2c_token_t expression (m2c_parser_context_t p);

m2c_token_t actual_parameters (m2c_parser_context_t p);

m2c_token_t assignment_or_proc_call (m2c_parser_context_t p) {
  m2c_astnode_t desig;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("assignmentOrProcCall");
  
  /* designator */
  lookahead = designator(p);
  desig = p->ast;
  
  /* ( ':=' expression | actualParameters )? */
  if (lookahead == TOKEN_ASSIGN) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* expression */
    if (match_set(p, FIRST(EXPRESSION), FOLLOW(ASSIGNMENT_OR_PROC_CALL))) {
      lookahead = expression(p);
      p->ast = m2c_ast_new_node(AST_ASSIGN, desig, p->ast, NULL);
      /* astnode: (ASSIGN designatorNode exprNode) */
    } /* end if */
  }
  /* | actualParameters */
  else if (lookahead == TOKEN_LEFT_PAREN) {
    lookahead = actual_parameters(p);
    p->ast = m2c_ast_new_node(AST_PCALL, desig, p->ast, NULL);
    /* astnode: (PCALL designatorNode argsNode) */
  } /* end if */
  
  return lookahead;
} /* end assignment_or_proc_call */


/* --------------------------------------------------------------------------
 * private function actual_parameters()
 * --------------------------------------------------------------------------
 * actualParameters :=
 *   '(' ( expression ( ',' expression )* )? ')'
 *   ;
 *
 * astnode: (ARGS exprNode+) | (EMPTY)
 * ----------------------------------------------------------------------- */

m2c_token_t actual_parameters (m2c_parser_context_t p) {
  m2c_fifo_t tmplist;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("actualParameters");
  
  /* '(' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ( expression ( ',' expression )* )? */
  if (m2c_tokenset_member(FIRST(EXPRESSION), lookahead)) {
    /* expression */
    lookahead = expression(p);
    tmplist = m2c_fifo_new_queue(p->ast);
  
    /* ( ',' expression )* */
    while (lookahead == TOKEN_COMMA) {
      /* ',' */
      lookahead = m2c_consume_sym(p->lexer);
    
      /* expression */
      if (match_set(p, FIRST(EXPRESSION), FOLLOW(EXPRESSION))) {
        lookahead = expression(p);
        m2c_fifo_enqueue(tmplist, p->ast);
      } /* end if */
    } /* end while */
    
    p->ast = m2c_ast_new_list_node(AST_ARGS, tmplist);
    m2c_fifo_release(tmplist);
  }
  else /* no arguments */ {
    p->ast = m2c_ast_empty_node();
  } /* end if */
  
  /* ')' */
  if (match_token(p, TOKEN_RIGHT_PAREN, FOLLOW(ACTUAL_PARAMETERS))) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end if */
  
  return lookahead;
} /* end actual_parameters */


/* --------------------------------------------------------------------------
 * private function return_statement()
 * --------------------------------------------------------------------------
 * returnStatement :=
 *   RETURN expression?
 *   ;
 *
 * astnode: (RETURN exprNode) | (RETURN (EMPTY))
 * ----------------------------------------------------------------------- */

m2c_token_t return_statement (m2c_parser_context_t p) {
  m2c_astnode_t expr;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("returnStatement");
  
  /* RETURN */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* expression? */
  if (m2c_tokenset_element(FIRST(EXPRESSION), lookahead)) {
    lookahead = expression(p);
    expr = p->ast;
  }
  else {
    expr = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_RETURN, expr, NULL);
  
  return lookahead;
} /* end return_statement */


/* --------------------------------------------------------------------------
 * private function if_statement()
 * --------------------------------------------------------------------------
 * ifStatement :=
 *   IF boolExpression THEN statementSequence
 *   ( ELSIF boolExpression THEN statementSequence )*
 *   ( ELSE statementSequence )?
 *   END
 *   ;
 *
 * boolExpression := expression ;
 *
 * astnode: (IF exprNode ifBranchNode elsifSeqNode elseBranchNode)
 * ----------------------------------------------------------------------- */

m2c_token_t if_statement (m2c_parser_context_t p) {
  m2c_astnode_t ifexpr, ifseq, elif, expr, stmtseq, elifseq, elseseq;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("ifStatement");
  
  /* IF */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* boolExpression */
  if (match_set(p, FIRST(EXPRESSION), RESYNC(ELSIF_OR_ELSE_OR_END))) {
    lookahead = expression(p);
    ifexpr = p->ast;
    
    /* THEN */
    if (match_token(p, TOKEN_THEN, RESYNC(ELSIF_OR_ELSE_OR_END))) {
      lookahead = m2c_consume_sym(p->lexer);
      
      /* check for empty statement sequence */
      if ((m2c_tokenset_element(RESYNC(ELSIF_OR_ELSE_OR_END), lookahead))) {
    
          /* empty statement sequence warning */
          m2c_emit_warning_w_pos
            (M2C_EMPTY_STMT_SEQ,
             m2c_lexer_lookahead_line(p->lexer),
             m2c_lexer_lookahead_column(p->lexer));
          p->warning_count++;
      }
      /* statementSequence */
      else if (match_set(p, FIRST(STATEMENT_SEQUENCE),
          RESYNC(ELSIF_OR_ELSE_OR_END))) {
        lookahead = statement_sequence(p);
        ifseq = p->ast;
      }
      else /* resync */ {
        lookahead = m2c_next_sym(p->lexer);
      } /* end if */
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  } /* end if */
  
  /* ( ELSIF boolExpression THEN statementSequence )* */
  while (lookahead == TOKEN_ELSIF) {
    
    /* ELSIF */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* boolExpression */
    if (match_set(p, FIRST(EXPRESSION), RESYNC(ELSIF_OR_ELSE_OR_END))) {
      lookahead = expression(p);
      expr = p->ast;
    
      /* THEN */
      if (match_token(p, TOKEN_THEN, RESYNC(ELSIF_OR_ELSE_OR_END))) {
        lookahead = m2c_consume_sym(p->lexer);
      
        /* check for empty statement sequence */
        if ((m2c_tokenset_element
            (RESYNC(ELSIF_OR_ELSE_OR_END), lookahead))) {
    
            /* empty statement sequence warning */
            m2c_emit_warning_w_pos
              (M2C_EMPTY_STMT_SEQ,
               m2c_lexer_lookahead_line(p->lexer),
               m2c_lexer_lookahead_column(p->lexer));
            p->warning_count++;
        }
        /* statementSequence */
        else if (match_set(p, FIRST(STATEMENT_SEQUENCE),
            RESYNC(ELSIF_OR_ELSE_OR_END))) {
          lookahead = statement_sequence(p);
          stmtseq = p->ast;
          
          elif = m2c_ast_new_node(AST_ELSIF, expr, stmtseq, NULL);
          m2c_fifo_enqueue(tmplist, elif);
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
  } /* end while */
  
  if (m2c_fifo_entry_count(tmplist) > 0) {
    elifseq = m2c_ast_new_list_node(ELSIFSEQ, tmplist);
  }
  else /* no ELSIF branches */ {
    elifseq = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(tmplist);
  
  /* ( ELSE statementSequence )? */
  if (lookahead == TOKEN_ELSE) {
  
    /* ELSE */
    lookahead = m2c_consume_sym(p->lexer);
  
    /* check for empty statement sequence */
    if (lookahead == TOKEN_END) {
  
        /* empty statement sequence warning */
        m2c_emit_warning_w_pos
          (M2C_EMPTY_STMT_SEQ,
           m2c_lexer_lookahead_line(p->lexer),
           m2c_lexer_lookahead_column(p->lexer));
        p->warning_count++;
    }
    /* statementSequence */
    else if (match_set(p, FIRST(STATEMENT_SEQUENCE), FOLLOW(IF_STATEMENT))) {
      lookahead = statement_sequence(p);
      elseseq = p->ast;
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  }
  else {
    elseseq = m2c_ast_empty_node();
  } /* end if */
  
  /* END */
  if (match_token(p, TOKEN_END, FOLLOW(IF_STATEMENT))) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IF, ifexpr, ifseq, elifseq, elseseq, NULL);
  
  return lookahead;
} /* end if_statement */


/* --------------------------------------------------------------------------
 * private function case_statement()
 * --------------------------------------------------------------------------
 * caseStatement :=
 *   CASE expression OF case ( '|' case )*
 *   ( ELSE statementSequence )?
 *   END
 *   ;
 *
 * NB: 'case' is a reserved word in C, we use case_branch() here instead
 *
 * astnode: (SWITCH exprNode (CASELIST caseBranchNode+) elseBranchNode)
 * ----------------------------------------------------------------------- */

m2c_token_t case_branch (m2c_parser_context_t p);

m2c_token_t case_statement (m2c_parser_context_t p) {
  m2c_astnode_t expr, caselist, elseseq;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("caseStatement");
  
  /* CASE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* expression */
  if (match_set(p, FIRST(EXPRESSION), RESYNC(ELSE_OR_END))) {
    lookahead = expression(p);
    expr = p->ast;
    
    /* OF */
    if (match_token(p, TOKEN_OF, RESYNC(ELSE_OR_END))) {
      lookahead = m2c_consume_sym(p->lexer);
      
      /* case */
      if (match_set(p, FIRST(CASE), RESYNC(ELSE_OR_END))) {
        lookahead = case_branch(p);
        tmplist = m2c_fifo_new_queue(p->ast);
        
        /* ( '| case )* */
        while (lookahead == TOKEN_BAR) {
          /* '|' */
          lookahead = m2c_consume_sym(p->lexer);
          
          /* case */
          if (match_set(p, FIRST(CASE), RESYNC(ELSE_OR_END))) {
            lookahead = case_branch(p);
            m2c_fifo_enqueue(tmplist, p->ast);
          }
          else /* resync */ {
            lookahead = m2c_next_sym(p->lexer);
          } /* end if */
        } /* end while */
      }
      else /* resync */ {
        lookahead = m2c_next_sym(p->lexer);
      } /* end if */
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  } /* end if */
  
  caselist = m2c_ast_new_list_node(AST_CASELIST, tmplist);
  m2c_fifo_release(tmplist);
  
  /* ( ELSE statementSequence )? */
  if (lookahead == TOKEN_ELSE) {
  
    /* ELSE */
    lookahead = m2c_consume_sym(p->lexer);
  
    /* check for empty statement sequence */
    if (lookahead == TOKEN_END) {
  
        /* empty statement sequence warning */
        m2c_emit_warning_w_pos
          (M2C_EMPTY_STMT_SEQ,
           m2c_lexer_lookahead_line(p->lexer),
           m2c_lexer_lookahead_column(p->lexer));
        p->warning_count++;
    }
    /* statementSequence */
    else if
      (match_set(p, FIRST(STATEMENT_SEQUENCE), FOLLOW(CASE_STATEMENT))) {
      lookahead = statement_sequence(p);
      else_branch = p->ast;
    }
    else /* resync */ {
      lookahead = m2c_next_sym(p->lexer);
    } /* end if */
  }
  else /* no ELSE branch */ {
    elseseq = m2c_ast_empty_node();
  } /* end if */
  
  /* END */
  if (match_token(p, TOKEN_END, FOLLOW(CASE_STATEMENT))) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_SWITCH, expr, caselist, elseseq, NULL);
  
  return lookahead;
} /* end case_statement */


/* --------------------------------------------------------------------------
 * private function case_branch()
 * --------------------------------------------------------------------------
 * case :=
 *   caseLabelList ':' statementSequence
 *   ;
 *
 * NB: 'case' is a reserved word in C, we use case_branch() here instead
 *
 * astnode: (CASE caseLabelListNode statementSeqNode)
 * ----------------------------------------------------------------------- */

m2c_token_t case_branch (m2c_parser_context_t p) {
  m2c_astnode_t cllist, stmtseq;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("case");
  
  /* caseLabelList */
  lookahead = case_label_list(p);
  cllist = p->ast;
  
  /* ':' */
  if (match_token(p, TOKEN_COLON, FOLLOW(CASE))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* check for empty statement sequence */
    if ((m2c_tokenset_element(FOLLOW(CASE), lookahead))) {
  
        /* empty statement sequence warning */
        m2c_emit_warning_w_pos
          (M2C_EMPTY_STMT_SEQ,
           m2c_lexer_lookahead_line(p->lexer),
           m2c_lexer_lookahead_column(p->lexer));
        p->warning_count++;
    }
    /* statementSequence */
    else if (match_set(p, FIRST(STATEMENT_SEQUENCE), FOLLOW(CASE))) {
      lookahead = statement_sequence(p);
      stmtseq = p->ast;
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_CASE, cllist, stmtseq, NULL);
  
  return lookahead;
} /* end case_branch */


/* --------------------------------------------------------------------------
 * private function loop_statement()
 * --------------------------------------------------------------------------
 * loopStatement :=
 *   LOOP statementSequence END
 *   ;
 *
 * astnode: (LOOP statementSeqNode)
 * ----------------------------------------------------------------------- */

m2c_token_t loop_statement (m2c_parser_context_t p) {
  m2c_astnode_t stmtseq;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("loopStatement");
  
  /* LOOP */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* check for empty statement sequence */
  if (lookahead == TOKEN_END) {

    /* empty statement sequence warning */
    m2c_emit_warning_w_pos
      (M2C_EMPTY_STMT_SEQ,
       m2c_lexer_lookahead_line(p->lexer),
       m2c_lexer_lookahead_column(p->lexer));
    p->warning_count++;
         
    /* END */
    lookahead = m2c_consume_sym(p->lexer);
  }
  /* statementSequence */
  else if (match_set(p, FIRST(STATEMENT_SEQUENCE), FOLLOW(LOOP_STATEMENT))) {
    lookahead = statement_sequence(p);
    stmtseq = p->ast;
    
    /* END */
    if (match_token(p, TOKEN_END, FOLLOW(LOOP_STATEMENT))) {
      lookahead = m2c_consume_sym(p->lexer);
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_LOOP, stmtseq, NULL);
  
  return lookahead;
} /* end loop_statement */


/* --------------------------------------------------------------------------
 * private function while_statement()
 * --------------------------------------------------------------------------
 * whileStatement :=
 *   WHILE boolExpression DO statementSequence END
 *   ;
 *
 * boolExpression := expression ;
 *
 * astnode: (WHILE exprNode statementSeqNode)
 * ----------------------------------------------------------------------- */

m2c_token_t while_statement (m2c_parser_context_t p) {
  m2c_astnode_t expr, stmtseq;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("whileStatement");
  
  /* WHILE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* boolExpression */
  if (match_set(p, FIRST(EXPRESSION), FOLLOW(WHILE_STATEMENT))) {
    lookahead = expression(p);
    expr = p->ast;
    
    /* DO */
    if (match_token(p, TOKEN_DO, FOLLOW(WHILE_STATEMENT))) {
      lookahead = m2c_consume_sym(p->lexer);
      
      /* check for empty statement sequence */
      if (lookahead == TOKEN_END) {

        /* empty statement sequence warning */
        m2c_emit_warning_w_pos
          (M2C_EMPTY_STMT_SEQ,
           m2c_lexer_lookahead_line(p->lexer),
           m2c_lexer_lookahead_column(p->lexer));
        p->warning_count++;
         
        /* END */
        lookahead = m2c_consume_sym(p->lexer);
      }
      /* statementSequence */
      else if
        (match_set(p, FIRST(STATEMENT_SEQUENCE), FOLLOW(WHILE_STATEMENT))) {
        lookahead = statement_sequence(p);
        stmtseq = p->ast;
    
        /* END */
        if (match_token(p, TOKEN_END, FOLLOW(WHILE_STATEMENT))) {
          lookahead = m2c_consume_sym(p->lexer);
        } /* end if */
      } /* end if */
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_WHILE, expr, stmtseq, NULL);
  
  return lookahead;
} /* end while_statement */


/* --------------------------------------------------------------------------
 * private function repeat_statement()
 * --------------------------------------------------------------------------
 * repeatStatement :=
 *   REPEAT statementSequence UNTIL boolExpression
 *   ;
 *
 * boolExpression := expression ;
 *
 * astnode: (REPEAT statementSeqNode exprNode)
 * ----------------------------------------------------------------------- */

m2c_token_t repeat_statement (m2c_parser_context_t p) {
  m2c_astnode_t stmtseq, expr;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("repeatStatement");
  
  /* REPEAT */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* check for empty statement sequence */
  if (lookahead == TOKEN_UNTIL) {
  
    /* empty statement sequence warning */
    m2c_emit_warning_w_pos
      (M2C_EMPTY_STMT_SEQ,
       m2c_lexer_lookahead_line(p->lexer),
       m2c_lexer_lookahead_column(p->lexer));
    p->warning_count++;
  }
  /* statementSequence */
  else if (match_set(p,
           FIRST(STATEMENT_SEQUENCE), FOLLOW(STATEMENT_SEQUENCE))) {
    lookahead = statement_sequence(p);
    stmtseq = p->ast;
  }
  else /* resync */ {
    lookahead = m2c_next_sym(p->lexer);
  } /* end if */
    
  /* UNTIL */
  if (match_token(p, TOKEN_UNTIL, FOLLOW(REPEAT_STATEMENT))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* boolExpression */
    if (match_set(p, FIRST(EXPRESSION), FOLLOW(REPEAT_STATEMENT))) {
      lookahead = expression(p);
      expr = p->ast;
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_REPEAT, stmtseq, expr, NULL);
  
  return lookahead;
} /* end repeat_statement */


/* --------------------------------------------------------------------------
 * private function for_statement()
 * --------------------------------------------------------------------------
 * forStatement :=
 *   FOR forLoopVariant ':=' startValue TO endValue
 *   ( BY stepValue )? DO statementSequence END
 *   ;
 *
 * forLoopVariant := Ident ;
 *
 * startValue, endValue := ordinalExpression ;
 *
 * ordinalExpression := expression
 *
 * stepValue := constExpression
 *
 * astnode: (FORTO identNode exprNode exprNode exprNode statementSeqNode)
 * ----------------------------------------------------------------------- */

m2c_token_t for_statement (m2c_parser_context_t p) {
  m2c_astnode_t id, start, end, step, stmtseq;
  m2c_string_t ident;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("forStatement");
  
  /* FOR */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* forLoopVariant */
  if (match_token(p, TOKEN_IDENTIFIER, RESYNC(FOR_LOOP_BODY))) {
    lookahead = m2c_consume_sym(p->lexer);
    ident = m2c_lexer_current_lexeme(p->lexer);
    id = m2c_ast_new_terminal_node(AST_IDENT, ident);
    
    /* ':=' */
    if (match_token(p, TOKEN_ASSIGN, RESYNC(FOR_LOOP_BODY))) {
      lookahead = m2c_consume_sym(p->lexer);
      
      /* startValue */
      if (match_set(p, FIRST(EXPRESSION), RESYNC(FOR_LOOP_BODY))) {
        lookahead = expression(p);
        start = p->ast;
        
        /* TO */
        if (match_token(p, TOKEN_TO, RESYNC(FOR_LOOP_BODY))) {
          lookahead = m2c_consume_sym(p->lexer);
          
          /* endValue */
          if (match_set(p, FIRST(EXPRESSION), RESYNC(FOR_LOOP_BODY))) {
            lookahead = expression(p);
            end = p->ast;
            
            /* ( BY stepValue )? */
            if (lookahead == TOKEN_BY) {
              lookahead = m2c_consume_sym(p->lexer);
              
              if (match_set(p, FIRST(EXPRESSION), RESYNC(FOR_LOOP_BODY))) {
                lookahead = const_expression(p);
                step = p->ast;
              } /* end if */
            }
            else /* no step value */ {
              step = m2c_ast_empty_node();
            } /* end if */
          } /* end if */
        } /* end if */
      } /* end if */
    } /* end if */
  } /* end if */
  
  /* resync */
  lookahead = m2c_next_sym(p->lexer);
  
  /* DO -- The FOR loop body */
  if (match_token(p, TOKEN_DO, FOLLOW(FOR_STATEMENT))) {
    lookahead = m2c_consume_sym(p->lexer);
    
    /* check for empty statement sequence */
    if (lookahead == TOKEN_END) {

      /* empty statement sequence warning */
      m2c_emit_warning_w_pos
        (M2C_EMPTY_STMT_SEQ,
         m2c_lexer_lookahead_line(p->lexer),
         m2c_lexer_lookahead_column(p->lexer));
      p->warning_count++;
       
      /* END */
      lookahead = m2c_consume_sym(p->lexer);
    }
    /* statementSequence */
    else if
      (match_set(p, FIRST(STATEMENT_SEQUENCE), FOLLOW(FOR_STATEMENT))) {
      lookahead = statement_sequence(p);
      stmtseq = p->ast;
  
      /* END */
      if (match_token(p, TOKEN_END, FOLLOW(FOR_STATEMENT))) {
        lookahead = m2c_consume_sym(p->lexer);
      } /* end if */
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_FORTO, id, start, end, step, stmtseq, NULL);
  
  return lookahead;
} /* end for_statement */


/* --------------------------------------------------------------------------
 * private function designator()
 * --------------------------------------------------------------------------
 * designator :=
 *   qualident ( '^' | selector )*
 *   ;
 *
 * astnode: identNode | (DEREF expr) | (DESIG headNode tailNode)
 * ----------------------------------------------------------------------- */

m2c_token_t selector (m2c_parser_context_t p);

m2c_token_t designator (m2c_parser_context_t p) {
  m2c_astnode_t head;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("designator");
  
  /* qualident */
  lookahead = qualident(p);
  /* astnode: (IDENT ident) | (QUALIDENT q0 q1 q2 ... qN ident) */
  
  /* ( '^' | selector )* */
  while ((lookahead == TOKEN_DEREF) ||
         (lookahead == TOKEN_PERIOD) ||
         (lookahead == TOKEN_LEFT_BRACKET)) {
    head = p->ast;
    
    if /* '^' */ (lookahead == TOKEN_DEREF) {
      lookahead = m2c_consume_sym(p->lexer);
      p->ast = m2c_ast_new_node(AST_DEREF, head, NULL);
      /* astnode: (DEREF expr) */
    }
    else /* selector */ {
      lookahead = selector(p);
      p->ast = m2c_ast_new_node(AST_DESIG, head, p->ast, NULL);
      /* astnode: (DESIG headNode tailNode) */
    } /* end if */
  } /* end if */
  
  return lookahead;
} /* end designator */


/* --------------------------------------------------------------------------
 * private function selector()
 * --------------------------------------------------------------------------
 * selector :=
 *   '.' Ident | '[' indexList ']'
 *   ;
 *
 * astnode: (FIELD identNode) | (INDEX exprNode+)
 * ----------------------------------------------------------------------- */

m2c_token_t index_list (m2c_parser_context_t p);

m2c_token_t selector (m2c_parser_context_t p) {
  m2c_string_t ident;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("selector");
  
  lookahead = m2c_next_sym(p->lexer);
      
  switch (lookahead) {
    
    /* '.' Ident */
    case TOKEN_PERIOD :
      /* '.' */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* Ident */
      if (match_token(p, TOKEN_IDENTIFIER, FOLLOW(SELECTOR))) {
        lookahead = m2c_consume_sym(p->lexer);
        ident = m2c_lexer_current_lexeme(p->lexer);
        id = m2c_ast_new_terminal_node(AST_IDENT, ident);
        p->ast = m2c_ast_new_node(AST_FIELD, id, NULL);
        /* astnode: (FIELD identNode) */
      } /* end if */
      break;
      
    /* '[' */
    case TOKEN_LEFT_BRACKET :
      /* '[' */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* indexList ']' */
      if (match_set(p, FIRST(EXPRESSION), FOLLOW(SELECTOR))) {
      
        /* indexList */
        lookahead = index_list(p);
        /* astnode: (INDEX exprNode+) */
        
        /* ']' */
        if (match_token(p, TOKEN_RIGHT_BRACKET, FOLLOW(SELECTOR))) {
          lookahead = m2c_consume_sym(p->lexer);
        } /* end if */
      } /* end if */
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
  } /* end switch */
      
  return lookahead;
} /* end selector */


/* --------------------------------------------------------------------------
 * private function index_list()
 * --------------------------------------------------------------------------
 * indexList :=
 *   expression ( ',' expression )*
 *   ;
 *
 * astnode: (INDEX exprNode+)
 * ----------------------------------------------------------------------- */

m2c_token_t index_list (m2c_parser_context_t p) {
  m2c_fifo_t tmplist;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("expressionList");
  
  /* expression */
  lookahead = expression(p);
  tmplist = m2c_fifo_new_queue(p->ast);
  
  /* ( ',' expression )* */
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* expression */
    if (match_set(p, FIRST(EXPRESSION), FOLLOW(EXPRESSION))) {
      lookahead = expression(p);
      m2c_fifo_enqueue(tmplist, p->ast);
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_list_node(AST_INDEX, tmplist);
  m2c_fifo_release(tmplist);
  
  return lookahead;
} /* end index_list */


/* --------------------------------------------------------------------------
 * private function expression()
 * --------------------------------------------------------------------------
 * expression :=
 *   simpleExpression ( operL1 simpleExpression )?
 *   ;
 *
 * operL1 := '=' | '#' | '<' | '<=' | '>' | '>=' | IN ;
 *
 * astnode:
 *  (EQ expr expr) | (NEQ expr expr) | (LT expr expr) | (LTEQ expr expr) |
 *  (GT expr expr) | (GTEQ expr expr) | (IN expr expr) | simpleExprNode
 * ----------------------------------------------------------------------- */

#define IS_LEVEL1_OPERATOR(_t) \
  (((_t) == TOKEN_EQUAL) || ((_t) == TOKEN_NOTEQUAL) || \
   ((_t) == TOKEN_LESS) || ((_t) == TOKEN_LESS_EQUAL) || \
   ((_t) == TOKEN_GREATER) || ((_t) == TOKEN_GREATER_EQUAL) || \
   ((_t) == TOKEN_IN))

m2c_token_t simple_expression (m2c_parser_context_t p);

m2c_token_t expression (m2c_parser_context_t p) {
  m2c_ast_nodetype_t nodetype;
  m2c_astnode_t left;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("expression");
  
  /* simpleExpression */
  lookahead = simple_expression(p);
  
  /* ( operL1 simpleExpression )? */
  if (IS_LEVEL1_OPERATOR(lookahead)) {
    left = p->ast;
    
    switch (lookahead) {
      case TOKEN_IN :
        nodetype = AST_IN;
        break;
      
      case TOKEN_EQUAL :
        nodetype = AST_EQ;
        break;
      
      case TOKEN_NOTEQUAL :
        nodetype = AST_NEQ;
        break;
      
      case TOKEN_LESS :
        nodetype = AST_LT;
        break;
      
      case TOKEN_LESS_EQUAL :
        nodetype = AST_LTEQ;
        break;
      
      case TOKEN_GREATER :
        nodetype = AST_GT;
        break;
      
      case TOKEN_GREATER_EQUAL :
        nodetype = AST_GTEQ;
        break;
    } /* end switch */
    
    lookahead = m2c_consume_sym(p->lexer);
    
    /* simpleExpression */
    if (match_set(p, FIRST(EXPRESSION), FOLLOW(SIMPLE_EXPRESSION))) {
      lookahead = simple_expression(p);
      p->ast = m2c_ast_new_node(nodetype, left, p->ast, NULL);
    } /* end if */
  } /* end if */
  
  return lookahead;
} /* end expression */


/* --------------------------------------------------------------------------
 * private function simple_expression()
 * --------------------------------------------------------------------------
 * simpleExpression :=
 *   ( '+' | '-' )? term ( operL2 term )*
 *   ;
 *
 * operL2 := '+' | '-' | OR ;
 *
 * astnode:
 *  (NEG expr) |
 *  (PLUS expr expr) | (MINUS expr expr) | (OR expr expr) | termNode
 * ----------------------------------------------------------------------- */

#define IS_LEVEL2_OPERATOR(_t) \
  (((_t) == TOKEN_PLUS) || ((_t) == TOKEN_MINUS) || ((_t) == TOKEN_OR))

m2c_token_t term (m2c_parser_context_t p);

m2c_token_t simple_expression (m2c_parser_context_t p) {
  m2c_ast_nodetype_t nodetype;
  m2c_astnode_t left;
  m2c_token_t lookahead;
  bool unary_minus = false;
  
  PARSER_DEBUG_INFO("simpleExpression");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* ( '+' | '-' )? */
  if (lookahead == TOKEN_PLUS) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else if (lookahead == TOKEN_MINUS) {
    lookahead = m2c_consume_sym(p->lexer);
    unary_minus = true;
  } /* end if */
  
  /* term */
  if (match_set(p, FIRST(TERM), FOLLOW(TERM))) {
    lookahead = term(p);
    
    if (unary_minus) {
      p->ast = m2c_ast_new_node(AST_NEG, p->ast, NULL);
    } /* end if */
  
    /* ( operL2 term )* */
    while (IS_LEVEL2_OPERATOR(lookahead)) {
      left = p->ast;
      
      /* operL2 */
      switch (lookahead) {
        case TOKEN_OR :
          nodetype = AST_OR;
          break;
      
        case TOKEN_PLUS :
          nodetype = AST_PLUS;
          break;
      
        case TOKEN_MINUS :
          nodetype = AST_MINUS;
          break;
      } /* end switch */
      
      lookahead = m2c_consume_sym(p->lexer);
    
      /* term */
      if (match_set(p, FIRST(TERM), FOLLOW(TERM))) {
        lookahead = term(p);        
        p->ast = m2c_ast_new_node(nodetype, left, p->ast, NULL);
      } /* end if */
    } /* end while */
  } /* end if */
    
  return lookahead;
} /* end simple_expression */


/* --------------------------------------------------------------------------
 * private function term()
 * --------------------------------------------------------------------------
 * term :=
 *   simpleTerm ( operL3 simpleTerm )*
 *   ;
 *
 * operL3 := '*' | '/' | DIV | MOD | AND ;
 *
 * astnode:
 *  (ASTERISK expr expr) | (SOLIDUS expr expr) |
 *  (DIV expr expr) | (MOD expr expr) | (AND expr expr) | simpleTermNode
 * ----------------------------------------------------------------------- */

#define IS_LEVEL3_OPERATOR(_t) \
  (((_t) == TOKEN_ASTERISK) || ((_t) == TOKEN_SOLIDUS) || \
   ((_t) == TOKEN_DIV) || ((_t) == TOKEN_MOD) || ((_t) == TOKEN_AND))

m2c_token_t simple_term (m2c_parser_context_t p);

m2c_token_t term (m2c_parser_context_t p) {
  m2c_ast_nodetype_t operation;
  m2c_astnode_t left, right;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("term");
  
  /* simpleTerm */
  lookahead = simple_term(p);
  left = p->ast;
  
  /* ( operL3 simpleTerm )* */
  while (IS_LEVEL3_OPERATOR(lookahead)) {
    left = p->ast;
    
    /* operL3 */
    switch (lookahead) {
      case TOKEN_AND :
        nodetype = AST_AND;
        break;
    
      case TOKEN_DIV :
        nodetype = AST_DIV;
        break;
    
      case TOKEN_MOD :
        nodetype = AST_MOD;
        break;
    
      case TOKEN_ASTERISK :
        nodetype = AST_ASTERISK;
        break;
    
      case TOKEN_SOLIDUS :
        nodetype = AST_SOLIDUS;
        break;
    } /* end switch */
      
    lookahead = m2c_consume_sym(p->lexer);
    
    /* simpleTerm */
    if (match_set(p, FIRST(SIMPLE_TERM), FOLLOW(SIMPLE_TERM))) {
      lookahead = simple_term(p);      
      p->ast = m2c_ast_new_node(nodetype, left, p->ast, NULL);
    } /* end if */
  } /* end while */
  
  return lookahead;
} /* end term */


/* --------------------------------------------------------------------------
 * private function simple_term()
 * --------------------------------------------------------------------------
 * simpleTerm :=
 *   NOT? factor
 *   ;
 *
 * astnode: (NOT expr) | factorNode
 * ----------------------------------------------------------------------- */

m2c_token_t factor (m2c_parser_context_t p);

m2c_token_t simple_term (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  bool negation = false;
  
  PARSER_DEBUG_INFO("simpleTerm");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* NOT? */
  if (lookahead == TOKEN_NOT) {
    lookahead = m2c_consume_sym(p->lexer);
    negation = true;
  } /* end if */
  
  /* factor */
  if (match_set(p, FIRST(FACTOR), FOLLOW(FACTOR))) {
    lookahead = factor(p);
    
    if (negation) {
      p->ast = m2c_ast_new_node(AST_NOT, p->ast, NULL);
    } /* end if */
  } /* end if */

  return lookahead;
} /* end simple_term */


/* --------------------------------------------------------------------------
 * private function factor()
 * --------------------------------------------------------------------------
 * factor :=
 *   NumberLiteral | StringLiteral | setValue |
 *   designatorOrFuncCall | '(' expression ')'
 *   ;
 *
 * astnode:
 *  (INTVAL value) | (REALVAL value) | (CHRVAL value) | (QUOTEDVAL value) |
 *  setValNode | designatorNode | funcCallNode | exprNode
 * ----------------------------------------------------------------------- */

m2c_token_t set_value (m2c_parser_context_t p);

m2c_token_t designator_or_func_call (m2c_parser_context_t p);

m2c_token_t factor (m2c_parser_context_t p) {
  m2c_string_t lexeme;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("factor");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
  
    /* NumberLiteral */
    case TOKEN_INTEGER :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_lexer_current_lexeme(p->lexer);
      p->ast = m2c_ast_new_terminal_node(AST_INTVAL, lexeme);
      break;
      
    case TOKEN_REAL :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_lexer_current_lexeme(p->lexer);
      p->ast = m2c_ast_new_terminal_node(AST_REALVAL, lexeme);
      break;
      
    case TOKEN_CHAR :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_lexer_current_lexeme(p->lexer);
      p->ast = m2c_ast_new_terminal_node(AST_CHRVAL, lexeme);
      break;
          
    /* | StringLiteral */
    case TOKEN_STRING :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_lexer_current_lexeme(p->lexer);
      p->ast = m2c_ast_new_terminal_node(AST_QUOTEDVAL, lexeme);
      break;
      
    /* | setValue */
    case TOKEN_LEFT_BRACE :
      lookahead = set_value(p);
      break;
      
    /* | designatorOrFuncCall */
    case TOKEN_IDENTIFIER :
      lookahead = designator_or_func_call(p);
      break;
      
    /* | '(' expression ')' */
    case TOKEN_LEFT_PAREN :
      lookahead = m2c_consume_sym(p->lexer);
      
      /* expression */
      if (match_set(p, FIRST(EXPRESSION), FOLLOW(FACTOR))) {
        lookahead = expression(p);
        
        /* ')' */
        if (match_token(p, TOKEN_RIGHT_PAREN, FOLLOW(FACTOR))) {
          lookahead = m2c_consume_sym(p->lexer);
        } /* end if */
      } /* end if */
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
  } /* end switch */
  
  return lookahead;
} /* end factor */


/* --------------------------------------------------------------------------
 * private function designator_or_func_call()
 * --------------------------------------------------------------------------
 * designatorOrFuncCall :=
 *   designator ( setValue | actualParameters )?
 *   ;
 *
 * astnode:
 *  (SETVAL designatorNode elemListNode) | (FCALL designatorNode argsNode)
 * ----------------------------------------------------------------------- */

m2c_token_t designator_or_func_call (m2c_parser_context_t p) {
  m2c_astnode_t desig;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("designatorOrFuncCall");
  
  /* designator */
  lookahead = designator(p);
  desig = p->ast;
  
  /* setValue */
  if (lookahead == TOKEN_LEFT_BRACE) {
    lookahead = set_value(p);
    /* TO DO: check designator for IDENT or QUALIDENT */
    p->ast = m2c_ast_replace_subnode(p->ast, 0, desig);
    /* astnode: (SETVAL designatorNode elemListNode) */
  }
  /* actualParameters */
  else if (lookahead == TOKEN_LEFT_PAREN) {
    lookahead = actual_parameters(p);
    p->ast = m2c_ast_new_node(AST_FCALL, desig, p->ast, NULL);
    /* astnode: (FCALL designatorNode argsNode) */
    
    if (match_token(p, TOKEN_RIGHT_PAREN, FOLLOW(DESIGNATOR_OR_FUNC_CALL))) {
      lookahead = m2c_consume_sym(p->lexer);
    } /* end if */
  } /* end if */
    
  return lookahead;
} /* end designator_or_func_call */


/* --------------------------------------------------------------------------
 * private function set_value()
 * --------------------------------------------------------------------------
 * setValue :=
 *   '{' element ( ',' element )* '}'
 *   ;
 *
 * astnode: (SETVAL (EMPTY) elemListNode)
 * ----------------------------------------------------------------------- */

m2c_token_t element (m2c_parser_context_t p);

m2c_token_t set_value (m2c_parser_context_t p) {
  m2c_astnode_t empty, elemlist;
  m2c_ast_fifo_t tmplist;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("setValue");
  
  /* '{' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* element */
  if (match_set(p, FIRST(ELEMENT), FOLLOW(SET_VALUE))) {
    lookahead = element(p);
    tmplist = m2c_fifo_new_queue(p->ast);
    
    /* ( ',' element )* */
    while (lookahead == TOKEN_COMMA) {
      /* ',' */
      lookahead = m2c_consume_sym(p->lexer);
    
      /* element */
      if (match_set(p, FIRST(ELEMENT), FOLLOW(SET_VALUE))) {
        lookahead = element(p);
        m2c_fifo_enqueue(tmplist, p->ast);
      }
      else /* resync */ {
        lookahead = m2c_next_sym(p->lexer);
      } /* end if */
    } /* end while */
    
    /* '}' */
    if (match_token(p, TOKEN_RIGHT_BRACE, FOLLOW(SET_VALUE))) {
      lookahead = m2c_consume_sym(p->lexer);
    } /* end if */
  } /* end if */
  
  if (m2c_fifo_entry_count(tmplist) > 0) {
    elemlist = m2c_ast_new_list_node(AST_ELEMLIST, tmplist);
  }
  else /* empty set */ {
    elemlist = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(tmplist);
  
  /* build AST node and pass it back in p->ast */
  empty = m2c_ast_empty_node();
  p->ast = m2c_ast_new_node(AST_SETVAL, empty, elemlist, NULL);
  
  return lookahead;
} /* end set_value */


/* --------------------------------------------------------------------------
 * private function element()
 * --------------------------------------------------------------------------
 * element :=
 *   expression ( '..' expression )?
 *   ;
 *
 * astnode: exprNode | (RANGE expr expr)
 * ----------------------------------------------------------------------- */

m2c_token_t element (m2c_parser_context_t p) {
  m2c_astnode_t lower;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("element");
  
  /* expression */
  lookahead = expression(p);
  
  /* ( '..' expression )? */
  if (lookahead == TOKEN_RANGE) {
    lower = p->ast;
    lookahead = m2c_consume_sym(p->lexer);
    
    /* expression */
    if (match_set(p, FIRST(EXPRESSION), FOLLOW(ELEMENT))) {
      lookahead = expression(p);
      p->ast = m2c_ast_new_node(AST_RANGE, lower, p->ast, NULL);
    } /* end if */
  } /* end if */
  
  return lookahead;
} /* end element */

/* END OF FILE */