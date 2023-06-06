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
#include "m2c-digest.h"
#include "m2c-tokenset.h"
#include "m2c-fileutils.h"
#include "m2c-production.h"
#include "m2c-first-sets.h"
#include "m2c-follow-sets.h"
#include "m2c-bindables.h"
#include "m2c-statistics.h"
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
  IFC_MODULE,   /* INTERFACE MODULE */
  IMP_MODULE,   /* IMPLEMENATION MODULE */
  PGM_MODULE    /* (*PROGRAM*) MODULE */
} m2c_module_context_t;


/* --------------------------------------------------------------------------
 * private type m2c_nonterminal_f
 * --------------------------------------------------------------------------
 * function pointer type for function to parse a non-terminal symbol.
 * ----------------------------------------------------------------------- */

typedef m2c_token_t (m2c_nonterminal_f) (m2c_parser_context_t);


/* --------------------------------------------------------------------------
 * private type deflist_context_t
 * --------------------------------------------------------------------------
 * Holds context for generic parse function definition_list().
 * ----------------------------------------------------------------------- */

typedef struct {
  m2c_nonterminal_f def_or_decl;
  m2c_production_t production;
  ast_node_type_t node_type;
} def_decl_context_t;


/* --------------------------------------------------------------------------
 * constDefinitionList context
 * ----------------------------------------------------------------------- */

static def_decl_context_t *const_def = {
  const_definition,   /* parse function */
  CONST_DEFINITION,   /* production rule */
  AST_CONSTDEFLIST    /* AST node type */
};

/* --------------------------------------------------------------------------
 * typeDefinitionList context
 * ----------------------------------------------------------------------- */

static def_decl_context_t *type_def = {
  type_definition,   /* parse function */
  TYPE_DEFINITION,   /* production rule */
  AST_TYPEDEFLIST    /* AST node type */
};

/* --------------------------------------------------------------------------
 * varDefinitionList context
 * ----------------------------------------------------------------------- */

static def_decl_context_t *var_def = {
  var_definition,   /* parse function */
  VAR_DEFINITION,   /* production rule */
  AST_VARDEFLIST    /* AST node type */
};

/* --------------------------------------------------------------------------
 * constDeclarationList context
 * ----------------------------------------------------------------------- */

static def_decl_context_t *const_decl = {
  const_declaration,   /* parse function */
  CONST_DECLARATION,   /* production rule */
  AST_CONSTDECLLIST    /* AST node type */
};

/* --------------------------------------------------------------------------
 * typeDeclarationList context
 * ----------------------------------------------------------------------- */

static def_decl_context_t *type_decl = {
  type_declaration,   /* parse function */
  TYPE_DECLARATION,   /* production rule */
  AST_TYPEDECLLIST    /* AST node type */
};

/* --------------------------------------------------------------------------
 * varDeclarationList context
 * ----------------------------------------------------------------------- */

static def_decl_context_t *var_decl = {
  var_declaration,   /* parse function */
  VAR_DECLARATION,   /* production rule */
  AST_VARDECLLIST    /* AST node type */
};


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
  /* filename */           const char *filename;
  /* basename */           const char *basename;
  /* suffix */             const char *suffix;
  /* lexer */              m2c_lexer_t lexer;
  /* stats */              m2c_stats_t stats;
  /* ast */                m2c_astnode_t ast;
  /* module_context */     m2c_module_context_t module_context;
  /* status */             m2c_parser_status_t status;
};

typedef struct m2c_parser_context_s m2c_parser_context_s;


/* --------------------------------------------------------------------------
 * function m2c_parse_file(srcpath, stats, status)
 * --------------------------------------------------------------------------
 * Parses the Modula-2 source file represented by srcpath, builds an abstract
 * syntax tree (AST) and returns it.  Returns an incomplete AST if errors are
 * encountered, or an empty AST if the source file cannot be found or opened,
 * or NULL if memory allocation failed.  Prints warnings and errors to stderr
 * and passes statistics in stats.  Passes the status in status.
 * ----------------------------------------------------------------------- */
 
m2c_ast_t m2c_parse_file
  (const char *srcpath,           /* in */
   m2c_stats_t *stats,            /* out */
   m2c_parser_status_t *status)   /* out */ {
   
  const char *filename;
  const char *basename;
  const char *suffix;
  m2c_parser_context_t p;
  m2c_astnode_t ast;
  
  if (srcpath == NULL) {
    SET_STATUS(status, M2C_PARSER_STATUS_INVALID_REFERENCE);
    return m2c_ast_empty_node();
  } /* end if */
  
  if (is_valid_pathname(srcpath) == false) {
    SET_STATUS(status, M2C_PARSER_STATUS_INVALID_PATHNAME);
    return m2c_ast_empty_node();
  } /* end if */
      
  /* set up parser context */
  p = malloc(sizeof(m2c_parser_context_s));
  
  if (p == NULL) {
    SET_STATUS(status, M2C_PARSER_STATUS_ALLOCATION_FAILED);
    return NULL;
  } /* end if */
  
  /* create lexer object */
  m2c_new_lexer(&(p->lexer), srcpath, NULL);
  
  if (p->lexer == NULL) {
    SET_STATUS(status, M2C_PARSER_STATUS_ALLOCATION_FAILED);
    free(p);
    return NULL;
  } /* end if */
  
  /* create new statistics object */
  p->stats = m2c_stats_new();
  
  if (p->stats == NULL) {
    SET_STATUS(status, M2C_PARSER_STATUS_ALLOCATION_FAILED);
    free(p->lexer);
    free(p);
    return NULL;
  } /* end if */
  
  split_pathname(srcpath, NULL, &filename, NULL);
  split_filename(filename, &basename, &suffix, NULL);
  
  /* init parser context */
  p->filename = filename;
  p->basename = basename;
  p->suffix = suffix;
  p->module_context = 0;
  p->ast = NULL;
  p->status = 0;
    
  /* parse and build AST */
  parse_start_symbol(p);
  ast = p->ast;
  
  /* update statistics line counter */
  m2c_stats_set_line_count(p->stats, m2c_lexer_current_line(p->lexer));
  
  /* pass back statistics and return status */
  *stats = p->stats;
  SET_STATUS(status, p->status);
  
  /* clean up and return */
  m2c_stats_release(p->stats);
  m2c_release_lexer(p->lexer);
  free(p);
  
  return ast;
} /* end m2c_parse_file */


/* --------------------------------------------------------------------------
 * private function match_token(p, expected_token, resync_set)
 * --------------------------------------------------------------------------
 * Matches  the lookahead symbol  to expected_token  and returns true if they
 * match.  Reports  a syntax error,  increments the error count  and  returns 
 * false if they do not match.
 * ----------------------------------------------------------------------- */

static bool match_token
  (m2c_parser_context_t p, m2c_token_t expected_token) {
  
  intstr_t lexeme;
  const char *lexstr;
  m2c_token_t lookahead;
  unsigned short line, column;
  
  lookahead = m2c_next_sym(p->lexer);
  
  if (lookahead == expected_token) {
    return true;
  }
  else /* no match */ {
    /* get details for offending lookahead symbol */
    line = m2c_lexer_lookahead_line(p->lexer);
    column = m2c_lexer_lookahead_column(p->lexer);
    lexeme = m2c_lexer_lookahead_lexeme(p->lexer);
    lexstr = intstr_char_ptr(lexeme);
    
    /* report error */
    m2c_emit_syntax_error_w_token
      (line, column, lookahead, lexstr, expected_token);
    
    /* print source line */
    if (m2c_option_verbose()) {
      m2c_print_line_and_mark_column(p->lexer, line, column);
    } /* end if */
    
    /* update error count */
    m2c_stats_inc(p->stats, M2C_STATS_SYNTAX_ERROR_COUNT);
    
    return false;
  } /* end if */
} /* end match_token */


/* --------------------------------------------------------------------------
 * private function match_set(p, expected_set)
 * --------------------------------------------------------------------------
 * Matches  the lookahead symbol  to set expected_set  and returns true if it
 * matches any of the tokens in the set.  Reports a syntax error  and returns
 * false if it does not match any of the tokens in the set.
 * ----------------------------------------------------------------------- */

static bool match_set
  (m2c_parser_context_t p, m2c_tokenset_t expected_set) {
  
  intstr_t lexeme;
  const char *lexstr;
  m2c_token_t lookahead;
  unsigned short line, column;
  
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
    lexstr = intstr_char_ptr(lexeme);
    
    /* report error */
    m2c_emit_syntax_error_w_set
      (line, column, lookahead, lexstr, expected_set);
    
    /* print source line */
    if (m2c_option_verbose()) {
      m2c_print_line_and_mark_column(p->lexer, line, column);
    } /* end if */
        
    /* update error count */
    m2c_stats_inc(p->stats, M2C_STATS_SYNTAX_ERROR_COUNT);
    
    return false;
  } /* end if */
} /* end match_set */


/* --------------------------------------------------------------------------
 * private function skip_to_token(p, token);
 * --------------------------------------------------------------------------
 * Consumes symbols  until the lookahead symbol  matches  token target_token.
 * Returns the new lookahead symbol.
 * ----------------------------------------------------------------------- */

static m2c_token_t skip_to_token
  (m2c_parser_context_t p, m2c_token_t target_token) {
  
  m2c_token_t lookahead;
  
  lookahead = m2c_next_sym(p->lexer);
  
  while (lookahead != target_token) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end while */
  
  return lookahead;
} /* end skip_to_token */


/* --------------------------------------------------------------------------
 * private function skip_to_set(p, set);
 * --------------------------------------------------------------------------
 * Consumes symbols until the lookahead symbol  matches  any token within set
 * target_set.  Returns the new lookahead symbol.
 * ----------------------------------------------------------------------- */

static m2c_token_t skip_to_set
  (m2c_parser_context_t p, m2c_tokenset_t target_set) {
  
  m2c_token_t lookahead;
  
  lookahead = m2c_next_sym(p->lexer);
  
  while (m2c_tokenset_member(target_set, lookahead) == false) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end while */
  
  return lookahead;
} /* end skip_to_set */


/* --------------------------------------------------------------------------
 * private function skip_to_token_or_set(p, token, set);
 * --------------------------------------------------------------------------
 * Consumes symbols  until the lookahead symbol matches token target_token or
 * any token within set target_set.  Returns the new lookahead symbol.
 * ----------------------------------------------------------------------- */

static m2c_token_t skip_to_token_or_set
  (m2c_parser_context_t p,
   m2c_token_t target_token, m2c_tokenset_t target_set) {
  
  m2c_token_t lookahead;
  
  lookahead = m2c_next_sym(p->lexer);
  
  while ((lookahead != target_token)
    && (m2c_tokenset_member(target_set, lookahead) == false)) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end while */
  
  return lookahead;
} /* end skip_to_token_or_set */


/* --------------------------------------------------------------------------
 * private function skip_to_token_list(p, ...);
 * --------------------------------------------------------------------------
 * Consumes symbols  until the lookahead symbol matches any token in the list
 * of tokens passed in.  Returns the new lookahead symbol.
 * ----------------------------------------------------------------------- */

static m2c_token_t skip_to_token_list
  (m2c_parser_context_t p, m2c_token_t first_token, ...) {
  m2c_token_t token, lookahead;
  
  va_list token_list;
  
  lookahead = m2c_next_sym(p->lexer);
  
  while (lookahead != TOKEN_EOF) {
    va_start(token_list, first_token);
    
    /* check current lookahead against token list */
    token = first_token;
    while ((token != NULL) && (lookahead != token)) {
      token = va_arg(token_list, m2c_token_t);
    } /* end while */
    
    va_end(token_list);
    
    /* match found */
    if (lookahead == token) {
      break;
    } /* end if */
    
    /* skip to next symbol */
    lookahead = m2c_consume_sym(p->lexer);
  } /* end while */
    
  return lookahead;
} /* end skip_to_token_list */


/* ************************************************************************ *
 * Syntax Analysis                                                          *
 * ************************************************************************ */

/* --------------------------------------------------------------------------
 * private procedure parse_start_symbol(p)
 * --------------------------------------------------------------------------
 * startSymbol = compilationUnit ;
 *
 * astnode: (FILE (FNAME "Foobar.mod") (KEY 0xF04FC729) moduleNode)
 * ----------------------------------------------------------------------- */

static void parse_start_symbol (m2c_parser_context_t p) {
  intstr_t filename;
  m2c_token_t lookahead;
  m2c_astnode_t filename_node, key_node, module_node;
  
  /* compilation unit */
  lookahead = compilation_unit(p);
  module_node = p->ast;
  
  if (lookahead != TOKEN_EOF) {
    /* TO DO: report error -- symbols after end of compilation unit */
    m2c_stats_inc(p->stats, M2C_STATS_SYNTAX_ERROR_COUNT);
    skip_to_token(p, TOKEN_EOF);
  } /* end if */
  
  /* filename node */
  filename = intstr_for_cstr(p->filename);
  filename_node = m2c_ast_new_terminal_node(AST_FNAME, filename);
  
  /* module-key node */
  key_node = m2c_ast_new_terminal_node(AST_KEY, m2c_lexer_digest(p->lexer));
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_FILE,
    filename_node, key_node, module_node, NULL);
  
  return;
} /* end parse_start_symbol */


/* --------------------------------------------------------------------------
 * private function compilation_unit()
 * --------------------------------------------------------------------------
 * compilationUnit :=
 *   interfaceModule | implementationModule | programModule
 *   ;
 *
 * astnode: ifcModuleNode | impModuleNode | pgmModuleNode
 * ----------------------------------------------------------------------- */

