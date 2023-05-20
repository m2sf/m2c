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
 * gen-first-sets.c                                                          *
 *                                                                           *
 * Utility program to generate FIRST set literals.                           *
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

#import "tokenset.h"
#import <stdio.h>


/* --------------------------------------------------------------------------
 * macro PRINT(name, tokenset)
 * ----------------------------------------------------------------------- */

#define PRINT(_name, _set) \
  printf("#define %s \\\n", _name); \
  m2c_tokenset_print_literal(_set); \
  printf("\n");
  

/* --------------------------------------------------------------------------
 * FIRST sets
 * ----------------------------------------------------------------------- */

int main() {

/* FIRST(compilationUnit) */

m2c_tokenset_t compilationUnit = m2c_new_tokenset_from_list(
  TOKEN_DEFINITION, TOKEN_IMPLEMENTATION, TOKEN_MODULE, 0
);

PRINT("COMPILATION_UNIT", compilationUnit);


/* FIRST(definitionModule) */

m2c_tokenset_t definitionModule = m2c_new_tokenset_from_list(
  TOKEN_DEFINITION, 0
);

PRINT("DEFINITION_MODULE", definitionModule);


/* FIRST(import) */

m2c_tokenset_t import = m2c_new_tokenset_from_list(
  TOKEN_IMPORT, 0
);

PRINT("IMPORT", import);


/* FIRST(definition) */

m2c_tokenset_t definition = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_TO, 0
);

PRINT("DEFINITION", definition);


/* FIRST(constDefinition) */

m2c_tokenset_t constDefinition = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, TOKEN_IDENT, 0
);

PRINT("CONST_DEFINITION", constDefinition);


/* FIRST(constantBinding) */

m2c_tokenset_t constantBinding = m2c_new_tokenset_from_list(
  M2C_TOKEN_LBRACKET, 0
);

PRINT("CONST_BINDING", constantBinding);


/* FIRST(constDeclaration) */

m2c_tokenset_t constDeclaration = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("CONST_DECLARATION", constDeclaration);


/* FIRST(constExpression) */

m2c_tokenset_t constExpression = m2c_new_tokenset_from_list(
  TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, M2C_TOKEN_IDENT, M2C_TOKEN_NOT,
  TOKEN_LBRACE, TOKEN_LPAREN, TOKEN_ASTERISK, TOKEN_SOLIDUS,
  TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_MINUS, 0
);

PRINT("CONST_EXPRESSION", constExpression);


/* FIRST(typeDefinition) */

m2c_tokenset_t typeDefinition = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("TYPE_DEFINITION", typeDefinition);


/* FIRST(aliasType) */

m2c_tokenset_t aliasType = m2c_new_tokenset_from_list(
  TOKEN_ALIAS, 0
);

PRINT("ALIAS_TYPE", aliasType);


/* FIRST(qualident) */

m2c_tokenset_t qualident = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("QUALIDENT", qualident);


/* FIRST(subrangeType) */

m2c_tokenset_t subrangeType = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, 0
);

PRINT("SUBRANGE_TYPE", subrangeType);


/* FIRST(constRange) */

m2c_tokenset_t constRange = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, 0
);

PRINT("CONST_RANGE", constRange);


/* FIRST(enumType) */

m2c_tokenset_t enumType = m2c_new_tokenset_from_list(
  TOKEN_LPAREN, 0
);

PRINT("ENUM_TYPE", enumType);


/* FIRST(identList) */

m2c_tokenset_t identList = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("IDENT_LIST", identList);


/* FIRST(setType) */

m2c_tokenset_t setType = m2c_new_tokenset_from_list(
  TOKEN_SET, 0
);

PRINT("SET_TYPE", setType);


/* FIRST(arrayType) */

m2c_tokenset_t arrayType = m2c_new_tokenset_from_list(
  TOKEN_ARRAY, 0
);

PRINT("ARRAY_TYPE", arrayType);


/* FIRST(recordType) */

m2c_tokenset_t recordType = m2c_new_tokenset_from_list(
  TOKEN_RECORD, 0
);

PRINT("RECORD_TYPE", recordType);


/* FIRST(recTypeToExtend) */

m2c_tokenset_t recTypeToExtend = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("REC_TYPE_TO_EXTEND", recTypeToExtend);


