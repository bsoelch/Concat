#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
typedef struct tuple0Impl tuple0;
typedef struct union1Impl union1;
typedef struct tuple2Impl tuple2;
typedef struct tuple3Impl tuple3;
typedef struct tuple4Impl tuple4;
typedef struct tuple5Impl tuple5;
typedef tuple0 (*procPtr0) (int32_t,int32_t);
typedef void (*procPtr1) (void);
typedef tuple2 (*procPtr2) (union1);
typedef tuple4 (*procPtr3) (void);
struct tuple0Impl{
int32_t e0;
int32_t e1;
};
struct union1Impl{
int32_t state;
union{
int32_t e0;
int64_t e1;
}value;
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
const int8_t* e0;
int64_t e1;
};
const int8_t stringChars0[] = {0x54,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x75,0x6e,0x69,0x63,0x6f,0x64,0x65,-0x1e,-0x7c,-0x5e,0x20,0x6d,0x75,0x6c,0x74,0x69,0x2d,0x6c,0x69,0x6e,0x65,0xa,0x20,0x73,0x74,0x72,0x69,0x6e,0x67};
const tuple5 string2 = {.e0=stringChars0+0,.e1=39};
const int8_t stringChars1[] = {0x53,0x74,0x72,0x69,0x6e,0x67,0x73,0x20,0x61,0x72,0x65,0x20,0x22,0x69,0x67,0x6e,0x6f,0x72,0x65,0x64,0x22,0x20,0x20};
const tuple5 string1 = {.e0=stringChars1+0,.e1=23};
const int8_t stringChars2[] = {0x48,0x65,0x6c,0x6c,0x6f};
const tuple5 string0 = {.e0=stringChars2+0,.e1=5};
void microLangInternal_checkArrayBounds(int64_t index,int64_t length){
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
tuple2 procedure3 (union1 arg0){
union1 local4 = arg0;
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
int32_t local5 = tmp4;
int32_t const tmp5 = (local5-((int32_t)1));
int32_t const tmp6 = (tmp5*((int32_t)3));
int32_t local6 = tmp6;
printf("%"PRIi32"\n",local5);
printf("%"PRIi32"\n",local6);
int32_t const tmp7 = (local5+((int32_t)1));
int32_t const tmp8 = (local6-((int32_t)1));
local5 = tmp8;
local6 = tmp7;
tuple0 const tmp9 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp10 = (tmp9).e0;
int32_t const tmp11 = (tmp9).e1;
int64_t const tmp12 = ((int64_t)tmp10);
int64_t const tmp13 = ((int64_t)tmp11);
tuple2 const tmp14 = (tuple2){.e0=tmp12,.e1=tmp13};
tuple2 local7 = tmp14;
int32_t const tmp15 = (++local5);
printf("%"PRIi32"\n",tmp15);
procedure2();
tuple4 const tmp16 = procedure4();
tuple3* const tmp17 = (tmp16).e0;
int32_t* const tmp18 = (tmp17)->e1;
int32_t const tmp19 = (*((tmp18)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp19);
int64_t const tmp20 = (local7).e1;
(local7).e0 = tmp20;
int64_t* const tmp21 = &((local7).e0);
printf("%p\n",(void*)tmp21);
tuple2* const tmp22 = &(local7);
tuple2* local8 = tmp22;
int64_t const tmp23 = ((local8)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp23);
int32_t const tmp24 = ((int32_t)1);
tuple5 const tmp25 = (string0);
const int8_t* const tmp26 = (tmp25).e0;
int64_t const tmp27 = (tmp25).e1;
microLangInternal_checkArrayBounds(tmp24,tmp27);
int8_t const tmp28 = (*((tmp26)+(tmp24)));
printf("%"PRIi8"\n",tmp28);
bool const tmp29 = (local5==((int32_t)0));
int32_t tmp31;
if(tmp29){
int32_t const tmp30 = (((int32_t)1)+((int32_t)1));
tmp31 = tmp30;
}else{
int32_t const tmp32 = (((int32_t)1)+((int32_t)1));
bool const tmp33 = (local6!=((int32_t)0));
int32_t tmp36;
bool tmp35;
if(tmp33){
bool const tmp34 = (local5>=((int32_t)42));
tmp35 = tmp34;
tmp36 = tmp32;
}else{
tmp35 = ((bool)0);
tmp36 = tmp32;
}
if(tmp35){
int32_t const tmp37 = (tmp36+((int32_t)2));
tmp31 = tmp37;
}else{
int32_t const tmp38 = (tmp36+((int32_t)3));
tmp31 = tmp38;
}
}
printf("%"PRIi32"\n",tmp31);
int32_t* const tmp39 = &(local5);
int32_t* local9 = tmp39;
int32_t* const tmp40 = (++local9);
(*(tmp40)) = ((int32_t)2);
int32_t local10 = ((int32_t)10);
do{
bool const tmp41 = (local10>((int32_t)0));
if(!tmp41)
  break;
printf("%"PRIi32"\n",local10);
int32_t const tmp42 = (local10-((int32_t)1));
local10 = tmp42;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple5 global5 = (string1);
int8_t global6 = ((int8_t)65);
tuple5 global7 = (string2);
