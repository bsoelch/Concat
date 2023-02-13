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
typedef struct tuple17Impl tuple17;
typedef struct tuple20Impl tuple20;
typedef struct tuple22Impl tuple22;
typedef struct tuple23Impl tuple23;
typedef struct array0Impl array0;
typedef struct array1Impl array1;
typedef struct array2Impl array2;
typedef struct array3Impl array3;
typedef struct array4Impl array4;
typedef struct array5Impl array5;
typedef struct array6Impl array6;
typedef struct array7Impl array7;
typedef tuple1 (*procPtr0) (int32_t, int32_t);
typedef void (*procPtr1) (int32_t, int32_t);
typedef void (*procPtr2) (void);
typedef int64_t (*procPtr3) (void);
typedef tuple6 (*procPtr4) (void);
typedef tuple16 (*procPtr5) (tuple14);
typedef void (*procPtr6) (int32_t const*, array1, array2);
typedef void (*procPtr7) (int32_t, enum11, int8_t, tuple14);
typedef tuple20 (*procPtr8) (tuple14, int64_t);
typedef void (*procPtr9) (tuple14, int64_t, int8_t);
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
struct tuple14Impl{
int8_t const* e0;
int64_t e1;
};
struct tuple16Impl{
tuple14 e0;
int64_t e1;
};
struct tuple17Impl{
int32_t e0;
int64_t e1;
};
struct tuple20Impl{
tuple14 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct tuple22Impl{
int64_t e0;
int64_t e1;
};
struct tuple23Impl{
tuple1 e0;
int32_t e1;
};
struct array0Impl{
int32_t data[3];
};
struct array1Impl{
tuple17 data[2];
};
struct array2Impl{
int64_t data[3][2];
};
struct array3Impl{
int64_t data[10][8][6][4][2];
};
struct array4Impl{
int32_t const* data[2];
};
struct array5Impl{
int32_t* data;
int64_t const sizes[1];
};
struct array6Impl{
array5* data;
int64_t const sizes[1];
};
struct array7Impl{
int64_t data[2];
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
void procedure18 (int32_t const*, array1, array2);
void procedure19 (void);
void procedure20 (int32_t, enum11, int8_t, tuple14);
tuple20 procedure21 (tuple14, int64_t);
void procedure22 (tuple14, int64_t, int8_t);
tuple14 const global23 = {.e0=stringChars1+0,.e1=23};
int8_t global24 = 65;
tuple14 const global25 = {.e0=stringChars0+0,.e1=39};
tuple17 const global26 = {.e0=1,.e1=1};
tuple23 const global27 = {.e0={.e0=1,.e1=1},.e1=2};
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
void procedure18 (int32_t const* arg0, array1 arg1, array2 arg2){
int32_t const* local0;
array3 local1;
int32_t const** local2;
array6 local3;
printf("%"PRIi64"\n",/*length*/((int64_t)3));
concatInternal_checkArrayBounds(((int32_t)0),((int64_t)2));
int64_t const tmp4 = (((/*size*/(int64_t[]){2,3}))[((int32_t)0)]);
printf("%"PRIi64"\n",tmp4);
concatInternal_checkArrayBounds(((int32_t)0),((int64_t)3));
int32_t const tmp5 = ((arg0)[((int32_t)0)]);
concatInternal_checkArrayBounds(tmp5,((int64_t)3));
int32_t const tmp6 = ((arg0)[tmp5]);
printf("%"PRIi32"\n",tmp6);
concatInternal_checkArrayBounds(((int32_t)0),((int64_t)2));
((arg1).data[((int32_t)0)]).e0 = ((int32_t)1);
}
void procedure19 (void){
int32_t tmp8;
int32_t tmp7 = 0;
do{
tmp8 = tmp7;
if(!((bool)1))
  break;
int32_t const tmp9 = (tmp8+((int32_t)1));
bool const tmp10 = (tmp9>((int32_t)5));
if(tmp10){
tmp8 = tmp9;
break;
}
tmp7 = tmp9;
}while(1);
printf("%"PRIi32"\n",tmp8);
int32_t tmp13;
int32_t tmp11 = 0;
do{
bool const tmp12 = (tmp11>((int32_t)5));
if(tmp12){
tmp13 = tmp11;
break;
}
tmp13 = tmp11;
if(!((bool)1))
  break;
int32_t const tmp14 = (tmp13+((int32_t)1));
tmp11 = tmp14;
}while(1);
printf("%"PRIi32"\n",tmp13);
int32_t tmp17;
bool tmp15 = 1;
int32_t tmp16 = 0;
do{
tmp17 = tmp16;
if(!tmp15)
  break;
int32_t const tmp18 = (tmp17+((int32_t)1));
bool const tmp19 = (tmp18<((int32_t)5));
if(tmp19){
tmp15 = ((bool)1);
tmp16 = tmp18;
continue;
}
tmp15 = ((bool)0);
tmp16 = tmp18;
}while(1);
printf("%"PRIi32"\n",tmp17);
int32_t tmp23;
int32_t tmp20 = 0;
do{
int32_t const tmp21 = (tmp20+((int32_t)1));
bool const tmp22 = (tmp21<((int32_t)5));
if(tmp22){
tmp20 = tmp21;
continue;
}
tmp23 = tmp21;
if(!((bool)0))
  break;
tmp20 = tmp23;
}while(1);
printf("%"PRIi32"\n",tmp23);
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
int32_t const tmp24 = (arg1).label;
switch(tmp24){
case 0:case 1:;
break;
case 2:;
concatInternal_checkEnumIndex(arg1.label,2);
int32_t const tmp25 = (arg1).data.e2;
printf("%"PRIi32"\n",tmp25);
break;
case 3:;
concatInternal_checkEnumIndex(arg1.label,3);
tuple10 const tmp26 = (arg1).data.e3;
int8_t const tmp27 = (tmp26).e1;
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
bool const local0 = tmp28;
}
tuple20 procedure21 (tuple14 arg0, int64_t arg1){
int64_t const local0 = arg1;
tuple14 const local1 = arg0;
int64_t const tmp29 = (local1).e1;
bool const tmp30 = (local0>=tmp29);
if(tmp30){
return (tuple20){.e0=local1,.e1=local0,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp31 = (local0+((int64_t)1));
int64_t const tmp32 = local0;
tuple14 const tmp33 = local1;
int8_t const* const tmp34 = (tmp33).e0;
int64_t const tmp35 = (tmp33).e1;
concatInternal_checkArrayBounds(tmp32,tmp35);
int8_t const tmp36 = (*((tmp34)+(tmp32)));
return (tuple20){.e0=local1,.e1=tmp31,.e2=tmp36,.e3=((bool)1)};
}
void procedure22 (tuple14 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp37 = (((int32_t)1)+((int32_t)1));
int32_t local0 = tmp37;
int32_t const tmp38 = (local0-((int32_t)1));
int32_t const tmp39 = (tmp38*((int32_t)3));
int32_t local1 = tmp39;
int32_t local2;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple8 local3;
tuple8* const tmp40 = &(local3);
tuple8 const* const tmp41 = ((tuple8 const*)tmp40);
tuple8 const tmp42 = {.e0=((int32_t)1),.e1=tmp41};
local3 = tmp42;
tuple8 const* const tmp43 = (local3).e1;
printf("%p\n",(void const*)tmp43);
tuple8 const** const tmp44 = &((local3).e1);
tuple8 const* const tmp45 = ((*(tmp44)))->e1;
printf("%p\n",(void const*)tmp45);
enum11 const tmp46 = {.label=2,.data={.e2=((int32_t)0)}};
enum11 local4 = tmp46;
enum11 const local5 = {.label=0,.data={0}};
int32_t const tmp47 = (local4).label;
bool const tmp48 = (tmp47==((int32_t)0));
printf("%s\n",tmp48?"true":"false");
int32_t const tmp49 = (local4).label;
bool const tmp50 = (tmp49==((int32_t)2));
printf("%s\n",tmp50?"true":"false");
concatInternal_checkEnumIndex(local4.label,2);
int32_t const tmp51 = (local4).data.e2;
printf("%"PRIi32"\n",tmp51);
tuple10 const tmp52 = {.e0=((int32_t)2),.e1=((int8_t)65)};
(local4).label = ((int32_t)3);
(local4).data.e3 = tmp52;
concatInternal_checkEnumIndex(local4.label,3);
tuple10* const tmp53 = &((local4).data.e3);
(tmp53)->e0 = ((int32_t)2);
int8_t const tmp54 = (tmp53)->e1;
printf("%"PRIi8"\n",tmp54);
enum12 local6 = /*enum*/0;
int32_t const tmp55 = ((int32_t)local6);
bool const tmp56 = (tmp55==((int32_t)1));
if(tmp56){
enum12 const tmp57 = /*enum*/2;
local6 = tmp57;
}
int64_t const tmp58 = procedure3();
printf("%"PRIi64"\n",tmp58);
procPtr10 const tmp59 = &(procedure0);
procPtr10 const local7 = tmp59;
tuple1 const tmp60 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp61 = (tmp60).e1;
printf("%"PRIi32"\n",tmp61);
int32_t const tmp62 = (tmp60).e0;
printf("%"PRIi32"\n",tmp62);
tuple9 const tmp63 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple9 const local8 = tmp63;
int32_t const tmp64 = (local8).e0;
int32_t const tmp65 = ((int32_t)(local8).e1);
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
tuple1 const tmp72 = procedure0(((int32_t)1),((int32_t)2));
int64_t const tmp73 = ((int64_t)(tmp72).e0);
int64_t const tmp74 = ((int64_t)(tmp72).e1);
tuple22 const tmp75 = {.e0=tmp73,.e1=tmp74};
tuple22 local9 = tmp75;
int32_t const tmp76 = (++local0);
printf("%"PRIi32"\n",tmp76);
local2 = ((int32_t)2);
procedure2();
tuple6 const tmp77 = procedure4();
tuple5 const* const tmp78 = (tmp77).e0;
int32_t const* const tmp79 = (tmp78)->e1;
int32_t const tmp80 = (*((tmp79)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp80);
(local9).e0 = (local9).e1;
int64_t* const tmp81 = &((local9).e0);
printf("%p\n",(void const*)tmp81);
tuple22* const tmp82 = &(local9);
tuple22 const* const tmp83 = ((tuple22 const*)tmp82);
tuple22 const* const local10 = tmp83;
int64_t const tmp84 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp84);
int32_t const tmp85 = 1;
tuple14 const tmp86 = {.e0=stringChars2+0,.e1=12};
int8_t const* const tmp87 = (tmp86).e0;
int64_t const tmp88 = (tmp86).e1;
concatInternal_checkArrayBounds(tmp85,tmp88);
int8_t const tmp89 = (*((tmp87)+(tmp85)));
printf("%"PRIi8"\n",tmp89);
tuple14 const tmp90 = {.e0=stringChars2+6,.e1=5};
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
tuple14 const tmp104 = {.e0=stringChars2+0,.e1=5};
tuple16 const tmp105 = procedure13(tmp104);
tuple14 tmp111;
int64_t tmp110;
int8_t tmp109;
int64_t tmp106 = (tmp105).e1;
tuple14 tmp107 = (tmp105).e0;
do{
tuple20 const tmp108 = procedure21(tmp107,tmp106);
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
procedure22(tmp111,tmp110,tmp109);
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
global24 = ((int8_t)66);
}
