#!/bin/sh
codeSrc="./code.concat"
codeQBETarget="./code.ssa"
codeAsmTarget="./code.s"
codeTarget="./code"
libPath="./lib/"
cArgs=( "-g" "-std=c17" "-lm" )
concatArgs=( -W -q -l $libPath )

if [[ "$@" == *"-R"* ]]; then
  "./selfCompiler.sh" -X || exit 1
  baseCompiler="./concatX"
  concatArgs=( -X -l $libPath )
elif [[ "$@" == *"-X"* ]]; then
  baseCompiler="./concatX"
  concatArgs=( -X -l $libPath )
else
  baseCompiler="./concat"
  concatArgs=( -W -l $libPath )
fi
if [[ "$@" == *"--no-core"* ]]; then
  concatArgs+=( --no-core )
fi

{
  echo "compile code.concat"
  echo "-----------------------------------------"
  $baseCompiler "$codeSrc" -o "$codeQBETarget" -p "./parser.out" -t "./typeCheck.out" ${concatArgs[@]}
} && {
  echo "compile generated QBE-code"
  echo "-----------------------------------------"
  qbe $codeQBETarget -o $codeAsmTarget
} && {
  echo "compile generated Assembly-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} $codeAsmTarget "extern.c" -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
