#!/bin/sh
compilerSrc=( "../src/concat.c" "../src/strings.c" )
compilerTarget="../concat"
codeSrc="./compiler.concat"
codeCTarget="./concat2.c"
codeTarget="./concat2"
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
  $codeTarget
}
