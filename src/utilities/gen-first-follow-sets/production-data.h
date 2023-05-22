/* Modula-2 R10 grammar production database */

/* #define PROD(_allcaps, _id, _first, _follow) */

PROD(COMPILATION_UNIT, compilationUnit,
  (TOKEN_DEFINITION, TOKEN_IMPLEMENTATION, TOKEN_MODULE, 0),
  (TOKEN_EOF, 0)
),
PROD(DEFINITION_MODULE, definitionModule,
  (TOKEN_DEFINITION, 0),
  (TOKEN_EOF, 0)
),
PROD(IMPORT, import,
  (TOKEN_IMPORT, 0),
  (TOKEN_IMPORT, TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE,
   TOKEN_TO, TOKEN_END, 0)
),
PROD(DEFINITION, definition,
  (TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_TO, 0),
  (TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_TO, 0)
),
PROD(CONST_DEFINITION, constDefinition,
  (TOKEN_LBRACKET, TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(CONSTANT_BINDING, constantBinding,
  (TOKEN_LBRACKET,0),
  (TOKEN_SEMICOLON, 0)
),
PROD(CONST_DECLARATION, constDeclaration,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(CONST_EXPRESSION, constExpression,
  (TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, M2C_TOKEN_IDENT, M2C_TOKEN_NOT,
   TOKEN_LBRACE, TOKEN_LPAREN, TOKEN_ASTERISK, TOKEN_SOLIDUS,
   TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_MINUS, 0),
  (TOKEN_SEMICOLON, TOKEN_DOT_DOT, TOKEN_RBRACKET, TOKEN_OF, TOKEN_COMMA,
   TOKEN_COLON, TOKEN_RBRACE, 0)
),
PROD(TYPE_DEFINITION, typeDefinition,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(ALIAS_TYPE, aliasType,
  (TOKEN_ALIAS, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(QUALIDENT, qualident,
  (TOKEN_IDENT, 0),
  (TOKEN_DOT, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_LPAREN, TOKEN_RPAREN,
  TOKEN_LBRACKET, TOKEN_WILDCARD, TOKEN_DEREF, TOKENDO, TOKEN_END, 0)
),
PROD(SUBRANGE_TYPE, subrangeType,
  (TOKEN_LBRACKET, 0),
  (TOKEN_SEMICOLON, TOKEN_END, 0)
),
PROD(CONST_RANGE, constRange,
  (TOKEN_LBRACKET, 0),
  (TOKEN_OF, 0)
),
PROD(ENUM_TYPE, enumType,
  (TOKEN_LPAREN, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(IDENT_LIST, identList,
  (TOKEN_IDENT, 0),
  (TOKEN_COLON, TOKEN_RPAREN, 0)
),
PROD(SET_TYPE, setType,
  (TOKEN_SET, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(ARRAY_TYPE, arrayType,
  (TOKEN_ARRAY, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(RECORD_TYPE, recordType,
  (TOKEN_RECORD, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(REC_TYPE_TO_EXTEND, recTypeToExtend,
  (TOKEN_IDENT, 0),
  (TOKEN_RPAREN, 0)
),
PROD(FIELD_LIST, fieldList,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, TOKEN_END, 0)
),
PROD(POINTER_TYPE, pointerType,
  (TOKEN_POINTER, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(OPAQUE_TYPE, opaqueType,
  (TOKEN_OPAQUE, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(PROCEDURE_TYPE, procedureType,
  (TOKEN_PROCEDURE, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(FORMAL_TYPE, formalType,
  (TOKEN_CONST, TOKEN_VAR, TOKEN_ARRAY, TOKEN_IDENT, TOKEN_ARGLIST, 0),
  (TOKEN_SEMICOLON, TOKEN_RPAREN, 0)
),
PROD(NON_ATTR_FORMAL_TYPE, nonAttrFormalType,
  (TOKEN_ARRAY, TOKEN_IDENT, TOKEN_ARGLIST, 0),
  (TOKEN_SEMICOLON, TOKEN_RPAREN, 0)
),
PROD(SIMPLE_FORMAL_TYPE, simpleFormalType,
  (TOKEN_ARRAY, TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, TOKEN_RPAREN, 0)
),
PROD(CASTING_FORMAL_TYPE, castingFormalType,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, TOKEN_RPAREN, 0)
),
PROD(VARIADIC_FORMAL_TYPE, variadicFormalType,
  (TOKEN_ARGLIST, 0),
  (TOKEN_SEMICOLON, TOKEN_RPAREN, 0)
),
PROD(PROCEDURE_HEADER, procedureHeader,
  (TOKEN_PROCEDURE, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(BINDING_SPECIFIER, bindingSpecifier,
  (TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_READ, TOKEN_WRITE,
   TOKEN_IDENT, 0),
  (TOKEN_RBRACKET, 0)
),
PROD(BINDABLE_IDENT, bindableIdent,
  (TOKEN_IDENT, 0),
  (TOKEN_RBRACKET, 0)
),
PROD(PROCEDURE_SIGNATURE, procedureSignature,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(FORMAL_PARAMS, formalParams,
  (TOKEN_CONST, TOKEN_VAR, TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, TOKEN_RPAREN, 0)
),
PROD(PROGRAM_MODULE, programModule,
  (TOKEN_MODULE, 0),
  (TOKEN_EOF, 0)
),
PROD(PRIVATE_IMPORT, privateImport,
  (TOKEN_IMPORT, 0),
  (TOKEN_IMPORT, TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE,
   TOKEN_UNQUALIFIED, TOKEN_TO, TOKEN_BEGIN, TOKEN_END, 0)
),
PROD(BLOCK, block,
  (TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
   TOKEN_TO, TOKEN_BEGIN, 0),
  (TOKEN_IDENT, 0)
),
PROD(IMPLEMENTATION_MODULE, implementationModule,
  (TOKEN_IMPLEMENTATION, 0),
  (TOKEN_EOF, 0)
),
PROD(POSSIBLY_EMPTY_BLOCK, possiblyEmptyBlock,
  (TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
   TOKEN_TO, TOKEN_BEGIN, TOKEN_END, 0),
  (TOKEN_IDENT, 0)
),
PROD(DECLARATION, declaration,
  (TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
   TOKEN_TO, 0),
  (TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
   TOKEN_TO, TOKEN_BEGIN, 0)
),
PROD(TYPE_DECLARATION, typeDeclaration,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(OCTETSEQ_TYPE, octetSeqType,
  (TOKEN_OCTETSEQ, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(PRIVATE_POINTER_TYPE, privatePointerType,
  (TOKEN_POINTER, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(INDETERMINATE_TARGET, indeterminateTarget,
  (TOKEN_RECORD, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(INDETERMINATE_FIELD, indeterminateField,
  (TOKEN_PLUS, 0),
  (TOKEN_END, 0)
),
PROD(VAR_DECLARATION, varDeclaration,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(ALIAS_DECLARATION, aliasDeclaration,
  (TOKEN_UNQUALIFIED, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(NAME_SELECTOR, nameSelector,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(STATEMENT_SEQUENCE, statementSeqence,
  (TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_IDENT, TOKEN_RETURN,
   TOKEN_COPY, TOKEN_READ, TOKEN_WRITE, TOKEN_IF, TOKEN_CASE, TOKEN_LOOP,
   TOKEN_WHILE, TOKEN_REPEAT, TOKEN_FOR, TOKEN_TO, TOKEN_EXIT, TOKEN_NOP, 0),
  (TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR, TOKEN_UNTIL, 0)
),
PROD(STATEMENT, statement,
  (TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_IDENT, TOKEN_RETURN,
   TOKEN_COPY, TOKEN_READ, TOKEN_WRITE, TOKEN_IF, TOKEN_CASE, TOKEN_LOOP,
   TOKEN_WHILE, TOKEN_REPEAT, TOKEN_FOR, TOKEN_TO, TOKEN_EXIT, TOKEN_NOP, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(MEM_MGT_OPERATION, memMgtOperation,
  (TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(NEW_STATEMENT, newStatement,
  (TOKEN_NEW, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(RETAIN_STATEMENT, retainStatement,
  (TOKEN_RETAIN, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(RELEASE_STATEMENT, releaseStatement,
  (TOKEN_RELEASE, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(UPDATE_OR_PROC_CALL, updateOrProcCall,
  (TOKEN_IDENT, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(INC_OR_DEC_SUFFIX, incOrDecSuffix,
  (TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(RETURN_STATEMENT, returnStatement,
  (TOKEN_RETURN, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(COPY_STATEMENT, copyStatement,
  (TOKEN_COPY, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(READ_STATEMENT, readStatement,
  (TOKEN_READ, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(WRITE_STATEMENT, writeStatement,
  (TOKEN_WRITE, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(INPUT_ARG, inputArg,
  (TOKEN_NEW, TOKEN_IDENT, 0),
  (TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, 0)
),
PROD(OUTPUT_ARGS, outputArgs,
  (TOKEN_NOT_EQUAL, TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER,
   TOKEN_CHAR_CODE, TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT,
   TOKEN_LPAREN, TOKEN_MINUS, 0),
  (TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, 0)
),
PROD(FORMATTED_ARGS, formattedArgs,
  (TOKEN_NOT_EQUAL, 0),
  (TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, 0)
),
PROD(IF_STATEMENT, ifStatement,
  (TOKEN_IF, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(CASE_STATEMENT, caseStatement,
  (TOKEN_CASE, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(CASE, case_,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_MINUS, 0),
  (TOKEN_BAR, TOKEN_ELSE, TOKEN_END, 0)
),
PROD(CASE_LABELS, caseLabels,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_MINUS, 0),
  (TOKEN_COMMA, TOKEN_SEMICOLON, 0)
),
PROD(LOOP_STATEMENT, loopStatement,
  (TOKEN_LOOP, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(WHILE_STATEMENT, whileStatement,
  (TOKEN_WHILE, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(REPEAT_STATEMENT, repeatStatement,
  (TOKEN_REPEAT, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(FOR_STATEMENT, forStatement,
  (TOKEN_FOR, 0),
  (TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, 0)
),
PROD(FOR_LOOP_VARIANTS, forLoopVariants,
  (TOKEN_IDENT, 0),
  (TOKEN_IN, 0)
),
PROD(ITERABLE_EXPR, iterableExpr,
  (TOKEN_IDENT, 0),
  (TOKEN_DO, 0)
),
PROD(VALUE_RANGE, valueRange,
  (TOKEN_LBRACKET, 0),
  (TOKEN_DO, 0)
),
PROD(DESIGNATOR, designator,
  (TOKEN_IDENT, 0),
  (TOKEN_ASSIGN, TOKEN_IDENT, TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE,
   TOKEN_END, TOKEN_BAR, TOKEN_UNTIL, TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS,
   TOKEN_LPAREN, TOKEN_COLON, 0)
),
PROD(DEREF_TAIL, derefTail,
  (TOKEN_DEREF, 0),
  (TOKEN_ASSIGN, TOKEN_IDENT, TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE,
   TOKEN_END, TOKEN_BAR, TOKEN_UNTIL, TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS,
   TOKEN_LPAREN, TOKEN_COLON, 0)
),
PROD(SUBSCRIPT_TAIL, subscriptTail,
  (TOKEN_LBRACKET, 0),
  (TOKEN_ASSIGN, TOKEN_IDENT, TOKEN_SEMICOLON, TOKEN_ELSIF, TOKEN_ELSE,
   TOKEN_END, TOKEN_BAR, TOKEN_UNTIL, TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS,
   TOKEN_LPAREN, TOKEN_COLON, 0)
),
PROD(TARGET_DESIGNATOR, targetDesignator,
  (TOKEN_IDENT, 0),
  (TOKEN_ASSIGN, 0)
),
PROD(DEREF_TARGET_TAIL, derefTargetTail,
  (TOKEN_DEREF, 0),
  (TOKEN_ASSIGN, 0)
),
PROD(BRACKET_TARGET_TAIL, bracketTargetTail,
  (TOKEN_LBRACKET, 0),
  (TOKEN_ASSIGN, 0)
),
PROD(DEREF, deref,
  (TOKEN_DEREF, 0),
  (TOKEN_DEREF, TOKEN_DOT, TOKEN_LBRACKET, TOKEN_ASSIGN, 0)
),
PROD(EXPRESSION_LIST, expressionList,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_MINUS, 0),
  (TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT, TOKEN_RBRACKET,
   TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(EXPRESSION, expression,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_MINUS, 0),
  (TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT, TOKEN_RBRACKET,
   TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(OPER_L1, operL1,
  (TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS, TOKEN_LESS_OR_EQ, TOKEN_GREATER,
   TOKEN_GREATER_OR_EQ, TOKEN_IDENTITY, TOKEN_IN, 0),
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD, TOKEN_AND,
   TOKEN_MINUS, 0)
),
PROD(SIMPLE_EXPRESSION, simpleExpression,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_MINUS, 0),
  (TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT, TOKEN_RBRACKET,
   TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(OPER_L2, operL2,
  (TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR, TOKEN_CONCAT, TOKEN_SET_DIFF, 0),
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD, TOKEN_AND, 0)
),
PROD(TERM, term,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0),
  (TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR, TOKEN_CONCAT, TOKEN_SET_DIFF,
   TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT, TOKEN_RBRACKET,
   TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(OPER_L3, operL3,
  (TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD, TOKEN_AND, 0),
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0)
),
PROD(SIMPLE_TERM, simpleTerm,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0),
  (TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD, TOKEN_AND,
   TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR, TOKEN_CONCAT, TOKEN_SET_DIFF,
   TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT, TOKEN_RBRACKET,
   TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(FACTOR, factor,
  (TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0),
  (TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT, TOKEN_RBRACKET,
   TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV,
   TOKEN_MOD, TOKEN_AND, TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR, TOKEN_CONCAT,
   TOKEN_SET_DIFF, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END, TOKEN_BAR,
   TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(SIMPLE_FACTOR, simpleFactor,
  (TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0),
  (TOKEN_CONV, TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT,
   TOKEN_RBRACKET, TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_ASTERISK, TOKEN_SOLIDUS,
   TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR,
   TOKEN_CONCAT, TOKEN_SET_DIFF, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(SOURCE_DESIGNATOR, sourceDesignator,
  (TOKEN_IDENT, 0),
  (TOKEN_CONV, TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT,
   TOKEN_RBRACKET, TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_ASTERISK, TOKEN_SOLIDUS,
   TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR,
   TOKEN_CONCAT, TOKEN_SET_DIFF, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(DEREF_SOURCE_TAIL, derefSourceTail,
  (TOKEN_DEREF, 0),
  (TOKEN_CONV, TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT,
   TOKEN_RBRACKET, TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_ASTERISK, TOKEN_SOLIDUS,
   TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR,
   TOKEN_CONCAT, TOKEN_SET_DIFF, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(BRACKET_SOURCE_TAIL, bracketSourceTail,
  (TOKEN_LBRACKET, 0),
  (TOKEN_CONV, TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT,
   TOKEN_RBRACKET, TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_ASTERISK, TOKEN_SOLIDUS,
   TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR,
   TOKEN_CONCAT, TOKEN_SET_DIFF, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(FUNCTION_CALL_TAIL, functionCallTail,
  (TOKEN_LPAREN, 0),
  (TOKEN_CONV, TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT,
   TOKEN_RBRACKET, TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_ASTERISK, TOKEN_SOLIDUS,
   TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR,
   TOKEN_CONCAT, TOKEN_SET_DIFF, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(STRUCTURED_VALUE, structuredValue,
  (TOKEN_LBRACE, 0),
  (TOKEN_CONV, TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON, TOKEN_DOT_DOT,
   TOKEN_RBRACKET, TOKEN_RPAREN, TOKEN_RBRACE, TOKEN_ASTERISK, TOKEN_SOLIDUS,
   TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR,
   TOKEN_CONCAT, TOKEN_SET_DIFF, TOKEN_OF, TOKEN_ELSIF, TOKEN_ELSE, TOKEN_END,
   TOKEN_BAR, TOKEN_UNTIL, TOKEN_THEN, TOKEN_DO, 0)
),
PROD(VALUE_COMPONENT, valueComponent,
  (TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
   TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
   TOKEN_MINUS, 0),
  (TOKEN_COMMA, TOKEN_RBRACE, 0)
),
PROD(TO_DO_LIST, toDoList,
  (TOKEN_TO, 0),
  (TOKEN_SEMICOLON, 0)
),
PROD(TRACKING_REF, trackingRef,
  (TOKEN_LPAREN, 0),
  (TOKEN_QUOTED_STRING, 0)
),
PROD(TASK_TO_DO, taskToDo,
  (TOKEN_QUOTED_STRING, 0),
  (TOKEN_SEMICOLON, TOKEN_END, 0)
)

/* END OF FILE */