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
 * Implementation of identifier to snake-case translation dictionary.        *
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
  


  return lookahead;
} /* end imp_or_pgm_mod_hdr_and_import */


/* --------------------------------------------------------------------------
 * private function private_import()
 * --------------------------------------------------------------------------
 * privateImport :=
 *   IMPORT moduleList
 *   ;
 *
 * alias moduleList = identList;
 * ----------------------------------------------------------------------- */

static m2c_token_t private_import () {
  m2c_token_t lookahead;
  


  return lookahead;
} /* end private_import */


/* --------------------------------------------------------------------------
 * private function ident_list()
 * --------------------------------------------------------------------------
 * identList :=
 *   Ident ( ',' Ident )*
 *   ;
 * ----------------------------------------------------------------------- */

static m2c_token_t ident_list () {
  m2c_token_t lookahead;
  


  return lookahead;
} /* end ident_list */


/* END OF FILE */