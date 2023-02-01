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
const int32_t* e1;
};
struct tuple5Impl{
const tuple4* e0;
int32_t e1;
};
struct tuple7Impl{
int32_t e0;
const tuple7* e1;
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
const int64_t* e1;
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
const int32_t* const tmp1 = ((const int32_t*)tmp0);
tuple4 const tmp2 = {.e0=((int32_t)0),.e1=tmp1};
tuple4 local1 = tmp2;
tuple4* const tmp3 = &(local1);
const tuple4* const tmp4 = ((const tuple4*)tmp3);
tuple5 const tmp5 = {.e0=tmp4,.e1=((int32_t)0)};
return tmp5;
}
tuple14 procedure13 (tuple12 arg0){
return (tuple14){.e0=arg0,.e1=((int64_t)0)};
}
void procedure14 (void){
int32_t tmp7;
int32_t tmp6 = 0;
do{
tmp7 = tmp6;
if(!((bool)1))
  break;
int32_t const tmp8 = (tmp7+((int32_t)1));
bool const tmp9 = (tmp8>((int32_t)5));
if(tmp9){
tmp7 = tmp8;
break;
}
tmp6 = tmp8;
}while(1);
printf("%"PRIi32"\n",tmp7);
int32_t tmp12;
int32_t tmp10 = 0;
do{
bool const tmp11 = (tmp10>((int32_t)5));
if(tmp11){
tmp12 = tmp10;
break;
}
tmp12 = tmp10;
if(!((bool)1))
  break;
int32_t const tmp13 = (tmp12+((int32_t)1));
tmp10 = tmp13;
}while(1);
printf("%"PRIi32"\n",tmp12);
int32_t tmp16;
bool tmp14 = 1;
int32_t tmp15 = 0;
do{
tmp16 = tmp15;
if(!tmp14)
  break;
int32_t const tmp17 = (tmp16+((int32_t)1));
bool const tmp18 = (tmp17<((int32_t)5));
if(tmp18){
tmp14 = ((bool)1);
tmp15 = tmp17;
continue;
}
tmp14 = ((bool)0);
tmp15 = tmp17;
}while(1);
printf("%"PRIi32"\n",tmp16);
int32_t tmp22;
int32_t tmp19 = 0;
do{
int32_t const tmp20 = (tmp19+((int32_t)1));
bool const tmp21 = (tmp20<((int32_t)5));
if(tmp21){
tmp19 = tmp20;
continue;
}
tmp22 = tmp20;
if(!((bool)0))
  break;
tmp19 = tmp22;
}while(1);
printf("%"PRIi32"\n",tmp22);
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
int32_t const tmp23 = (arg1).label;
switch(tmp23){
case 0:case 1:;
break;
case 2:;
enum9 const tmp24 = arg1;
concatInternal_checkEnumIndex(tmp24.label,2);
int32_t const tmp25 = (tmp24).data.e2;
printf("%"PRIi32"\n",tmp25);
break;
case 3:;
enum9 const tmp26 = arg1;
concatInternal_checkEnumIndex(tmp26.label,3);
int8_t const tmp27 = (tmp26).data.e3;
printf("%"PRIi8"\n",tmp27);
break;
}
bool tmp28;
switch(arg2){
case 32:case 10:case 9:case 11:case 12:case 13:;
tmp28 = ((bool)1);
break;
default:
tmp28 = ((bool)0);
break;
}
bool local0 = tmp28;
}
tuple16 procedure16 (tuple12 arg0, int64_t arg1){
int64_t local0 = arg1;
tuple12 local1 = arg0;
int64_t const tmp29 = (local1).e1;
bool const tmp30 = (local0>=tmp29);
if(tmp30){
return (tuple16){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp31 = (local0+((int64_t)1));
int64_t const tmp32 = local0;
tuple12 const tmp33 = local1;
const int8_t* const tmp34 = (tmp33).e0;
int64_t const tmp35 = (tmp33).e1;
concatInternal_checkArrayBounds(tmp32,tmp35);
int8_t const tmp36 = (*((tmp34)+(tmp32)));
return (tuple16){.e0=local1,.e1=tmp31,.e2=tmp36,.e3=((bool)1)};
}
void procedure17 (tuple12 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp37 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp37;
int32_t const tmp38 = (local0-((int32_t)1));
int32_t const tmp39 = (tmp38*((int32_t)3));
int32_t local1 = tmp39;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple7 local3;
tuple7* const tmp40 = &(local3);
const tuple7* const tmp41 = ((const tuple7*)tmp40);
tuple7 const tmp42 = {.e0=((int32_t)1),.e1=tmp41};
local3 = tmp42;
const tuple7* const tmp43 = (local3).e1;
printf("%p\n",(const void*)tmp43);
const tuple7* const tmp44 = (local3).e1;
const tuple7* const tmp45 = (tmp44)->e1;
printf("%p\n",(const void*)tmp45);
enum9 const tmp46 = {.label=2,.data={.e2=((int32_t)0)}};
enum9 local4 = tmp46;
enum9 local5 = {.label=0,.data={0}};
int32_t const tmp47 = (local4).label;
bool const tmp48 = (tmp47==((int32_t)0));
printf("%s\n",tmp48?"true":"false");
int32_t const tmp49 = (local4).label;
bool const tmp50 = (tmp49==((int32_t)2));
printf("%s\n",tmp50?"true":"false");
enum9 const tmp51 = local4;
concatInternal_checkEnumIndex(tmp51.label,2);
int32_t const tmp52 = (tmp51).data.e2;
printf("%"PRIi32"\n",tmp52);
enum10 local6 = /*enum*/0;
int32_t const tmp53 = /*label*/local6;
bool const tmp54 = (tmp53==((int32_t)1));
if(tmp54){
enum10 const tmp55 = /*enum*/2;
local6 = tmp55;
}
int64_t const tmp56 = procedure3();
printf("%"PRIi64"\n",tmp56);
procPtr9 const tmp57 = &(procedure0);
procPtr9 local7 = tmp57;
tuple0 const tmp58 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp59 = (tmp58).e1;
printf("%"PRIi32"\n",tmp59);
int32_t const tmp60 = (tmp58).e0;
printf("%"PRIi32"\n",tmp60);
tuple8 const tmp61 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple8 local8 = tmp61;
int32_t const tmp62 = (local8).e0;
int8_t const tmp63 = (local8).e1;
int32_t const tmp64 = ((int32_t)tmp63);
int32_t const tmp65 = (tmp62+tmp64);
printf("%"PRIi32"\n",tmp65);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp66 = ((int8_t)local0);
int64_t const tmp67 = ((int64_t)tmp66);
int64_t const tmp68 = (tmp67+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp68);
int32_t const tmp69 = (local0+((int32_t)1));
int32_t const tmp70 = (local1-((int32_t)1));
local0 = tmp70;
local1 = tmp69;
tuple0 const tmp71 = procedure0(((int32_t)1),((int32_t)2));
int32_t const tmp72 = (tmp71).e0;
int32_t const tmp73 = (tmp71).e1;
int64_t const tmp74 = ((int64_t)tmp72);
int64_t const tmp75 = ((int64_t)tmp73);
tuple18 const tmp76 = {.e0=tmp74,.e1=tmp75};
tuple18 local9 = tmp76;
int32_t const tmp77 = (++local0);
printf("%"PRIi32"\n",tmp77);
local2 = ((int32_t)2);
procedure2();
tuple5 const tmp78 = procedure4();
const tuple4* const tmp79 = (tmp78).e0;
const int32_t* const tmp80 = (tmp79)->e1;
int32_t const tmp81 = (*((tmp80)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp81);
int64_t const tmp82 = (local9).e1;
(local9).e0 = tmp82;
int64_t* const tmp83 = &((local9).e0);
printf("%p\n",(const void*)tmp83);
tuple18* const tmp84 = &(local9);
const tuple18* const tmp85 = ((const tuple18*)tmp84);
const tuple18* local10 = tmp85;
int64_t const tmp86 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp86);
int32_t const tmp87 = 1;
tuple12 const tmp88 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp89 = (tmp88).e0;
int64_t const tmp90 = (tmp88).e1;
concatInternal_checkArrayBounds(tmp87,tmp90);
int8_t const tmp91 = (*((tmp89)+(tmp87)));
printf("%"PRIi8"\n",tmp91);
tuple12 const tmp92 = {.e0=stringChars2+6,.e1=5};
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
int32_t* local11 = tmp104;
int32_t* const tmp105 = (++local11);
(*(tmp105)) = ((int32_t)2);
tuple12 const tmp106 = {.e0=stringChars2+0,.e1=5};
tuple14 const tmp107 = procedure13(tmp106);
tuple12 tmp113;
int64_t tmp112;
int8_t tmp111;
int64_t tmp108 = (tmp107).e1;
tuple12 tmp109 = (tmp107).e0;
do{
tuple16 const tmp110 = procedure16(tmp109,tmp108);
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
}
