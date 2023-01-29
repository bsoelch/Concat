#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//internal declarations
typedef struct tuple0Impl tuple0;
typedef struct tuple4Impl tuple4;
typedef struct tuple5Impl tuple5;
typedef struct tuple7Impl tuple7;
typedef struct tuple8Impl tuple8;
typedef struct enum9Impl enum9;
typedef int32_t enum10;
typedef struct enum11Impl enum11;
typedef struct tuple12Impl tuple12;
typedef struct tuple14Impl tuple14;
typedef struct tuple16Impl tuple16;
typedef struct tuple18Impl tuple18;
typedef struct tuple19Impl tuple19;
typedef struct tuple20Impl tuple20;
typedef tuple0 (*procPtr0) (int32_t, int32_t);
typedef void (*procPtr1) (int32_t, int32_t);
typedef void (*procPtr2) (void);
typedef int64_t (*procPtr3) (void);
typedef tuple5 (*procPtr4) (void);
typedef tuple14 (*procPtr5) (tuple12);
typedef void (*procPtr6) (int32_t, enum9, tuple12);
typedef tuple16 (*procPtr7) (tuple12, int64_t);
typedef void (*procPtr8) (tuple12, int64_t, int8_t);
struct tuple0Impl{
int32_t e0;
int32_t e1;
};
struct tuple4Impl{
int32_t e0;
int32_t* e1;
};
struct tuple5Impl{
tuple4* e0;
int32_t e1;
};
struct tuple7Impl{
int32_t e0;
tuple7* e1;
};
struct tuple8Impl{
int32_t e0;
int8_t e1;
};
struct enum9Impl{
union{
int32_t e2;
int8_t e3;
} data;
int32_t const label;
};
struct enum11Impl{
union{
int64_t* e1;
} data;
int32_t const label;
};
struct tuple12Impl{
const int8_t* e0;
int64_t e1;
};
struct tuple14Impl{
tuple12 e0;
int64_t e1;
};
struct tuple16Impl{
tuple12 e0;
int64_t e1;
int8_t e2;
bool e3;
};
struct tuple18Impl{
int64_t e0;
int64_t e1;
};
struct tuple19Impl{
int32_t e0;
int64_t e1;
};
struct tuple20Impl{
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
tuple0 procedure0 (int32_t, int32_t);
void procedure1 (int32_t, int32_t);
void procedure2 (void);
int64_t procedure3 (void);
tuple5 procedure4 (void);
enum9 global10 = {.label=0,.data={0}};
enum9 global11 = {.label=1,.data={0}};
enum9 global12 = {.label=2,.data={.e2=2}};
tuple14 procedure13 (tuple12);
void procedure14 (void);
void procedure15 (int32_t, enum9, tuple12);
tuple16 procedure16 (tuple12, int64_t);
void procedure17 (tuple12, int64_t, int8_t);
tuple12 global18 = {.e0=stringChars1+0,.e1=23};
int8_t global19 = 65;
tuple12 global20 = {.e0=stringChars0+0,.e1=39};
tuple19 global21 = {.e0=1,.e1=1};
tuple20 global22 = {.e0={.e0=1,.e1=1},.e1=2};
//procedures code
tuple0 procedure0 (int32_t arg0, int32_t arg1){
procedure2();
return (tuple0){.e0=arg1,.e1=arg0};
}
void procedure1 (int32_t arg0, int32_t arg1){
}
void procedure2 (void){
return ;
}
int64_t procedure3 (void){
return ((int64_t)12345678987654321);
}
tuple5 procedure4 (void){
int32_t local0 = 0;
int32_t* const tmp0 = &(local0);
tuple4 const tmp1 = {.e0=((int32_t)0),.e1=tmp0};
tuple4 local1 = tmp1;
tuple4* const tmp2 = &(local1);
tuple5 const tmp3 = {.e0=tmp2,.e1=((int32_t)0)};
return tmp3;
}
tuple14 procedure13 (tuple12 arg0){
return (tuple14){.e0=arg0,.e1=((int64_t)0)};
}
void procedure14 (void){
int32_t tmp5;
int32_t tmp4 = 0;
do{
tmp5 = tmp4;
if(!((bool)1))
  break;
int32_t const tmp6 = (tmp5+((int32_t)1));
bool const tmp7 = (tmp6>((int32_t)5));
if(tmp7){
tmp5 = tmp6;
break;
}
tmp4 = tmp6;
}while(1);
printf("%"PRIi32"\n",tmp5);
int32_t tmp10;
int32_t tmp8 = 0;
do{
bool const tmp9 = (tmp8>((int32_t)5));
if(tmp9){
tmp10 = tmp8;
break;
}
tmp10 = tmp8;
if(!((bool)1))
  break;
int32_t const tmp11 = (tmp10+((int32_t)1));
tmp8 = tmp11;
}while(1);
printf("%"PRIi32"\n",tmp10);
int32_t tmp14;
bool tmp12 = 1;
int32_t tmp13 = 0;
do{
tmp14 = tmp13;
if(!tmp12)
  break;
int32_t const tmp15 = (tmp14+((int32_t)1));
bool const tmp16 = (tmp15<((int32_t)5));
if(tmp16){
tmp12 = ((bool)1);
tmp13 = tmp15;
continue;
}
tmp12 = ((bool)0);
tmp13 = tmp15;
}while(1);
printf("%"PRIi32"\n",tmp14);
int32_t tmp20;
int32_t tmp17 = 0;
do{
int32_t const tmp18 = (tmp17+((int32_t)1));
bool const tmp19 = (tmp18<((int32_t)5));
if(tmp19){
tmp17 = tmp18;
continue;
}
tmp20 = tmp18;
if(!((bool)0))
  break;
tmp17 = tmp20;
}while(1);
printf("%"PRIi32"\n",tmp20);
}
void procedure15 (int32_t arg0, enum9 arg1, tuple12 arg2){
