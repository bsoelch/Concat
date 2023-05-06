#!/bin/sh
baseCompiler="../concat"
compilerSrc="./compiler.concat"
codeCTarget="./concat2.c"
codeTarget="./concat2"
codeCTarget2="./concat3.c"
codeTarget2="./concat3"
codeCTarget3="./concat3_2.c"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

# clear console
clear
echo "-----------------------------------------" && {
  echo "recompile compiler"
  echo "-----------------------------------------"
  $baseCompiler "$compilerSrc" -o "$codeCTarget" -W -q -l "../lib/"
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget "./extern.c" -o $codeTarget
} && {
  echo "compile compiler with compiler"
  echo "-----------------------------------------"
  $codeTarget $compilerSrc -l "../lib/" -o $codeCTarget2
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget2 "./extern.c" -o $codeTarget2
} && {
  echo "check if compiler output is stable under recompilation"
  echo "-----------------------------------------"
  $codeTarget2 $compilerSrc -l "../lib/" -o $codeCTarget3
} && {
  diff $codeCTarget2 $codeCTarget3 && mv "./concat3" $baseCompiler
}
