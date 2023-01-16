#!/bin/sh
compilerSrc="./concat.c"
compilerTarget="./concat"
codeSrc="./code.txt"
codeCTarget="./code.c"
codeTarget="./code"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

# clear console
clear
echo "recompile compiler"
echo "-----------------------------------------"
gcc ${cArgs[@]} $compilerSrc -o $compilerTarget && {
  echo "compile program"
  echo "-----------------------------------------"
  $compilerTarget $codeSrc $codeCTarget
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
