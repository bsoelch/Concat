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
tuple6 const tmp12 = (tuple6){.e0=((int32_t)0),.e1=((int8_t)65)};
tuple6 local12 = tmp12;
int32_t const tmp13 = (local12).e0;
int8_t const tmp14 = (local12).e1;
int32_t const tmp15 = ((int32_t)tmp14);
int32_t const tmp16 = (tmp13+tmp15);
printf("%"PRIi32"\n",tmp16);
printf("%"PRIi32"\n",local8);
printf("%"PRIi32"\n",local9);
int8_t const tmp17 = ((int8_t)local8);
int64_t const tmp18 = ((int64_t)tmp17);
int64_t const tmp19 = (tmp18+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp19);
int32_t const tmp20 = (local8+((int32_t)1));
int32_t const tmp21 = (local9-((int32_t)1));
local8 = tmp21;
local9 = tmp20;
tuple0 const tmp22 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp23 = (tmp22).e0;
int32_t const tmp24 = (tmp22).e1;
int64_t const tmp25 = ((int64_t)tmp23);
int64_t const tmp26 = ((int64_t)tmp24);
tuple2 const tmp27 = (tuple2){.e0=tmp25,.e1=tmp26};
tuple2 local13 = tmp27;
int32_t const tmp28 = (++local8);
printf("%"PRIi32"\n",tmp28);
local10 = ((int32_t)2);
procedure2();
tuple4 const tmp29 = procedure4();
tuple3* const tmp30 = (tmp29).e0;
int32_t* const tmp31 = (tmp30)->e1;
int32_t const tmp32 = (*((tmp31)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp32);
int64_t const tmp33 = (local13).e1;
(local13).e0 = tmp33;
int64_t* const tmp34 = &((local13).e0);
printf("%p\n",(void*)tmp34);
tuple2* const tmp35 = &(local13);
tuple2* local14 = tmp35;
int64_t const tmp36 = ((local14)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp36);
int32_t const tmp37 = ((int32_t)1);
tuple8 const tmp38 = (string0);
const int8_t* const tmp39 = (tmp38).e0;
int64_t const tmp40 = (tmp38).e1;
concatInternal_checkArrayBounds(tmp37,tmp40);
int8_t const tmp41 = (*((tmp39)+(tmp37)));
printf("%"PRIi8"\n",tmp41);
bool const tmp42 = (local8==((int32_t)0));
int32_t tmp44;
if(tmp42){
int32_t const tmp43 = (((int32_t)1)+((int32_t)1));
tmp44 = tmp43;
}else{
int32_t const tmp45 = (((int32_t)1)+((int32_t)1));
bool const tmp46 = (local9!=((int32_t)0));
int32_t tmp49;
bool tmp48;
if(tmp46){
bool const tmp47 = (local8>=((int32_t)42));
tmp48 = tmp47;
tmp49 = tmp45;
}else{
tmp48 = ((bool)0);
tmp49 = tmp45;
}
if(tmp48){
int32_t const tmp50 = (tmp49+((int32_t)2));
tmp44 = tmp50;
}else{
int32_t const tmp51 = (tmp49+((int32_t)3));
tmp44 = tmp51;
}
}
printf("%"PRIi32"\n",tmp44);
int32_t* const tmp52 = &(local8);
int32_t* local15 = tmp52;
int32_t* const tmp53 = (++local15);
(*(tmp53)) = ((int32_t)2);
int32_t local16 = ((int32_t)10);
do{
bool const tmp54 = (local16>((int32_t)0));
if(!tmp54)
  break;
printf("%"PRIi32"\n",local16);
int32_t const tmp55 = (local16-((int32_t)1));
local16 = tmp55;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple8 global8 = (string1);
int8_t global9 = ((int8_t)65);
tuple8 global10 = (string2);
