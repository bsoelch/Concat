#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple0Impl tuple0;
typedef struct tuple3Impl tuple3;
typedef struct tuple4Impl tuple4;
typedef struct tuple6Impl tuple6;
typedef struct tuple7Impl tuple7;
typedef struct enum8Impl enum8;
typedef struct enum9Impl enum9;
typedef struct enum10Impl enum10;
typedef struct tuple11Impl tuple11;
typedef struct tuple13Impl tuple13;
typedef struct tuple14Impl tuple14;
typedef struct tuple16Impl tuple16;
typedef struct tuple17Impl tuple17;
typedef struct tuple18Impl tuple18;
typedef tuple0 (*procPtr0) (int32_t, int32_t);
typedef void (*procPtr1) (void);
typedef int64_t (*procPtr2) (void);
typedef tuple4 (*procPtr3) (void);
typedef tuple13 (*procPtr4) (tuple11);
typedef tuple14 (*procPtr5) (tuple11, int64_t);
typedef void (*procPtr6) (tuple11, int64_t, int8_t);
struct tuple0Impl{
int32_t e0;
int32_t e1;
};
struct tuple3Impl{
int32_t e0;
int32_t* e1;
};
struct tuple4Impl{
tuple3* e0;
int32_t e1;
};
struct tuple6Impl{
int32_t e0;
tuple6* e1;
};
struct tuple7Impl{
int32_t e0;
int8_t e1;
};
struct enum8Impl{
union{
int32_t e2;
int8_t e3;
} data;
int32_t const label;
};
struct enum9Impl{
int32_t const label;
};
struct enum10Impl{
union{
int64_t* e1;
} data;
int32_t const label;
};
struct tuple11Impl{
const int8_t* e0;
int64_t e1;
};
struct tuple13Impl{
tuple11 e0;
int64_t e1;
};
struct tuple14Impl{
tuple11 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct tuple16Impl{
int64_t e0;
int64_t e1;
};
struct tuple17Impl{
int32_t e0;
int64_t e1;
};
struct tuple18Impl{
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
tuple0 procedure1 (int32_t, int32_t);
void procedure2 (void);
int64_t procedure3 (void);
tuple4 procedure4 (void);
enum8 global10 = {.label=0,.data={0}};
enum8 global11 = {.label=1,.data={0}};
enum8 global12 = {.label=2,.data={.e2=2}};
tuple13 procedure13 (tuple11);
void procedure14 (void);
tuple14 procedure15 (tuple11, int64_t);
void procedure16 (tuple11, int64_t, int8_t);
tuple11 global17 = {.e0=stringChars1+0,.e1=23};
int8_t global18 = 65;
tuple11 global19 = {.e0=stringChars0+0,.e1=39};
tuple17 global20 = {.e0=1,.e1=1};
tuple18 global21 = {.e0={.e0=1,.e1=1},.e1=2};
//procedures code
tuple0 procedure1 (int32_t arg0, int32_t arg1){
procedure2();
return (tuple0){.e0=arg0,.e1=arg1};
}
void procedure2 (void){
return ;
}
int64_t procedure3 (void){
return ((int64_t)12345678987654321);
}
tuple4 procedure4 (void){
int32_t local5 = 0;
int32_t* const tmp0 = &(local5);
tuple3 const tmp1 = {.e0=((int32_t)0),.e1=tmp0};
tuple3 local6 = tmp1;
tuple3* const tmp2 = &(local6);
tuple4 const tmp3 = {.e0=tmp2,.e1=((int32_t)0)};
return tmp3;
}
tuple13 procedure13 (tuple11 arg0){
return (tuple13){.e0=arg0,.e1=((int64_t)0)};
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
tuple14 procedure15 (tuple11 arg0, int64_t arg1){
int64_t local16 = arg1;
tuple11 local17 = arg0;
int64_t const tmp21 = (local17).e1;
bool const tmp22 = (local16>=tmp21);
if(tmp22){
return (tuple14){.e0=local17,.e1=local16,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp23 = (local16+((int64_t)1));
int64_t const tmp24 = local16;
tuple11 const tmp25 = local17;
const int8_t* const tmp26 = (tmp25).e0;
int64_t const tmp27 = (tmp25).e1;
concatInternal_checkArrayBounds(tmp24,tmp27);
int8_t const tmp28 = (*((tmp26)+(tmp24)));
return (tuple14){.e0=local17,.e1=tmp23,.e2=tmp28,.e3=((bool)1)};
}
void procedure16 (tuple11 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp29 = (((int32_t)1)+((int32_t)1));
int32_t local17 = tmp29;
int32_t const tmp30 = (local17-((int32_t)1));
int32_t const tmp31 = (tmp30*((int32_t)3));
int32_t local18 = tmp31;
int32_t local19;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple6 local20;
tuple6* const tmp32 = &(local20);
tuple6 const tmp33 = {.e0=((int32_t)1),.e1=tmp32};
local20 = tmp33;
tuple6* const tmp34 = (local20).e1;
printf("%p\n",(void*)tmp34);
tuple6* const tmp35 = (local20).e1;
tuple6* const tmp36 = (tmp35)->e1;
printf("%p\n",(void*)tmp36);
enum8 const tmp37 = {.label=2,.data={.e2=((int32_t)0)}};
enum8 local21 = tmp37;
enum8 local22 = {.label=0,.data={0}};
int32_t const tmp38 = (local21).label;
bool const tmp39 = (tmp38==((int32_t)0));
printf("%s\n",tmp39?"true":"false");
int32_t const tmp40 = (local21).label;
bool const tmp41 = (tmp40==((int32_t)2));
printf("%s\n",tmp41?"true":"false");
enum8 const tmp42 = local21;
concatInternal_checkEnumIndex(tmp42.label,2);
int32_t const tmp43 = (tmp42).data.e2;
printf("%"PRIi32"\n",tmp43);
int64_t const tmp44 = procedure3();
printf("%"PRIi64"\n",tmp44);
procPtr0 const tmp45 = &(procedure1);
procPtr0 local23 = tmp45;
tuple0 const tmp46 = (local23)(((int32_t)1),((int32_t)1));
int32_t const tmp47 = (tmp46).e1;
printf("%"PRIi32"\n",tmp47);
int32_t const tmp48 = (tmp46).e0;
printf("%"PRIi32"\n",tmp48);
tuple7 const tmp49 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple7 local24 = tmp49;
int32_t const tmp50 = (local24).e0;
int8_t const tmp51 = (local24).e1;
int32_t const tmp52 = ((int32_t)tmp51);
int32_t const tmp53 = (tmp50+tmp52);
printf("%"PRIi32"\n",tmp53);
printf("%"PRIi32"\n",local17);
printf("%"PRIi32"\n",local18);
int8_t const tmp54 = ((int8_t)local17);
int64_t const tmp55 = ((int64_t)tmp54);
int64_t const tmp56 = (tmp55+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp56);
int32_t const tmp57 = (local17+((int32_t)1));
int32_t const tmp58 = (local18-((int32_t)1));
local17 = tmp58;
local18 = tmp57;
tuple0 const tmp59 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp60 = (tmp59).e0;
int32_t const tmp61 = (tmp59).e1;
int64_t const tmp62 = ((int64_t)tmp60);
int64_t const tmp63 = ((int64_t)tmp61);
tuple16 const tmp64 = {.e0=tmp62,.e1=tmp63};
tuple16 local25 = tmp64;
int32_t const tmp65 = (++local17);
printf("%"PRIi32"\n",tmp65);
local19 = ((int32_t)2);
procedure2();
tuple4 const tmp66 = procedure4();
tuple3* const tmp67 = (tmp66).e0;
int32_t* const tmp68 = (tmp67)->e1;
int32_t const tmp69 = (*((tmp68)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp69);
int64_t const tmp70 = (local25).e1;
(local25).e0 = tmp70;
int64_t* const tmp71 = &((local25).e0);
printf("%p\n",(void*)tmp71);
tuple16* const tmp72 = &(local25);
tuple16* local26 = tmp72;
int64_t const tmp73 = ((local26)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp73);
int32_t const tmp74 = 1;
tuple11 const tmp75 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp76 = (tmp75).e0;
int64_t const tmp77 = (tmp75).e1;
concatInternal_checkArrayBounds(tmp74,tmp77);
int8_t const tmp78 = (*((tmp76)+(tmp74)));
printf("%"PRIi8"\n",tmp78);
tuple11 const tmp79 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp80 = (tmp79).e1;
printf("%"PRIi64"\n",tmp80);
bool const tmp81 = (local17==((int32_t)0));
int32_t tmp83;
if(tmp81){
int32_t const tmp82 = (((int32_t)1)+((int32_t)1));
tmp83 = tmp82;
}else{
int32_t const tmp84 = (((int32_t)1)+((int32_t)1));
bool const tmp85 = (local18!=((int32_t)0));
int32_t tmp88;
bool tmp87;
if(tmp85){
bool const tmp86 = (local17>=((int32_t)42));
tmp87 = tmp86;
tmp88 = tmp84;
}else{
tmp87 = ((bool)0);
tmp88 = tmp84;
}
if(tmp87){
int32_t const tmp89 = (tmp88+((int32_t)2));
tmp83 = tmp89;
}else{
int32_t const tmp90 = (tmp88+((int32_t)3));
tmp83 = tmp90;
}
}
printf("%"PRIi32"\n",tmp83);
int32_t* const tmp91 = &(local17);
int32_t* local27 = tmp91;
int32_t* const tmp92 = (++local27);
(*(tmp92)) = ((int32_t)2);
tuple11 const tmp93 = {.e0=stringChars2+0,.e1=5};
tuple13 const tmp94 = procedure13(tmp93);
tuple11 tmp100;
int64_t tmp99;
int8_t tmp98;
int64_t tmp95 = (tmp94).e1;
tuple11 tmp96 = (tmp94).e0;
do{
tuple14 const tmp97 = procedure15(tmp96,tmp95);
tmp98 = (tmp97).e2;
tmp99 = (tmp97).e1;
tmp100 = (tmp97).e0;
bool const tmp101 = (tmp97).e3;
if(!tmp101)
  break;
printf("%"PRIi8"\n",tmp98);
tmp95 = tmp99;
tmp96 = tmp100;
}while(1);
procedure16(tmp100,tmp99,tmp98);
int32_t tmp104;
bool tmp102 = 1;
int32_t tmp103 = 5;
do{
tmp104 = tmp103;
if(!tmp102)
  break;
printf("%"PRIi32"\n",tmp104);
int32_t const tmp105 = (tmp104-((int32_t)1));
bool const tmp106 = (tmp105>((int32_t)0));
tmp102 = tmp106;
tmp103 = tmp105;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
