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
 * m2c-ast-nodetype.c                                                        *
 *                                                                           *
 * Implementation of the AST nodetype module.                                *
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

#include "m2c-ast-nodetype.h"
#include <stddef.h>


/* --------------------------------------------------------------------------
 * Arity by node type
 * ----------------------------------------------------------------------- */

static uint8_t arity_table[] = {
  0, /* AST_INVALID */
  0, /* AST_EMPTY */ 
  3, /* AST_FILE */ 
  4, /* AST_INTERFACE */ 
  4, /* AST_IMPLEMENTATION */ 
  4, /* AST_PROGRAM */
  ?, /* AST_IMPORT */
  ?  /* AST_REEXP */
  3, /* AST_CONST */
  2, /* AST_TYPE */
  4, /* AST_PROCDEF */
  3, /* AST_PROCDECL */
  ?, /* AST_UNQ */
  ?, /* AST_TODO */
  2, /* AST_ALIAS */
  3, /* AST_SUBR */
  3, /* AST_ENUM */
  2, /* AST_SET */
  3, /* AST_ARRAY */
  3, /* AST_RECORD */
  2, /* AST_OPAQUE */
  2, /* AST_POINTER */
  ?, /* AST_PROCTYPE */  
  2, /* AST_FIELD */
  3, /* AST_INDFIELD */
  ?, /* AST_OPENARRAY */
  ?, /* AST_CONSTP */
  ?, /* AST_VARP */
  ?, /* AST_ARGLIST */
  ?, /* AST_CASTADDR */
  ?, /* AST_CASTOCTSEQ */
  ?, /* AST_FPARAMS */
  2, /* AST_BLOCK */    
  2, /* AST_ASSIGN */
  2, /* AST_COPY */
  2, /* AST_PCALL */
  1, /* AST_RETURN */
  2, /* AST_NEW */
  3, /* AST_NEWARG */
  3, /* AST_NEWCAP */
  1, /* AST_RETAIN */
  1, /* AST_RELEASE */
  ?, /* AST_IF */
  ?, /* AST_ELSIF */
  ?, /* AST_SWITCH */
  2, /* AST_CASE */
  2, /* AST_CASELBL */
  1, /* AST_LOOP */
  2, /* AST_WHILE */
  2, /* AST_REPEAT */
  3, /* AST_FOR */
  0, /* AST_EXIT */
  2, /* AST_READ */
  2, /* AST_READNEW */
  2, /* AST_WRITE */
  3, /* AST_WRITEF */
  0, /* AST_NOP */
  ?, /* AST_EXPR */
  2, /* AST_EQ */
  2, /* AST_NEQ */
  2, /* AST_LT */
  2, /* AST_LTEQ */
  2, /* AST_GT */
  2, /* AST_GTEQ */
  2, /* AST_IDTY */
  2, /* AST_IN */
  2, /* AST_PLUS */
  2, /* AST_MINUS */
  1, /* AST_NEG */
  2, /* AST_CONCAT */
  2, /* AST_SETDIFF */
  2, /* AST_OR */
  2, /* AST_ASTERISK */
  2, /* AST_SOLIDUS */
  2, /* AST_DIV */
  2, /* AST_MOD */
  2, /* AST_AND */
  1, /* AST_NOT */
  2, /* AST_TYPECONV */
  ?, /* AST_DESIG */
  2, /* AST_SUBSCRIPT */
  1, /* AST_DEREF */
  1, /* AST_SELECT */
  2, /* AST_FCALL */
  ?, /* AST_STRUCT */
  2, /* AST_RANGE */
  2, /* AST_INSERT */
  3, /* AST_SLICE */
}; /* end arity_table */


/* --------------------------------------------------------------------------
 * Human readable names for node types
 * ----------------------------------------------------------------------- */

static const char *name_table[] = {
  "\0", "EMPTY", "FILE", "INTERFACE", "IMPLEMENTATION", "PROGRAM", "IMPORT",
  "RE-EXPORT", "CONST", "TYPE", "PROCDEF", "PROCDECL", "UNQ", "TO-DO",
  "ALIAS", "SUBR", "ENUM", "SET", "ARRAY", "RECORD", "OPAQUE", "POINTER",
  "PROCTYPE", "FIELD", "INDFIELD", "OPEN-ARRAY", "CONSTP", "VARP", "ARGLIST",
  "CAST-ADDR", "CAST-OCTSEQ", "FPARAMS", "BLOCK", "ASSIGN", "COPY", "PCALL",
  "RETURN", "NEW", "NEWARG", "NEWCAP", "RETAIN", "RELEASE", "IF", "ELSIF",
  "SWITCH", "CASE", "CASELBL", "LOOP", "WHILE", "REPEAT", "FOR", "EXIT",
  "READ", "READ-NEW", "WRITE", "WRITEF", "NOP", "EXPR", "EQ", "NEQ", "LT",
  "LTEQ", "GT", "GTEQ", "IDTY", "IN", "+", "-", "NEG", "CONCAT", "SETDIFF",
  "OR", "*", "/", "DIV", "MOD", "AND", "NOT", "TYPE-CONV", "DESIG",
  "SUBSCRIPT", "DEREF", "SELECT", "FCALL", "STRUCT", "RANGE", "INSERT",
  "SLICE", "IMPORT-LIST", "RE-EXPORT-LIST", "DEF-LIST", "FIELDLIST-SEQ",
  "FTYPE-LIST", "FPARAM-LIST", "STMT-SEQ", "ELSIF-SEQ", "CASE-LIST",
  "EXPR-LIST", "ARGS", "IDENT", "FILENAME", "KEY", "INTVAL", "REALVAL",
  "CHRVAL", "QUOTEDVAL", "IDENT-LIST", "QUALIDENT"
}; /* end name_table */


/* --------------------------------------------------------------------------
 * function m2c_ast_is_legal_subnode_count(node_type, subnode_count)
 * --------------------------------------------------------------------------
 * Returns true if the given subnode count is a legal value for the given
 * node type, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_ast_is_legal_subnode_count
  (m2c_ast_nodetype_t node_type, unsigned short subnode_count) {
  
  /* for non-terminal nodes, look up arity in table */
  if (AST_IS_NONTERMINAL_NODETYPE(node_type)) {
    if (subnode_count == arity_table[node_type]) {
      return true;
    } /* end if */
  }
  /* for list nodes, arity is always variable but non-zero */
  else if (AST_IS_LIST_NODETYPE(node_type)) {
    if (subnode_count > 0) {
      return true;
    } /* end if */
  }
  /* for terminal nodes, arity is always one */
  else if (AST_IS_TERMINAL_NODETYPE(node_type)) {
    if (subnode_count == 1) {
      return true;
    } /* end if */
  } /* end if */
  
  /* otherwise subnode count is invalid */
  return false;
} /* end m2c_ast_is_legal_subnode_count */


/* --------------------------------------------------------------------------
 * function m2c_name_for_nodetype(node_type)
 * --------------------------------------------------------------------------
 * Returns a const pointer to a NUL terminated character string with a human
 * readable name for node_type,  or empty string if node_type is invalid.
 * ----------------------------------------------------------------------- */

const char *m2c_name_for_nodetype (m2c_ast_nodetype_t node_type) {
  if (AST_IS_VALID_NODETYPE(node_type)) {
    return name_table[node_type];
  }
  else /* invalid node type */ {
    return name_table[0];
  } /* end if */
} /* end m2c_name_for_nodetype */


/* END OF FILE */