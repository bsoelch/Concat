#!/bin/sh
#!/bin/sh
baseCompiler="./concat"
compilerSrc="./concat.concat/compiler.concat"
compilerCTarget="./build/concatX.c"
compilerTarget="./concatX"
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
  $compilerTarget "$codeSrc" -o "$codeCTarget" -X -W -p "./parser.out" -t "./typeCheck.out"
} && {
  echo "compile generated C-code"
  echo "-----------------------------------------"
  gcc ${cArgs[@]} -Wno-unused $codeCTarget "./extern.c" -o $codeTarget
} && {
  echo "run compiled code"
  echo "-----------------------------------------"
  $codeTarget
}
