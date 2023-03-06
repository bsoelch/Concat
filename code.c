#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct array3Impl array3;
struct array3Impl{
int8_t const* data;
int64_t sizes[1];
};
const int8_t arrayData0[13] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64,0x21,0x00};
const int8_t arrayData1[2] = {0xa,0x00};
//global code
// ./code.concat
void* concat_alloc (size_t, int64_t);
void* concat_stdOut (void);
void* concat_fopen (array3);
void concat_fputs (void*, array3);
void concat_puts (array3);
void concat_fclose (void*);
//procedures code
// ./code.concat
int main(void){
int32_t const tmp0 = (((int32_t)1024)*((int32_t)1024));
int32_t const tmp1 = (tmp0*((int32_t)16));
int32_t* const tmp2 = concat_alloc(((size_t)sizeof(int32_t)),((int64_t)tmp1));
int32_t* const local0 = tmp2;
printf("%p\n",(void const*)local0);
int32_t const tmp3 = (*((local0)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp3);
int32_t const tmp4 = (*((local0)+(((int32_t)1000000))));
printf("%"PRIi32"\n",tmp4);
int8_t const* const tmp5 = (arrayData0+0);
concat_puts(((array3){.data=tmp5,.sizes={12}}));
}
void concat_puts (array3 arg0){
void* const tmp6 = concat_stdOut();
concat_fputs(tmp6,arg0);
int8_t const* const tmp7 = (arrayData1+0);
concat_fputs(tmp6,((array3){.data=tmp7,.sizes={1}}));
}
