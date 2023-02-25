cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

cd "./tests/"
for f in *.concat; do ## go through all concat files in the test directory
  echo "------------------------"
  echo "$f :"
  touch "${f%.*}.in" # create input file if it does not exist
  ../concat "$f" -o "${f%.*}.c" -q > "${f%.*}.out" 2> "${f%.*}.err" &&
  gcc ${cArgs[@]} -Wno-unused "${f%.*}.c" -o "${f%.*}" >> "${f%.*}.out" 2>> "${f%.*}.err" &&
  cat "${f%.*}.in" | "./${f%.*}" >> "${f%.*}.out" 2>> "${f%.*}.err"
  ## TODO compare with expected output, print passed/failed
done
