#!/bin/sh
compilerSrc=( "../src/concat.c" "../src/strings.c" )
compilerTarget="../concat"
codeSrc="./compiler.concat"
codeCTarget="./concat2.c"
codeTarget="./concat2"
codeTarget2="./concat3"
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
  $codeTarget $codeSrc
} && {
  echo "compile compiled code"
  echo "-----------------------------------------"
  mv -f "./out.c" "./out0.c"
  gcc ${cArgs[@]} -Wno-unused "./out0.c" "./extern.c" -o $codeTarget2
} && {
  echo "compile compiler with compiler"
  echo "-----------------------------------------"
  $codeTarget2 "./test.concat"
  diff "./out.c" "./out0.c"
}
