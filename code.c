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
typedef struct array0Impl array0;
typedef struct array1Impl array1;
typedef struct array2Impl array2;
typedef struct array3Impl array3;
typedef struct array4Impl array4;
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
int32_t label;
};
struct enum13Impl{
union{
int64_t const* e1;
} data;
int32_t label;
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
struct array0Impl{
int32_t data[3];
};
struct array1Impl{
int64_t data[10][8][6][4][2];
};
struct array2Impl{
int32_t const* data[2];
};
struct array3Impl{
int32_t* data;
int64_t const sizes[1];
};
struct array4Impl{
array3* data;
int64_t const sizes[1];
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
int32_t const global14 = 1;
int32_t const global15 = 0;
void procedure16 (void);
int64_t const global17 = 2;
void procedure18 (void);
void procedure19 (void);
void procedure20 (int32_t, enum11, int8_t, tuple14);
tuple18 procedure21 (tuple14, int64_t);
void procedure22 (tuple14, int64_t, int8_t);
tuple14 const global23 = {.e0=stringChars1+0,.e1=23};
int8_t global24 = 65;
tuple14 const global25 = {.e0=stringChars0+0,.e1=39};
tuple21 const global26 = {.e0=1,.e1=1};
tuple22 const global27 = {.e0={.e0=1,.e1=1},.e1=2};
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
void procedure16 (void){
printf("%"PRIi32"\n",global15);
printf("%"PRIi32"\n",global14);
printf("%"PRIi64"\n",global17);
}
void procedure18 (void){
int32_t const* local0;
array1 local1;
int32_t const** local2;
array4 local3;
}
void procedure19 (void){
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
void procedure20 (int32_t arg0, enum11 arg1, int8_t arg2, tuple14 arg3){
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
int32_t const tmp22 = (arg1).data.e2;
printf("%"PRIi32"\n",tmp22);
break;
case 3:;
concatInternal_checkEnumIndex(arg1.label,3);
tuple10 const tmp23 = (arg1).data.e3;
int8_t const tmp24 = (tmp23).e1;
printf("%"PRIi8"\n",tmp24);
break;
}
bool tmp25;
switch(arg2){
case 32:case 10:case 9:case 11:case 12:case 13:;
tmp25 = ((bool)1);
break;
default:
tmp25 = ((bool)0);
break;
}
bool const local0 = tmp25;
}
tuple18 procedure21 (tuple14 arg0, int64_t arg1){
int64_t const local0 = arg1;
tuple14 const local1 = arg0;
int64_t const tmp26 = (local1).e1;
bool const tmp27 = (local0>=tmp26);
if(tmp27){
return (tuple18){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp28 = (local0+((int64_t)1));
int64_t const tmp29 = local0;
tuple14 const tmp30 = local1;
int8_t const* const tmp31 = (tmp30).e0;
int64_t const tmp32 = (tmp30).e1;
concatInternal_checkArrayBounds(tmp29,tmp32);
int8_t const tmp33 = (*((tmp31)+(tmp29)));
return (tuple18){.e0=local1,.e1=tmp28,.e2=tmp33,.e3=((bool)1)};
}
void procedure22 (tuple14 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp34 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp34;
int32_t const tmp35 = (local0-((int32_t)1));
int32_t const tmp36 = (tmp35*((int32_t)3));
int32_t local1 = tmp36;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple8 local3;
tuple8* const tmp37 = &(local3);
tuple8 const* const tmp38 = ((tuple8 const*)tmp37);
tuple8 const tmp39 = {.e0=((int32_t)1),.e1=tmp38};
local3 = tmp39;
tuple8 const* const tmp40 = (local3).e1;
printf("%p\n",(void const*)tmp40);
tuple8 const** const tmp41 = &((local3).e1);
tuple8 const* const tmp42 = ((*(tmp41)))->e1;
printf("%p\n",(void const*)tmp42);
enum11 const tmp43 = {.label=2,.data={.e2=((int32_t)0)}};
enum11 local4 = tmp43;
enum11 const local5 = {.label=0,.data={0}};
int32_t const tmp44 = (local4).label;
bool const tmp45 = (tmp44==((int32_t)0));
printf("%s\n",tmp45?"true":"false");
int32_t const tmp46 = (local4).label;
bool const tmp47 = (tmp46==((int32_t)2));
printf("%s\n",tmp47?"true":"false");
concatInternal_checkEnumIndex(local4.label,2);
int32_t const tmp48 = (local4).data.e2;
printf("%"PRIi32"\n",tmp48);
tuple10 const tmp49 = {.e0=((int32_t)2),.e1=((int8_t)65)};
(local4).label = ((int32_t)3);
(local4).data.e3 = tmp49;
concatInternal_checkEnumIndex(local4.label,3);
tuple10* const tmp50 = &((local4).data.e3);
(tmp50)->e0 = ((int32_t)2);
int8_t const tmp51 = (tmp50)->e1;
printf("%"PRIi8"\n",tmp51);
enum12 local6 = /*enum*/0;
int32_t const tmp52 = ((int32_t)local6);
bool const tmp53 = (tmp52==((int32_t)1));
if(tmp53){
enum12 const tmp54 = /*enum*/2;
local6 = tmp54;
}
int64_t const tmp55 = procedure3();
printf("%"PRIi64"\n",tmp55);
procPtr9 const tmp56 = &(procedure0);
procPtr9 const local7 = tmp56;
tuple1 const tmp57 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp58 = (tmp57).e1;
printf("%"PRIi32"\n",tmp58);
int32_t const tmp59 = (tmp57).e0;
printf("%"PRIi32"\n",tmp59);
tuple9 const tmp60 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple9 const local8 = tmp60;
int32_t const tmp61 = (local8).e0;
int32_t const tmp62 = ((int32_t)(local8).e1);
int32_t const tmp63 = (tmp61+tmp62);
printf("%"PRIi32"\n",tmp63);
printf("%"PRIi32"\n",local0);
printf("%"PRIi32"\n",local1);
int8_t const tmp64 = ((int8_t)local0);
int64_t const tmp65 = ((int64_t)tmp64);
int64_t const tmp66 = (tmp65+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp66);
int32_t const tmp67 = (local0+((int32_t)1));
int32_t const tmp68 = (local1-((int32_t)1));
local0 = tmp68;
local1 = tmp67;
tuple1 const tmp69 = procedure0(((int32_t)1),((int32_t)2));
int64_t const tmp70 = ((int64_t)(tmp69).e0);
int64_t const tmp71 = ((int64_t)(tmp69).e1);
tuple20 const tmp72 = {.e0=tmp70,.e1=tmp71};
tuple20 local9 = tmp72;
int32_t const tmp73 = (++local0);
printf("%"PRIi32"\n",tmp73);
local2 = ((int32_t)2);
procedure2();
tuple6 const tmp74 = procedure4();
tuple5 const* const tmp75 = (tmp74).e0;
int32_t const* const tmp76 = (tmp75)->e1;
int32_t const tmp77 = (*((tmp76)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp77);
(local9).e0 = (local9).e1;
int64_t* const tmp78 = &((local9).e0);
printf("%p\n",(void const*)tmp78);
tuple20* const tmp79 = &(local9);
tuple20 const* const tmp80 = ((tuple20 const*)tmp79);
tuple20 const* const local10 = tmp80;
int64_t const tmp81 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp81);
int32_t const tmp82 = 1;
tuple14 const tmp83 = {.e0=stringChars2+0,.e1=12};
int8_t const* const tmp84 = (tmp83).e0;
int64_t const tmp85 = (tmp83).e1;
concatInternal_checkArrayBounds(tmp82,tmp85);
int8_t const tmp86 = (*((tmp84)+(tmp82)));
printf("%"PRIi8"\n",tmp86);
tuple14 const tmp87 = {.e0=stringChars2+6,.e1=5};
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
tuple14 const tmp101 = {.e0=stringChars2+0,.e1=5};
tuple16 const tmp102 = procedure13(tmp101);
tuple14 tmp108;
int64_t tmp107;
int8_t tmp106;
int64_t tmp103 = (tmp102).e1;
tuple14 tmp104 = (tmp102).e0;
do{
tuple18 const tmp105 = procedure21(tmp104,tmp103);
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
procedure22(tmp108,tmp107,tmp106);
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
global24 = ((int8_t)66);
}