static m2c_token_t compilation_unit (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("compilationUnit");
  
  m2c_token_t lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
    case TOKEN_INTERFACE :
      if (is_def_suffix(p->suffix) == false) {
        /* TO DO: report error -- incorrect file type */
        m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
      } /* end if */
      
      p->module_context = IFC_MODULE;
      lookahead = interface_module(p);
      break;
      
    case TOKEN_IMPLEMENTATION :
      if (is_mod_suffix(p->suffix) == false) {
        /* TO DO: report error -- incorrect file type */
        m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
      } /* end if */
      
      p->module_context = IMP_MODULE;
      lookahead = implementation_module(p);
      break;
      
    case TOKEN_MODULE:
      if (is_mod_suffix(p->suffix) == false) {
        /* TO DO: report error -- incorrect file type */
        m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
      } /* end if */
      
      p->module_context = PGM_MODULE;
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
 * private function interface_module()
 * --------------------------------------------------------------------------
 * interfaceModule :=
 *   INTERFACE MODULE moduleIdent ';'
 *   import* declaration* END moduleIdent '.'
 *   ;
 *
 * moduleIdent := Ident ;
 *
 * astnode: (INTERFACE identNode impList defDeclList)
 * ----------------------------------------------------------------------- */

static m2c_token_t import (m2c_parser_context_t p);
static m2c_token_t declaration (m2c_parser_context_t p);

static m2c_token_t interface_module (m2c_parser_context_t p) {
  intstr_t ident1, ident2;
  m2c_token_t lookahead;
  m2c_fifo_t imp_list, dd_list;
  m2c_astnode_t id_node, imp_node, dd_node;
  
  PARSER_DEBUG_INFO("interfaceModule");
  
  /* INTERFACE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* MODULE */
  if (match_token(p, TOKEN_MODULE))) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_IDENT, TOKEN_SEMICOLON, NULL);
  } /* end if */
  
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = ident(p);
    id_node = p->ast;
    
    ident1 = m2c_lexer_current_lexeme(p->lexer);
    if (intstr_for_cstr(p->basename) != ident1) {
      /* TO DO: report error -- module identifier does not match filename */
      m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
    } /* end if */
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
  
  dd_list = m2c_fifo_new_queue(NULL);
  
  /* declaration* */
  while (m2c_tokenset_element(FIRST(DECLARATION), lookahead)) {
    lookahead = declaration(p);
    m2c_fifo_enqueue(dd_list, p->ast);
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
      m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
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
  imp_node = m2c_ast_new_term_list_node(AST_IMPORT, imp_list);
  dd_node = m2c_ast_new_term_list_node(AST_DEFDECL, dd_list);
  
  p->ast = m2c_ast_new_node(AST_INTERFACE, id_node, imp_node, dd_node, NULL);
  
  m2c_fifo_release(imp_list);
  m2c_fifo_release(dd_list);
  
  return lookahead;
} /* end interface_module */


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

static m2c_token_t ident (m2c_parser_context_t p);

static m2c_token_t import (m2c_parser_context_t p) {
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
      skip_to_token_list
        (p, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_IMPORT, NULL);
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
        skip_to_token_list
          (p, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_IMPORT, NULL);
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
  p->ast = m2c_ast_new_node(AST_IMPORT, imp_list, rxp_list, NULL);
  
  m2c_fifo_release_queue(imp_list);
  m2c_fifo_release_queue(rxp_list);
  
  return lookahead;
} /* end import */


/* --------------------------------------------------------------------------
 * private function declaration()
 * --------------------------------------------------------------------------
 * declaration :=
 *   CONST ( publicConstDefn ';' )+ |
 *   TYPE ( publicTypeDefn ';' )+ |
 *   VAR ( varDefinition ';' )+ |
 *   procedureDeclaration ';' |
 *   toDoList ';'
 *   ;
 *
 * alias procedureDeclaration = ProcedureHeader ;
 * ----------------------------------------------------------------------- */

#define const_definition_list(_p) def_or_decl_list(const_def, _p)
#define type_definition_list(_p) def_or_decl_list(type_def, _p)
#define var_definition_list(_p) def_or_decl_list(var_def, _p)
static m2c_token_t definition_list
  (def_decl_context_t *context, m2c_parser_context_t p);
#define procedure_declaration(_p) procedureHeader(_p)
static m2c_token_t procedure_header (m2c_parser_context_t p);
static m2c_token_t to_do_list (m2c_parser_context_t p);

m2c_token_t declaration (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("declaration");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
    
    /* CONST */
    case TOKEN_CONST :
      /* (publicConstDefn ';')+ */
      lookahead = const_definition_list(p);
      break;
      
    /* | TYPE */
    case TOKEN_TYPE :
      /* (publicTypeDefn ';')+ */
      lookahead = type_definition_list(p);
      break;
      
    /* | VAR */
    case TOKEN_VAR :
      /* (varDefinition ';')+ */
      lookahead = var_definition_list(p);
      break;
      
    /* | procedureDeclaration */
    case TOKEN_PROCEDURE :
      lookahead = procedure_declaration(p); /* p->ast holds ast-node */
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON)) {
        lookahead = m2c_consume_sym(p->lexer);
      }
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
      }
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
} /* end declaration */


/* --------------------------------------------------------------------------
 * private function definition_list(context, p)
 * --------------------------------------------------------------------------
 * Parses one of  publicConstDefnList, publicTypeDefnList, varDefinitionList,
 * constDefinitionList,  impModTypeDefnList  and  pgmModTypeDefnList,  depen-
 * ding on parameter context,  constructs its AST node,  passes it  in p->ast
 * and returns the new lookahead symbol.
 *
 * (1) public const definition list context:
 *
 * publicConstDefnList :=
 *   publicConstDefn ';' (publicConstDefn ';')*
 *   ;
 *
 * astnode: (CONSTDEFLIST constDefnNode1 constDefnNode2 ... constDefnNodeN)
 *
 * (2) public type definition list context:
 *
 * publicTypeDefnList :=
 *   publicTypeDefn ';' (publicTypeDefn ';')*
 *   ;
 *
 * astnode: (TYPEDEFLIST typeDefnNode1 typeDefnNode2 ... typeDefnNodeN)
 *
 * (3) var definition list context:
 *
 * varDefinitionList :=
 *   varDefinition ';' (varDefinition ';')*
 *   ;
 *
 * astnode: (VARDEFLIST varDefnNode1 varDefnNode2 ... varDefnNodeN)
 *
 * (4) const definition list context:
 *
 * constDefinitionList :=
 *   constDefinition ';' (constDefinition ';')*
 *   ;
 *
 * astnode: (CONSTDEFLIST constDefnNode1 constDefnNode2 ... constDefnNodeN)
 *
 * (5) implementation module type definition list context:
 *
 * impModTypeDefnList :=
 *   impModTypeDefn ';' (impModTypeDefn ';')*
 *   ;
 *
 * astnode: (TYPEDEFNLIST typeDefnNode1 typeDefnNode2 ... typeDefnNodeN)
 *
 * (6) program module type definition list context:
 *
 * pgmModTypeDefnList :=
 *   pgmModTypeDefn ';' (pgmModTypeDefn ';')*
 *   ;
 *
 * astnode: (TYPEDEFNLIST typeDefnNode1 typeDefnNode2 ... typeDefnNodeN)
 * ----------------------------------------------------------------------- */

static m2c_token_t definition_list
  (def_decl_context_t *context, m2c_parser_context_t p) {
  
  m2c_token_t lookahead;
  m2c_fifo_t node_list;
  
  node_list = m2c_fifo_new_queue(NULL);
  
  /* const/type/var/Definition ';' */
  
  /* const/type/varDefinition */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = context->def_or_decl(p);
    m2c_fifo_enqueue(node_list, p->ast);
      
    /* ';' */
    if (match_token(p, TOKEN_SEMICOLON)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set
          (p, TOKEN_SEMICOLON, FOLLOW(context->production));
    } /* end if */
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(context->production));
  } /* end if */
  
  /* (const/type/varDefinition ';')* */
  
  /* const/type/varDefinition */
  while (match_token(p, TOKEN_IDENT)) {
    lookahead = context->def_or_decl(p); /* p-ast holds ast-node */
    m2c_fifo_enqueue(node_list, p->ast);
    
    /* ';' */
    if (match_token == TOKEN_SEMICOLON) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(context->production));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(context->node_type, node_list, NULL);
  
  m2c_fifo_releast(node_list);
  
  return lookahead;
} /* end definition_list */


/* --------------------------------------------------------------------------
 * private function public_const_defn()
 * --------------------------------------------------------------------------
 * publicConstDefn :=
 *   ( '[' bindableIdent ']' )?  constDefinition
 *   ;
 *
 * astNode: (CONST (BINDTO idNode) constDefNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t const_binding (m2c_parser_context_t p);
static m2c_token_t const_definition (m2c_parser_context_t p);

static m2c_token_t public_const_defn (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t bind_node, def_node;
    
  PARSER_DEBUG_INFO("publicConstDefn");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* ( '[' bindableIdent ']' )? */
  if (lookahead == TOKEN_LBRACKET) {
    /* '[' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* bindableIdent */
    if (match_token(p, TOKEN_IDENT)) {
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_current_lexeme(p->lexer);
      
      /* bindableIdent = COLLATION | TLIMIT */
      if ((lexeme == m2c_res_ident(RESIDENT_COLLATION))
        || (lexeme == m2c_res_ident(RESIDENT_TLIMIT))) {
        lookahead = m2c_consume_sym(p->lexer);
        bind_node = m2c_new_ast_terminal_node(AST_BINDTO, lexeme);
      }
      /* identifier not bindable */
      else {
        lookahead = m2c_consume_sym(p);
        bind_node = m2c_ast_empty_node();
        
        /* TO DO: error -- invalid binding specifier */
        m2c_stats_inc(p->stats, M2C_STATS_SYNTAX_ERROR_COUNT);
      } /* end if */
    }
    else /* resync */ {
      lookahead =
        skip_to_token_list(p, TOKEN_RBRACKET, TOKEN_IDENT, NULL);
    } /* end if */
    
    /* ']' */
    if (match_token(p, TOKEN_RBRACKET)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(CONST_DEFINITION));
    } /* end if */
  } /* end if */
  
  /* constDefinition */
  if (match_token(p, TOKEN_IDENT)) {
    p->ast = bind_node;
    lookahead = const_definition(p);
    def_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(CONST_DEFINITION));
    def_node = m2c_ast_empty_node();
  } /* end if */
  
  /* pass AST node back in p->ast */
  p->ast = def_node;
  
  return lookahead;
} /* end public_const_defn */


/* --------------------------------------------------------------------------
 * private function const_definition()
 * --------------------------------------------------------------------------
 * constDefinition :=
 *   ident ( ':' typeIdent )? '=' constExpression
 *   ;
 *
 * alias constExpression = expression ;
 *
 * astNode:
 *   (CONST (EMPTY) (ID constId) (ID typeId) exprNode) |
 *   (CONST (BINDTO bindId) (ID constId) (ID typeId) exprNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t const_definition (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t bind_id, const_id, type_id, expr_node;
  
  PARSER_DEBUG_INFO("constDefinition");
  
  /* bind node left in p->ast by caller */
  bind_node = p->ast;
  
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
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(CONST_DECLARATION));
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast =
    m2c_ast_new_node
      (AST_CONST, bind_node, const_id, type_id, expr_node, NULL);
  
  return lookahead;
} /* end const_definition */


/* --------------------------------------------------------------------------
 * private function ident()
 * --------------------------------------------------------------------------
 * alias ident = StdIdent ;
 *
 * astNode: (IDENT "lexeme")
 * ----------------------------------------------------------------------- */

static m2c_token_t ident (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("ident");
  
  lookahead = m2c_consume_sym(p->lexer);
  lexeme = m2c_current_lexeme(p->lexer);
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_terminal_node(IDENT, lexeme);
  
  return lookahead;
} /* end ident */


