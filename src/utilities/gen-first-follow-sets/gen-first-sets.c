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

#import "tokenset.h"

/* FIRST sets */

int main() {

m2c_tokenset_t compilationUnit = m2c_new_tokenset_from_list(
  TOKEN_DEFINITION, TOKEN_IMPLEMENTATION, TOKEN_MODULE
);

m2c_tokenset_t definitionModule = m2c_new_tokenset_from_list(
  TOKEN_DEFINITION
);

m2c_tokenset_t import = m2c_new_tokenset_from_list(
  TOKEN_IMPORT
);

m2c_tokenset_t definition = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_TO
);

m2c_tokenset_t constDefinition = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET, TOKEN_IDENT
);

m2c_tokenset_t constantBinding = m2c_new_tokenset_from_list(
  M2C_TOKEN_LBRACKET
);

m2c_tokenset_t constDeclaration = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t constExpression = m2c_new_tokenset_from_list(
  TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, M2C_TOKEN_IDENT, M2C_TOKEN_NOT,
  TOKEN_LBRACE, TOKEN_LPAREN, TOKEN_ASTERISK, TOKEN_SOLIDUS,
  TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_MINUS
);

m2c_tokenset_t typeDefinition = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t aliasType = m2c_new_tokenset_from_list(
  TOKEN_ALIAS
);

m2c_tokenset_t qualident = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t subrangeType = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET
);

m2c_tokenset_t constRange = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET
);

m2c_tokenset_t enumType = m2c_new_tokenset_from_list(
  TOKEN_LPAREN
);

m2c_tokenset_t identList = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t setType = m2c_new_tokenset_from_list(
  TOKEN_SET
);

m2c_tokenset_t arrayType = m2c_new_tokenset_from_list(
  TOKEN_ARRAY
);

m2c_tokenset_t recordType = m2c_new_tokenset_from_list(
  TOKEN_RECORD
);

m2c_tokenset_t recTypeToExtend = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t fieldList = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t pointerType = m2c_new_tokenset_from_list(
  TOKEN_POINTER
);

m2c_tokenset_t opaqueType = m2c_new_tokenset_from_list(
  TOKEN_OPAQUE
);

m2c_tokenset_t procedureType = m2c_new_tokenset_from_list(
  TOKEN_PROCEDURE
);

m2c_tokenset_t formalType = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_VAR, TOKEN_ARRAY, TOKEN_IDENT, TOKEN_ARGLIST
);

m2c_tokenset_t nonAttrFormalType = m2c_new_tokenset_from_list(
  TOKEN_ARRAY, TOKEN_IDENT, TOKEN_ARGLIST
);

m2c_tokenset_t simpleFormalType = m2c_new_tokenset_from_list(
  TOKEN_ARRAY, TOKEN_IDENT
);

m2c_tokenset_t castingFormalType = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t variadicFormalType = m2c_new_tokenset_from_list(
  TOKEN_ARGLIST
);

m2c_tokenset_t procedureHeader = m2c_new_tokenset_from_list(
  TOKEN_PROCEDURE
);

m2c_tokenset_t bindingSpecifier = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_READ, TOKEN_WRITE,
  TOKEN_IDENT
);

m2c_tokenset_t bindableIdent = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t procedureSignature = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t formalParams = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_VAR, TOKEN_IDENT
);

m2c_tokenset_t programModule = m2c_new_tokenset_from_list(
  TOKEN_MODULE
);

m2c_tokenset_t privateImport = m2c_new_tokenset_from_list(
  TOKEN_IMPORT
);

m2c_tokenset_t block = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
  TOKEN_TO, TOKEN_BEGIN
);

m2c_tokenset_t implementationModule = m2c_new_tokenset_from_list(
  TOKEN_IMPLEMENTATION
);

m2c_tokenset_t possiblyEmptyBlock = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
  TOKEN_TO, TOKEN_BEGIN, TOKEN_END
);

m2c_tokenset_t declaration = m2c_new_tokenset_from_list(
  TOKEN_CONST, TOKEN_TYPE, TOKEN_VAR, TOKEN_PROCEDURE, TOKEN_UNQUALIFIED,
  TOKEN_TO
);

m2c_tokenset_t typeDeclaration = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t octetSeqType = m2c_new_tokenset_from_list(
  TOKEN_OCTETSEQ
);

m2c_tokenset_t privatePointerType = m2c_new_tokenset_from_list(
  TOKEN_POINTER
);

m2c_tokenset_t indeterminateTarget = m2c_new_tokenset_from_list(
  RECORD
);

m2c_tokenset_t indeterminateField = m2c_new_tokenset_from_list(
  TOKEN_PLUS
);

m2c_tokenset_t varDeclaration = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t aliasDeclaration = m2c_new_tokenset_from_list(
  TOKEN_UNQUALIFIED
);

m2c_tokenset_t nameSelector = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t statementSeqence = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_IDENT, TOKEN_RETURN,
  TOKEN_COPY, TOKEN_READ, TOKEN_WRITE, TOKEN_IF, TOKEN_CASE, TOKEN_LOOP,
  TOKEN_WHILE, TOKEN_REPEAT, TOKEN_FOR, TOKEN_TO, TOKEN_EXIT, TOKEN_NOP
);

