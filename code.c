#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple0Impl tuple0;
typedef struct tuple4Impl tuple4;
typedef struct tuple5Impl tuple5;
typedef struct tuple7Impl tuple7;
typedef struct tuple8Impl tuple8;
typedef struct enum9Impl enum9;
typedef int32_t enum10;
typedef struct enum11Impl enum11;
typedef struct tuple12Impl tuple12;
typedef struct tuple14Impl tuple14;
typedef struct tuple16Impl tuple16;
typedef struct tuple18Impl tuple18;
typedef struct tuple19Impl tuple19;
typedef struct tuple20Impl tuple20;
typedef tuple0 (*procPtr0) (int32_t, int32_t);
typedef void (*procPtr1) (int32_t, int32_t);
typedef void (*procPtr2) (void);
typedef int64_t (*procPtr3) (void);
typedef tuple5 (*procPtr4) (void);
typedef tuple14 (*procPtr5) (tuple12);
typedef void (*procPtr6) (int32_t, enum9, int8_t, tuple12);
typedef tuple16 (*procPtr7) (tuple12, int64_t);
typedef void (*procPtr8) (tuple12, int64_t, int8_t);
typedef tuple0 (*procPtr9) (int32_t, int32_t);
struct tuple0Impl{
int32_t e0;
int32_t e1;
};
struct tuple4Impl{
int32_t e0;
int32_t const* e1;
};
struct tuple5Impl{
tuple4 const* e0;
int32_t e1;
};
struct tuple7Impl{
int32_t e0;
tuple7 const* e1;
};
struct tuple8Impl{
int32_t e0;
int8_t e1;
};
struct enum9Impl{
union{
int32_t e2;
int8_t e3;
} data;
int32_t const label;
};
struct enum11Impl{
union{
int64_t const* e1;
} data;
int32_t const label;
};
struct tuple12Impl{
int8_t const* e0;
int64_t e1;
};
struct tuple14Impl{
tuple12 e0;
int64_t e1;
};
struct tuple16Impl{
tuple12 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct tuple18Impl{
int64_t e0;
int64_t e1;
};
struct tuple19Impl{
int32_t e0;
int64_t e1;
};
struct tuple20Impl{
tuple0 e0;
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
tuple0 procedure0 (int32_t, int32_t);
void procedure1 (int32_t, int32_t);
void procedure2 (void);
int64_t procedure3 (void);
tuple5 procedure4 (void);
enum9 global10 = {.label=0,.data={0}};
enum9 global11 = {.label=1,.data={0}};
enum9 global12 = {.label=2,.data={.e2=2}};
tuple14 procedure13 (tuple12);
void procedure14 (void);
void procedure15 (int32_t, enum9, int8_t, tuple12);
tuple16 procedure16 (tuple12, int64_t);
void procedure17 (tuple12, int64_t, int8_t);
tuple12 global18 = {.e0=stringChars1+0,.e1=23};
int8_t global19 = 65;
tuple12 global20 = {.e0=stringChars0+0,.e1=39};
tuple19 global21 = {.e0=1,.e1=1};
tuple20 global22 = {.e0={.e0=1,.e1=1},.e1=2};
//procedures code
tuple0 procedure0 (int32_t arg0, int32_t arg1){
procedure2();
return (tuple0){.e0=arg1,.e1=arg0};
}
void procedure1 (int32_t arg0, int32_t arg1){
}
void procedure2 (void){
return ;
}
int64_t procedure3 (void){
return ((int64_t)12345678987654321);
}
tuple5 procedure4 (void){
int32_t local0 = 0;
int32_t const* const tmp0 = &(local0);
tuple4 const tmp1 = {.e0=((int32_t)0),.e1=tmp0};
tuple4 local1 = tmp1;
tuple4* const tmp2 = &(local1);
tuple4 const* const tmp3 = ((tuple4 const*)tmp2);
tuple5 const tmp4 = {.e0=tmp3,.e1=((int32_t)0)};
return tmp4;
}
tuple14 procedure13 (tuple12 arg0){
return (tuple14){.e0=arg0,.e1=((int64_t)0)};
}
void procedure14 (void){
int32_t tmp6;
int32_t tmp5 = 0;
do{
tmp6 = tmp5;
if(!((bool)1))
  break;
int32_t const tmp7 = (tmp6+((int32_t)1));
bool const tmp8 = (tmp7>((int32_t)5));
if(tmp8){
tmp6 = tmp7;
break;
}
tmp5 = tmp7;
}while(1);
printf("%"PRIi32"\n",tmp6);
int32_t tmp11;
int32_t tmp9 = 0;
do{
bool const tmp10 = (tmp9>((int32_t)5));
if(tmp10){
tmp11 = tmp9;
break;
}
tmp11 = tmp9;
if(!((bool)1))
  break;
int32_t const tmp12 = (tmp11+((int32_t)1));
tmp9 = tmp12;
}while(1);
printf("%"PRIi32"\n",tmp11);
int32_t tmp15;
bool tmp13 = 1;
int32_t tmp14 = 0;
do{
tmp15 = tmp14;
if(!tmp13)
  break;
int32_t const tmp16 = (tmp15+((int32_t)1));
bool const tmp17 = (tmp16<((int32_t)5));
if(tmp17){
tmp13 = ((bool)1);
tmp14 = tmp16;
continue;
}
tmp13 = ((bool)0);
tmp14 = tmp16;
}while(1);
printf("%"PRIi32"\n",tmp15);
int32_t tmp21;
int32_t tmp18 = 0;
do{
int32_t const tmp19 = (tmp18+((int32_t)1));
bool const tmp20 = (tmp19<((int32_t)5));
if(tmp20){
tmp18 = tmp19;
continue;
}
tmp21 = tmp19;
if(!((bool)0))
  break;
tmp18 = tmp21;
}while(1);
printf("%"PRIi32"\n",tmp21);
}
void procedure15 (int32_t arg0, enum9 arg1, int8_t arg2, tuple12 arg3){
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
int32_t const tmp22 = (arg1).label;
switch(tmp22){
case 0:case 1:;
break;
case 2:;
enum9 const tmp23 = arg1;
concatInternal_checkEnumIndex(tmp23.label,2);
int32_t const tmp24 = (tmp23).data.e2;
printf("%"PRIi32"\n",tmp24);
break;
case 3:;
enum9 const tmp25 = arg1;
concatInternal_checkEnumIndex(tmp25.label,3);
int8_t const tmp26 = (tmp25).data.e3;
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
bool local0 = tmp27;
}
tuple16 procedure16 (tuple12 arg0, int64_t arg1){
int64_t local0 = arg1;
tuple12 local1 = arg0;
int64_t const tmp28 = (local1).e1;
bool const tmp29 = (local0>=tmp28);
if(tmp29){
return (tuple16){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp30 = (local0+((int64_t)1));
int64_t const tmp31 = local0;
tuple12 const tmp32 = local1;
int8_t const* const tmp33 = (tmp32).e0;
int64_t const tmp34 = (tmp32).e1;
concatInternal_checkArrayBounds(tmp31,tmp34);
int8_t const tmp35 = (*((tmp33)+(tmp31)));
return (tuple16){.e0=local1,.e1=tmp30,.e2=tmp35,.e3=((bool)1)};
}
void procedure17 (tuple12 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp36 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp36;
int32_t const tmp37 = (local0-((int32_t)1));
int32_t const tmp38 = (tmp37*((int32_t)3));
int32_t local1 = tmp38;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple7 local3;
tuple7* const tmp39 = &(local3);
tuple7 const* const tmp40 = ((tuple7 const*)tmp39);
tuple7 const tmp41 = {.e0=((int32_t)1),.e1=tmp40};
local3 = tmp41;
tuple7 const* const tmp42 = (local3).e1;
printf("%p\n",(const void*)tmp42);
tuple7 const* const tmp43 = (local3).e1;
tuple7 const* const tmp44 = (tmp43)->e1;
printf("%p\n",(const void*)tmp44);
enum9 const tmp45 = {.label=2,.data={.e2=((int32_t)0)}};
enum9 local4 = tmp45;
enum9 local5 = {.label=0,.data={0}};
int32_t const tmp46 = (local4).label;
bool const tmp47 = (tmp46==((int32_t)0));
printf("%s\n",tmp47?"true":"false");
int32_t const tmp48 = (local4).label;
bool const tmp49 = (tmp48==((int32_t)2));
printf("%s\n",tmp49?"true":"false");
enum9 const tmp50 = local4;
concatInternal_checkEnumIndex(tmp50.label,2);
int32_t const tmp51 = (tmp50).data.e2;
printf("%"PRIi32"\n",tmp51);
enum10 local6 = /*enum*/0;
int32_t const tmp52 = /*label*/local6;
bool const tmp53 = (tmp52==((int32_t)1));
if(tmp53){
enum10 const tmp54 = /*enum*/2;
local6 = tmp54;
}
int64_t const tmp55 = procedure3();
printf("%"PRIi64"\n",tmp55);
procPtr9 const tmp56 = &(procedure0);
procPtr9 local7 = tmp56;
tuple0 const tmp57 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp58 = (tmp57).e1;
printf("%"PRIi32"\n",tmp58);
int32_t const tmp59 = (tmp57).e0;
printf("%"PRIi32"\n",tmp59);
tuple8 const tmp60 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple8 local8 = tmp60;
int32_t const tmp61 = (local8).e0;
int8_t const tmp62 = (local8).e1;
int32_t const tmp63 = ((int32_t)tmp62);
int32_t const tmp64 = (tmp61+tmp63);
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
tuple0 const tmp70 = procedure0(((int32_t)1),((int32_t)2));
int32_t const tmp71 = (tmp70).e0;
int32_t const tmp72 = (tmp70).e1;
int64_t const tmp73 = ((int64_t)tmp71);
int64_t const tmp74 = ((int64_t)tmp72);
tuple18 const tmp75 = {.e0=tmp73,.e1=tmp74};
tuple18 local9 = tmp75;
int32_t const tmp76 = (++local0);
printf("%"PRIi32"\n",tmp76);
local2 = ((int32_t)2);
procedure2();
tuple5 const tmp77 = procedure4();
tuple4 const* const tmp78 = (tmp77).e0;
int32_t const* const tmp79 = (tmp78)->e1;
int32_t const tmp80 = (*((tmp79)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp80);
(local9).e0 = (local9).e1;
int64_t* const tmp81 = &((local9).e0);
printf("%p\n",(const void*)tmp81);
tuple18* const tmp82 = &(local9);
tuple18 const* const tmp83 = ((tuple18 const*)tmp82);
tuple18 const* local10 = tmp83;
int64_t const tmp84 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp84);
int32_t const tmp85 = 1;
tuple12 const tmp86 = {.e0=stringChars2+0,.e1=12};
int8_t const* const tmp87 = (tmp86).e0;
int64_t const tmp88 = (tmp86).e1;
concatInternal_checkArrayBounds(tmp85,tmp88);
int8_t const tmp89 = (*((tmp87)+(tmp85)));
printf("%"PRIi8"\n",tmp89);
tuple12 const tmp90 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp91 = (tmp90).e1;
printf("%"PRIi64"\n",tmp91);
bool const tmp92 = (local0==((int32_t)0));
int32_t tmp94;
if(tmp92){
int32_t const tmp93 = (((int32_t)1)+((int32_t)1));
tmp94 = tmp93;
}else{
int32_t const tmp95 = (((int32_t)1)+((int32_t)1));
bool const tmp96 = (local1!=((int32_t)0));
int32_t tmp99;
bool tmp98;
if(tmp96){
bool const tmp97 = (local0>=((int32_t)42));
tmp98 = tmp97;
tmp99 = tmp95;
}else{
tmp98 = ((bool)0);
tmp99 = tmp95;
}
if(tmp98){
int32_t const tmp100 = (tmp99+((int32_t)2));
tmp94 = tmp100;
}else{
int32_t const tmp101 = (tmp99+((int32_t)3));
tmp94 = tmp101;
}
}
printf("%"PRIi32"\n",tmp94);
int32_t* const tmp102 = &(local0);
int32_t* local11 = tmp102;
int32_t* const tmp103 = (++local11);
(*(tmp103)) = ((int32_t)2);
tuple12 const tmp104 = {.e0=stringChars2+0,.e1=5};
tuple14 const tmp105 = procedure13(tmp104);
tuple12 tmp111;
int64_t tmp110;
int8_t tmp109;
int64_t tmp106 = (tmp105).e1;
tuple12 tmp107 = (tmp105).e0;
do{
tuple16 const tmp108 = procedure16(tmp107,tmp106);
tmp109 = (tmp108).e2;
tmp110 = (tmp108).e1;
tmp111 = (tmp108).e0;
bool const tmp112 = (tmp108).e3;
if(!tmp112)
  break;
printf("%"PRIi8"\n",tmp109);
tmp106 = tmp110;
tmp107 = tmp111;
}while(1);
procedure17(tmp111,tmp110,tmp109);
int32_t tmp115;
bool tmp113 = 1;
int32_t tmp114 = 5;
do{
tmp115 = tmp114;
if(!tmp113)
  break;
printf("%"PRIi32"\n",tmp115);
int32_t const tmp116 = (tmp115-((int32_t)1));
bool const tmp117 = (tmp116>((int32_t)0));
tmp113 = tmp117;
tmp114 = tmp116;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
