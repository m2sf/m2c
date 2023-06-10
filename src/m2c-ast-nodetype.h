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
 * m2c-ast-nodetype.h                                                        *
 *                                                                           *
 * Public interface for the AST nodetype module.                             *
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

#ifndef M2C_AST_NODETYPE_H
#define M2C_AST_NODETYPE_H

/* --------------------------------------------------------------------------
 * imports
 * ----------------------------------------------------------------------- */

#include <stdbool.h>


/* --------------------------------------------------------------------------
 * type m2c_astnode_type_t
 * --------------------------------------------------------------------------
 * Enumerated values representing AST node types.
 * ----------------------------------------------------------------------- */

typedef enum {

  /* Invalid Sentinel */
  
  AST_INVALID,

  /* Empty Node Type */
  
  AST_EMPTY,
  
  /* Root Node Type */
  
  AST_FILE,
  
  /* N o n - T e r m i n a l   N o d e   T y p e s */
  
  /* Module Node Types */
  
  AST_INTERFACE,        /* interface module */
  AST_IMPLEMENTATION,   /* implementation module */
  AST_PROGRAM,          /* program module */
  
  /* Import Node Types */
  
  AST_IMPORT,           /* import */
  AST_REEXP,            /* re-export */
    
  /* Definition and Declaration Node Types */
  
  AST_CONST,            /* constant definition */
  AST_TYPE,             /* type definition */
  AST_PROCDEF,          /* procedure definition */
  AST_PROCDECL,         /* procedure declaration */
  AST_UNQ,              /* unqualified alias definition */
  AST_TODO,             /* to do entry */
  
  /* Type Constructor Node Types */
  
  AST_ALIAS             /* alias type */
  AST_SUBR,             /* subrange type */
  AST_ENUM,             /* enumeration type */
  AST_SET,              /* set type node */
  AST_ARRAY,            /* array type */
  AST_RECORD,           /* simple record type */
  AST_OPAQUE,           /* opaque type */
  AST_POINTER,          /* pointer type */
  AST_PROCTYPE,         /* procedure type */  
    
  /* Record Field Node Types */
  
  AST_FIELD,            /* record field */
  AST_INDFIELD,         /* indeterminate field */
  
  /* Formal Type and Formal Parameter Node Types */
  
  AST_OPENARRAY,        /* open array formal type */
  AST_CONSTP,           /* CONST formal type */
  AST_VARP,             /* VAR formal type */
  AST_ARGLIST,          /* variadic formal type*/
  AST_CASTADDR,         /* casting formal type */
  AST_CASTOCTSEQ,       /* casting formal type */
  AST_FPARAMS,          /* formal parameters */

  /* Implementation/Program Module Node Types */

  AST_BLOCK,            /* block */    
  
  AST_ASSIGN,           /* assignment */
  AST_COPY,             /* copy statement */
  AST_PCALL,            /* procedure call */
  AST_RETURN,           /* RETURN statement */
  AST_NEW,              /* NEW statement */
  AST_NEWARG,           /* NEW with initialisation */
  AST_NEWCAP,           /* NEW with capacity */
  AST_RETAIN,           /* RETAIN statement */
  AST_RELEASE,          /* RELEASE statement */
  
  AST_IF,               /* IF statement */
  AST_ELSIF,            /* ELSIF branch */
  
  AST_SWITCH,           /* CASE statement */
  AST_CASE,             /* case branch */
  AST_CASELBL,          /* case labels */
  
  AST_LOOP,             /* LOOP statement */
  AST_WHILE,            /* WHILE statement */
  AST_REPEAT,           /* REPEAT statement */
  AST_FOR,              /* FOR statement */
  AST_EXIT,             /* EXIT statement */
  
  AST_READ,             /* READ statement */
  AST_READNEW,          /* READ with NEW */
  AST_WRITE,            /* WRITE statement */
  AST_WRITEF,           /* WRITE formatted */
  AST_NOP,              /* NOP statement */
    
  /* Expression Node Types */
      
  AST_EXPR,             /* expression */
  
  AST_EQ,               /* equality */
  AST_NEQ,              /* inequality */
  AST_LT,               /* less-than, strict subset */
  AST_LTEQ,             /* less-than-or-equal, subset */
  AST_GT,               /* greater-than, strict superset */
  AST_GTEQ,             /* greater-than-or-equal, superset */
  AST_IDTY,             /* identity */
  AST_IN,               /* membership */
  
  AST_PLUS,             /* sum */
  AST_MINUS,            /* difference */
  AST_NEG,              /* arithmetic negation */
  AST_CONCAT,           /* concatenation */
  AST_SETDIFF,          /* set difference */
  AST_OR,               /* logical disjunction */
  
  AST_ASTERISK,         /* multiplication, intersection */
  AST_SOLIDUS,          /* real division, symmetric difference */
  AST_DIV,              /* integer division */
  AST_MOD,              /* modulus */
  AST_AND,              /* logical conjunction */
  
  AST_NOT,              /* logical negation */
  
  AST_TYPECONV,         /* type conversion */
  
  /* Designator Node Types */
  
  AST_DESIG,            /* designator */
  
  AST_SUBSCRIPT,        /* subscript */
  AST_DEREF,            /* dereference */
  AST_SELECT,           /* name or field selection */
  AST_FCALL,            /* function call */
  
  /* Compound Value Node Types */
  
  AST_STRUCT,           /* structured value */
  AST_RANGE,            /* value range */
  
  /* L-Designator Node Types */
  
  AST_INSERT,           /* insertion */
  AST_SLICE,            /* slice */
  
  /* N o n - T e r m i n a l   L i s t - N o d e   T y p e s */
  
  AST_IMPLIST,          /* import list */
  AST_RXPLIST,          /* re-export list */
  AST_DEFLIST,          /* definition list */
  AST_IDENTLIST,        /* identifier list */
  AST_FIELDLISTSEQ,     /* field list sequence */
  AST_FTYPELIST,        /* formal type list */
  AST_FPARAMLIST,       /* formal parameter list */
  AST_STMTSEQ,          /* statement sequence */
  AST_ELSIFSEQ,         /* ELSIF branch sequence */
  AST_CASELIST,         /* case list */
  AST_EXPRLIST,         /* expression list */
  AST_ARGS,             /* actual parameter list */
  
  /* T e r m i n a l   N o d e   T y p e s */
  
  /* Identifier Node Types */
  
  AST_IDENT,            /* identifier */
  AST_FILENAME,         /* module file name */
  AST_KEY,              /* module key */
  
  /* Literal Value Node Types */
  
  AST_INTVAL,           /* whole number value */
  AST_REALVAL,          /* real number value */
  AST_CHRVAL,           /* character code value */
  AST_QUOTEDVAL,        /* quoted literal value */
    
  /* T e r m i n a l   L i s t   N o d e   T y p e s */
  
  AST_IDENTLIST,        /* identifier list */
  AST_QUALIDENT,        /* qualified identifier */
  
  /* Enumeration Terminator */
  
  AST_END_MARK   /* marks the end of this enumeration */
  
} m2c_astnode_type_t;


