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
typedef struct array2Impl array2;
typedef struct array3Impl array3;
typedef struct array4Impl array4;
typedef struct array5Impl array5;
typedef struct array6Impl array6;
typedef struct array9Impl array9;
typedef struct array10Impl array10;
typedef struct array11Impl array11;
typedef tuple1 (*procPtr0) (int32_t, int32_t);
typedef void (*procPtr1) (int32_t, int32_t);
typedef void (*procPtr2) (void);
typedef int64_t (*procPtr3) (void);
typedef tuple6 (*procPtr4) (void);
typedef tuple15 (*procPtr5) (array0);
typedef void (*procPtr6) (int32_t const*, array3, array4);
typedef void (*procPtr7) (int32_t, enum11, int8_t, array0);
typedef tuple19 (*procPtr8) (array0, int64_t);
typedef void (*procPtr9) (array0, int64_t, int8_t);
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
struct tuple16Impl{
int32_t e0;
int64_t e1;
};
struct array2Impl{
tuple16 data[2];
};
struct array3Impl{
tuple16 data[2];
};
struct array4Impl{
int64_t data[3][2];
};
int64_t const arraySizes4[2]={2,3};
struct array5Impl{
int64_t data[10][8][6][4][2];
};
struct array6Impl{
int64_t data[10][8][6][4][2];
};
struct array9Impl{
int32_t const* data;
int64_t sizes[1];
};
struct array10Impl{
int32_t* data;
int64_t sizes[1];
};
struct array11Impl{
array10 const* data;
int64_t sizes[1];
};
struct tuple19Impl{
array0 e0;
int64_t e1;
int8_t e2;
bool e3;
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
void concat_arrayTest (int32_t const*, array3, array4);
void concat_breakTest (void);
void concat_switchTest (int32_t, enum11, int8_t, array0);
tuple19 concat_nextChar (array0, int64_t);
void concat_endItr (array0, int64_t, int8_t);
array0 const concat_s0 = {.data=(arrayData1+0),.sizes={23}};
int8_t concat_c0 = 65;
array0 const concat_s1 = {.data=(arrayData0+0),.sizes={39}};
tuple16 const concat_x = {.e0=1,.e1=1};
tuple22 const concat_y = {.e0={.e0=1,.e1=1},.e1=2};
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
void concat_arrayTest (int32_t const* arg0, array3 arg1, array4 arg2){
int32_t const* local0;
array6 local1;
int32_t const** local2;
array11 local3;
printf("%"PRIi64"\n",/*length*/((int64_t)3));
concatInternal_checkArrayBounds(((int32_t)0),((int64_t)2));
int64_t const tmp6 = ((arraySizes4)[((int32_t)0)]);
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
enum11 const tmp44 = {.label=2,.data={.e2=((int32_t)0)}};
enum11 local4 = tmp44;
enum11 const local5 = {.label=0,.data={0}};
int32_t const tmp45 = (local4).label;
bool const tmp46 = (tmp45==((int32_t)0));
printf("%s\n",tmp46?"true":"false");
int32_t const tmp47 = (local4).label;
bool const tmp48 = (tmp47==((int32_t)2));
printf("%s\n",tmp48?"true":"false");
concatInternal_checkEnumIndex(local4.label,2);
int32_t const tmp49 = (local4).data.e2;
printf("%"PRIi32"\n",tmp49);
tuple10 const tmp50 = {.e0=((int32_t)2),.e1=((int8_t)65)};
(local4).label = ((int32_t)3);
(local4).data.e3 = tmp50;
concatInternal_checkEnumIndex(local4.label,3);
tuple10* const tmp51 = &((local4).data.e3);
(tmp51)->e0 = ((int32_t)2);
int8_t const tmp52 = (tmp51)->e1;
printf("%"PRIi8"\n",tmp52);
enum12 local6 = /*enum*/0;
int32_t const tmp53 = ((int32_t)local6);
bool const tmp54 = (tmp53==((int32_t)1));
if(tmp54){
enum12 const tmp55 = /*enum*/2;
local6 = tmp55;
}
int64_t const tmp56 = concat_anInt();
printf("%"PRIi64"\n",tmp56);
procPtr10 const tmp57 = &(concat_proc0);
procPtr10 const local7 = tmp57;
tuple1 const tmp58 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp59 = (tmp58).e1;
printf("%"PRIi32"\n",tmp59);
int32_t const tmp60 = (tmp58).e0;
printf("%"PRIi32"\n",tmp60);
tuple9 const tmp61 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple9 const local8 = tmp61;
int32_t const tmp62 = (local8).e0;
int32_t const tmp63 = ((int32_t)(local8).e1);
int32_t const tmp64 = (tmp62+tmp63);
printf("%"PRIi32"\n",tmp64);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp65 = ((int8_t)local0);
int64_t const tmp66 = ((int64_t)tmp65);
int64_t const tmp67 = (tmp66+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp67);
int32_t const tmp68 = (local0+((int32_t)1));
int32_t const tmp69 = (local1-((int32_t)1));
local0 = tmp69;
local1 = tmp68;
tuple1 const tmp70 = concat_proc0(((int32_t)1),((int32_t)2));
int64_t const tmp71 = ((int64_t)(tmp70).e0);
int64_t const tmp72 = ((int64_t)(tmp70).e1);
tuple21 const tmp73 = {.e0=tmp71,.e1=tmp72};
tuple21 local9 = tmp73;
int32_t const tmp74 = (++local0);
printf("%"PRIi32"\n",tmp74);
local2 = ((int32_t)2);
concat_doNothing();
tuple6 const tmp75 = concat_ptrTest();
tuple5 const* const tmp76 = (tmp75).e0;
int32_t const* const tmp77 = (tmp76)->e1;
int32_t const tmp78 = (*((tmp77)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp78);
(local9).e0 = (local9).e1;
int64_t* const tmp79 = &((local9).e0);
printf("%p\n",(void const*)tmp79);
tuple21* const tmp80 = &(local9);
tuple21 const* const tmp81 = ((tuple21 const*)tmp80);
tuple21 const* const local10 = tmp81;
int64_t const tmp82 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp82);
int8_t const* const tmp83 = (arrayData2+0);
printf("%p\n",(void const*)tmp83);
int8_t const* const tmp84 = (arrayData2+0);
concatInternal_checkArrayBounds(((int32_t)1),((int64_t)12));
int8_t const tmp85 = ((tmp84)[((int32_t)1)]);
printf("%"PRIi8"\n",tmp85);
printf("%"PRIi64"\n",/*length*/((int64_t)5));
bool const tmp86 = (local0==((int32_t)0));
int32_t tmp88;
if(tmp86){
int32_t const tmp87 = (((int32_t)1)+((int32_t)1));
tmp88 = tmp87;
}else{
int32_t const tmp89 = (((int32_t)1)+((int32_t)1));
bool const tmp90 = (local1!=((int32_t)0));
int32_t tmp93;
bool tmp92;
if(tmp90){
bool const tmp91 = (local0>=((int32_t)42));
tmp92 = tmp91;
tmp93 = tmp89;
}else{
tmp92 = ((bool)0);
tmp93 = tmp89;
}
if(tmp92){
int32_t const tmp94 = (tmp93+((int32_t)2));
tmp88 = tmp94;
}else{
int32_t const tmp95 = (tmp93+((int32_t)3));
tmp88 = tmp95;
}
}
printf("%"PRIi32"\n",tmp88);
int32_t* const tmp96 = &(local0);
int32_t* const tmp97 = ((int32_t*)tmp96);
int32_t* local11 = tmp97;
int32_t* const tmp98 = (++local11);
(*(tmp98)) = ((int32_t)2);
int8_t const* const tmp99 = (arrayData2+0);
tuple15 const tmp100 = concat_startItr(((array0){.data=tmp99,.sizes={5}}));
array0 tmp106;
int64_t tmp105;
int8_t tmp104;
int64_t tmp101 = (tmp100).e1;
array0 tmp102 = (tmp100).e0;
do{
tuple19 const tmp103 = concat_nextChar(tmp102,tmp101);
tmp104 = (tmp103).e2;
tmp105 = (tmp103).e1;
tmp106 = (tmp103).e0;
bool const tmp107 = (tmp103).e3;
if(!tmp107)
  break;
printf("%"PRIi8"\n",tmp104);
tmp101 = tmp105;
tmp102 = tmp106;
}while(1);
concat_endItr(tmp106,tmp105,tmp104);
int32_t tmp110;
bool tmp108 = 1;
int32_t tmp109 = 5;
do{
tmp110 = tmp109;
if(!tmp108)
  break;
printf("%"PRIi32"\n",tmp110);
int32_t const tmp111 = (tmp110-((int32_t)1));
bool const tmp112 = (tmp111>((int32_t)0));
tmp108 = tmp112;
tmp109 = tmp111;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
concat_c0 = ((int8_t)66);
}
