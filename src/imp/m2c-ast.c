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
 * m2c-ast.c                                                                 *
 *                                                                           *
 * Implementation of the abstract syntax tree (AST) module.                  *
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

#include "m2c-ast.h"

#include <stddef.h>
#include <stdlib.h>


/* --------------------------------------------------------------------------
 * hidden type m2c_astnode_struct_t
 * --------------------------------------------------------------------------
 * record type representing an AST node object.
 * ----------------------------------------------------------------------- */

struct m2c_astnode_struct_t {
  /* node_type */     m2c_ast_nodetype_t node_type;
  /* subnode_count */ unsigned short subnode_count;
  /* subnode_table */ m2c_astnode_variant subnode_table[];
};

typedef struct m2c_astnode_struct_t m2c_astnode_struct_t;

union m2c_astnode_variant {
  intstr_t terminal;
  m2c_astnode_t non_terminal;
};

typedef union m2c_astnode_variant m2c_astnode_variant;


/* --------------------------------------------------------------------------
 * empty node singleton
 * ----------------------------------------------------------------------- */

static const m2c_astnode_struct_t m2c_ast_empty_node_struct = {
  AST_EMPTY, 0, NULL
};


/* --------------------------------------------------------------------------
 * function m2c_ast_empty_node()
 * --------------------------------------------------------------------------
 * Returns the empty node singleton.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_empty_node (void) {
  return (m2c_astnode_t) &m2c_ast_empty_node_struct;
} /* end m2c_ast_empty_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_new_node(node_type, subnode0, subnode1, subnode2, ...)
 * --------------------------------------------------------------------------
 * Allocates a new branch node of the given node type, stores the subnodes of
 * the argument list in the node and returns the node, or NULL on failure.
 *
 * pre-conditions:
 * o  node_type must be a valid node type
 * o  a NULL terminated argument list of valid ast nodes must be passed
 *    and type and number of subnodes must match the given node type.
 *
 * post-conditions:
 * o  newly allocated and populated ast node is returned
 *
 * error-conditions:
 * o  if allocation fails, no node is allocated and NULL is returned
 * o  if node_type is invalid, no node is allocated and NULL is returned
 * o  if type and number of subnodes does not match the given node type,
 *    no node is allocated and NULL is returned
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_new_node
  (m2c_ast_nodetype_t node_type, ...) {
  
  m2c_astnode_t this_subnode, new_node;
  unsigned short subnode_count, index;
  va_list subnode_list;
  
  if (!m2c_ast_is_nonterminal_nodetype(node_type)) {
    return NULL;
  } /* end if */
  
  /* count subnodes in arglist */
  subnode_count = 0;
  va_start(subnode_list, node_type);
  this_subnode = va_arg(subnode_list, m2c_astnode_t);
  while (this_subnode != NULL) {
    subnode_count++;
    this_subnode = va_arg(subnode_list, m2c_astnode_t);
  } /* end while */
  
  va_end(subnode_list);
  
  /* verify subnode count */
  if (!m2c_ast_is_legal_subnode_count(node_type, subnode_count)) {
    return NULL;
  } /* end if */
  
  if (node_type == AST_EMPTY) {
    return (m2c_astnode_t) &m2c_ast_empty_node_struct;
  } /* end if */
  
  /* allocate node */
  new_node = malloc
    (sizeof(m2c_astnode_struct_t) + subnode_count * sizeof(m2c_astnode_t));
  
  /* initialise fields */
  new_node->node_type = node_type;
  new_node->subnode_count = subnode_count;
  
  /* store subnodes in table */
  va_start(subnode_list, node_type);
  for (index = 0; index < subnode_count; index++) {
    new_node->subnode_table[index].non_terminal =
      va_arg(subnode_list, m2c_astnode_t);
  } /* end for */
  
  va_end(subnode_list);
  
  return new_node;
} /* end m2c_ast_new_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_new_list_node(node_type, node_list)
 * --------------------------------------------------------------------------
 * Allocates a new branch node of the given node type, stores the subnodes of
 * the given node list in the node and returns the node, or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_new_list_node
  (m2c_ast_nodetype_t node_type, m2c_fifo_t node_list) {
  
  m2c_astnode_t new_node;
  unsigned short subnode_count, index;
  
  if (!AST_IS_NONTERMINAL_LIST_NODETYPE(node_type)) {
    return NULL;
  } /* end if */
    
  if (node_list == NULL) {
    return NULL;
  } /* end if */
  
  subnode_count = m2c_fifo_entry_count(node_list);
  
  if (subnode_count == 0) {
    return NULL;
  } /* end if */
  
  /* allocate node */
  new_node = malloc
    (sizeof(m2c_astnode_struct_t) + subnode_count * sizeof(m2c_astnode_t));
  
  /* initialise fields */
  new_node->node_type = node_type;
  new_node->subnode_count = subnode_count;
  
  /* store subnodes in table */
  for (index = 0; index < subnode_count; index++) {
    new_node->subnode_table[index].non_terminal = m2c_fifo_dequeue(node_list);
  } /* end for */
  
  return new_node;
} /* end m2c_ast_new_list_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_new_terminal_node(node_type, value)
 * --------------------------------------------------------------------------
 * Allocates a  new terminal node  of the given  node type,  stores the given
 * value in the node and returns the node, or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_new_terminal_node
  (m2c_ast_nodetype_t type, intstr_t value) {
  
  m2c_astnode_t new_node;
  
  if (!AST_IS_TERMINAL_NODETYPE(node_type)) {
    return NULL;
  } /* end if */
  
  /* verify subnode count */
  if (!m2c_ast_is_legal_subnode_count(node_type, 1)) {
    return NULL;
  } /* end if */
  
  /* allocate node */
  new_node = malloc
    (sizeof(m2c_astnode_struct_t) + sizeof(m2c_astnode_t));
  
  /* initialise fields */
  new_node->node_type = node_type;
  new_node->subnode_count = 1;
  
  /* store value */
  new_node->subnode_table[0].terminal = value;
  
  return new_node;
} /* end m2c_ast_new_terminal_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_new_terminal_list_node(node_type, value_list)
 * --------------------------------------------------------------------------
 * Allocates a new terminal node of the given node type, stores the values of
 * the given value list in the node and returns the node, or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_new_terminal_list_node
  (m2c_ast_nodetype_t node_type, m2c_fifo_t value_list) {
  
  m2c_astnode_t new_node;
  unsigned short subnode_count, index;
  
  if (!AST_IS_TERMINAL_LIST_NODETYPE(node_type)) {
    return NULL;
  } /* end if */
  
  if (value_list == NULL) {
    return NULL;
  } /* end if */
  
  subnode_count = m2c_fifo_entry_count(value_list);
  
  if (subnode_count == 0) {
    return NULL;
  } /* end if */
  
  /* allocate node */
  new_node = malloc
    (sizeof(m2c_astnode_struct_t) + subnode_count * sizeof(m2c_astnode_t));
  
  /* initialise fields */
  new_node->node_type = node_type;
  new_node->subnode_count = subnode_count;
  
  /* store values in table */
  for (index = 0; index < subnode_count; index++) {
    new_node->subnode_table[index].terminal = m2c_fifo_dequeue(value_list);
  } /* end for */
  
  return new_node;
} /* end m2c_ast_new_terminal_list_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_nodetype(node)
 * --------------------------------------------------------------------------
 * Returns the node type of node,  or AST_INVALID if node is NULL.
 * ----------------------------------------------------------------------- */

