#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
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
tuple0 procedure1 (int32_t arg0, int32_t arg1){
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
int32_t local10 = tmp4;
int32_t const tmp5 = (local10-((int32_t)1));
int32_t const tmp6 = (tmp5*((int32_t)3));
int32_t local11 = tmp6;
int32_t local12;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple5 local13;
tuple5* const tmp7 = &(local13);
tuple5 const tmp8 = {.e0=((int32_t)1),.e1=tmp7};
local13 = tmp8;
tuple5* const tmp9 = (local13).e1;
printf("%p\n",(void*)tmp9);
tuple5* const tmp10 = (local13).e1;
tuple5* const tmp11 = (tmp10)->e1;
printf("%p\n",(void*)tmp11);
enum7 const tmp12 = {.label=2,.data={.e2=((int32_t)0)}};
enum7 local14 = tmp12;
enum7 local15 = {.label=0,.data={0}};
int32_t const tmp13 = (local14).label;
bool const tmp14 = (tmp13==((int32_t)0));
printf("%s\n",tmp14?"true":"false");
int32_t const tmp15 = (local14).label;
bool const tmp16 = (tmp15==((int32_t)2));
printf("%s\n",tmp16?"true":"false");
enum7 const tmp17 = local14;
concatInternal_checkEnumIndex(tmp17.label,2);
int32_t const tmp18 = (tmp17).data.e2;
printf("%"PRIi32"\n",tmp18);
procPtr0 local16 = &(procedure1);
tuple0 const tmp19 = (local16)(((int32_t)1),((int32_t)1));
int32_t const tmp20 = (tmp19).e1;
printf("%"PRIi32"\n",tmp20);
int32_t const tmp21 = (tmp19).e0;
printf("%"PRIi32"\n",tmp21);
tuple6 const tmp22 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple6 local17 = tmp22;
int32_t const tmp23 = (local17).e0;
int8_t const tmp24 = (local17).e1;
int32_t const tmp25 = ((int32_t)tmp24);
int32_t const tmp26 = (tmp23+tmp25);
printf("%"PRIi32"\n",tmp26);
printf("%"PRIi32"\n",local10);
printf("%"PRIi32"\n",local11);
int8_t const tmp27 = ((int8_t)local10);
int64_t const tmp28 = ((int64_t)tmp27);
int64_t const tmp29 = (tmp28+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp29);
int32_t const tmp30 = (local10+((int32_t)1));
int32_t const tmp31 = (local11-((int32_t)1));
local10 = tmp31;
local11 = tmp30;
tuple0 const tmp32 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp33 = (tmp32).e0;
int32_t const tmp34 = (tmp32).e1;
int64_t const tmp35 = ((int64_t)tmp33);
int64_t const tmp36 = ((int64_t)tmp34);
tuple2 const tmp37 = {.e0=tmp35,.e1=tmp36};
tuple2 local18 = tmp37;
int32_t const tmp38 = (++local10);
printf("%"PRIi32"\n",tmp38);
local12 = ((int32_t)2);
procedure2();
tuple4 const tmp39 = procedure4();
tuple3* const tmp40 = (tmp39).e0;
int32_t* const tmp41 = (tmp40)->e1;
int32_t const tmp42 = (*((tmp41)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp42);
int64_t const tmp43 = (local18).e1;
(local18).e0 = tmp43;
int64_t* const tmp44 = &((local18).e0);
printf("%p\n",(void*)tmp44);
tuple2* const tmp45 = &(local18);
tuple2* local19 = tmp45;
int64_t const tmp46 = ((local19)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp46);
int32_t const tmp47 = 1;
tuple10 const tmp48 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp49 = (tmp48).e0;
int64_t const tmp50 = (tmp48).e1;
concatInternal_checkArrayBounds(tmp47,tmp50);
int8_t const tmp51 = (*((tmp49)+(tmp47)));
printf("%"PRIi8"\n",tmp51);
tuple10 const tmp52 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp53 = (tmp52).e1;
printf("%"PRIi64"\n",tmp53);
bool const tmp54 = (local10==((int32_t)0));
int32_t tmp56;
if(tmp54){
int32_t const tmp55 = (((int32_t)1)+((int32_t)1));
tmp56 = tmp55;
}else{
int32_t const tmp57 = (((int32_t)1)+((int32_t)1));
bool const tmp58 = (local11!=((int32_t)0));
int32_t tmp61;
bool tmp60;
if(tmp58){
bool const tmp59 = (local10>=((int32_t)42));
tmp60 = tmp59;
tmp61 = tmp57;
}else{
tmp60 = ((bool)0);
tmp61 = tmp57;
}
if(tmp60){
int32_t const tmp62 = (tmp61+((int32_t)2));
tmp56 = tmp62;
}else{
int32_t const tmp63 = (tmp61+((int32_t)3));
tmp56 = tmp63;
}
}
printf("%"PRIi32"\n",tmp56);
int32_t* const tmp64 = &(local10);
int32_t* local20 = tmp64;
int32_t* const tmp65 = (++local20);
(*(tmp65)) = ((int32_t)2);
int32_t local21 = 10;
do{
bool const tmp66 = (local21>((int32_t)0));
if(!tmp66)
  break;
printf("%"PRIi32"\n",local21);
int32_t const tmp67 = (local21-((int32_t)1));
local21 = tmp67;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple10 const tmp68 = {.e0=stringChars1+0,.e1=23};
tuple10 global10 = tmp68;
int8_t global11 = 65;
tuple10 const tmp69 = {.e0=stringChars0+0,.e1=39};
tuple10 global12 = tmp69;