/* --------------------------------------------------------------------------
 * private function type_definition()
 * --------------------------------------------------------------------------
 * typeDefinition :=
 *   ident '=' type
 *   ;
 *
 * astNode: (TYPE identNode typeNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t type (m2c_parser_context_t p);

static m2c_token_t type_definition (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_tokenset_t first_set;
  m2c_astnode_t ident_node, type_node;
    
  PARSER_DEBUG_INFO("typeDefinition");
  
  /* FIRST(interfaceType) | FIRST(implementationType) | FIRST(programType) */
  switch (p->module_context) {
    case IFC_MODULE :
      first_set = FIRST(IFC_MOD_TYPE);
      break;
      
    case IMP_MODULE :
      first_set = FIRST(IMP_MOD_TYPE);
      break;
    
    case PGM_MODULE :
      first_set = FIRST(PGM_MOD_TYPE);
      break;
  } /* end switch */
  
  /* Ident */
  lookahead = ident(p);
  ident_node = p->ast;
  
  /* '=' */
  if (match_token(p, TOKEN_EQUAL)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(first_set));
  } /* end if */
  
  /* type */
  if (match_set(p, first_set)) {
    lookahead = type(p);
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
 * Parses rule interfaceType, implementationType or programType  depending on
 * p->module_context,  constructs  its  AST node,  passes it  in  p->ast  and
 * returns the new lookahead symbol.
 *
 * (1) interface module context:
 *
 * interfaceType :=
 *   aliasType | derivedType | subrangeType | enumType | setType |
 *   arrayType | recordType | pointerType | opaqueType | procedureType
 *   ;
 *
 * (2) program module context:
 *
 * programType :=
 *   aliasType | derivedType | subrangeType | enumType | setType |
 *   arrayType | recordType | pointerType | procedureType
 *   ;
 *
 * (3) implementation module context:
 *
 * implementationType :=
 *   aliasType | derivedType | subrangeType | enumType | setType |
 *   arrayType | recordType | privatePointerType | procedureType
 *   ;
 *
 * astNode: aliasTypeNode | derivedTypeNode | subrangeTypeNode |
 *   enumTypeNode | setTypeNode | arrayTypeNode | recordTypeNode |
 *   pointerTypeNode | opaqueTypeNode | indeterminateTypeNode |
 *   procedureTypeNode
 * ----------------------------------------------------------------------- */

static m2c_token_t derived_type (m2c_parser_context_t p);
static m2c_token_t subrange_type (m2c_parser_context_t p);
static m2c_token_t enum_type (m2c_parser_context_t p);
static m2c_token_t set_type (m2c_parser_context_t p);
static m2c_token_t array_type (m2c_parser_context_t p);
static m2c_token_t pointer_type (m2c_parser_context_t p);
static m2c_token_t private_pointer_type (m2c_parser_context_t p);
static m2c_token_t procedure_type (m2c_parser_context_t p);

static m2c_token_t type (m2c_parser_context_t p) {
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
      
    /* | opaqueType */
    case TOKEN_OPAQUE :
      if (p->module_context == IFC_MODULE) {
        lookahead = opaque_type(p); /* p->ast holds ast-node */
      }
      else {
        /* fatal error -- we should never get here */
        exit (-1)
      } /* end if */
      break;
    
    /* | pointerType | privatePointerType */
    case TOKEN_POINTER :
      if ((p->module_context != IMP_MODULE)) {
        lookahead = pointer_type(p); /* p->ast holds ast-node */
      }
      else /* IMP_MODULE */ {
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

static m2c_token_t qualident (m2c_parser_context_t p);

static m2c_token_t alias_type (m2c_parser_context_t p) {
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

static m2c_token_t qualident (m2c_parser_context_t p) {
  intstr_t first_ident, tail_ident;
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

static m2c_token_t subrange_type (m2c_parser_context_t p) {
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

static m2c_token_t ident_list (m2c_parser_context_t p);

static m2c_token_t enum_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node, list_node;
  
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

static m2c_token_t ident_list (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  m2c_fifo_t tmp_list;
  unsigned short line, column;
  
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

static m2c_token_t set_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node;
  
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

static m2c_token_t array_type (m2c_parser_context_t p) {
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

static m2c_token_t field_list_sequence (m2c_parser_context_t p);

static m2c_token_t record_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node, list_node;
  
  PARSER_DEBUG_INFO("recordType");
  
  /* RECORD */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ( '(' recTypeToExtend ')' )? */
  if (lookahead == TOKEN_LPAREN) {
    /* '(' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* typeIdent | NIL */
    if (match_token(p, TOKEN_IDENT)) {
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

static m2c_token_t field_list (m2c_parser_context_t p);

static m2c_token_t field_list_sequence (m2c_parser_context_t p) {
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
 * private macro field_list()
 * --------------------------------------------------------------------------
 * alias fieldList = varDefinition ;
 * ----------------------------------------------------------------------- */
 
#define field_list(_p) var_definition(_p)


/* --------------------------------------------------------------------------
 * private function pointer_type()
 * --------------------------------------------------------------------------
 * pointerType :=
 *   POINTER TO typeIdent
 *   ;
 *
 * astNode: (POINTER qualidentNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t pointer_type (m2c_parser_context_t p) {
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
 
static m2c_token_t opaque_type (m2c_parser_context_t p) {
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

static m2c_token_t formal_type (m2c_parser_context_t p);

static m2c_token_t procedure_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node, list_node;
  
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

static m2c_token_t formal_type (m2c_parser_context_t p) {
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
} /* end formal_type */


/* --------------------------------------------------------------------------
 * private function formal_type()
 * --------------------------------------------------------------------------
 * formalType :=
 *   ( CONST | VAR )? nonAttrFormalType
 *   ;
 *
 * astNode: formalTypeNode
 * ----------------------------------------------------------------------- */

static m2c_token_t non_attr_formal_type (m2c_parser_context_t p);

static m2c_token_t formal_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t type_node;
  
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

static m2c_token_t non_attr_formal_type (m2c_parser_context_t p) {
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

static m2c_token_t simple_formal_type (m2c_parser_context_t p) {
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

static m2c_token_t casting_formal_type (m2c_parser_context_t p) {
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

static m2c_token_t variadic_formal_type (m2c_parser_context_t p) {
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
 * astNode: (VARDEF idlistNode typeId)
 * ----------------------------------------------------------------------- */

static m2c_token_t var_definition (m2c_parser_context_t p) {
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
  p->ast = m2c_ast_new_node(AST_VARDEF, list_node, type_node, NULL);
  
  return lookahead;
} /* end var_definition */


/* --------------------------------------------------------------------------
 * private function anon_type()
 * --------------------------------------------------------------------------
 * anonType :=
 *   typeIdent | subrangeType | arrayType | procedureType
 *   ;
 * ----------------------------------------------------------------------- */

static m2c_token_t anon_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("anonType");
  
  lookahead = m2c_next_sym(p->lexer);
  
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
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end anon_type */


/* --------------------------------------------------------------------------
 * private function procedure_header()
 * --------------------------------------------------------------------------
 * procedureHeader :=
 *   PROCEDURE ( '[' bindingSpecifier ']' )? procedureSignature
 *   ;
 *
 * astnode: (PROCDECL bindSpecNode signatureNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t procedure_signature (m2c_parser_context_t p);

static m2c_token_t procedure_header (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t bind_node, psig_node;
  
  PARSER_DEBUG_INFO("procedureHeader");
  
  /* PROCEDURE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ( '[' bindingSpecifier ']' )? */
  if (lookahead == TOKEN_LBRACKET) {
    /* '[' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* bindingSpecifier */
    if (match_set(p, FIRST(BINDING_SPECIFIER))) {
      lookahead = binding_specifier(p);
      bind_node = p->ast;
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_RBRACKET, FIRST(PROCEDURE_SIGNATURE));
      bind_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ']' */
    if (match_token(p, TOKEN_RBRACKET)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FIRST(PROCEDURE_SIGNATURE));
    } /* end if */
  }
  else /* no binding specifier */ {
    bind_node = m2c_ast_empty_node();
  } /* end if */
  
  /* procedureSignature */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = procedure_signature(p);
    psig_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(PROCEDURE_HEADER));
    psig_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_PROCDECL, bind_node, psig_node, NULL);
  
  return lookahead;
} /* end procedure_header */


/* --------------------------------------------------------------------------
 * private function binding_specifier()
 * --------------------------------------------------------------------------
 * bindingSpecifier :=
 *   NEW ( argListFlag | capacityFlag )? | RETAIN | RELEASE |
 *   READ allocFlag? | WRITE formatFlag? | bindableIdent
 *   ;
 *
 * astnode:
 *   (BINDTO "NEW") | (BINDTO "NEW+") | (BINDTO "NEW*") | (BINDTO "ident")
 * ----------------------------------------------------------------------- */

static m2c_token_t binding_specifier (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  
  lookahead = m2c_next_sym(p->lexer);
  lexeme = m2c_lookahead_lexeme(p->lexer);
  
  switch (lookahead) {
    case TOKEN_NEW :
      /* NEW */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* ('+' | '*')? */
      if (lookahead == TOKEN_PLUS) {
        /* '+' */
        lookahead = m2c_consume_sym(p->lexer);
        lexeme = m2c_lexeme_for_bindable(BINDABLE_NEWARGS);
      }
      else if (lookahead == TOKEN_HASH) {
        /* '#' */
        lookahead = m2c_consume_sym(p->lexer);
        lexeme = m2c_lexeme_for_bindable(BINDABLE_NEWCAP);
      }
      else /* no flag */ {
        lexeme = m2c_lexeme_for_bindable(BINDABLE_NEW);
      } /* end if */
      break;
      
    case TOKEN_READ :
      /* READ */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* '*'? */
      if (lookahead == TOKEN_ASTERISK) {
        /* '*' */
        lookahead = m2c_consume_sym(p->lexer);
        lexeme = m2c_lexeme_for_bindable(BINDABLE_READNEW);
      }
      else /* no flag */ {
        bind_target = m2c_lexeme_for_bindable(BINDABLE_READ);
      } /* end if */
      break;
    
    case TOKEN_RELEASE :
      /* RELEASE */
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_lexeme_for_bindable(BINDABLE_RELEASE);
      break;
    
    case TOKEN_RETAIN :
      /* RETAIN */
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_lexeme_for_bindable(BINDABLE_RETAIN);
      break;
    
    case TOKEN_WRITE :
      /* WRITE */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* '#'? */
      if (lookahead == TOKEN_HASH) {
        /* '#' */
        lookahead = m2c_consume_sym(p->lexer);
        lexeme = m2c_lexeme_for_bindable(BINDABLE_WRITEF);
      }
      else /* no flag */ {
        lexeme = m2c_lexeme_for_bindable(BINDABLE_WRITE);
      } /* end if */
      break;
    
    case TOKEN_IDENT :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_current_lexeme(p->lexer);
      
      if (m2c_bindable_for_lexeme(lexeme) == BINDABLE_INVALID) {
        /* TO DO: report error -- symbol not bindable */
        lexeme = intstr_empty_string();
      } /* end if */
      break;
  } /* end switch */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_terminal_node(AST_BINDTO, lexeme);
  
  return lookahead;
} /* end binding_specifier */


/* --------------------------------------------------------------------------
 * private function procedure_signature()
 * --------------------------------------------------------------------------
 * procedureSignature :=
 *   Ident ( '(' formalParamList ')' )? ( ':' returnedType )?
 *   ;
 *
 * astnode: (PSIG identNode formalParamListNode returnTypeNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t formal_param_list (m2c_parser_context_t p);

static m2c_token_t procedure_signature (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t id_node, list_node, type_node;
  
  PARSER_DEBUG_INFO("procedureSignature");
  
  /* Ident */
  lookahead = ident(p);
  id_node = p->ast;
    
  /* ( '(' formalParams ')' )? */
  if (lookahead == TOKEN_LEFT_PAREN) {
    
    /* '(' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* formalParamList */
    if (match_set(p, FIRST(FORMAL_PARAMS))) {
      lookahead = formal_param_list(p);
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
      lookahead =
        skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(PROCEDURE_SIGNATURE));
    } /* end if */
  }
  else /* no formal parameter list */ {
    list_node = m2c_ast_empty_node();
  } /* end if */
  
  /* ( ':' returnedType )? */
  if (lookahead == TOKEN_COLON) {
    /* ':' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* returnedType */
    if (match_token(p, TOKEN_IDENTIFIER)) {
      lookahead = qualident(p);
      type_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(PROCEDURE_TYPE));
      type_node = m2c_ast_empty_node();
    } /* end if */
  }
  else /* no return type */ {
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_PSIG, id_node, list_node, type_node, NULL);
  
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

static m2c_token_t formal_params (m2c_parser_context_t p);

static m2c_token_t formal_param_list (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t param_list;
  
  PARSER_DEBUG_INFO("formalParamList");
  
  /* formalParams */
  lookahead = formal_params(p);
  param_list = m2c_fifo_new_queue(p->ast);
  
  /* (';' formalParams)* */
  while (match_token(p, TOKEN_SEMICOLON)) {
    /* ';' */
    lookahead = m2c_consume_sym(p->lexer);
    
    if (match_set(p, FIRST(FORMAL_PARAMS))) {
      lookahead = formal_params(p);
      param_list = m2c_fifo_enqueue(def_list, p->ast);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(FORMAL_PARAMS));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_FPARAMLIST, param_list);
  
  m2c_fifo_releast(param_list);
  
  return lookahead;
} /* end formal_param_list */


/* --------------------------------------------------------------------------
 * private function formal_params()
 * --------------------------------------------------------------------------
 * formalParams :=
 *   ( CONST | VAR )? identList ':' nonAttrFormalType
 *   ;
 *
 * astNode: (FPARAMS attr idlist ftype)
 * ----------------------------------------------------------------------- */

static m2c_token_t formal_params (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t attr_node, list_node, type_node;
  
  PARSER_DEBUG_INFO("formalParams");
  
  lookahead = m2c_next_sym(p->lexer);
    
  /* (CONST | VAR)? */
  if (lookahead == TOKEN_CONST) {
    lookahead = m2c_consume_sym(p->lexer);
    attr_node = m2c_ast_new_node(AST_CONSTP, NULL);
  }
  else if (lookahead == TOKEN_VAR) {
    lookahead = m2c_consume_sym(p->lexer);
    attr_node = m2c_ast_new_node(AST_VARP, NULL);
  }
  else /* no attribute */ {
    attr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* identList */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = ident_list(p);
    list_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_or_set(p, TOKEN_COLON, FIRST(NON_ATTR_FORMAL_TYPE));
    list_node = m2c_ast_node_empty();
  } /* end if */
  
  /* ':' */
  if (match_token(p, TOKEN_COLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(NON_ATTR_FORMAL_TYPE));
  } /* end if */
  
  /* nonAttrFormalType */
  if (match_set(p, FIRST(NON_ATTR_FORMAL_TYPE))) {
    lookahead = non_attr_formal_type(p);
    type_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(NON_ATTR_FORMAL_TYPE));
    type_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast =
    m2c_ast_new_node(AST_FPARAMS, attr_node, list_node, type_node, NULL);
  
  return lookahead;
} /* end formal_params */


/* ************************************************************************ *
 * Implementation and Program Module Syntax                                 *
 * ************************************************************************ */


/* --------------------------------------------------------------------------
 * private function program_module()
 * --------------------------------------------------------------------------
 * programModule :=
 *   MODULE moduleIdent ';'
 *   privateImport* block moduleIdent '.'
 *   ;
 *
 * astNode: (PGMMOD moduleIdent implist blockNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t block (m2c_parser_context_t p);

static m2c_token_t program_module (m2c_parser_context_t p) {
  intstr_t ident1, ident2;
  m2c_token_t lookahead;
  m2c_fifo_t imp_list;
  m2c_astnode_t id_node, list_node, block_node;
  
  PARSER_DEBUG_INFO("programModule");
  
  /* MODULE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = ident(p);
    id_node = p->ast;
    
    ident1 = m2c_lexer_current_lexeme(p->lexer);
    if (intstr_for_cstr(p->basename) != ident1) {
      /* TO DO: report error -- module identifier does not match filename */
      m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
    } /* end if */
  }
  else /* resync */ {
    lookahead =
      skip_to_token_list(p, TOKEN_SEMICOLON, TOKEN_IMPORT, TOKEN_BEGIN, NULL);
      ident1 = intstr_empty_string();
      id_node = m2c_ast_empty_node();
  } /* end if */
  
  /* ';' */
  if (match_token(p, TOKEN_SEMICOLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_IMPORT, TOKEN_BEGIN, NULL);
  } /* end if */
  
  imp_list = m2c_fifo_new_queue(NULL);
  
  /* import* */
  while (lookahead == TOKEN_IMPORT) {
    lookahead = private_import(p);
    m2c_fifo_enqueue(imp_list, p->ast);
  } /* end while */
  
  if (m2c_fifo_entry_count(tmp_list) > 0) {
    list_node = m2c_ast_new_list_node(AST_IMPLIST, imp_list, NULL);
  }
  else /* no import list */ {
    list_node = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(imp_list);
  
  /* block */
  if (match_set(p, FIRST(BLOCK))) {
    lookahead = block(p);
    block_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_list(p, TOKEN_IDENT, TOKEN_DOT, TOKEN_EOF, NULL);
    block_node = m2c_ast_empty_node();
  } /* end if */
    
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = m2c_consume_sym(p->lexer);
    ident2 = m2c_lexer_current_lexeme(p->lexer);
      
    if (ident1 != ident2) {
      /* TO DO: report error -- module identifiers don't match */ 
      m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
    } /* end if */
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_DOT, TOKEN_EOF, NULL);
  } /* end if */
  
  if (match_token(p, TOKEN_DOT)) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end if */  
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IMPMOD, id_node, imp_node, block_node, NULL);
  
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

static m2c_token_t private_import (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t list_node, empty_node;
  
  PARSER_DEBUG_INFO("privateImport");
  
  empty_node = m2c_ast_empty_node();
  
  /* IMPORT */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* moduleList */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = ident_list(p);
    list_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(PRIVATE_IMPORT));
    list_node = empty_node;
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IMPORT, list_node, empty_node, NULL);
  
  return lookahead;
} /* end private_import */


/* --------------------------------------------------------------------------
 * private function block()
 * --------------------------------------------------------------------------
 * block :=
 *   definition* BEGIN statementSequence END
 *   ;
 *
 * astnode: (BLOCK defnListNode statementSeqNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t definition (m2c_parser_context_t p);
static m2c_token_t statement_sequence (m2c_parser_context_t p);

static m2c_token_t block (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t defn_list;
  m2c_astnode_t list_node, sseq_node;
  
  PARSER_DEBUG_INFO("block");
  
  lookahead = m2c_next_sym(p->lexer);
  
  defn_list = m2c_fifo_new_queue(NULL);
    
  /* definition */
  while (m2c_tokenset_element(FIRST(DECLARATION), lookahead)) {
    lookahead = definition(p);
    m2c_fifo_enqueue(defn_list, p->ast);
  } /* end while */
  
  /* construct definition list node */
  if (m2c_fifo_entry_count(defn_list) > 0) {
    list_node = m2c_ast_new_list_node(AST_DEFNLIST, defn_list);
  }
  else /* no definitions */ {
    list_node = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(defn_list);
  
  /* BEGIN */
  if (match_token(p, TOKEN_BEGIN)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(STATEMENT_SEQUENCE));
  } /* end if */
  
  /* statementSequence */
  if (match_set(p, FIRST(STATEMENT_SEQUENCE))) {
    lookahead = statement_sequence(p);
    sseq_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(BLOCK));
    sseq_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_BLOCK, list_node, sseq_node, NULL);
  
  return lookahead;
} /* end block */


/* --------------------------------------------------------------------------
 * private function definition()
 * --------------------------------------------------------------------------
 * definition :=
 *   CONST ( constDefinition ';' )+ |
 *   TYPE ( typeDefinition ';' )+ |
 *   VAR ( varDefinition ';' )+ |
 *   procedureHeader ';' block ident ';' |
 *   aliasDefinition ';' |
 *   toDoList ';'
 *   ;
 * ----------------------------------------------------------------------- */

#define const_definition_list(_p) def_or_decl_list(const_defn, _p)
#define type_definition_list(_p) def_or_decl_list(type_defn, _p)
#define var_definition_list(_p) def_or_decl_list(var_defn, _p)
static m2c_token_t procedure_definition (m2c_parser_context_t p);
static m2c_token_t alias_definition (m2c_parser_context_t p);
static m2c_token_t to_do_list (m2c_parser_context_t p);

static m2c_token_t definition (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("definition");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
    
    /* CONST */
    case TOKEN_CONST :
      /* (constDefinition ';')+ */
      lookahead = const_definition_list(p);
      break;
      
    /* | procedureDefinition */
    case TOKEN_PROCEDURE :
      lookahead = procedure_definition(p); /* p->ast holds ast-node */
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON)) {
        lookahead = m2c_consume_sym(p->lexer);
      }
      else /* resync */ {
        lookahead = skip_to_set(p, FOLLOW(DECLARATION));
      } /* end if */
      break;
      
    /* | toDoList */
    case TOKEN_TO :
      lookahead = to_do_list(p); /* p->ast holds ast-node */
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON)) {
        lookahead = m2c_consume_sym(p->lexer);
      }
      else /* resync */ {
        lookahead = skip_to_set(p, FOLLOW(DECLARATION));
      } /* end if */
      break;
      
    /* | TYPE */
    case TOKEN_TYPE :
      /* (typeDefinition';')+ */
      lookahead = type_definition_list(p);
      break;
      
    /* aliasDefinition ';' */
    case TOKEN_UNQUALIFIED :
      /* aliasDefinition */
      lookahead = alias_definition(p);
      
      /* ';' */
      if (match_token(p, TOKEN_SEMICOLON)) {
        lookahead = m2c_consume_sym(p->lexer);
      }
      else /* resync */ {
        lookahead = skip_to_set(p, FOLLOW(DECLARATION));
      } /* end if */
      break;
    
    /* | VAR */
    case TOKEN_VAR :
      /* (varDefinition ';')+ */
      lookahead = var_definition_list(p);
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
  } /* end switch */
  
  /* AST node is passed through in p->ast */
  
  return lookahead;
} /* end definition */


