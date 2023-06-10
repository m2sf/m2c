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
  
} /* end m2c_ast_new_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_new_list_node(node_type, node_list)
 * --------------------------------------------------------------------------
 * Allocates a new branch node of the given node type, stores the subnodes of
 * the given node list in the node and returns the node, or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_new_list_node
  (m2c_ast_nodetype_t node_type, m2c_fifo_t node_list) {
  
} /* end m2c_ast_new_list_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_new_terminal_node(node_type, value)
 * --------------------------------------------------------------------------
 * Allocates a  new terminal node  of the given  node type,  stores the given
 * value in the node and returns the node, or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_new_terminal_node
  (m2c_ast_nodetype_t type, intstr_t value) {
  
} /* end m2c_ast_new_terminal_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_new_terminal_list_node(node_type, value_list)
 * --------------------------------------------------------------------------
 * Allocates a new terminal node of the given node type, stores the values of
 * the given value list in the node and returns the node, or NULL on failure.
 * ----------------------------------------------------------------------- */

m2c_astnode_t m2c_ast_new_terminal_list_node
  (m2c_ast_nodetype_t node_type, m2c_fifo_t value_list) {
  
} /* end m2c_ast_new_terminal_list_node */


/* --------------------------------------------------------------------------
 * function m2c_ast_nodetype(node)
 * --------------------------------------------------------------------------
 * Returns the node type of node,  or AST_INVALID if node is NULL.
 * ----------------------------------------------------------------------- */

m2c_ast_nodetype_t m2c_ast_nodetype (m2c_astnode_t node) {

} /* end m2c_ast_nodetype */


/* --------------------------------------------------------------------------
 * function m2c_ast_subnode_count(node)
 * --------------------------------------------------------------------------
 * Returns the number of subnodes or values of node. 
 * ----------------------------------------------------------------------- */

unsigned short m2c_ast_subnode_count (m2c_astnode_t node) {

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