m2c_ast_nodetype_t m2c_ast_nodetype (m2c_astnode_t node) {
  
  if (node == NULL) {
    return AST_INVALID;
  } /* end if */
  
  return node->node_type;  
} /* end m2c_ast_nodetype */


/* --------------------------------------------------------------------------
 * function m2c_ast_subnode_count(node)
 * --------------------------------------------------------------------------
 * Returns the number of subnodes or values of node. 
 * ----------------------------------------------------------------------- */

unsigned short m2c_ast_subnode_count (m2c_astnode_t node) {
  
  if (node == NULL) {
    return 0;
  } /* end if */
  
  return node->subnode_count;  
} /* end m2c_ast_subnode_count */


/* --------------------------------------------------------------------------
 * function m2c_ast_subnode_at_index(node, index)
 * --------------------------------------------------------------------------
 * Returns the subnode of node with the given index or NULL if no subnode of
 * the given index is stored in node.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_subnode_at_index
  (m2c_astnode_t node, unsigned short index) {
  
 /* end m2c_ast_subnode_at_index */}


/* --------------------------------------------------------------------------
 * function m2c_ast_value_at_index(terminal_node, index)
 * --------------------------------------------------------------------------
 * Returns the value stored at the given index in a terminal node,
 * or NULL if the node does not store any value at the given index.
 * ----------------------------------------------------------------------- */

intstr_t m2c_ast_value_at_index
  (m2c_astnode_t node, unsigned short index) {
  
} /* end m2c_ast_value_at_index */


/* --------------------------------------------------------------------------
 * function m2c_ast_replace_subnode(in_node, at_index, with_subnode)
 * --------------------------------------------------------------------------
 * Replaces a subnode and returns the replaced node,  or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_replace_subnode
  (m2c_astnode_t in_node, unsigned short at_index, m2c_astnode_t with_subnode) {
  
} /* end m2c_ast_replace_subnode */


/* --------------------------------------------------------------------------
 * function m2c_ast_replace_value(in_node, at_index, with_value)
 * --------------------------------------------------------------------------
 * Replaces a value and returns the replaced value,  or NULL on failure.
 * ----------------------------------------------------------------------- */

intstr_t m2c_ast_replace_value
  (m2c_astnode_t in_node, unsigned short at_index, intstr_t with_value) {
  
} /* end m2c_ast_replace_value */


/* --------------------------------------------------------------------------
 * function m2c_ast_release_node(node)
 * --------------------------------------------------------------------------
 * Deallocates node. 
 * ----------------------------------------------------------------------- */

void m2c_ast_release_node (m2c_astnode_t node);


/* END OF FILE */