/* --------------------------------------------------------------------------
 * private function implementation_module()
 * --------------------------------------------------------------------------
 * implementationModule :=
 *   IMPLEMENTATION MODULE moduleIdent ';'
 *   privateImport* privateBlock moduleIdent '.'
 *   ;
 *
 * astNode: (IMPMOD moduleIdent implist blockNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t implementation_module (m2c_parser_context_t p) {
  intstr_t ident1, ident2;
  m2c_token_t lookahead;
  m2c_fifo_t imp_list;
  m2c_astnode_t id_node, imp_node, block_node;
  
  PARSER_DEBUG_INFO("implementationModule");
  
  /* IMPLEMENTATION */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* MODULE */
  if (match_token(p, TOKEN_MODULE)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_IDENT, TOKEN_SEMICOLON, NULL);
  } /* end if */
  
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = ident(p);
    id_node = p->ast;
    
    ident1 = m2c_lexer_current_lexeme(p->lexer);
    if (intstr_for_cstr(p->basename) != ident1) {
      /* TO DO: report error -- module identifier does not match filename */
      m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
    } /* end if */
  }
  else /* resync */ {
    lookahead =
      skip_to_token_list(p, TOKEN_SEMICOLON, TOKEN_IMPORT, NULL);
      ident1 = intstr_empty_string();
      id_node = m2c_ast_empty_node();
  } /* end if */
  
  /* ';' */
  if (match_token(p, TOKEN_SEMICOLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_IMPORT, FIRST(BLOCK);
  } /* end if */
  
  imp_list = m2c_fifo_new_queue(NULL);
  
  /* privateImport* */
  while (lookahead == TOKEN_IMPORT) {
    lookahead = private_import(p);
    m2c_fifo_enqueue(imp_list, p->ast);
  } /* end while */
  
  if (m2c_fifo_entry_count(tmp_list) > 0) {
    imp_node = m2c_ast_new_list_node(AST_IMPLIST, imp_list, NULL);
  }
  else /* no import list */ {
    list_node = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(imp_list);
  
  /* privateBlock */
  if (match_set(p, FIRST(BLOCK))) {
    lookahead = private_block(p);
    block_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_list(p, TOKEN_IDENT, TOKEN_DOT, TOKEN_EOF, NULL);
    block_node = m2c_ast_empty_node();
  } /* end if */
  
  /* moduleIdent */
  if (match_token(p, TOKEN_IDENTIFIER)) {
    lookahead = m2c_consume_sym(p->lexer);
    ident2 = m2c_lexer_current_lexeme(p->lexer);
      
    if (ident1 != ident2) {
      /* TO DO: report error -- module identifiers don't match */ 
      m2c_stats_inc(p->stats, M2C_STATS_SEMANTIC_ERROR_COUNT);
    } /* end if */
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_DOT, TOKEN_EOF, NULL);
  } /* end if */
  
  if (match_token(p, TOKEN_DOT)) {
    lookahead = m2c_consume_sym(p->lexer);
  } /* end if */  
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_IMPMOD, id_node, imp_node, block_node, NULL);
  
  return lookahead;
} /* end implementation_module */


/* --------------------------------------------------------------------------
 * private function private_block()
 * --------------------------------------------------------------------------
 * privateBlock :=
 *   privateDefinition* ( BEGIN statementSequence )? END
 *
 * astnode: (BLOCK defnListNode statementSeqNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t private_block (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t defn_list;
  m2c_astnode_t list_node, sseq_node, empty_node;
  
  PARSER_DEBUG_INFO("privateBlock");
  
  lookahead = m2c_next_sym(p->lexer);
  
  empty_node = m2c_ast_empty_node();
  defn_list = m2c_fifo_new_queue(NULL);
    
  /* privateDefinition */
  while (m2c_tokenset_element(FIRST(DEFINITION), lookahead)) {
    lookahead = definition(p);
    m2c_fifo_enqueue(defn_list, p->ast);
  } /* end while */
  
  /* construct definition list node */
  if (m2c_fifo_entry_count(defn_list) > 0) {
    list_node = m2c_ast_new_list_node(AST_DEFNLIST, defn_list);
  }
  else /* no declarations */ {
    list_node = m2c_ast_empty_node();
  } /* end if */
  
  m2c_fifo_release(defn_list);
  
  /* ( BEGIN statementSequence )? */
  if (lookahead == TOKEN_BEGIN)) {
    /* BEGIN */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* statementSequence */
    if (match_set(p, FIRST(STATEMENT_SEQUENCE))) {
      lookahead = statement_sequence(p);
      sseq_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(BLOCK));
      sseq_node = m2c_ast_empty_node();
    } /* end if */
  }
  else /* no module initialisation */ {
    sseq_node = m2c_ast_empty_node();
  } /* end if */
  
  if ((list_node == empty_node) && (sseq_node == empty_node)) {
    /* TO DO: issue warning -- empty implementation module */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_BLOCK, list_node, sseq_node, NULL);
  
  return lookahead;
} /* end private_block */


/* --------------------------------------------------------------------------
 * private function private_pointer_type()
 * --------------------------------------------------------------------------
 * privatePointerType :=
 *   POINTER TO ( determinateTarget | indeterminateTarget )
 *   ;
 *
 * astnode:
 *  (POINTER identNode) | (PRIVPTR targetNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t private_pointer_type (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t tgt_node;
    
  PARSER_DEBUG_INFO("privatePointerType");
  
  /* POINTER */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* TO */
  if (match_token(p, TOKEN_TO)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_IDENT, TOKEN_RECORD, NULL);
  } /* end if */
  
  /* determinateTarget | indeterminateTarget */
  if (match_set(p, IDENT_OR_RECORD)) {
    switch (lookahead) {
      /* determinateTarget | */
      case TOKEN_IDENT :
        lookahead = ident(p);
        break;
        
      /* indeterminateTarget */
      case TOKEN_RECORD :
        lookahead = indeterminate_target(p);
        break;
        
      default :
        /* TO DO: fatal error -- we should never get here */
        exit (-1);
    } /* end switch */
    tgt_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(PRIVATE_POINTER_TYPE));
    tgt_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(PRIVPTR tgt_node, NULL);
   
  return lookahead;
} /* end private_pointer_type */


/* --------------------------------------------------------------------------
 * private function procedure_definition()
 * --------------------------------------------------------------------------
 * procedureDefinition :=
 *   procedureHeader ';' block Ident
 *   ;
 *
 * astnode: (PROC procDefinitionNode blockNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t procedure_definition (m2c_parser_context_t p) {
  intstr_t ident;
  m2c_token_t lookahead;
  m2c_astnode_t decl_node, block_node;
  
  PARSER_DEBUG_INFO("procedureDefinition");
  
  /* procedureHeader */
  lookahead = procedure_header(p);
  decl_node = p->ast;
  
  /* ';' */
  if (match_token(p, TOKEN_SEMICOLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(BLOCK));
  } /* end if */
    
  /* block */
  if (match_set(p, FIRST(BLOCK))) {
    lookahead = block(p);
    block_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_IDENT, FOLLOW(DEFINITION));
    block_node = m2c_ast_empty_node();
  } /* end if */
      
  /* Ident */
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = m2c_consume_sym(p->lexer);
    ident = m2c_lexer_current_lexeme(p->lexer);
        
    /* TO DO: check if procedure identifiers match */
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(DEFINITION));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_PROC, decl_node, block_node, NULL);
  
  return lookahead;
} /* end procedure_definition */


/* --------------------------------------------------------------------------
 * private function statement_sequence()
 * --------------------------------------------------------------------------
 * statementSequence :=
 *   statement ( ';' statement )*
 *   ;
 *
 * astnode: (STMTSEQ stmtNode+)
 * ----------------------------------------------------------------------- */

static m2c_token_t statement (m2c_parser_context_t p);

