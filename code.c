#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
typedef struct tuple0Impl tuple0;
typedef struct union0Impl union0;
typedef struct tuple1Impl tuple1;
typedef struct tuple2Impl tuple2;
typedef struct tuple3Impl tuple3;
typedef struct tuple4Impl tuple4;
typedef tuple0 (*procPtr0) (int32_t,int32_t);
typedef void (*procPtr1) (void);
typedef tuple1 (*procPtr2) (union0);
typedef tuple3 (*procPtr3) (void);
struct tuple0Impl{
int32_t e0;
int32_t e1;
};
struct union0Impl{
int32_t state;
union{
int32_t e0;
int32_t e1;
}value;
};
struct tuple1Impl{
int64_t e0;
int64_t e1;
};
struct tuple2Impl{
int32_t e0;
int32_t* e1;
};
struct tuple3Impl{
tuple2* e0;
int32_t e1;
};
struct tuple4Impl{
const int8_t* e0;
int64_t e1;
};
const int8_t stringChars0[] = {0x54,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x75,0x6e,0x69,0x63,0x6f,0x64,0x65,-0x1e,-0x7c,-0x5e,0x20,0x6d,0x75,0x6c,0x74,0x69,0x2d,0x6c,0x69,0x6e,0x65,0xa,0x20,0x73,0x74,0x72,0x69,0x6e,0x67};
const tuple4 string2 = {.e0=stringChars0+0,.e1=39};
const int8_t stringChars1[] = {0x53,0x74,0x72,0x69,0x6e,0x67,0x73,0x20,0x61,0x72,0x65,0x20,0x22,0x69,0x67,0x6e,0x6f,0x72,0x65,0x64,0x22,0x20,0x20};
const tuple4 string1 = {.e0=stringChars1+0,.e1=23};
const int8_t stringChars2[] = {0x48,0x65,0x6c,0x6c,0x6f};
const tuple4 string0 = {.e0=stringChars2+0,.e1=5};
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
tuple1 procedure3 (union0 arg0){
union0 local4 = arg0;
return (tuple1){.e0=((int64_t)12345678987654321),.e1=((int64_t)98765432123456789)};
}
tuple3 procedure4 (void){
int32_t local5 = ((int32_t)0);
int32_t* const tmp0 = &(local5);
tuple2 const tmp1 = (tuple2){.e0=((int32_t)0),.e1=tmp0};
tuple2 local6 = tmp1;
tuple2* const tmp2 = &(local6);
tuple3 const tmp3 = (tuple3){.e0=tmp2,.e1=((int32_t)0)};
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
int64_t const tmp7 = ((int64_t)local5);
int64_t const tmp8 = (tmp7+((int64_t)1000000000000000000));
printf("%"PRIi64"\n",tmp8);
int32_t const tmp9 = (local5+((int32_t)1));
int32_t const tmp10 = (local6-((int32_t)1));
local5 = tmp10;
local6 = tmp9;
tuple0 const tmp11 = procedure1(((int32_t)1),((int32_t)2));
int32_t const tmp12 = (tmp11).e0;
int32_t const tmp13 = (tmp11).e1;
int64_t const tmp14 = ((int64_t)tmp12);
int64_t const tmp15 = ((int64_t)tmp13);
tuple1 const tmp16 = (tuple1){.e0=tmp14,.e1=tmp15};
tuple1 local7 = tmp16;
int32_t const tmp17 = (++local5);
printf("%"PRIi32"\n",tmp17);
procedure2();
tuple3 const tmp18 = procedure4();
tuple2* const tmp19 = (tmp18).e0;
int32_t* const tmp20 = (tmp19)->e1;
int32_t const tmp21 = (*((tmp20)+(((int32_t)0))));
printf("%"PRIi32"\n",tmp21);
int64_t const tmp22 = (local7).e1;
(local7).e0 = tmp22;
int64_t* const tmp23 = &((local7).e0);
printf("%p\n",(void*)tmp23);
tuple1* const tmp24 = &(local7);
tuple1* local8 = tmp24;
int64_t const tmp25 = ((local8)+(((int32_t)0)))->e0;
printf("%"PRIi64"\n",tmp25);
int32_t const tmp26 = ((int32_t)1);
tuple4 const tmp27 = (string0);
const int8_t* const tmp28 = (tmp27).e0;
int64_t const tmp29 = (tmp27).e1;
microLangInternal_checkArrayBounds(tmp26,tmp29);
int8_t const tmp30 = (*((tmp28)+(tmp26)));
printf("%"PRIi8"\n",tmp30);
bool const tmp31 = (local5==((int32_t)0));
int32_t tmp33;
if(tmp31){
int32_t const tmp32 = (((int32_t)1)+((int32_t)1));
tmp33 = tmp32;
}else{
int32_t const tmp34 = (((int32_t)1)+((int32_t)1));
bool const tmp35 = (local6!=((int32_t)0));
int32_t tmp38;
bool tmp37;
if(tmp35){
bool const tmp36 = (local5>=((int32_t)42));
tmp37 = tmp36;
tmp38 = tmp34;
}else{
tmp37 = ((bool)0);
tmp38 = tmp34;
}
if(tmp37){
int32_t const tmp39 = (tmp38+((int32_t)2));
tmp33 = tmp39;
}else{
int32_t const tmp40 = (tmp38+((int32_t)3));
tmp33 = tmp40;
}
}
printf("%"PRIi32"\n",tmp33);
int32_t* const tmp41 = &(local5);
int32_t* local9 = tmp41;
int32_t* const tmp42 = (++local9);
(*(tmp42)) = ((int32_t)2);
int32_t local10 = ((int32_t)10);
do{
bool const tmp43 = (local10>((int32_t)0));
if(!tmp43)
  break;
printf("%"PRIi32"\n",local10);
int32_t const tmp44 = (local10-((int32_t)1));
local10 = tmp44;
}while(1);
printf("%s\n",((bool)1)?"true":"false");
}
tuple4 global5 = (string1);
int8_t global6 = ((int8_t)65);
tuple4 global7 = (string2);
