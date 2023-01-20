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
int32_t current;
};
struct tuple8Impl{
const int8_t* e0;
int64_t e1;
};
const int8_t stringChars0[40] = {0x54,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x75,0x6e,0x69,0x63,0x6f,0x64,0x65,-0x1e,-0x7c,-0x5e,0x20,0x6d,0x75,0x6c,0x74,0x69,0x2d,0x6c,0x69,0x6e,0x65,0xa,0x20,0x73,0x74,0x72,0x69,0x6e,0x67,0x00};
const tuple8 string2 = {.e0=stringChars0+0,.e1=39};
const int8_t stringChars1[24] = {0x53,0x74,0x72,0x69,0x6e,0x67,0x73,0x20,0x61,0x72,0x65,0x20,0x22,0x69,0x67,0x6e,0x6f,0x72,0x65,0x64,0x22,0x20,0x20,0x00};
const tuple8 string1 = {.e0=stringChars1+0,.e1=23};
const int8_t stringChars2[6] = {0x48,0x65,0x6c,0x6c,0x6f,0x00};
const tuple8 string0 = {.e0=stringChars2+0,.e1=5};
void concatInternal_checkArrayBounds(int64_t index,int64_t length){
  if(index>=0 && index<length)
    return;
  fprintf(stderr,"array index out of bounds: %"PRIi64" size: %"PRIi64"\n",index,length);
  exit(1);
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
int32_t local5 = ((int32_t)0);
int32_t* const tmp0 = &(local5);
tuple3 const tmp1 = (tuple3){.e0=((int32_t)0),.e1=tmp0};
tuple3 local6 = tmp1;
tuple3* const tmp2 = &(local6);
tuple4 const tmp3 = (tuple4){.e0=tmp2,.e1=((int32_t)0)};
return tmp3;
}
int main(void){
int32_t const tmp4 = (((int32_t)1)+((int32_t)1));
int32_t local8 = tmp4;
int32_t const tmp5 = (local8-((int32_t)1));
int32_t const tmp6 = (tmp5*((int32_t)3));
int32_t local9 = tmp6;
int32_t local10;
tuple5 local11;
tuple5* const tmp7 = &(local11);
tuple5 const tmp8 = (tuple5){.e0=((int32_t)1),.e1=tmp7};
local11 = tmp8;
tuple5* const tmp9 = (local11).e1;
printf("%p\n",(void*)tmp9);
tuple5* const tmp10 = (local11).e1;
tuple5* const tmp11 = (tmp10)->e1;
printf("%p\n",(void*)tmp11);
enum7 const tmp12 = (enum7){.current=2,.data={.e2=((int32_t)0)}};
enum7 local12 = tmp12;
tuple6 const tmp13 = (tuple6){.e0=((int32_t)0),.e1=((int8_t)65)};
tuple6 local13 = tmp13;
int32_t const tmp14 = (local13).e0;
int8_t const tmp15 = (local13).e1;
int32_t const tmp16 = ((int32_t)tmp15);
int32_t const tmp17 = (tmp14+tmp16);
printf("%"PRIi32"\n",tmp17);
printf("%"PRIi32"\n",local8);
printf("%"PRIi32"\n",local9);
int8_t const tmp18 = ((int8_t)local8);
int64_t const tmp19 = ((int64_t)tmp18);
int64_t const tmp20 = (tmp19+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp20);
int32_t const tmp21 = (local8+((int32_t)1));
int32_t const tmp22 = (local9-((int32_t)1));
local8 = tmp22;
local9 = tmp21;
tuple0 const tmp23 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp24 = (tmp23).e0;
int32_t const tmp25 = (tmp23).e1;
int64_t const tmp26 = ((int64_t)tmp24);
int64_t const tmp27 = ((int64_t)tmp25);
tuple2 const tmp28 = (tuple2){.e0=tmp26,.e1=tmp27};
tuple2 local14 = tmp28;
int32_t const tmp29 = (++local8);
printf("%"PRIi32"\n",tmp29);
local10 = ((int32_t)2);
procedure2();
tuple4 const tmp30 = procedure4();
tuple3* const tmp31 = (tmp30).e0;
int32_t* const tmp32 = (tmp31)->e1;
int32_t const tmp33 = (*((tmp32)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp33);
int64_t const tmp34 = (local14).e1;
(local14).e0 = tmp34;
int64_t* const tmp35 = &((local14).e0);
printf("%p\n",(void*)tmp35);
tuple2* const tmp36 = &(local14);
tuple2* local15 = tmp36;
int64_t const tmp37 = ((local15)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp37);
int32_t const tmp38 = ((int32_t)1);
tuple8 const tmp39 = (string0);
const int8_t* const tmp40 = (tmp39).e0;
int64_t const tmp41 = (tmp39).e1;
concatInternal_checkArrayBounds(tmp38,tmp41);
int8_t const tmp42 = (*((tmp40)+(tmp38)));
printf("%"PRIi8"\n",tmp42);
bool const tmp43 = (local8==((int32_t)0));
int32_t tmp45;
if(tmp43){
int32_t const tmp44 = (((int32_t)1)+((int32_t)1));
tmp45 = tmp44;
}else{
int32_t const tmp46 = (((int32_t)1)+((int32_t)1));
bool const tmp47 = (local9!=((int32_t)0));
int32_t tmp50;
bool tmp49;
if(tmp47){
bool const tmp48 = (local8>=((int32_t)42));
tmp49 = tmp48;
tmp50 = tmp46;
}else{
tmp49 = ((bool)0);
tmp50 = tmp46;
}
if(tmp49){
int32_t const tmp51 = (tmp50+((int32_t)2));
tmp45 = tmp51;
}else{
int32_t const tmp52 = (tmp50+((int32_t)3));
tmp45 = tmp52;
}
}
printf("%"PRIi32"\n",tmp45);
int32_t* const tmp53 = &(local8);
int32_t* local16 = tmp53;
int32_t* const tmp54 = (++local16);
(*(tmp54)) = ((int32_t)2);
int32_t local17 = ((int32_t)10);
do{
bool const tmp55 = (local17>((int32_t)0));
if(!tmp55)
  break;
printf("%"PRIi32"\n",local17);
int32_t const tmp56 = (local17-((int32_t)1));
local17 = tmp56;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple8 global8 = (string1);
int8_t global9 = ((int8_t)65);
tuple8 global10 = (string2);
