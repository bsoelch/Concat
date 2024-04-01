cArgs=( "-g" "-lm" )

nTested=0
nPassed=0
nFailed=0

if [[ "$@" == *"-X"* ]]; then ## if X in arguments
    concatPath="../../concatXX"
else
    concatPath="../../concatX"
fi

clear
echo "running tests in experimental mode"
## TODO? recompile compiler
cd "./tests/"
for d in */; do ## go through all directories in tests folder 
cd "$d"
echo "------------------------"
echo "${d%/}:"
for f in *.concat; do ## go through all concat files in the test directory
  echo "  $f"
  ## create input & expected output files if they do not exist
  
  touch ".${f%.*}.out"
  touch ".${f%.*}.err"
  
  args=( )
  if [ -f "${f%.*}.args" ] 
  then
    mapfile -t args < "${f%.*}.args"
  fi
  
  $concatPath -X "$f" -o "${f%.*}.ssa" -l "../../lib/" -q > ".${f%.*}.out" 2> ".${f%.*}.err" &&
  qbe "${f%.*}.ssa" -o "${f%.*}.s" >> ".${f%.*}.out" 2>> ".${f%.*}.err" &&
  cc ${cArgs[@]} "${f%.*}.s" "../../extern.c" -o "${f%.*}" >> ".${f%.*}.out" 2>> ".${f%.*}.err" &&
  ( [ -f "${f%.*}.in" ] && cat "${f%.*}.in" || echo "" ) | # use ${f%.*}.in as input if it exists otherwise use empty stdin
    "./${f%.*}" "${args[@]}" >> ".${f%.*}.out" 2>> ".${f%.*}.err"
  nTested=$((nTested+1))
  ##compare output with expected output, print passed/failed remove output files
  if ( [ -f "${f%.*}.out" ] || [ -s ".${f%.*}.out" ] ) && ! ( cmp -s "${f%.*}.out" ".${f%.*}.out" )
  then
    echo "    FAILED (out)"
    if ( [ -f "${f%.*}.out" ] && [ -s ".${f%.*}.out" ] )
    then 
      echo "------------------------"
      diff "${f%.*}.out" ".${f%.*}.out"
      echo "------------------------"
    fi
    nFailed=$((nFailed+1))
    continue
  fi
  if ( [ -f "${f%.*}.err" ] || [ -s ".${f%.*}.err" ] ) && ! ( cmp -s "${f%.*}.err" ".${f%.*}.err" )
  then
    echo "    FAILED (err)"
    if ( [ -f "${f%.*}.err" ] && [ -s ".${f%.*}.err" ] )
    then 
      echo "------------------------"
      diff "${f%.*}.err" ".${f%.*}.err"
      echo "------------------------"
    fi
    nFailed=$((nFailed+1))
    continue
  fi
  echo "    PASSED"
  nPassed=$((nPassed+1))
  ## remove temporary files, if they exist
  [ -f ".${f%.*}.out" ] && rm ".${f%.*}.out"
  [ -f ".${f%.*}.err" ] && rm ".${f%.*}.err"
  [ -f "${f%.*}.ssa" ] && rm "${f%.*}.ssa"
  [ -f "${f%.*}.s" ] && rm "${f%.*}.s"
  [ -f "${f%.*}" ] && rm "${f%.*}"
done
cd ..
done

echo "Tested $nTested files: $nPassed passed, $nFailed failed"
  
