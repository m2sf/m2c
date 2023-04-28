## M2C — A C-hosted Modula-2 to C Translator Project ##
Welcome to the M2C Modula-2 to C Translator Project

### Objective ###

The objective of this project is to develop a via-C bootstrap compiler for the revised Modula-2 language described in

[*Modula-2 Revision 2010, Language Report for the Bootstrap Kernel Subset (BSK)*, by Kowarsch and Sutcliffe, July 2020](https://github.com/m2sf/PDFs/blob/master/M2BSK%20Language%20Description.pdf).

### Grammar ###

The grammar of the translator's command line interface is in the project repository

https://github.com/m2sf/m2bsk/blob/master/grammar/m2cli-grammar.gll

The grammar of the compiler's input language is in the project repository

https://github.com/m2sf/m2c/blob/main/grammar/m2c-grammar.gll

For a graphical representation of the grammar, see section
[Syntax Diagrams](https://github.com/m2sf/m2bsk/wiki/Language-Specification-(D)-:-Syntax-Diagrams).

### Language Specification ###

An online version of the language specification is here:

https://github.com/m2sf/m2bsk/wiki/Language-Specification


The authoritative language specification (PDF) is available for download:

https://github.com/m2sf/PDFs/blob/master/M2BSK%20Language%20Description.pdf


### Scope ###

The translator supports the Bootstrap Kernel (BSK) _**subset**_ of Modula-2 R10. It does not support earlier PIM or ISO dialects.

For a list of facilities that have been omitted in the subset, see [Omissions](https://github.com/m2sf/m2bsk/wiki/Omissions)

### Targets ###

The translator will generate C99 sources that can be compiled with any C compiler that supports the C99 standard.

### License ###

M2C is licensed under the GNU Lesser General Public License (LGPL) both v.2.1 and v.3.

### Prerequisites ###

M2C is written in a subset of the C99 standard of C.

**There are no dependencies on any third party libraries.**

### OS support ###

M2C will run on any operating system with target support by the host-compiler.

### Development Languages ###

* M2C itself is written in a subset of C99.
* The syntax diagram generator script is written in TCL/TK (not required to build M2C)
* Build configuration scripts are written in the prevalent shell language of the hosting platform

### Collaboration ###

If you would like to contribute to the project, please get in touch by email

trijezdci (gmail)

+++
