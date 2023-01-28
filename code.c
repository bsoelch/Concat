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
typedef struct tuple15Impl tuple15;
typedef struct tuple17Impl tuple17;
typedef struct tuple18Impl tuple18;
typedef struct tuple19Impl tuple19;
typedef tuple0 (*procPtr0) (int32_t, int32_t);
typedef tuple0 (*procPtr1) (int32_t, int32_t);
typedef void (*procPtr2) (void);
typedef int64_t (*procPtr3) (void);
typedef tuple5 (*procPtr4) (void);
typedef tuple14 (*procPtr5) (tuple12);
typedef tuple15 (*procPtr6) (tuple12, int64_t);
typedef void (*procPtr7) (tuple12, int64_t, int8_t);
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
struct tuple15Impl{
tuple12 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct tuple17Impl{
int64_t e0;
int64_t e1;
};
struct tuple18Impl{
int32_t e0;
int64_t e1;
};
struct tuple19Impl{
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
tuple0 procedure2 (int32_t, int32_t);
void procedure3 (void);
int64_t procedure4 (void);
tuple5 procedure5 (void);
enum9 global11 = {.label=0,.data={0}};
enum9 global12 = {.label=1,.data={0}};
enum9 global13 = {.label=2,.data={.e2=2}};
tuple14 procedure14 (tuple12);
void procedure15 (void);
tuple15 procedure16 (tuple12, int64_t);
void procedure17 (tuple12, int64_t, int8_t);
tuple12 global18 = {.e0=stringChars1+0,.e1=23};
int8_t global19 = 65;
tuple12 global20 = {.e0=stringChars0+0,.e1=39};
tuple18 global21 = {.e0=1,.e1=1};
tuple19 global22 = {.e0={.e0=1,.e1=1},.e1=2};
//procedures code
tuple0 procedure1 (int32_t arg0, int32_t arg1){
procedure3();
return (tuple0){.e0=arg0,.e1=arg1};
}
tuple0 procedure2 (int32_t arg0, int32_t arg1){
return (tuple0){.e0=arg0,.e1=arg1};
}
void procedure3 (void){
return ;
}
int64_t procedure4 (void){
return ((int64_t)12345678987654321);
}
tuple5 procedure5 (void){
int32_t local6 = 0;
int32_t* const tmp0 = &(local6);
tuple4 const tmp1 = {.e0=((int32_t)0),.e1=tmp0};
tuple4 local7 = tmp1;
tuple4* const tmp2 = &(local7);
tuple5 const tmp3 = {.e0=tmp2,.e1=((int32_t)0)};
return tmp3;
}
tuple14 procedure14 (tuple12 arg0){
return (tuple14){.e0=arg0,.e1=((int64_t)0)};
}
void procedure15 (void){
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
tuple15 procedure16 (tuple12 arg0, int64_t arg1){
int64_t local17 = arg1;
tuple12 local18 = arg0;
int64_t const tmp21 = (local18).e1;
bool const tmp22 = (local17>=tmp21);
if(tmp22){
return (tuple15){.e0=local18,.e1=local17,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp23 = (local17+((int64_t)1));
int64_t const tmp24 = local17;
tuple12 const tmp25 = local18;
const int8_t* const tmp26 = (tmp25).e0;
int64_t const tmp27 = (tmp25).e1;
concatInternal_checkArrayBounds(tmp24,tmp27);
int8_t const tmp28 = (*((tmp26)+(tmp24)));
return (tuple15){.e0=local18,.e1=tmp23,.e2=tmp28,.e3=((bool)1)};
}
void procedure17 (tuple12 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp29 = (((int32_t)1)+((int32_t)1));
int32_t local18 = tmp29;
int32_t const tmp30 = (local18-((int32_t)1));
int32_t const tmp31 = (tmp30*((int32_t)3));
int32_t local19 = tmp31;
int32_t local20;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple7 local21;
tuple7* const tmp32 = &(local21);
tuple7 const tmp33 = {.e0=((int32_t)1),.e1=tmp32};
local21 = tmp33;
tuple7* const tmp34 = (local21).e1;
printf("%p\n",(void*)tmp34);
tuple7* const tmp35 = (local21).e1;
tuple7* const tmp36 = (tmp35)->e1;
printf("%p\n",(void*)tmp36);
enum9 const tmp37 = {.label=2,.data={.e2=((int32_t)0)}};
enum9 local22 = tmp37;
enum9 local23 = {.label=0,.data={0}};
int32_t const tmp38 = (local22).label;
bool const tmp39 = (tmp38==((int32_t)0));
printf("%s\n",tmp39?"true":"false");
int32_t const tmp40 = (local22).label;
bool const tmp41 = (tmp40==((int32_t)2));
printf("%s\n",tmp41?"true":"false");
enum9 const tmp42 = local22;
concatInternal_checkEnumIndex(tmp42.label,2);
int32_t const tmp43 = (tmp42).data.e2;
printf("%"PRIi32"\n",tmp43);
enum10 local24 = /*enum*/0;
int32_t const tmp44 = /*label*/local24;
bool const tmp45 = (tmp44==((int32_t)1));
if(tmp45){
enum10 const tmp46 = /*enum*/2;
local24 = tmp46;
}
int64_t const tmp47 = procedure4();
printf("%"PRIi64"\n",tmp47);
procPtr0 const tmp48 = &(procedure1);
procPtr0 local25 = tmp48;
tuple0 const tmp49 = (local25)(((int32_t)1),((int32_t)1));
int32_t const tmp50 = (tmp49).e1;
printf("%"PRIi32"\n",tmp50);
int32_t const tmp51 = (tmp49).e0;
printf("%"PRIi32"\n",tmp51);
tuple8 const tmp52 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple8 local26 = tmp52;
int32_t const tmp53 = (local26).e0;
int8_t const tmp54 = (local26).e1;
int32_t const tmp55 = ((int32_t)tmp54);
int32_t const tmp56 = (tmp53+tmp55);
printf("%"PRIi32"\n",tmp56);
printf("%"PRIi32"\n",local18);
printf("%"PRIi32"\n",local19);
int8_t const tmp57 = ((int8_t)local18);
int64_t const tmp58 = ((int64_t)tmp57);
int64_t const tmp59 = (tmp58+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp59);
int32_t const tmp60 = (local18+((int32_t)1));
int32_t const tmp61 = (local19-((int32_t)1));
local18 = tmp61;
local19 = tmp60;
tuple0 const tmp62 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp63 = (tmp62).e0;
int32_t const tmp64 = (tmp62).e1;
int64_t const tmp65 = ((int64_t)tmp63);
int64_t const tmp66 = ((int64_t)tmp64);
tuple17 const tmp67 = {.e0=tmp65,.e1=tmp66};
tuple17 local27 = tmp67;
int32_t const tmp68 = (++local18);
printf("%"PRIi32"\n",tmp68);
local20 = ((int32_t)2);
procedure3();
tuple5 const tmp69 = procedure5();
tuple4* const tmp70 = (tmp69).e0;
int32_t* const tmp71 = (tmp70)->e1;
int32_t const tmp72 = (*((tmp71)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp72);
int64_t const tmp73 = (local27).e1;
(local27).e0 = tmp73;
int64_t* const tmp74 = &((local27).e0);
printf("%p\n",(void*)tmp74);
tuple17* const tmp75 = &(local27);
tuple17* local28 = tmp75;
int64_t const tmp76 = ((local28)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp76);
int32_t const tmp77 = 1;
tuple12 const tmp78 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp79 = (tmp78).e0;
int64_t const tmp80 = (tmp78).e1;
concatInternal_checkArrayBounds(tmp77,tmp80);
int8_t const tmp81 = (*((tmp79)+(tmp77)));
printf("%"PRIi8"\n",tmp81);
tuple12 const tmp82 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp83 = (tmp82).e1;
printf("%"PRIi64"\n",tmp83);
bool const tmp84 = (local18==((int32_t)0));
int32_t tmp86;
if(tmp84){
int32_t const tmp85 = (((int32_t)1)+((int32_t)1));
tmp86 = tmp85;
}else{
int32_t const tmp87 = (((int32_t)1)+((int32_t)1));
bool const tmp88 = (local19!=((int32_t)0));
int32_t tmp91;
bool tmp90;
if(tmp88){
bool const tmp89 = (local18>=((int32_t)42));
tmp90 = tmp89;
tmp91 = tmp87;
}else{
tmp90 = ((bool)0);
tmp91 = tmp87;
}
if(tmp90){
int32_t const tmp92 = (tmp91+((int32_t)2));
tmp86 = tmp92;
}else{
int32_t const tmp93 = (tmp91+((int32_t)3));
tmp86 = tmp93;
}
}
printf("%"PRIi32"\n",tmp86);
int32_t* const tmp94 = &(local18);
int32_t* local29 = tmp94;
int32_t* const tmp95 = (++local29);
(*(tmp95)) = ((int32_t)2);
tuple12 const tmp96 = {.e0=stringChars2+0,.e1=5};
tuple14 const tmp97 = procedure14(tmp96);
tuple12 tmp103;
int64_t tmp102;
int8_t tmp101;
int64_t tmp98 = (tmp97).e1;
tuple12 tmp99 = (tmp97).e0;
do{
tuple15 const tmp100 = procedure16(tmp99,tmp98);
tmp101 = (tmp100).e2;
tmp102 = (tmp100).e1;
tmp103 = (tmp100).e0;
bool const tmp104 = (tmp100).e3;
if(!tmp104)
  break;
printf("%"PRIi8"\n",tmp101);
tmp98 = tmp102;
tmp99 = tmp103;
}while(1);
procedure17(tmp103,tmp102,tmp101);
int32_t tmp107;
bool tmp105 = 1;
int32_t tmp106 = 5;
do{
tmp107 = tmp106;
if(!tmp105)
  break;
printf("%"PRIi32"\n",tmp107);
int32_t const tmp108 = (tmp107-((int32_t)1));
bool const tmp109 = (tmp108>((int32_t)0));
tmp105 = tmp109;
tmp106 = tmp108;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
