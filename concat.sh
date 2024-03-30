#!/bin/sh
baseCompiler="./concat"
codeSrc="./code.concat"
codeCTarget="./code.c"
codeTarget="./code"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" "-lm" )

# clear console
clear
{
"./selfCompiler.sh"
} && {
  echo "compile program"
  echo "-----------------------------------------"
  $baseCompiler "$codeSrc" -o "$codeCTarget" -W -p "./parser.out" -t "./typeCheck.out"
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget "./extern.c" -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
