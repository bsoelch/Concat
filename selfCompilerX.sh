#!/bin/sh
baseCompiler="./concatX"
tmpCompiler="./concatXX"
compilerSrc="./concat.concat/compiler.concat"
codeQBETarget="./build/concat2.ssa"
codeAsmTarget="./build/concat2.s"
codeTarget="./build/concat2"
codeQBETarget2="./build/concat3.ssa"
codeAsmTarget2="./build/concat3.s"
codeTarget2="./build/concat3"
codeQBETarget3="./build/concat4.ssa"
libPath="./lib/"
externCFiles=( "./extern.c" )
cArgs=( "-g" "-std=c17" "-lm" )
concatArgs=( -X -W -q -l $libPath )
bootstrapQBE="./bootstrap/latest.ssa"

# clear console
ulimit -S -s 16384 ## increase stack size
clear
echo "-----------------------------------------" && {
  echo "recompile compiler"
  echo "-----------------------------------------"
  $baseCompiler "$compilerSrc" -o "$codeQBETarget" ${concatArgs[@]}
} && {
  echo "compile generated QBE-code"
  echo "-----------------------------------------"
  qbe "$codeQBETarget" -o "$codeAsmTarget"
} && {
  echo "compile generated Assembly-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} "$codeAsmTarget" ${externCFiles[@]} -o "$codeTarget"
} && {
  if [[ "$@" == *"-X"* ]]; then
    mv $codeTarget $tmpCompiler
    exit 0
  fi
} && {
  echo "compiler compiler with itself"
  echo "-----------------------------------------"
  $codeTarget "$compilerSrc" -o "$codeQBETarget2" ${concatArgs[@]}
} && {
  echo "compile generated QBE-code"
  echo "-----------------------------------------"
  qbe "$codeQBETarget2" -o "$codeAsmTarget2"
} && {
  echo "compile generated Assembly-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} "$codeAsmTarget2" ${externCFiles[@]} -o "$codeTarget2"
} && {
  echo "check if compiler output is stable under recompilation"
  echo "-----------------------------------------"
  $codeTarget2 "$compilerSrc" -o "$codeQBETarget3" ${concatArgs[@]}
} && {
  diff $codeQBETarget2 $codeQBETarget3 && {
    mv $codeTarget2 $baseCompiler
    mv $codeQBETarget3 $bootstrapQBE
  }
}
