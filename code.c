#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple3Impl tuple3;
typedef struct tuple5Impl tuple5;
typedef struct tuple23Impl tuple23;
typedef struct array2Impl array2;
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
struct tuple23Impl{
int8_t* e0;
int64_t e1;
int64_t e2;
};
const int8_t arrayData0[13] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64,0x21,0x00};
const int8_t arrayData1[2] = {0xa,0x00};
//global declarations
// ./code.concat
tuple23 concat__Fcode_dconcat_Esb_X3c_X3cc (tuple23, int8_t);
tuple23 concat__Fcode_dconcat_Esb_X3c_X3cs (tuple23, array2);
int32_t concat__Fcode_dconcat_Etest (void);
tuple23 concat__Fcode_dconcat_EnewStringBuffer (int64_t);
tuple23 concat__Fcode_dconcat_EstringBufferEnsureCap (tuple23, int64_t);
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
//global code
// ./lib/io.concat
int32_t const concat_io_dERROR__NONE = 0;
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
int8_t const* const tmp5 = (arrayData0+0);
int32_t const tmp6 = concat_io_dputs(((array2){.data=tmp5,.sizes={12}}));
printf("%"PRIi32"\n",tmp6);
tuple23 const tmp7 = concat__Fcode_dconcat_EnewStringBuffer(((int64_t)16));
int8_t const* const tmp8 = (arrayData0+0);
tuple23 const tmp9 = concat__Fcode_dconcat_Esb_X3c_X3cs(tmp7,((array2){.data=tmp8,.sizes={5}}));
tuple23 const tmp10 = concat__Fcode_dconcat_Esb_X3c_X3cc(tmp9,((int8_t)32));
int8_t const* const tmp11 = (arrayData0+6);
tuple23 const tmp12 = concat__Fcode_dconcat_Esb_X3c_X3cs(tmp10,((array2){.data=tmp11,.sizes={5}}));
tuple23 const tmp13 = concat__Fcode_dconcat_Esb_X3c_X3cc(tmp12,((int8_t)10));
tuple23 const local1 = tmp13;
void* const tmp14 = concat_io_dstdOut();
int8_t* const tmp15 = (local1).e0;
int64_t const tmp16 = (local1).e2;
tuple5 const tmp17 = concat_io_dfwrite(tmp14,((int8_t const*)tmp15),((int64_t)0),tmp16);
int32_t const tmp18 = (tmp17).e1;
printf("%"PRIi32"\n",tmp18);
int64_t const tmp19 = (tmp17).e0;
printf("%"PRIi64"\n",tmp19);
}
int32_t concat__Fcode_dconcat_Etest (void){
return ((int32_t)9);
}
tuple23 concat__Fcode_dconcat_EnewStringBuffer (int64_t arg0){
int8_t* const tmp20 = concat_mem_dalloc(((size_t)sizeof(int8_t)),arg0);
tuple23 const tmp21 = {.e0=tmp20,.e1=arg0,.e2=((int64_t)0)};
return tmp21;
}
tuple23 concat__Fcode_dconcat_EstringBufferEnsureCap (tuple23 arg0, int64_t arg1){
tuple23 local0 = arg0;
int64_t const tmp22 = (local0).e1;
bool const tmp23 = (tmp22>arg1);
if(tmp23){
return local0;
}
int64_t const tmp24 = (arg1/((int64_t)8));
int64_t const tmp25 = (arg1+tmp24);
int64_t const tmp26 = (tmp25+((int64_t)8));
int8_t** const tmp27 = &((local0).e0);
int8_t* const tmp28 = concat_mem_drealloc(((size_t)sizeof(int8_t)),(*(tmp27)),tmp26);
(local0).e0 = tmp28;
(local0).e1 = tmp26;
return local0;
}
tuple23 concat__Fcode_dconcat_Esb_X3c_X3cc (tuple23 arg0, int8_t arg1){
tuple23 local0 = arg0;
int64_t const tmp29 = (local0).e2;
int64_t const tmp30 = (tmp29+((int64_t)1));
tuple23 const tmp31 = concat__Fcode_dconcat_EstringBufferEnsureCap(local0,tmp30);
local0 = tmp31;
int64_t* const tmp32 = &((local0).e2);
int64_t const tmp33 = ((*(tmp32))++);
int8_t** const tmp34 = &((local0).e0);
(*(((*(tmp34)))+(tmp33))) = arg1;
return local0;
}
tuple23 concat__Fcode_dconcat_Esb_X3c_X3cs (tuple23 arg0, array2 arg1){
tuple23 local0 = arg0;
int64_t const tmp35 = (local0).e2;
int64_t const tmp36 = ((arg1).sizes[0]);
int64_t const tmp37 = (tmp35+tmp36);
tuple23 const tmp38 = concat__Fcode_dconcat_EstringBufferEnsureCap(local0,tmp37);
local0 = tmp38;
int8_t* const tmp39 = (local0).e0;
int64_t const tmp40 = (local0).e2;
int8_t* const tmp41 = (tmp39+tmp40);
int8_t const* const tmp42 = ((int8_t const*)arg1.data);
int64_t const tmp43 = ((arg1).sizes[0]);
int8_t* const tmp44 = concat_mem_dcopy(((size_t)sizeof(int8_t)),tmp41,tmp42,tmp43);
int64_t const tmp45 = (local0).e2;
int64_t const tmp46 = ((arg1).sizes[0]);
int64_t const tmp47 = (tmp45+tmp46);
(local0).e2 = tmp47;
return local0;
}
// ./lib/io.concat
int32_t concat_io_dfputc (void* arg0, int8_t arg1){
int8_t const* const tmp48 = &(arg1);
tuple5 const tmp49 = concat_io_dfwrite(arg0,((int8_t const*)tmp48),((int64_t)0),((int64_t)1));
int64_t const tmp50 = (tmp49).e0;
int32_t const tmp51 = (tmp49).e1;
return tmp51;
}
int32_t concat_io_dfputs (void* arg0, array2 arg1){
int64_t local0 = 0;
int64_t const tmp52 = ((arg1).sizes[0]);
int64_t local1 = tmp52;
int8_t const* const tmp53 = ((int8_t const*)arg1.data);
int8_t const* const local2 = tmp53;
do{
bool const tmp54 = (local1>((int64_t)0));
if(!tmp54)
  break;
tuple5 const tmp55 = concat_io_dfwrite(arg0,local2,local0,local1);
int32_t const tmp56 = (tmp55).e1;
int32_t const tmp57 = tmp56;
bool const tmp58 = (tmp57!=((int32_t)0));
if(tmp58){
int64_t const tmp59 = (tmp55).e0;
return tmp56;
}
int64_t const tmp60 = (tmp55).e0;
int64_t const tmp61 = (local1-tmp60);
local1 = tmp61;
int64_t const tmp62 = (local0+tmp60);
local0 = tmp62;
}while(1);
return ((int32_t)0);
}
tuple3 concat_io_dfstart (void* arg0){
return (tuple3){.e0=arg0,.e1=((int32_t)0)};
}
tuple3 concat_io_dfappendStr (void* arg0, int32_t arg1, array2 arg2){
int32_t const tmp63 = arg1;
bool const tmp64 = (tmp63!=((int32_t)0));
if(tmp64){
return (tuple3){.e0=arg0,.e1=arg1};
}
int32_t const tmp65 = concat_io_dfputs(arg0,arg2);
return (tuple3){.e0=arg0,.e1=tmp65};
}
tuple3 concat_io_dfappendC (void* arg0, int32_t arg1, int8_t arg2){
int32_t const tmp66 = arg1;
bool const tmp67 = (tmp66!=((int32_t)0));
if(tmp67){
return (tuple3){.e0=arg0,.e1=arg1};
}
int32_t const tmp68 = concat_io_dfputc(arg0,arg2);
return (tuple3){.e0=arg0,.e1=tmp68};
}
int32_t concat_io_dputs (array2 arg0){
void* const tmp69 = concat_io_dstdOut();
tuple3 const tmp70 = concat_io_dfstart(tmp69);
void* const tmp71 = (tmp70).e0;
int32_t const tmp72 = (tmp70).e1;
tuple3 const tmp73 = concat_io_dfappendStr(tmp71,tmp72,arg0);
void* const tmp74 = (tmp73).e0;
int32_t const tmp75 = (tmp73).e1;
int8_t const* const tmp76 = (arrayData1+0);
tuple3 const tmp77 = concat_io_dfappendStr(tmp74,tmp75,((array2){.data=tmp76,.sizes={1}}));
void* const tmp78 = (tmp77).e0;
int32_t const tmp79 = (tmp77).e1;
return tmp79;
}
