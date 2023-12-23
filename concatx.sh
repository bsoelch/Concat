#!/bin/sh
#!/bin/sh
baseCompiler="./concat"
compilerSrc="./concat.concat/compiler.concat"
compilerCTarget="./build/concat2.c"
compilerTarget="./build/concat2"
codeSrc="./code.concat"
codeCTarget="./code.c"
codeTarget="./code"
libPath="./lib/"
externCFiles=( "./extern.c" )
cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" "-lm" )
concatArgs=( -W -q -l $libPath )

# clear console
clear
echo "-----------------------------------------" && {
  echo "recompile experimental compiler"
  echo "-----------------------------------------"
  $baseCompiler "$compilerSrc" -o "$compilerCTarget" ${concatArgs[@]}
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $compilerCTarget ${externCFiles[@]} -o $compilerTarget
} && {
  echo "compile code with experimental compiler"
  echo "-----------------------------------------"
  $compilerTarget "$codeSrc" -o "$codeCTarget" -W -p "./parser.out" -t "./typeCheck.out" -X
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget "./extern.c" -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
