#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>

#include <dirent.h>

typedef int32_t FileError;
static const FileError FILE_ERR_NONE=0;
static const FileError FILE_ERR_END_OF_FILE=-1;
static const FileError FILE_ERR_IO=1;
static const FileError FILE_ERR_PATH_OVERFLOW=2;
static const FileError FILE_ERR_OPEN_MODE=3;
static const FileError FILE_ERR_FILE_NOT_FOUND=4;
static const FileError FILE_ERR_FILE_IS_DIR=5;
static const FileError FILE_ERR_FILE_ALREADY_EXISTS=6;
static const FileError FILE_ERR_ACCESS_DENIED=7;
static const FileError FILE_ERR_INVALID_FILE=8;

FILE* concatIOHelper_stdin(void){
  return (FILE*)stdin;
}
FILE* concatIOHelper_stdout(void){
  return (FILE*)stdout;
}
FILE* concatIOHelper_stderr(void){
  return (FILE*)stderr;
}
void concatIOHelper_resetErrno(void){
  errno=0;
}
FileError concatIOHelper_checkErrno(void){
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

char* concatIOHelper_readdirname(DIR* dir){
  struct dirent *entry = readdir(dir);
  if (entry) return entry -> d_name;
  return NULL;
}
