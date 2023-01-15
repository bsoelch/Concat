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
int32_t const tmp10 = ((int64_t)(tmp9).e0);
int32_t const tmp11 = ((int64_t)(tmp9).e1);
tuple2 const tmp12 = (tuple2){.e0=tmp10,.e1=tmp11};
tuple2 local7 = tmp12;
int32_t const tmp13 = (++local5);
printf("%"PRIi32"\n",tmp13);
procedure2();
tuple4 const tmp14 = procedure4();
tuple3* const tmp15 = (tmp14).e0;
int32_t* const tmp16 = (tmp15)->e1;
int32_t const tmp17 = (*((tmp16)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp17);
int64_t const tmp18 = (local7).e1;
(local7).e0 = tmp18;
int64_t* const tmp19 = &((local7).e0);
printf("%p\n",(void*)tmp19);
tuple2* const tmp20 = &(local7);
tuple2* local8 = tmp20;
int64_t const tmp21 = ((local8)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp21);
int32_t const tmp22 = ((int32_t)1);
tuple5 const tmp23 = (string0);
const int8_t* const tmp24 = (tmp23).e0;
int64_t const tmp25 = (tmp23).e1;
microLangInternal_checkArrayBounds(tmp22,tmp25);
int8_t const tmp26 = (*((tmp24)+(tmp22)));
printf("%"PRIi8"\n",tmp26);
bool const tmp27 = (local5!=((int32_t)0));
int32_t tmp29;
if(tmp27){
int32_t const tmp28 = (((int32_t)1)+((int32_t)1));
tmp29 = tmp28;
}else{
bool const tmp30 = (local6!=((int32_t)0));
bool const tmp31 = (local5>=((int32_t)42));
bool const tmp32 = (tmp30&tmp31);
if(tmp32){
int32_t const tmp33 = (((int32_t)1)+((int32_t)2));
tmp29 = tmp33;
}else{
int32_t const tmp34 = (((int32_t)1)+((int32_t)3));
tmp29 = tmp34;
}
}
printf("%"PRIi32"\n",tmp29);
int32_t* const tmp35 = &(local5);
int32_t* local9 = tmp35;
int32_t* const tmp36 = (++local9);
(*(tmp36)) = ((int32_t)2);
int32_t local10 = ((int32_t)10);
do{
bool const tmp37 = (local10>((int32_t)0));
if(!tmp37)
  break;
printf("%"PRIi32"\n",local10);
int32_t const tmp38 = (local10-((int32_t)1));
local10 = tmp38;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple5 global5 = (string1);
int8_t global6 = ((int8_t)65);
tuple5 global7 = (string2);
