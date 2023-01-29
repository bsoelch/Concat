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
typedef void (*procPtr6) (int32_t, enum9, tuple12);
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
void procedure15 (int32_t, enum9, tuple12);
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
void procedure15 (int32_t arg0, enum9 arg1, tuple12 arg2){
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
}
tuple16 procedure16 (tuple12 arg0, int64_t arg1){
int64_t local0 = arg1;
tuple12 local1 = arg0;
int64_t const tmp26 = (local1).e1;
bool const tmp27 = (local0>=tmp26);
if(tmp27){
return (tuple16){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp28 = (local0+((int64_t)1));
int64_t const tmp29 = local0;
tuple12 const tmp30 = local1;
const int8_t* const tmp31 = (tmp30).e0;
int64_t const tmp32 = (tmp30).e1;
concatInternal_checkArrayBounds(tmp29,tmp32);
int8_t const tmp33 = (*((tmp31)+(tmp29)));
return (tuple16){.e0=local1,.e1=tmp28,.e2=tmp33,.e3=((bool)1)};
}
void procedure17 (tuple12 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp34 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp34;
int32_t const tmp35 = (local0-((int32_t)1));
int32_t const tmp36 = (tmp35*((int32_t)3));
int32_t local1 = tmp36;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple7 local3;
tuple7* const tmp37 = &(local3);
tuple7 const tmp38 = {.e0=((int32_t)1),.e1=tmp37};
local3 = tmp38;
tuple7* const tmp39 = (local3).e1;
printf("%p\n",(void*)tmp39);
tuple7* const tmp40 = (local3).e1;
tuple7* const tmp41 = (tmp40)->e1;
printf("%p\n",(void*)tmp41);
enum9 const tmp42 = {.label=2,.data={.e2=((int32_t)0)}};
enum9 local4 = tmp42;
enum9 local5 = {.label=0,.data={0}};
int32_t const tmp43 = (local4).label;
bool const tmp44 = (tmp43==((int32_t)0));
printf("%s\n",tmp44?"true":"false");
int32_t const tmp45 = (local4).label;
bool const tmp46 = (tmp45==((int32_t)2));
printf("%s\n",tmp46?"true":"false");
enum9 const tmp47 = local4;
concatInternal_checkEnumIndex(tmp47.label,2);
int32_t const tmp48 = (tmp47).data.e2;
printf("%"PRIi32"\n",tmp48);
enum10 local6 = /*enum*/0;
int32_t const tmp49 = /*label*/local6;
bool const tmp50 = (tmp49==((int32_t)1));
if(tmp50){
enum10 const tmp51 = /*enum*/2;
local6 = tmp51;
}
int64_t const tmp52 = procedure3();
printf("%"PRIi64"\n",tmp52);
procPtr0 const tmp53 = &(procedure0);
procPtr0 local7 = tmp53;
tuple0 const tmp54 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp55 = (tmp54).e1;
printf("%"PRIi32"\n",tmp55);
int32_t const tmp56 = (tmp54).e0;
printf("%"PRIi32"\n",tmp56);
tuple8 const tmp57 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple8 local8 = tmp57;
int32_t const tmp58 = (local8).e0;
int8_t const tmp59 = (local8).e1;
int32_t const tmp60 = ((int32_t)tmp59);
int32_t const tmp61 = (tmp58+tmp60);
printf("%"PRIi32"\n",tmp61);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp62 = ((int8_t)local0);
int64_t const tmp63 = ((int64_t)tmp62);
int64_t const tmp64 = (tmp63+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp64);
int32_t const tmp65 = (local0+((int32_t)1));
int32_t const tmp66 = (local1-((int32_t)1));
local0 = tmp66;
local1 = tmp65;
tuple0 const tmp67 = procedure0(((int32_t)1),((int32_t)2));
int32_t const tmp68 = (tmp67).e0;
int32_t const tmp69 = (tmp67).e1;
int64_t const tmp70 = ((int64_t)tmp68);
int64_t const tmp71 = ((int64_t)tmp69);
tuple18 const tmp72 = {.e0=tmp70,.e1=tmp71};
tuple18 local9 = tmp72;
int32_t const tmp73 = (++local0);
printf("%"PRIi32"\n",tmp73);
local2 = ((int32_t)2);
procedure2();
tuple5 const tmp74 = procedure4();
tuple4* const tmp75 = (tmp74).e0;
int32_t* const tmp76 = (tmp75)->e1;
int32_t const tmp77 = (*((tmp76)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp77);
int64_t const tmp78 = (local9).e1;
(local9).e0 = tmp78;
int64_t* const tmp79 = &((local9).e0);
printf("%p\n",(void*)tmp79);
tuple18* const tmp80 = &(local9);
tuple18* local10 = tmp80;
int64_t const tmp81 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp81);
int32_t const tmp82 = 1;
tuple12 const tmp83 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp84 = (tmp83).e0;
int64_t const tmp85 = (tmp83).e1;
concatInternal_checkArrayBounds(tmp82,tmp85);
int8_t const tmp86 = (*((tmp84)+(tmp82)));
printf("%"PRIi8"\n",tmp86);
tuple12 const tmp87 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp88 = (tmp87).e1;
printf("%"PRIi64"\n",tmp88);
bool const tmp89 = (local0==((int32_t)0));
int32_t tmp91;
if(tmp89){
int32_t const tmp90 = (((int32_t)1)+((int32_t)1));
tmp91 = tmp90;
}else{
int32_t const tmp92 = (((int32_t)1)+((int32_t)1));
bool const tmp93 = (local1!=((int32_t)0));
int32_t tmp96;
bool tmp95;
if(tmp93){
bool const tmp94 = (local0>=((int32_t)42));
tmp95 = tmp94;
tmp96 = tmp92;
}else{
tmp95 = ((bool)0);
tmp96 = tmp92;
}
if(tmp95){
int32_t const tmp97 = (tmp96+((int32_t)2));
tmp91 = tmp97;
}else{
int32_t const tmp98 = (tmp96+((int32_t)3));
tmp91 = tmp98;
}
}
printf("%"PRIi32"\n",tmp91);
int32_t* const tmp99 = &(local0);
int32_t* local11 = tmp99;
int32_t* const tmp100 = (++local11);
(*(tmp100)) = ((int32_t)2);
tuple12 const tmp101 = {.e0=stringChars2+0,.e1=5};
tuple14 const tmp102 = procedure13(tmp101);
tuple12 tmp108;
int64_t tmp107;
int8_t tmp106;
int64_t tmp103 = (tmp102).e1;
tuple12 tmp104 = (tmp102).e0;
do{
tuple16 const tmp105 = procedure16(tmp104,tmp103);
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
procedure17(tmp108,tmp107,tmp106);
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
}
