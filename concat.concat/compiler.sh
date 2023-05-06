#!/bin/sh
baseCompiler="../concat"
compilerTarget="./concat2"
compilerSrc="./compiler.concat"
compilerCTarget="./concat2.c"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

# clear console
clear
{
  echo "recompile compiler"
  echo "-----------------------------------------"
  $baseCompiler "$compilerSrc" -o "$compilerCTarget" -W -q -l "../lib/"
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $compilerCTarget "./extern.c" -o $compilerTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $compilerTarget "./test.concat" "-l" "../lib/"
}