m2c_tokenset_t statement = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE, TOKEN_IDENT, TOKEN_RETURN,
  TOKEN_COPY, TOKEN_READ, TOKEN_WRITE, TOKEN_IF, TOKEN_CASE, TOKEN_LOOP,
  TOKEN_WHILE, TOKEN_REPEAT, TOKEN_FOR, TOKEN_TO, TOKEN_EXIT, TOKEN_NOP
);

m2c_tokenset_t memMgtOperation = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_RETAIN, TOKEN_RELEASE
);

m2c_tokenset_t newStatement = m2c_new_tokenset_from_list(
  TOKEN_NEW
);

m2c_tokenset_t retainStatement = m2c_new_tokenset_from_list(
  TOKEN_RETAIN
);

m2c_tokenset_t releaseStatement = m2c_new_tokenset_from_list(
  TOKEN_RELEASE
);

m2c_tokenset_t updateOrProcCall = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t incOrDecSuffix = m2c_new_tokenset_from_list(
  TOKEN_PLUS_PLUS, TOKEN_MINUS_MINUS
);

m2c_tokenset_t returnStatement = m2c_new_tokenset_from_list(
  TOKEN_RETURN
);

m2c_tokenset_t copyStatement = m2c_new_tokenset_from_list(
  TOKEN_COPY
);

m2c_tokenset_t readStatement = m2c_new_tokenset_from_list(
  TOKEN_READ
);

m2c_tokenset_t writeStatement = m2c_new_tokenset_from_list(
  TOKEN_WRITE
);

m2c_tokenset_t inputArg = m2c_new_tokenset_from_list(
  TOKEN_NEW, TOKEN_IDENT
);

m2c_tokenset_t outputArgs = m2c_new_tokenset_from_list(
  TOKEN_NOT_EQUAL, TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER,
  TOKEN_CHAR_CODE, TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT,
  TOKEN_LPAREN, TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD,
  TOKEN_AND, TOKEN_MINUS
);

m2c_tokenset_t formattedArgs = m2c_new_tokenset_from_list(
  TOKEN_NOT_EQUAL
);

m2c_tokenset_t ifStatement = m2c_new_tokenset_from_list(
  TOKEN_IF
);

m2c_tokenset_t caseStatement = m2c_new_tokenset_from_list(
  TOKEN_CASE
);

m2c_tokenset_t case_ = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_MINUS
);

m2c_tokenset_t case_labels = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_ASTERISK, TOKEN_SOLIDUS, TOKEN_DIV, TOKEN_MOD, TOKEN_AND, TOKEN_MINUS
);

m2c_tokenset_t loopStatement = m2c_new_tokenset_from_list(
  TOKEN_LOOP
);

m2c_tokenset_t whileStatement = m2c_new_tokenset_from_list(
  TOKEN_WHILE
);

m2c_tokenset_t repeatStatement = m2c_new_tokenset_from_list(
  TOKEN_REPEAT
);

m2c_tokenset_t forStatement = m2c_new_tokenset_from_list(
  TOKEN_FOR
);

m2c_tokenset_t forLoopVariants = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t iterableExpr = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t valueRange = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET
);

m2c_tokenset_t designator = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t derefTail = m2c_new_tokenset_from_list(
  TOKEN_DEREF
);

m2c_tokenset_t subscriptTail = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET
);

m2c_tokenset_t targetDesignator = m2c_new_tokenset_from_list(
  TOKEN_IDENT
);

m2c_tokenset_t derefTargetTail = m2c_new_tokenset_from_list(
  TOKEN_DEREF
);

m2c_tokenset_t bracketTargetTail = m2c_new_tokenset_from_list(
  TOKEN_LBRACKET
);

m2c_tokenset_t deref = m2c_new_tokenset_from_list(
  TOKEN_DEREF
);

m2c_tokenset_t expressionList = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_MINUS
);

m2c_tokenset_t expression = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_MINUS
);

m2c_tokenset_t operL1 = m2c_new_tokenset_from_list(
  TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS, TOKEN_LESS_OR_EQ, TOKEN_GREATER,
  TOKEN_GREATER_OR_EQ, TOKEN_IDENTITY, TOKEN_IN
);

m2c_tokenset_t simpleExpression = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
  TOKEN_MINUS
);

m2c_tokenset_t operL2 = m2c_new_tokenset_from_list(
  TOKEN_PLUS, TOKEN_MINUS, TOKEN_OR, TOKEN_CONCAT, TOKEN_SET_DIFF
);

m2c_tokenset_t term = m2c_new_tokenset_from_list(
  TOKEN_NOT, TOKEN_WHOLE_NUMBER, TOKEN_REAL_NUMBER, TOKEN_CHAR_CODE,
  TOKEN_QUOTED_STRING, TOKEN_LBRACE, TOKEN_IDENT, TOKEN_LPAREN,
);

m2c_tokenset_t operL3 = m2c_new_tokenset_from_list(
  TOKEN_ASTERISK, TOKEN_S
);



} /* end main */

/* END OF FILE */