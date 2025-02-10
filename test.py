#!/usr/bin/python
import os
import subprocess
import sys

CURRENT_DIR=os.getcwd()
TESTS_REL_PATH= "/tests/"
TESTS_PATH=CURRENT_DIR+TESTS_REL_PATH

C_ARGS=["-g","-lm"]
nTested=0
nPassed=0
failed=[]
concatPath=CURRENT_DIR+"/concat"

def runInEachSubdir(f,path=TESTS_PATH):
  for file in os.listdir(os.fsencode(path)):
    file=os.fsdecode(file)
    if os.path.isdir(path+file):
      f(path=(path+file+"/"))

def runTest(fileName,filePath):
  global nTested
  global nPassed
  print(fileName+": ",end="")
  basePath=filePath+fileName[:-len(".concat")]
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
  ssaPath=basePath+".ssa"
  asmPath=basePath+".s"
  codePath=basePath
  retCode=(subprocess.run([
    concatPath,
    filePath+fileName,
    "-o",ssaPath,
    "-l","./lib/"
  ],stdout=outFile,stderr=errFile).returncode==0 and
  subprocess.run([
      "qbe",
      ssaPath,
      "-o",asmPath
    ],stdout=outFile,stderr=errFile).returncode==0 and
  subprocess.run([
      "cc",
      *C_ARGS,
      asmPath,
      "./extern.c",
      "-o",codePath
    ],stdout=outFile,stderr=errFile).returncode==0 and
  subprocess.run([
      codePath,
      *args
    ],stdin=inFile,stdout=outFile,stderr=errFile).returncode)
  if inFile is not None:
    inFile.close()
  outFile.close()
  errFile.close()
  nTested+=1
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
  if progErr!=expectedProgErr:
    print("FAILED (err)")
    failed.append(filePath+fileName)
  elif progOut!=expectedProgOut:
    print("FAILED (out)")
    failed.append(filePath+fileName)
  else:
    print("PASSED")
    nPassed+=1
    try:
      os.remove(ssaPath)
      os.remove(asmPath)
      os.remove(codePath)
    except FileNotFoundError:
      pass
    os.remove(outPath)
    os.remove(errPath)

def runTests(path):
  if not os.path.isdir(path):
    if os.path.isfile(path) and path.endswith(".concat"):
      runTest(path,"")
    elif os.path.isfile(path+".concat"):
      runTest(path+".concat","")
    return
  if path[-1]!='/':
    path+='/'
  print("-----------------------")
  relPath=("."+path[len(CURRENT_DIR):])if path.startswith(CURRENT_DIR)else path
  print(relPath+":\n") ## double new line
  for file in os.listdir(os.fsencode(path)):
    file=os.fsdecode(file)
    if file.endswith(".concat"):
      runTest(file,relPath)

def main():
  global concatPath
  ## XXX? should test-script clear console
  subprocess.run(["clear"])
  ## TODO ignore `-X` / `-full` after `--`
  if "-X" in sys.argv:## experimental mode
    concatPath=CURRENT_DIR+"/concatX"
    print("running tests in experimental mode")
  full = "-full" in sys.argv
  try:
    dirIndex=sys.argv.index("--")
    dirs=sys.argv[dirIndex+1:]
    if full:
      print("`-full` will be ignored when using explicit arguments")
  except ValueError:
    dirs=None
  if dirs is not None:
    for d in dirs:
      runTests(d)
  else:
    runInEachSubdir(runTests)
    if full:
      runTests(CURRENT_DIR+"/examples/")
  if nPassed==nTested:
    print(f"\nSUCCESS: passed {nPassed} of {nTested} tests")
    sys.exit(0)
  else:
    print(f"\nFAILED {nTested-nPassed} of {nTested} tests:")
    for f in failed:
       print("  ",f)
    sys.exit(1)

main()

