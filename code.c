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
int8_t* e0;
int64_t e1;
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
printf("%"PRIi32"\n",local8);
printf("%"PRIi32"\n",local9);
int64_t const tmp12 = ((int64_t)local8);
int64_t const tmp13 = (tmp12+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp13);
int32_t const tmp14 = (local8+((int32_t)1));
int32_t const tmp15 = (local9-((int32_t)1));
local8 = tmp15;
local9 = tmp14;
tuple0 const tmp16 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp17 = (tmp16).e0;
int32_t const tmp18 = (tmp16).e1;
int64_t const tmp19 = ((int64_t)tmp17);
int64_t const tmp20 = ((int64_t)tmp18);
tuple2 const tmp21 = (tuple2){.e0=tmp19,.e1=tmp20};
tuple2 local12 = tmp21;
int32_t const tmp22 = (++local8);
printf("%"PRIi32"\n",tmp22);
local10 = ((int32_t)2);
procedure2();
tuple4 const tmp23 = procedure4();
tuple3* const tmp24 = (tmp23).e0;
int32_t* const tmp25 = (tmp24)->e1;
int32_t const tmp26 = (*((tmp25)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp26);
int64_t const tmp27 = (local12).e1;
(local12).e0 = tmp27;
int64_t* const tmp28 = &((local12).e0);
printf("%p\n",(void*)tmp28);
tuple2* const tmp29 = &(local12);
tuple2* local13 = tmp29;
int64_t const tmp30 = ((local13)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp30);
int32_t const tmp31 = ((int32_t)1);
tuple8 const tmp32 = (string0);
const int8_t* const tmp33 = (tmp32).e0;
int64_t const tmp34 = (tmp32).e1;
concatInternal_checkArrayBounds(tmp31,tmp34);
int8_t const tmp35 = (*((tmp33)+(tmp31)));
printf("%"PRIi8"\n",tmp35);
bool const tmp36 = (local8==((int32_t)0));
int32_t tmp38;
if(tmp36){
int32_t const tmp37 = (((int32_t)1)+((int32_t)1));
tmp38 = tmp37;
}else{
int32_t const tmp39 = (((int32_t)1)+((int32_t)1));
bool const tmp40 = (local9!=((int32_t)0));
int32_t tmp43;
bool tmp42;
if(tmp40){
bool const tmp41 = (local8>=((int32_t)42));
tmp42 = tmp41;
tmp43 = tmp39;
}else{
tmp42 = ((bool)0);
tmp43 = tmp39;
}
if(tmp42){
int32_t const tmp44 = (tmp43+((int32_t)2));
tmp38 = tmp44;
}else{
int32_t const tmp45 = (tmp43+((int32_t)3));
tmp38 = tmp45;
}
}
printf("%"PRIi32"\n",tmp38);
int32_t* const tmp46 = &(local8);
int32_t* local14 = tmp46;
int32_t* const tmp47 = (++local14);
(*(tmp47)) = ((int32_t)2);
int32_t local15 = ((int32_t)10);
do{
bool const tmp48 = (local15>((int32_t)0));
if(!tmp48)
  break;
printf("%"PRIi32"\n",local15);
int32_t const tmp49 = (local15-((int32_t)1));
local15 = tmp49;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple8 global8 = (string1);
int8_t global9 = ((int8_t)65);
tuple8 global10 = (string2);
