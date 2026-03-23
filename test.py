#!/usr/bin/python
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
import argparse

CURRENT_DIR=os.getcwd()
TESTS_REL_PATH= "/tests/"
TESTS_PATH=CURRENT_DIR+TESTS_REL_PATH

C_ARGS=["-g","-lm"]
nTested=0
nPassed=0
failed=[]
concatPath=CURRENT_DIR+"/concat"
thread_count = None

def runInEachSubdir(f,path=TESTS_PATH):
  for file in os.listdir(os.fsencode(path)):
    file=os.fsdecode(file)
    if os.path.isdir(path+file):
      f(path+file+"/")

def runTest(fileName,filePath,*,cctFlags=None):
  basePath=filePath+fileName[:-len(".concat")]
  cctFlags = cctFlags if cctFlags is not None else []
  try:
    with open(basePath+".args") as f:
      args=f.read().split("\n")
  except FileNotFoundError:
    args=[]
  try:
    inFile=open(basePath+".in",mode="r")
  except FileNotFoundError:
    inFile=None
  outPath=basePath+".out2"
  errPath=basePath+".err2"
  ## delete+append to correctly handle edits to file during comand execution
  try:
    os.remove(outPath)
  except FileNotFoundError:pass
  try:
    os.remove(errPath)
  except FileNotFoundError:pass
  outFile=open(outPath,mode="a+")
  errFile=open(errPath,mode="a+")
  codePath=basePath
  retCode=(subprocess.run([
     concatPath,
     filePath+fileName,
     "-l","./lib/",
     "-o",codePath,
     *cctFlags
  ],stdout=outFile,stderr=errFile).returncode==0 and
  subprocess.run([
      codePath,
      *args
  ],stdin=inFile,stdout=outFile,stderr=errFile).returncode)
  if inFile is not None:
    inFile.close()
  outFile.close()
  errFile.close()
  with open(outPath,mode="r") as f:
    progOut=f.read()
  with open(errPath,mode="r") as f:
    progErr=f.read()
  if os.path.isfile(basePath+".out"):
    with open(basePath+".out",mode="r") as f:
      expectedProgOut=f.read()
  else:
    expectedProgOut=""
  if os.path.isfile(basePath+".err"):
    with open(basePath+".err",mode="r") as f:
      expectedProgErr=f.read()
  else:
    expectedProgErr=""
  failedPath = None
  if progErr!=expectedProgErr:
    print(fileName+": FAILED (err)")
    failedPath = filePath+fileName
  elif progOut!=expectedProgOut:
    print(fileName+": FAILED (out)")
    failedPath = filePath+fileName
  else:
    print(fileName+": PASSED")
    try:
      os.remove(codePath+".ssa")
      os.remove(codePath+".s")
      os.remove(codePath)
    except FileNotFoundError:
      pass
    os.remove(outPath)
    os.remove(errPath)
  return failedPath

def runTests(path,*,cctFlags=None):
  global nPassed
  global nTested
  global failed
  global thread_count
  if not os.path.isdir(path):
    if os.path.isfile(path) and path.endswith(".concat"):
      runTest(path,"",cctFlags=cctFlags)
    elif os.path.isfile(path+".concat"):
      runTest(path+".concat","",cctFlags=cctFlags)
    return
  if path[-1]!='/':
    path+='/'
  print("-----------------------")
  relPath=("."+path[len(CURRENT_DIR):])if path.startswith(CURRENT_DIR)else path
  print(relPath+":\n") ## double new line
  if thread_count is not None:
    with ThreadPoolExecutor(max_workers = thread_count) as executor:
      futures = []
      for file in os.listdir(os.fsencode(path)):
        file = os.fsdecode(file)
        if file.endswith(".concat"):
          futures.append(executor.submit(runTest,file,relPath,cctFlags=cctFlags))
      nTested += len(futures)
      for future in futures:
        failedPath = future.result()  # Raises exception if occurred
        if failedPath is not None:
          failed.append(failedPath)
        else:
          nPassed +=1
    return
  for file in os.listdir(os.fsencode(path)):
    file=os.fsdecode(file)
    if file.endswith(".concat"):
      nTested += 1
      failedPath = runTest(file,relPath,cctFlags=cctFlags)
      if failedPath is not None:
        failed.append(failedPath)
      else:
        nPassed +=1

def main():
  global concatPath
  global thread_count
  ## XXX? should test-script clear console
  subprocess.run(["clear"])
  parser = argparse.ArgumentParser(description="Concat Compiler Test")
  parser.add_argument('-X', action='store_true', help='Use developement version of compiler')
  parser.add_argument('-XX', action='store_true', help='Run test code with `-X` flag (enables `-X`)')
  parser.add_argument('--full', '-full', action='store_true', help='Include examples')
  parser.add_argument('-j', type=int, help='Number of threads, should be followed by a number', default=None)
  args, unknown = parser.parse_known_args()
  # Determine behavior based on flags
  concatPath = CURRENT_DIR + "/concat"
  cctFlags = []
  if args.XX:
    concatPath = CURRENT_DIR + "/concatX"
    cctFlags.append("-X")
  elif args.X:
    concatPath = CURRENT_DIR + "/concatX"
    print("running tests on development version of compiler")
  thread_count = args.j
  if thread_count is not None:
    print(f"running asynchronously on {thread_count} threads")
  try:
    dirIndex=sys.argv.index("--")
    dirs=sys.argv[dirIndex+1:]
    if args.full:
      print("`-full` will be ignored when using explicit arguments")
  except ValueError:
    dirs=None
  if dirs is not None:
    for d in dirs:
      runTests(d,cctFlags=cctFlags)
  else:
    runInEachSubdir(lambda p:runTests(p,cctFlags=cctFlags))
    if args.full:
      runTests(CURRENT_DIR+"/examples/",cctFlags=cctFlags)
  if nPassed==nTested:
    print(f"\nSUCCESS: passed {nPassed} of {nTested} tests")
    sys.exit(0)
  else:
    print(f"\nFAILED {nTested-nPassed} of {nTested} tests:")
    for f in failed:
       print("  ",f)
    sys.exit(1)

main()

