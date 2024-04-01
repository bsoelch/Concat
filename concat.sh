#!/bin/sh
baseCompiler="./concat"
codeSrc="./code.concat"
codeCTarget="./code.c"
codeTarget="./code"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" "-lm" )

if [[ "$@" == *"-R"* ]]; then
  "./selfCompiler.sh" -X || exit 1
  baseCompiler="./concatXX"
else
  baseCompiler="./concat"
fi
{
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
