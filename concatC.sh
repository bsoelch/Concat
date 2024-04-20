#!/bin/sh
baseCompiler="./concatC"
codeSrc="./code.concat"
codeCTarget="./code.c"
codeTarget="./code"
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" "-lm" )

if [[ "$@" == *"-R"* ]]; then
  "./selfCompilerC.sh" -X || exit 1
  baseCompiler="./concatX"
  concatArgs=( -C -X -l $libPath )
elif [[ "$@" == *"-R"* ]]; then
  baseCompiler="./concatX"
  concatArgs=( -C -X -l $libPath )
else
  baseCompiler="./concatC"
  concatArgs=( -C -W -l $libPath )
fi
{
  echo "compile program"
  echo "-----------------------------------------"
  $baseCompiler "$codeSrc" -o "$codeCTarget" -W -p "./parser.out" -t "./typeCheck.out" ${concatArgs[@]}
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget "./extern.c" -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
