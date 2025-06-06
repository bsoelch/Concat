#!/bin/sh
baseCompiler="./concat"
tmpCompiler="./concatX"
compilerSrc="./concat.concat/compiler.concat"
codeTarget="./build/concat"
codeTarget2="./build/concat2"
codeQBETarget2="./build/concat2.ssa"
codeQBETarget3="./build/concat3.ssa"
libPath="./lib/"
externCFiles=( "./extern.c" )
cArgs=( "-g" "-std=c17" "-lm" )
concatArgs=( -W -l $libPath )
prevBootstrapQBE="./bootstrap/prev.ssa"
bootstrapQBE="./bootstrap/latest.ssa"

# clear console
ulimit -S -s 16384 ## increase stack size
clear
echo "-----------------------------------------" && {
  echo "recompile compiler"
  echo "-----------------------------------------"
  $baseCompiler "$compilerSrc" -o "$codeTarget" ${concatArgs[@]}
} && {
  if [[ "$@" == *"-X"* ]]; then
    mv $codeTarget $tmpCompiler
    exit 0
  fi
} && {
  echo "compiler compiler with itself"
  echo "-----------------------------------------"
  $codeTarget "$compilerSrc" -o "$codeTarget2" ${concatArgs[@]}
} && {
  echo "check if compiler output is stable under recompilation"
  echo "-----------------------------------------"
  $codeTarget2 "$compilerSrc" -S -o "$codeQBETarget3" ${concatArgs[@]} --no-inc-version
} && {
  diff $codeQBETarget2 $codeQBETarget3 && {
    mv $codeTarget2 $baseCompiler
    mv $bootstrapQBE $prevBootstrapQBE
    mv $codeQBETarget3 $bootstrapQBE
  }
}
