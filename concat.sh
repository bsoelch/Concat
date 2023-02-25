#!/bin/sh
compilerSrc=( "./src/concat.c" "./src/strings.c" )
compilerTarget="./concat"
codeSrc="./code.concat"
codeCTarget="./code.c"
codeTarget="./code"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

# clear console
clear
echo "recompile compiler"
echo "-----------------------------------------"
gcc ${cArgs[@]} ${compilerSrc[@]} -o $compilerTarget && {
  echo "compile program"
  echo "-----------------------------------------"
  $compilerTarget "$codeSrc" -o "$codeCTarget" -W -p "./parser.out" -t "./typeCheck.out"
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget "./extern.c" -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