m2c_tokenset_t fieldList = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);


/* FIRST(pointerType) */

m2c_tokenset_t pointerType = m2c_new_tokenset_from_list(
  TOKEN_POINTER, 0
);

PRINT("POINTER_TYPE", pointerType);


/* FIRST(opaqueType) */

m2c_tokenset_t opaqueType = m2c_new_tokenset_from_list(
  TOKEN_OPAQUE, 0
);

PRINT("OPAQUE_TYPE", opaqueType);


/* FIRST(procedureType) */

m2c_tokenset_t procedureType = m2c_new_tokenset_from_list(
  TOKEN_PROCEDURE, 0
);

PRINT("POCEDURE_TYPE", procedureType);


/* FIRST(formalType) */

m2c_tokenset_t formalType = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_VAR, TOKEN_ARRAY, TOKEN_IDENT, TOKEN_ARGLIST, 0
);

PRINT("FORMAL_TYPE", formalType);


/* FIRST(nonAttrFormalType) */

m2c_tokenset_t nonAttrFormalType = m2c_new_tokenset_from_list(
  TOKEN_ARRAY, TOKEN_IDENT, TOKEN_ARGLIST, 0
);

PRINT("NON_ATTR_FORMAL_TYPE", nonAttrFormalType);


/* FIRST(simpleFormalType) */

m2c_tokenset_t simpleFormalType = m2c_new_tokenset_from_list(
  TOKEN_ARRAY, TOKEN_IDENT, 0
);

PRINT("SIMPLE_FORMAL_TYPE", simpleFormalType);


/* FIRST(castingFormalType) */

m2c_tokenset_t castingFormalType = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("CASTING_FORMAL_TYPE", castingFormalType);


/* FIRST(variadicFormalType) */

m2c_tokenset_t variadicFormalType = m2c_new_tokenset_from_list(
  TOKEN_ARGLIST, 0
);

PRINT("VARIADIC_FORMAL_TYPE", variadicFormalType);


/* FIRST(procedureHeader) */

m2c_tokenset_t procedureHeader = m2c_new_tokenset_from_list(
  TOKEN_PROCEDURE, 0
);

PRINT("PROCEDURE_HEADER", procedureHeader);


/* FIRST(bindingSpecifier) */

m2c_tokenset_t bindingSpecifier = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_READ, TOKEN_WRITE,
  TOKEN_IDENT, 0
);

PRINT("BINDING_SPECIFIER", bindingSpecifier);


/* FIRST(bindableIdent) */

m2c_tokenset_t bindableIdent = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("BINDABLE_IDENT", bindableIdent);


/* FIRST(procedureSignature) */

m2c_tokenset_t procedureSignature = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("PROCEDURE_SIGNATURE", procedureSignature);


/* FIRST(formalParams) */

m2c_tokenset_t formalParams = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_VAR, TOKEN_IDENT, 0
);

PRINT("FORMAL_PARAMS", formalParams);


/* FIRST(programModule) */

m2c_tokenset_t programModule = m2c_new_tokenset_from_list(
  TOKEN_MODULE, 0
);

PRINT("PROGRAM_MODULE", programModule);


/* FIRST(privateImport) */

m2c_tokenset_t privateImport = m2c_new_tokenset_from_list(
  TOKEN_IMPORT, 0
);

PRINT("PRIVATE_IMPORT", privateImport);


/* FIRST(block) */

m2c_tokenset_t block = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
  TOKEN_TO, TOKEN_BEGIN, 0
);

PRINT("BLOCK", block);


/* FIRST(implementationModule) */

m2c_tokenset_t implementationModule = m2c_new_tokenset_from_list(
  TOKEN_IMPLEMENTATION, 0
);

PRINT("IMPLEMENTATION_MODULE", implementationModule);


/* FIRST(possiblyEmptyBlock) */

m2c_tokenset_t possiblyEmptyBlock = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
  TOKEN_TO, TOKEN_BEGIN, TOKEN_END, 0
);

PRINT("POSSIBLY_EMPTY_BLOCK", possiblyEmptyBlock);


/* FIRST(declaration) */

m2c_tokenset_t declaration = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
  TOKEN_TO, 0
);

PRINT("DECLARATION", declaration);