static m2c_token_t statement_sequence (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t stmt_list;
  
  PARSER_DEBUG_INFO("statementSequence");
  
  /* statement */
  lookahead = statement(p);
  stmt_list = m2c_fifo_new_queue(p->ast);
  
  /* ( ';' statement )* */
  while (lookahead == TOKEN_SEMICOLON) {
    /* ';' */
    lookahead = m2c_consume_sym(p->lexer);
        
    /* statement */
    if (match_set(p, FIRST(STATEMENT))) {
      lookahead = statement(p);
      m2c_fifo_enqueue(stmt_list, p->ast);
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(STATEMENT));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_list_node(AST_STMTSEQ, stmt_list);
  
  m2c_fifo_release(stmt_list);
  
  return lookahead;
} /* end statement_sequence */


/* --------------------------------------------------------------------------
 * private function statement()
 * --------------------------------------------------------------------------
 * statement :=
 *   memMgtOperation | updateOrProcCall | returnStatement | copyStatement |
 *   readStatement | writeStateent | ifStatement | caseStatement |
 *   loopStatement | whileStatement | repeatStatement | forStatement |
 *   toDoList | EXIT | NOP
 *   ;
 * ----------------------------------------------------------------------- */

static m2c_token_t new_statement (m2c_parser_context_t p);
static m2c_token_t retain_statement (m2c_parser_context_t p);
static m2c_token_t release_statement (m2c_parser_context_t p);
static m2c_token_t update_or_proc_call (m2c_parser_context_t p);
static m2c_token_t return_statement (m2c_parser_context_t p);
static m2c_token_t copy_statement (m2c_parser_context_t p);
static m2c_token_t read_statement (m2c_parser_context_t p);
static m2c_token_t write_statement (m2c_parser_context_t p);
static m2c_token_t if_statement (m2c_parser_context_t p);
static m2c_token_t case_statement (m2c_parser_context_t p);
static m2c_token_t loop_statement (m2c_parser_context_t p);
static m2c_token_t while_statement (m2c_parser_context_t p);
static m2c_token_t repeat_statement (m2c_parser_context_t p);
static m2c_token_t for_statement (m2c_parser_context_t p);

static m2c_token_t statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("statement");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
  
    /* newStatement */
    case TOKEN_NEW :
      new_statement(p);
      break;
      
    /* | retainStatement */
    case TOKEN_RETAIN :
      retain_statement(p);
      break;
      
    /* | releaseStatement */
    case TOKEN_RELEASE :
      lookahead = release_statement(p);
      break;
      
    /* | updateOrProcCall */
    case TOKEN_IDENT :
      lookahead = update_or_proc_call(p);
      break;
      
    /* | returnStatement */
    case TOKEN_RETURN :
      lookahead = return_statement(p);
      break;
      
    /* | copyStatement */
    case TOKEN_COPY :
      lookahead = copy_statement(p);
      break;
      
    /* | readStatement */
    case TOKEN_READ :
      lookahead = read_statement(p);
      break;
      
    /* | writeStatement */
    case TOKEN_WRITE :
      lookahead = write_statement(p);
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
      
    /* | toDoList */
    case TOKEN_TO :
      lookahead = to_do_list(p);
      break;
      
    /* | EXIT */
    case TOKEN_EXIT :
      lookahead = m2c_consume_sym(p->lexer);
      p->ast = m2c_ast_new_node(AST_EXIT, NULL);
      break;
      
    /* | NOP */
    case TOKEN_NOP :
      lookahead = m2c_consume_sym(p->lexer);
      p->ast = m2c_ast_new_node(AST_NOP, NULL);
      break;
      
    default : /* unreachable code */
      /* fatal error -- abort */
      exit(-1);
    } /* end switch */
  
  return lookahead;
} /* end statement */


/* --------------------------------------------------------------------------
 * private function new_statement()
 * --------------------------------------------------------------------------
 * newStatement :=
 *   NEW designator ( ':=' structuredValue | CAPACITY expression )?
 *   ;
 *
 * astnode:
 *  (NEW desigNode) | (NEWINIT desigNode initValNode) | (NEWCAP valueNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t designator (m2c_parser_context_t p);
static m2c_token_t expression (m2c_parser_context_t p);

static m2c_token_t new_statement (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  m2c_astnode_t id_node, init_node, capv_node;
  
  PARSER_DEBUG_INFO("newStatement");
  
  /* NEW */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* designator */
  if (match_set(p, FIRST(DESIGNATOR))) {
    lookahead = designator(p);
    id_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_ASSIGN, TOKEN_IDENT);
    id_node = m2c_ast_empty_node();
  } /* end if */
  
  lexeme = m2c_lookahead_lexeme(p->lexer);
  
  /* ( ':=' structuredValue | CAPACITY expression )? */
  if (lookahead == TOKEN_ASSIGN) {
    /* ':=' */
    lookahead = m2c_consume_sym(p);
    
    /* structuredValue */
    if (match_token(p, TOKEN_LBRACE)) {
      lookahead = structured_value(p);
      init_node = p->ast;
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(STATEMENT));
      init_node = m2c_ast_empty_node();
    } /* end if */
    p->ast = m2c_ast_new_node(AST_NEWINIT, id_node, init_node, NULL);
  }
  else if ((lookahead == TOKEN_IDENT) 
    && (lexeme == m2c_res_ident(RESIDENT_CAPACITY))) {
    /* CAPACITY */
    lookahead = m2c_consume_sym(p);
    
    /* expression */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = expression(p);
      capv_node = p->ast;
    }
    else /* resync */ {
      lookahead =
        skip_to_token_or_set(p, TOKEN_SEMICOLON, FOLLOW(STATEMENT));
      capv_node = m2c_ast_empty_node();
    } /* end if */
    p->ast = m2c_ast_new_node(AST_NEWCAP, id_node, capv_node, NULL);
  }
  else /* no parameters */ {
    p->ast = m2c_ast_new_node(AST_NEW, id_node, NULL);
  } /* end if */
  
  return lookahead;
} /* end new_statement */


/* --------------------------------------------------------------------------
 * private function retain_statement()
 * --------------------------------------------------------------------------
 * retainStatement :=
 *   RETAIN designator
 *   ;
 *
 * astnode:
 *  (RETAIN desigNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t retain_statement (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  m2c_astnode_t id_node;
  
  PARSER_DEBUG_INFO("retainStatement");
  
  /* RETAIN */
  lookahead = m2c_consume_sym(p->lexer);
  
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = designator(p);
    id_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(STATEMENT));
    id_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_RETAIN, id_node, NULL);
      
  return lookahead;
} /* end retain_statement */


/* --------------------------------------------------------------------------
 * private function release_statement()
 * --------------------------------------------------------------------------
 * releaseStatement :=
 *   RELEASE designator
 *   ;
 *
 * astnode:
 *  (RELEASE desigNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t release_statement (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  m2c_astnode_t id_node;
  
  PARSER_DEBUG_INFO("releaseStatement");
  
  /* RELEASE */
  lookahead = m2c_consume_sym(p->lexer);
  
  if (match_token(p, TOKEN_IDENT)) {
    lookahead = designator(p);
    id_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(STATEMENT));
    id_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_RELEASE, id_node, NULL);
      
  return lookahead;
} /* end release_statement */


/* --------------------------------------------------------------------------
 * private function update_or_proc_call()
 * --------------------------------------------------------------------------
 * updateOrProcCall :=
 *   designator ( IncOrDecSuffix | '(' expressionList ')' )? |
 *   targetDesignator ':=' expression
 *   ;
 *
 * astnode:
 *  
 * ----------------------------------------------------------------------- */

static m2c_token_t update_or_proc_call (m2c_parser_context_t p) {
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("updateOrProcCall");
  
  /* TO DO */
  
  return lookahead;
} /* end update_or_proc_call */


/* --------------------------------------------------------------------------
 * private function return_statement()
 * --------------------------------------------------------------------------
 * returnStatement :=
 *   RETURN expression?
 *   ;
 *
 * astnode: (RETURN exprNode) | (RETURN (EMPTY))
 * ----------------------------------------------------------------------- */

static m2c_token_t return_statement (m2c_parser_context_t p) {
  m2c_astnode_t expr_node;
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("returnStatement");
  
  /* RETURN */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* expression? */
  if (m2c_tokenset_element(FIRST(EXPRESSION), lookahead)) {
    lookahead = expression(p);
    expr_node = p->ast;
  }
  else {
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_RETURN, expr_node, NULL);
  
  return lookahead;
} /* end return_statement */


/* --------------------------------------------------------------------------
 * private function copy_statement()
 * --------------------------------------------------------------------------
 * copyStatement :=
 *   COPY targetDesignator ':=' expression
 *   ;
 *
 * astnode:
 *   (COPY designatorNode expressionNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t copy_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
    
  PARSER_DEBUG_INFO("copyStatement");
  
  /* COPY */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* targetDesignator */
  if (match_set(p, FIRST(TARGET_DESIGNATOR))) {
    lookahead = target_designator(p);
    id_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(EXPRESSION));
    id_node = m2c_ast_empty_node();
  } /* end if */
  
  /* ':=' */
  if (match_token(p, TOKEN_ASSIGN)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(EXPRESSION));
  } /* end if */
  
  /* expression */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(EXPRESSION));
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_COPY, id_node, expr_node, NULL);
  
  return lookahead;
} /* end copy_statement */


/* --------------------------------------------------------------------------
 * private function read_statement()
 * --------------------------------------------------------------------------
 * readStatement :=
 *   READ ( '@' chan ':' )?
 *   inputArg ( ',' inputArg )*
 *   ;
 *
 * astnode:
 *   (READ chanNode inputArgNode+)
 * ----------------------------------------------------------------------- */

static m2c_token_t input_arg (m2c_parser_context_t p);

static m2c_token_t read_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t arg_list;
  m2c_astnode_t chan_node;
    
  PARSER_DEBUG_INFO("readStatement");
  
  /* READ */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ( '@' chan ':' )? */
  if (lookahead == TOKEN_AT_SIGN) {
    /* '@' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* chan */
    if (match_set(p, FIRST(DESIGNATOR))) {
      lookahead = designator(p);
      chan_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set();
      chan_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ':' */
    if (match_token(p, TOKEN_COLON)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FIRST(INPUT_ARG));
    } /* end if */
  }
  else /* chan parameter omitted */ {
    chan_node = m2c_ast_empty_node();
  } /* end if */
  
  arg_list = m2c_fifo_new_list(NULL);
  
  /* inputArg */
  if (match_set(p, FIRST(INPUT_ARG))) {
    lookahead = input_arg(p);
    m2c_fifo_enqueue(arg_list, p->ast);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(INPUT_ARG));
  } /* end if */
  
  /* (',' inputArg)* */
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* inputArg */
    if (match_set(p, FIRST(INPUT_ARG))) {
      lookahead = input_arg(p);
      m2c_fifo_enqueue(arg_list, p->ast);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(INPUT_ARG));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_READ, chan_node, arg_list, NULL);
  
  m2c_fifo_release(arg_list);
  
  return lookahead;
} /* end read_statement */


/* --------------------------------------------------------------------------
 * private function input_arg()
 * --------------------------------------------------------------------------
 * inputArg :=
 *   NEW? designator
 *   ;
 *
 * astnode:
 *   (READARG designator) | (READNEW designator)
 * ----------------------------------------------------------------------- */

static m2c_token_t input_arg (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t id_node;
  m2c_ast_nodetype_t node_type;
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* NEW? */
  if (lookahead == TOKEN_NEW) {
    /* NEW */
    lookahead = m2c_consume_sym(p->lexer);
    node_type = AST_READNEW;
  }
  else /* no allocation */ {
    node_type = AST_READARG;
  } /* end if */
  
  /* designator */
  if (match_set(p, FIRST(DESIGNATOR))) {
    lookahead = designator(p);
    id_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW_DESIGNATOR);
    id_node = m2c_ast_empty_node();
  } /* end if */
    
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(node_type, id_node, NULL);
  
  return lookahead;
} /* end input_arg */


/* --------------------------------------------------------------------------
 * private function write_statement()
 * --------------------------------------------------------------------------
 * writeStatement :=
 *   WRITE ( '@' chan ':' )?
 *   outputArgs ( ',' outputArgs )*
 *   ;
 *
 * astnode:
 *   (WRITE chanNode outputArgsNode+)
 * ----------------------------------------------------------------------- */

static m2c_token_t write_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t arg_list;
  m2c_astnode_t chan_node;
    
  PARSER_DEBUG_INFO("writeStatement");
  
  /* WRITE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* ( '@' chan ':' )? */
  if (lookahead == TOKEN_AT_SIGN) {
    /* '@' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* chan */
    if (match_set(p, FIRST(DESIGNATOR))) {
      lookahead = designator(p);
      chan_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set();
      chan_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ':' */
    if (match_token(p, TOKEN_COLON)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FIRST(INPUT_ARG));
    } /* end if */
  }
  else /* chan parameter omitted */ {
    chan_node = m2c_ast_empty_node();
  } /* end if */
  
  arg_list = m2c_fifo_new_list(NULL);
  
  /* outputArgs */
  if (match_set(p, FIRST(OUTPUT_ARGS))) {
    lookahead = output_args(p);
    m2c_fifo_enqueue(arg_list, p->ast);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(OUTPUT_ARGS));
  } /* end if */
  
  /* (',' outputArgs)* */
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* outputArgs */
    if (match_set(p, FIRST(OUTPUT_ARGS))) {
      lookahead = output_args(p);
      m2c_fifo_enqueue(arg_list, p->ast);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(OUTPUT_ARGS));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_WRITE, chan_node, arg_list, NULL);
  
  m2c_fifo_release(arg_list);
  
  return lookahead;
} /* end write_statement */


/* --------------------------------------------------------------------------
 * private function output_args()
 * --------------------------------------------------------------------------
 * outputArgs :=
 *   formattedArgs | unformattedArg 
 *   ;
 *
 * formatted_args :=
 *   '#' '(' fmtStr ',' unformattedArgs ')'
 *   ;
 *
 * alias fmtStr = expression ;
 * alias unformattedArg = expression ;
 * alias unformattedArgs = expression_list;
 *
 * astnode:
 *   (WRITEARG exprNode | (FMTARG fmtNode exprListNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t output_args (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t fmt_node, args_node;
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* formattedArgs | */
  if (lookahead == TOKEN_HASH) {
    /* '#' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* '(' */
    if (match_token(p, TOKEN_LPAREN)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_token(p, FIRST(EXPRESSION));
    } /* end if */
    
    /* fmtStr */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = expression(p);
      fmt_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_COMMA, FIRST(EXPRESSION));
      fmt_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ',' */
    if (match_token(p, TOKEN_COMMA)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_token(p, FIRST(EXPRESSION));
    } /* end if */
    
    /* unformattedArgs */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = expression_list(p);
      args_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_RPAREN, FOLLOW(OUTPUT_ARGS));
      args_node = m2c_ast_empty_node();
    } /* end if */
    
    /* ')' */
    if (match_token(p, TOKEN_RPAREN)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_token(p, FOLLOW(OUTPUT_ARGS));
    } /* end if */
    
    /* build AST node and pass it back in p->ast */
    p->ast = m2c_ast_new_node(AST_FMTARG, fmt_node, args_node, NULL);
  }
  else /* unformattedArg */ {
    lookahead = expression(p);
    p->ast = m2c_ast_new_node(AST_WRITEARG, p->ast, NULL);
  } /* end if */
  
  return lookahead;
} /* end output_args */


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
 * astnode:
 *  (IF exprNode stmtSeqNode (ELIF expr stmtSeqNode) ... (ELSE stmtSeqNode))
 * ----------------------------------------------------------------------- */

