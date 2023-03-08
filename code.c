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
tuple22 concat_std_dnewStringBuffer (int64_t);
tuple22 concat_std_dsb__appendBin (tuple22, int64_t);
tuple22 concat_std_dstringBufferEnsureCap (tuple22, int64_t);
int8_t const* const concat__Flib_X2fstring_dconcat_Estd_dstringChars;
tuple22 concat_std_dsb__appendC (tuple22, int8_t);
tuple22 concat_std_dsb__appendI (tuple22, int64_t);
tuple22 concat_std_dsb__appendStr (tuple22, array2);
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
tuple22 const tmp19 = concat_std_dsb__appendBin(tmp18,((int64_t)9223372036854775807));
tuple22 const tmp20 = concat_std_dsb__appendC(tmp19,((int8_t)10));
tuple22 const tmp21 = concat_std_dsb__appendHex(tmp20,((int64_t)9223372036854775807));
tuple22 const tmp22 = concat_std_dsb__appendC(tmp21,((int8_t)10));
tuple22 const tmp23 = concat_std_dsb__appendI(tmp22,((int64_t)9223372036854775807));
tuple22 const tmp24 = concat_std_dsb__appendC(tmp23,((int8_t)10));
tuple22 const tmp25 = concat_std_dsb__appendBin(tmp24,((int64_t)-9223372036854775807 -1));
tuple22 const tmp26 = concat_std_dsb__appendC(tmp25,((int8_t)10));
tuple22 const tmp27 = concat_std_dsb__appendHex(tmp26,((int64_t)-9223372036854775807 -1));
tuple22 const tmp28 = concat_std_dsb__appendC(tmp27,((int8_t)10));
tuple22 const tmp29 = concat_std_dsb__appendI(tmp28,((int64_t)-9223372036854775807 -1));
tuple22 const tmp30 = concat_std_dsb__appendC(tmp29,((int8_t)10));
tuple22 const local1 = tmp30;
void* const tmp31 = concat_io_dstdOut();
int8_t* const tmp32 = (local1).e0;
int64_t const tmp33 = (local1).e2;
tuple5 const tmp34 = concat_io_dfwrite(tmp31,((int8_t const*)tmp32),((int64_t)0),tmp33);
int32_t const tmp35 = (tmp34).e1;
printf("%"PRIi32"\n",tmp35);
int64_t const tmp36 = (tmp34).e0;
printf("%"PRIi64"\n",tmp36);
int32_t const tmp37 = ((int32_t)(((uint32_t)((int32_t)-1))/((uint32_t)((int32_t)2))));
printf("%"PRIi32"\n",tmp37);
}
int32_t concat__Fcode_dconcat_Etest (void){
return ((int32_t)9);
}
// ./lib/io.concat
int32_t concat_io_dfputc (void* arg0, int8_t arg1){
int8_t const* const tmp38 = &(arg1);
tuple5 const tmp39 = concat_io_dfwrite(arg0,((int8_t const*)tmp38),((int64_t)0),((int64_t)1));
int64_t const tmp40 = (tmp39).e0;
int32_t const tmp41 = (tmp39).e1;
return tmp41;
}
int32_t concat_io_dfputs (void* arg0, array2 arg1){
int64_t local0 = 0;
int64_t const tmp42 = ((arg1).sizes[0]);
int64_t local1 = tmp42;
int8_t const* const tmp43 = ((int8_t const*)arg1.data);
int8_t const* const local2 = tmp43;
do{
bool const tmp44 = (local1>((int64_t)0));
if(!tmp44)
  break;
tuple5 const tmp45 = concat_io_dfwrite(arg0,local2,local0,local1);
int32_t const tmp46 = (tmp45).e1;
int32_t const tmp47 = tmp46;
bool const tmp48 = (tmp47!=((int32_t)0));
if(tmp48){
int64_t const tmp49 = (tmp45).e0;
return tmp46;
}
int64_t const tmp50 = (tmp45).e0;
int64_t const tmp51 = (local1-tmp50);
local1 = tmp51;
int64_t const tmp52 = (local0+tmp50);
local0 = tmp52;
}while(1);
return ((int32_t)0);
}
tuple3 concat_io_dfstart (void* arg0){
return (tuple3){.e0=arg0,.e1=((int32_t)0)};
}
tuple3 concat_io_dfappendStr (void* arg0, int32_t arg1, array2 arg2){
int32_t const tmp53 = arg1;
bool const tmp54 = (tmp53!=((int32_t)0));
if(tmp54){
return (tuple3){.e0=arg0,.e1=arg1};
}
int32_t const tmp55 = concat_io_dfputs(arg0,arg2);
return (tuple3){.e0=arg0,.e1=tmp55};
}
tuple3 concat_io_dfappendC (void* arg0, int32_t arg1, int8_t arg2){
int32_t const tmp56 = arg1;
bool const tmp57 = (tmp56!=((int32_t)0));
if(tmp57){
return (tuple3){.e0=arg0,.e1=arg1};
}
int32_t const tmp58 = concat_io_dfputc(arg0,arg2);
return (tuple3){.e0=arg0,.e1=tmp58};
}
int32_t concat_io_dputs (array2 arg0){
void* const tmp59 = concat_io_dstdOut();
tuple3 const tmp60 = concat_io_dfstart(tmp59);
void* const tmp61 = (tmp60).e0;
int32_t const tmp62 = (tmp60).e1;
tuple3 const tmp63 = concat_io_dfappendStr(tmp61,tmp62,arg0);
void* const tmp64 = (tmp63).e0;
int32_t const tmp65 = (tmp63).e1;
tuple3 const tmp66 = concat_io_dfappendStr(tmp64,tmp65,((array2){.data=(arrayData4+0),.sizes={1}}));
void* const tmp67 = (tmp66).e0;
int32_t const tmp68 = (tmp66).e1;
return tmp68;
}
// ./lib/string.concat
tuple22 concat_std_dnewStringBuffer (int64_t arg0){
int8_t* const tmp69 = concat_mem_dalloc(((size_t)sizeof(int8_t)),arg0);
tuple22 const tmp70 = {.e0=tmp69,.e1=arg0,.e2=((int64_t)0)};
return tmp70;
}
tuple22 concat_std_dstringBufferEnsureCap (tuple22 arg0, int64_t arg1){
tuple22 local0 = arg0;
int64_t const tmp71 = (local0).e1;
bool const tmp72 = (tmp71>arg1);
if(tmp72){
return local0;
}
int64_t const tmp73 = (arg1/((int64_t)8));
int64_t const tmp74 = (arg1+tmp73);
int64_t const tmp75 = (tmp74+((int64_t)8));
int8_t** const tmp76 = &((local0).e0);
int8_t* const tmp77 = concat_mem_drealloc(((size_t)sizeof(int8_t)),(*(tmp76)),tmp75);
(local0).e0 = tmp77;
(local0).e1 = tmp75;
return local0;
}
tuple22 concat_std_dsb__appendC (tuple22 arg0, int8_t arg1){
tuple22 local0 = arg0;
int64_t const tmp78 = (local0).e2;
int64_t const tmp79 = (tmp78+((int64_t)1));
tuple22 const tmp80 = concat_std_dstringBufferEnsureCap(local0,tmp79);
local0 = tmp80;
int64_t* const tmp81 = &((local0).e2);
int64_t const tmp82 = ((*(tmp81))++);
int8_t** const tmp83 = &((local0).e0);
(*(((*(tmp83)))+(tmp82))) = arg1;
return local0;
}
tuple22 concat_std_dsb__appendStr (tuple22 arg0, array2 arg1){
tuple22 local0 = arg0;
int64_t const tmp84 = (local0).e2;
int64_t const tmp85 = ((arg1).sizes[0]);
int64_t const tmp86 = (tmp84+tmp85);
tuple22 const tmp87 = concat_std_dstringBufferEnsureCap(local0,tmp86);
local0 = tmp87;
int8_t* const tmp88 = (local0).e0;
int64_t const tmp89 = (local0).e2;
int8_t* const tmp90 = (tmp88+tmp89);
int8_t const* const tmp91 = ((int8_t const*)arg1.data);
int64_t const tmp92 = ((arg1).sizes[0]);
int8_t* const tmp93 = concat_mem_dcopy(((size_t)sizeof(int8_t)),tmp90,tmp91,tmp92);
int64_t const tmp94 = (local0).e2;
int64_t const tmp95 = ((arg1).sizes[0]);
int64_t const tmp96 = (tmp94+tmp95);
(local0).e2 = tmp96;
return local0;
}
tuple22 concat_std_dsb__appendInt_X3f (tuple22 arg0, int64_t arg1, int32_t arg2){
bool const tmp97 = (arg2<((int32_t)2));
if(tmp97){
return arg0;
}
array5 local0 = {0};
bool const tmp98 = (arg1<((int64_t)0));
tuple22 tmp102 = arg0;
int64_t tmp101 = arg1;
if(tmp98){
tuple22 const tmp99 = concat_std_dsb__appendC(arg0,((int8_t)45));
int64_t const tmp100 = (-arg1);
tmp101 = tmp100;
tmp102 = tmp99;
}else{
bool const tmp103 = (arg1==((int64_t)0));
if(tmp103){
tuple22 const tmp104 = concat_std_dsb__appendC(arg0,((int8_t)48));
return tmp104;
}
}
tuple22 tmp111;
int32_t tmp110;
int64_t tmp109;
int64_t tmp105 = tmp101;
int32_t tmp106 = 63;
tuple22 tmp107 = tmp102;
do{
bool const tmp108 = (tmp105!=((int64_t)0));
tmp109 = tmp105;
tmp110 = tmp106;
tmp111 = tmp107;
if(!tmp108)
  break;
int64_t const tmp112 = ((int64_t)arg2);
int64_t const tmp113 = (tmp109)%((uint64_t)tmp112);
concatInternal_checkArrayBounds(tmp113,((int64_t)61));
int8_t const tmp114 = ((((int8_t const*)(arrayData0+0)))[tmp113]);
concatInternal_checkArrayBounds(tmp110,((int64_t)64));
((local0).data[tmp110]) = tmp114;
int32_t const tmp115 = (tmp110-((int32_t)1));
int64_t const tmp116 = ((int64_t)arg2);
int64_t const tmp117 = ((int64_t)(((uint64_t)tmp109)/((uint64_t)tmp116)));
tmp105 = tmp117;
tmp106 = tmp115;
tmp107 = tmp111;
}while(1);
tuple22 tmp123;
int32_t tmp122;
int32_t tmp118 = tmp110;
tuple22 tmp119 = tmp111;
do{
int32_t const tmp120 = (tmp118+((int32_t)1));
bool const tmp121 = (tmp120<((int32_t)64));
tmp122 = tmp120;
tmp123 = tmp119;
if(!tmp121)
  break;
concatInternal_checkArrayBounds(tmp122,((int64_t)64));
int8_t const tmp124 = ((local0).data[tmp122]);
tuple22 const tmp125 = concat_std_dsb__appendC(tmp123,tmp124);
tmp118 = tmp122;
tmp119 = tmp125;
}while(1);
return tmp123;
}
tuple22 concat_std_dsb__appendBin (tuple22 arg0, int64_t arg1){
tuple22 const tmp126 = concat_std_dsb__appendInt_X3f(arg0,arg1,((int32_t)2));
return tmp126;
}
tuple22 concat_std_dsb__appendI (tuple22 arg0, int64_t arg1){
tuple22 const tmp127 = concat_std_dsb__appendInt_X3f(arg0,arg1,((int32_t)10));
return tmp127;
}
tuple22 concat_std_dsb__appendHex (tuple22 arg0, int64_t arg1){
tuple22 const tmp128 = concat_std_dsb__appendInt_X3f(arg0,arg1,((int32_t)16));
return tmp128;
}
