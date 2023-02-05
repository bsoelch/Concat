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
tuple8 const** const tmp43 = &((local3).e1);
tuple8 const* const tmp44 = ((*(tmp43)))->e1;
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
concatInternal_checkEnumIndex(local4.label,3);
(local4).data.e3 = tmp52;
concatInternal_checkEnumIndex(local4.label,3);
tuple10* const tmp55 = &((local4).data.e3);
((*(tmp55))).e0 = ((int32_t)2);
concatInternal_checkEnumIndex(local4.label,3);
tuple10* const tmp57 = &((local4).data.e3);
int8_t const tmp58 = ((*(tmp57))).e1;
printf("%"PRIi8"\n",tmp58);
enum12 local6 = /*enum*/0;
int32_t const tmp59 = /*label*/local6;
bool const tmp60 = (tmp59==((int32_t)1));
if(tmp60){
enum12 const tmp61 = /*enum*/2;
local6 = tmp61;
}
int64_t const tmp62 = procedure3();
printf("%"PRIi64"\n",tmp62);
procPtr9 const tmp63 = &(procedure0);
procPtr9 const local7 = tmp63;
tuple1 const tmp64 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp65 = (tmp64).e1;
printf("%"PRIi32"\n",tmp65);
int32_t const tmp66 = (tmp64).e0;
printf("%"PRIi32"\n",tmp66);
tuple9 const tmp67 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple9 const local8 = tmp67;
int32_t const tmp68 = (local8).e0;
int32_t const tmp69 = ((int32_t)(local8).e1);
int32_t const tmp70 = (tmp68+tmp69);
printf("%"PRIi32"\n",tmp70);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp71 = ((int8_t)local0);
int64_t const tmp72 = ((int64_t)tmp71);
int64_t const tmp73 = (tmp72+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp73);
int32_t const tmp74 = (local0+((int32_t)1));
int32_t const tmp75 = (local1-((int32_t)1));
local0 = tmp75;
local1 = tmp74;
tuple1 const tmp76 = procedure0(((int32_t)1),((int32_t)2));
int64_t const tmp77 = ((int64_t)(tmp76).e0);
int64_t const tmp78 = ((int64_t)(tmp76).e1);
tuple20 const tmp79 = {.e0=tmp77,.e1=tmp78};
tuple20 local9 = tmp79;
int32_t const tmp80 = (++local0);
printf("%"PRIi32"\n",tmp80);
local2 = ((int32_t)2);
procedure2();
tuple6 const tmp81 = procedure4();
tuple5 const* const tmp82 = (tmp81).e0;
int32_t const* const tmp83 = (tmp82)->e1;
int32_t const tmp84 = (*((tmp83)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp84);
(local9).e0 = (local9).e1;
int64_t* const tmp85 = &((local9).e0);
printf("%p\n",(const void*)tmp85);
tuple20* const tmp86 = &(local9);
tuple20 const* const tmp87 = ((tuple20 const*)tmp86);
tuple20 const* const local10 = tmp87;
int64_t const tmp88 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp88);
int32_t const tmp89 = 1;
tuple14 const tmp90 = {.e0=stringChars2+0,.e1=12};
int8_t const* const tmp91 = (tmp90).e0;
int64_t const tmp92 = (tmp90).e1;
concatInternal_checkArrayBounds(tmp89,tmp92);
int8_t const tmp93 = (*((tmp91)+(tmp89)));
printf("%"PRIi8"\n",tmp93);
tuple14 const tmp94 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp95 = (tmp94).e1;
printf("%"PRIi64"\n",tmp95);
bool const tmp96 = (local0==((int32_t)0));
int32_t tmp98;
if(tmp96){
int32_t const tmp97 = (((int32_t)1)+((int32_t)1));
tmp98 = tmp97;
}else{
int32_t const tmp99 = (((int32_t)1)+((int32_t)1));
bool const tmp100 = (local1!=((int32_t)0));
int32_t tmp103;
bool tmp102;
if(tmp100){
bool const tmp101 = (local0>=((int32_t)42));
tmp102 = tmp101;
tmp103 = tmp99;
}else{
tmp102 = ((bool)0);
tmp103 = tmp99;
}
if(tmp102){
int32_t const tmp104 = (tmp103+((int32_t)2));
tmp98 = tmp104;
}else{
int32_t const tmp105 = (tmp103+((int32_t)3));
tmp98 = tmp105;
}
}
printf("%"PRIi32"\n",tmp98);
int32_t* const tmp106 = &(local0);
int32_t* local11 = tmp106;
int32_t* const tmp107 = (++local11);
(*(tmp107)) = ((int32_t)2);
tuple14 const tmp108 = {.e0=stringChars2+0,.e1=5};
tuple16 const tmp109 = procedure13(tmp108);
tuple14 tmp115;
int64_t tmp114;
int8_t tmp113;
int64_t tmp110 = (tmp109).e1;
tuple14 tmp111 = (tmp109).e0;
do{
tuple18 const tmp112 = procedure16(tmp111,tmp110);
tmp113 = (tmp112).e2;
tmp114 = (tmp112).e1;
tmp115 = (tmp112).e0;
bool const tmp116 = (tmp112).e3;
if(!tmp116)
  break;
printf("%"PRIi8"\n",tmp113);
tmp110 = tmp114;
tmp111 = tmp115;
}while(1);
procedure17(tmp115,tmp114,tmp113);
int32_t tmp119;
bool tmp117 = 1;
int32_t tmp118 = 5;
do{
tmp119 = tmp118;
if(!tmp117)
  break;
printf("%"PRIi32"\n",tmp119);
int32_t const tmp120 = (tmp119-((int32_t)1));
bool const tmp121 = (tmp120>((int32_t)0));
tmp117 = tmp121;
tmp118 = tmp120;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
global19 = ((int8_t)66);
}
