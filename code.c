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
void procedure14 (void);
tuple12 procedure15 (tuple10, int64_t);
void procedure16 (tuple10, int64_t, int8_t);
tuple10 global17 = {.e0=stringChars1+0,.e1=23};
int8_t global18 = 65;
tuple10 global19 = {.e0=stringChars0+0,.e1=39};
tuple14 global20 = {.e0=1,.e1=1};
tuple15 global21 = {.e0={.e0=1,.e1=1},.e1=2};
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
tuple12 procedure15 (tuple10 arg0, int64_t arg1){
int64_t local16 = arg1;
tuple10 local17 = arg0;
int64_t const tmp21 = (local17).e1;
bool const tmp22 = (local16>=tmp21);
if(tmp22){
return (tuple12){.e0=local17,.e1=local16,.e2=((int8_t)0),.e3=((bool)0)};
}
int64_t const tmp23 = (local16+((int64_t)1));
int64_t const tmp24 = local16;
tuple10 const tmp25 = local17;
const int8_t* const tmp26 = (tmp25).e0;
int64_t const tmp27 = (tmp25).e1;
concatInternal_checkArrayBounds(tmp24,tmp27);
int8_t const tmp28 = (*((tmp26)+(tmp24)));
return (tuple12){.e0=local17,.e1=tmp23,.e2=tmp28,.e3=((bool)1)};
}
void procedure16 (tuple10 arg0, int64_t arg1, int8_t arg2){
}
int main(void){
int32_t const tmp29 = (((int32_t)1)+((int32_t)1));
int32_t local17 = tmp29;
int32_t const tmp30 = (local17-((int32_t)1));
int32_t const tmp31 = (tmp30*((int32_t)3));
int32_t local18 = tmp31;
int32_t local19;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple5 local20;
tuple5* const tmp32 = &(local20);
tuple5 const tmp33 = {.e0=((int32_t)1),.e1=tmp32};
local20 = tmp33;
tuple5* const tmp34 = (local20).e1;
printf("%p\n",(void*)tmp34);
tuple5* const tmp35 = (local20).e1;
tuple5* const tmp36 = (tmp35)->e1;
printf("%p\n",(void*)tmp36);
enum7 const tmp37 = {.label=2,.data={.e2=((int32_t)0)}};
enum7 local21 = tmp37;
enum7 local22 = {.label=0,.data={0}};
int32_t const tmp38 = (local21).label;
bool const tmp39 = (tmp38==((int32_t)0));
printf("%s\n",tmp39?"true":"false");
int32_t const tmp40 = (local21).label;
bool const tmp41 = (tmp40==((int32_t)2));
printf("%s\n",tmp41?"true":"false");
enum7 const tmp42 = local21;
concatInternal_checkEnumIndex(tmp42.label,2);
int32_t const tmp43 = (tmp42).data.e2;
printf("%"PRIi32"\n",tmp43);
procPtr0 const tmp44 = &(procedure1);
procPtr0 local23 = tmp44;
tuple0 const tmp45 = (local23)(((int32_t)1),((int32_t)1));
int32_t const tmp46 = (tmp45).e1;
printf("%"PRIi32"\n",tmp46);
int32_t const tmp47 = (tmp45).e0;
printf("%"PRIi32"\n",tmp47);
tuple6 const tmp48 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple6 local24 = tmp48;
int32_t const tmp49 = (local24).e0;
int8_t const tmp50 = (local24).e1;
int32_t const tmp51 = ((int32_t)tmp50);
int32_t const tmp52 = (tmp49+tmp51);
printf("%"PRIi32"\n",tmp52);
printf("%"PRIi32"\n",local17);
printf("%"PRIi32"\n",local18);
int8_t const tmp53 = ((int8_t)local17);
int64_t const tmp54 = ((int64_t)tmp53);
int64_t const tmp55 = (tmp54+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp55);
int32_t const tmp56 = (local17+((int32_t)1));
int32_t const tmp57 = (local18-((int32_t)1));
local17 = tmp57;
local18 = tmp56;
tuple0 const tmp58 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp59 = (tmp58).e0;
int32_t const tmp60 = (tmp58).e1;
int64_t const tmp61 = ((int64_t)tmp59);
int64_t const tmp62 = ((int64_t)tmp60);
tuple2 const tmp63 = {.e0=tmp61,.e1=tmp62};
tuple2 local25 = tmp63;
int32_t const tmp64 = (++local17);
printf("%"PRIi32"\n",tmp64);
local19 = ((int32_t)2);
procedure2();
tuple4 const tmp65 = procedure4();
tuple3* const tmp66 = (tmp65).e0;
int32_t* const tmp67 = (tmp66)->e1;
int32_t const tmp68 = (*((tmp67)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp68);
int64_t const tmp69 = (local25).e1;
(local25).e0 = tmp69;
int64_t* const tmp70 = &((local25).e0);
printf("%p\n",(void*)tmp70);
tuple2* const tmp71 = &(local25);
tuple2* local26 = tmp71;
int64_t const tmp72 = ((local26)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp72);
int32_t const tmp73 = 1;
tuple10 const tmp74 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp75 = (tmp74).e0;
int64_t const tmp76 = (tmp74).e1;
concatInternal_checkArrayBounds(tmp73,tmp76);
int8_t const tmp77 = (*((tmp75)+(tmp73)));
printf("%"PRIi8"\n",tmp77);
tuple10 const tmp78 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp79 = (tmp78).e1;
printf("%"PRIi64"\n",tmp79);
bool const tmp80 = (local17==((int32_t)0));
int32_t tmp82;
if(tmp80){
int32_t const tmp81 = (((int32_t)1)+((int32_t)1));
tmp82 = tmp81;
}else{
int32_t const tmp83 = (((int32_t)1)+((int32_t)1));
bool const tmp84 = (local18!=((int32_t)0));
int32_t tmp87;
bool tmp86;
if(tmp84){
bool const tmp85 = (local17>=((int32_t)42));
tmp86 = tmp85;
tmp87 = tmp83;
}else{
tmp86 = ((bool)0);
tmp87 = tmp83;
}
if(tmp86){
int32_t const tmp88 = (tmp87+((int32_t)2));
tmp82 = tmp88;
}else{
int32_t const tmp89 = (tmp87+((int32_t)3));
tmp82 = tmp89;
}
}
printf("%"PRIi32"\n",tmp82);
int32_t* const tmp90 = &(local17);
int32_t* local27 = tmp90;
int32_t* const tmp91 = (++local27);
(*(tmp91)) = ((int32_t)2);
tuple10 const tmp92 = {.e0=stringChars2+0,.e1=5};
tuple11 const tmp93 = procedure13(tmp92);
tuple10 tmp99;
int64_t tmp98;
int8_t tmp97;
int64_t tmp94 = (tmp93).e1;
tuple10 tmp95 = (tmp93).e0;
do{
tuple12 const tmp96 = procedure15(tmp95,tmp94);
tmp97 = (tmp96).e2;
tmp98 = (tmp96).e1;
tmp99 = (tmp96).e0;
bool const tmp100 = (tmp96).e3;
if(!tmp100)
  break;
printf("%"PRIi8"\n",tmp97);
tmp94 = tmp98;
tmp95 = tmp99;
}while(1);
procedure16(tmp99,tmp98,tmp97);
int32_t tmp103;
bool tmp101 = 1;
int32_t tmp102 = 5;
do{
tmp103 = tmp102;
if(!tmp101)
  break;
printf("%"PRIi32"\n",tmp103);
int32_t const tmp104 = (tmp103-((int32_t)1));
bool const tmp105 = (tmp104>((int32_t)0));
tmp101 = tmp105;
tmp102 = tmp104;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
