#!/bin/sh
baseCompiler="./concat"
compilerSrc="./concat.concat/compiler.concat"
codeQBETarget="./build/concat2.ssa"
codeAsmTarget="./build/concat2.s"
codeTarget="./concatXX"
## XXX complete cycle once compiler is stable
libPath="./lib/"
externCFiles=( "./extern.c" )
cArgs=( "-g" "-std=c17" "-lm" )
concatArgs=( -W -q -l $libPath )

# clear console
clear
echo "-----------------------------------------" && {
  echo "recompile compiler"
  echo "-----------------------------------------"
  $baseCompiler "$compilerSrc" -X -o "$codeQBETarget" ${concatArgs[@]}
} && {
  echo "compile generated QBE-code"
  echo "-----------------------------------------"
  qbe "$codeQBETarget" -o "$codeAsmTarget"
} && {
  echo "compile generated Assembly-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} "$codeAsmTarget" ${externCFiles[@]} -o "$codeTarget"
} && { ## XXX close loop once compiler is stable
  echo "compile test-file with compiler"
  echo "-----------------------------------------"
  $codeTarget "./code.concat" -X -W -o "./code.ssa" -p "./parser.out" -t "./typeCheck.out"
}