static m2c_token_t if_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t elif_list;
  m2c_astnode_t if_expr_node, if_stmt_seq_node;
  m2c_astnode_t expr_node, stmt_seq_node;
  m2c_astnode_t elif_node, else_node;
  
  PARSER_DEBUG_INFO("ifStatement");
  
  /* IF */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* boolExpression */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    if_expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_THEN, FIRST(STATEMENT));
    if_expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* THEN */
  if (match_token(p, TOKEN_THEN)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(STATEMENT));
  } /* end if */
  
  /* statementSequence */
  if (match_set(p, FIRST(STATEMENT))) {
    lookahead = statement_sequence(p);
    if_stmt_seq_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_list(p, TOKEN_ELSE, TOKEN_ELSIF, TOKEN_END, NULL);
    if_stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
  
  elif_list = m2c_fifo_new_queue(NULL);
  
  /* ( ELSIF boolExpression THEN statementSequence )* */
  while (lookahead == TOKEN_ELSIF) {
    /* ELSIF */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* boolExpression */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = expression(p);
      expr_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_THEN, FIRST(STATEMENT));
      expr_node = m2c_ast_empty_node();
    } /* end if */
  
    /* THEN */
    if (match_token(p, TOKEN_THEN)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FIRST(STATEMENT));
    } /* end if */
    
    /* statementSequence */
    if (match_set(p, FIRST(STATEMENT))) {
      lookahead = statement_sequence(p);
      stmt_seq_node = p->ast;
    }
    else /* resync */ {
      lookahead =
        skip_to_token_list(p, TOKEN_ELSE, TOKEN_ELSIF, TOKEN_END, NULL);
        stmt_seq_node = m2c_ast_empty_node();
    } /* end if */
    
    /* build elsif AST node */
    elif_node = m2c_new_ast_node(AST_ELIF, expr_node, stmt_seq_node, NULL);
      
    /* add node to list */
    m2c_fifo_enqueue(elif_list, elif_node)
  } /* end while */
    
  /* ( ELSE statementSequence )? */
  if (lookahead == TOKEN_ELSE) {
    /* ELSE */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* statementSequence */
    if (match_set(p, FIRST(STATEMENT))) {
      lookahead = statement_sequence(p);
      stmt_seq_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_END, FOLLOW(STATEMENT));
      stmt_seq_node = m2c_ast_empty_node();
    } /* end if */
  }
  else /* no else-branch */ {
    stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build else AST node */
  else_node = m2c_ast_new_node(AST_ELSE, stmt_seq_node, NULL);
  
  /* END */
  if (match_token(p, TOKEN_END)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(STATEMENT));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast =
    m2c_ast_new_node(AST_IF,
      if_expr_node, if_stmt_seq_node, elif_list, else_node, NULL);
  
  m2c_fifo_release(elif_list);
  
  return lookahead;
} /* end if_statement */


/* --------------------------------------------------------------------------
 * private function case_statement()
 * --------------------------------------------------------------------------
 * caseStatement :=
 *   CASE expression OF ( '|' case )+
 *   ( ELSE statementSequence )?
 *   END
 *   ;
 *
 * NB: 'case' is a reserved word in C, we use case_branch() here instead
 *
 * astnode: (SWITCH exprNode (CASELIST caseBranchNode+) elseBranchNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t case_branch (m2c_parser_context_t p);

static m2c_token_t case_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t case_list;
  m2c_astnode_t expr_node, case_list_node, else_node;
  
  PARSER_DEBUG_INFO("caseStatement");
  
  /* CASE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* expression */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_OF, TOKEN_BAR, NULL);
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* OF */
  if (match_token(p, TOKEN_OF)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token(p, TOKEN_BAR);
  } /* end if */
  
  case_list = m2c_fifo_new_queue(NULL);
  
  /* ('|' case)+ */
  if (match_token(p, TOKEN_BAR)) {
    while (lookahead == TOKEN_BAR) {
      /* '|' */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* case */
      if (match_set(p, FIRST(EXPRESSION))) {
        lookahead = case_branch(p);
        m2c_fifo_enqueue(case_list, p->ast);
      }
      else /* resync */ {
        lookahead =
          skip_to_token_list(p, TOKEN_BAR, TOKEN_ELSE, TOKEN_END, NULL);
      } /* end if */
    } /* end while */
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_ELSE, TOKEN_END, NULL);
  } /* end if */
  
  /* build case list AST node */
  case_list_node = m2c_ast_new_node(AST_CASELIST, case_list, NULL);
  
  /* ( ELSE statementSequence )? */
  if (lookahead == TOKEN_ELSE) {
    /* ELSE */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* statementSequence */
    if (match_set(p, FIRST(STATEMENT))) {
      lookahead = statement_sequence(p);
      stmt_seq_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_or_set(p, TOKEN_END, FOLLOW(STATEMENT));
      stmt_seq_node = m2c_ast_empty_node();
    } /* end if */
  }
  else /* no else-branch */ {
    stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build else AST node */
  else_node = m2c_ast_new_node(AST_ELSE, stmt_seq_node, NULL);
  
  /* END */
  if (match_token(p, TOKEN_END)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(STATEMENT));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast =
    m2c_ast_new_node(AST_SWITCH, expr_node, case_list_node, else_node, NULL);
  
  m2c_fifo_release(case_list);
  
  return lookahead;
} /* end case_statement */


/* --------------------------------------------------------------------------
 * private function case_branch()
 * --------------------------------------------------------------------------
 * case :=
 *   caseLabels ( ',' caseLabels )* : statementSequence
 *   ;
 *
 * NB: 'case' is a reserved word in C, we use case_branch() here instead
 *
 * astnode: (CASE caseLabelListNode statementSeqNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t case_branch (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("case");
  
  /* caseLabels */
  lookahead = case_labels(p);
  label_list = m2c_fifo_new_queue(p->ast);
  
  /* (',' caseLabels)* */
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* caseLabels */
    if (match_set(p, FIRST(CASE_LABELS))) {
      lookahead = case_labels(p);
      m2c_fifo_enqueue(case_list, p->ast);
    }
    else /* resync */ {
      lookahead = skip_to_token_list(p, TOKEN_COMMA, TOKEN_COLON, NULL);
    } /* end if */
  } /* end while */
  
  /* ':' */
  if (match_token(p, TOKEN_COLON)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(STATEMENT));
  } /* end if */
  
  /* statementSequence */
  if (match_set(p, FIRST(STATEMENT))) {
    lookahead = statement_sequence(p);
    stmt_seq_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(CASE));
    stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_CASE, case_list, stmt_seq_node, NULL);
  
  m2c_fifo_release(case_list);
  
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

static m2c_token_t loop_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t stmt_seq_node;
  
  PARSER_DEBUG_INFO("loopStatement");
  
  /* LOOP */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* statementSequence */
  if (match_set(p, FIRST(STATEMENT_SEQUENCE))) {
    lookahead = statement_sequence(p);
    stmt_seq_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_END, FOLLOW(LOOP_STATEMENT));
    stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
  
  /* END */
  if (match_token(p, TOKEN_END)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(LOOP_STATEMENT));
  } /* end if*/
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_LOOP, stmt_seq_node, NULL);
    
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

static m2c_token_t while_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t expr_node, stmt_seq_node;
  
  PARSER_DEBUG_INFO("whileStatement");
  
  /* WHILE */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* boolExpression */
  if (match_set(p, FIRST(EXPRESSION)) {
    lookahead = expression(p);
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_DO, FIRST(STATEMENT_SEQUENCE));
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* DO */
  if (match_token(p, TOKEN_DO)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(STATEMENT_SEQUENCE));
  } /* end if */
  
  /* statementSequence */
  if (match_set(p, FIRST(STATEMENT_SEQUENCE))) {
    lookahead = statement_sequence(p);
    stmt_seq_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_END, FOLLOW(WHILE_STATEMENT));
    stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
  
  /* END */
  if (match_token(p, TOKEN_END)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(WHILE_STATEMENT));
  } /* end if*/
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_WHILE, expr_node, stmt_seq_node, NULL);
  
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

static m2c_token_t repeat_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t expr_node, stmt_seq_node;
  
  PARSER_DEBUG_INFO("repeatStatement");
  
  /* REPEAT */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* statementSequence */
  if (match_set(p, FIRST(STATEMENT_SEQUENCE)) {
    stmt_seq_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_or_set(p, TOKEN_UNTIL, FIRST(EXPRESSION));
      stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
    
  /* UNTIL */
  if (match_token(p, TOKEN_UNTIL)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(EXPRESSION));
  } /* end if */
    
  /* boolExpression */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(REPEAT_STATEMENT));
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_REPEAT, expr_node, stmt_seq_node, NULL);
  
  return lookahead;
} /* end repeat_statement */


/* --------------------------------------------------------------------------
 * private function for_statement()
 * --------------------------------------------------------------------------
 * forStatement :=
 *   FOR forLoopVariants IN iterableExpr DO statementSequence END
 *   ;
 *
 * .forLoopVariants :=
 *   accessor descender? ( ',' value )?
 *   ;
 *
 * alias accessor, value = StdIdent ;
 * alias descender = '--' ;
 *
 * astnode:
 *   (FOR (ASC accessorNode valueNode iterExprNode) statementSeqNode) |
 *   (FOR (DESC accessorNode valueNode iterExprNode) statementSeqNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t for_statement (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_nodetype_t node_type;
  m2c_astnode_t acc_node, val_node, expr_node, iter_node, stmt_seq_node;
  
  PARSER_DEBUG_INFO("forStatement");
  
  /* FOR */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* .forLoopVariants */
  
  /* accessor */
  if (match_token(p, TOKEN_IDENT)) {
    /* accessor */
    lookahead = ident(p);
    acc_node = p->ast;
    
    /* descender? */
    if (lookahead == TOKEN_MINUS_MINUS) {
      /* '--' */
      lookahead = m2c_consume_sym(p->lexer);
      node_type = AST_DESC;
    }
    else /* no descender */ {
      node_type = AST_ASC;
    } /* end if */
  }
  else /* resync */ {
    lookahead =
      skip_to_token_list(p, TOKEN_COMMA, TOKEN_IDENT, TOKEN_IN, NULL);
    acc_node = m2c_ast_empty_node();
  } /* end if */
  
  /* (',' value)? */
  if (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* value */
    if (match_token(p, TOKEN_IDENT)) {
      lookahead = ident(p);
      val_node = p->ast;
    }
    else /* resync */ {
      lookahead =
        skip_to_token(p, TOKEN_IN, TOKEN_LBRACKET, TOKEN_IDENT, NULL);
        val_node = m2c_ast_empty_node();
    } /* end if */
  }
  else /* no value */ {
    val_node = m2c_ast_empty_node();
  } /* end if */
  
  /* end .forLoopVariants */
  
  /* IN */
  if (match_token(p, TOKEN_IN)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_DO, FIRST(ITERABLE_EXPR));
  } /* end if */
  
  /* iterableExpr */
  if (match_set(p, FIRST(ITERABLE_EXPR))) {
    lookahead = iterable_expr(p);
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_DO, FIRST(STATEMENT_SEQUENCE));
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build iterator AST node */
  iter_node = m2c_ast_new_node(node_type, acc_node, val_node, expr_node, NULL);
  
  /* DO */
  if (match_token(p, TOKEN_IN)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_END, FIRST(STATEMENT_SEQUENCE));
  } /* end if */
  
  /* statementSequence */
  if (match_set(p, FIRST(STATEMENT_SEQUENCE)) {
    stmt_seq_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_or_set(p, TOKEN_END, FOLLOW(FOR_STATEMENT));
      stmt_seq_node = m2c_ast_empty_node();
  } /* end if */
    
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_FOR, iter_node, stmt_seq_node, NULL);
  
  return lookahead;
} /* end for_statement */


/* --------------------------------------------------------------------------
 * private function iterable_expr()
 * --------------------------------------------------------------------------
 * iterableExpr :=
 *   valueRange OF typeIdent | collectionOrTypeIdent valueRange?
 *   ;
 *
 * alias typeIdent, collectionOrTypeIdent = qualident ;
 *
 * astnode:
 *   (ITEREXPR identNode rangeNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t value_range (m2c_parser_context_t p);

static m2c_token_t iterable_expr (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t id_node, range_node;
  
  /* valueRange OF typeIdent */
  if (lookahead = TOKEN_LBRACKET) {
    /* valueRange */
    if (match_token(p, TOKEN_LBRACKET)) {
      lookahead = value_range(p);
      range_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_list(p, TOKEN_OF, TOKEN_IDENT, NULL);
      range_node = m2c_ast_empty_node();
    } /* end if */
    
    /* OF */
    if (match_token(p, TOKEN_OF)) {
      lookahead = m2c_consume_sym(p->lexer);
    }
    else /* resync */ {
      lookahead = skip_to_token_list(p, TOKEN_IDENT, TOKEN_IN, NULL));
    } /* end if */
    
    /* typeIdent */
    if (match_token(p, TOKEN_IDENT)) {
      lookahead = qualident(p);
      id_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token(p, TOKEN_IN);
      id_node = m2c_ast_empty_node();
    } /* end if */
  }
  /* | collectionOrTypeIdent valueRange? */
  else {
    /* collectionOrTypeIdent */
    match_token(p, TOKEN_IDENT) {
      lookahead = qualident(p);
      id_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_token_list(p, TOKEN_LBRACKET, TOKEN_IN, NULL);
      id_node = m2c_ast_empty_node();
    } /* end if */
    
    /* valueRange? */
    if (lookahead == TOKEN_LBRACKET) {
      lookahead = value_range(p);
      range_node = p->ast;
    }
    else /* no value range */ {
      lookahead = skip_to_token(p, TOKEN_IN);
      range_node = m2c_ast_empty_node();
    } /* end if */
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_ITEREXPR, id_node, range_node, NULL);
  
  return lookahead;
} /* end iterable_expr */


