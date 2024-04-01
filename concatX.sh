#!/bin/sh
#!/bin/sh
baseCompiler="./concatX"
codeSrc="./code.concat"
codeQBETarget="./code.ssa"
codeAsmTarget="./code.s"
codeTarget="./code"
libPath="./lib/"
cArgs=( "-g" "-std=c17" "-lm" )
concatArgs=( -X -W -q -l $libPath )

if [[ "$@" == *"-R"* ]]; then
  "./selfCompilerX.sh"
else  
  # clear console
  clear
  echo "-----------------------------------------" 
fi && {
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