/* FIRST(typeDeclaration) */

m2c_tokenset_t typeDeclaration = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("TYPE_DECLARATION", typeDeclaration);


/* FIRST(octetSeqType) */

m2c_tokenset_t octetSeqType = m2c_new_tokenset_from_list(
  TOKEN_OCTETSEQ, 0
);

PRINT("OCTETSEQ_TYPE", octetSeqType);


/* FIRST(privatePointerType) */

m2c_tokenset_t privatePointerType = m2c_new_tokenset_from_list(
  TOKEN_POINTER, 0
);

PRINT("PRIVATE_POINTER_TYPE", privatePointerType);


/* FIRST(indeterminateTarget) */

m2c_tokenset_t indeterminateTarget = m2c_new_tokenset_from_list(
  RECORD, 0
);

PRINT("INDETERMINATE_TARGET", indeterminateTarget);


/* FIRST(indeterminateField) */

m2c_tokenset_t indeterminateField = m2c_new_tokenset_from_list(
  TOKEN_PLUS, 0
);

PRINT("INDETERMINATE_FIELD", indeterminateField);


/* FIRST(varDeclaration) */

m2c_tokenset_t varDeclaration = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("VAR_DECLARATION", varDeclaration);


/* FIRST(aliasDeclaration) */

m2c_tokenset_t aliasDeclaration = m2c_new_tokenset_from_list(
  TOKEN_UNQUALIFIED, 0
);

PRINT("ALIAS_DECLARATION", aliasDeclaration);


/* FIRST(nameSelector) */

m2c_tokenset_t nameSelector = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("NAME_SELECTOR", nameSelector);


/* FIRST(statementSeqence) */

m2c_tokenset_t statementSeqence = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_IDENT, TOKEN_RETURN,
  TOKEN_COPY, TOKEN_READ, TOKEN_WRITE, TOKEN_IF, TOKEN_CASE, TOKEN_LOOP,
  TOKEN_WHILE, TOKEN_REPEAT, TOKEN_FOR, TOKEN_TO, TOKEN_EXIT, TOKEN_NOP, 0
);

PRINT("STATEMENT_SEQUENCE", statementSeqence);


/* FIRST(statement) */

m2c_tokenset_t statement = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_IDENT, TOKEN_RETURN,
  TOKEN_COPY, TOKEN_READ, TOKEN_WRITE, TOKEN_IF, TOKEN_CASE, TOKEN_LOOP,
  TOKEN_WHILE, TOKEN_REPEAT, TOKEN_FOR, TOKEN_TO, TOKEN_EXIT, TOKEN_NOP, 0
);

PRINT("STATEMENT", statement);


/* FIRST(memMgtOperation) */

m2c_tokenset_t memMgtOperation = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, 0
);

PRINT("MEM_MGT_OPERATION", memMgtOperation);


/* FIRST(newStatement) */

m2c_tokenset_t newStatement = m2c_new_tokenset_from_list(
  TOKEN_NEW, 0
);

PRINT("NEW_STATEMENT", newStatement);


/* FIRST(retainStatement) */

m2c_tokenset_t retainStatement = m2c_new_tokenset_from_list(
  TOKEN_RETAIN, 0
);

PRINT("RETAIN_STATEMENT", retainStatement);


/* FIRST(releaseStatement) */

m2c_tokenset_t releaseStatement = m2c_new_tokenset_from_list(
  TOKEN_RELEASE, 0
);

PRINT("RELEASE_STATEMENT", releaseStatement);


/* FIRST(updateOrProcCall) */

m2c_tokenset_t updateOrProcCall = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("UPDATE_OR_PROC_CALL", updateOrProcCall);


/* FIRST(incOrDecSuffix) */

m2c_tokenset_t incOrDecSuffix = m2c_new_tokenset_from_list(
  TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS, 0
);

PRINT("INC_OR_DEC_SUFFIX", incOrDecSuffix);


/* FIRST(returnStatement) */

m2c_tokenset_t returnStatement = m2c_new_tokenset_from_list(
  TOKEN_RETURN, 0
);

PRINT("RETURN_STATEMENT", returnStatement);


/* FIRST(copyStatement) */

m2c_tokenset_t copyStatement = m2c_new_tokenset_from_list(
  TOKEN_COPY, 0
);