/* --------------------------------------------------------------------------
 * private function value_range()
 * --------------------------------------------------------------------------
 * valueRange :=
 *   '[' firstValue '..' lastValue ']'
 *   ;
 *
 * alias firstValue, lastValue = expression;
 *
 * astnode: (RANGE valueNode valueNode)
 * ----------------------------------------------------------------------- */


static m2c_token_t value_range (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t val1_node, val2_node;
  
  PARSER_DEBUG_INFO("valueRange");
  
  /* '[' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* firstValue */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    val1_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_DOT_DOT, FIRST(EXPRESSION));
    val1_node = m2c_ast_empty_node();
  } /* end if */
  
  /* '..' */
  if (match_token(p, TOKEN_DOT_DOT)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FIRST(EXPRESSION));
  } /* end if */
  
  /* lastValue */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    val2_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, TOKEN_RBRACKET, FOLLOW(VALUE_RANGE));
    val2_node = m2c_ast_empty_node();
  } /* end if */
  
  /* ']' */
  if (match_token(p, TOKEN_RBRACKET)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_token_or_set(p, FOLLOW(VALUE_RANGE));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_RANGE, val1_node, val2_node, NULL);
  
  return lookahead;
} /* end value_range */


/* --------------------------------------------------------------------------
 * private function designator()
 * --------------------------------------------------------------------------
 * designator :=
 *   qualident ( derefTail | subscriptTail )?
 *   ;
 *
 * astnode: identNode | (DEREF expr) | (DESIG headNode tailNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t selector (m2c_parser_context_t p);

static m2c_token_t designator (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t id_node, tail_node;
    
  PARSER_DEBUG_INFO("designator");
  
  /* qualident */
  lookahead = qualident(p);
  
  /* ( derefTail | subscriptTail )? */
  if (match_set(p, FIRST_DEREF_AND_SUBSCRIPT)) {
    id_node = p->ast;
    
    /* derefTail */
    if (lookahead == TOKEN_DEREF) {
      lookahead = deref_tail(p);
      tail_node = p->ast;      
    }
    /* | subscriptTail */
    else  {
      lookahead = subscript_tail(p);
      tail_node = p->ast;
    } /* end if */
    
    p->ast = m2c_ast_new_node(AST_DESIG, id_node, tail_node, NULL);
  } /* end if */
  
  return lookahead;
} /* end designator */


/* --------------------------------------------------------------------------
 * private function deref_tail()
 * --------------------------------------------------------------------------
 * derefTail :=
 *   deref ( '.' designator | subscriptTail )?
 *   ;
 *
 * astnode: (DEREFTAIL derefNode tailNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t deref_tail (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("derefTail");
  
  /* deref */
  lookahead = deref(p);
  deref_node = p->ast;
  
  /* ( '.' designator | subscriptTail )? */
  
  /* '.' designator */
  if (lookahead == TOKEN_DOT) {
    /* '.' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* designator */
    if (match_set(p, FIRST(DESIGNATOR))) {
      lookahead = designator(p);
      tail_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(DEREF_TAIL));
      tail_node = m2c_ast_empty_node();
    } /* end if */
  }
  /* subscriptTail */
  else if (lookahead == TOKEN_LBRACKET) {
    lookahead = subscript_tail(p);
    tail_node = p->ast;
  }
  else /* no tail */ {
    tail_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_DEREFTAIL, deref_node, tail_node, NULL);
  
  return lookahead;
} /* end deref_tail */


/* --------------------------------------------------------------------------
 * private function subscript_tail()
 * --------------------------------------------------------------------------
 * subscriptTail :=
 *   '[' expression ']' ( '.' designator | derefTail )?
 *   ;
 *
 * astnode:
 * ----------------------------------------------------------------------- */

static m2c_token_t subscript_tail (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("subscriptTail");
  
  /* '[' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* expression */
  if (match_set(p, FIRST(EXPRESSION))) {
    lookahead = expression(p);
    expr_node = p->ast;
  }
  else /* resync */ {
    lookahead =
      skip_to_token_list(p, TOKEN_RBRACKET, TOKEN_DOT, TOKEN_DEREF, NULL);
    expr_node = m2c_ast_empty_node();
  } /* end if */
  
  /* '] */
  if (match_token(p, TOKEN_LBRACKET)) {
    lookahead = m2c_consume_sym(p);
  }
  else /* resync */ {
    lookahead = skip_to_token_list(p, TOKEN_DOT, TOKEN_DEREF, NULL);
  } /* end if */
  
  /* ( '.' designator | derefTail )? */
  
  /* '.' designator */
  if (lookahead == TOKEN_DOT) {
    /* '.' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* designator */
    if (match_set(p, FIRST(DESIGNATOR))) {
      lookahead = designator(p);
      tail_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(SUBSCRIPT_TAIL));
      tail_node = m2c_ast_empty_node();
    } /* end if */
  }
  /* derefTail */
  else if (lookahead == TOKEN_DEREF) {
    lookahead = deref_tail(p);
    tail_node = p->ast;
  }
  else /* no tail */ {
    tail_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_SUBSCRTAIL, expr_node, tail_node, NULL);
  
  return lookahead;
} /* end subscript_tail */


/* --------------------------------------------------------------------------
 * private function target_designator()
 * --------------------------------------------------------------------------
 * targetDesignator :=
 *   qualident ( derefTargetTail | bracketTargetTail )?
 *   ;
 *
 * astnode:
 *   identNode | (DEREF identNode tailNode) | (SUBSCR identNode tailNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t target_designator (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("targetDesignator");
  
  /* qualident */
  lookahead = qualident(p);
  
  id_node = p->ast;
  
  /* ( derefTargetTail | bracketTargetTail )? */
  if (lookahead == TOKEN_LBRACKET) {
    lookahead = bracket_target_tail(p);
    p->ast = m2c_ast_new_node(AST_SUBSCR, id_node, p->ast, NULL);
  }
  else if (lookahead == TOKEN_DEREF) {
    lookahead = deref_target_tail(p);
    p->ast = m2c_ast_new_node(AST_DEREF, id_node, p->ast, NULL);
  } /* end if */
  
  return lookahead;
} /* end target_designator */


/* --------------------------------------------------------------------------
 * private function deref_target_tail()
 * --------------------------------------------------------------------------
 * derefTargetTail :=
 *   deref ( '.' targetDesignator | bracketTargetTail )?
 *   ;
 *
 * astnode:
 * ----------------------------------------------------------------------- */

static m2c_token_t deref_target_tail (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("derefTargetTail");
  
  /* deref */
  lookahead = deref(p);
  deref_node = p->ast;
  
  /* ( '.' targetDesignator | bracketTargetTail )? */
  
  /* '.' targetDesignator */
  if (lookahead == TOKEN_DOT) {
    /* '.' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* targetDesignator */
    if (match_set(p, FIRST(TARGET_DESIGNATOR))) {
      lookahead = target_designator(p);
      tail_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(DEREF_TARGET_TAIL));
      tail_node = m2c_ast_empty_node();
    } /* end if */    
  }
  /* bracketTargetTail */
  else if (lookahead == TOKEN_LBRACKET) {
    lookahead = bracket_target_tail(p);
    tail_node = p->ast;
  }
  else /* no tail */
    tail_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_DEREFTAIL, deref_node, tail_node, NULL);
  
  return lookahead;
} /* end deref_target_tail */


/* --------------------------------------------------------------------------
 * private function bracket_target_tail()
 * --------------------------------------------------------------------------
 * bracketTargetTail :=
 *   '[' expression
 *   (']' ( '.' targetDesignator | derefTargetTail )? | '..' expression ']')?
 *   ;
 *
 * astnode:
 * ----------------------------------------------------------------------- */

static m2c_token_t bracket_target_tail (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("bracketTargetTail");
  
  /* TO DO */
  
  return lookahead;
} /* end bracket_target_tail */


/* --------------------------------------------------------------------------
 * private function deref()
 * --------------------------------------------------------------------------
 * deref :=
 *   ( '^' )+
 *   ;
 *
 * astnode: (DEREF desigNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t deref (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("deref");
  
  /* '^'+ */
  while (lookahead == TOKEN_DEREF) {
    lookahead = m2c_consume_sym(p->lexer);
    p->ast = m2c_ast_new_node(AST_DEREF, p->ast);
  } /* end while */
  
  return lookahead;
} /* end deref */


/* --------------------------------------------------------------------------
 * private function expression_list()
 * --------------------------------------------------------------------------
 * expressionList :=
 *   expression ( ',' expression )*
 *   ;
 *
 * astnode: (INDEX exprNode+)
 * ----------------------------------------------------------------------- */

static m2c_token_t expression_list (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_fifo_t expr_list;
  
  PARSER_DEBUG_INFO("expressionList");
  
  /* expression */
  lookahead = expression(p);
  
  expr_list = m2c_fifo_new_queue(p->ast);
  
  /* ( ',' expression )* */
  while (lookahead == TOKEN_COMMA) {
    /* ',' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* expression */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = expression(p);
      m2c_fifo_enqueue(expr_list, p->ast);
    }
    else /* rsync */ {
      lookahead = skip_to_set(p, FOLLOW(EXPRESSION_LIST));
    } /* end if */
  } /* end while */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_list_node(AST_INDEX, expr_list);
  
  m2c_fifo_release(expr_list);
  
  return lookahead;
} /* end expression_list */


/* --------------------------------------------------------------------------
 * private function expression()
 * --------------------------------------------------------------------------
 * expression :=
 *   simpleExpression ( OperL1 simpleExpression )?
 *   ;
 *
 * .OperL1 := '=' | '#' | '<' | '<=' | '>' | '>=' | '==' | IN ;
 *
 * astnode:
 *  simpleExprNode |
 *  (EQ expr expr) | (NEQ expr expr) | (LT expr expr) | (LTEQ expr expr) |
 *  (GT expr expr) | (GTEQ expr expr) | (IDTY expr expr) | (IN expr expr) 
 * ----------------------------------------------------------------------- */

static m2c_token_t simple_expression (m2c_parser_context_t p);

static m2c_token_t expression (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_nodetype_t node_type;
  m2c_astnode_t left_node, right_node;
    
  PARSER_DEBUG_INFO("expression");
  
  /* simpleExpression */
  lookahead = simple_expression(p);
  
  /* ( OperL1 simpleExpression )? */
  if (M2C_IS_OPER_L1_TOKEN(lookahead)) {
    left_node = p->ast;
    
    switch (lookahead) {
      case TOKEN_IN :
        node_type = AST_IN;
        break;
      
      case TOKEN_EQUAL :
        node_type = AST_EQ;
        break;
      
      case TOKEN_NOT_EQUAL :
        node_type = AST_NEQ;
        break;
      
      case TOKEN_LESS :
        node_type = AST_LT;
        break;
      
      case TOKEN_LESS_OR_EQ :
        node_type = AST_LTEQ;
        break;
      
      case TOKEN_GREATER :
        node_type = AST_GT;
        break;
      
      case TOKEN_GREATER_OR_EQ :
        nodetype = AST_GTEQ;
        break;
        
      case TOKEN_IDENTITY :
        nodetype = AST_IDTY;
        break;
    } /* end switch */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* simpleExpression */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = simple_expression(p);
      right_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(EXPRESSION));
      right_node = m2c_ast_empty_node();
    } /* end if */
    
    p->ast = m2c_ast_new_node(node_type, left_node, right_node, NULL);
  } /* end if */
  
  return lookahead;
} /* end expression */


/* --------------------------------------------------------------------------
 * private function simple_expression()
 * --------------------------------------------------------------------------
 * simpleExpression :=
 *   '-' factor | term ( OperL2 term )*
 *   ;
 *
 * .OperL2 := '+' | '-' | OR | ConcatOp | SetDiffOp ;
 *
 * alias ConcatOp = '&' ;
 * alias SetDiffOp = '\' ;
 *
 * astnode:
 *  termNode | (NEG expr) | (PLUS expr expr) | (MINUS expr expr) |
 *  (OR expr expr) | (CONCAT expr expr) | (SETDIFF expr expr)
 * ----------------------------------------------------------------------- */

static m2c_token_t term (m2c_parser_context_t p);

static m2c_token_t simple_expression (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_nodetype_t node_type;
  m2c_astnode_t left_node, right_node;
  
  PARSER_DEBUG_INFO("simpleExpression");
  
  lookahead = m2c_next_sym(p->lexer);
  
  /* '-' factor */
  if (lookahead = TOKEN_MINUS) {
    /* '-' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* factor */
    if (match_set(p, FIRST(FACTOR))) {
      lookahead = factor(p);
      expr_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(SIMPLE_EXPRESSION));
      expr_node = m2c_ast_empty_node();
    } /* end if */
    
    p->ast = m2c_ast_new_node(AST_NEG, expr_node, NULL);
  }
  /* term (OperL2 term)* */
  else {
    /* term */
    lookahead = term(p);
    /* p->ast holds simple term node */
    
    /* (OperL2 term)* */
    while (M2C_IS_OPER_L2_TOKEN(lookahead)) {
      left_node = p->ast;
      
      /* OperL2 */
      switch (lookahead) {
        case TOKEN_OR :
          node_type = AST_OR;
          break;
      
        case TOKEN_PLUS :
          node_type = AST_PLUS;
          break;
      
        case TOKEN_MINUS :
          node_type = AST_MINUS;
          break;
          
        case TOKEN_CONCAT :
          node_type = AST_CONCAT;
          break;
          
        case TOKEN_SET_DIFF :
          node_type = AST_SETDIFF;
          break;
      } /* end switch */
      
      /* term */
      if (match_set(p, FIRST(TERM))) {
        lookahead = term(p);
        right_node = p->ast;
      }
      else /* resync */ {
        lookahead = skip_to_set(p, FIRST_OPERL2_AND_FOLLOW_SIMPLE_EXPRESSION);
        right_node = m2c_ast_empty_node();
      } /* end if */
      
      p->ast = m2c_ast_new_node(node_type, left_node, right_node, NULL);
    } /* end while */
  } /* end if */
      
  return lookahead;
} /* end simple_expression */


/* --------------------------------------------------------------------------
 * private function term()
 * --------------------------------------------------------------------------
 * term :=
 *   simpleTerm ( OperL3 simpleTerm )*
 *   ;
 *
 * .OperL3 := '*' | '/' | DIV | MOD | AND ;
 *
 * astnode:
 *  (ASTERISK expr expr) | (SOLIDUS expr expr) |
 *  (DIV expr expr) | (MOD expr expr) | (AND expr expr) | simpleTermNode
 * ----------------------------------------------------------------------- */

