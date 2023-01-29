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
struct tuple0Impl{
int32_t e0;
int32_t e1;
};
struct tuple4Impl{
int32_t e0;
int32_t* e1;
};
struct tuple5Impl{
tuple4* e0;
int32_t e1;
};
struct tuple7Impl{
int32_t e0;
tuple7* e1;
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
int64_t* e1;
} data;
int32_t const label;
};
struct tuple12Impl{
const int8_t* e0;
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
int32_t* const tmp0 = &(local0);
tuple4 const tmp1 = {.e0=((int32_t)0),.e1=tmp0};
tuple4 local1 = tmp1;
tuple4* const tmp2 = &(local1);
tuple5 const tmp3 = {.e0=tmp2,.e1=((int32_t)0)};
return tmp3;
}
tuple14 procedure13 (tuple12 arg0){
return (tuple14){.e0=arg0,.e1=((int64_t)0)};
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
int32_t const tmp21 = (arg1).label;
switch(tmp21){
case 0:case 1:;
break;
case 2:;
enum9 const tmp22 = arg1;
concatInternal_checkEnumIndex(tmp22.label,2);
int32_t const tmp23 = (tmp22).data.e2;
printf("%"PRIi32"\n",tmp23);
break;
case 3:;
enum9 const tmp24 = arg1;
concatInternal_checkEnumIndex(tmp24.label,3);
int8_t const tmp25 = (tmp24).data.e3;
printf("%"PRIi8"\n",tmp25);
break;
}
bool tmp26;
switch(arg2){
case 32:case 10:case 9:case 11:case 12:case 13:;
tmp26 = ((bool)1);
break;
default:
tmp26 = ((bool)0);
break;
}
bool local0 = tmp26;
}
tuple16 procedure16 (tuple12 arg0, int64_t arg1){
int64_t local0 = arg1;
tuple12 local1 = arg0;
int64_t const tmp27 = (local1).e1;
bool const tmp28 = (local0>=tmp27);
if(tmp28){
return (tuple16){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp29 = (local0+((int64_t)1));
int64_t const tmp30 = local0;
tuple12 const tmp31 = local1;
const int8_t* const tmp32 = (tmp31).e0;
int64_t const tmp33 = (tmp31).e1;
concatInternal_checkArrayBounds(tmp30,tmp33);
int8_t const tmp34 = (*((tmp32)+(tmp30)));
return (tuple16){.e0=local1,.e1=tmp29,.e2=tmp34,.e3=((bool)1)};
}
void procedure17 (tuple12 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp35 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp35;
int32_t const tmp36 = (local0-((int32_t)1));
int32_t const tmp37 = (tmp36*((int32_t)3));
int32_t local1 = tmp37;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple7 local3;
tuple7* const tmp38 = &(local3);
tuple7 const tmp39 = {.e0=((int32_t)1),.e1=tmp38};
local3 = tmp39;
tuple7* const tmp40 = (local3).e1;
printf("%p\n",(void*)tmp40);
tuple7* const tmp41 = (local3).e1;
tuple7* const tmp42 = (tmp41)->e1;
printf("%p\n",(void*)tmp42);
enum9 const tmp43 = {.label=2,.data={.e2=((int32_t)0)}};
enum9 local4 = tmp43;
enum9 local5 = {.label=0,.data={0}};
int32_t const tmp44 = (local4).label;
bool const tmp45 = (tmp44==((int32_t)0));
printf("%s\n",tmp45?"true":"false");
int32_t const tmp46 = (local4).label;
bool const tmp47 = (tmp46==((int32_t)2));
printf("%s\n",tmp47?"true":"false");
enum9 const tmp48 = local4;
concatInternal_checkEnumIndex(tmp48.label,2);
int32_t const tmp49 = (tmp48).data.e2;
printf("%"PRIi32"\n",tmp49);
enum10 local6 = /*enum*/0;
int32_t const tmp50 = /*label*/local6;
bool const tmp51 = (tmp50==((int32_t)1));
if(tmp51){
enum10 const tmp52 = /*enum*/2;
local6 = tmp52;
}
int64_t const tmp53 = procedure3();
printf("%"PRIi64"\n",tmp53);
procPtr0 const tmp54 = &(procedure0);
procPtr0 local7 = tmp54;
tuple0 const tmp55 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp56 = (tmp55).e1;
printf("%"PRIi32"\n",tmp56);
int32_t const tmp57 = (tmp55).e0;
printf("%"PRIi32"\n",tmp57);
tuple8 const tmp58 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple8 local8 = tmp58;
int32_t const tmp59 = (local8).e0;
int8_t const tmp60 = (local8).e1;
int32_t const tmp61 = ((int32_t)tmp60);
int32_t const tmp62 = (tmp59+tmp61);
printf("%"PRIi32"\n",tmp62);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp63 = ((int8_t)local0);
int64_t const tmp64 = ((int64_t)tmp63);
int64_t const tmp65 = (tmp64+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp65);
int32_t const tmp66 = (local0+((int32_t)1));
int32_t const tmp67 = (local1-((int32_t)1));
local0 = tmp67;
local1 = tmp66;
tuple0 const tmp68 = procedure0(((int32_t)1),((int32_t)2));
int32_t const tmp69 = (tmp68).e0;
int32_t const tmp70 = (tmp68).e1;
int64_t const tmp71 = ((int64_t)tmp69);
int64_t const tmp72 = ((int64_t)tmp70);
tuple18 const tmp73 = {.e0=tmp71,.e1=tmp72};
tuple18 local9 = tmp73;
int32_t const tmp74 = (++local0);
printf("%"PRIi32"\n",tmp74);
local2 = ((int32_t)2);
procedure2();
tuple5 const tmp75 = procedure4();
tuple4* const tmp76 = (tmp75).e0;
int32_t* const tmp77 = (tmp76)->e1;
int32_t const tmp78 = (*((tmp77)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp78);
int64_t const tmp79 = (local9).e1;
(local9).e0 = tmp79;
int64_t* const tmp80 = &((local9).e0);
printf("%p\n",(void*)tmp80);
tuple18* const tmp81 = &(local9);
tuple18* local10 = tmp81;
int64_t const tmp82 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp82);
int32_t const tmp83 = 1;
tuple12 const tmp84 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp85 = (tmp84).e0;
int64_t const tmp86 = (tmp84).e1;
concatInternal_checkArrayBounds(tmp83,tmp86);
int8_t const tmp87 = (*((tmp85)+(tmp83)));
printf("%"PRIi8"\n",tmp87);
tuple12 const tmp88 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp89 = (tmp88).e1;
printf("%"PRIi64"\n",tmp89);
bool const tmp90 = (local0==((int32_t)0));
int32_t tmp92;
if(tmp90){
int32_t const tmp91 = (((int32_t)1)+((int32_t)1));
tmp92 = tmp91;
}else{
int32_t const tmp93 = (((int32_t)1)+((int32_t)1));
bool const tmp94 = (local1!=((int32_t)0));
int32_t tmp97;
bool tmp96;
if(tmp94){
bool const tmp95 = (local0>=((int32_t)42));
tmp96 = tmp95;
tmp97 = tmp93;
}else{
tmp96 = ((bool)0);
tmp97 = tmp93;
}
if(tmp96){
int32_t const tmp98 = (tmp97+((int32_t)2));
tmp92 = tmp98;
}else{
int32_t const tmp99 = (tmp97+((int32_t)3));
tmp92 = tmp99;
}
}
printf("%"PRIi32"\n",tmp92);
int32_t* const tmp100 = &(local0);
int32_t* local11 = tmp100;
int32_t* const tmp101 = (++local11);
(*(tmp101)) = ((int32_t)2);
tuple12 const tmp102 = {.e0=stringChars2+0,.e1=5};
tuple14 const tmp103 = procedure13(tmp102);
tuple12 tmp109;
int64_t tmp108;
int8_t tmp107;
int64_t tmp104 = (tmp103).e1;
tuple12 tmp105 = (tmp103).e0;
do{
tuple16 const tmp106 = procedure16(tmp105,tmp104);
tmp107 = (tmp106).e2;
tmp108 = (tmp106).e1;
tmp109 = (tmp106).e0;
bool const tmp110 = (tmp106).e3;
if(!tmp110)
  break;
printf("%"PRIi8"\n",tmp107);
tmp104 = tmp108;
tmp105 = tmp109;
}while(1);
procedure17(tmp109,tmp108,tmp107);
int32_t tmp113;
bool tmp111 = 1;
int32_t tmp112 = 5;
do{
tmp113 = tmp112;
if(!tmp111)
  break;
printf("%"PRIi32"\n",tmp113);
int32_t const tmp114 = (tmp113-((int32_t)1));
bool const tmp115 = (tmp114>((int32_t)0));
tmp111 = tmp115;
tmp112 = tmp114;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
