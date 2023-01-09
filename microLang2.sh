#!/bin/sh
compilerSrc="./microLang2.c"
compilerTarget="./microLang2"
codeSrc="./code.txt"
codeCTarget="./out.c"
codeTarget="./a.out"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

echo "${cArgs[@]}"

# clear console
clear
echo "recompile compiler"
echo "-----------------------------------------"
gcc ${cArgs[@]} $compilerSrc -o $compilerTarget && {
  echo "compile program"
  echo "-----------------------------------------"
  ./microLang2 $codeSrc $codeCTarget
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
