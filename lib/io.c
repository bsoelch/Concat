#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>

typedef void concatIOHelper_File;

typedef int32_t IOError;
static const IOError FILE_ERR_NONE=0;
static const IOError FILE_ERR_END_OF_FILE=-1;
static const IOError FILE_ERR_IO=1;
static const IOError FILE_ERR_PATH_OVERFLOW=2;
static const IOError FILE_ERR_OPEN_MODE=3;
static const IOError FILE_ERR_FILE_NOT_FOUND=4;
static const IOError FILE_ERR_FILE_IS_DIR=5;
static const IOError FILE_ERR_FILE_ALREADY_EXISTS=6;
static const IOError FILE_ERR_ACCESS_DENIED=7;
static const IOError FILE_ERR_INVALID_FILE=8;

concatIOHelper_File* concatIOHelper_stdin(void){
  return (concatIOHelper_File*)stdin;
}
concatIOHelper_File* concatIOHelper_stdout(void){
  return (concatIOHelper_File*)stdout;
}
concatIOHelper_File* concatIOHelper_stderr(void){
  return (concatIOHelper_File*)stderr;
}
void concatIOHelper_resetErrno(void){
  errno=0;
}
IOError concatIOHelper_checkErrno(void){
  switch(errno){
    case 0:
      return FILE_ERR_NONE;
    case EINVAL://wrong open mode
      return FILE_ERR_OPEN_MODE;
    case ENOENT://file does not exist
      return FILE_ERR_FILE_NOT_FOUND;
    case EISDIR:  //file is directory
      return FILE_ERR_FILE_IS_DIR;
    case EEXIST:  //file already exists
      return FILE_ERR_FILE_ALREADY_EXISTS;
    case EACCES://access denied
    case EROFS://cannot write file
      return FILE_ERR_ACCESS_DENIED;
    case EBADF:
      return FILE_ERR_INVALID_FILE;
  }
  return FILE_ERR_IO;
}
// wrappers converting between FILE* and void*
concatIOHelper_File* concatIOHelper_fopen(char const* path,char const* mode){
  return (concatIOHelper_File*)fopen(path,mode);
}
void concatIOHelper_clearerr(concatIOHelper_File* f){
  clearerr((FILE*)f);
}
int concatIOHelper_feof(concatIOHelper_File* f){
  return feof((FILE*)f);
}
int concatIOHelper_ferror(concatIOHelper_File* f){
  return ferror((FILE*)f);
}
size_t concatIOHelper_fread(void* buffer,size_t eltSize,size_t eltCount,concatIOHelper_File* f){
  return fread(buffer,eltSize,eltCount,(FILE*)f);
}
size_t concatIOHelper_fwrite(void const* buffer,size_t eltSize,size_t eltCount,concatIOHelper_File* f){
  return fwrite(buffer,eltSize,eltCount,(FILE*)f);
}
int concatIOHelper_fflush(concatIOHelper_File* f){
  return fflush((FILE*)f);
}
int concatIOHelper_fclose(concatIOHelper_File* f){
  return fclose((FILE*)f);
}

