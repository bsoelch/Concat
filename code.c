#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple3Impl tuple3;
typedef struct tuple5Impl tuple5;
typedef struct tuple22Impl tuple22;
typedef struct array2Impl array2;
typedef struct array5Impl array5;
struct array2Impl{
int8_t const* data;
int64_t sizes[1];
};
struct tuple3Impl{
void* e0;
int32_t e1;
};
struct tuple5Impl{
int64_t e0;
int32_t e1;
};
struct tuple22Impl{
int8_t* e0;
int64_t e1;
int64_t e2;
};
struct array5Impl{
int8_t data[64];
};
const int8_t arrayData0[62] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x58,0x59,0x5a,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x00};
const int8_t arrayData1[13] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64,0x21,0x00};
const int8_t arrayData2[3] = {0x30,0x78,0x00};
const int8_t arrayData3[3] = {0x30,0x62,0x00};
const int8_t arrayData4[2] = {0xa,0x00};
void concatInternal_checkArrayBounds(int64_t index,int64_t length){
  if(index>=0 && index<length)
    return;
  fprintf(stderr,"array index out of bounds: %"PRIi64" size: %"PRIi64"\n",index,length);
  exit(1);
}
//global declarations
// ./code.concat
int32_t concat__Fcode_dconcat_Etest (void);
// ./include.concat
extern void concat_test (void);
// ./tests/parser/empty.concat
// ./lib/io.concat
extern void* concat_io_dstdErr (void);
int32_t const concat_io_dIOError;
int32_t concat_io_dfputc (void*, int8_t);
int32_t concat_io_dfputs (void*, array2);
tuple3 concat_io_dfappendC (void*, int32_t, int8_t);
extern int32_t concat_io_dfclose (void*);
extern void* concat_io_dstdOut (void);
extern int32_t concat_io_dfflush (void*);
extern void* concat_io_dstdIn (void);
extern tuple5 concat_io_dfread (void*, int8_t*, int64_t, int64_t);
tuple3 concat_io_dfappendStr (void*, int32_t, array2);
int32_t const concat_io_dERROR__NONE;
extern tuple5 concat_io_dfwrite (void*, int8_t const*, int64_t, int64_t);
int32_t concat_io_dputs (array2);
extern tuple3 concat_io_dfopen (array2);
tuple3 concat_io_dfstart (void*);
// ./lib/mem.concat
extern void* concat_mem_dcalloc (size_t, int64_t);
extern void* concat_mem_drealloc (size_t, void*, int64_t);
extern void* concat_mem_dclear (size_t, void*, int64_t);
extern void concat_mem_dfree (size_t, void*);
extern void* concat_mem_dalloc (size_t, int64_t);
extern void* concat_mem_dcopy (size_t, void*, void const*, int64_t);
extern void* concat_mem_dmove (size_t, void*, void const*, int64_t);
// ./lib/string.concat
tuple22 concat_std_dsb__appendInt_X3f (tuple22, int64_t, int32_t);
tuple22 concat_std_dsb__appendUHex (tuple22, int64_t);
tuple22 concat_std_dnewStringBuffer (int64_t);
tuple22 concat_std_dsb__appendBin (tuple22, int64_t);
tuple22 concat_std_dstringBufferEnsureCap (tuple22, int64_t);
int8_t const* const concat__Flib_X2fstring_dconcat_Estd_dstringChars;
tuple22 concat_std_dsb__appendC (tuple22, int8_t);
tuple22 concat_std_dsb__appendUBin (tuple22, int64_t);
tuple22 concat_std_dsb__appendI (tuple22, int64_t);
tuple22 concat_std_dsb__appendU (tuple22, int64_t);
tuple22 concat_std_dsb__appendStr (tuple22, array2);
tuple22 concat_std_dsb__appendInt_X3f_X3f (tuple22, int64_t, int32_t, bool);
tuple22 concat_std_dsb__appendHex (tuple22, int64_t);
//global code
// ./lib/io.concat
int32_t const concat_io_dERROR__NONE = 0;
// ./lib/string.concat
int8_t const* const concat__Flib_X2fstring_dconcat_Estd_dstringChars = (arrayData0+0);
//procedures code
// ./code.concat
int main(void){
int32_t const tmp0 = (((int32_t)1024)*((int32_t)1024));
int32_t const tmp1 = (tmp0*((int32_t)16));
int32_t* const tmp2 = concat_mem_dalloc(((size_t)sizeof(int32_t)),((int64_t)tmp1));
int32_t* const local0 = tmp2;
printf("%p\n",(void const*)local0);
int32_t const tmp3 = (*((local0)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp3);
int32_t const tmp4 = (*((local0)+(((int32_t)1000000))));
printf("%"PRIi32"\n",tmp4);
int32_t const tmp5 = concat_io_dputs(((array2){.data=(arrayData1+0),.sizes={12}}));
printf("%"PRIi32"\n",tmp5);
tuple22 const tmp6 = concat_std_dnewStringBuffer(((int64_t)16));
tuple22 const tmp7 = concat_std_dsb__appendStr(tmp6,((array2){.data=(arrayData1+0),.sizes={5}}));
tuple22 const tmp8 = concat_std_dsb__appendC(tmp7,((int8_t)32));
tuple22 const tmp9 = concat_std_dsb__appendStr(tmp8,((array2){.data=(arrayData1+6),.sizes={5}}));
tuple22 const tmp10 = concat_std_dsb__appendC(tmp9,((int8_t)10));
tuple22 const tmp11 = concat_std_dsb__appendI(tmp10,((int64_t)-12345678));
tuple22 const tmp12 = concat_std_dsb__appendC(tmp11,((int8_t)10));
tuple22 const tmp13 = concat_std_dsb__appendStr(tmp12,((array2){.data=(arrayData2+0),.sizes={2}}));
tuple22 const tmp14 = concat_std_dsb__appendHex(tmp13,((int64_t)19088743));
tuple22 const tmp15 = concat_std_dsb__appendC(tmp14,((int8_t)10));
tuple22 const tmp16 = concat_std_dsb__appendStr(tmp15,((array2){.data=(arrayData3+0),.sizes={2}}));
tuple22 const tmp17 = concat_std_dsb__appendBin(tmp16,((int64_t)201));
tuple22 const tmp18 = concat_std_dsb__appendC(tmp17,((int8_t)10));
tuple22 const tmp19 = concat_std_dsb__appendHex(tmp18,((int64_t)9223372036854775807));
tuple22 const tmp20 = concat_std_dsb__appendC(tmp19,((int8_t)10));
tuple22 const tmp21 = concat_std_dsb__appendI(tmp20,((int64_t)-9223372036854775807 -1));
tuple22 const tmp22 = concat_std_dsb__appendC(tmp21,((int8_t)10));
tuple22 const tmp23 = concat_std_dsb__appendU(tmp22,((int64_t)-1));
tuple22 const tmp24 = concat_std_dsb__appendC(tmp23,((int8_t)10));
tuple22 const tmp25 = concat_std_dsb__appendUBin(tmp24,((int64_t)-1));
tuple22 const tmp26 = concat_std_dsb__appendC(tmp25,((int8_t)10));
tuple22 const local1 = tmp26;
void* const tmp27 = concat_io_dstdOut();
int8_t* const tmp28 = (local1).e0;
int64_t const tmp29 = (local1).e2;
tuple5 const tmp30 = concat_io_dfwrite(tmp27,((int8_t const*)tmp28),((int64_t)0),tmp29);
int32_t const tmp31 = (tmp30).e1;
printf("%"PRIi32"\n",tmp31);
int64_t const tmp32 = (tmp30).e0;
printf("%"PRIi64"\n",tmp32);
}
int32_t concat__Fcode_dconcat_Etest (void){
return ((int32_t)9);
}
// ./lib/io.concat
int32_t concat_io_dfputc (void* arg0, int8_t arg1){
int8_t const* const tmp0 = &(arg1);
tuple5 const tmp1 = concat_io_dfwrite(arg0,((int8_t const*)tmp0),((int64_t)0),((int64_t)1));
int64_t const tmp2 = (tmp1).e0;
int32_t const tmp3 = (tmp1).e1;
return tmp3;
}
int32_t concat_io_dfputs (void* arg0, array2 arg1){
int64_t local0 = 0;
int64_t const tmp0 = ((arg1).sizes[0]);
int64_t local1 = tmp0;
int8_t const* const tmp1 = ((int8_t const*)arg1.data);
int8_t const* const local2 = tmp1;
do{
bool const tmp2 = (local1>((int64_t)0));
if(!tmp2)
  break;
tuple5 const tmp3 = concat_io_dfwrite(arg0,local2,local0,local1);
int32_t const tmp4 = (tmp3).e1;
int32_t const tmp5 = tmp4;
bool const tmp6 = (tmp5!=((int32_t)0));
if(tmp6){
int64_t const tmp7 = (tmp3).e0;
return tmp4;
}
int64_t const tmp8 = (tmp3).e0;
int64_t const tmp9 = (local1-tmp8);
local1 = tmp9;
int64_t const tmp10 = (local0+tmp8);
local0 = tmp10;
}while(1);
return ((int32_t)0);
}
tuple3 concat_io_dfstart (void* arg0){
return (tuple3){.e0=arg0,.e1=((int32_t)0)};
}
tuple3 concat_io_dfappendStr (void* arg0, int32_t arg1, array2 arg2){
int32_t const tmp0 = arg1;
bool const tmp1 = (tmp0!=((int32_t)0));
if(tmp1){
return (tuple3){.e0=arg0,.e1=arg1};
}
int32_t const tmp2 = concat_io_dfputs(arg0,arg2);
return (tuple3){.e0=arg0,.e1=tmp2};
}
tuple3 concat_io_dfappendC (void* arg0, int32_t arg1, int8_t arg2){
int32_t const tmp0 = arg1;
bool const tmp1 = (tmp0!=((int32_t)0));
if(tmp1){
return (tuple3){.e0=arg0,.e1=arg1};
}
int32_t const tmp2 = concat_io_dfputc(arg0,arg2);
return (tuple3){.e0=arg0,.e1=tmp2};
}
int32_t concat_io_dputs (array2 arg0){
void* const tmp0 = concat_io_dstdOut();
tuple3 const tmp1 = concat_io_dfstart(tmp0);
void* const tmp2 = (tmp1).e0;
int32_t const tmp3 = (tmp1).e1;
tuple3 const tmp4 = concat_io_dfappendStr(tmp2,tmp3,arg0);
void* const tmp5 = (tmp4).e0;
int32_t const tmp6 = (tmp4).e1;
tuple3 const tmp7 = concat_io_dfappendStr(tmp5,tmp6,((array2){.data=(arrayData4+0),.sizes={1}}));
void* const tmp8 = (tmp7).e0;
int32_t const tmp9 = (tmp7).e1;
return tmp9;
}
// ./lib/string.concat
tuple22 concat_std_dnewStringBuffer (int64_t arg0){
int8_t* const tmp0 = concat_mem_dalloc(((size_t)sizeof(int8_t)),arg0);
tuple22 const tmp1 = {.e0=tmp0,.e1=arg0,.e2=((int64_t)0)};
return tmp1;
}
tuple22 concat_std_dstringBufferEnsureCap (tuple22 arg0, int64_t arg1){
tuple22 local0 = arg0;
int64_t const tmp0 = (local0).e1;
bool const tmp1 = (tmp0>arg1);
if(tmp1){
return local0;
}
int64_t const tmp2 = (arg1/((int64_t)8));
int64_t const tmp3 = (arg1+tmp2);
int64_t const tmp4 = (tmp3+((int64_t)8));
int8_t** const tmp5 = &((local0).e0);
int8_t* const tmp6 = concat_mem_drealloc(((size_t)sizeof(int8_t)),(*(tmp5)),tmp4);
(local0).e0 = tmp6;
(local0).e1 = tmp4;
return local0;
}
tuple22 concat_std_dsb__appendC (tuple22 arg0, int8_t arg1){
tuple22 local0 = arg0;
int64_t const tmp0 = (local0).e2;
int64_t const tmp1 = (tmp0+((int64_t)1));
tuple22 const tmp2 = concat_std_dstringBufferEnsureCap(local0,tmp1);
local0 = tmp2;
int64_t* const tmp3 = &((local0).e2);
int64_t const tmp4 = ((*(tmp3))++);
int8_t** const tmp5 = &((local0).e0);
(*(((*(tmp5)))+(tmp4))) = arg1;
return local0;
}
tuple22 concat_std_dsb__appendStr (tuple22 arg0, array2 arg1){
tuple22 local0 = arg0;
int64_t const tmp0 = (local0).e2;
int64_t const tmp1 = ((arg1).sizes[0]);
int64_t const tmp2 = (tmp0+tmp1);
tuple22 const tmp3 = concat_std_dstringBufferEnsureCap(local0,tmp2);
local0 = tmp3;
int8_t* const tmp4 = (local0).e0;
int64_t const tmp5 = (local0).e2;
int8_t* const tmp6 = (tmp4+tmp5);
int8_t const* const tmp7 = ((int8_t const*)arg1.data);
int64_t const tmp8 = ((arg1).sizes[0]);
int8_t* const tmp9 = concat_mem_dcopy(((size_t)sizeof(int8_t)),tmp6,tmp7,tmp8);
int64_t const tmp10 = (local0).e2;
int64_t const tmp11 = ((arg1).sizes[0]);
int64_t const tmp12 = (tmp10+tmp11);
(local0).e2 = tmp12;
return local0;
}
tuple22 concat_std_dsb__appendInt_X3f_X3f (tuple22 arg0, int64_t arg1, int32_t arg2, bool arg3){
bool const tmp0 = (arg2<((int32_t)2));
if(tmp0){
return arg0;
}
array5 local0 = {0};
bool const tmp1 = (arg1<((int64_t)0));
bool const tmp2 = (tmp1&arg3);
tuple22 tmp6 = arg0;
int64_t tmp5 = arg1;
if(tmp2){
tuple22 const tmp3 = concat_std_dsb__appendC(arg0,((int8_t)45));
int64_t const tmp4 = (-arg1);
tmp5 = tmp4;
tmp6 = tmp3;
}else{
bool const tmp7 = (arg1==((int64_t)0));
if(tmp7){
tuple22 const tmp8 = concat_std_dsb__appendC(arg0,((int8_t)48));
return tmp8;
}
}
tuple22 tmp15;
int32_t tmp14;
int64_t tmp13;
int64_t tmp9 = tmp5;
int32_t tmp10 = 63;
tuple22 tmp11 = tmp6;
do{
bool const tmp12 = (tmp9!=((int64_t)0));
tmp13 = tmp9;
tmp14 = tmp10;
tmp15 = tmp11;
if(!tmp12)
  break;
int64_t const tmp16 = ((int64_t)arg2);
int64_t const tmp17 = ((int64_t)(((uint64_t)tmp13)%((uint64_t)tmp16)));
concatInternal_checkArrayBounds(tmp17,((int64_t)61));
int8_t const tmp18 = ((((int8_t const*)(arrayData0+0)))[tmp17]);
concatInternal_checkArrayBounds(tmp14,((int64_t)64));
((local0).data[tmp14]) = tmp18;
int32_t const tmp19 = (tmp14-((int32_t)1));
int64_t const tmp20 = ((int64_t)arg2);
int64_t const tmp21 = ((int64_t)(((uint64_t)tmp13)/((uint64_t)tmp20)));
tmp9 = tmp21;
tmp10 = tmp19;
tmp11 = tmp15;
}while(1);
tuple22 tmp27;
int32_t tmp26;
int32_t tmp22 = tmp14;
tuple22 tmp23 = tmp15;
do{
int32_t const tmp24 = (tmp22+((int32_t)1));
bool const tmp25 = (tmp24<((int32_t)64));
tmp26 = tmp24;
tmp27 = tmp23;
if(!tmp25)
  break;
concatInternal_checkArrayBounds(tmp26,((int64_t)64));
int8_t const tmp28 = ((local0).data[tmp26]);
tuple22 const tmp29 = concat_std_dsb__appendC(tmp27,tmp28);
tmp22 = tmp26;
tmp23 = tmp29;
}while(1);
return tmp27;
}
tuple22 concat_std_dsb__appendInt_X3f (tuple22 arg0, int64_t arg1, int32_t arg2){
tuple22 const tmp0 = concat_std_dsb__appendInt_X3f_X3f(arg0,arg1,arg2,((bool)1));
return tmp0;
}
tuple22 concat_std_dsb__appendBin (tuple22 arg0, int64_t arg1){
tuple22 const tmp0 = concat_std_dsb__appendInt_X3f(arg0,arg1,((int32_t)2));
return tmp0;
}
tuple22 concat_std_dsb__appendI (tuple22 arg0, int64_t arg1){
tuple22 const tmp0 = concat_std_dsb__appendInt_X3f(arg0,arg1,((int32_t)10));
return tmp0;
}
tuple22 concat_std_dsb__appendHex (tuple22 arg0, int64_t arg1){
tuple22 const tmp0 = concat_std_dsb__appendInt_X3f(arg0,arg1,((int32_t)16));
return tmp0;
}
tuple22 concat_std_dsb__appendUBin (tuple22 arg0, int64_t arg1){
tuple22 const tmp0 = concat_std_dsb__appendInt_X3f_X3f(arg0,arg1,((int32_t)2),((bool)0));
return tmp0;
}
tuple22 concat_std_dsb__appendU (tuple22 arg0, int64_t arg1){
tuple22 const tmp0 = concat_std_dsb__appendInt_X3f_X3f(arg0,arg1,((int32_t)10),((bool)0));
return tmp0;
}
tuple22 concat_std_dsb__appendUHex (tuple22 arg0, int64_t arg1){
tuple22 const tmp0 = concat_std_dsb__appendInt_X3f_X3f(arg0,arg1,((int32_t)16),((bool)0));
return tmp0;
}