static m2c_token_t simple_term (m2c_parser_context_t p);

static m2c_token_t term (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_ast_nodetype_t node_type;
  m2c_astnode_t left_node, right_node;
  
  PARSER_DEBUG_INFO("term");
  
  /* simpleTerm */
  lookahead = simple_term(p);
  /* p->ast holds simple term node */
  
  /* (OperL3 simpleTerm)* */
  while (M2C_IS_OPER_L3_TOKEN(lookahead)) {
    left_node = p->ast;
    
    /* OperL3 */
    switch (lookahead) {
      case TOKEN_AND :
        node_type = AST_AND;
        break;
    
      case TOKEN_DIV :
        node_type = AST_DIV;
        break;
    
      case TOKEN_MOD :
        node_type = AST_MOD;
        break;
    
      case TOKEN_ASTERISK :
        node_type = AST_ASTERISK;
        break;
    
      case TOKEN_SOLIDUS :
        node_type = AST_SOLIDUS;
        break;
    } /* end switch */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* simpleTerm */
    if (match_set(p, FIRST(SIMPLE_TERM))) {
      lookahead = simple_term(p);
      right_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FIRST_OPERL3_AND_FOLLOW_TERM);
      right_node = m2c_ast_empty_node();
    } /* end if */
      
    p->ast = m2c_ast_new_node(node_type, left_node, right_node, NULL);
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
 * astnode: factorNode | (NOT factorNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t factor (m2c_parser_context_t p);

static m2c_token_t simple_term (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t value_node;
  bool not_flag;
  
  PARSER_DEBUG_INFO("simpleTerm");
  
  /* NOT? */
  if (lookahead == TOKEN_NOT) {
    lookahead = m2c_consume_sym(p->lexer);
    not_flag = true;
  }
  else {
    not_flag = false;
  } /* end if */
  
  /* factor */
  if (match_set(p, FIRST(FACTOR))) {
    lookahead = factor(p);
    value_node = p->ast;
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(SIMPLE_TERM));
    value_node = m2c_ast_empty_node();
  } /* end if */
  
  if (not_flag == true) {
    p->ast = m2c_ast_new_node(AST_NOT, value_node, NULL);
  }
  else /* not_flag == false */ {
    p->ast = value_node;
  } /* end if */
  
  return lookahead;
} /* end simple_term */


/* --------------------------------------------------------------------------
 * private function factor()
 * --------------------------------------------------------------------------
 * factor :=
 *   simpleFactor ( TypeConvOp typeIdent )?
 *   ;
 *
 * astnode: simpleFactorNode | (CONV simpleFactorNode typeIdentNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t simple_factor (m2c_parser_context_t p);

static m2c_token_t factor (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t value_node, type_node;
  
  PARSER_DEBUG_INFO("factor");
  
  /* simpleFactor */
  lookahead = simple_factor(p);
  /* p->ast holds simple factor node */
  
  /* ( TypeConvOp typeIdent )? */
  if (match_token(p, TOKEN_TYPE_CONV)) {
    value_node = p->ast;
    
    /* '::' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* typeIdent */
    if (match_token(p, TOKEN_IDENT)) {
      lookahead = qualident(p);
      type_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(FACTOR)));
      type_node = m2c_ast_empty_node();
    } /* end if */
    
    /* build type conversion AST node */
    p->ast = m2c_ast_new_node(AST_CONV, value_node, type_node, NULL);
  } /* end if */
  
  return lookahead;
} /* end factor */


/* --------------------------------------------------------------------------
 * private function simple_factor()
 * --------------------------------------------------------------------------
 * simpleFactor :=
 *   NumberLiteral | StringLiteral |
 *   structuredValue | sourceDesignator | '(' expression ')'
 *   ;
 *
 * astnode:
 *  (INTVAL value) | (REALVAL value) | (CHRVAL value) | (QUOTEDVAL value) |
 *  structuredValueNode | designatorNode | exprNode
 * ----------------------------------------------------------------------- */

static m2c_token_t source_designator (m2c_parser_context_t p);
static m2c_token_t structured_value (m2c_parser_context_t p);

static m2c_token_t simple_factor (m2c_parser_context_t p) {
  intstr_t lexeme;
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("simpleFactor");
  
  lookahead = m2c_next_sym(p->lexer);
  
  switch (lookahead) {
    /* sourceDesignator */
    case TOKEN_IDENT :
      lookahead = source_designator(p);
      /* p->ast holds designator node */
      break;
      
    /* | WholeNumber */
    case TOKEN_WHOLE_NUMBER :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_current_lexeme(p->lexer);
      p->ast = ast_new_terminal_node(AST_INTVAL, lexeme);
      break;
      
    /* | RealNumber */
    case TOKEN_REAL_NUMBER :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_current_lexeme(p->lexer);
      p->ast = ast_new_terminal_node(AST_REALVAL, lexeme);
      break;
    
    /* | CharCode */
    case TOKEN_CHAR_CODE :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_current_lexeme(p->lexer);
      p->ast = ast_new_terminal_node(AST_CHRVAL, lexeme);
      break;
    
    /* | QuotedString */
    case TOKEN_QUOTED_STRING :
      lookahead = m2c_consume_sym(p->lexer);
      lexeme = m2c_current_lexeme(p->lexer);
      p->ast = ast_new_terminal_node(AST_QUOTEDVAL, lexeme);
      break;
      
    /* | '(' expression ')' */
    case TOKEN_LPAREN :
      /* '(' */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* expression */
      if (match_set(p, FIRST(EXPRESSION))) {
        lookahead = expression(p);
        /* p->ast holds expression node */
      }
      else /* resync */ {
        lookahead = skip_to_set(p, FOLLOW(SIMPLE_FACTOR));
        p->ast = m2c_ast_empty_node();
      } /* end if */
      
      /* ')' */
      if (match_token(p, TOKEN_RPAREN)) {
        lookahead = m2c_consume_sym(p->lexer);
      }
      else /* resync */ {
        lookahead = skip_to_set(p, FOLLOW(SIMPLE_FACTOR));
      } /* end if */      
      
    /* | structuredValue */
    case TOKEN_LBRACE :
      lookahead = structured_value(p);
      /* p->ast holds structured value node */
      break;
  } /* end switch */
  
  return lookahead;
} /* end simple_factor */


/* --------------------------------------------------------------------------
 * private function source_designator()
 * --------------------------------------------------------------------------
 * sourceDesignator :=
 *   qualident ( functionCallTail | derefSourceTail | bracketSourceTail )?
 *   ;
 *
 * astnode:
 *   identNode | (FCALL identNode tailNode) | (DEREF identNode tailNode) |
 *   (SUBSCR identNode tailNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t function_call_tail (m2c_parser_context_t p);

static m2c_token_t source_designator (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("sourceDesignator");
  
  /* qualident */
  lookahead = qualident(p);
  
  id_node = p->ast;
  
  /* ( functionCallTail | derefSourceTail | bracketSourceTail )? */
  if ((lookahead == TOKEN_LPAREN) {
    lookahead = function_call_tail(p);
    p->ast = m2c_ast_new_node(AST_FCALL, id_node, p->ast, NULL);
  }
  else if (lookahead == TOKEN_LBRACKET) {
    lookahead = bracket_source_tail(p);
    p->ast = m2c_ast_new_node(AST_SUBSCR, id_node, p->ast, NULL);
  }
  else if (lookahead == TOKEN_DEREF) {
    lookahead = deref_source_tail(p);
    p->ast = m2c_ast_new_node(AST_DEREF, id_node, p->ast, NULL);
  } /* end if */
  
  return lookahead;
} /* end source_designator */


/* --------------------------------------------------------------------------
 * private function function_call_tail()
 * --------------------------------------------------------------------------
 * functionCallTail :=
 *   '(' expressionList? ')'
 *   ;
 *
 * astnode: (EMPTY) | exprNode | exprListNode
 * ----------------------------------------------------------------------- */

static m2c_token_t function_call_tail (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t expr_list_node;
  
  PARSER_DEBUG_INFO("functionCallTail");
  
  /* '(' */
  lookahead = m2c_consume_sym(p->lexer);
  
  /* expressionList? */
  if (m2c_tokenset_element(FIRST(EXPRESSION), lookahead)) {
    lookahead = expression_list(p);
    expr_list_node = p->ast;
  }
  else /* empty */ {
    expr_list_node = m2c_ast_empty_node();
  } /* end if */
  
  /* ')' */
  if (match_token(p, TOKEN_RPAREN)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(FUNCTION_CALL_TAIL));
  } /* end if */
  
  /* pass AST back in p->ast */
  p->ast = expr_list_node;
  
  return lookahead;
} /* end function_call_tail */


/* --------------------------------------------------------------------------
 * private function deref_source_tail()
 * --------------------------------------------------------------------------
 * derefSourceTail :=
 *   deref ( '.' sourceDesignator | functionCallTail | bracketSourceTail )?
 *   ;
 *
 * astnode:
 * ----------------------------------------------------------------------- */

static m2c_token_t deref_source_tail (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("derefSourceTail");
  
  /* deref */
  lookahead = deref(p);
  deref_node = p->ast;
  
  /* ( '.' sourceDesignator | functionCallTail | bracketSourceTail )? */
  
  /* '.' sourceDesignator */
  if (lookahead == TOKEN_DOT) {
    /* '.' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* sourceDesignator */
    if (match_set(p, FIRST(DESIGNATOR))) {
      lookahead = designator(p);
      tail_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(DEREF_SOURCE_TAIL));
      tail_node = m2c_ast_empty_node();
    } /* end if */
  }
  
  /* functionCallTail */
  else if (lookahead == TOKEN_LPAREN) {
    lookahead = function_call_tail(p);
    tail_node = p->ast;
  }
  
  /* bracketSourceTail */
  else if (lookahead == TOKEN_LBRACKET) {
    lookahead = bracket_source_tail(p);
    tail_node = p->ast;
  }
  
  else /* no tail */ {
    tail_node = m2c_ast_empty_node();
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_node(AST_DEREFTAIL, deref_node, tail_node, NULL);
  
  return lookahead;
} /* end deref_source_tail */


/* --------------------------------------------------------------------------
 * private function bracket_source_tail()
 * --------------------------------------------------------------------------
 * bracketSourceTail :=
 *   '[' expression
 *   ( ']' ( '.' sourceDesignator | functionCallTail | derefSourceTail )? |
 *     '..' expression ']' )?
 *   ;
 *
 * astnode:
 * ----------------------------------------------------------------------- */

static m2c_token_t bracket_source_tail (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("bracketSourceTail");
  
  /* TO DO */
  
  return lookahead;
} /* end bracket_source_tail */


/* --------------------------------------------------------------------------
 * private function structured_value()
 * --------------------------------------------------------------------------
 * structuredValue :=
 *   '{' ( valueComponent ( ',' valueComponent )* ) '}'
 *   ;
 *
 * astnode: (STRUCT (EMPTY)) | (STRUCT valNode1 .. valNodeN)
 * ----------------------------------------------------------------------- */

static m2c_token_t value_component (m2c_parser_context_t p);

static m2c_token_t structured_value (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  
  PARSER_DEBUG_INFO("structuredValue");
  
  /* '{' */
  lookahead = m2c_consume_sym(p->lexer);
  
  val_list = m2c_fifo_new_queue(NULL);
  
  /* ( valueComponent (',' valueComponent)* )? */
  if (m2c_tokenset_element(FIRST(VALUE_COMPONENT, lookahead))) {
    /* valueComponent */
    lookahead = value_component(p);
    m2c_fifo_enqueue(val_list, p->ast);
    
    /* (',' valueComponent)* */
    while (lookahead == TOKEN_COMMA) {
      /* ',' */
      lookahead = m2c_consume_sym(p->lexer);
      
      /* valueComponent */
      if (match_set(p, FIRST(VALUE_COMPONENT))) {
        lookahead = value_component(p);
        m2c_fifo_enqueue(val_list, p->ast);
      }
      else /* resync */ {
        lookahead = skip_to_token_or_set(p, TOKEN_COMMA, TOKEN_RBRACE, NULL);
      } /* end if */
    } /* end while */
  } /* end if */
  
  /* '}' */
  if (match_token(p, TOKEN_RBRACE)) {
    lookahead = m2c_consume_sym(p->lexer);
  }
  else /* resync */ {
    lookahead = skip_to_set(p, FOLLOW(STRUCTURED_VALUE));
  } /* end if */
  
  /* build AST node and pass it back in p->ast */
  p->ast = m2c_ast_new_list_node(AST_STRUCT, val_list, NULL);
  
  return lookahead;
} /* end structured_value */


/* --------------------------------------------------------------------------
 * private function value_component()
 * --------------------------------------------------------------------------
 * valueComponent :=
 *   constExpression ( '..' constExpression )? |
 *   runtimeExpression
 *   ;
 *
 * astnode: exprNode | (CONSTRANGE exprNode exprNode)
 * ----------------------------------------------------------------------- */

static m2c_token_t value_component (m2c_parser_context_t p) {
  m2c_token_t lookahead;
  m2c_astnode_t expr1_node, expr2_node;
    
  PARSER_DEBUG_INFO("valueComponent");
  
  /* expression */
  lookahead = expression(p);
  /* p->ast holds expression node */
  
  /* ( '..' expression )? */
  if (lookahead == TOKEN_DOT_DOT) {
    expr1_node = p->ast;
    
    /* '..' */
    lookahead = m2c_consume_sym(p->lexer);
    
    /* expression */
    if (match_set(p, FIRST(EXPRESSION))) {
      lookahead = expression(p);
      expr2_node = p->ast;
    }
    else /* resync */ {
      lookahead = skip_to_set(p, FOLLOW(VALUE_COMPONENT));
      expr2_node = m2c_ast_empty_node();
    } /* end if */
    
    /* build constant range AST node */
    p->ast =
      m2c_ast_new_list_node(AST_CONSTRANGE, expr1_node, expr2_node, NULL);
  } /* end if */
  
  return lookahead;
} /* end value_component */

/* END OF FILE */