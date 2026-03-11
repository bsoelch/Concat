# Concat
Concat is a self-hosted concatenative Programing Language

!!! The language is still in early development, there are frequent breaking changes
use at your own risk !!!

<!--TODO short description of language -->

A (partial) description of the syntax can be found in the `doc` folder <!-- TODO link to syntax -->

## Examples

Hello World:
```
import print
using std.literals.

proc( -> ) =>: main
  "Hello World!":s io.println
end
```
Print prime factors:

```
#+
Prints all prime factors of a given number
+#
import stringBuilder
import print
import numbers/intToStr
## define overloaded <+ operator to simplify string building
overload: <+ sb.appendC sb.appendS sb.appendI64 end

proc( stringBuilder i64 -> stringBuilder ) =>: appendFactors
  $ 0 < if -_ end =:: mut: n ## ensure number is non-negative
  2:i64 while $ $ * n <= do =:: p
    while n p u% 0 == do
      ' ' <+ p <+
      n p / n=
    end
  finally p 1 + end ..drop
  n 1 > if ' ' <+ n <+ end
end
proc( i64 : n -> ) =>: printFactors
  256 sb.create
    "the factors of " <+ n <+ " are:" <+
    n appendFactors '\n' <+
    $ sb.asStr io.print
  sb.free ..drop
end

proc( -> ) =>: main
  2 printFactors
  42 printFactors
  64 printFactors
  1001 printFactors
  2381 printFactors
end
```

more examples can be found in the `examples` folder

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
