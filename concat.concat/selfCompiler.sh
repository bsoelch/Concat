#!/bin/sh
compilerSrc=( "../src/concat.c" "../src/strings.c" )
compilerTarget="../concat"
codeSrc="./compiler.concat"
codeCTarget="./concat2.c"
codeTarget="./concat2"
codeCTarget2="./concat3.c"
codeTarget2="./concat3"
codeCTarget3="./concat3_2.c"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

# clear console
clear
echo "recompile compiler"
echo "-----------------------------------------"
gcc ${cArgs[@]} ${compilerSrc[@]} -o $compilerTarget && {
  echo "compile program"
  echo "-----------------------------------------"
  $compilerTarget "$codeSrc" -o "$codeCTarget" -W -q -l "../lib/"
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget "./extern.c" -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget $codeSrc -l "../lib/" -o $codeCTarget2
} && {
  echo "compile compiled code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget2 "./extern.c" -o $codeTarget2
} && {
  echo "compile compiler with compiler"
  echo "-----------------------------------------"
  $codeTarget2 $codeSrc -l "../lib/" -o $codeCTarget3
  diff $codeCTarget2 $codeCTarget3
}
