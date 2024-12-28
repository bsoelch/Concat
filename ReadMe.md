# Concat
Concat is a self-hosted concatenative Programing Language

!!! The language is still in active development, there are frequent breaking changes
use at your own risk !!!

<!--TODO short description of language -->

A (partial) description of the syntax can be found in the `doc` folder <!-- TODO link to syntax -->

## Examples

Hello World:
```
import io

proc( -> ) =>: main
  "Hello World!" io.puts ..drop
end
```
Print prime factors:

```
import stringBuilder
import stringIO
import numbers/printInt
## define overloaded <+ operator to simplify string building
overload: <+ sb.appendC sb.appendS sb.appendI64 end

proc( stringBuilder i64 -> stringBuilder ) =>: appendFactors
  $ 0 < if -_ end =:: mut: n ## ensure number is non-negative
  2:i64 while $ $ * n <= do =:: p
    while n p ..umod 0 == do
      ' ' <+ p <+
      n p / n =
    end
  finally p 1 + end ..drop
  n 1 > if ' ' <+ n <+ end
end
proc( i64 : n -> ) =>: printFactors
  256 sb.create
    "the factors of " <+ n <+ " are:" <+
    n appendFactors '\n' <+
    io.stdOut $1 io.fputSb ..drop
  sb.free ..drop
end

proc( -> ) =>: main
  2 printFactors
  42 printFactors
  64 printFactors
  1001 printFactors
  2381 printFactors
end
#+ prints:
the factors of 2 are: 2
the factors of 42 are: 2 3 7
the factors of 64 are: 2 2 2 2 2 2
the factors of 1001 are: 7 11 13
the factors of 2381 are: 2381
+#
```

## Usage
Currently Concat used QBE <!--TODO link to QBE --> as its only back-end.
To use Concant you will need QBE and some Assembler for your target architecture.

Bootstrapping (using QBE and GCC):
```sh
qbe ./bootstrap/latest.ssa -o concat.s # compile QBE to Assembly
gcc concat.s extern.c -lm -o concat # create binary from Assembly code
./selfCompiler.sh # recompile the compiler from source files
```

Compiling a File:
```sh
./concat <srcFile> -o <target.ssa> ## concat to QBE
qbe <target.ssa> <target.s> ## QBE to Assembly
gcc -g <target.s> extern.c -lm -o <target> ## Assembly to binary, keep debug symbols
```
