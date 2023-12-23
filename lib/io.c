#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>

typedef void concat_File;
typedef int32_t IOError;
typedef int32_t OpenMode;

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

static const OpenMode OPEN_READ=1;
static const OpenMode OPEN_WRITE=2;
static const OpenMode OPEN_APPEND=4;

concat_File* concat_io_dstdIn(void){
  return stdin;
}
concat_File* concat_io_dstdOut(void){
  return stdout;
}
concat_File* concat_io_dstdErr(void){
  return stderr;
}

static IOError getErrorId(int errnoVal){
  switch(errnoVal){
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

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif
static char fopenBuffer[MAX_PATH+1];//XXX make buffer thread safe
IOError concat_io_d__fopen(int8_t const* nameChars,int64_t nameLength,OpenMode mode,FILE** file){
  *file=NULL;
  char const* path=(char const*)nameChars;
  if(nameChars[nameLength-1]!='\0'){
    if(nameLength>MAX_PATH){
      return FILE_ERR_PATH_OVERFLOW;
    }
    memcpy(fopenBuffer,nameChars,nameLength*sizeof(int8_t));
    fopenBuffer[nameLength]='\0';
    path=fopenBuffer;
  }
  char const* openMode="r+b";
  if((mode&OPEN_READ)!=0){
    if((mode&OPEN_APPEND)!=0){
      openMode="a+b";
    }else if((mode&OPEN_WRITE)==0){
      openMode="rb";
    }
  }else if(mode!=0){
    if((mode&OPEN_APPEND)!=0){
      openMode="ab";
    }else{
      openMode="wb";
    }
  }
  errno=0;//reset errno
  *file=fopen(path,openMode);
  if(*file==NULL){
    return getErrorId(errno);
  }
  return FILE_ERR_NONE;
}
IOError concat_io_d__fread(concat_File* file,int8_t* buffer,int64_t off,int64_t count,int64_t* N){
  *N=0;
  if(file==NULL)
    return FILE_ERR_INVALID_FILE;
  clearerr(file);//reset error flags
  errno=0;//reset errno
  *N=fread(buffer+off,sizeof(int8_t),count,file);
  if(feof(file))
    return FILE_ERR_END_OF_FILE;
  if(ferror(file))
    return getErrorId(errno);
  return FILE_ERR_NONE;
}
IOError concat_io_d__fwrite(concat_File* file,int8_t* buffer,int64_t off,int64_t count,int64_t* N){
  *N=0;
  if(file==NULL)
    return FILE_ERR_INVALID_FILE;
  clearerr(file);//reset error flags
  errno=0;//reset errno
  *N=fwrite(buffer+off,sizeof(int8_t),count,file);
  if(feof(file))
    return FILE_ERR_END_OF_FILE;
  if(ferror(file))
    return getErrorId(errno);
  return FILE_ERR_NONE;
}
IOError concat_io_dfflush(concat_File* file){
  if(file==NULL)
    return FILE_ERR_INVALID_FILE;
  errno=0;//reset errno
  if(fflush(file)!=0)
    return getErrorId(errno);
  return FILE_ERR_NONE;
}
IOError concat_io_dfclose(concat_File* file){
  if(file==NULL)
    return FILE_ERR_NONE;
  errno=0;//reset errno
  if(fclose(file)!=0)
    return getErrorId(errno);
  return FILE_ERR_NONE;
}

