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

def runTests(path):
  global nTested
  global nPassed
  prevDir=os.getcwd()
  print("-----------------------")
  relPath=("."+path[len(CURRENT_DIR):])if path.startswith(CURRENT_DIR)else path
  print(relPath+":\n") ## double new line
  for file in os.listdir(os.fsencode(path)):
    file=os.fsdecode(file)
    if file.endswith(".concat"):
      print(file+": ",end="")
      basePath=relPath+file[:-len(".concat")]
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
        relPath+file,
        "-o",ssaPath,
        "-l","./lib/",
        "-q"
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
      if inFile!=None:inFile.close()
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
        failed.append(relPath+file)
      elif progOut!=expectedProgOut:
        print("FAILED (out)")
        failed.append(relPath+file)
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

if "-X" in sys.argv:## experimental mode
    concatPath=CURRENT_DIR+"/concatX"
    print("running tests in experimental mode")

## XXX? should test-script clear console
subprocess.run(["clear"])
runInEachSubdir(runTests)
runTests(CURRENT_DIR+"/examples/")
if nPassed==nTested:
  print(f"\nSUCCESS: passed {nPassed} of {nTested} tests")
else:
  print(f"\nFAILED {nTested-nPassed} of {nTested} tests:")
  for f in failed:
     print("  ",f)
  