PRINT("COPY_STATEMENT", copyStatement);


/* FIRST(readStatement) */

m2c_tokenset_t readStatement = m2c_new_tokenset_from_list(
  TOKEN_READ, 0
);

PRINT("READ_STATEMENT", readStatement);


/* FIRST(writeStatement) */

m2c_tokenset_t writeStatement = m2c_new_tokenset_from_list(
  TOKEN_WRITE, 0
);

PRINT("READ_STATEMENT", readStatement);


/* FIRST(inputArg) */

m2c_tokenset_t inputArg = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_IDENT, 0
);

PRINT("INPUT_ARG", inputArg);


/* FIRST(outputArgs) */

m2c_tokenset_t outputArgs = m2c_new_tokenset_from_list(
  TOKEN_NOT_EQUAL, TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER,
  TOKEN_CHAR_CODE, TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT,
  TOKEN_LPAREN, TOKEN_MINUS, 0
);

PRINT("OUTPUT_ARGS", outputArgs);


/* FIRST(formattedArgs) */

m2c_tokenset_t formattedArgs = m2c_new_tokenset_from_list(
  TOKEN_NOT_EQUAL, 0
);

PRINT("FORMATTED_ARGS", formattedArgs);


/* FIRST(ifStatement) */

m2c_tokenset_t ifStatement = m2c_new_tokenset_from_list(
  TOKEN_IF, 0
);

PRINT("IF_STATEMENT", ifStatement);


/* FIRST(caseStatement) */

m2c_tokenset_t caseStatement = m2c_new_tokenset_from_list(
  TOKEN_CASE, 0
);

PRINT("CASE_STATEMENT", caseStatement);


/* FIRST(case) */

m2c_tokenset_t case_ = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, TOKEN_MINUS, 0
);

PRINT("CASE", case_);


/* FIRST(caseLabels) */

m2c_tokenset_t caseLabels = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, TOKEN_MINUS, 0
);

PRINT("CASE_LABELS", caseLabels);


/* FIRST(loopStatement) */

m2c_tokenset_t loopStatement = m2c_new_tokenset_from_list(
  TOKEN_LOOP, 0
);

PRINT("LOOP_STATEMENT", loopStatement);


/* FIRST(whileStatement) */

m2c_tokenset_t whileStatement = m2c_new_tokenset_from_list(
  TOKEN_WHILE, 0
);

PRINT("WHILE_STATEMENT", whileStatement);


/* FIRST(repeatStatement) */

m2c_tokenset_t repeatStatement = m2c_new_tokenset_from_list(
  TOKEN_REPEAT, 0
);

PRINT("REPEAT_STATEMENT", repeatStatement);


/* FIRST(forStatement) */

m2c_tokenset_t forStatement = m2c_new_tokenset_from_list(
  TOKEN_FOR, 0
);

PRINT("FOR_STATEMENT", forStatement);


/* FIRST(forLoopVariants) */

m2c_tokenset_t forLoopVariants = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("FOR_LOOP_VARIANTS", forLoopVariants);


/* FIRST(iterableExpr) */

m2c_tokenset_t iterableExpr = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("ITERABLE_EXPR", iterableExpr);


/* FIRST(valueRange) */

m2c_tokenset_t valueRange = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, 0
);

PRINT("VALUE_RANGE", valueRange);


/* FIRST(designator) */

m2c_tokenset_t designator = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("DESIGNATOR", designator);


/* FIRST(derefTail) */

m2c_tokenset_t derefTail = m2c_new_tokenset_from_list(
  TOKEN_DEREF, 0
);

PRINT("DEREF_TAIL", derefTail);


/* FIRST(subscriptTail) */

m2c_tokenset_t subscriptTail = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, 0
);

PRINT("SUBSCRIPT_TAIL", subscriptTail);


/* FIRST(targetDesignator) */

m2c_tokenset_t targetDesignator = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("TARGET_DESIGNATOR", targetDesignator);


/* FIRST(derefTargetTail) */

m2c_tokenset_t derefTargetTail = m2c_new_tokenset_from_list(
  TOKEN_DEREF, 0
);

PRINT("DEREF_TARGET_TAIL", derefTargetTail);


/* FIRST(bracketTargetTail) */

