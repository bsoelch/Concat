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
typedef tuple0 (*procPtr0) (int32_t,int32_t);
typedef void (*procPtr1) (void);
typedef tuple2 (*procPtr2) (tuple1);
typedef tuple4 (*procPtr3) (void);
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
int32_t e0;
int64_t e1;
};
struct tuple12Impl{
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
tuple10 global13 = {.e0=stringChars1+0,.e1=23};
int8_t global14 = 65;
tuple10 global15 = {.e0=stringChars0+0,.e1=39};
tuple11 global16 = {.e0=1,.e1=1};
tuple12 global17 = {.e0={.e0=1,.e1=1},.e1=2};
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
int main(void){
int32_t const tmp4 = (((int32_t)1)+((int32_t)1));
int32_t local13 = tmp4;
int32_t const tmp5 = (local13-((int32_t)1));
int32_t const tmp6 = (tmp5*((int32_t)3));
int32_t local14 = tmp6;
int32_t local15;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple5 local16;
tuple5* const tmp7 = &(local16);
tuple5 const tmp8 = {.e0=((int32_t)1),.e1=tmp7};
local16 = tmp8;
tuple5* const tmp9 = (local16).e1;
printf("%p\n",(void*)tmp9);
tuple5* const tmp10 = (local16).e1;
tuple5* const tmp11 = (tmp10)->e1;
printf("%p\n",(void*)tmp11);
enum7 const tmp12 = {.label=2,.data={.e2=((int32_t)0)}};
enum7 local17 = tmp12;
enum7 local18 = {.label=0,.data={0}};
int32_t const tmp13 = (local17).label;
bool const tmp14 = (tmp13==((int32_t)0));
printf("%s\n",tmp14?"true":"false");
int32_t const tmp15 = (local17).label;
bool const tmp16 = (tmp15==((int32_t)2));
printf("%s\n",tmp16?"true":"false");
enum7 const tmp17 = local17;
concatInternal_checkEnumIndex(tmp17.label,2);
int32_t const tmp18 = (tmp17).data.e2;
printf("%"PRIi32"\n",tmp18);
procPtr0 const tmp19 = &(procedure1);
procPtr0 local19 = tmp19;
tuple0 const tmp20 = (local19)(((int32_t)1),((int32_t)1));
int32_t const tmp21 = (tmp20).e1;
printf("%"PRIi32"\n",tmp21);
int32_t const tmp22 = (tmp20).e0;
printf("%"PRIi32"\n",tmp22);
tuple6 const tmp23 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple6 local20 = tmp23;
int32_t const tmp24 = (local20).e0;
int8_t const tmp25 = (local20).e1;
int32_t const tmp26 = ((int32_t)tmp25);
int32_t const tmp27 = (tmp24+tmp26);
printf("%"PRIi32"\n",tmp27);
printf("%"PRIi32"\n",local13);
printf("%"PRIi32"\n",local14);
int8_t const tmp28 = ((int8_t)local13);
int64_t const tmp29 = ((int64_t)tmp28);
int64_t const tmp30 = (tmp29+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp30);
int32_t const tmp31 = (local13+((int32_t)1));
int32_t const tmp32 = (local14-((int32_t)1));
local13 = tmp32;
local14 = tmp31;
tuple0 const tmp33 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp34 = (tmp33).e0;
int32_t const tmp35 = (tmp33).e1;
int64_t const tmp36 = ((int64_t)tmp34);
int64_t const tmp37 = ((int64_t)tmp35);
tuple2 const tmp38 = {.e0=tmp36,.e1=tmp37};
tuple2 local21 = tmp38;
int32_t const tmp39 = (++local13);
printf("%"PRIi32"\n",tmp39);
local15 = ((int32_t)2);
procedure2();
tuple4 const tmp40 = procedure4();
tuple3* const tmp41 = (tmp40).e0;
int32_t* const tmp42 = (tmp41)->e1;
int32_t const tmp43 = (*((tmp42)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp43);
int64_t const tmp44 = (local21).e1;
(local21).e0 = tmp44;
int64_t* const tmp45 = &((local21).e0);
printf("%p\n",(void*)tmp45);
tuple2* const tmp46 = &(local21);
tuple2* local22 = tmp46;
int64_t const tmp47 = ((local22)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp47);
int32_t const tmp48 = 1;
tuple10 const tmp49 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp50 = (tmp49).e0;
int64_t const tmp51 = (tmp49).e1;
concatInternal_checkArrayBounds(tmp48,tmp51);
int8_t const tmp52 = (*((tmp50)+(tmp48)));
printf("%"PRIi8"\n",tmp52);
tuple10 const tmp53 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp54 = (tmp53).e1;
printf("%"PRIi64"\n",tmp54);
bool const tmp55 = (local13==((int32_t)0));
int32_t tmp57;
if(tmp55){
int32_t const tmp56 = (((int32_t)1)+((int32_t)1));
tmp57 = tmp56;
}else{
int32_t const tmp58 = (((int32_t)1)+((int32_t)1));
bool const tmp59 = (local14!=((int32_t)0));
int32_t tmp62;
bool tmp61;
if(tmp59){
bool const tmp60 = (local13>=((int32_t)42));
tmp61 = tmp60;
tmp62 = tmp58;
}else{
tmp61 = ((bool)0);
tmp62 = tmp58;
}
if(tmp61){
int32_t const tmp63 = (tmp62+((int32_t)2));
tmp57 = tmp63;
}else{
int32_t const tmp64 = (tmp62+((int32_t)3));
tmp57 = tmp64;
}
}
printf("%"PRIi32"\n",tmp57);
int32_t* const tmp65 = &(local13);
int32_t* local23 = tmp65;
int32_t* const tmp66 = (++local23);
(*(tmp66)) = ((int32_t)2);
int32_t local24 = 10;
do{
bool const tmp67 = (local24>((int32_t)0));
if(!tmp67)
  break;
printf("%"PRIi32"\n",local24);
int32_t const tmp68 = (local24-((int32_t)1));
local24 = tmp68;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
