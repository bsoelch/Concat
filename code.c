#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple0Impl tuple0;
typedef struct tuple1Impl tuple1;
typedef struct tuple2Impl tuple2;
typedef struct tuple3Impl tuple3;
typedef struct tuple4Impl tuple4;
typedef struct tuple5Impl tuple5;
typedef struct tuple6Impl tuple6;
typedef struct enum7Impl enum7;
typedef struct enum8Impl enum8;
typedef struct enum9Impl enum9;
typedef struct tuple10Impl tuple10;
typedef struct tuple11Impl tuple11;
typedef struct tuple12Impl tuple12;
typedef struct tuple14Impl tuple14;
typedef struct tuple15Impl tuple15;
typedef tuple0 (*procPtr0) (int32_t,int32_t);
typedef void (*procPtr1) (void);
typedef tuple2 (*procPtr2) (tuple1);
typedef tuple4 (*procPtr3) (void);
typedef tuple11 (*procPtr4) (tuple10);
typedef tuple12 (*procPtr5) (tuple10,int64_t);
typedef void (*procPtr6) (tuple10,int64_t,int8_t);
struct tuple0Impl{
int32_t e0;
int32_t e1;
};
struct tuple1Impl{
int32_t e0;
};
struct tuple2Impl{
int64_t e0;
int64_t e1;
};
struct tuple3Impl{
int32_t e0;
int32_t* e1;
};
struct tuple4Impl{
tuple3* e0;
int32_t e1;
};
struct tuple5Impl{
int32_t e0;
tuple5* e1;
};
struct tuple6Impl{
int32_t e0;
int8_t e1;
};
struct enum7Impl{
union{
int32_t e2;
int8_t e3;
} data;
int32_t const label;
};
struct enum8Impl{
int32_t const label;
};
struct enum9Impl{
union{
int64_t* e1;
} data;
int32_t const label;
};
struct tuple10Impl{
const int8_t* e0;
int64_t e1;
};
struct tuple11Impl{
tuple10 e0;
int64_t e1;
};
struct tuple12Impl{
tuple10 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct tuple14Impl{
int32_t e0;
int64_t e1;
};
struct tuple15Impl{
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
tuple2 procedure3 (tuple1 arg0);
tuple4 procedure4 (void);
enum7 global10 = {.label=0,.data={0}};
enum7 global11 = {.label=1,.data={0}};
enum7 global12 = {.label=2,.data={.e2=2}};
tuple11 procedure13 (tuple10 arg0);
tuple12 procedure14 (tuple10, int64_t);
void procedure15 (tuple10, int64_t, int8_t);
tuple10 global16 = {.e0=stringChars1+0,.e1=23};
int8_t global17 = 65;
tuple10 global18 = {.e0=stringChars0+0,.e1=39};
tuple14 global19 = {.e0=1,.e1=1};
tuple15 global20 = {.e0={.e0=1,.e1=1},.e1=2};
//procedures code
tuple0 procedure1 (int32_t arg0, int32_t arg1){
procedure2();
return (tuple0){.e0=arg0,.e1=arg1};
}
void procedure2 (void){
return ;
}
tuple2 procedure3 (tuple1 arg0){
tuple1 local4 = arg0;
return (tuple2){.e0=((int64_t)12345678987654321),.e1=((int64_t)98765432123456789)};
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
tuple11 procedure13 (tuple10 arg0){
return (tuple11){.e0=arg0,.e1=((int64_t)0)};
}
tuple12 procedure14 (tuple10 arg0, int64_t arg1){
int64_t local15 = arg1;
tuple10 local16 = arg0;
int64_t const tmp4 = (local16).e1;
bool const tmp5 = (local15>=tmp4);
if(tmp5){
return (tuple12){.e0=local16,.e1=local15,.e2=((int8_t)0),.e3=((bool)0)};
}else{
int64_t const tmp6 = (local15+((int64_t)1));
int64_t const tmp7 = local15;
tuple10 const tmp8 = local16;
const int8_t* const tmp9 = (tmp8).e0;
int64_t const tmp10 = (tmp8).e1;
concatInternal_checkArrayBounds(tmp7,tmp10);
int8_t const tmp11 = (*((tmp9)+(tmp7)));
return (tuple12){.e0=local16,.e1=tmp6,.e2=tmp11,.e3=((bool)1)};
}
}
void procedure15 (tuple10 arg0, int64_t arg1, int8_t arg2){
int8_t local16 = arg2;
int64_t local17 = arg1;
tuple10 local18 = arg0;
return ;
}
int main(void){
int32_t const tmp12 = (((int32_t)1)+((int32_t)1));
int32_t local16 = tmp12;
int32_t const tmp13 = (local16-((int32_t)1));
int32_t const tmp14 = (tmp13*((int32_t)3));
int32_t local17 = tmp14;
int32_t local18;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple5 local19;
tuple5* const tmp15 = &(local19);
tuple5 const tmp16 = {.e0=((int32_t)1),.e1=tmp15};
local19 = tmp16;
tuple5* const tmp17 = (local19).e1;
printf("%p\n",(void*)tmp17);
tuple5* const tmp18 = (local19).e1;
tuple5* const tmp19 = (tmp18)->e1;
printf("%p\n",(void*)tmp19);
enum7 const tmp20 = {.label=2,.data={.e2=((int32_t)0)}};
enum7 local20 = tmp20;
enum7 local21 = {.label=0,.data={0}};
int32_t const tmp21 = (local20).label;
bool const tmp22 = (tmp21==((int32_t)0));
printf("%s\n",tmp22?"true":"false");
int32_t const tmp23 = (local20).label;
bool const tmp24 = (tmp23==((int32_t)2));
printf("%s\n",tmp24?"true":"false");
enum7 const tmp25 = local20;
concatInternal_checkEnumIndex(tmp25.label,2);
int32_t const tmp26 = (tmp25).data.e2;
printf("%"PRIi32"\n",tmp26);
procPtr0 const tmp27 = &(procedure1);
procPtr0 local22 = tmp27;
tuple0 const tmp28 = (local22)(((int32_t)1),((int32_t)1));
int32_t const tmp29 = (tmp28).e1;
printf("%"PRIi32"\n",tmp29);
int32_t const tmp30 = (tmp28).e0;
printf("%"PRIi32"\n",tmp30);
tuple6 const tmp31 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple6 local23 = tmp31;
int32_t const tmp32 = (local23).e0;
int8_t const tmp33 = (local23).e1;
int32_t const tmp34 = ((int32_t)tmp33);
int32_t const tmp35 = (tmp32+tmp34);
printf("%"PRIi32"\n",tmp35);
printf("%"PRIi32"\n",local16);
printf("%"PRIi32"\n",local17);
int8_t const tmp36 = ((int8_t)local16);
int64_t const tmp37 = ((int64_t)tmp36);
int64_t const tmp38 = (tmp37+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp38);
int32_t const tmp39 = (local16+((int32_t)1));
int32_t const tmp40 = (local17-((int32_t)1));
local16 = tmp40;
local17 = tmp39;
tuple0 const tmp41 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp42 = (tmp41).e0;
int32_t const tmp43 = (tmp41).e1;
int64_t const tmp44 = ((int64_t)tmp42);
int64_t const tmp45 = ((int64_t)tmp43);
tuple2 const tmp46 = {.e0=tmp44,.e1=tmp45};
tuple2 local24 = tmp46;
int32_t const tmp47 = (++local16);
printf("%"PRIi32"\n",tmp47);
local18 = ((int32_t)2);
procedure2();
tuple4 const tmp48 = procedure4();
tuple3* const tmp49 = (tmp48).e0;
int32_t* const tmp50 = (tmp49)->e1;
int32_t const tmp51 = (*((tmp50)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp51);
int64_t const tmp52 = (local24).e1;
(local24).e0 = tmp52;
int64_t* const tmp53 = &((local24).e0);
printf("%p\n",(void*)tmp53);
tuple2* const tmp54 = &(local24);
tuple2* local25 = tmp54;
int64_t const tmp55 = ((local25)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp55);
int32_t const tmp56 = 1;
tuple10 const tmp57 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp58 = (tmp57).e0;
int64_t const tmp59 = (tmp57).e1;
concatInternal_checkArrayBounds(tmp56,tmp59);
int8_t const tmp60 = (*((tmp58)+(tmp56)));
printf("%"PRIi8"\n",tmp60);
tuple10 const tmp61 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp62 = (tmp61).e1;
printf("%"PRIi64"\n",tmp62);
bool const tmp63 = (local16==((int32_t)0));
int32_t tmp65;
if(tmp63){
int32_t const tmp64 = (((int32_t)1)+((int32_t)1));
tmp65 = tmp64;
}else{
int32_t const tmp66 = (((int32_t)1)+((int32_t)1));
bool const tmp67 = (local17!=((int32_t)0));
int32_t tmp70;
bool tmp69;
if(tmp67){
bool const tmp68 = (local16>=((int32_t)42));
tmp69 = tmp68;
tmp70 = tmp66;
}else{
tmp69 = ((bool)0);
tmp70 = tmp66;
}
if(tmp69){
int32_t const tmp71 = (tmp70+((int32_t)2));
tmp65 = tmp71;
}else{
int32_t const tmp72 = (tmp70+((int32_t)3));
tmp65 = tmp72;
}
}
printf("%"PRIi32"\n",tmp65);
int32_t* const tmp73 = &(local16);
int32_t* local26 = tmp73;
int32_t* const tmp74 = (++local26);
(*(tmp74)) = ((int32_t)2);
int32_t local27 = 10;
tuple10 const tmp75 = {.e0=stringChars2+0,.e1=5};
tuple11 const tmp76 = procedure13(tmp75);
tuple10 tmp82;
int64_t tmp81;
int8_t tmp80;
int64_t tmp77 = (tmp76).e1;
tuple10 tmp78 = (tmp76).e0;
do{
tuple12 const tmp79 = procedure14(tmp78,tmp77);
tmp80 = (tmp79).e2;
tmp81 = (tmp79).e1;
tmp82 = (tmp79).e0;
bool const tmp83 = (tmp79).e3;
if(!tmp83)
  break;
printf("%"PRIi8"\n",tmp80);
tmp77 = tmp81;
tmp78 = tmp82;
}while(1);
procedure15(tmp82,tmp81,tmp80);
printf("%s\n",((bool)1)?"true":"false");
}
