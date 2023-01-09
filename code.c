#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
typedef struct tuple0Impl tuple0;
typedef struct union1Impl union1;
typedef struct tuple2Impl tuple2;
typedef struct tuple3Impl tuple3;
typedef tuple0 (*procPtr0) (int32_t,int32_t);
typedef void (*procPtr1) (void);
typedef tuple2 (*procPtr2) (union1);
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
const int8_t* e0;
int64_t e1;
};
const int8_t stringChars0[] = {0x54,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x75,0x6e,0x69,0x63,0x6f,0x64,0x65,-0x1e,-0x7c,-0x5e,0x20,0x6d,0x75,0x6c,0x74,0x69,0x2d,0x6c,0x69,0x6e,0x65,0xa,0x20,0x73,0x74,0x72,0x69,0x6e,0x67};
const tuple3 string2 = {.e0=stringChars0+0,.e1=39};
const int8_t stringChars1[] = {0x53,0x74,0x72,0x69,0x6e,0x67,0x73,0x20,0x61,0x72,0x65,0x20,0x22,0x69,0x67,0x6e,0x6f,0x72,0x65,0x64,0x22,0x20,0x20};
const tuple3 string1 = {.e0=stringChars1+0,.e1=23};
const int8_t stringChars2[] = {0x48,0x65,0x6c,0x6c,0x6f};
const tuple3 string0 = {.e0=stringChars2+0,.e1=5};
void microLangInternal_checkArrayBounds(int64_t index,int64_t length){
  if(index>=0 && index<length)
    return;
  fprintf(stderr,"array index out of bounds: %"PRIi64" size: %"PRIi64"\n",index,length);
  exit(1);
}
tuple0 procedure1 (int32_t arg0, int32_t arg1){
return (tuple0){.e0=((int32_t)0),.e1=((int32_t)1)};
}
void procedure2 (void){
return ;
}
tuple2 procedure3 (union1 arg0){
return (tuple2){.e0=((int64_t)12345678987654321),.e1=((int64_t)98765432123456789)};
}
int main(void){
int32_t local4 = (((int32_t)1)+((int32_t)1));
int32_t local5 = ((local4-((int32_t)1))*((int32_t)3));
printf("%"PRIi32"\n",local4);
printf("%"PRIi32"\n",local5);
const int32_t tmp0 = (local4+((int32_t)1));
local4 = (local5-((int32_t)1));
local5 = tmp0;
const tuple0 tmp1 = procedure1(((int32_t)1),((int32_t)2));
tuple2 local6 = (tuple2){.e0=((int64_t)(tmp1).e0),.e1=((int64_t)(tmp1).e1)};
printf("%"PRIi32"\n",(++local4));
procedure2();
(local6).e0 = (local6).e1;
const int32_t tmp2 = ((int32_t)1);
const tuple3 tmp3 = (string0);
microLangInternal_checkArrayBounds(tmp2,(tmp3).e1);
printf("%"PRIi8"\n",(*(((tmp3).e0)+(tmp2))));
if((local4!=((int32_t)0))){
printf("%"PRIi32"\n",((int32_t)1));
}else{
if(((local5!=((int32_t)0))&&(local4>=((int32_t)42)))){
printf("%"PRIi32"\n",((int32_t)2));
}else{
printf("%"PRIi32"\n",((int32_t)3));
}
}
int32_t* local7 = &(local4);
(*((++local7))) = ((int32_t)2);
int32_t local8 = ((int32_t)10);
while((local8>((int32_t)0))){
printf("%"PRIi32"\n",local8);
local8 = (local8-((int32_t)1));
}
printf("%s\n",((bool)1)?"true":"false");
}
tuple3 global4 = (string1);
int8_t global5 = ((int8_t)65);
tuple3 global6 = (string2);
