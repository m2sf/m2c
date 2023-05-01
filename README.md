## M2C — A C-hosted Modula-2 to C Translator ##
Welcome to the M2C Modula-2 to C Translator and Compiler Project


### Objective ###

The objective of this project is to develop a highly portable, reliable, readable
and maintainable Modula-2 to C translator and via-C compiler that generates
authentic and readable C source code.


### Language ###

M2C supports the bootstrap kernel (BSK) subset of Modula-2 R10 (M2R10).

An online version of the language specification is here:

https://github.com/m2sf/m2bsk/wiki/Language-Specification

The authoritative language specification (PDF) is available for download:

https://github.com/m2sf/PDFs/blob/master/M2BSK%20Language%20Description.pdf

**M2C does not support the earlier PIM or ISO dialects.**

For a PIM Modula-2 translator see the [M2T project](https://github.com/trijezdci/m2t).


### Grammar ###

The grammar of M2C's command line interface is in the project repository

https://github.com/m2sf/m2c/blob/main/grammar/cli-grammar.gll

The grammar of Modula-2 R10 is in the project repository

https://github.com/m2sf/m2c/blob/main/grammar/m2c-grammar.gll

For a graphical representation of the grammar, see section
[Syntax Diagrams](https://github.com/m2sf/m2bsk/wiki/Language-Specification-(D)-:-Syntax-Diagrams).


### Targets ###

M2C will generate C99 sources compileable with any C compiler that supports the C99 standard.


### License ###

M2C is licensed under the GNU Lesser General Public License (LGPL) both v.2.1 and v.3.


### Prerequisites ###

M2C is written in a subset of the C99 standard of C.

**There are no dependencies on any third party libraries.**


### OS support ###

M2C will compile and run on any target system supported by the host-compiler.

However, M2C's file system acces and pathname parsing libraries are system dependent.
Library versions for AmigaOS, DOS/Windows, POSIX/Unix and OpenVMS are provided. For
any other systems, these libraries may have to be adapted.


### Development Languages ###

* M2C itself is written in a subset of C99.
* The syntax diagram generator script is written in TCL/TK (not required to build M2C)
* Build configuration scripts are written in the prevalent shell language of the hosting platform


### Collaboration ###

If you would like to contribute to the project, please get in touch by email

trijezdci (gmail)

+++
