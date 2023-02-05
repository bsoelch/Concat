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
typedef struct tuple14Impl tuple14;
typedef struct tuple16Impl tuple16;
typedef struct tuple18Impl tuple18;
typedef struct tuple20Impl tuple20;
typedef struct tuple21Impl tuple21;
typedef struct tuple22Impl tuple22;
typedef tuple1 (*procPtr0) (int32_t, int32_t);
typedef void (*procPtr1) (int32_t, int32_t);
typedef void (*procPtr2) (void);
typedef int64_t (*procPtr3) (void);
typedef tuple6 (*procPtr4) (void);
typedef tuple16 (*procPtr5) (tuple14);
typedef void (*procPtr6) (int32_t, enum11, int8_t, tuple14);
typedef tuple18 (*procPtr7) (tuple14, int64_t);
typedef void (*procPtr8) (tuple14, int64_t, int8_t);
typedef tuple1 (*procPtr9) (int32_t, int32_t);
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
int32_t const label;
};
struct enum13Impl{
union{
int64_t const* e1;
} data;
int32_t const label;
};
struct tuple14Impl{
int8_t const* e0;
int64_t e1;
};
struct tuple16Impl{
tuple14 e0;
int64_t e1;
};
struct tuple18Impl{
tuple14 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct tuple20Impl{
int64_t e0;
int64_t e1;
};
struct tuple21Impl{
int32_t e0;
int64_t e1;
};
struct tuple22Impl{
tuple1 e0;
int32_t e1;
};
const int8_t stringChars0[40] = {0x54,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x75,0x6e,0x69,0x63,0x6f,0x64,0x65,-0x1e,-0x7c,-0x5e,0x20,0x6d,0x75,0x6c,0x74,0x69,0x2d,0x6c,0x69,0x6e,0x65,0xa,0x20,0x73,0x74,0x72,0x69,0x6e,0x67,0x00};
const int8_t stringChars1[24] = {0x53,0x74,0x72,0x69,0x6e,0x67,0x73,0x20,0x61,0x72,0x65,0x20,0x22,0x69,0x67,0x6e,0x6f,0x72,0x65,0x64,0x22,0x20,0x20,0x00};
const int8_t stringChars2[13] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64,0x21,0x00};
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
tuple1 procedure0 (int32_t, int32_t);
void procedure1 (int32_t, int32_t);
void procedure2 (void);
int64_t procedure3 (void);
tuple6 procedure4 (void);
enum11 const global10 = {.label=0,.data={0}};
enum11 const global11 = {.label=1,.data={0}};
enum11 const global12 = {.label=2,.data={.e2=2}};
tuple16 procedure13 (tuple14);
void procedure14 (void);
void procedure15 (int32_t, enum11, int8_t, tuple14);
tuple18 procedure16 (tuple14, int64_t);
void procedure17 (tuple14, int64_t, int8_t);
tuple14 const global18 = {.e0=stringChars1+0,.e1=23};
int8_t global19 = 65;
tuple14 const global20 = {.e0=stringChars0+0,.e1=39};
tuple21 const global21 = {.e0=1,.e1=1};
tuple22 const global22 = {.e0={.e0=1,.e1=1},.e1=2};
//procedures code
tuple1 procedure0 (int32_t arg0, int32_t arg1){
procedure2();
return (tuple1){.e0=arg1,.e1=arg0};
}
void procedure1 (int32_t arg0, int32_t arg1){
}
void procedure2 (void){
return ;
}
int64_t procedure3 (void){
return ((int64_t)12345678987654321);
}
tuple6 procedure4 (void){
int32_t const local0 = 0;
int32_t const* const tmp0 = &(local0);
tuple5 const tmp1 = {.e0=((int32_t)0),.e1=tmp0};
tuple5 const local1 = tmp1;
tuple5 const* const tmp2 = &(local1);
tuple6 const tmp3 = {.e0=tmp2,.e1=((int32_t)0)};
return tmp3;
}
tuple16 procedure13 (tuple14 arg0){
return (tuple16){.e0=arg0,.e1=((int64_t)0)};
}
void procedure14 (void){
int32_t tmp5;
int32_t tmp4 = 0;
do{
tmp5 = tmp4;
if(!((bool)1))
  break;
int32_t const tmp6 = (tmp5+((int32_t)1));
bool const tmp7 = (tmp6>((int32_t)5));
if(tmp7){
tmp5 = tmp6;
break;
}
tmp4 = tmp6;
}while(1);
printf("%"PRIi32"\n",tmp5);
int32_t tmp10;
int32_t tmp8 = 0;
do{
bool const tmp9 = (tmp8>((int32_t)5));
if(tmp9){
tmp10 = tmp8;
break;
}
tmp10 = tmp8;
if(!((bool)1))
  break;
int32_t const tmp11 = (tmp10+((int32_t)1));
tmp8 = tmp11;
}while(1);
printf("%"PRIi32"\n",tmp10);
int32_t tmp14;
bool tmp12 = 1;
int32_t tmp13 = 0;
do{
tmp14 = tmp13;
if(!tmp12)
  break;
int32_t const tmp15 = (tmp14+((int32_t)1));
bool const tmp16 = (tmp15<((int32_t)5));
if(tmp16){
tmp12 = ((bool)1);
tmp13 = tmp15;
continue;
}
tmp12 = ((bool)0);
tmp13 = tmp15;
}while(1);
printf("%"PRIi32"\n",tmp14);
int32_t tmp20;
int32_t tmp17 = 0;
do{
int32_t const tmp18 = (tmp17+((int32_t)1));
bool const tmp19 = (tmp18<((int32_t)5));
if(tmp19){
tmp17 = tmp18;
continue;
}
tmp20 = tmp18;
if(!((bool)0))
  break;
tmp17 = tmp20;
}while(1);
printf("%"PRIi32"\n",tmp20);
}
void procedure15 (int32_t arg0, enum11 arg1, int8_t arg2, tuple14 arg3){
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
int32_t const tmp21 = (arg1).label;
switch(tmp21){
case 0:case 1:;
break;
case 2:;
concatInternal_checkEnumIndex(arg1.label,2);
int32_t const tmp23 = (arg1).data.e2;
printf("%"PRIi32"\n",tmp23);
break;
case 3:;
concatInternal_checkEnumIndex(arg1.label,3);
tuple10 const tmp25 = (arg1).data.e3;
int8_t const tmp26 = (tmp25).e1;
printf("%"PRIi8"\n",tmp26);
break;
}
bool tmp27;
switch(arg2){
case 32:case 10:case 9:case 11:case 12:case 13:;
tmp27 = ((bool)1);
break;
default:
tmp27 = ((bool)0);
break;
}
bool const local0 = tmp27;
}
tuple18 procedure16 (tuple14 arg0, int64_t arg1){
int64_t const local0 = arg1;
tuple14 const local1 = arg0;
int64_t const tmp28 = (local1).e1;
bool const tmp29 = (local0>=tmp28);
if(tmp29){
return (tuple18){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp30 = (local0+((int64_t)1));
int64_t const tmp31 = local0;
tuple14 const tmp32 = local1;
int8_t const* const tmp33 = (tmp32).e0;
int64_t const tmp34 = (tmp32).e1;
concatInternal_checkArrayBounds(tmp31,tmp34);
int8_t const tmp35 = (*((tmp33)+(tmp31)));
return (tuple18){.e0=local1,.e1=tmp30,.e2=tmp35,.e3=((bool)1)};
}
void procedure17 (tuple14 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp36 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp36;
int32_t const tmp37 = (local0-((int32_t)1));
int32_t const tmp38 = (tmp37*((int32_t)3));
int32_t local1 = tmp38;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple8 local3;
tuple8* const tmp39 = &(local3);
tuple8 const* const tmp40 = ((tuple8 const*)tmp39);
tuple8 const tmp41 = {.e0=((int32_t)1),.e1=tmp40};
local3 = tmp41;
tuple8 const* const tmp42 = (local3).e1;
printf("%p\n",(const void*)tmp42);
tuple8 const* const tmp43 = (local3).e1;
tuple8 const* const tmp44 = (tmp43)->e1;
printf("%p\n",(const void*)tmp44);
enum11 const tmp45 = {.label=2,.data={.e2=((int32_t)0)}};
enum11 local4 = tmp45;
enum11 const local5 = {.label=0,.data={0}};
int32_t const tmp46 = (local4).label;
bool const tmp47 = (tmp46==((int32_t)0));
printf("%s\n",tmp47?"true":"false");
int32_t const tmp48 = (local4).label;
bool const tmp49 = (tmp48==((int32_t)2));
printf("%s\n",tmp49?"true":"false");
concatInternal_checkEnumIndex(local4.label,2);
int32_t const tmp51 = (local4).data.e2;
printf("%"PRIi32"\n",tmp51);
tuple10 const tmp52 = {.e0=((int32_t)2),.e1=((int8_t)65)};
enum11 const tmp53 = {.label=3,.data={.e3=tmp52}};
enum11 local6 = tmp53;
concatInternal_checkEnumIndex(local6.label,3);
tuple10* const tmp55 = &((local6).data.e3);
(tmp55)->e0 = ((int32_t)2);
int8_t const tmp56 = (tmp55)->e1;
printf("%"PRIi8"\n",tmp56);
enum12 local7 = /*enum*/0;
int32_t const tmp57 = /*label*/local7;
bool const tmp58 = (tmp57==((int32_t)1));
if(tmp58){
enum12 const tmp59 = /*enum*/2;
local7 = tmp59;
}
int64_t const tmp60 = procedure3();
printf("%"PRIi64"\n",tmp60);
procPtr9 const tmp61 = &(procedure0);
procPtr9 const local8 = tmp61;
tuple1 const tmp62 = (local8)(((int32_t)1),((int32_t)1));
int32_t const tmp63 = (tmp62).e1;
printf("%"PRIi32"\n",tmp63);
int32_t const tmp64 = (tmp62).e0;
printf("%"PRIi32"\n",tmp64);
tuple9 const tmp65 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple9 const local9 = tmp65;
int32_t const tmp66 = (local9).e0;
int32_t const tmp67 = ((int32_t)(local9).e1);
int32_t const tmp68 = (tmp66+tmp67);
printf("%"PRIi32"\n",tmp68);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp69 = ((int8_t)local0);
int64_t const tmp70 = ((int64_t)tmp69);
int64_t const tmp71 = (tmp70+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp71);
int32_t const tmp72 = (local0+((int32_t)1));
int32_t const tmp73 = (local1-((int32_t)1));
local0 = tmp73;
local1 = tmp72;
tuple1 const tmp74 = procedure0(((int32_t)1),((int32_t)2));
int64_t const tmp75 = ((int64_t)(tmp74).e0);
int64_t const tmp76 = ((int64_t)(tmp74).e1);
tuple20 const tmp77 = {.e0=tmp75,.e1=tmp76};
tuple20 local10 = tmp77;
int32_t const tmp78 = (++local0);
printf("%"PRIi32"\n",tmp78);
local2 = ((int32_t)2);
procedure2();
tuple6 const tmp79 = procedure4();
tuple5 const* const tmp80 = (tmp79).e0;
int32_t const* const tmp81 = (tmp80)->e1;
int32_t const tmp82 = (*((tmp81)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp82);
(local10).e0 = (local10).e1;
int64_t* const tmp83 = &((local10).e0);
printf("%p\n",(const void*)tmp83);
tuple20* const tmp84 = &(local10);
tuple20 const* const tmp85 = ((tuple20 const*)tmp84);
tuple20 const* const local11 = tmp85;
int64_t const tmp86 = ((local11)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp86);
int32_t const tmp87 = 1;
tuple14 const tmp88 = {.e0=stringChars2+0,.e1=12};
int8_t const* const tmp89 = (tmp88).e0;
int64_t const tmp90 = (tmp88).e1;
concatInternal_checkArrayBounds(tmp87,tmp90);
int8_t const tmp91 = (*((tmp89)+(tmp87)));
printf("%"PRIi8"\n",tmp91);
tuple14 const tmp92 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp93 = (tmp92).e1;
printf("%"PRIi64"\n",tmp93);
bool const tmp94 = (local0==((int32_t)0));
int32_t tmp96;
if(tmp94){
int32_t const tmp95 = (((int32_t)1)+((int32_t)1));
tmp96 = tmp95;
}else{
int32_t const tmp97 = (((int32_t)1)+((int32_t)1));
bool const tmp98 = (local1!=((int32_t)0));
int32_t tmp101;
bool tmp100;
if(tmp98){
bool const tmp99 = (local0>=((int32_t)42));
tmp100 = tmp99;
tmp101 = tmp97;
}else{
tmp100 = ((bool)0);
tmp101 = tmp97;
}
if(tmp100){
int32_t const tmp102 = (tmp101+((int32_t)2));
tmp96 = tmp102;
}else{
int32_t const tmp103 = (tmp101+((int32_t)3));
tmp96 = tmp103;
}
}
printf("%"PRIi32"\n",tmp96);
int32_t* const tmp104 = &(local0);
int32_t* local12 = tmp104;
int32_t* const tmp105 = (++local12);
(*(tmp105)) = ((int32_t)2);
tuple14 const tmp106 = {.e0=stringChars2+0,.e1=5};
tuple16 const tmp107 = procedure13(tmp106);
tuple14 tmp113;
int64_t tmp112;
int8_t tmp111;
int64_t tmp108 = (tmp107).e1;
tuple14 tmp109 = (tmp107).e0;
do{
tuple18 const tmp110 = procedure16(tmp109,tmp108);
tmp111 = (tmp110).e2;
tmp112 = (tmp110).e1;
tmp113 = (tmp110).e0;
bool const tmp114 = (tmp110).e3;
if(!tmp114)
  break;
printf("%"PRIi8"\n",tmp111);
tmp108 = tmp112;
tmp109 = tmp113;
}while(1);
procedure17(tmp113,tmp112,tmp111);
int32_t tmp117;
bool tmp115 = 1;
int32_t tmp116 = 5;
do{
tmp117 = tmp116;
if(!tmp115)
  break;
printf("%"PRIi32"\n",tmp117);
int32_t const tmp118 = (tmp117-((int32_t)1));
bool const tmp119 = (tmp118>((int32_t)0));
tmp115 = tmp119;
tmp116 = tmp118;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
global19 = ((int8_t)66);
}
