#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple1Impl tuple1;
typedef struct tuple5Impl tuple5;
typedef struct tuple6Impl tuple6;
typedef struct tuple8Impl tuple8;
typedef struct tuple9Impl tuple9;
typedef struct tuple10Impl tuple10;
typedef struct enum11Impl enum11;
typedef int32_t enum12;
typedef struct enum13Impl enum13;
typedef struct tuple15Impl tuple15;
typedef struct tuple16Impl tuple16;
typedef struct tuple19Impl tuple19;
typedef struct tuple21Impl tuple21;
typedef struct tuple22Impl tuple22;
typedef struct array0Impl array0;
typedef struct array1Impl array1;
typedef struct array2Impl array2;
typedef struct array3Impl array3;
typedef struct array4Impl array4;
typedef struct array5Impl array5;
typedef struct array7Impl array7;
typedef struct array8Impl array8;
typedef struct array9Impl array9;
typedef tuple1 (*procPtr10) (int32_t, int32_t);
struct tuple1Impl{
int32_t e0;
int32_t e1;
};
struct tuple5Impl{
int32_t e0;
int32_t const* e1;
};
struct tuple6Impl{
tuple5 const* e0;
int32_t e1;
};
struct tuple8Impl{
int32_t e0;
tuple8 const* e1;
};
struct tuple9Impl{
int32_t e0;
int8_t e1;
};
struct tuple10Impl{
int32_t e0;
int8_t e1;
};
struct enum11Impl{
union{
int32_t e2;
tuple10 e3;
} data;
int32_t label;
};
struct enum13Impl{
union{
int64_t const* e1;
} data;
int32_t label;
};
struct array0Impl{
int8_t const* data;
int64_t sizes[1];
};
struct tuple15Impl{
array0 e0;
int64_t e1;
};
struct array1Impl{
int32_t data[3];
};
struct tuple16Impl{
int32_t e0;
int64_t e1;
};
struct array2Impl{
tuple16 data[2];
};
struct array3Impl{
int64_t data[3][2];
};
int64_t const arraySizes3[2]={2,3};
struct array4Impl{
int64_t data[10][8][6][4][2];
};
struct array5Impl{
int32_t const* data[2];
};
struct array7Impl{
int32_t const* data;
int64_t sizes[1];
};
struct array8Impl{
array7 const* data;
int64_t sizes[1];
};
struct tuple19Impl{
array0 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct array9Impl{
int32_t data[3][2];
};
struct tuple21Impl{
int64_t e0;
int64_t e1;
};
struct tuple22Impl{
tuple1 e0;
int32_t e1;
};
const int8_t arrayData0[40] = {0x54,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x75,0x6e,0x69,0x63,0x6f,0x64,0x65,-0x1e,-0x7c,-0x5e,0x20,0x6d,0x75,0x6c,0x74,0x69,0x2d,0x6c,0x69,0x6e,0x65,0xa,0x20,0x73,0x74,0x72,0x69,0x6e,0x67,0x00};
const int8_t arrayData1[24] = {0x53,0x74,0x72,0x69,0x6e,0x67,0x73,0x20,0x61,0x72,0x65,0x20,0x22,0x69,0x67,0x6e,0x6f,0x72,0x65,0x64,0x22,0x20,0x20,0x00};
const int8_t arrayData2[13] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64,0x21,0x00};
void concatInternal_checkArrayBounds(int64_t index,int64_t length){
  if(index>=0 && index<length)
    return;
  fprintf(stderr,"array index out of bounds: %"PRIi64" size: %"PRIi64"\n",index,length);
  exit(1);
}
void concatInternal_checkEnumIndex(int64_t current,int64_t expected){
  if(current==expected)
    return;
  fprintf(stderr,"enum index (%"PRIi64") does not match current value (%"PRIi64")\n",expected,current);
  exit(2);
}
//global code
tuple1 concat_proc0 (int32_t, int32_t);
void concat_argTest (int32_t, int32_t);
void concat_doNothing (void);
int64_t concat_anInt (void);
tuple6 concat_ptrTest (void);
enum11 const concat_e1 = {.label=0,.data={0}};
enum11 const concat_e2 = {.label=1,.data={0}};
enum11 const concat_e3 = {.label=2,.data={.e2=2}};
tuple15 concat_startItr (array0);
int32_t const concat_subspace_dmut_dspaceVar = 1;
int32_t const concat_mut_dspaceVar = 0;
void concat_spaceTest (void);
int64_t const concat_subspace_dmut_dsubspaceVar = 2;
void concat_arrayTest (int32_t const*, array2, array3);
void concat_breakTest (void);
void concat_switchTest (int32_t, enum11, int8_t, array0);
tuple19 concat_nextChar (array0, int64_t);
void concat_endItr (array0, int64_t, int8_t);
array0 const concat_s0 = {.data=(arrayData1+0),.sizes={23}};
int8_t concat_c0 = 65;
array0 const concat_s1 = {.data=(arrayData0+0),.sizes={39}};
tuple16 const concat_x = {.e0=1,.e1=1};
tuple22 const concat_y = {.e0={.e0=1,.e1=1},.e1=2};
void* concat_alloc (size_t, int64_t);
void* concat_stdOut (void);
void* concat_fopen (array0);
void concat_fputs (void*, array0);
void concat_puts (array0);
void concat_fclose (void*);
//procedures code
tuple1 concat_proc0 (int32_t arg0, int32_t arg1){
concat_doNothing();
return (tuple1){.e0=arg1,.e1=arg0};
}
void concat_argTest (int32_t arg0, int32_t arg1){
}
void concat_doNothing (void){
return ;
}
int64_t concat_anInt (void){
return ((int64_t)12345678987654321);
}
tuple6 concat_ptrTest (void){
int32_t const local0 = 0;
int32_t const* const tmp0 = &(local0);
int32_t const* const tmp1 = ((int32_t const*)tmp0);
tuple5 const tmp2 = {.e0=((int32_t)0),.e1=tmp1};
tuple5 const local1 = tmp2;
tuple5 const* const tmp3 = &(local1);
tuple5 const* const tmp4 = ((tuple5 const*)tmp3);
tuple6 const tmp5 = {.e0=tmp4,.e1=((int32_t)0)};
return tmp5;
}
tuple15 concat_startItr (array0 arg0){
return (tuple15){.e0=arg0,.e1=((int64_t)0)};
}
void concat_spaceTest (void){
printf("%"PRIi32"\n",((int32_t)0));
printf("%"PRIi32"\n",((int32_t)1));
printf("%"PRIi64"\n",concat_subspace_dmut_dsubspaceVar);
}
void concat_arrayTest (int32_t const* arg0, array2 arg1, array3 arg2){
int32_t const* local0;
array4 local1;
int32_t const** local2;
array8 local3;
printf("%"PRIi64"\n",/*length*/((int64_t)3));
concatInternal_checkArrayBounds(((int32_t)0),((int64_t)2));
int64_t const tmp6 = ((arraySizes3)[((int32_t)0)]);
printf("%"PRIi64"\n",tmp6);
concatInternal_checkArrayBounds(((int32_t)0),((int64_t)3));
int32_t const tmp7 = ((arg0)[((int32_t)0)]);
concatInternal_checkArrayBounds(tmp7,((int64_t)3));
int32_t const tmp8 = ((arg0)[tmp7]);
printf("%"PRIi32"\n",tmp8);
concatInternal_checkArrayBounds(((int32_t)0),((int64_t)2));
((arg1).data[((int32_t)0)]).e0 = ((int32_t)1);
}
void concat_breakTest (void){
int32_t tmp10;
int32_t tmp9 = 0;
do{
tmp10 = tmp9;
if(!((bool)1))
  break;
int32_t const tmp11 = (tmp10+((int32_t)1));
bool const tmp12 = (tmp11>((int32_t)5));
if(tmp12){
tmp10 = tmp11;
break;
}
tmp9 = tmp11;
}while(1);
printf("%"PRIi32"\n",tmp10);
int32_t tmp15;
int32_t tmp13 = 0;
do{
bool const tmp14 = (tmp13>((int32_t)5));
if(tmp14){
tmp15 = tmp13;
break;
}
tmp15 = tmp13;
if(!((bool)1))
  break;
int32_t const tmp16 = (tmp15+((int32_t)1));
tmp13 = tmp16;
}while(1);
printf("%"PRIi32"\n",tmp15);
int32_t tmp19;
bool tmp17 = 1;
int32_t tmp18 = 0;
do{
tmp19 = tmp18;
if(!tmp17)
  break;
int32_t const tmp20 = (tmp19+((int32_t)1));
bool const tmp21 = (tmp20<((int32_t)5));
if(tmp21){
tmp17 = ((bool)1);
tmp18 = tmp20;
continue;
}
tmp17 = ((bool)0);
tmp18 = tmp20;
}while(1);
printf("%"PRIi32"\n",tmp19);
int32_t tmp25;
int32_t tmp22 = 0;
do{
int32_t const tmp23 = (tmp22+((int32_t)1));
bool const tmp24 = (tmp23<((int32_t)5));
if(tmp24){
tmp22 = tmp23;
continue;
}
tmp25 = tmp23;
if(!((bool)0))
  break;
tmp22 = tmp25;
}while(1);
printf("%"PRIi32"\n",tmp25);
}
void concat_switchTest (int32_t arg0, enum11 arg1, int8_t arg2, array0 arg3){
switch(arg0){
case 1:case 2:;
return ;
case 3:;
break;
case 0:;
if(((bool)0)){
return ;
}else{
break;
}
case -1:;
if(((bool)0)){
break;
}else{
break;
}
break;
default:
break;
}
int32_t const tmp26 = (arg1).label;
switch(tmp26){
case 0:case 1:;
break;
case 2:;
concatInternal_checkEnumIndex(arg1.label,2);
int32_t const tmp27 = (arg1).data.e2;
printf("%"PRIi32"\n",tmp27);
break;
case 3:;
concatInternal_checkEnumIndex(arg1.label,3);
tuple10 const tmp28 = (arg1).data.e3;
int8_t const tmp29 = (tmp28).e1;
printf("%"PRIi8"\n",tmp29);
break;
}
bool tmp30;
switch(arg2){
case 32:case 10:case 9:case 11:case 12:case 13:;
tmp30 = ((bool)1);
break;
default:
tmp30 = ((bool)0);
break;
}
bool const local0 = tmp30;
}
tuple19 concat_nextChar (array0 arg0, int64_t arg1){
int64_t const local0 = arg1;
array0 const local1 = arg0;
int64_t const tmp31 = ((local1).sizes[0]);
bool const tmp32 = (local0>=tmp31);
if(tmp32){
return (tuple19){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp33 = (local0+((int64_t)1));
concatInternal_checkArrayBounds(local0,((local1).sizes[0]));
int8_t const tmp34 = ((local1).data[local0]);
return (tuple19){.e0=local1,.e1=tmp33,.e2=tmp34,.e3=((bool)1)};
}
void concat_endItr (array0 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp35 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp35;
int32_t const tmp36 = (local0-((int32_t)1));
int32_t const tmp37 = (tmp36*((int32_t)3));
int32_t local1 = tmp37;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple8 local3;
tuple8* const tmp38 = &(local3);
tuple8 const* const tmp39 = ((tuple8 const*)tmp38);
tuple8 const tmp40 = {.e0=((int32_t)1),.e1=tmp39};
local3 = tmp40;
tuple8 const* const tmp41 = (local3).e1;
printf("%p\n",(void const*)tmp41);
tuple8 const** const tmp42 = &((local3).e1);
tuple8 const* const tmp43 = ((*(tmp42)))->e1;
printf("%p\n",(void const*)tmp43);
array9 local4 = {0};
concatInternal_checkArrayBounds(((int32_t)1),((int64_t)2));
concatInternal_checkArrayBounds(((int32_t)2),((int64_t)3));
((local4).data[((int32_t)2)][((int32_t)1)]) = ((int32_t)-14121);
int32_t const* const tmp44 = ((int32_t const*)local4.data);
int32_t const tmp45 = (*((tmp44)+(((int32_t)5))));
printf("%"PRIi32"\n",tmp45);
enum11 const tmp46 = {.label=2,.data={.e2=((int32_t)0)}};
enum11 local5 = tmp46;
enum11 const local6 = {.label=0,.data={0}};
int32_t const tmp47 = (local5).label;
bool const tmp48 = (tmp47==((int32_t)0));
printf("%s\n",tmp48?"true":"false");
int32_t const tmp49 = (local5).label;
bool const tmp50 = (tmp49==((int32_t)2));
printf("%s\n",tmp50?"true":"false");
concatInternal_checkEnumIndex(local5.label,2);
int32_t const tmp51 = (local5).data.e2;
printf("%"PRIi32"\n",tmp51);
tuple10 const tmp52 = {.e0=((int32_t)2),.e1=((int8_t)65)};
(local5).label = ((int32_t)3);
(local5).data.e3 = tmp52;
concatInternal_checkEnumIndex(local5.label,3);
tuple10* const tmp53 = &((local5).data.e3);
(tmp53)->e0 = ((int32_t)2);
int8_t const tmp54 = (tmp53)->e1;
printf("%"PRIi8"\n",tmp54);
enum12 local7 = /*enum*/0;
int32_t const tmp55 = ((int32_t)local7);
bool const tmp56 = (tmp55==((int32_t)1));
if(tmp56){
enum12 const tmp57 = /*enum*/2;
local7 = tmp57;
}
int64_t const tmp58 = concat_anInt();
printf("%"PRIi64"\n",tmp58);
procPtr10 const tmp59 = &(concat_proc0);
procPtr10 const local8 = tmp59;
tuple1 const tmp60 = (local8)(((int32_t)1),((int32_t)1));
int32_t const tmp61 = (tmp60).e1;
printf("%"PRIi32"\n",tmp61);
int32_t const tmp62 = (tmp60).e0;
printf("%"PRIi32"\n",tmp62);
tuple9 const tmp63 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple9 const local9 = tmp63;
int32_t const tmp64 = (local9).e0;
int32_t const tmp65 = ((int32_t)(local9).e1);
int32_t const tmp66 = (tmp64+tmp65);
printf("%"PRIi32"\n",tmp66);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp67 = ((int8_t)local0);
int64_t const tmp68 = ((int64_t)tmp67);
int64_t const tmp69 = (tmp68+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp69);
int32_t const tmp70 = (local0+((int32_t)1));
int32_t const tmp71 = (local1-((int32_t)1));
local0 = tmp71;
local1 = tmp70;
tuple1 const tmp72 = concat_proc0(((int32_t)1),((int32_t)2));
int64_t const tmp73 = ((int64_t)(tmp72).e0);
int64_t const tmp74 = ((int64_t)(tmp72).e1);
tuple21 const tmp75 = {.e0=tmp73,.e1=tmp74};
tuple21 local10 = tmp75;
int32_t const tmp76 = (++local0);
printf("%"PRIi32"\n",tmp76);
local2 = ((int32_t)2);
concat_doNothing();
tuple6 const tmp77 = concat_ptrTest();
tuple5 const* const tmp78 = (tmp77).e0;
int32_t const* const tmp79 = (tmp78)->e1;
int32_t const tmp80 = (*((tmp79)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp80);
(local10).e0 = (local10).e1;
int64_t* const tmp81 = &((local10).e0);
printf("%p\n",(void const*)tmp81);
tuple21* const tmp82 = &(local10);
tuple21 const* const tmp83 = ((tuple21 const*)tmp82);
tuple21 const* const local11 = tmp83;
int64_t const tmp84 = ((local11)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp84);
int8_t const* const tmp85 = (arrayData2+0);
printf("%p\n",(void const*)tmp85);
int8_t const* const tmp86 = (arrayData2+0);
concatInternal_checkArrayBounds(((int32_t)1),((int64_t)12));
int8_t const tmp87 = ((tmp86)[((int32_t)1)]);
printf("%"PRIi8"\n",tmp87);
printf("%"PRIi64"\n",/*length*/((int64_t)5));
bool const tmp88 = (local0==((int32_t)0));
int32_t tmp90;
if(tmp88){
int32_t const tmp89 = (((int32_t)1)+((int32_t)1));
tmp90 = tmp89;
}else{
int32_t const tmp91 = (((int32_t)1)+((int32_t)1));
bool const tmp92 = (local1!=((int32_t)0));
int32_t tmp95;
bool tmp94;
if(tmp92){
bool const tmp93 = (local0>=((int32_t)42));
tmp94 = tmp93;
tmp95 = tmp91;
}else{
tmp94 = ((bool)0);
tmp95 = tmp91;
}
if(tmp94){
int32_t const tmp96 = (tmp95+((int32_t)2));
tmp90 = tmp96;
}else{
int32_t const tmp97 = (tmp95+((int32_t)3));
tmp90 = tmp97;
}
}
printf("%"PRIi32"\n",tmp90);
int32_t* const tmp98 = &(local0);
int32_t* const tmp99 = ((int32_t*)tmp98);
int32_t* local12 = tmp99;
int32_t* const tmp100 = (++local12);
(*(tmp100)) = ((int32_t)2);
int8_t const* const tmp101 = (arrayData2+0);
tuple15 const tmp102 = concat_startItr(((array0){.data=tmp101,.sizes={5}}));
array0 tmp108;
int64_t tmp107;
int8_t tmp106;
int64_t tmp103 = (tmp102).e1;
array0 tmp104 = (tmp102).e0;
do{
tuple19 const tmp105 = concat_nextChar(tmp104,tmp103);
tmp106 = (tmp105).e2;
tmp107 = (tmp105).e1;
tmp108 = (tmp105).e0;
bool const tmp109 = (tmp105).e3;
if(!tmp109)
  break;
printf("%"PRIi8"\n",tmp106);
tmp103 = tmp107;
tmp104 = tmp108;
}while(1);
concat_endItr(tmp108,tmp107,tmp106);
int32_t tmp112;
bool tmp110 = 1;
int32_t tmp111 = 5;
do{
tmp112 = tmp111;
if(!tmp110)
  break;
printf("%"PRIi32"\n",tmp112);
int32_t const tmp113 = (tmp112-((int32_t)1));
bool const tmp114 = (tmp113>((int32_t)0));
tmp110 = tmp114;
tmp111 = tmp113;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
concat_c0 = ((int8_t)66);
int32_t const tmp115 = (((int32_t)1024)*((int32_t)1024));
int32_t const tmp116 = (tmp115*((int32_t)16));
int32_t* const tmp117 = concat_alloc(((size_t)sizeof(int32_t)),((int64_t)tmp116));
int32_t* const local13 = tmp117;
printf("%p\n",(void const*)local13);
int32_t const tmp118 = (*((local13)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp118);
int32_t const tmp119 = (*((local13)+(((int32_t)1000000))));
printf("%"PRIi32"\n",tmp119);
int8_t const* const tmp120 = (arrayData2+0);
concat_puts(((array0){.data=tmp120,.sizes={12}}));
}
void concat_puts (array0 arg0){
void* const tmp121 = concat_stdOut();
concat_fputs(tmp121,arg0);
int8_t const* const tmp122 = (arrayData0+31);
concat_fputs(tmp121,((array0){.data=tmp122,.sizes={1}}));
}
