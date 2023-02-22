#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


int32_t* concat_allocI32 (int64_t count){
  void* res=malloc(count*sizeof(int32_t));
  if(res==NULL){
    fputs("malloc failed",stderr);
    exit(EXIT_FAILURE);
  }
  return res;
}
