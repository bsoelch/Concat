cArgs=( "-g" "-Wall" "-Wextra" "-Wshadow" "-Wold-style-definition" "-Wcast-qual" "-Werror" "-pedantic" )

cd "./tests/"
for d in */; do ##go through all directories in tests folder 
cd "$d"
for f in *.concat; do ## go through all concat files in the test directory
  echo "------------------------"
  echo "$f :"
  # create input & expected output files if they do not exist
  touch "${f%.*}.in"
  touch "${f%.*}.out"
  touch "${f%.*}.err"
  ../../concat "$f" -o "${f%.*}.c" -q > ".${f%.*}.out" 2> ".${f%.*}.err" &&
  gcc ${cArgs[@]} -Wno-unused "${f%.*}.c" -o "${f%.*}" >> ".${f%.*}.out" 2>> ".${f%.*}.err" &&
  cat "${f%.*}.in" | "./${f%.*}" >> ".${f%.*}.out" 2>> ".${f%.*}.err"
  ## TODO compare output with expected output, print passed/failed remove output files
done
cd ..
done
