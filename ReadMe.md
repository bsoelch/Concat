# Concat
Concat is a self-hosted concatenative Programing Language

!!! The language is still in early development, there are frequent breaking changes
use at your own risk !!!

<!--TODO short description of language -->

A (partial) description of the syntax can be found in the `doc` folder <!-- TODO link to syntax -->

Hello World Program:
```
'print' *import

proc( -> ) =>: main
  "Hello World!":s io.println
end
```
A list of examples can be found in the `examples` folder

## Usage
Currently Concat used [QBE](https://c9x.me/compile/) as its back-end.
To use Concant you will need QBE and GCC (as assembler) for your target architecture.

Bootstrapping (using QBE and GCC):
*Due to a bug in the QBE register-allocator bootstraping currently does not work on ARM targets*
```sh
qbe ./bootstrap/latest.ssa -o concat.s # compile QBE to Assembly
gcc concat.s lib/extern.c -lm -o concat # create binary from Assembly code
./selfCompiler.sh # recompile the compiler from source files
```

Compiling a File:
```sh
./concat <srcFile> -o <target>
```
