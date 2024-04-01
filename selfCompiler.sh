#!/bin/sh
baseCompiler="./concat"
tmpCompiler="./concatXX"
compilerSrc="./concat.concat/compiler.concat"
codeCTarget="./build/concat2.c"
codeTarget="./build/concat2"
codeCTarget2="./build/concat3.c"
codeTarget2="./build/concat3"
codeCTarget3="./build/concat3_2.c"
libPath="./lib/"
externCFiles=( "./extern.c" )
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" "-lm" )
concatArgs=( -W -q -l $libPath )
bootstrapC="./bootstrap/latest.c"

# clear console
clear
echo "-----------------------------------------" && {
  echo "recompile compiler"
  echo "-----------------------------------------"
  $baseCompiler "$compilerSrc" -o "$codeCTarget" ${concatArgs[@]}
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget ${externCFiles[@]} -o $codeTarget
} && {
  if [[ "$@" == *"-X"* ]]; then
    mv $codeTarget $tmpCompiler
    exit 0
  fi
} && {
  echo "compile compiler with compiler"
  echo "-----------------------------------------"
  $codeTarget "$compilerSrc" -o "$codeCTarget2" ${concatArgs[@]}
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget2 ${externCFiles[@]} -o $codeTarget2
} && {
  echo "check if compiler output is stable under recompilation"
  echo "-----------------------------------------"
  $codeTarget2 "$compilerSrc" -o "$codeCTarget3" ${concatArgs[@]}
} && {
  diff $codeCTarget2 $codeCTarget3 && {
    mv $codeTarget2 $baseCompiler
    mv $codeCTarget2 $bootstrapC
  }
}
