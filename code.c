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
tuple4 const* const tmp2 = &(local1);
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
concatInternal_checkEnumIndex(arg1.label,2);
int32_t const tmp23 = (arg1).data.e2;
printf("%"PRIi32"\n",tmp23);
break;
case 3:;
concatInternal_checkEnumIndex(arg1.label,3);
int8_t const tmp25 = (arg1).data.e3;
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
int8_t const* const tmp32 = (tmp31).e0;
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
tuple7 const* const tmp39 = ((tuple7 const*)tmp38);
tuple7 const tmp40 = {.e0=((int32_t)1),.e1=tmp39};
local3 = tmp40;
tuple7 const* const tmp41 = (local3).e1;
printf("%p\n",(const void*)tmp41);
tuple7 const* const tmp42 = (local3).e1;
tuple7 const* const tmp43 = (tmp42)->e1;
printf("%p\n",(const void*)tmp43);
enum9 const tmp44 = {.label=2,.data={.e2=((int32_t)0)}};
enum9 local4 = tmp44;
enum9 local5 = {.label=0,.data={0}};
int32_t const tmp45 = (local4).label;
bool const tmp46 = (tmp45==((int32_t)0));
printf("%s\n",tmp46?"true":"false");
int32_t const tmp47 = (local4).label;
bool const tmp48 = (tmp47==((int32_t)2));
printf("%s\n",tmp48?"true":"false");
concatInternal_checkEnumIndex(local4.label,2);
int32_t const tmp50 = (local4).data.e2;
printf("%"PRIi32"\n",tmp50);
concatInternal_checkEnumIndex(local4.label,2);
(local4).data.e2 = ((int32_t)2);
concatInternal_checkEnumIndex(local4.label,2);
int32_t const tmp53 = (local4).data.e2;
printf("%"PRIi32"\n",tmp53);
enum10 local6 = /*enum*/0;
int32_t const tmp54 = /*label*/local6;
bool const tmp55 = (tmp54==((int32_t)1));
if(tmp55){
enum10 const tmp56 = /*enum*/2;
local6 = tmp56;
}
int64_t const tmp57 = procedure3();
printf("%"PRIi64"\n",tmp57);
procPtr9 const tmp58 = &(procedure0);
procPtr9 local7 = tmp58;
tuple0 const tmp59 = (local7)(((int32_t)1),((int32_t)1));
int32_t const tmp60 = (tmp59).e1;
printf("%"PRIi32"\n",tmp60);
int32_t const tmp61 = (tmp59).e0;
printf("%"PRIi32"\n",tmp61);
tuple8 const tmp62 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple8 local8 = tmp62;
int32_t const tmp63 = (local8).e0;
int8_t const tmp64 = (local8).e1;
int32_t const tmp65 = ((int32_t)tmp64);
int32_t const tmp66 = (tmp63+tmp65);
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
tuple0 const tmp72 = procedure0(((int32_t)1),((int32_t)2));
int32_t const tmp73 = (tmp72).e0;
int32_t const tmp74 = (tmp72).e1;
int64_t const tmp75 = ((int64_t)tmp73);
int64_t const tmp76 = ((int64_t)tmp74);
tuple18 const tmp77 = {.e0=tmp75,.e1=tmp76};
tuple18 local9 = tmp77;
int32_t const tmp78 = (++local0);
printf("%"PRIi32"\n",tmp78);
local2 = ((int32_t)2);
procedure2();
tuple5 const tmp79 = procedure4();
tuple4 const* const tmp80 = (tmp79).e0;
int32_t const* const tmp81 = (tmp80)->e1;
int32_t const tmp82 = (*((tmp81)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp82);
(local9).e0 = (local9).e1;
int64_t* const tmp83 = &((local9).e0);
printf("%p\n",(const void*)tmp83);
tuple18 const* const tmp84 = &(local9);
tuple18 const* local10 = tmp84;
int64_t const tmp85 = ((local10)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp85);
int32_t const tmp86 = 1;
tuple12 const tmp87 = {.e0=stringChars2+0,.e1=12};
int8_t const* const tmp88 = (tmp87).e0;
int64_t const tmp89 = (tmp87).e1;
concatInternal_checkArrayBounds(tmp86,tmp89);
int8_t const tmp90 = (*((tmp88)+(tmp86)));
printf("%"PRIi8"\n",tmp90);
tuple12 const tmp91 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp92 = (tmp91).e1;
printf("%"PRIi64"\n",tmp92);
bool const tmp93 = (local0==((int32_t)0));
int32_t tmp95;
if(tmp93){
int32_t const tmp94 = (((int32_t)1)+((int32_t)1));
tmp95 = tmp94;
}else{
int32_t const tmp96 = (((int32_t)1)+((int32_t)1));
bool const tmp97 = (local1!=((int32_t)0));
int32_t tmp100;
bool tmp99;
if(tmp97){
bool const tmp98 = (local0>=((int32_t)42));
tmp99 = tmp98;
tmp100 = tmp96;
}else{
tmp99 = ((bool)0);
tmp100 = tmp96;
}
if(tmp99){
int32_t const tmp101 = (tmp100+((int32_t)2));
tmp95 = tmp101;
}else{
int32_t const tmp102 = (tmp100+((int32_t)3));
tmp95 = tmp102;
}
}
printf("%"PRIi32"\n",tmp95);
int32_t* const tmp103 = &(local0);
int32_t* local11 = tmp103;
int32_t* const tmp104 = (++local11);
(*(tmp104)) = ((int32_t)2);
tuple12 const tmp105 = {.e0=stringChars2+0,.e1=5};
tuple14 const tmp106 = procedure13(tmp105);
tuple12 tmp112;
int64_t tmp111;
int8_t tmp110;
int64_t tmp107 = (tmp106).e1;
tuple12 tmp108 = (tmp106).e0;
do{
tuple16 const tmp109 = procedure16(tmp108,tmp107);
tmp110 = (tmp109).e2;
tmp111 = (tmp109).e1;
tmp112 = (tmp109).e0;
bool const tmp113 = (tmp109).e3;
if(!tmp113)
  break;
printf("%"PRIi8"\n",tmp110);
tmp107 = tmp111;
tmp108 = tmp112;
}while(1);
procedure17(tmp112,tmp111,tmp110);
int32_t tmp116;
bool tmp114 = 1;
int32_t tmp115 = 5;
do{
tmp116 = tmp115;
if(!tmp114)
  break;
printf("%"PRIi32"\n",tmp116);
int32_t const tmp117 = (tmp116-((int32_t)1));
bool const tmp118 = (tmp117>((int32_t)0));
tmp114 = tmp118;
tmp115 = tmp117;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
