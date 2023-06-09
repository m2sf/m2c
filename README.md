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

For a PIM Modula-2 translator see the [MOTTO project](https://github.com/trijezdci/motto).

For an ISO Modula-2 compiler that generates C sources, see [XDS-C](https://github.com/excelsior-oss/xds#xds-c).


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

Compiling M2C requires a C compiler that supports the C99 standard of C.

**There are no dependencies on any third party libraries.**


### OS support ###

M2C will compile and run on any target system supported by the host-compiler.

However, the [file system access](https://github.com/m2sf/m2c/tree/main/src/lib/filesys)
and [pathname parsing](https://github.com/m2sf/m2c/tree/main/src/lib/pathnames) libraries
are platform dependent. Implementations for AmigaOS, DOS/Windows, POSIX/Unix
and VMS are provided.

Adaptation of these libraries may be needed for other platforms.


### Development Languages ###

* M2C itself is written in a subset of C99.
* The syntax diagram generator script is written in TCL/TK (not required to build M2C)
* Build configuration scripts are written in the prevalent shell language of the hosting platform


### Contact ###

If you have questions or would like to contribute to the project, get in touch via

* [Modula2 Telegram group](https://t.me/+hTKSWC2mWoM1OGVl) chat

* [email](mailto:REMOVE+REVERSE.com.gmail@trijezdci) to the project maintainer

+++
