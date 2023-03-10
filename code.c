#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple11Impl tuple11;
typedef struct tuple15Impl tuple15;
typedef struct tuple31Impl tuple31;
typedef struct tuple44Impl tuple44;
typedef struct tuple46Impl tuple46;
typedef struct tuple60Impl tuple60;
typedef struct array1Impl array1;
typedef struct array3Impl array3;
struct tuple11Impl{
int8_t const* e0;
int64_t e1;
};
struct tuple15Impl{
tuple11 e0;
tuple11 e1;
};
struct tuple31Impl{
int8_t* e0;
int64_t e1;
int64_t e2;
};
struct array1Impl{
int8_t const* data;
int64_t sizes[1];
};
struct array3Impl{
int8_t data[64];
};
struct tuple44Impl{
void* e0;
int32_t e1;
};
struct tuple46Impl{
int64_t e0;
int32_t e1;
};
struct tuple60Impl{
tuple31 e0;
int32_t e1;
};
const int8_t arrayData0[62] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x58,0x59,0x5a,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x00};
const int8_t arrayData1[14] = {0x2e,0x2f,0x63,0x6f,0x64,0x65,0x2e,0x63,0x6f,0x6e,0x63,0x61,0x74,0x00};
const int8_t arrayData2[2] = {0xa,0x00};
void concatInternal_checkArrayBounds(int64_t index,int64_t length){
  if(index>=0 && index<length)
    return;
  fprintf(stderr,"array index out of bounds: %"PRIi64" size: %"PRIi64"\n",index,length);
  exit(1);
}
//global declarations
// ./code.concat
// ./include.concat
extern void concat_test (void);
// ./tests/parser/empty.concat
// ./lib/stringIO.concat
int32_t concat_io_dfputs2 (void*, tuple11);
int32_t concat_io_dfputSb (void*, tuple31);
tuple44 concat_io_dfappendSb (void*, int32_t, tuple31);
tuple44 concat_io_dfappendStr2 (void*, int32_t, tuple11);
tuple60 concat_sb_dappendFile (tuple31, void*);
// ./lib/stringBuilder.concat
tuple31 concat_sb_dappendC (tuple31, int8_t);
tuple31 concat_sb_dappendInt_X3f (tuple31, int64_t, int32_t);
tuple31 concat_sb_dappendI (tuple31, int64_t);
tuple31 concat_sb_dappendU (tuple31, int64_t);
tuple31 concat_sb_dappendUHex (tuple31, int64_t);
tuple31 concat_sb_dappendStr (tuple31, array1);
tuple31 concat_sb_dcreate (int64_t);
tuple31 concat_sb_dappendInt_X3f_X3f (tuple31, int64_t, int32_t, bool);
tuple31 concat_sb_dappendHex (tuple31, int64_t);
tuple11 concat_sb_dtoStr (tuple31);
int8_t const* const concat__Flib_X2fstringBuilder_dconcat_Esb_dstringChars;
tuple31 concat_sb_dappendStr2 (tuple31, tuple11);
tuple31 concat_sb_dappendUBin (tuple31, int64_t);
tuple31 concat_sb_densureCap (tuple31, int64_t);
tuple31 concat_sb_dappendSb (tuple31, tuple31);
tuple31 concat_sb_dappendBin (tuple31, int64_t);
// ./lib/mem.concat
extern void* concat_mem_dcalloc (size_t, int64_t);
extern void* concat_mem_drealloc (size_t, void*, int64_t);
extern int32_t concat_mem_dcompare (size_t, void const*, void const*, int64_t);
extern void* concat_mem_dclear (size_t, void*, int64_t);
extern void concat_mem_dfree (size_t, void*);
extern void* concat_mem_dalloc (size_t, int64_t);
extern void* concat_mem_dcopy (size_t, void*, void const*, int64_t);
extern void* concat_mem_dmove (size_t, void*, void const*, int64_t);
// ./lib/string.concat
bool concat_string_disSubstring (tuple11, tuple11);
tuple11 concat_string_dtail (tuple11, int64_t);
bool concat_string_deq (tuple11, tuple11);
int32_t concat_string_dcompare (tuple11, tuple11);
tuple11 concat_string_dcreate (int8_t const*, int64_t);
int32_t concat__Flib_X2fstring_dconcat_Estring_dhash (tuple11);
tuple11 const concat__Flib_X2fstring_dconcat_Estring_dEMPTY;
tuple11 concat_string_dslice (tuple11, int64_t, int64_t);
tuple11 concat_string_dhead (tuple11, int64_t);
bool concat_string_dcontains (tuple11, int8_t);
int8_t concat__Flib_X2fstring_dconcat_Estring_dcharAt (tuple11, int64_t);
int64_t concat_string_dindexOf (tuple11, int8_t);
int64_t concat_string_dlastIndexOfStr (tuple11, tuple11);
int64_t concat_string_dlastIndexOf (tuple11, int8_t);
tuple15 concat_string_dsplit (tuple11, int8_t);
int64_t concat_string_dindexOfStr (tuple11, tuple11);
// ./lib/io.concat
int32_t const concat_io_dFILE__ERR__INVALID__FILE;
extern void* concat_io_dstdErr (void);
int32_t const concat_io_dIOError;
int32_t concat_io_dfputc (void*, int8_t);
int32_t concat_io_dfputs (void*, array1);
tuple44 concat_io_dfappendC (void*, int32_t, int8_t);
extern int32_t concat_io_dfclose (void*);
int32_t const concat_io_dFILE__ERR__ACCESS__DENIED;
int32_t const concat_io_dFILE__ERR__PATH__OVERFLOW;
int32_t const concat_io_dOPEN__WRITE;
int32_t const concat_io_dFILE__ERR__FILE__NOT__FOUND;
int32_t const concat_io_dFILE__ERROR__NONE;
extern void* concat_io_dstdOut (void);
extern int32_t concat_io_dfflush (void*);
int32_t const concat_io_dOPEN__READ;
int32_t const concat_io_dFILE__ERR__OPEN__MODE;
int32_t const concat_io_dFILE__ERR__IO;
int32_t const concat_io_dFILE__ERR__END__OF__FILE;
extern void* concat_io_dstdIn (void);
int32_t const concat_io_dOPEN__APPEND;
extern tuple46 concat_io_dfread (void*, int8_t*, int64_t, int64_t);
tuple44 concat_io_dfappendStr (void*, int32_t, array1);
int32_t const concat_io_dOpenMode;
extern tuple46 concat_io_dfwrite (void*, int8_t const*, int64_t, int64_t);
int32_t const concat_io_dFILE__ERR__FILE__IS__DIR;
int32_t concat_io_dputs (array1);
int32_t const concat_io_dFILE__ERR__FILE__ALREADY__EXISTS;
extern tuple44 concat_io_dfopen (array1, int32_t);
tuple44 concat_io_dfstart (void*);
//global code
// ./lib/stringBuilder.concat
int8_t const* const concat__Flib_X2fstringBuilder_dconcat_Esb_dstringChars = (arrayData0+0);
// ./lib/string.concat
tuple11 const concat__Flib_X2fstring_dconcat_Estring_dEMPTY = {.e0=(arrayData0+0),.e1=0};
// ./lib/io.concat
int32_t const concat_io_dFILE__ERROR__NONE = 0;
int32_t const concat_io_dFILE__ERR__END__OF__FILE = -1;
int32_t const concat_io_dFILE__ERR__IO = 1;
int32_t const concat_io_dFILE__ERR__PATH__OVERFLOW = 2;
int32_t const concat_io_dFILE__ERR__OPEN__MODE = 3;
int32_t const concat_io_dFILE__ERR__FILE__NOT__FOUND = 4;
int32_t const concat_io_dFILE__ERR__FILE__IS__DIR = 5;
int32_t const concat_io_dFILE__ERR__FILE__ALREADY__EXISTS = 6;
int32_t const concat_io_dFILE__ERR__ACCESS__DENIED = 7;
int32_t const concat_io_dFILE__ERR__INVALID__FILE = 8;
int32_t const concat_io_dOPEN__READ = 1;
int32_t const concat_io_dOPEN__WRITE = 2;
int32_t const concat_io_dOPEN__APPEND = 4;
//procedures code
// ./code.concat
int main(void){
tuple44 const tmp0 = concat_io_dfopen(((array1){.data=(arrayData1+0),.sizes={13}}),((int32_t)1));
void* const tmp1 = (tmp0).e0;
void* const local2 = tmp1;
tuple31 tmp2 = concat_sb_dcreate(((int64_t)128));
tuple31 tmp3 = concat_sb_dappendC(tmp2,((int8_t)10));
tuple60 const tmp4 = concat_sb_dappendFile(tmp3,local2);
tuple31 tmp5 = (tmp4).e0;
tuple31 tmp6 = concat_sb_dappendC(tmp5,((int8_t)10));
void* const tmp7 = concat_io_dstdOut();
int32_t const tmp8 = concat_io_dfputSb(tmp7,tmp6);
}
// ./lib/stringIO.concat
int32_t concat_io_dfputs2 (void* arg0, tuple11 arg1){
int64_t local0 = 0;
tuple11 tmp0 = arg1;
int64_t const tmp1 = (tmp0).e1;
int64_t local1 = tmp1;
tuple11 tmp2 = arg1;
int8_t const* const tmp3 = (tmp2).e0;
int8_t const* const local2 = tmp3;
do{
bool const tmp4 = (local1>((int64_t)0));
if(!tmp4)
  break;
tuple46 const tmp5 = concat_io_dfwrite(arg0,local2,local0,local1);
int32_t const tmp6 = (tmp5).e1;
int32_t const tmp7 = tmp6;
bool const tmp8 = (tmp7!=((int32_t)0));
if(tmp8){
int64_t const tmp9 = (tmp5).e0;
return tmp6;
}
int64_t const tmp10 = (tmp5).e0;
int64_t const tmp11 = (local1-tmp10);
local1 = tmp11;
int64_t const tmp12 = (local0+tmp10);
local0 = tmp12;
}while(1);
return ((int32_t)0);
}
int32_t concat_io_dfputSb (void* arg0, tuple31 arg1){
tuple31 tmp0 = arg1;
tuple11 tmp1 = concat_sb_dtoStr(tmp0);
int32_t const tmp2 = concat_io_dfputs2(arg0,tmp1);
return tmp2;
}
tuple44 concat_io_dfappendStr2 (void* arg0, int32_t arg1, tuple11 arg2){
int32_t const tmp0 = arg1;
bool const tmp1 = (tmp0!=((int32_t)0));
if(tmp1){
return (tuple44){.e0=arg0,.e1=arg1};
}
tuple11 tmp2 = arg2;
int32_t const tmp3 = concat_io_dfputs2(arg0,tmp2);
return (tuple44){.e0=arg0,.e1=tmp3};
}
tuple44 concat_io_dfappendSb (void* arg0, int32_t arg1, tuple31 arg2){
int32_t const tmp0 = arg1;
bool const tmp1 = (tmp0!=((int32_t)0));
if(tmp1){
return (tuple44){.e0=arg0,.e1=arg1};
}
tuple31 tmp2 = arg2;
tuple11 tmp3 = concat_sb_dtoStr(tmp2);
int32_t const tmp4 = concat_io_dfputs2(arg0,tmp3);
return (tuple44){.e0=arg0,.e1=tmp4};
}
tuple60 concat_sb_dappendFile (tuple31 arg0, void* arg1){
int32_t local0 = 16;
int32_t local1 = 0;
tuple31 tmp0 = arg0;
tuple31 tmp4;
tuple31 tmp1 = tmp0;
do{
int32_t const tmp2 = local1;
bool const tmp3 = (tmp2==((int32_t)0));
tmp4 = tmp1;
if(!tmp3)
  break;
int64_t const tmp5 = (tmp4).e2;
int64_t const tmp6 = ((int64_t)local0);
int64_t const tmp7 = (tmp5+tmp6);
tuple31 tmp8 = concat_sb_densureCap(tmp4,tmp7);
int8_t** const tmp9 = &((tmp8).e0);
int8_t* const tmp10 = (*(tmp9));
int64_t const tmp11 = (tmp8).e2;
tuple46 const tmp12 = concat_io_dfread(arg1,tmp10,tmp11,((int64_t)local0));
local1 = (tmp12).e1;
int64_t const tmp13 = (tmp12).e0;
int64_t* const tmp14 = &((tmp8).e2);
int64_t const tmp15 = (*(tmp14));
int64_t const tmp16 = (tmp15+tmp13);
(tmp8).e2 = tmp16;
int64_t const tmp17 = ((int64_t)local0);
bool const tmp18 = (tmp13==tmp17);
bool const tmp19 = (local0<=((int32_t)8192));
bool const tmp20 = (tmp18&tmp19);
tuple31 tmp22 = tmp8;
if(tmp20){
int32_t const tmp21 = (local0*((int32_t)2));
local0 = tmp21;
tmp22 = tmp8;
}
tmp1 = tmp22;
}while(1);
int32_t const tmp23 = local1;
bool const tmp24 = (tmp23==((int32_t)-1));
tuple31 tmp26;
int32_t tmp25;
if(tmp24){
tmp25 = ((int32_t)0);
tmp26 = tmp4;
}else{
tmp25 = local1;
tmp26 = tmp4;
}
return (tuple60){.e0=tmp26,.e1=tmp25};
}
// ./lib/stringBuilder.concat
tuple31 concat_sb_dcreate (int64_t arg0){
int8_t* const tmp0 = concat_mem_dalloc(((size_t)sizeof(int8_t)),arg0);
tuple31 tmp1 = {.e0=tmp0,.e1=arg0,.e2=((int64_t)0)};
return tmp1;
}
tuple31 concat_sb_densureCap (tuple31 arg0, int64_t arg1){
tuple31 tmp0 = arg0;
int64_t const tmp1 = (tmp0).e1;
bool const tmp2 = (tmp1>arg1);
if(tmp2){
return tmp0;
}
int64_t const tmp3 = (arg1/((int64_t)8));
int64_t const tmp4 = (arg1+tmp3);
int64_t const tmp5 = (tmp4+((int64_t)8));
int8_t** const tmp6 = &((tmp0).e0);
int8_t* const tmp7 = concat_mem_drealloc(((size_t)sizeof(int8_t)),(*(tmp6)),tmp5);
(tmp0).e0 = tmp7;
(tmp0).e1 = tmp5;
return tmp0;
}
tuple31 concat_sb_dappendC (tuple31 arg0, int8_t arg1){
tuple31 tmp0 = arg0;
int64_t const tmp1 = (tmp0).e2;
int64_t const tmp2 = (tmp1+((int64_t)1));
tuple31 tmp3 = concat_sb_densureCap(tmp0,tmp2);
int8_t** const tmp4 = &((tmp3).e0);
int64_t* const tmp5 = &((tmp3).e2);
int64_t const tmp6 = ((*(tmp5))++);
(*(((*(tmp4)))+(tmp6))) = arg1;
return tmp3;
}
tuple31 concat_sb_dappendStr (tuple31 arg0, array1 arg1){
tuple31 tmp0 = arg0;
int64_t const tmp1 = (tmp0).e2;
int64_t const tmp2 = ((arg1).sizes[0]);
int64_t const tmp3 = (tmp1+tmp2);
tuple31 tmp4 = concat_sb_densureCap(tmp0,tmp3);
int8_t** const tmp5 = &((tmp4).e0);
int8_t* const tmp6 = (*(tmp5));
int64_t const tmp7 = (tmp4).e2;
int8_t* const tmp8 = (tmp6+tmp7);
int8_t const* const tmp9 = ((int8_t const*)arg1.data);
int64_t const tmp10 = ((arg1).sizes[0]);
int8_t* const tmp11 = concat_mem_dcopy(((size_t)sizeof(int8_t)),tmp8,tmp9,tmp10);
int64_t const tmp12 = (tmp4).e2;
int64_t const tmp13 = ((arg1).sizes[0]);
int64_t const tmp14 = (tmp12+tmp13);
(tmp4).e2 = tmp14;
return tmp4;
}
tuple31 concat_sb_dappendStr2 (tuple31 arg0, tuple11 arg1){
tuple31 tmp0 = arg0;
tuple31 local0 = tmp0;
tuple11 tmp1 = arg1;
int64_t const tmp2 = (local0).e2;
int64_t const tmp3 = (tmp1).e1;
int64_t const tmp4 = (tmp2+tmp3);
tuple31 tmp5 = concat_sb_densureCap(local0,tmp4);
local0 = tmp5;
int8_t* const tmp6 = (local0).e0;
int64_t const tmp7 = (local0).e2;
int8_t* const tmp8 = (tmp6+tmp7);
tuple11 tmp9 = arg1;
tuple11 tmp10 = arg1;
int8_t const* const tmp11 = (tmp9).e0;
int64_t const tmp12 = (tmp10).e1;
int8_t* const tmp13 = concat_mem_dcopy(((size_t)sizeof(int8_t)),tmp8,tmp11,tmp12);
tuple11 tmp14 = arg1;
int64_t const tmp15 = (local0).e2;
int64_t const tmp16 = (tmp14).e1;
int64_t const tmp17 = (tmp15+tmp16);
(local0).e2 = tmp17;
return local0;
}
tuple31 concat_sb_dappendSb (tuple31 arg0, tuple31 arg1){
tuple31 tmp0 = arg0;
tuple31 local0 = tmp0;
tuple31 tmp1 = arg1;
int64_t const tmp2 = (local0).e2;
int64_t const tmp3 = (tmp1).e2;
int64_t const tmp4 = (tmp2+tmp3);
tuple31 tmp5 = concat_sb_densureCap(local0,tmp4);
local0 = tmp5;
int8_t* const tmp6 = (local0).e0;
int64_t const tmp7 = (local0).e2;
int8_t* const tmp8 = (tmp6+tmp7);
tuple31 tmp9 = arg1;
tuple31 tmp10 = arg1;
int8_t* const tmp11 = (tmp9).e0;
int64_t const tmp12 = (tmp10).e2;
int8_t* const tmp13 = concat_mem_dcopy(((size_t)sizeof(int8_t)),tmp8,((int8_t const*)tmp11),tmp12);
tuple31 tmp14 = arg1;
int64_t const tmp15 = (local0).e2;
int64_t const tmp16 = (tmp14).e2;
int64_t const tmp17 = (tmp15+tmp16);
(local0).e2 = tmp17;
return local0;
}
tuple11 concat_sb_dtoStr (tuple31 arg0){
tuple31 tmp18 = arg0;
int8_t** const tmp0 = &((tmp18).e0);
int8_t* const tmp1 = (*(tmp0));
int64_t const tmp2 = (tmp18).e2;
tuple11 tmp3 = concat_string_dcreate(((int8_t const*)tmp1),tmp2);
return tmp3;
}
tuple31 concat_sb_dappendInt_X3f_X3f (tuple31 arg0, int64_t arg1, int32_t arg2, bool arg3){
bool const tmp0 = (arg2<((int32_t)2));
if(tmp0){
tuple31 tmp1 = arg0;
return tmp1;
}
array3 tmp2 = {0};
array3 local0 = tmp2;
tuple31 tmp3 = arg0;
bool const tmp4 = (arg1<((int64_t)0));
bool const tmp5 = (tmp4&arg3);
tuple31 tmp9 = tmp3;
int64_t tmp8 = arg1;
if(tmp5){
tuple31 tmp6 = concat_sb_dappendC(tmp3,((int8_t)45));
int64_t const tmp7 = (-arg1);
tmp8 = tmp7;
tmp9 = tmp6;
}else{
bool const tmp10 = (arg1==((int64_t)0));
if(tmp10){
tuple31 tmp11 = concat_sb_dappendC(tmp3,((int8_t)48));
return tmp11;
}
}
tuple31 tmp18;
int32_t tmp17;
int64_t tmp16;
int64_t tmp12 = tmp8;
int32_t tmp13 = 63;
tuple31 tmp14 = tmp9;
do{
bool const tmp15 = (tmp12!=((int64_t)0));
tmp16 = tmp12;
tmp17 = tmp13;
tmp18 = tmp14;
if(!tmp15)
  break;
int64_t const tmp19 = ((int64_t)arg2);
int64_t const tmp20 = ((int64_t)(((uint64_t)tmp16)%((uint64_t)tmp19)));
concatInternal_checkArrayBounds(tmp20,((int64_t)61));
int8_t const tmp21 = ((((int8_t const*)(arrayData0+0)))[tmp20]);
concatInternal_checkArrayBounds(tmp17,((int64_t)64));
((local0).data[tmp17]) = tmp21;
int32_t const tmp22 = (tmp17-((int32_t)1));
int64_t const tmp23 = ((int64_t)arg2);
int64_t const tmp24 = ((int64_t)(((uint64_t)tmp16)/((uint64_t)tmp23)));
tmp12 = tmp24;
tmp13 = tmp22;
tmp14 = tmp18;
}while(1);
tuple31 tmp30;
int32_t tmp29;
int32_t tmp25 = tmp17;
tuple31 tmp26 = tmp18;
do{
int32_t const tmp27 = (tmp25+((int32_t)1));
bool const tmp28 = (tmp27<((int32_t)64));
tmp29 = tmp27;
tmp30 = tmp26;
if(!tmp28)
  break;
concatInternal_checkArrayBounds(tmp29,((int64_t)64));
int8_t const tmp31 = ((local0).data[tmp29]);
tuple31 tmp32 = concat_sb_dappendC(tmp30,tmp31);
tmp25 = tmp29;
tmp26 = tmp32;
}while(1);
return tmp30;
}
tuple31 concat_sb_dappendInt_X3f (tuple31 arg0, int64_t arg1, int32_t arg2){
tuple31 tmp33 = arg0;
tuple31 tmp0 = concat_sb_dappendInt_X3f_X3f(tmp33,arg1,arg2,((bool)1));
return tmp0;
}
tuple31 concat_sb_dappendBin (tuple31 arg0, int64_t arg1){
tuple31 tmp1 = arg0;
tuple31 tmp0 = concat_sb_dappendInt_X3f(tmp1,arg1,((int32_t)2));
return tmp0;
}
tuple31 concat_sb_dappendI (tuple31 arg0, int64_t arg1){
tuple31 tmp1 = arg0;
tuple31 tmp0 = concat_sb_dappendInt_X3f(tmp1,arg1,((int32_t)10));
return tmp0;
}
tuple31 concat_sb_dappendHex (tuple31 arg0, int64_t arg1){
tuple31 tmp1 = arg0;
tuple31 tmp0 = concat_sb_dappendInt_X3f(tmp1,arg1,((int32_t)16));
return tmp0;
}
tuple31 concat_sb_dappendUBin (tuple31 arg0, int64_t arg1){
tuple31 tmp1 = arg0;
tuple31 tmp0 = concat_sb_dappendInt_X3f_X3f(tmp1,arg1,((int32_t)2),((bool)0));
return tmp0;
}
tuple31 concat_sb_dappendU (tuple31 arg0, int64_t arg1){
tuple31 tmp1 = arg0;
tuple31 tmp0 = concat_sb_dappendInt_X3f_X3f(tmp1,arg1,((int32_t)10),((bool)0));
return tmp0;
}
tuple31 concat_sb_dappendUHex (tuple31 arg0, int64_t arg1){
tuple31 tmp1 = arg0;
tuple31 tmp0 = concat_sb_dappendInt_X3f_X3f(tmp1,arg1,((int32_t)16),((bool)0));
return tmp0;
}
// ./lib/string.concat
tuple11 concat_string_dcreate (int8_t const* arg0, int64_t arg1){
tuple11 tmp0 = {.e0=arg0,.e1=arg1};
return tmp0;
}
int32_t concat_string_dcompare (tuple11 arg0, tuple11 arg1){
tuple11 tmp0 = arg0;
tuple11 tmp1 = arg1;
int64_t const tmp2 = (tmp0).e1;
int64_t const tmp3 = (tmp1).e1;
bool const tmp4 = (tmp2<tmp3);
int64_t tmp6;
if(tmp4){
tuple11 tmp5 = arg0;
tmp6 = (tmp5).e1;
}else{
tuple11 tmp7 = arg1;
tmp6 = (tmp7).e1;
}
int64_t const local0 = tmp6;
tuple11 tmp8 = arg0;
tuple11 tmp9 = arg1;
int8_t const* const tmp10 = (tmp9).e0;
int32_t const tmp11 = concat_mem_dcompare(((size_t)sizeof(int8_t)),(tmp8).e0,tmp10,local0);
bool const tmp12 = (tmp11!=((int32_t)0));
if(tmp12){
return tmp11;
}else{
}
tuple11 tmp13 = arg0;
tuple11 tmp14 = arg1;
int64_t const tmp15 = (tmp13).e1;
int64_t const tmp16 = (tmp14).e1;
int64_t const tmp17 = (tmp15-tmp16);
bool const tmp18 = (tmp17>((int64_t)0));
if(tmp18){
return ((int32_t)1);
}else{
bool const tmp19 = (tmp17<((int64_t)0));
if(tmp19){
return ((int32_t)-1);
}else{
return ((int32_t)0);
}
}
}
bool concat_string_deq (tuple11 arg0, tuple11 arg1){
tuple11 tmp20 = arg0;
tuple11 tmp21 = arg1;
int32_t const tmp0 = concat_string_dcompare(tmp20,tmp21);
bool const tmp1 = (tmp0==((int32_t)0));
return tmp1;
}
int32_t concat__Flib_X2fstring_dconcat_Estring_dhash (tuple11 arg0){
int32_t tmp7;
int32_t tmp6;
int32_t tmp0 = 0;
int32_t tmp1 = 0;
do{
tuple11 tmp2 = arg0;
int64_t const tmp3 = ((int64_t)tmp0);
int64_t const tmp4 = (tmp2).e1;
bool const tmp5 = (tmp3<tmp4);
tmp6 = tmp0;
tmp7 = tmp1;
if(!tmp5)
  break;
int32_t const tmp8 = (tmp7*((int32_t)31));
tuple11 tmp9 = arg0;
int8_t const* const tmp10 = (tmp9).e0;
int32_t const tmp11 = ((int32_t)(*((tmp10)+(tmp6))));
int32_t const tmp12 = (tmp8+tmp11);
int32_t const tmp13 = (tmp6+((int32_t)1));
tmp0 = tmp13;
tmp1 = tmp12;
}while(1);
return tmp7;
}
int8_t concat__Flib_X2fstring_dconcat_Estring_dcharAt (tuple11 arg0, int64_t arg1){
bool const tmp0 = (arg1<((int64_t)0));
tuple11 tmp1 = arg0;
int64_t const tmp2 = (tmp1).e1;
bool const tmp3 = (arg1>=tmp2);
bool const tmp4 = (tmp0|tmp3);
if(tmp4){
return ((int8_t)0);
}
tuple11 tmp5 = arg0;
int8_t const* const tmp6 = (tmp5).e0;
int8_t const tmp7 = (*((tmp6)+(arg1)));
return tmp7;
}
int64_t concat_string_dindexOf (tuple11 arg0, int8_t arg1){
int32_t tmp5;
int32_t tmp0 = 0;
do{
tuple11 tmp1 = arg0;
int64_t const tmp2 = ((int64_t)tmp0);
int64_t const tmp3 = (tmp1).e1;
bool const tmp4 = (tmp2<tmp3);
tmp5 = tmp0;
if(!tmp4)
  break;
tuple11 tmp6 = arg0;
int8_t const* const tmp7 = (tmp6).e0;
int8_t const tmp8 = (*((tmp7)+(tmp5)));
bool const tmp9 = (tmp8==arg1);
if(tmp9){
int64_t const tmp10 = ((int64_t)tmp5);
return tmp10;
}
int32_t const tmp11 = (tmp5+((int32_t)1));
tmp0 = tmp11;
}while(1);
return ((int64_t)-1);
}
int64_t concat_string_dlastIndexOf (tuple11 arg0, int8_t arg1){
tuple11 tmp0 = arg0;
int64_t const tmp1 = (tmp0).e1;
int64_t const tmp2 = (tmp1-((int64_t)1));
int64_t tmp5;
int64_t tmp3 = tmp2;
do{
bool const tmp4 = (tmp3>=((int64_t)0));
tmp5 = tmp3;
if(!tmp4)
  break;
tuple11 tmp6 = arg0;
int8_t const* const tmp7 = (tmp6).e0;
int8_t const tmp8 = (*((tmp7)+(tmp5)));
bool const tmp9 = (tmp8==arg1);
if(tmp9){
return tmp5;
}
int64_t const tmp10 = (tmp5-((int64_t)1));
tmp3 = tmp10;
}while(1);
return tmp5;
}
int64_t concat_string_dindexOfStr (tuple11 arg0, tuple11 arg1){
tuple11 tmp0 = arg1;
tuple11 tmp1 = arg0;
int64_t const tmp2 = (tmp0).e1;
int64_t const tmp3 = (tmp1).e1;
bool const tmp4 = (tmp2>tmp3);
if(tmp4){
return ((int64_t)-1);
}
tuple11 tmp5 = arg0;
tuple11 tmp6 = arg1;
int64_t const tmp7 = (tmp5).e1;
int64_t const tmp8 = (tmp6).e1;
int64_t const tmp9 = (tmp7-tmp8);
int64_t const local0 = tmp9;
int32_t tmp13;
int32_t tmp10 = 0;
do{
int64_t const tmp11 = ((int64_t)tmp10);
bool const tmp12 = (tmp11<=local0);
tmp13 = tmp10;
if(!tmp12)
  break;
tuple11 tmp14 = arg0;
int8_t const* const tmp15 = (tmp14).e0;
int8_t const* const tmp16 = (tmp15+tmp13);
tuple11 tmp17 = arg1;
int64_t const tmp18 = (tmp17).e1;
tuple11 tmp19 = concat_string_dcreate(tmp16,tmp18);
tuple11 tmp20 = arg1;
int32_t const tmp21 = concat_string_dcompare(tmp19,tmp20);
bool const tmp22 = (tmp21==((int32_t)0));
if(tmp22){
int64_t const tmp23 = ((int64_t)tmp13);
return tmp23;
}
int32_t const tmp24 = (tmp13+((int32_t)1));
tmp10 = tmp24;
}while(1);
return ((int64_t)-1);
}
int64_t concat_string_dlastIndexOfStr (tuple11 arg0, tuple11 arg1){
tuple11 tmp0 = arg1;
tuple11 tmp1 = arg0;
int64_t const tmp2 = (tmp0).e1;
int64_t const tmp3 = (tmp1).e1;
bool const tmp4 = (tmp2>tmp3);
if(tmp4){
return ((int64_t)-1);
}
tuple11 tmp5 = arg0;
tuple11 tmp6 = arg1;
int64_t const tmp7 = (tmp5).e1;
int64_t const tmp8 = (tmp6).e1;
int64_t const tmp9 = (tmp7-tmp8);
int64_t tmp12;
int64_t tmp10 = tmp9;
do{
bool const tmp11 = (tmp10>=((int64_t)0));
tmp12 = tmp10;
if(!tmp11)
  break;
tuple11 tmp13 = arg0;
int8_t const* const tmp14 = (tmp13).e0;
int8_t const* const tmp15 = (tmp14+tmp12);
tuple11 tmp16 = arg1;
int64_t const tmp17 = (tmp16).e1;
tuple11 tmp18 = concat_string_dcreate(tmp15,tmp17);
tuple11 tmp19 = arg1;
int32_t const tmp20 = concat_string_dcompare(tmp18,tmp19);
bool const tmp21 = (tmp20==((int32_t)0));
if(tmp21){
return tmp12;
}
int64_t const tmp22 = (tmp12-((int64_t)1));
tmp10 = tmp22;
}while(1);
return tmp12;
}
bool concat_string_dcontains (tuple11 arg0, int8_t arg1){
tuple11 tmp0 = arg0;
int64_t const tmp1 = concat_string_dindexOf(tmp0,arg1);
bool const tmp2 = (tmp1!=((int64_t)-1));
return tmp2;
}
bool concat_string_disSubstring (tuple11 arg0, tuple11 arg1){
tuple11 tmp0 = arg0;
tuple11 tmp1 = arg1;
int64_t const tmp2 = concat_string_dindexOfStr(tmp0,tmp1);
bool const tmp3 = (tmp2!=((int64_t)-1));
return tmp3;
}
tuple11 concat_string_dhead (tuple11 arg0, int64_t arg1){
bool const tmp0 = (arg1<=((int64_t)0));
if(tmp0){
tuple11 tmp1 = concat__Flib_X2fstring_dconcat_Estring_dEMPTY;
return tmp1;
}
tuple11 tmp2 = arg0;
int64_t const tmp3 = (tmp2).e1;
bool const tmp4 = (arg1>=tmp3);
if(tmp4){
tuple11 tmp5 = arg0;
return tmp5;
}
tuple11 tmp6 = arg0;
int8_t const* const tmp7 = (tmp6).e0;
tuple11 tmp8 = concat_string_dcreate(tmp7,arg1);
return tmp8;
}
tuple11 concat_string_dtail (tuple11 arg0, int64_t arg1){
bool const tmp0 = (arg1<=((int64_t)0));
if(tmp0){
tuple11 tmp1 = arg0;
return tmp1;
}
tuple11 tmp2 = arg0;
int64_t const tmp3 = (tmp2).e1;
bool const tmp4 = (arg1>=tmp3);
if(tmp4){
tuple11 tmp5 = concat__Flib_X2fstring_dconcat_Estring_dEMPTY;
return tmp5;
}
tuple11 tmp6 = arg0;
int8_t const* const tmp7 = (tmp6).e0;
int8_t const* const tmp8 = (tmp7+arg1);
tuple11 tmp9 = arg0;
int64_t const tmp10 = (tmp9).e1;
int64_t const tmp11 = (tmp10-arg1);
tuple11 tmp12 = concat_string_dcreate(tmp8,tmp11);
return tmp12;
}
tuple11 concat_string_dslice (tuple11 arg0, int64_t arg1, int64_t arg2){
bool const tmp0 = (arg2<=((int64_t)0));
if(tmp0){
tuple11 tmp1 = concat__Flib_X2fstring_dconcat_Estring_dEMPTY;
return tmp1;
}
tuple11 tmp2 = arg0;
int64_t const tmp3 = (tmp2).e1;
bool const tmp4 = (arg1>=tmp3);
if(tmp4){
tuple11 tmp5 = concat__Flib_X2fstring_dconcat_Estring_dEMPTY;
return tmp5;
}
bool const tmp6 = (arg1<=((int64_t)0));
if(tmp6){
tuple11 tmp7 = arg0;
int8_t const* const tmp8 = (tmp7).e0;
tuple11 tmp9 = concat_string_dcreate(tmp8,arg2);
return tmp9;
}
int64_t const tmp10 = (arg1+arg2);
tuple11 tmp11 = arg0;
int64_t const tmp12 = (tmp11).e1;
bool const tmp13 = (tmp10>=tmp12);
if(tmp13){
tuple11 tmp14 = arg0;
int8_t const* const tmp15 = (tmp14).e0;
int8_t const* const tmp16 = (tmp15+arg1);
tuple11 tmp17 = arg0;
int64_t const tmp18 = (tmp17).e1;
int64_t const tmp19 = (tmp18-arg1);
tuple11 tmp20 = concat_string_dcreate(tmp16,tmp19);
return tmp20;
}
tuple11 tmp21 = arg0;
int8_t const* const tmp22 = (tmp21).e0;
int8_t const* const tmp23 = (tmp22+arg1);
tuple11 tmp24 = concat_string_dcreate(tmp23,arg2);
return tmp24;
}
tuple15 concat_string_dsplit (tuple11 arg0, int8_t arg1){
tuple11 tmp0 = arg0;
int64_t const tmp1 = concat_string_dindexOf(tmp0,arg1);
int64_t const local0 = tmp1;
bool const tmp2 = (local0==((int64_t)-1));
if(tmp2){
tuple11 tmp3 = concat__Flib_X2fstring_dconcat_Estring_dEMPTY;
tuple11 tmp4 = arg0;
return (tuple15){.e0=tmp3,.e1=tmp4};
}
tuple11 tmp5 = arg0;
int64_t const tmp6 = (local0+((int64_t)1));
tuple11 tmp7 = concat_string_dtail(tmp5,tmp6);
tuple11 tmp8 = arg0;
tuple11 tmp9 = concat_string_dhead(tmp8,local0);
return (tuple15){.e0=tmp7,.e1=tmp9};
}
// ./lib/io.concat
int32_t concat_io_dfputc (void* arg0, int8_t arg1){
int8_t const* const tmp0 = &(arg1);
tuple46 const tmp1 = concat_io_dfwrite(arg0,((int8_t const*)tmp0),((int64_t)0),((int64_t)1));
int64_t const tmp2 = (tmp1).e0;
int32_t const tmp3 = (tmp1).e1;
return tmp3;
}
int32_t concat_io_dfputs (void* arg0, array1 arg1){
int64_t local0 = 0;
int64_t const tmp0 = ((arg1).sizes[0]);
int64_t local1 = tmp0;
int8_t const* const tmp1 = ((int8_t const*)arg1.data);
int8_t const* const local2 = tmp1;
do{
bool const tmp2 = (local1>((int64_t)0));
if(!tmp2)
  break;
tuple46 const tmp3 = concat_io_dfwrite(arg0,local2,local0,local1);
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
tuple44 concat_io_dfstart (void* arg0){
return (tuple44){.e0=arg0,.e1=((int32_t)0)};
}
tuple44 concat_io_dfappendStr (void* arg0, int32_t arg1, array1 arg2){
int32_t const tmp0 = arg1;
bool const tmp1 = (tmp0!=((int32_t)0));
if(tmp1){
return (tuple44){.e0=arg0,.e1=arg1};
}
int32_t const tmp2 = concat_io_dfputs(arg0,arg2);
return (tuple44){.e0=arg0,.e1=tmp2};
}
tuple44 concat_io_dfappendC (void* arg0, int32_t arg1, int8_t arg2){
int32_t const tmp0 = arg1;
bool const tmp1 = (tmp0!=((int32_t)0));
if(tmp1){
return (tuple44){.e0=arg0,.e1=arg1};
}
int32_t const tmp2 = concat_io_dfputc(arg0,arg2);
return (tuple44){.e0=arg0,.e1=tmp2};
}
int32_t concat_io_dputs (array1 arg0){
void* const tmp0 = concat_io_dstdOut();
tuple44 const tmp1 = concat_io_dfstart(tmp0);
void* const tmp2 = (tmp1).e0;
int32_t const tmp3 = (tmp1).e1;
tuple44 const tmp4 = concat_io_dfappendStr(tmp2,tmp3,arg0);
void* const tmp5 = (tmp4).e0;
int32_t const tmp6 = (tmp4).e1;
tuple44 const tmp7 = concat_io_dfappendStr(tmp5,tmp6,((array1){.data=(arrayData2+0),.sizes={1}}));
void* const tmp8 = (tmp7).e0;
int32_t const tmp9 = (tmp7).e1;
return tmp9;
}
