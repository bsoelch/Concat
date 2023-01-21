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
typedef struct tuple8Impl tuple8;
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
struct tuple8Impl{
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
int32_t local8 = tmp4;
int32_t const tmp5 = (local8-((int32_t)1));
int32_t const tmp6 = (tmp5*((int32_t)3));
int32_t local9 = tmp6;
int32_t local10;
printf("%"PRIi32"\n",((int32_t)11001001));
tuple5 local11;
tuple5* const tmp7 = &(local11);
tuple5 const tmp8 = {.e0=((int32_t)1),.e1=tmp7};
local11 = tmp8;
tuple5* const tmp9 = (local11).e1;
printf("%p\n",(void*)tmp9);
tuple5* const tmp10 = (local11).e1;
tuple5* const tmp11 = (tmp10)->e1;
printf("%p\n",(void*)tmp11);
enum7 const tmp12 = {.label=2,.data={.e2=((int32_t)0)}};
enum7 local12 = tmp12;
enum7 local13 = {.label=0,.data={0}};
int32_t const tmp13 = (local12).label;
bool const tmp14 = (tmp13==((int32_t)0));
printf("%s\n",tmp14?"true":"false");
int32_t const tmp15 = (local12).label;
bool const tmp16 = (tmp15==((int32_t)2));
printf("%s\n",tmp16?"true":"false");
enum7 const tmp17 = local12;
concatInternal_checkEnumIndex(tmp17.label,2);
int32_t const tmp18 = (tmp17).data.e2;
printf("%"PRIi32"\n",tmp18);
procPtr0 local14 = &(procedure1);
tuple6 const tmp19 = {.e0=((int32_t)0),.e1=((int8_t)65)};
tuple6 local15 = tmp19;
int32_t const tmp20 = (local15).e0;
int8_t const tmp21 = (local15).e1;
int32_t const tmp22 = ((int32_t)tmp21);
int32_t const tmp23 = (tmp20+tmp22);
printf("%"PRIi32"\n",tmp23);
printf("%"PRIi32"\n",local8);
printf("%"PRIi32"\n",local9);
int8_t const tmp24 = ((int8_t)local8);
int64_t const tmp25 = ((int64_t)tmp24);
int64_t const tmp26 = (tmp25+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp26);
int32_t const tmp27 = (local8+((int32_t)1));
int32_t const tmp28 = (local9-((int32_t)1));
local8 = tmp28;
local9 = tmp27;
tuple0 const tmp29 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp30 = (tmp29).e0;
int32_t const tmp31 = (tmp29).e1;
int64_t const tmp32 = ((int64_t)tmp30);
int64_t const tmp33 = ((int64_t)tmp31);
tuple2 const tmp34 = {.e0=tmp32,.e1=tmp33};
tuple2 local16 = tmp34;
int32_t const tmp35 = (++local8);
printf("%"PRIi32"\n",tmp35);
local10 = ((int32_t)2);
procedure2();
tuple4 const tmp36 = procedure4();
tuple3* const tmp37 = (tmp36).e0;
int32_t* const tmp38 = (tmp37)->e1;
int32_t const tmp39 = (*((tmp38)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp39);
int64_t const tmp40 = (local16).e1;
(local16).e0 = tmp40;
int64_t* const tmp41 = &((local16).e0);
printf("%p\n",(void*)tmp41);
tuple2* const tmp42 = &(local16);
tuple2* local17 = tmp42;
int64_t const tmp43 = ((local17)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp43);
int32_t const tmp44 = 1;
tuple8 const tmp45 = {.e0=stringChars2+0,.e1=12};
const int8_t* const tmp46 = (tmp45).e0;
int64_t const tmp47 = (tmp45).e1;
concatInternal_checkArrayBounds(tmp44,tmp47);
int8_t const tmp48 = (*((tmp46)+(tmp44)));
printf("%"PRIi8"\n",tmp48);
tuple8 const tmp49 = {.e0=stringChars2+6,.e1=5};
int64_t const tmp50 = (tmp49).e1;
printf("%"PRIi64"\n",tmp50);
bool const tmp51 = (local8==((int32_t)0));
int32_t tmp53;
if(tmp51){
int32_t const tmp52 = (((int32_t)1)+((int32_t)1));
tmp53 = tmp52;
}else{
int32_t const tmp54 = (((int32_t)1)+((int32_t)1));
bool const tmp55 = (local9!=((int32_t)0));
int32_t tmp58;
bool tmp57;
if(tmp55){
bool const tmp56 = (local8>=((int32_t)42));
tmp57 = tmp56;
tmp58 = tmp54;
}else{
tmp57 = ((bool)0);
tmp58 = tmp54;
}
if(tmp57){
int32_t const tmp59 = (tmp58+((int32_t)2));
tmp53 = tmp59;
}else{
int32_t const tmp60 = (tmp58+((int32_t)3));
tmp53 = tmp60;
}
}
printf("%"PRIi32"\n",tmp53);
int32_t* const tmp61 = &(local8);
int32_t* local18 = tmp61;
int32_t* const tmp62 = (++local18);
(*(tmp62)) = ((int32_t)2);
int32_t local19 = 10;
do{
bool const tmp63 = (local19>((int32_t)0));
if(!tmp63)
  break;
printf("%"PRIi32"\n",local19);
int32_t const tmp64 = (local19-((int32_t)1));
local19 = tmp64;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple8 const tmp65 = {.e0=stringChars1+0,.e1=23};
tuple8 global8 = tmp65;
int8_t global9 = 65;
tuple8 const tmp66 = {.e0=stringChars0+0,.e1=39};
tuple8 global10 = tmp66;
