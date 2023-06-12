!! ======================================================================== !!
!! M2C Modula-2 Compiler & Translator                                       !!  
!!                                                                          !!
!! Copyright (c) 2015-2023 Benjamin Kowarsch                                !!
!!                                                                          !!
!! @synopsis                                                                !!
!!                                                                          !!
!! M2C is a portable  Modula-2 to C translator  and via-C compiler  for the !!
!! bootstrap subset of the revised Modula-2 language described in           !!
!!                                                                          !!
!! https://github.com/m2sf/m2bsk/wiki/Language-Specification                !!
!!                                                                          !!
!! In translator mode, M2C translates Modula-2 source files to semantically !!
!! equivalent C source files.  In compiler mode, it translates the Modula-2 !!
!! source files  to C,  then compiles the resulting C sources to object and !!
!! executable files using the host system's resident C compiler and linker. !!
!!                                                                          !!
!! Further information at https://github.com/m2sf/m2c/wiki                  !!
!!                                                                          !!
!! @file                                                                    !!
!!                                                                          !!
!! c99-templates.tpl                                                        !!
!!                                                                          !!
!! Templates for generating C99 output.                                     !!
!!                                                                          !!
!! @license                                                                 !!
!!                                                                          !!
!! M2C is free software: You can redistribute and modify it under the terms !!
!! of the GNU Lesser General Public License (LGPL) either version 2.1 or at !!
!! your choice version 3, both published by the Free Software Foundation.   !!
!!                                                                          !!
!! M2C is distributed in the hope it may be useful,  but  strictly  WITHOUT !!
!! ANY WARRANTY;  without even  the implied warranty of  MERCHANTABILITY or !!
!! FITNESS FOR ANY PARTICULAR PURPOSE.  Read the license for more details.  !!
!!                                                                          !!
!! You should have received a copy of the GNU Lesser General Public License !!
!! along with M2C.  If not, see <https://www.gnu.org/copyleft/lesser.html>. !!
!! ======================================================================== !!

preamble {%
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// %%target_filename%% -- C99 source file
//
// auto-generated by M2C Modula-2 Compiler & Translator
//
// Modula-2 source details
//   filename : %%source_filename%%
//   module   : %%module_ident%%
//   key      : %%module_key%%
//
// For details on M2C, see https://github.com/m2sf/m2c
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
%}

#
#
eof {%
/* END OF FILE */
%}

interface {%
%%preamble%%
%%incl_guard_start%%
%%import%%
%%definition%%
%%incl_guard_end%%
%%eof%%
%}


implementation {%
%%preamble%%
%%import%%
%%definition%%
%%block%%
%%eof%%
%}


program {%
%%preamble%%
%%import%%
%%definition%%
%%block%%
%%eof%%
%}


