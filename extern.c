#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

void* concat_alloc (size_t size,int64_t count){
  void* res=malloc(count*size);
  if(res==NULL){
    fputs("malloc failed\n",stderr);
    exit(EXIT_FAILURE);
  }
  return res;
}

#define MAX_PATH 4096
typedef struct{
  int8_t const* data;
  int64_t sizes[1];
}string;
static char fopenBuffer[MAX_PATH+1];
void* concat_stdOut(void){
  return stdout;
}
void* concat_fopen (string str){//XXX error flag / input type
  char const* path=(char const*)str.data;
  if(str.data[str.sizes[0]-1]!='\0'){
    if(str.sizes[0]>MAX_PATH){
      fputs("path overflow\n",stderr);
      exit(EXIT_FAILURE);
    }
    memcpy(fopenBuffer,str.data,str.sizes[0]*sizeof(int8_t));
    fopenBuffer[str.sizes[0]]='\0';
    path=fopenBuffer;
  }
  FILE* file=fopen(path,"w+");
  if(file==NULL){
    fputs("fopen failed\n",stderr);
    exit(EXIT_FAILURE);
  }
  return file;
}
void concat_fputs (void* file, string str){//TODO error handling
  fwrite(str.data,sizeof(int8_t),str.sizes[0],file);
}
void concat_fclose (void* file){
  fclose(file);
}