m2c_tokenset_t bracketTargetTail = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, 0
);

PRINT("BRACKET_TARGET_TAIL", bracketTargetTail);


/* FIRST(deref) */

m2c_tokenset_t deref = m2c_new_tokenset_from_list(
  TOKEN_DEREF, 0
);

PRINT("DEREF", deref);


/* FIRST(expressionList) */

m2c_tokenset_t expressionList = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_MINUS, 0
);

PRINT("EXPRESSION_LIST", expressionList);


/* FIRST(expression) */

m2c_tokenset_t expression = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_MINUS, 0
);

PRINT("EXPRESSION", expression);


/* FIRST(operL1) */

m2c_tokenset_t operL1 = m2c_new_tokenset_from_list(
  TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS, TOKEN_LESS_OR_EQ, TOKEN_GREATER,
  TOKEN_GREATER_OR_EQ, TOKEN_IDENTITY, TOKEN_IN, 0
);

PRINT("OPER_L1", operL1);


/* FIRST(simpleExpression) */

m2c_tokenset_t simpleExpression = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_MINUS, 0
);

PRINT("SIMPLE_EXPRESSION", simpleExpression);


/* FIRST(operL2) */

m2c_tokenset_t operL2 = m2c_new_tokenset_from_list(
  TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR, TOKEN_CONCAT, TOKEN_SET_DIFF, 0
);

PRINT("OPER_L2", operL2);


/* FIRST(term) */

m2c_tokenset_t term = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,, 0
);

PRINT("TERM", term);


/* FIRST(operL3) */

m2c_tokenset_t operL3 = m2c_new_tokenset_from_list(
  TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD, TOKEN_AND, 0
);

PRINT("OPER_L3", operL3);


/* FIRST(simpleTerm) */

m2c_tokenset_t simpleTerm = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0
);

PRINT("SIMPLE_TERM", simpleTerm);


/* FIRST(factor) */

m2c_tokenset_t factor = m2c_new_tokenset_from_list(
  TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0
);

PRINT("FACTOR", factor);


/* FIRST(simpleFactor) */

m2c_tokenset_t simpleFactor = m2c_new_tokenset_from_list(
  TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN, 0
);

PRINT("SIMPLE_FACTOR", simpleFactor);


/* FIRST(sourceDesignator) */

m2c_tokenset_t sourceDesignator = m2c_new_tokenset_from_list(
  TOKEN_IDENT, 0
);

PRINT("SOURCE_DESIGNATOR", sourceDesignator);


/* FIRST(derefSourceTail) */

m2c_tokenset_t derefSourceTail = m2c_new_tokenset_from_list(
  TOKEN_DEREF, 0
);

PRINT("DEREF_SOURCE_TAIL", sourceDesignator);


/* FIRST(bracketSourceTail) */

m2c_tokenset_t bracketSourceTail = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, 0
);

PRINT("BRACKET_SOURCE_TAIL", bracketSourceTail);


/* FIRST(functionCallTail) */

m2c_tokenset_t functionCallTail = m2c_new_tokenset_from_list(
  TOKEN_LPAREN, 0
);

PRINT("FUNCTION_CALL_TAIL", functionCallTail);


/* FIRST(structuredValue) */

m2c_tokenset_t structuredValue = m2c_new_tokenset_from_list(
  TOKEN_LBRACE, 0
);

PRINT("STRUCTURED_VALUE", structuredValue);


/* FIRST(valueComponent) */

m2c_tokenset_t valueComponent = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_MINUS, 0
);

PRINT("VALUE_COMPONENT", valueComponent);


/* FIRST(toDoList) */

m2c_tokenset_t toDoList = m2c_new_tokenset_from_list(
  TOKEN_TO, 0
);

PRINT("TO_DO_LIST", toDoList);


/* FIRST(trackingRef) */

m2c_tokenset_t trackingRef = m2c_new_tokenset_from_list(
  TOKEN_LPAREN, 0
);

PRINT("TRACKING_REF", trackingRef);


/* FIRST(taskToDo) */

m2c_tokenset_t taskToDo = m2c_new_tokenset_from_list(
  TOKEN_QUOTED_STRING, 0
);

PRINT("TASK_TO_DO", taskToDo);

} /* end main */

/* END OF FILE */