#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


void* concat_mem_dalloc (size_t size,int64_t count){
  if(size*count==0)
    return NULL;
  void* res=malloc(count*size);
  if(res==NULL){
    fputs("malloc failed\n",stderr);
    exit(EXIT_FAILURE);
  }
  return res;
}
void* concat_mem_dcalloc (size_t size,int64_t count){
  if(size==0||count==0)
    return NULL;
  void* res=calloc(count,size);
  if(res==NULL){
    fputs("calloc failed\n",stderr);
    exit(EXIT_FAILURE);
  }
  return res;
}
void* concat_mem_drealloc (size_t size,void* prev,int64_t count){
  void* res=realloc(prev,count*size);
  if(res==NULL){
    fputs("realloc failed\n",stderr);
    exit(EXIT_FAILURE);
  }
  return res;
}
void* concat_mem_dclear(size_t size,void* ptr,int64_t count){
  memset(ptr,0,count*size);
  return ptr;
}
void* concat_mem_dcopy(size_t size,void* target,void const* src,int64_t count){
  memcpy(target,src,count*size);
  return target;
}
void* concat_mem_dmove(size_t size,void* target,void const* src,int64_t count){
  memmove(target,src,count*size);
  return target;
}
void concat_mem_dfree(size_t size,void* ptr){
  (void)size;//size only exits as type-info for concat
  free(ptr);
}