/* --------------------------------------------------------------------------
 * AST node type groupings.
 * ----------------------------------------------------------------------- */

#define AST_FIRST_VALID_NODETYPE (AST_INVALID+1)
#define AST_LAST_VALID_NODETYPE (AST_END_MARK-1)

#define AST_FIRST_NONTERMINAL AST_EMPTY
#define AST_LAST_NONTERMINAL AST_SLICE

#define AST_FIRST_NONTERMINAL_LIST AST_IMPLIST
#define AST_LAST_NONTERMINAL_LIST AST_ARGS

#define AST_FIRST_TERMINAL AST_IDENT
#define AST_LAST_TERMINAL AST_QUOTEDVAL

#define AST_FIRST_TERMINAL_LIST AST_IDENTLIST
#define AST_LAST_TERMINAL_LIST AST_QUALIDENT

#define AST_FIRST_EXPRESSION AST_EXPR
#define AST_LAST_EXPRESSION AST_STRUCT

#define AST_FIRST_LITERAL AST_INTVAL
#define AST_LAST_LITERAL AST_QUOTEDVAL


/* --------------------------------------------------------------------------
 * macro AST_IS_VALID_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is a valid node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_VALID_NODETYPE(_type) \
  ((_type >= AST_FIRST_VALID_NODETYPE) && (_type <= AST_LAST_VALID_NODETYPE))


/* --------------------------------------------------------------------------
 * macro AST_IS_NONTERMINAL_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is a non-terminal node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_NONTERMINAL_NODETYPE(_type) \
  ((_type >= AST_FIRST_NONTERMINAL) && (_type <= AST_LAST_NONTERMINAL))


/* --------------------------------------------------------------------------
 * macro AST_IS_NONTERMINAL_LIST_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is a non-terminal list node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_NONTERMINAL_LIST_NODETYPE(_type) \
  ((_type >= AST_FIRST_NONTERMINAL_LIST) \
   && (_type <= AST_LAST_NONTERMINAL_LIST))


/* --------------------------------------------------------------------------
 * macro AST_IS_TERMINAL_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is a terminal node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_TERMINAL_NODETYPE(_type) \
  ((_type >= AST_FIRST_TERMINAL) && (_type <= AST_LAST_TERMINAL))


/* --------------------------------------------------------------------------
 * macro AST_IS_TERMINAL_LIST_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is a terminal list node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_TERMINAL_LIST_NODETYPE(_type) \
  ((_type >= AST_FIRST_TERMINAL_LIST) && (_type <= AST_LAST_TERMINAL_LIST))


/* --------------------------------------------------------------------------
 * macro AST_IS_EXPRESSION_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is an expression node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_EXPRESSION_NODETYPE(_type) \
  ((_type >= AST_FIRST_EXPRESSION) && (_type <= AST_LAST_EXPRESSION))


/* --------------------------------------------------------------------------
 * macro AST_IS_LITERAL_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is a literal node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_LITERAL_NODETYPE(_type) \
  ((_type >= AST_FIRST_LITERAL) && (_type <= AST_LAST_LITERAL))


/* --------------------------------------------------------------------------
 * macro AST_IS_LIST_NODETYPE(node_type)
 * --------------------------------------------------------------------------
 * Returns true if node_type is a list node type, else false.
 * ----------------------------------------------------------------------- */

#define AST_IS_LIST_NODETYPE(_type) \
  ((AST_IS_NONTERMINAL_LIST_NODETYPE(_type)) ||
   (AST_IS_TERMINAL_LIST_NODETYPE(_type)))


/* --------------------------------------------------------------------------
 * function m2c_ast_is_legal_subnode_count(node_type, subnode_count)
 * --------------------------------------------------------------------------
 * Returns true if the given subnode count is a legal value for the given
 * node type, otherwise false.
 * ----------------------------------------------------------------------- */

bool m2c_ast_is_legal_subnode_count
  (m2c_ast_nodetype_t node_type, unsigned short subnode_count);


/* --------------------------------------------------------------------------
 * function m2c_name_for_nodetype(node_type)
 * --------------------------------------------------------------------------
 * Returns an immutable pointer to a NUL terminated character string with
 * a human readable name for node_type, or NULL if node_type is invalid.
 * ----------------------------------------------------------------------- */

const char *m2c_name_for_nodetype (m2c_ast_nodetype_t node_type);


#endif /* M2C_AST_NODETYPE_H */

/* END OF FILE */