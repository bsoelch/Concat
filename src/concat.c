#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "strings.h"

//errors in C code -> negative values
#define ERROR_MEMORY -1
#define ERROR_IO -2
#define ERROR_UNIMPLEMENTED -3
//errors in compiled code -> positive values
#define ERROR_TYPE 1
#define ERROR_SYNTAX 2 
#define ERROR_PARSE_INT 3
#define ERROR_INT_OVERFLOW 4
#define ERROR_REDECLARATION 5
#define WARNING_CODEPOINT_OUT_OF_RANGE 6
#define ERROR_EOF 8 //end of file


//exit codes for errors in compiled program
#define PROG_EXIT_CODE_ARRAY_OUT_OF_RANGE 1
#define PROG_EXIT_CODE_WRONG_ENUM_INDEX   2

// default initial capacity for arrays
#define INIT_CAP 128 

//negate indices (internal errors have negative error codes)
char const* const internalErrors [] = {[-ERROR_MEMORY]="ERROR_MEMORY",[-ERROR_IO]="ERROR_IO",[-ERROR_UNIMPLEMENTED]="ERROR_UNIMPLEMENTED",};
char const* const compilerErrors [] = {
[ERROR_TYPE]="type error",[ERROR_SYNTAX]="syntax error",[ERROR_PARSE_INT]="invalid character while parsing integer",[ERROR_INT_OVERFLOW]="integer exceeds maximum allowed value",
[ERROR_REDECLARATION]="redeclaration",[WARNING_CODEPOINT_OUT_OF_RANGE]="code-point out of range",
[ERROR_EOF]="unexpected end of file",};
char const* errorName(int errorCode){
  if(errorCode<-(int)(sizeof(internalErrors)/sizeof(char*))||errorCode>(int)(sizeof(compilerErrors)/sizeof(char*)))
    return "unknown error";
  if(errorCode<0){
    return internalErrors[-errorCode];
  }
  if(errorCode>0){
    return compilerErrors[errorCode];
  }
  return "no error";
}

bool allowWarnings=true;
typedef struct{
  char const* fileName;
  size_t line;
  size_t posInLine;
}FilePosition;
void printFilePosition(FilePosition pos,FILE* out){
  fprintf(out,"%s:%zu:%zu",pos.fileName,pos.line,pos.posInLine);
}

__attribute__((noreturn)) void handleError(char const* message,int errCode,FilePosition pos){
  if(message!=NULL){
    fputs(message,stderr);
    fputs("\n",stderr);
  }
  fprintf(stderr,"%s at ",errorName(errCode));
  printFilePosition(pos,stderr);
  fputs("\n",stderr);
  exit(EXIT_FAILURE);
}
void handleWarning(char const* message,int errCode,FilePosition pos){
  if(message!=NULL){
    fputs("Warning:\n  ",stderr);
    fputs(message,stderr);
    fputs("\n  ",stderr);
  }
  fprintf(stderr,"%s at ",errorName(errCode));
  printFilePosition(pos,stderr);
  fputs("\n",stderr);
  if(!allowWarnings)
    exit(EXIT_FAILURE);
}
typedef struct{
  bool isError;
  union{
    int64_t  i64;
    int    error;
  }as;
}IntOrErrorCode;


#define MAX_CODEPOINT 0x10FFFF
//writes an Unicode code-point to target
int writeUnicodeChar(int64_t codepoint,char* target){
  if(codepoint<(1<<7)){//0 *******
    *target=codepoint;
    return 1;
  }
  if(codepoint<(1<<11)){//110 ***** 10 ******
    *target=0xc0|(codepoint>>6);
    *(target+1)=0x80|(codepoint&0x3f);
    return 2;
  }
  if(codepoint<(1<<16)){//1110 **** 10 ****** 10 ******
    *target=0xe0|(codepoint>>12);
    *(target+1)=0x80|((codepoint>>6)&0x3f);
    *(target+2)=0x80|(codepoint&0x3f);
    return 3;
  }
  if(codepoint<(1<<21)){//11110 *** 10 ****** 10 ****** 10 ******
    *target=0xf0|(codepoint>>18);
    *(target+1)=0x80|((codepoint>>12)&0x3f);
    *(target+2)=0x80|((codepoint>>6)&0x3f);
    *(target+3)=0x80|(codepoint&0x3f);
    return 4;
  }
  //not valid Unicode code-points but still valid sequences following the UTF-8 encoding scheme 
  if(codepoint<(1<<26)){//111110 ** 10 ****** 10 ****** 10 ****** 10 ******
    *target=0xf8|(codepoint>>24);
    *(target+1)=0x80|((codepoint>>18)&0x3f);
    *(target+2)=0x80|((codepoint>>12)&0x3f);
    *(target+3)=0x80|((codepoint>>6)&0x3f);
    *(target+4)=0x80|(codepoint&0x3f);
    return 5;
  }
  if(codepoint<(1LL<<32)){//1111110 * 10 ****** 10 ****** 10 ****** 10 ****** 10 ******
    *target=0xfc|(codepoint>>30);
    *(target+1)=0x80|((codepoint>>24)&0x3f);
    *(target+2)=0x80|((codepoint>>18)&0x3f);
    *(target+3)=0x80|((codepoint>>12)&0x3f);
    *(target+4)=0x80|((codepoint>>6)&0x3f);
    *(target+5)=0x80|(codepoint&0x3f);
    return 6;
  }
  if(codepoint<(1LL<<37)){//11111110 10 ****** 10 ****** 10 ****** 10 ****** 10 ****** 10 ******
    *target=0xfe|(codepoint>>36);
    *(target+1)=0x80|((codepoint>>30)&0x3f);
    *(target+2)=0x80|((codepoint>>24)&0x3f);
    *(target+3)=0x80|((codepoint>>18)&0x3f);
    *(target+4)=0x80|((codepoint>>12)&0x3f);
    *(target+5)=0x80|((codepoint>>6)&0x3f);
    *(target+6)=0x80|(codepoint&0x3f);
    return 7;
  }
  //invalid code-point
  return 0;
}

typedef enum{
  OP_PRINT,
  OP_CONSTANT,
  
  OP_PRE_DECLARE,
  OP_DECLARE,
  OP_GET,
  OP_SET,
  OP_GET_LABEL,
  OP_SET_LABEL,
  
  OP_IDENTIFIER,
  OP_SET_IDENTIFIER,
  OP_IDENTIFIER_ADDRESS,

  OP_NEW,
  OP_CAST,
  OP_ADDR_OF,// (pointer to given value)
  
  OP_BINARY_OPERATOR, 
  OP_UNARY_OPERATOR,  
  
  OP_CHECK_ARRAY_BOUNDS,//special operation for checking array bounds             params: index length            exits the program if index < 0 or index >= length
  OP_CHECK_ENUM_INDEX,//special operation for checking if enum index corresponds to current value  params: enum   exits the program if enum.lable != data.asI64 
  
  OP_CODE_BLOCK,  
  OP_END_BLOCK,  
  
  OP_RETURN,       
  OP_CALL,         // procType procId  
  OP_CALL_PTR,    
  ENTRY_POINT,     //entry point of the program, starts the main code section, section will close at the matching BLOCK_END 
  
  //compile-time operations
  OP_MODIFY_STACK,  
  OP_COMPILER_INFO,
}OpType;
char const* opName(OpType type){
  switch(type){
    case OP_PRINT:return "OP_PRINT";
    case OP_CONSTANT:return "OP_CONSTANT";
    case OP_DECLARE:return "OP_DECLARE";
    case OP_PRE_DECLARE:return "OP_PRE_DECLARE";
    case OP_GET:return "OP_GET";
    case OP_GET_LABEL:return "OP_GET_LABEL";
    case OP_SET:return "OP_SET";
    case OP_SET_LABEL:return "OP_SET_LABEL";
    case OP_IDENTIFIER:return "OP_IDENTIFIER";
    case OP_SET_IDENTIFIER:return "OP_SET_IDENTIFIER";
    case OP_IDENTIFIER_ADDRESS:return "OP_IDENTIFIER_ADDRESS";
    case OP_BINARY_OPERATOR:return "OP_BINARY_OPERATOR";
    case OP_UNARY_OPERATOR:return "OP_UNARY_OPERATOR";  
    case OP_CODE_BLOCK:return "OP_CODE_BLOCK";
    case OP_END_BLOCK:return "OP_END_BLOCK";
    case OP_RETURN:return "OP_RETURN";      
    case OP_CALL:return "OP_CALL";
    case OP_CALL_PTR:return "OP_CALL_PTR";
    case ENTRY_POINT:return "ENTRY_POINT";
    case OP_NEW:return "OP_NEW";
    case OP_CAST:return "OP_CAST";
    case OP_ADDR_OF:return "OP_ADDR_OF";
    case OP_CHECK_ARRAY_BOUNDS:return "OP_CHECK_ARRAY_BOUNDS";
    case OP_CHECK_ENUM_INDEX:return "OP_CHECK_ENUM_INDEX";
    case OP_MODIFY_STACK:return "OP_MODIFY_STACK";
    case OP_COMPILER_INFO:return "OP_COMPILER_INFO";
  }
  return "UNDEFINED";
}
char const* CHECK_BOUNDS_NAME="concatInternal_checkArrayBounds";
char const* CHECK_ENUM_INDEX_NAME="concatInternal_checkEnumIndex";
//labels
#define LABEL_CAP 4096
typedef int32_t LabelId;
const LabelId LABEL_ID_UNKNOWN=-1;
typedef struct{
  String label;
  FilePosition declaredAt;
  bool isMutable;
}Label;
Label labelBuffer[LABEL_CAP];
int32_t labelBufferCount=0;
String getLabelName(LabelId labelId){
  if(labelId<0||labelId>=labelBufferCount)
    return EMPTY_STRING;
  return labelBuffer[labelId].label;
}
Label label(LabelId labelId,FilePosition pos){
  if(labelId<0||labelId>=labelBufferCount)
    handleError("label id out of range",ERROR_MEMORY,pos);
  return labelBuffer[labelId];
}
LabelId newLabel(String label,bool isMutable,FilePosition declaredAt){
  if(labelBufferCount>=LABEL_CAP)
    handleError("exceeded label capacity",ERROR_MEMORY,declaredAt);
  labelBuffer[labelBufferCount]=(Label){.label=label,.isMutable=isMutable,.declaredAt=declaredAt};
  return labelBufferCount++;
}
int32_t findLabel(LabelId labelOffset,int32_t labelCount,String const* labelName){
  for(int32_t i=0;i<labelCount;i++){
    if(stringCompare(getLabelName(labelOffset+i),*labelName)==0)
      return i;
  }
  return -1;
}

//types
typedef enum{
  TYPECLASS_UNDEFINED,
  TYPECLASS_PRIMITIVE,
  TYPECLASS_POINTER,
  TYPECLASS_TUPLE,
  TYPECLASS_PROC_IN,
  TYPECLASS_LABELED_PROC_IN,
  TYPECLASS_PROC_OUT,//behaves like tuple but will not be directly used
  TYPECLASS_PROCEDURE,
  TYPECLASS_TYPE_OF,
  TYPECLASS_OPAQUE,
  TYPECLASS_STRUCT,
  TYPECLASS_ENUM,
  TYPECLASS_ENUM_LABEL,
  TYPECLASS_ARRAY,
  TYPECLASS_ARRAY_VIEW,
}TypeClass;

typedef enum{
  PRIMITIVE_VOID,
  PRIMITIVE_BOOL,
  PRIMITIVE_I8,
  PRIMITIVE_I32,
  PRIMITIVE_I64,
  PRIMITIVE_FLOAT,
}PrimitiveType;
typedef struct CompositeType CompositeType;
typedef CompositeType TupleType;
typedef struct ProcedureType ProcedureType;
typedef struct ArrayType ArrayType;
typedef struct DataType{
  union{
    PrimitiveType primitive;
    struct DataType const* type;
    CompositeType const* composite;
    TupleType const* tuple;//name alias for composite
    ProcedureType const* procedure;
    ArrayType const* array;
    int64_t typeId;
  }typeDataAs;
  TypeClass typeClass;
  bool isMutable;
}DataType;
#define FLAG_IS_TUPLE      1
#define FLAG_IS_PROC_IN    2
#define FLAG_IS_PROC_OUT   4
#define FLAG_IS_STRUCT     8
#define FLAG_IS_ENUM       16
#define FLAG_VOID_ONLY     32
struct CompositeType{
  DataType const* types;
  int32_t labelOffset;//offset in labelBuffer
  int32_t id;
  int16_t typeCount;
  int16_t flags;
};
struct ProcedureType{
  struct DataType const* inType;
  struct DataType const* outType;
  int32_t id;
};
struct ArrayType{
  DataType const* base;
  int64_t const* sizes;
  int32_t dims;
  int32_t id;
  bool sizeKnown;
  bool sizeUsed;
  bool viewOnly;
  bool isMutable;
};

const DataType TYPE_UNDEFINED={.typeClass=TYPECLASS_UNDEFINED,.typeDataAs={0}};

#define MAX_TYPES       4096
#define MAX_COMPOSITE   1024
#define MAX_ARRAY_TYPES 1024
#define MAX_PROC_TYPES  1024

size_t wrappedTypeCount=0;
DataType wrappedTypes[MAX_TYPES];
int32_t compositeCount=0;
CompositeType compositeTypes[MAX_COMPOSITE];
int32_t procTypeCount=0;
ProcedureType procTypes[MAX_PROC_TYPES];
int32_t arrayTypeCount=0;
ArrayType arrayTypes[MAX_ARRAY_TYPES];

typedef struct{
  int32_t id;
  bool isArray;//array or composite
}MultiTypeDeclaration;
int32_t declaredMultiTypeCount=0;
MultiTypeDeclaration declaredMultiTypes[MAX_TYPES];
void declareMultiType(int32_t id,bool isArray){
  if(declaredMultiTypeCount>=MAX_TYPES){
    fputs("exceeded maximum number of allowed multi-types",stderr);
    exit(1);
  }
  declaredMultiTypes[declaredMultiTypeCount++]=(MultiTypeDeclaration){.id=id,.isArray=isArray};
}

bool isArrayType(DataType const* type){
  return type->typeClass==TYPECLASS_ARRAY||type->typeClass==TYPECLASS_ARRAY_VIEW;
}
bool isMutableType(DataType const* type){
  if(isArrayType(type))
    return type->typeDataAs.array->isMutable;
  return type->isMutable;
}
bool typeEquals(DataType const* a,DataType const* b){
  if(a->typeClass!=b->typeClass)
    return false;
  if(isMutableType(a)!=isMutableType(b))
    return false;
  switch(a->typeClass){
    case TYPECLASS_UNDEFINED://distinguish different pre-declared types
      return a->typeDataAs.typeId==b->typeDataAs.typeId;
    case TYPECLASS_PRIMITIVE:
      return a->typeDataAs.primitive==b->typeDataAs.primitive;
    case TYPECLASS_POINTER:
    case TYPECLASS_TYPE_OF:
      return typeEquals(a->typeDataAs.type,b->typeDataAs.type);
    case TYPECLASS_TUPLE:
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      return a->typeDataAs.composite->id==b->typeDataAs.composite->id;
    case TYPECLASS_PROCEDURE:
      return typeEquals(a->typeDataAs.procedure->inType,b->typeDataAs.procedure->inType)&&
              typeEquals(a->typeDataAs.procedure->outType,b->typeDataAs.procedure->outType);
    case TYPECLASS_OPAQUE:
      return a->typeDataAs.typeId==b->typeDataAs.typeId;
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:
      return a->typeDataAs.array==b->typeDataAs.array;
  }
  return false;
}

int numberRank(PrimitiveType t){
  switch(t){
    case PRIMITIVE_I8:
      return 8;
    case PRIMITIVE_I32:
      return 32;
    case PRIMITIVE_I64:
      return 64;
    case PRIMITIVE_FLOAT:
      return 65;
    case PRIMITIVE_VOID:
      return -1;
    case PRIMITIVE_BOOL:
      return -1;
  }
  return false;
}
PrimitiveType numberByRank(int t){
  switch(t){
    case 8:
      return PRIMITIVE_I8;
    case 32:
      return PRIMITIVE_I32;
    case 64:
      return PRIMITIVE_I64;
    case 65:
      return PRIMITIVE_FLOAT;
  }
  return PRIMITIVE_VOID;
}
bool isInteger(PrimitiveType t){
  switch(t){
    case PRIMITIVE_I8:
    case PRIMITIVE_I32:
    case PRIMITIVE_I64:
      return true;
    case PRIMITIVE_VOID:
    case PRIMITIVE_BOOL:
    case PRIMITIVE_FLOAT:
      return false;
  }
  return false;
}

bool isPrimitiveType(DataType const* type){
  return type->typeClass==TYPECLASS_PRIMITIVE;
}
bool isVoidType(DataType const* type){
  return isPrimitiveType(type)&&type->typeDataAs.primitive==PRIMITIVE_VOID;
}
bool isBoolType(DataType const* type){
  return isPrimitiveType(type)&&type->typeDataAs.primitive==PRIMITIVE_BOOL;
}
bool isIntType(DataType const* type){
  return isPrimitiveType(type)&&isInteger(type->typeDataAs.primitive);
}
bool isNumberType(DataType const* type){
  return isPrimitiveType(type)&&numberRank(type->typeDataAs.primitive)>-1;
}
bool isPointerType(DataType const* type){
  return type->typeClass==TYPECLASS_POINTER;
}
bool isCallableType(DataType const* type){
  if(isPointerType(type))
    type=type->typeDataAs.type;
  return type->typeClass==TYPECLASS_PROCEDURE;
}
bool isTupleType(DataType const* type){
  switch(type->typeClass){
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
      return true;
    case TYPECLASS_UNDEFINED:
    case TYPECLASS_PRIMITIVE:
    case TYPECLASS_POINTER:
    case TYPECLASS_PROCEDURE:
    case TYPECLASS_TYPE_OF:
    case TYPECLASS_OPAQUE:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:
      return false;
  }
  return false;
}

DataType arrayType(bool isView,DataType const* base, int32_t dims,int64_t const* sizes,bool isMutable);
bool makeMutable(DataType* t){
  switch(t->typeClass){
    case TYPECLASS_POINTER:
      t->isMutable=true;
      return true;
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:
      *t=arrayType(t->typeClass==TYPECLASS_ARRAY_VIEW,t->typeDataAs.array->base,t->typeDataAs.array->dims,t->typeDataAs.array->sizes,true);
      t->isMutable=true;
      return true;
    case TYPECLASS_UNDEFINED:
    case TYPECLASS_PRIMITIVE:
    case TYPECLASS_TUPLE://mutability of composite types controlled by their container 
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_PROCEDURE:
    case TYPECLASS_TYPE_OF:
    case TYPECLASS_OPAQUE:
    case TYPECLASS_ENUM_LABEL:
      return false;
  }
  return false;
}
DataType primitiveType(PrimitiveType id){
  return (DataType){.typeClass=TYPECLASS_PRIMITIVE,.typeDataAs={.primitive=id}};
}
DataType opaqueType(int64_t typeId){
  return (DataType){.typeClass=TYPECLASS_OPAQUE,.typeDataAs={.typeId=typeId}};
}
DataType* bufferedType(DataType const* target){
  for(size_t i=0;i<wrappedTypeCount;i++){
    if(typeEquals(target,&(wrappedTypes[i])))
      return wrappedTypes+i;
  }
  if(wrappedTypeCount+1>=MAX_TYPES){
    fputs("type buffer overflow",stderr);
    return NULL;
  }
  wrappedTypes[wrappedTypeCount]=*target;
  return wrappedTypes+wrappedTypeCount++;
}
DataType wrapperType(TypeClass typeClass,DataType const* target){
  DataType* buffered=bufferedType(target);
  if(buffered==NULL)
    return TYPE_UNDEFINED;
  return (DataType){.typeClass=typeClass,.typeDataAs={.type=buffered}};
}
DataType pointerType(DataType const* target,bool mutable){
  DataType ptr=wrapperType(TYPECLASS_POINTER,target);
  if(mutable)
    makeMutable(&ptr);
  return ptr;
}
DataType typeOfType(DataType const* conent){
  return wrapperType(TYPECLASS_TYPE_OF,conent);
}
int64_t indexOfTypeArray(DataType const* base,size_t baseLen,DataType const* child,size_t childLen){
  if(childLen>baseLen)
    return -1;
  bool isMatch;
  for(size_t off=0;off<=baseLen-childLen;off++){
    isMatch=true;
    for(size_t i=0;i<childLen;i++){
      if(!typeEquals(&(base[i+off]),&(child[i]))){
        isMatch=false;
        break;
      }
    }
    if(isMatch)
      return off;
  }
  return -1;
}

DataType compositeType(TypeClass typeClass,DataType const* elements,int32_t labelOffset,int32_t eltCount){
  if(eltCount==0&&(typeClass!=TYPECLASS_PROC_IN)&&(typeClass!=TYPECLASS_LABELED_PROC_IN)&&(typeClass!=TYPECLASS_PROC_OUT)){
    return TYPE_UNDEFINED;//only procedure in/out can be empty composites
  }
  int16_t classFlag;
  switch(typeClass){
    case TYPECLASS_PROC_IN:
      classFlag=FLAG_IS_PROC_IN;
      break;
    case TYPECLASS_LABELED_PROC_IN:
      if(labelOffset==-1)
        return TYPE_UNDEFINED;
      classFlag=FLAG_IS_PROC_IN;
      break;
    case TYPECLASS_PROC_OUT:
      classFlag=FLAG_IS_PROC_OUT;
      break;
    case TYPECLASS_TUPLE:
      classFlag=FLAG_IS_TUPLE;
      break;
    case TYPECLASS_STRUCT:
      if(labelOffset==-1)
        return TYPE_UNDEFINED;
      classFlag=FLAG_IS_STRUCT;
      break;
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      if(labelOffset==-1)
        return TYPE_UNDEFINED;
      classFlag=FLAG_IS_ENUM;
      break;
    default:
      return TYPE_UNDEFINED;
  }
  bool isVoid=true;
  for(int32_t i=0;i<eltCount;i++){
    if(!isVoidType(elements+i)){
      isVoid=false;
      break;
    }
  }
  if(isVoid)
    classFlag|=FLAG_VOID_ONLY;
  if(eltCount==0){//empty composite
    int32_t match=-1;
    for(int32_t i=0;i<compositeCount;i++){
      if(compositeTypes[i].typeCount==0){
        match=i;
        break;
      }
    }
    if(match==-1){
      if(compositeCount+1>=MAX_COMPOSITE)
        return TYPE_UNDEFINED;
      match=compositeCount;
      compositeTypes[compositeCount]=(CompositeType){.id=compositeCount,.typeCount=0,.types=NULL,.labelOffset=labelOffset,.flags=classFlag};
      declareMultiType(compositeCount,false);
      compositeCount++;
    }
    return (DataType){.typeClass=typeClass,.typeDataAs.composite=compositeTypes+match};
  }
  int64_t typeMatch=-1,matchIndex,typesIndex;
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].typeCount==eltCount||(typeMatch==-1&&compositeTypes[i].typeCount>eltCount)){
      typesIndex=indexOfTypeArray(compositeTypes[i].types,compositeTypes[i].typeCount,elements,eltCount);
      if(typesIndex==-1)
        continue;
      typeMatch=i;
      matchIndex=typesIndex;
      if(labelOffset!=compositeTypes[i].labelOffset)
        continue;//labels do not match -> cannot reuse composite
      if(compositeTypes[i].typeCount==eltCount){
        compositeTypes[i].flags|=classFlag;
        return (DataType){.typeClass=typeClass,.typeDataAs.composite=compositeTypes+i};
      }
    }
  }
  if(compositeCount+1>=MAX_COMPOSITE)
    return TYPE_UNDEFINED;
  DataType const* types;
  if(typeMatch!=-1){
    types=compositeTypes[typeMatch].types+matchIndex;
  }else{
    DataType* newTypes=malloc(eltCount*sizeof(DataType));//will persist until program exits
    if(newTypes==NULL)
      return TYPE_UNDEFINED;
    memcpy(newTypes,elements,eltCount*sizeof(DataType));
    types=newTypes;
  }
  compositeTypes[compositeCount]=(CompositeType){.id=compositeCount,.typeCount=eltCount,.types=types,.labelOffset=labelOffset,.flags=classFlag};
  declareMultiType(compositeCount,false);
  return (DataType){.typeClass=typeClass,.typeDataAs={.composite=compositeTypes+(compositeCount++)}};
}
DataType procedureType(DataType const* inType,DataType const* outType){
  for(int32_t i=0;i<procTypeCount;i++){
    if(typeEquals(procTypes[i].inType,inType)&&typeEquals(procTypes[i].outType,outType))
      return (DataType){.typeClass=TYPECLASS_PROCEDURE,.typeDataAs={.procedure=procTypes+i}};
  }
  inType=bufferedType(inType);
  outType=bufferedType(outType);
  if(inType==NULL||outType==NULL)
    return TYPE_UNDEFINED;
  procTypes[procTypeCount]=(ProcedureType){.id=procTypeCount,.inType=inType,.outType=outType};
  return (DataType){.typeClass=TYPECLASS_PROCEDURE,.typeDataAs={.procedure=procTypes+procTypeCount++}};
}
DataType asUnlabeledProc(DataType const* procType,FilePosition pos){
  if(!isCallableType(procType))
    handleError("expected a callable type",ERROR_TYPE,pos);
  DataType const* baseType=procType;
  bool isPtr=false;
  if(isPointerType(procType)){
    procType=procType->typeDataAs.type;
    isPtr=true;
  }
  ProcedureType const* proc=procType->typeDataAs.procedure;
  if(proc->inType->typeClass==TYPECLASS_PROC_IN)
    return *baseType;
  //replaces labeled types with their canonical unlabeled version 
  DataType in=compositeType(TYPECLASS_PROC_IN,proc->inType->typeDataAs.composite->types,LABEL_ID_UNKNOWN,proc->inType->typeDataAs.composite->typeCount);
  if(in.typeClass==TYPECLASS_UNDEFINED)
    handleError("unexpected error while allocating type",ERROR_MEMORY,pos);
  DataType newProc=procedureType(&in,proc->outType);
  return isPtr?pointerType(&newProc,false):newProc;
}
DataType arrayType(bool isView,DataType const* base, int32_t dims,int64_t const* sizes,bool isMutable){
  if(dims<=0)
    return TYPE_UNDEFINED;
  base=bufferedType(base);
  if(base==NULL||(isCallableType(base)&&!isPointerType(base)))
    return TYPE_UNDEFINED;
  for(int32_t i=0;i<arrayTypeCount;i++){
    if(!typeEquals(arrayTypes[i].base,base)||arrayTypes[i].dims!=dims||arrayTypes[i].isMutable!=isMutable)
      continue;
    if(arrayTypes[i].sizes==sizes){//same array or both NULL
      arrayTypes[i].viewOnly&=isView;
      return (DataType){.typeClass=isView?TYPECLASS_ARRAY_VIEW:TYPECLASS_ARRAY,.typeDataAs={.array=arrayTypes+i}};
    }
    if(sizes==NULL||arrayTypes[i].sizes==NULL)
      continue;
    bool match=true;
    for(int32_t j=0;j<dims;j++){
      if(arrayTypes[i].sizes[j]!=sizes[j]){
        match=false;
        break;
      }
    }
    if(match){
      arrayTypes[i].viewOnly&=isView;
      return (DataType){.typeClass=isView?TYPECLASS_ARRAY_VIEW:TYPECLASS_ARRAY,.typeDataAs={.array=arrayTypes+i}};
    }
  }
  int64_t* mSizes=NULL;
  if(sizes!=NULL){
    mSizes=malloc(dims*sizeof(*mSizes));//XXX reuse array of previous types
    if(mSizes==NULL)
      return TYPE_UNDEFINED;
    memcpy(mSizes,sizes,dims*sizeof(*mSizes));
  }
  arrayTypes[arrayTypeCount]=(ArrayType){.base=base,.dims=dims,.sizes=mSizes,.id=arrayTypeCount,.sizeUsed=false,.sizeKnown=sizes!=NULL,.isMutable=isMutable,.viewOnly=isView};
  declareMultiType(arrayTypeCount,true);
  return (DataType){.typeClass=isView?TYPECLASS_ARRAY_VIEW:TYPECLASS_ARRAY,.typeDataAs={.array=arrayTypes+arrayTypeCount++}};
}

char const* typeClassName(TypeClass cls){
  switch(cls){
    case TYPECLASS_UNDEFINED:
      return "UNDEFINED";
    case TYPECLASS_PRIMITIVE:
      return "primitive";
    case TYPECLASS_POINTER:
      return "pointer";
    case TYPECLASS_TUPLE:
      return "tuple";
    case TYPECLASS_PROC_IN:
      return "procedure arguments";
    case TYPECLASS_LABELED_PROC_IN:
      return "labeled procedure arguments";
    case TYPECLASS_PROC_OUT:
      return "return types";
    case TYPECLASS_PROCEDURE:
      return "procedure";
    case TYPECLASS_TYPE_OF:
      return "type";
    case TYPECLASS_OPAQUE:
      return "opaque type";
    case TYPECLASS_STRUCT:
      return "structure";
    case TYPECLASS_ENUM:
      return "enum";
    case TYPECLASS_ENUM_LABEL:
      return "enum label";
    case TYPECLASS_ARRAY:
      return "array";
    case TYPECLASS_ARRAY_VIEW:
      return "array view";
  }
  fprintf(stderr,"unexpected type-class %i",cls);
  return "";
}
char const* primitiveName(PrimitiveType t){
  switch(t){
    case PRIMITIVE_VOID:
      return "void";
    case PRIMITIVE_BOOL:
      return "bool";
    case PRIMITIVE_I8:
      return "i8";
    case PRIMITIVE_I32:
      return "i32";
    case PRIMITIVE_I64:
      return "i64";
    case PRIMITIVE_FLOAT:
      return "float";
  }
  fprintf(stderr,"unexpected primitive type %i",t);
  return "";
}
void printTypeFlags(DataType const* type,FILE* file){
  if(isMutableType(type))
    fputs(" mut",file);
}
void printTypeNameIntenal(DataType const* type,FILE* file,bool noRecurse){
  String labelName;
  switch(type->typeClass){
    case TYPECLASS_UNDEFINED:
      if(type->typeDataAs.typeId>0){
        fprintf(file,"auto (%"PRIi64")",type->typeDataAs.typeId);
        printTypeFlags(type,file);
        return;
      }
      fputs("UNDEFINED",file);
      printTypeFlags(type,file);
      return;
    case TYPECLASS_PRIMITIVE:
      fprintf(file,"%s",primitiveName(type->typeDataAs.primitive));
      printTypeFlags(type,file);
      return;
    case TYPECLASS_OPAQUE:
      fprintf(file,"opaque (%"PRIi64")",type->typeDataAs.typeId);
      printTypeFlags(type,file);
      return;
    case TYPECLASS_POINTER:
    case TYPECLASS_TYPE_OF:
      printTypeNameIntenal(type->typeDataAs.type,file,noRecurse);
      fprintf(file," %s",typeClassName(type->typeClass));
      printTypeFlags(type,file);
      return;
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      if(type->typeClass!=TYPECLASS_PROC_IN&&type->typeClass!=TYPECLASS_LABELED_PROC_IN&&type->typeClass!=TYPECLASS_PROC_OUT){
        fprintf(file,"%s (%"PRIi32") ",typeClassName(type->typeClass),type->typeDataAs.composite->id);
        if(noRecurse){
          printTypeFlags(type,file);
          return;
        }
        fputs("(",file);
      }
      for(int32_t e=0;e<type->typeDataAs.composite->typeCount;e++){
        if((type->typeClass==TYPECLASS_ENUM||type->typeClass==TYPECLASS_ENUM_LABEL)&&isVoidType(&(type->typeDataAs.composite->types[e]))){
          labelName=getLabelName(type->typeDataAs.composite->labelOffset+e);
          //void-type in enum -> only print label
          fprintf(file," %"PRI_STR,PRI_STR_ARGS(labelName));
          continue;
        }
        fputs(" ",file);
        printTypeNameIntenal(&(type->typeDataAs.composite->types[e]),file,true);//only one recursion level
        if(type->typeClass==TYPECLASS_TUPLE||type->typeClass==TYPECLASS_PROC_IN||type->typeClass==TYPECLASS_PROC_OUT||type->typeDataAs.composite->labelOffset==-1)
          continue;
        labelName=getLabelName(type->typeDataAs.composite->labelOffset+e);
        fprintf(file," : %"PRI_STR,PRI_STR_ARGS(labelName));
      }
      if(type->typeClass!=TYPECLASS_PROC_IN&&type->typeClass!=TYPECLASS_LABELED_PROC_IN&&type->typeClass!=TYPECLASS_PROC_OUT){
        fputs(" )",file);
      }
      printTypeFlags(type,file);
      return;
    case TYPECLASS_PROCEDURE:
      fprintf(file,"%s (%"PRIi32") ",typeClassName(type->typeClass),type->typeDataAs.procedure->id);
      if(noRecurse){
        printTypeFlags(type,file);
        return;
      }
      fputs("( ",file);
      printTypeNameIntenal(type->typeDataAs.procedure->inType,file,true);
      fputs(" => ",file);
      printTypeNameIntenal(type->typeDataAs.procedure->outType,file,true);
      fputs(" )",file);
      printTypeFlags(type,file);
      return;
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:
      printTypeNameIntenal(type->typeDataAs.array->base,file,noRecurse);
      for(int32_t i=0;i<type->typeDataAs.array->dims;i++){
        if(type->typeDataAs.array->sizeKnown){
          fprintf(file," %"PRIi64,type->typeDataAs.array->sizes[i]);
          continue;
        }
        if(type->typeDataAs.array->dims>1)
          fputs(" _",file);
      }
      fprintf(file," %s",typeClassName(type->typeClass));
      printTypeFlags(type,file);
      return;
  }
  fprintf(file,"unknown type-class %i",type->typeClass);
}
void printTypeName(DataType const* type,FILE* file){
  printTypeNameIntenal(type,file,false);
}

char const* primitiveNameC(PrimitiveType t){
  switch(t){
    case PRIMITIVE_VOID:
      return "void";
    case PRIMITIVE_BOOL:
      return "bool";
    case PRIMITIVE_I8:
      return "int8_t";
    case PRIMITIVE_I32:
      return "int32_t";
    case PRIMITIVE_I64:
      return "int64_t";
    case PRIMITIVE_FLOAT:
      return "double";
  }
  fprintf(stderr,"unexpected primitive type %i",t);
  return "";
}
void printTypeNameC(DataType const* type,FILE* file){
  switch(type->typeClass){
    case TYPECLASS_TYPE_OF://type of does not correspond to a C-type
    case TYPECLASS_OPAQUE://pointer to opaque type -> void pointer
    case TYPECLASS_UNDEFINED:
      fputs("void",file);
      return;
    case TYPECLASS_PRIMITIVE:
      fprintf(file,"%s",primitiveNameC(type->typeDataAs.primitive));
      return;
    case TYPECLASS_POINTER:
      printTypeNameC(type->typeDataAs.type,file);
      if(isCallableType(type))
        return;
      if(!isMutableType(type))
        fputs(" const",file);
      fputs("*",file);
      return;
    case TYPECLASS_ARRAY:
      fprintf(file,"array%"PRIi32,type->typeDataAs.array->id);
      return;
    case TYPECLASS_ARRAY_VIEW:
      if(!type->typeDataAs.array->sizeKnown){//array with unknown size
        fprintf(file,"array%"PRIi32,type->typeDataAs.array->id); 
        return; 
      }
      printTypeNameC(type->typeDataAs.array->base,file);
      if(!isMutableType(type))
        fputs(" const",file);
      fputs("*",file);
      return;
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
      if(type->typeDataAs.composite->typeCount==0){
        fputs("void",file);
        return;
      }
      if(type->typeDataAs.composite->typeCount==1){
        printTypeNameC(type->typeDataAs.composite->types+0,file);
        return;
      }
      fprintf(file,"tuple%"PRIi32,type->typeDataAs.composite->id);
      return;
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
      fprintf(file,"tuple%"PRIi32,type->typeDataAs.composite->id);
      return;
    case TYPECLASS_PROCEDURE:
      fprintf(file,"procPtr%"PRIi32,type->typeDataAs.procedure->id);
      return;
    case TYPECLASS_ENUM:
      fprintf(file,"enum%"PRIi32,type->typeDataAs.composite->id);
      return;
    case TYPECLASS_ENUM_LABEL:
      fputs("int32_t",file);
      return;
  }
  fprintf(file,"unknown type-class %i\n",type->typeClass);
}
//operators
typedef enum{
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  MOD,
  AND,
  OR,
  XOR,
  GT,
  GE,
  EQ,
  NE,
  LE,
  LT,
}BinaryOperator;
char const* binOpName(BinaryOperator op){
  switch(op){
    case ADD:return "ADD";
    case SUBTRACT:return "SUBTRACT";
    case MULTIPLY:return "MULTIPLY";
    case DIVIDE:return "DIVIDE";
    case MOD:return "MOD";
    case AND:return "AND";
    case OR:return "OR";
    case XOR:return "XOR";
    case GT:return "GT";
    case GE:return "GE";
    case EQ:return "EQ";
    case NE:return "NE";
    case LE:return "LE";
    case LT:return "LT";
  }
  return "UNDEFINED";
}
typedef enum{
  NEGATE,
  INCREMENT,
  DECREMENT,
  NOT,
  FLIP,
}UnaryOperator;
char const* unOpName(UnaryOperator op){
  switch(op){
    case NEGATE:return "NEGATE";
    case INCREMENT:return "INCREMENT";
    case DECREMENT:return "DECREMENT";
    case NOT:return "NOT";
    case FLIP:return "FLIP";
  }
  return "UNDEFINED";
}
typedef enum{
  ID_LOCAL_VAR,
  ID_GLOBAL_VAR,
  ID_ARGUMENT,
  ID_PROCEDURE,
  ID_TUPLE,//base element of a tuple-access chain
  ID_TUPLE_ELEMENT,//chain element in tuple-access chain
  ID_ENUM_LABEL,   //get label of enum
  ID_ENUM_ELEMENT, //get element of enum
  ID_POINTER,
  ID_POINTER_OFFSET,
  ID_ARRAY_ELEMENT,
  ID_ARRAY_SIZE,
  ID_INTERMEDIATE_RESULT,
  ID_TMP_VAR,
  ID_TYPE,
}IdentifierType;
typedef struct{
  int32_t id;
  int32_t labelId;
  IdentifierType type;
  bool isMutable;
}IdentifierInfo;
char const* const idNames []={[ID_LOCAL_VAR]="local variable",[ID_GLOBAL_VAR]="global variable",[ID_ARGUMENT]="procedure argument",
  [ID_PROCEDURE]="procedure",[ID_TUPLE]="(tuple element)",[ID_TUPLE_ELEMENT]="tuple element",[ID_ENUM_LABEL]="enum label",[ID_ENUM_ELEMENT]="enum element",[ID_POINTER]="pointer value",
  [ID_POINTER_OFFSET]="pointer offset",[ID_ARRAY_ELEMENT]="array element",[ID_ARRAY_SIZE]="array size",[ID_INTERMEDIATE_RESULT]="intermediate result",
  [ID_TMP_VAR]="temporary variable",[ID_TYPE]="type"};
void printIdInfo(IdentifierInfo info,FILE* out){
  if(info.isMutable)
    fputs("mutable ",out);
  if(info.labelId==LABEL_ID_UNKNOWN){
    fprintf(out,"%s (%"PRIi32")",idNames[info.type],info.id);
    return;
  }
  String labelName=getLabelName(info.labelId);
  fprintf(out,"%s \"%"PRI_STR"\" (%"PRIi32")",idNames[info.type],PRI_STR_ARGS(labelName),info.id);
}

typedef enum{
  BLOCK_PROCEDURE, 
  BLOCK_IF,        // if( EXPR ){
  BLOCK_IF2,       // if(EXPR){ ... } (auto-closes at end of current if -statement)
  BLOCK_ELSE,      // }else{
  BLOCK_WHILE,     // while( EXPR ){
  BLOCK_DO,        // do{
  BLOCK_BREAK,     // break;
  BLOCK_CONTINUE,  // continue;
  BLOCK_SWITCH,    // switch( EXPR ){
  BLOCK_CASE,      // case e1 : ... case eN :
  BLOCK_DEFAULT,   // default: 
  BLOCK_UNKNOWN,   // end of unknown block
}BlockType;
char const* const blockNames []={[BLOCK_PROCEDURE]="procedure",[BLOCK_IF]="if",
  [BLOCK_IF2]="_if",[BLOCK_ELSE]="else",[BLOCK_WHILE]="while",[BLOCK_DO]="do",[BLOCK_BREAK]="break",[BLOCK_CONTINUE]="continue",
  [BLOCK_SWITCH]="switch",[BLOCK_CASE]="case",[BLOCK_DEFAULT]="default",
  [BLOCK_UNKNOWN]="unknown"};

typedef struct{
  int32_t id;
  int16_t subId;
  BlockType type;
}BlockData;

typedef struct{
  size_t offset;
  size_t count;
}CaseInfo;
typedef struct{
  int64_t value;
  FilePosition pos;
}LabelData;

typedef struct{
  //indices of cases
  CaseInfo* cases;
  size_t caseCount;
  size_t caseCap;
  //case labels
  LabelData* labelData;
  size_t labelCount;
  size_t labelCap;
  bool hasDefault;
}SwitchData;
#define SWITCH_CAP 1024
SwitchData switchStatements[SWITCH_CAP];
size_t switchCount=0;
SwitchData* newSwitchData(FilePosition pos){
  if(switchCount>=SWITCH_CAP)
    handleError("exceeded switch capacity",ERROR_MEMORY,pos);
  switchStatements[switchCount].caseCap=128;
  switchStatements[switchCount].cases=calloc(128,sizeof(CaseInfo));
  switchStatements[switchCount].labelCap=256;
  switchStatements[switchCount].labelData=calloc(256,sizeof(LabelData));
  if(switchStatements[switchCount].cases==NULL||switchStatements[switchCount].labelData==NULL)
    handleError("unable to allocate switch data",ERROR_MEMORY,pos);
  return &switchStatements[switchCount++];
}

typedef enum{
  STACK_OP_DUP,
  STACK_OP_OVER,
  STACK_OP_DROP,
  STACK_OP_SWAP,
  //XXX?  rotate
}StackOperation;
typedef struct{
  //XXX multi-drop/dup
  StackOperation op;
}StackModification;
typedef enum{
  COMPILERINFO_TYPES,
  COMPILERINFO_STACK,
}CompilerInfoType;
typedef struct{
  int32_t maxCount;
  CompilerInfoType infoType;
}CompilerInfo;


typedef int32_t NamespaceId;
const NamespaceId NAMESPACE_ID_NONE=-1;
typedef int32_t NamespaceImportId;
const NamespaceImportId NAMESPACE_IMPORT_NONE=-1;
typedef struct {
  NamespaceId current;
  NamespaceImportId namespaceImports;
}NamespaceInfo;

typedef struct{
  LabelId label;
  NamespaceInfo namespaceInfo;
}LocalLabel;

typedef struct{
  OpType opType;
  DataType dataType;
  FilePosition filePos;
  union{
    int64_t i64;
    BinaryOperator binOp;
    UnaryOperator unOp;
    IdentifierInfo idInfo;
    BlockData block;
    String string;
    LocalLabel localLabel;
    StackModification stackMod;
    CompilerInfo compilerInfo;
    DataType const* sourceType;
  }dataAs;
}Operation;

void printOperation(Operation op,FILE* out){
  fprintf(out,"%s ",opName(op.opType));
  if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
    printTypeName(&op.dataType,out);
    fputs("  ",out);
  }
  switch(op.opType){
    case OP_CONSTANT:
    case OP_CHECK_ENUM_INDEX:
      fprintf(out,"(%"PRIi64")",op.dataAs.i64);
      break;
    case OP_GET:
    case OP_SET:
    case OP_DECLARE:
    case OP_PRE_DECLARE:
    case OP_CALL:
      printIdInfo(op.dataAs.idInfo,out);
      break;
    case OP_BINARY_OPERATOR:
      fprintf(out,"%s",binOpName(op.dataAs.binOp));
      break;
    case OP_UNARY_OPERATOR:
      fprintf(out,"%s",unOpName(op.dataAs.unOp));
      break;
    case OP_CODE_BLOCK:
    case OP_END_BLOCK:
      fprintf(out,"%s (%"PRIi32":%"PRIi16")",blockNames[op.dataAs.block.type],op.dataAs.block.id,op.dataAs.block.subId);
      break;
    case OP_GET_LABEL:
    case OP_SET_LABEL:
      fprintf(out,"%"PRI_STR"",PRI_STR_ARGS(op.dataAs.string));
      break;
    case OP_IDENTIFIER:
    case OP_SET_IDENTIFIER:
    case OP_IDENTIFIER_ADDRESS:
      fprintf(out,"%"PRI_STR"",PRI_STR_ARGS(getLabelName(op.dataAs.localLabel.label)));
      break;
    case OP_CAST:
      fputs("[ ",out);
      printTypeName(op.dataAs.sourceType,out);
      fputs(" ]",out);
      break;
    default:
      //ignore remaining types
      break;
  }
  fputs("\n",out);
}

#define MAX_NAMESPACES 1024

typedef struct{
  NamespaceId parent;
  String name;
  
  NamespaceId* children;
  size_t childCount;
  size_t childCap;
}Namespace;
Namespace namespaceBuffer[MAX_NAMESPACES];
size_t buffredNamespaces=0;
NamespaceId allocNamespace(NamespaceId parent,String name){
  if(buffredNamespaces>=MAX_NAMESPACES)
    return NAMESPACE_ID_NONE;
  namespaceBuffer[buffredNamespaces].parent=parent;
  namespaceBuffer[buffredNamespaces].name=name;
  size_t initCap=16;
  namespaceBuffer[buffredNamespaces].children=malloc(initCap);
  namespaceBuffer[buffredNamespaces].childCap=initCap;
  namespaceBuffer[buffredNamespaces].childCount=0;
  if(namespaceBuffer[buffredNamespaces].children==NULL)
    return NAMESPACE_ID_NONE;
  return buffredNamespaces++;
}
bool namespaceTrieInit(void){
  if(buffredNamespaces>0)
    return false;
  return allocNamespace(NAMESPACE_ID_NONE,EMPTY_STRING)==-1;
}
NamespaceId childId(NamespaceId base,String childName,bool create){
  if(base==NAMESPACE_ID_NONE||childName.length==0)
    return NAMESPACE_ID_NONE;
  for(size_t i=0;i<namespaceBuffer[base].childCount;i++){
    NamespaceId childId=namespaceBuffer[base].children[i];
    if(stringCompare(namespaceBuffer[childId].name,childName)==0)
      return childId;
  }
  if(create&&namespaceBuffer[base].childCount<namespaceBuffer[base].childCap){
    NamespaceId newChild=allocNamespace(base,childName);
    if(newChild!=NAMESPACE_ID_NONE)
      namespaceBuffer[base].children[namespaceBuffer[base].childCount++]=newChild;
    return newChild;
  }
  return NAMESPACE_ID_NONE;
}
NamespaceId namespaceIdRelative(NamespaceId root,String* path,size_t pathLength,bool create){
  if(root==NAMESPACE_ID_NONE||pathLength==0)
    return NAMESPACE_ID_NONE;
  NamespaceId id=root;
  for(size_t i=0;i<pathLength;i++){
    id=childId(id,path[i],create);
    if(id==NAMESPACE_ID_NONE)
      return NAMESPACE_ID_NONE;
  }
  return id;
}
NamespaceId namespaceId(String* path,size_t pathLength,bool create){
  return namespaceIdRelative(0,path,pathLength,create);
}
//returns true if adding namespace failed
NamespaceId addNamespace(String* path,size_t pathLength){
  return namespaceId(path,pathLength,true);
}
NamespaceId findNamespace(NamespaceId base,String name){
  if(name.length==0)
    return base;
  SlicedString slice=sliceAtChar(name,'.');
  NamespaceId id=childId(base,slice.head,false);
  while(slice.tail.length>0){
    slice=sliceAtChar(slice.tail,'.');
    id=childId(id,slice.head,false);
    if(id==NAMESPACE_ID_NONE)
      return NAMESPACE_ID_NONE;
  }
  return id;
}

typedef struct{
  NamespaceId imported;
  NamespaceImportId parent;
  NamespaceImportId firstChild;
  NamespaceImportId next;
}NamespaceImport;
NamespaceImport namespaceImportBuffer [MAX_NAMESPACES];
size_t bufferedNamespaceImports=0;

NamespaceImportId namespaceImportId(NamespaceImportId parent,NamespaceId import){
  if(bufferedNamespaceImports>=MAX_NAMESPACES)
    return NAMESPACE_IMPORT_NONE;
  if(bufferedNamespaceImports>0){
    NamespaceImportId childId=(parent==NAMESPACE_IMPORT_NONE)?0:namespaceImportBuffer[parent].firstChild;
    if(childId==NAMESPACE_IMPORT_NONE){
      namespaceImportBuffer[parent].firstChild=bufferedNamespaceImports;
    }
    NamespaceImportId prevChild=NAMESPACE_IMPORT_NONE;
    while(childId!=NAMESPACE_IMPORT_NONE){//check all previous child nodes of the parent import
      if(namespaceImportBuffer[childId].imported==import)
        return childId;
      prevChild=childId;
      childId=namespaceImportBuffer[childId].next;
    }
    if(prevChild!=NAMESPACE_IMPORT_NONE){
      namespaceImportBuffer[prevChild].next=bufferedNamespaceImports;
    }
  }
  namespaceImportBuffer[bufferedNamespaceImports].parent=parent;
  namespaceImportBuffer[bufferedNamespaceImports].imported=import;
  namespaceImportBuffer[bufferedNamespaceImports].firstChild=NAMESPACE_IMPORT_NONE;
  namespaceImportBuffer[bufferedNamespaceImports].next=NAMESPACE_IMPORT_NONE;
  return bufferedNamespaceImports++;
}

typedef struct{
  NamespaceImportId prevImports;
}NamespaceBlock;

#define MAX_COMPILER_BLOCKS 128
NamespaceBlock compilerBlocks [MAX_COMPILER_BLOCKS];
size_t compilerBlockCount=0;

void startNamespace(NamespaceInfo* namespace,String label,FilePosition pos){
  if(compilerBlockCount>=MAX_COMPILER_BLOCKS)
    handleError("compiler block overflow",ERROR_MEMORY,pos);
  compilerBlocks[compilerBlockCount++]=(NamespaceBlock){.prevImports=namespace->namespaceImports};
  namespace->current=childId(namespace->current,label,true);
  if(namespace->current==NAMESPACE_ID_NONE)
    handleError("storing namespace failed",ERROR_MEMORY,pos);
}
void importNamespace(NamespaceInfo* namespace,String label,FilePosition pos){
  NamespaceId uSpaceId=findNamespace(0,label);
  if(uSpaceId==NAMESPACE_ID_NONE){
    fprintf(stderr,"namespace '%"PRI_STR"' does not exist\n",PRI_STR_ARGS(label));
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  NamespaceImportId importId=namespaceImportId(namespace->namespaceImports,uSpaceId);
  if(importId==NAMESPACE_IMPORT_NONE)
    handleError("error while allocating namespace import information",ERROR_MEMORY,pos);
  namespace->namespaceImports=importId;
}
void endCompileTimeBlock(NamespaceInfo* namespace,FilePosition pos){
  if(compilerBlockCount==0)
    handleError("no open compiler blocks",ERROR_SYNTAX,pos);
  compilerBlockCount--;
  //if block is namespace
  if(namespace->current==NAMESPACE_ID_NONE)
    handleError("invalid value for current namespace",ERROR_MEMORY,pos);
  namespace->current=namespaceBuffer[namespace->current].parent;
  if(namespace->current==NAMESPACE_ID_NONE)
    handleError("no open namespaces",ERROR_SYNTAX,pos);
  namespace->namespaceImports=compilerBlocks[compilerBlockCount].prevImports;
}

#define SCOPE_NODE_CAP 8192
#define SCOPE_CAP 256
#define SCOPE_MAP_CAP 1024
typedef struct ScopeNode ScopeNode;
struct ScopeNode{
  String key;
  DataType type;
  ScopeNode* next;
  NamespaceId namespaceId;
  int32_t labelId;
  int32_t id;
  IdentifierType idType;
};
typedef struct Scope{
  ScopeNode** nodes;
  
  BlockType scopeType;
  NamespaceImportId prevImports;
  size_t nodeBufferOffset;
  struct Scope* parent;
}Scope;
ScopeNode scopeNodeBuffer [SCOPE_NODE_CAP];
size_t scopeNodeCount=0;
Scope scopeBuffer [SCOPE_CAP];
size_t scopeCount=0;
ScopeNode* allocScopeNode(void){
  if(scopeNodeCount+1>=SCOPE_NODE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of variables %i\n",SCOPE_NODE_CAP);
    return NULL;
  }
  return scopeNodeBuffer+(scopeNodeCount++);
}
Scope* openScope(BlockType scopeType,NamespaceInfo namespace){
  if(scopeCount+1>=SCOPE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of nested scopes %i\n",SCOPE_CAP);
    return NULL;
  }
  scopeBuffer[scopeCount].nodes=calloc(SCOPE_MAP_CAP,sizeof(ScopeNode*));
  scopeBuffer[scopeCount].nodeBufferOffset=scopeNodeCount;
  scopeBuffer[scopeCount].scopeType=scopeType;
  scopeBuffer[scopeCount].prevImports=namespace.namespaceImports;
  scopeBuffer[scopeCount].parent=scopeCount>0?scopeBuffer+(scopeCount-1):NULL;
  return scopeBuffer+(scopeCount++);
}
bool closeScope(NamespaceInfo* namespace){
  if(scopeCount<=0)
    return false;
  scopeCount--;
  free(scopeBuffer[scopeCount].nodes);
  namespace->namespaceImports=scopeBuffer[scopeCount].prevImports;
  scopeNodeCount=scopeBuffer[scopeCount].nodeBufferOffset;
  return true;
}
ScopeNode** findNode(Scope* scope,String name,NamespaceId namespaceId){
  if(scope==NULL)
    return NULL;
  uint32_t hash=stringHash(name);
  ScopeNode** node=scope->nodes+(hash%SCOPE_MAP_CAP);
  while(*node!=NULL){
    if(stringCompare((*node)->key,name)==0&&(*node)->namespaceId==namespaceId)
      return node;
    node=&((*node)->next);
  }
  return node;
}
int getIdentifier(NamespaceInfo namespace,String name,ScopeNode** out){
  int64_t dotIndex=lastIndexOfChar(name,'.');
  NamespaceId mNamespaceId=namespace.current;
  NamespaceId relativeSpace;
  if(mNamespaceId==NAMESPACE_ID_NONE){
    fputs("invalid value for namespace.current",stderr);
    return ERROR_MEMORY;
  }
  String path=dotIndex>0?sliceEnd(name,dotIndex):EMPTY_STRING;
  if(dotIndex>0){
    name=sliceStart(name,dotIndex+1);
  }
  int32_t level=scopeCount-1;
  ScopeNode** node;
  *out=NULL;
  while(level>0){//check local variables
    relativeSpace=findNamespace(mNamespaceId,path);
    if(relativeSpace!=NAMESPACE_ID_NONE){
      node=findNode(scopeBuffer+level,name,relativeSpace);//all non-global variables are in the same namespace
      if(node==NULL)
        return ERROR_MEMORY;
      if(*node!=NULL){
        *out=*node;
        return 0;
      }
    }
    level--;
  }
  while(mNamespaceId!=NAMESPACE_ID_NONE){//check global variables in current namespace and all parent namespaces
    relativeSpace=findNamespace(mNamespaceId,path);
    if(relativeSpace!=NAMESPACE_ID_NONE){
      node=findNode(scopeBuffer+level,name,relativeSpace);
      if(node==NULL)
        return ERROR_MEMORY;
      if(*node!=NULL){
        *out=*node;
        return 0;
      }
    }
    mNamespaceId=namespaceBuffer[mNamespaceId].parent;
  }
  NamespaceImportId import=namespace.namespaceImports;//XXX warn if multiple imports match
  while(import!=NAMESPACE_IMPORT_NONE){//check all imports in reverse order
    relativeSpace=findNamespace(namespaceImportBuffer[import].imported,path);
    if(relativeSpace!=NAMESPACE_ID_NONE){
      node=findNode(scopeBuffer+level,name,relativeSpace);
      if(node==NULL)
        return ERROR_MEMORY;
      if(*node!=NULL){
        *out=*node;
        return 0;
      }
    }
    import=namespaceImportBuffer[import].parent;
  }
  return ERROR_SYNTAX;
}
ScopeNode* declareIdentifier(NamespaceInfo namespace,LabelId labelId,DataType type,IdentifierType idType,int32_t id,FilePosition pos){
  Label mLabel=label(labelId,pos);
  if(mLabel.isMutable){
    if(idType==ID_TYPE)
      handleError("type definitions cannot be mutable",ERROR_SYNTAX,mLabel.declaredAt);
    if(idType==ID_PROCEDURE)
      handleError("procedures cannot be mutable",ERROR_SYNTAX,mLabel.declaredAt);
  }
  if(containsChar(mLabel.label,'.'))
    handleError("'.' is not allowed in declared identifiers",ERROR_SYNTAX,pos);
  ScopeNode** node=findNode(scopeBuffer+(scopeCount-1),mLabel.label,namespace.current);
  if(node==NULL)
    handleError("unable to access scope node",ERROR_MEMORY,mLabel.declaredAt);
  if(*node!=NULL){
    fprintf(stderr,"re-declaration of %s '%"PRI_STR"'\n",idNames[idType],PRI_STR_ARGS(mLabel.label));
    fprintf(stderr,"previous declaration: %s '%"PRI_STR"' at ",idNames[(*node)->idType],PRI_STR_ARGS((*node)->key));
    printFilePosition(label((*node)->labelId,pos).declaredAt,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_SYNTAX,mLabel.declaredAt);
  }
  ScopeNode* shaddow;
  getIdentifier(namespace,mLabel.label,&shaddow);
  if(shaddow!=NULL){
    fprintf(stderr,"Warning:\n  declaration of %s '%"PRI_STR"'\n",idNames[idType],PRI_STR_ARGS(mLabel.label));
    fprintf(stderr,"  shadows previous declaration: %s '%"PRI_STR"' at ",idNames[shaddow->idType],PRI_STR_ARGS(shaddow->key));
    printFilePosition(label(shaddow->labelId,pos).declaredAt,stderr);
    fputs("\n",stderr);
    handleWarning(NULL,ERROR_SYNTAX,mLabel.declaredAt);
  }
  *node=allocScopeNode();
  if(*node==NULL)
    handleError("unable to allocate scope node",ERROR_MEMORY,mLabel.declaredAt);
  (*node)->key=mLabel.label;
  (*node)->namespaceId=namespace.current;
  (*node)->type=type;
  (*node)->idType=idType;
  (*node)->id=id;
  (*node)->labelId=labelId;
  (*node)->next=NULL;
  return *node;
}

typedef struct{
  Operation* ops;
  size_t opCount;
  Operation* globalOps;
  size_t globalCount;
  
  Scope* globalScope;
  int32_t predeclaredTypes;
  bool hasEntryPoint;
  bool hasCheckBounds;
  bool hasCheckEnum;
}Program;

typedef struct{
  String  value;
  int32_t stringId;
  int32_t charsId;
  int32_t charsOffset;
  bool    isBaseString;//true if this string has its own char Array, false if the chars of this string are a sub-string of a previous string
}ProgramString;

#define MAX_PROG_STRINGS 1024
size_t progStringCount=0;
ProgramString programStrings[MAX_PROG_STRINGS];
int32_t stringLabelOffset;

void initStringLabels(void){
  stringLabelOffset=labelBufferCount;
  FilePosition dummyPos=(FilePosition){.fileName="compiler.string",.line=0,.posInLine=0};
  newLabel(cstrToStr("raw"),false,dummyPos);
  newLabel(cstrToStr("length"),false,dummyPos);
}

DataType progStringType(void){
  DataType chr=primitiveType(PRIMITIVE_I8);//store in intermediate value to allow call by reference
  return arrayType(true,&chr,1,NULL,false);//create string type
}
int64_t addProgString(String s,FilePosition pos){
  if(progStringCount+1>=MAX_PROG_STRINGS)
    handleError("exceeded string capacity",ERROR_MEMORY,pos);
  for(size_t i=0;i<progStringCount;i++){
    if(stringCompare(programStrings[i].value,s)==0)
      return i;
  }
  programStrings[progStringCount]=(ProgramString){.value=s,.stringId=progStringCount,.charsId=-1,.charsOffset=-1};
  return progStringCount++;
}
int progStringCmp(void const* a,void const* b){
  return ((ProgramString const*)b)->value.length-((ProgramString const*)a)->value.length;
}
void initProgStringChars(void){
  qsort(programStrings,progStringCount,sizeof(ProgramString),&progStringCmp);
  int32_t charId=-1,charOff=0,charIds=0;
  bool isBaseString=true;
  for(size_t i=0;i<progStringCount;i++){
    isBaseString=true;
    for(size_t j=0;j<i;j++){
      if(programStrings[j].isBaseString){//search all previous base-strings
         charOff=indexOfString(programStrings[j].value,programStrings[i].value);
         if(charOff>-1){
           charId=j;
           isBaseString=false;
           break;
         }
      }
    }
    if(isBaseString){
      charId=charIds++;
      charOff=0;
    }
    programStrings[i].charsId=charId;
    programStrings[i].charsOffset=charOff;
    programStrings[i].isBaseString=isBaseString;
  }
}

size_t compileOp(FILE* target,size_t compiledOps,Operation const* op,size_t opSize,bool isGlobal);

size_t tupleElementAccess(FILE* target,int32_t depth,Operation const* op,size_t opCount,bool isPtr){
  if(depth<0||opCount<(size_t)depth)
    handleError(NULL,ERROR_MEMORY,op->filePos);
  size_t size=0;
  for(int32_t i=0;i<depth;i++){
    if(((op+size)->opType!=OP_GET&&(op+size)->opType!=OP_SET)||(op+size)->dataAs.idInfo.type!=ID_TUPLE_ELEMENT){
      fputs("unexpected operation for tuple access: ",stderr);
      printOperation(*(op+size),stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_SYNTAX,op->filePos);
    }
    if(isPtr){
      fprintf(target,"->e%"PRIi32,(op+size)->dataAs.idInfo.id);
      isPtr=false;
    }else{
      fprintf(target,".e%"PRIi32,(op+size)->dataAs.idInfo.id);
    }
    size++;
  }
  return size;
} 
void printProcArgumentTypesC(DataType const* inType,FILE* target,bool printArgNames){
  if(inType->typeClass!=TYPECLASS_PROC_IN&&inType->typeClass!=TYPECLASS_LABELED_PROC_IN){
    fprintf(stderr,"unexpected procedure argument type-class: %s\n",typeClassName(inType->typeClass));
    exit(1);
  }
  CompositeType const* inTypes=inType->typeDataAs.composite;
  if(inTypes->typeCount==0)
    fputs("void",target);
  for(int32_t e=0;e<inTypes->typeCount;e++){
    if(e>0)
      fputs(", ",target);
    printTypeNameC(&(inTypes->types[e]),target);
    if(printArgNames)
      fprintf(target," arg%"PRIi32,e);
  }
}
void printProcedureSignatureC(ProcedureType const* procedure,int32_t procId,FILE* target,bool printArgNames){
  printTypeNameC(procedure->outType,target);
  fprintf(target," procedure%" PRIi32" (",procId);
  DataType const* inType=procedure->inType;
  printProcArgumentTypesC(inType,target,printArgNames);
  fputs(")",target);
}

#define COMPILE_OP_RETURN_ERROR(target, op,opSize)\
                size+=compileOp(target,compiledOps+size,op+size,opSize-size,isGlobal);\

size_t compileGetValue(FILE* target,size_t compiledOps,Operation const* op,size_t size,size_t opSize,bool isGlobal){
  switch(op->dataAs.idInfo.type){
    case ID_TMP_VAR:
    case ID_INTERMEDIATE_RESULT:
      fprintf(target,"tmp%" PRIi32,op->dataAs.idInfo.id);
      return size;
    case ID_LOCAL_VAR:
      fprintf(target,"local%" PRIi32,op->dataAs.idInfo.id);
      return size;
    case ID_ARGUMENT:
      fprintf(target,"arg%" PRIi32,op->dataAs.idInfo.id);
      return size;
    case ID_GLOBAL_VAR:
      fprintf(target,"global%" PRIi32,op->dataAs.idInfo.id);
      return size;
    case ID_PROCEDURE:
      fprintf(target,"procedure%" PRIi32,op->dataAs.idInfo.id);
      return size;
    case ID_TUPLE:
      //1. get tuple
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      //2. tuple element access
      return size+tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,false);
    case ID_TUPLE_ELEMENT:
      handleError("tuple access without base tuple",ERROR_SYNTAX,op->filePos);
      break;
    case ID_ENUM_LABEL:
      if(op->dataType.typeDataAs.composite->flags&FLAG_VOID_ONLY){
        fputs("/*label*/",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        return size;
      }
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(").label",target);
      return size;
    case ID_ENUM_ELEMENT:
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fprintf(target,").data.e%"PRIi32,op->dataAs.idInfo.id);
      return size;
    case ID_POINTER:
      if(op->dataAs.idInfo.id==0){
        fputs("(*(",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs("))",target);
        return size;
      }
      //base value
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      //tuple element access
      return size+tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,true);
    case ID_POINTER_OFFSET:
      if(op->dataAs.idInfo.id==0){
        fputs("(*((",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(")+(",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(")))",target);
        return size;
      }
      //base value
      fputs("((",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")+(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs("))",target);
      //tuple element access
      return size+tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,true);
    case ID_ARRAY_ELEMENT:
      fputs("((",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      if(op->dataType.typeClass!=TYPECLASS_ARRAY_VIEW||!op->dataType.typeDataAs.array->sizeKnown)
        fputs(".data",target);
      fputs("[",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs("])",target);
      return size+tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,false);
    case ID_ARRAY_SIZE:;
      if(op->dataAs.idInfo.id==0){//is length
        if(op->dataType.typeDataAs.array->sizeKnown){
          fprintf(target,"/*length*/((int64_t)%"PRIi64")",op->dataType.typeDataAs.array->sizes[0]);
          return size;
        }
        fputs("((",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fprintf(target,").sizes[%"PRIi32"])",op->dataType.typeDataAs.array->dims-1);
        return size;
      }
      if(op->dataType.typeDataAs.array->sizeKnown){
        fprintf(target,"arraySizes%"PRIi32,op->dataType.typeDataAs.array->id);
        return size;
      }
      fputs("((",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fprintf(target,").sizes)");
      return size;
    case ID_TYPE:
      handleError("type information is not accessible at runtime",ERROR_SYNTAX,op->filePos);
      break;
  }
  fprintf(stderr,"unexpected operation for %s %s \n",opName(op->opType),idNames[op->dataAs.idInfo.type]);
  handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
  return size;
}
size_t compileProcArgs(FILE* target,size_t compiledOps,Operation const* op,size_t size,size_t opSize,bool isGlobal){
  DataType const* in=op->dataType.typeDataAs.procedure->inType;
  DataType const* out=op->dataType.typeDataAs.procedure->outType;
  if(in->typeClass!=TYPECLASS_PROC_IN&&in->typeClass!=TYPECLASS_LABELED_PROC_IN){
    fprintf(stderr,"unexpected procedure argument type-class: %s\n",typeClassName(in->typeClass));
    handleError(NULL,ERROR_MEMORY,op->filePos);
  }
  if(out->typeClass!=TYPECLASS_PROC_OUT){
    fprintf(stderr,"unexpected procedure return type-class: %s\n",typeClassName(out->typeClass));
    handleError(NULL,ERROR_MEMORY,op->filePos);
  }
  fputs("(",target);
  for(int32_t e=0;e<in->typeDataAs.composite->typeCount;e++){
    if(e>0)
      fputs(",",target);
    COMPILE_OP_RETURN_ERROR(target,op,opSize);
  }
  fputs(")",target);
  if(out->typeDataAs.composite->typeCount==0)//function without return value terminates statement
    fputs(";\n",target);
  return size;
} 

size_t compileOp(FILE* target,size_t compiledOps,Operation const* op,size_t opSize,bool isGlobal){
  if(opSize<1)
    handleError("missing operation",ERROR_SYNTAX,op->filePos);
  size_t size=1;
  bool needCast=(!isGlobal)&&(compiledOps==0||(op-1)->opType!=OP_DECLARE);
  switch(op->opType){
    case OP_PRINT:
      fputs("printf(\"%",target);
      bool boolMode=false;
      switch(op->dataType.typeClass){
        case TYPECLASS_PRIMITIVE:
          switch(op->dataType.typeDataAs.primitive){
             case PRIMITIVE_BOOL:
              fputs("s",target);
              boolMode=true;
              break;
             case PRIMITIVE_I8:
              fputs("\"PRIi8\"",target);
              break;
            case PRIMITIVE_I32:
              fputs("\"PRIi32\"",target);
              break;
            case PRIMITIVE_I64:
              fputs("\"PRIi64\"",target);
              break;
            case PRIMITIVE_FLOAT:
              fputs("f",target);
              break;
            default:
              fprintf(stderr,"printing primitive Type %s is not supported\n",primitiveName(op->dataType.typeDataAs.primitive));
              handleError(NULL,ERROR_TYPE,op->filePos);
          }
          break;
        case TYPECLASS_POINTER:
        case TYPECLASS_ARRAY_VIEW:
          fputs("p",target);
          break;
        default:
          fputs("printing values of type ",stderr);
          printTypeName(&op->dataType,stderr);
          fputs(" is (currently) not supported\n",stderr);
          handleError(NULL,ERROR_TYPE,op->filePos);
      }
      fputs("\\n\",",target);
      if(isPointerType(&op->dataType)||isArrayType(&op->dataType)){
        fputs("(void const*)",target);
      }
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      if(isArrayType(&op->dataType)&&!op->dataType.typeDataAs.array->sizeKnown){
        fputs(".data",target);
      }
      if(boolMode){
        fputs("?\"true\":\"false\"",target);
      }
      fputs(");\n",target);
      return size;
    case OP_CONSTANT:
      if(needCast){
        fputs("((",target);
        printTypeNameC(&(op->dataType),target);
        fputs(")",target);
      }
      if(op->dataType.typeClass==TYPECLASS_ENUM_LABEL){
        fprintf(target,"%" PRIi64,op->dataAs.i64);
        if(needCast)
          fputs(")",target);
        return size;
      }
      DataType strType=progStringType();
      if(typeEquals(&op->dataType,&strType)){
        int64_t i=-1;
        for(size_t j=0;j<progStringCount;j++){//find string in reordered string array
          if(programStrings[j].stringId==op->dataAs.i64){
            i=j;
            break;
          }
        }   
        fprintf(target,"{.data=stringChars%"PRIi32"+%"PRIi32",.sizes={%zu}}",programStrings[i].charsId,programStrings[i].charsOffset,programStrings[i].value.length);
        if(needCast)
          fputs(")",target);
        return size;
      }
      if(!isPrimitiveType(&(op->dataType))){
          fputs("constants of non-primitive type ",stderr);
          printTypeName(&op->dataType,stderr);
          fputs(" are not supported\n",stderr);
          handleError(NULL,ERROR_TYPE,op->filePos);
      }
      switch(op->dataType.typeDataAs.primitive){
        case PRIMITIVE_BOOL:
        case PRIMITIVE_I8:
        case PRIMITIVE_I32:
        case PRIMITIVE_I64:
          fprintf(target,"%" PRIi64,op->dataAs.i64);
          if(needCast)
            fputs(")",target);
          return size;
        default:
          fprintf(stderr,"%s constants are (currently) not supported",primitiveName(op->dataType.typeDataAs.primitive));
          handleError(NULL,ERROR_TYPE,op->filePos);
      }
      break;
    case OP_CHECK_ARRAY_BOUNDS:
      fprintf(target,"%s(",CHECK_BOUNDS_NAME);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//index
      fputs(",",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//length
      fputs(");\n",target);
      return size;
    case OP_CHECK_ENUM_INDEX:
      fprintf(target,"%s(",CHECK_ENUM_INDEX_NAME);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//index
      fprintf(target,".label,%"PRIi64");\n",op->dataAs.i64);
      return size;
    case OP_GET:
      return compileGetValue(target,compiledOps,op,size,opSize,isGlobal);
    case OP_SET:
      size=compileGetValue(target,compiledOps,op,size,opSize,isGlobal);
      fputs(" = ",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(";\n",target);
      return size;
    case OP_PRE_DECLARE:
      if(op->dataAs.idInfo.type!=ID_PROCEDURE)
        printTypeNameC(&(op->dataType),target);
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
          fprintf(target," tmp%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return size;
        case ID_LOCAL_VAR:
          fprintf(target," local%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return size;
        case ID_GLOBAL_VAR:
          fprintf(target," global%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return size;
        case ID_PROCEDURE:
          if(!isCallableType(&(op->dataType))||isPointerType(&(op->dataType)))
            handleError("invalid type for ID_PROCEDURE",ERROR_TYPE,op->filePos);
          printProcedureSignatureC(op->dataType.typeDataAs.procedure,op->dataAs.idInfo.id,target,false);
          fputs(";\n",target);
          return size;
        case ID_INTERMEDIATE_RESULT:
        case ID_ARGUMENT:
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_ARRAY_ELEMENT:
        case ID_ARRAY_SIZE:
        case ID_TYPE:
          fprintf(stderr,"cannot pre-declare %s\n",idNames[op->dataAs.idInfo.type]);
          handleError(NULL,ERROR_SYNTAX,op->filePos);
          break;
      }
      break;
    case OP_DECLARE:
      if(op->dataAs.idInfo.type!=ID_PROCEDURE)
        printTypeNameC(&(op->dataType),target);
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
        case ID_INTERMEDIATE_RESULT:
          if(!op->dataAs.idInfo.isMutable)
            fputs(" const",target);
          fprintf(target," tmp%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_LOCAL_VAR:
          if(!op->dataAs.idInfo.isMutable)
            fputs(" const",target);
          fprintf(target," local%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_GLOBAL_VAR:
          if(!op->dataAs.idInfo.isMutable)
            fputs(" const",target);
          fprintf(target," global%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_PROCEDURE:
          if(!isCallableType(&(op->dataType))||isPointerType(&(op->dataType)))
            handleError("invalid type for ID_PROCEDURE",ERROR_TYPE,op->filePos);
          printProcedureSignatureC(op->dataType.typeDataAs.procedure,op->dataAs.idInfo.id,target,true);
          fputs("{\n",target);
          return size;
        case ID_ARGUMENT:
          handleError("cannot declare arguments",ERROR_SYNTAX,op->filePos);
          break;
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
          handleError("cannot declare tuple elements",ERROR_SYNTAX,op->filePos);
          break;
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
          handleError("cannot declare enum elements",ERROR_SYNTAX,op->filePos);
          break;
        case ID_POINTER:
        case ID_POINTER_OFFSET:
          handleError("cannot declare pointers",ERROR_SYNTAX,op->filePos);
          break;
        case ID_ARRAY_ELEMENT:
        case ID_ARRAY_SIZE:
          handleError("cannot declare arrays",ERROR_SYNTAX,op->filePos);
          break;
        case ID_TYPE:
          handleError("cannot declare types",ERROR_SYNTAX,op->filePos);
          break;
      }
      if(op->dataAs.idInfo.type!=ID_PROCEDURE){
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(";\n",target);
      }
      return size;
    case OP_NEW:
      if(op->dataType.typeClass==TYPECLASS_TUPLE||op->dataType.typeClass==TYPECLASS_STRUCT){
        if(needCast){
          fputs("(",target);
          printTypeNameC(&(op->dataType),target);
          fputs(")",target);
        }
        fputs("{",target);
        for(int32_t e=0;e<op->dataType.typeDataAs.composite->typeCount;e++){
          if(e>0)
            fputs(",",target);
          fprintf(target,".e%"PRIi32"=",e);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
        }
        fputs("}",target);
        return size;
      }
      if(op->dataType.typeClass==TYPECLASS_ENUM){
        if(needCast){
          fputs("(",target);
          printTypeNameC(&(op->dataType),target);
          fputs(")",target);
        }
        if(op->dataType.typeDataAs.composite->flags&FLAG_VOID_ONLY){
          fprintf(target,"/*enum*/%"PRIi64,op->dataAs.i64);
          return size;
        }
        fprintf(target,"{.label=%"PRIi64,op->dataAs.i64);
        if(isVoidType(op->dataType.typeDataAs.composite->types+op->dataAs.i64)){
          fputs(",.data={0}}",target);
          return size;
        }
        fprintf(target,",.data={.e%"PRIi64"=",op->dataAs.i64);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs("}}",target);
        return size;
      }
      handleError("unexpected type for OP_NEW",ERROR_UNIMPLEMENTED,op->filePos);
      break;
    case OP_CAST:
      if(op->dataAs.sourceType->typeClass==TYPECLASS_ENUM&&((op->dataAs.sourceType->typeDataAs.composite->flags&FLAG_VOID_ONLY)==0)){
        fputs("(",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(").label",target);
        return size;
      }
      fputs("((",target);
      printTypeNameC(&(op->dataType),target);
      fputs(")",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      return size;
    case OP_ADDR_OF:
      fputs("&(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      return size;
    case OP_UNARY_OPERATOR:
      fputs("(",target);
      switch(op->dataAs.unOp){
        case NEGATE:
          fputs("-",target);
          break;
        case INCREMENT:
          fputs("++",target);
          break;
        case DECREMENT:
          fputs("--",target);
          break;
        case NOT:
          fputs("!",target);
          break;
        case FLIP:
          fputs("~",target);
          break;
      }
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      return size;
    case OP_BINARY_OPERATOR:
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      switch(op->dataAs.binOp){
        case ADD:
          fputs("+",target);
          break;
        case SUBTRACT:
          fputs("-",target);
          break;
        case MULTIPLY:
          fputs("*",target);
          break;
        case DIVIDE:
          fputs("/",target);
          break;
        case MOD:
          fputs("%",target);
          break;
        case AND:
          fputs("&",target);
          break;
        case OR:
          fputs("|",target);
          break;
        case XOR:
          fputs("^",target);
          break;
        case GT:
          fputs(">",target);
          break;
        case GE:
          fputs(">=",target);
          break;
        case EQ:
          fputs("==",target);
          break;
        case NE:
          fputs("!=",target);
          break;
        case LE:
          fputs("<=",target);
          break;
        case LT:
          fputs("<",target);
          break;
      }
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      return size;
    case OP_CODE_BLOCK:
      switch(op->dataAs.block.type){
        case BLOCK_PROCEDURE:
          handleError("block procedure should be eliminated at compile time",ERROR_SYNTAX,op->filePos);
          break;
        case BLOCK_IF:
        case BLOCK_IF2:
          fputs("if(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("){\n",target);
          return size;
        case BLOCK_WHILE:
          fputs("if(!",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(")\n  break;\n",target);
          return size;
        case BLOCK_DO:
          fputs("do{\n",target);
          return size;
        case BLOCK_ELSE:
          fputs("}else{\n",target);
          return size;
        case BLOCK_BREAK:
          fputs("break;\n",target);
          return size;
        case BLOCK_CONTINUE:
          fputs("continue;\n",target);
          return size;
        case BLOCK_SWITCH:
          if(!isIntType(&op->dataType)&&op->dataType.typeClass!=TYPECLASS_ENUM_LABEL){
            fputs("compiling switch-case of type ",stderr);
            printTypeName(&op->dataType,stderr);
            fputs(" is not implemented\n",stderr);
            handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
          }
          fputs("switch(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("){\n",target);
          return size;
        case BLOCK_CASE:
          if(!isIntType(&op->dataType)&&op->dataType.typeClass!=TYPECLASS_ENUM_LABEL){
            fputs("compiling switch-case of type ",stderr);
            printTypeName(&op->dataType,stderr);
            fputs(" is not implemented\n",stderr);
            handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
          }
          {
            if(op->dataAs.block.id<0||(size_t)op->dataAs.block.id>=switchCount)
              handleError("invalid switch block id",ERROR_MEMORY,op->filePos);
            SwitchData switchData=switchStatements[op->dataAs.block.id];
            if(op->dataAs.block.subId<0||(size_t)op->dataAs.block.subId>=switchData.caseCount)
              handleError("invalid case id",ERROR_MEMORY,op->filePos);
            size_t off=switchData.cases[op->dataAs.block.subId].offset;
            size_t count=switchData.cases[op->dataAs.block.subId].count;
            for(size_t i=0;i<count;i++){
              fprintf(target,"case %"PRIi64":",switchData.labelData[off+i].value);
            }
            fputs(";\n",target);//prevent error on initialization after case
          }
          return size;
        case BLOCK_DEFAULT:
          if(!isIntType(&op->dataType)&&op->dataType.typeClass!=TYPECLASS_ENUM_LABEL){
            fputs("compiling switch-case of type ",stderr);
            printTypeName(&op->dataType,stderr);
            fputs(" is not implemented\n",stderr);
            handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
          }
          fputs("default:\n",target);
          return size;
        case BLOCK_UNKNOWN:
          fprintf(stderr,"code block %s should not exist at this stage of compilation\n",blockNames[op->dataAs.block.type]);
          handleError(NULL,ERROR_SYNTAX,op->filePos);
      }
      break;
    case OP_END_BLOCK:
      if(op->dataAs.block.type==BLOCK_WHILE){
          fputs("}while(1);\n",target);
          return size;
      }
      fputs("}\n",target);
      return size;
    case OP_RETURN:
      fputs("return ",target);
      if(op->dataType.typeDataAs.composite->typeCount==0){
        fputs(";\n",target);
        return size;
      }
      if(op->dataType.typeDataAs.composite->typeCount==1){
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(";\n",target);
        return size;
      }
      fprintf(target,"(tuple%"PRIi32"){",op->dataType.typeDataAs.composite->id);
      for(int32_t e=0;e<op->dataType.typeDataAs.composite->typeCount;e++){
        if(e>0)
          fputs(",",target);
        fprintf(target,".e%"PRIi32"=",e);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
      }
      fputs("};\n",target);
      return size;
    case ENTRY_POINT:
      fputs("int main(void){\n",target);
      return size;
    case OP_CALL_PTR:
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      return compileProcArgs(target,compiledOps,op,size,opSize,isGlobal);
    case OP_CALL:
      if(op->dataAs.idInfo.type!=ID_PROCEDURE){
        fprintf(stderr,"calling %s directly is not supported\n",idNames[op->dataAs.idInfo.type]);
        handleError(NULL,ERROR_SYNTAX,op->filePos);
      }
      fprintf(target,"procedure%"PRIi32,op->dataAs.idInfo.id);
      return compileProcArgs(target,compiledOps,op,size,opSize,isGlobal);
    case OP_GET_LABEL:
    case OP_SET_LABEL:
    case OP_IDENTIFIER:
    case OP_SET_IDENTIFIER:
    case OP_IDENTIFIER_ADDRESS:
    case OP_MODIFY_STACK:
    case OP_COMPILER_INFO:
      fprintf(stderr,"operation %s should not exist at this stage of compilation\n",opName(op->opType));
      handleError(NULL,ERROR_SYNTAX,op->filePos);
      break;
    default:
      fputs("operation ",stderr);
      printOperation(*op,stderr);
      fputs(" is not implemented\n",stderr);
      handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
      break;
  }
  fputs("implementation for compiling operation ",stderr);
  printOperation(*op,stderr);
  fputs(" is incomplete\n",stderr);
  handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
  return 0;
}

bool isUsedTuple(CompositeType const* composite){
  if((composite->flags&(FLAG_IS_TUPLE|FLAG_IS_STRUCT))!=0) 
    return true;
  return (composite->flags&(FLAG_IS_PROC_OUT))!=0&&composite->typeCount>1;
}
void compileToC(FILE* target,Program const* p){
  fputs("#include <stdlib.h>\n",target);
  fputs("#include <stdio.h>\n",target);
  fputs("#include <inttypes.h>\n",target);
  fputs("#include <string.h>\n",target);
  fputs("#include <stdbool.h>\n",target);
  fputs("//internal declarations\n",target);
  //initialize strings
  if(progStringCount>0)
    initProgStringChars();//initialize characters
  //declare composite types
  for(int32_t i=0;i<compositeCount;i++){
    if(isUsedTuple(&compositeTypes[i])){
      fprintf(target,"typedef struct tuple%"PRIi32"Impl tuple%"PRIi32";\n",i,i);
    }
    if(compositeTypes[i].flags&(FLAG_IS_ENUM)){
      if(compositeTypes[i].flags&FLAG_VOID_ONLY){
        fprintf(target,"typedef int32_t enum%"PRIi32";\n",i);
      }else{
        fprintf(target,"typedef struct enum%"PRIi32"Impl enum%"PRIi32";\n",i,i);
      }
    }
  }
  for(int32_t i=0;i<arrayTypeCount;i++){
      fprintf(target,"typedef struct array%"PRIi32"Impl array%"PRIi32";\n",i,i);
  }
  //declare procedure pointers
  for(int32_t i=0;i<procTypeCount;i++){//XXX only declare used procedure-pointers
    fputs("typedef ",target);
    printTypeNameC(procTypes[i].outType,target);
    fprintf(target," (*procPtr%"PRIi32") (",i);
    printProcArgumentTypesC(procTypes[i].inType,target,false);
    fputs(");\n",target);
  }
  for(int32_t i=0;i<declaredMultiTypeCount;i++){//got through multi-types in order of declaration
    int32_t id=declaredMultiTypes[i].id;
    if(declaredMultiTypes[i].isArray){
      if(arrayTypes[id].viewOnly&&arrayTypes[id].sizeKnown)
        continue;//skip view-only arrays with known size
      //initialize array types
      fprintf(target,"struct array%"PRIi32"Impl{\n",id);
      printTypeNameC(arrayTypes[id].base,target);
      if(arrayTypes[id].sizeKnown){
        fputs(" data",target);
        for(int32_t d=arrayTypes[id].dims-1;d>=0;d--){//C orders sizes the other way around 
          fprintf(target,"[%"PRIi64"]",arrayTypes[id].sizes[d]);
        }    
      }else{
        if(!arrayTypes[id].isMutable)
          fputs(" const",target);
        fputs("* data",target);
      }
      fputs(";\n",target);
      if(!arrayTypes[id].sizeKnown)
        fprintf(target,"int64_t sizes[%"PRIi32"];\n",arrayTypes[id].dims);//don't declare as const to allow overwriting structure
      fputs("};\n",target);
      if((!arrayTypes[id].sizeKnown)||(!arrayTypes[id].sizeUsed))
        continue;
      fprintf(target,"int64_t const arraySizes%"PRIi32"[%"PRIi32"]={",arrayTypes[id].id,arrayTypes[id].dims);
      for(int32_t d=0;d<arrayTypes[id].dims;d++){
        if(d>0)
          fputs(",",target);
        fprintf(target,"%"PRIi64,arrayTypes[id].sizes[d]);
      }
      fputs("};\n",target);
      continue;
    }
    //initialize composite types
    if(isUsedTuple(&compositeTypes[id])){
      fprintf(target,"struct tuple%"PRIi32"Impl{\n",id);
      for(int16_t e=0;e<compositeTypes[id].typeCount;e++){
        printTypeNameC(&(compositeTypes[id].types[e]),target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("};\n",target);
    }
    if(((compositeTypes[id].flags&FLAG_IS_ENUM)!=0)&&((compositeTypes[id].flags&FLAG_VOID_ONLY)==0)){
      fprintf(target,"struct enum%"PRIi32"Impl{\n",id);
      fputs("union{\n",target);
      for(int16_t e=0;e<compositeTypes[id].typeCount;e++){
        if(isVoidType(&(compositeTypes[id].types[e])))
          continue;//skip void types
        printTypeNameC(&(compositeTypes[id].types[e]),target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("} data;\n",target);
      fputs("int32_t label;\n",target);
      fputs("};\n",target);
    }
  }
  //initialize strings
  for(size_t i=0;i<progStringCount;i++){
    if(programStrings[i].isBaseString){
      fprintf(target,"const %s stringChars%"PRIi32"[%"PRIi64"] = {",primitiveNameC(PRIMITIVE_I8),programStrings[i].charsId,programStrings[i].value.length+1);
      String str=programStrings[i].value;
      for(size_t j=0;j<str.length;j++){
        if(charAt(str,j)<0)
          fprintf(target,"-0x%"PRIx8,-charAt(str,j));
        else
          fprintf(target,"0x%"PRIx8,charAt(str,j));
        fputs(",",target);
      }
      fputs("0x00};\n",target);
    }
  }
  if(p->hasCheckBounds){
    fprintf(target,"void %s(int64_t index,int64_t length){\n",CHECK_BOUNDS_NAME);
    fputs("  if(index>=0 && index<length)\n    return;\n",target);
    fputs("  fprintf(stderr,\"array index out of bounds: %\"PRIi64\" size: %\"PRIi64\"\\n\",index,length);\n",target);
    fprintf(target,"  exit(%i);\n",PROG_EXIT_CODE_ARRAY_OUT_OF_RANGE);
    fputs("}\n",target);
  }
  if(p->hasCheckEnum){
    fprintf(target,"void %s(int64_t current,int64_t expected){\n",CHECK_ENUM_INDEX_NAME);
    fputs("  if(current==expected)\n    return;\n",target);
    fputs("  fprintf(stderr,\"enum index (%\"PRIi64\") does not match current value (%\"PRIi64\")\\n\",expected,current);\n",target);
    fprintf(target,"  exit(%i);\n",PROG_EXIT_CODE_WRONG_ENUM_INDEX);
    fputs("}\n",target);
  }
  fputs("//global code\n",target);
  for(size_t i=0;i<p->globalCount;){
    i+=compileOp(target,i,p->globalOps+i,p->globalCount-i,true);
  }
  fputs("//procedures code\n",target);
  for(size_t i=0;i<p->opCount;){
    i+=compileOp(target,i,p->ops+i,p->opCount-i,false);
  }
}

typedef struct{
  char* code;
  size_t codeSize;
  FilePosition currentPos;
  FilePosition wordStart;
}CodeFile;

typedef struct{
  NamespaceInfo namespaceInfo;
  Scope* currentScope;
  Operation* parsedOps;
  size_t parsedOpCount;
  size_t parsedOpCap;
  int32_t globalVars;
  int32_t localVars;
  int32_t currentProcId;
  int32_t procScope;
  int32_t scopeLevel;
  
  int32_t predeclaredTypes;
  int32_t opaqueTypeCount;
  int64_t entryPointIndex;
}ParserState;
//compute the next id for a variable of the given id-type relative to the given compiler state
int32_t nextId(IdentifierType idType,ParserState* state){
  switch(idType){
    //global 
    case ID_GLOBAL_VAR:
    case ID_TYPE:
    case ID_PROCEDURE:
      return state->globalVars++;
    //local
    case ID_LOCAL_VAR:
    case ID_ARGUMENT:
    case ID_INTERMEDIATE_RESULT:
    case ID_TMP_VAR:
    //inline 
    case ID_TUPLE:
    case ID_TUPLE_ELEMENT:
    case ID_ENUM_LABEL:
    case ID_ENUM_ELEMENT:
    case ID_POINTER:
    case ID_POINTER_OFFSET:
    case ID_ARRAY_ELEMENT:
    case ID_ARRAY_SIZE:
      return state->localVars++;//local 
  }
  return scopeNodeCount;
}


Operation opDeclareIntermediate(DataType const* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_DECLARE,.dataType=*type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_INTERMEDIATE_RESULT,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}};
}
Operation opGetIntermediate(DataType const* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_GET,.dataType=*type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_INTERMEDIATE_RESULT,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}};
}
Operation opPredeclareTmpVar(DataType const* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_PRE_DECLARE,.dataType=*type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}
Operation opDeclareTmpVar(DataType const* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_DECLARE,.dataType=*type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}
Operation opGetTmpVar(DataType const* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_GET,.dataType=*type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}
Operation opSetTmpVar(DataType const* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_SET,.dataType=*type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}

Operation opBinaryOperator(BinaryOperator binOpType,FilePosition pos){
  return (Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={.binOp=binOpType}};
}
Operation opUnaryOperator(UnaryOperator unOpType,FilePosition pos){
  return (Operation){.opType=OP_UNARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={.unOp=unOpType}};
}
Operation opCodeBlock(BlockType blockType,FilePosition pos){
  // id/sub-id will be initialized in type check phase
  return (Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={.block={.type=blockType,.id=-1,.subId=-1}}};
}
Operation opEndCodeBlock(BlockType blockType,FilePosition pos){
  // id/sub-id will be initialized in type check phase
  return (Operation){.opType=OP_END_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={.block={.type=blockType,.id=-1,.subId=-1}}};
}
Operation opConstant(DataType type,int64_t constData,FilePosition pos){
  return (Operation){.opType=OP_CONSTANT,.dataType=type,.filePos=pos,.dataAs={.i64=constData}};
}

//XXX more operation generator functions

void updateFilePosition(CodeFile* codeFile){
  if(*(codeFile->code)=='\n'){
    codeFile->currentPos.line++;
    codeFile->currentPos.posInLine=1;
  }else{
    codeFile->currentPos.posInLine++;
  }
  codeFile->codeSize--;
  codeFile->code++;
}
void skipWhitespaces(CodeFile* codeFile){
  while(codeFile->codeSize>0&&((*(codeFile->code))==0||isspace(*(codeFile->code)))){
    updateFilePosition(codeFile);
  }
}

int toDigit(char c){
  if(c>='0'&&c<='9')
    return c-'0';
  if(c>='A'&&c<='Z')
    return c-'A'+10;
  if(c>='a'&&c<='z')
    return c-'a'+10;
  return -1;
}
IntOrErrorCode parseInt(String number,int base){
  bool detectBase=(base==0);
  if(detectBase)
    base=10;
  size_t i=0;
  int digit;
  uint64_t value=0;
  bool negate=false;
  if(number.length==0)
    return (IntOrErrorCode){.isError=true,.as={.error=ERROR_PARSE_INT}};
  if(number.length>1&&charAt(number,0)=='-'){
    i++;
    negate=true;
  }
  if(detectBase&&number.length>i+1&&charAt(number,i)=='0'){
    i++;
    if(charAt(number,i)=='x'||charAt(number,i)=='X'){
      base=16;
      i++;
    }else if(charAt(number,i)=='b'||charAt(number,i)=='B'){
      base=2;
      i++;
    }
  }
  size_t i0=i;
  bool overflow=false;
  uint64_t maxSaveValue=negate?(INT64_MAX/base):-(INT64_MIN/base);
  for(;i<number.length;i++){
    if(i>i0&&i<number.length-1&&(charAt(number,i)=='_'||charAt(number,i)=='\''))
      continue;//ignore _ and ' if they are in the interior of the number
    if(value>maxSaveValue){
      overflow=true;//check if remaining word is integer before returning overflow error
    }
    value*=base;
    digit=toDigit(charAt(number,i));
    if(digit<0||digit>=base)
      return (IntOrErrorCode){.isError=true,.as={.error=ERROR_PARSE_INT}};
    value+=digit;
  }
  if(overflow){
    fprintf(stderr,"value %"PRI_STR" does not fit in a 64-bit integer\n",PRI_STR_ARGS(number));
    return (IntOrErrorCode){.isError=true,.as={.error=ERROR_INT_OVERFLOW}};
  }
  return (IntOrErrorCode){.isError=false,.as={.i64=negate?-value:value}};
}
String readStringLiteral(CodeFile* codeFile,char* end,size_t endLength,bool doEspaceSeqs){
  if(codeFile->codeSize<1){
    handleError("unexpected end of file",ERROR_EOF,codeFile->currentPos);
    return EMPTY_STRING;
  }
  //skip first char
  updateFilePosition(codeFile);
  char* wordChars=codeFile->code;
  size_t wordLength=0,delta=0;
  size_t sequenceLength;//length of escape sequence for multi-char sequences
  size_t endChars=0;//number of chars of end sequence already encountered 
  while(codeFile->codeSize>0){
    if(*(codeFile->code)==end[endChars]){
      if(++endChars>=endLength)
        break;
    }else{
      endChars=0;
    }
    if(delta>0){//copy chars to position in unescaped string
      wordChars[wordLength]=*(codeFile->code);
    }
    if(doEspaceSeqs&&*(codeFile->code)=='\\'){//escaped characters
      if(codeFile->codeSize<=1){
        handleError("unexpected end of file",ERROR_EOF,codeFile->currentPos);
        return EMPTY_STRING;
      }
      delta++;
      updateFilePosition(codeFile);//ignore the \ character
      switch(*(codeFile->code)){//decode escape sequence
        case 'b':
          wordChars[wordLength]='\b';
          break;
        case 'n':
          wordChars[wordLength]='\n';
          break;
        case 't':
          wordChars[wordLength]='\t';
          break;
        case 'r':
          wordChars[wordLength]='\r';
          break;
        case 'f':
          wordChars[wordLength]='\f';
          break;
        case 'v':
          wordChars[wordLength]='\v';
          break;
        case '\'':
        case '\\':
        case '"':
          wordChars[wordLength]=*(codeFile->code);//copy character
          break; 
        case 'x':
        case 'u':
        case 'U':
          sequenceLength=*(codeFile->code)=='U'?9:*(codeFile->code)=='u'?5:3;
          if(codeFile->codeSize<sequenceLength){
            handleError("unexpected end of file",ERROR_EOF,codeFile->currentPos);
            return EMPTY_STRING;
          }
          IntOrErrorCode val=parseInt(newString((codeFile->code)+1,sequenceLength-1),16);
          if(val.isError){
            handleError("unexpected end of file",val.as.error,codeFile->currentPos);
            return EMPTY_STRING;
          }
          int l;
          if(*(codeFile->code)=='x'){
            wordChars[wordLength]=val.as.i64;
            l=1;
          }else{
            if((val.as.i64<0||val.as.i64>MAX_CODEPOINT)){
              fprintf(stderr,"codepoint %"PRIi64" outside allowed range (0 to %i)\n",val.as.i64,MAX_CODEPOINT);
              handleError(NULL,WARNING_CODEPOINT_OUT_OF_RANGE,codeFile->currentPos);
            }
            l=writeUnicodeChar(val.as.i64,wordChars+wordLength);
          }
          wordLength+=l;
          delta+=sequenceLength-l;
          (codeFile->code)+=sequenceLength;
          (codeFile->codeSize)-=sequenceLength;
          codeFile->currentPos.posInLine+=sequenceLength;//no newline in unicode escape sequence
          continue;//skip to next iteration of loop
        default:
          fprintf(stderr,"unsupported escape sequence '\\%c'\n",*(codeFile->code));
          handleError(NULL,ERROR_SYNTAX,codeFile->currentPos);
          break; 
      }
    }
    wordLength++;
    updateFilePosition(codeFile);
  }
  if(codeFile->codeSize==0){
    fprintf(stderr,"unfinished comment or string literal %"PRI_STR" \n",(int)wordLength,codeFile->code);
    handleError(NULL,ERROR_EOF,codeFile->currentPos);
    return EMPTY_STRING;
  }
  //move code-pointer to position after word
  updateFilePosition(codeFile);
  wordChars[wordLength]=0;//zero terminate string
  return newString(wordChars,wordLength);
}
//constants for the wordType flag of nextWord
//allow to determine which type of word was read
#define WORD_TYPE_IDENTIFIER 0
#define WORD_TYPE_STRING    1
#define WORD_TYPE_CHAR      2

String nextWord(CodeFile* codeFile,int* wordType){
  skipWhitespaces(codeFile);
  if(codeFile->codeSize<=0){//end of file
    //don't set wordType to ERROR_EOF, file is allowed to end at this point
    return EMPTY_STRING;
  }
  codeFile->wordStart=codeFile->currentPos;
  if(wordType)
    *wordType=WORD_TYPE_IDENTIFIER;
  if(*(codeFile->code)=='"'){
    if(wordType)
      *wordType=WORD_TYPE_STRING;
    return readStringLiteral(codeFile,"\"",1,true);
  }
  if(*(codeFile->code)=='\''){
    if(wordType)
      *wordType=WORD_TYPE_CHAR;
    return readStringLiteral(codeFile,"'",1,true);
  }
  if(codeFile->codeSize>=2&&*(codeFile->code)=='#'){
    if(*(codeFile->code+1)=='#'){//line comment
      readStringLiteral(codeFile,"\n",1,false);//ignore everything up to next new-line
      return EMPTY_STRING;
    }
    if(*(codeFile->code+1)=='+'){//inline comment
      readStringLiteral(codeFile,"+#",2,false);
      return EMPTY_STRING;
    }
  }
  char* wordChars=codeFile->code;
  size_t wordLength=0;
  while(codeFile->codeSize>0&&(*(codeFile->code))!=0&&!isspace(*(codeFile->code))){
    updateFilePosition(codeFile);
    wordLength++;
  }
  if(codeFile->codeSize>0){
    updateFilePosition(codeFile);//skip first white-space
    *(codeFile->code-1)=0;//replace white-space with \0 to allow printing word as C-string
  }
  return newString(wordChars,wordLength);
}


LabelId readLabel(CodeFile* codeFile,char const* labelType){
  int wordType=0;
  bool isMutable=false,isModifer;
  String label;
  do{
    label=nextWord(codeFile,&wordType);
    isModifer=false;
    if(wordType!=WORD_TYPE_IDENTIFIER){
      fprintf(stderr,"%s have to be identifiers\n",labelType);
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
    }
    if(label.length==0){
      fprintf(stderr,"%s have to be non-empty\n",labelType);
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
    }
    if(wordEquals(&label,"mut")){
      if(isMutable){
        fprintf(stderr,"%s is already mutable\n",labelType);
        handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
      }
      isMutable=true;
      isModifer=true;
    }
  }while(isModifer);
  return newLabel(label,isMutable,codeFile->wordStart);
}


//temporary buffer for storing constants
#define CONST_BUFFER_CAP       512
#define MAX_COMPOSITE_ELEMENTS 128
#define MAX_ARRAY_DIMS         64
typedef enum{
  CONSTANT_INT,
  CONSTANT_CHAR,
  CONSTANT_STRING,
  CONSTANT_TYPE,
}ConstantType;
char const* constTypeName(ConstantType type){
  switch(type){
    case CONSTANT_INT:return "int";
    case CONSTANT_CHAR:return "char";
    case CONSTANT_STRING:return "string";
    case CONSTANT_TYPE:return "type";
  }
  return "unknown type";
}
typedef struct{
  union{
    DataType type;
    String  string;
    int64_t charId;
    int64_t i64;
  }valueAs;
  FilePosition pos;
  ConstantType type;
}ConstantValue;
size_t bufferedConstants=0;
ConstantValue constBuffer[CONST_BUFFER_CAP];
DataType compositeTypeBuffer[MAX_COMPOSITE_ELEMENTS];
int64_t arrayDimsBuffer[MAX_ARRAY_DIMS];
int64_t arrayDimsCount=0;
void pushIntConstant(ConstantType constType,int64_t constId,FilePosition pos){
  if(bufferedConstants>=CONST_BUFFER_CAP)
    handleError("constant buffer overflow",ERROR_MEMORY,pos);
  constBuffer[bufferedConstants++]=(ConstantValue){.type=constType,.valueAs.i64=constId,.pos=pos};
}
void pushStringConstant(String value,FilePosition pos){
  if(bufferedConstants>=CONST_BUFFER_CAP)
    handleError("constant buffer overflow",ERROR_MEMORY,pos);
  constBuffer[bufferedConstants++]=(ConstantValue){.type=CONSTANT_STRING,.valueAs.string=value,.pos=pos};
}
void pushTypeConstant(DataType type,FilePosition pos){
  if(bufferedConstants>=CONST_BUFFER_CAP)
    handleError("constant buffer overflow",ERROR_MEMORY,pos);
  constBuffer[bufferedConstants++]=(ConstantValue){.type=CONSTANT_TYPE,.valueAs.type=type,.pos=pos};
}
DataType popTypeConstant(FilePosition pos,char const* argumentName,bool allowVoid){
  if(bufferedConstants==0){
    fprintf(stderr,"missing %s\n",argumentName);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  bufferedConstants--;
  if(constBuffer[bufferedConstants].type!=CONSTANT_TYPE){
    fprintf(stderr,"wrong constant type for %s expected type got %s\n",argumentName,constTypeName(constBuffer[bufferedConstants].type));
    fputs(" declared at ",stderr);
    printFilePosition(constBuffer[bufferedConstants].pos,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  if(!allowVoid&&isVoidType(&constBuffer[bufferedConstants].valueAs.type)){
    fprintf(stderr,"missing %s\n",argumentName);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  return constBuffer[bufferedConstants].valueAs.type;
}
int64_t* popArraySize(FilePosition pos){
  arrayDimsCount=0;
  while(constBuffer[bufferedConstants-1].type!=CONSTANT_TYPE){
    bufferedConstants--;
    arrayDimsCount++;
  }
  for(int64_t i=0;i<arrayDimsCount;i++){
    if(constBuffer[bufferedConstants+i].type!=CONSTANT_INT){//XXX use _ to signal unknown dimension sizes
      fprintf(stderr,"unexpected constant for array size expected int got %s\n",constTypeName(constBuffer[bufferedConstants+i].type));
      handleError(NULL,ERROR_SYNTAX,pos);
    }
    if(constBuffer[bufferedConstants+i].valueAs.i64<=0){
      fprintf(stderr,"invalid array size: %"PRIi64" array sizes have to be greater than 0\n",constBuffer[bufferedConstants+i].valueAs.i64);
      handleError(NULL,ERROR_SYNTAX,pos);
    }
    arrayDimsBuffer[i]=constBuffer[bufferedConstants+i].valueAs.i64;
  }
  return arrayDimsBuffer;
}
DataType* popTypeConstants(size_t count,FilePosition pos,char const* argumentName,bool allowVoid){
  if(count>MAX_COMPOSITE_ELEMENTS){
    fprintf(stderr,"composite type exceeds maximum element count (%i)\n",MAX_COMPOSITE_ELEMENTS);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  for(int64_t i=count-1;i>=0;i--){
    compositeTypeBuffer[i]=popTypeConstant(pos,argumentName,allowVoid);
  }
  return compositeTypeBuffer;
}

#define LABEL_TYPE_NONE    0 // no labels
#define LABEL_TYPE_STRUCT  1 // exactly one label per type
#define LABEL_TYPE_ENUM    2 // labels without type are allowed
#define LABEL_TYPE_PROC_IN 3 // types are allowed to have labels, if one type has labels than all types have to have an label
 
bool readType(String name,CodeFile* codeFile,ParserState* state);

bool readConstants(String word,int wordType,CodeFile* codeFile,ParserState* state){
  FilePosition wordPos=codeFile->wordStart;
  if(wordType==WORD_TYPE_STRING){
    pushStringConstant(word,wordPos);
    return true;
  }
  if(wordType==WORD_TYPE_CHAR){
    if(word.length!=1){//TODO? handle Unicode characters
      fprintf(stderr,"character literal '%"PRI_STR"' contains more that one character\n",PRI_STR_ARGS(word));
      handleError(NULL,ERROR_SYNTAX,wordPos);
    }
    pushIntConstant(CONSTANT_CHAR,charAt(word,0),wordPos);
    return true;
  }
  IntOrErrorCode asInt=parseInt(word,0);//try to parse word as int
  if(!asInt.isError){
    pushIntConstant(CONSTANT_INT,asInt.as.i64,wordPos);
    return true;
  }
  if(asInt.as.error!=ERROR_PARSE_INT)
    handleError(NULL,asInt.as.error,wordPos);
  if(word.length==0)
    return true;
  return readType(word,codeFile,state);
}
//reads a composite type of the given type-class, the result is stored in the type buffer
//return 0 if a type was read, otherwise a nonzero error-code if a type error occurs this method will return a syntax error
void readCompositeType(TypeClass typeClass,CodeFile* codeFile,ParserState* state,int labelType,char const* endString,bool checkEmpty){
  String word;
  int wordType;
  size_t initOffset=bufferedConstants;
  int32_t labelOffset=labelBufferCount;
  size_t currentOffset=initOffset;
  int typesSinceLabel=0;//if there has been a type since the last label
  do{
    word=nextWord(codeFile,&wordType);
    if(readConstants(word,wordType,codeFile,state)){
      typesSinceLabel+=(bufferedConstants-currentOffset);
      currentOffset=bufferedConstants;
      continue;
    }
    if(wordEquals(&word,endString))
      break;
    if(labelType!=LABEL_TYPE_NONE&&typesSinceLabel>0&&wordEquals(&word,":")){//start label
      if(typesSinceLabel>1){
        fprintf(stderr,"too many types for field declaration expected 1 got %i\n",typesSinceLabel);
        handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
        return;
      }
      typesSinceLabel=0;
      readLabel(codeFile,"labels");//label is stored in label buffer
      continue;
    }
    if(labelType!=LABEL_TYPE_ENUM||typesSinceLabel>0||wordEquals(&word,"mut")){
      fprintf(stderr,"unknown type name '%"PRI_STR"' \n",PRI_STR_ARGS(word));
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
      return;
    }
    //untyped enum label
    newLabel(word,false,codeFile->wordStart);//label is stored in label buffer
    pushTypeConstant(primitiveType(PRIMITIVE_VOID),codeFile->wordStart);
    currentOffset=bufferedConstants;
    typesSinceLabel=0;
  }while(1);
  if(checkEmpty&&bufferedConstants==initOffset){
    handleError("empty composite type",ERROR_SYNTAX,codeFile->wordStart);
    return;
  }
  //search duplicate labels
  if(labelOffset!=labelBufferCount){
    for(LabelId i=labelOffset+1;i<labelBufferCount;i++){
      for(LabelId j=labelOffset;j<i;j++){
        if(stringCompare(getLabelName(i),getLabelName(j))==0){
          fprintf(stderr,"duplicate label '%"PRI_STR"' in %s \n",PRI_STR_ARGS(getLabelName(i)),typeClassName(typeClass));
          fputs("  previous declaration at ",stderr);
          printFilePosition(label(j,codeFile->wordStart).declaredAt,stderr);
          fputs("\n",stderr);
          handleError(NULL,ERROR_SYNTAX,label(i,codeFile->wordStart).declaredAt);
        }
      }
    }
  }
  size_t maxOffset=bufferedConstants;
  DataType* elements=popTypeConstants(maxOffset-initOffset,codeFile->wordStart,"composite elements",labelType==LABEL_TYPE_ENUM);
  if(labelType==LABEL_TYPE_NONE||(labelType==LABEL_TYPE_PROC_IN&&labelOffset==labelBufferCount)){
    pushTypeConstant(compositeType(typeClass,elements,LABEL_ID_UNKNOWN,maxOffset-initOffset),codeFile->wordStart);
  }else{
    if(typesSinceLabel>0){
      fprintf(stderr,"missing label in %s\n",typeClassName(typeClass));
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
      return;
    }
    if(typeClass==TYPECLASS_PROC_IN)
      typeClass=TYPECLASS_LABELED_PROC_IN;
    pushTypeConstant(compositeType(typeClass,elements,labelOffset,maxOffset-initOffset),codeFile->wordStart);
  }
  if(typeEquals(&(constBuffer[initOffset].valueAs.type),&TYPE_UNDEFINED)){//XXX? peek type
    handleError("unknown error while creating composite type",ERROR_SYNTAX,codeFile->wordStart);
    return;
  }
  if(checkEmpty&&maxOffset-initOffset==1){
    fputs("WARNING:\n  single element composite type: ",stderr);
    printTypeName(&(constBuffer[initOffset].valueAs.type),stderr);
    fputs(" at ",stderr);
    printFilePosition(codeFile->wordStart,stderr);
    fputs("\n",stderr);
  }
}
//reads a type starting with the identifier name, the result is stored in the type buffer
//return true if a type was read, false otherwise
bool readType(String name,CodeFile* codeFile,ParserState* state){
  if(name.length==0){
    handleError("empty type name",ERROR_MEMORY,codeFile->wordStart);
    return false;
  }
  if(bufferedConstants>=CONST_BUFFER_CAP){//buffer overflow
    handleError("exceeded type capacity",ERROR_MEMORY,codeFile->wordStart);
    return false;
  }
  //primitive types
  if(wordEquals(&name,"void")){
    handleError("using the void type directly is not supported",ERROR_SYNTAX,codeFile->wordStart);
    return false;
  }
  if(wordEquals(&name,"bool")){
    pushTypeConstant(primitiveType(PRIMITIVE_BOOL),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"i8")||wordEquals(&name,"char")){
    pushTypeConstant(primitiveType(PRIMITIVE_I8),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"i32")){
    pushTypeConstant(primitiveType(PRIMITIVE_I32),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"i64")){
    pushTypeConstant(primitiveType(PRIMITIVE_I64),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"float")){
    pushTypeConstant(primitiveType(PRIMITIVE_FLOAT),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"string")){
    pushTypeConstant(progStringType(),codeFile->wordStart);
    return true ;
  }
  //composite types
  int r;
  if(wordEquals(&name,"ptr")){
    int64_t* dims=popArraySize(codeFile->wordStart);
    DataType target=popTypeConstant(codeFile->wordStart,"pointer argument",false);
    if(arrayDimsCount>0){
      pushTypeConstant(arrayType(true,&target,arrayDimsCount,dims,false),codeFile->wordStart);
      return true;
    }
    if(target.typeClass==TYPECLASS_ARRAY){
      target.typeClass=TYPECLASS_ARRAY_VIEW;//pointer to array -> array view
      pushTypeConstant(target,codeFile->wordStart);
      return true;
    }
    pushTypeConstant(pointerType(&target,false),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"mut")){
    DataType target=popTypeConstant(codeFile->wordStart,"mutability argument",false);
    if(isMutableType(&target))
      handleError("type is already mutable",ERROR_TYPE,codeFile->wordStart);
    if(!makeMutable(&target)){
      fprintf(stderr,"%s types cannot be mutable\n",typeClassName(target.typeClass));
      handleError(NULL,ERROR_TYPE,codeFile->wordStart);
    }
    pushTypeConstant(target,codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"array")){
    int64_t* dims=popArraySize(codeFile->wordStart);
    DataType target=popTypeConstant(codeFile->wordStart,"array argument",false);
    if(arrayDimsCount==0){//array without size arguments
      pushTypeConstant(arrayType(false,&target,1,NULL,false),codeFile->wordStart);
      return true;
    }
    pushTypeConstant(arrayType(false,&target,arrayDimsCount,dims,false),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"proc(")){
    readCompositeType(TYPECLASS_PROC_IN,codeFile,state,LABEL_TYPE_PROC_IN,"=>",false);
    readCompositeType(TYPECLASS_PROC_OUT,codeFile,state,LABEL_TYPE_NONE,")",false);
    DataType out=popTypeConstant(codeFile->wordStart,"procedure input",false);
    DataType in=popTypeConstant(codeFile->wordStart,"procedure output",false);
    pushTypeConstant(procedureType(&in,&out),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"tuple(")||wordEquals(&name,"(")){
    readCompositeType(TYPECLASS_TUPLE,codeFile,state,LABEL_TYPE_NONE,")",true);
    return true;
  }
  if(wordEquals(&name,"struct(")){
    readCompositeType(TYPECLASS_STRUCT,codeFile,state,LABEL_TYPE_STRUCT,")",true);
    return true;
  }
  if(wordEquals(&name,"enum(")){
    readCompositeType(TYPECLASS_ENUM,codeFile,state,LABEL_TYPE_ENUM,")",true);
    return true;
  }
  ScopeNode* asIdentifier;
  r=getIdentifier(state->namespaceInfo,name,&asIdentifier);
  if(r<0){//internal error while reading identifier
    handleError("error while reading identifier",r,codeFile->wordStart);
    return false;
  }
  if(r>0||asIdentifier->idType!=ID_TYPE)//identifier does not exist / is not a type
    return false;
  //identifier
  if(typeEquals(&(asIdentifier->type),&TYPE_UNDEFINED))
    return false;
  pushTypeConstant(asIdentifier->type,codeFile->wordStart);
  if(asIdentifier->type.typeClass==TYPECLASS_OPAQUE){//ensure token after opaque type is ptr
    int wordType;
    String word=nextWord(codeFile,&wordType);
    if(wordType!=WORD_TYPE_IDENTIFIER){
      handleError("expected an identifier",ERROR_SYNTAX,codeFile->wordStart);
      return false;
    }
    if(!wordEquals(&word,"ptr")){
      fprintf(stderr,"unexpected word after opaque type '%"PRI_STR"' expected 'ptr' got '%"PRI_STR"'\n",PRI_STR_ARGS(name),PRI_STR_ARGS(word));
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
      return false;
    }
    DataType target=popTypeConstant(codeFile->wordStart,"pointer argument",false);
    pushTypeConstant(pointerType(&target,false),codeFile->wordStart);
  }
  return true;
}

void pushOperation(ParserState* state,Operation op);
void requireCompileTimeTypes(ParserState* state,String* opName,DataType* typeOut,size_t nTypes,FilePosition pos){
  if(bufferedConstants<nTypes){
    fprintf(stderr,"not enough type arguments for operation '%"PRI_STR"' need %zu got %zu\n",PRI_STR_ARGS(*opName),nTypes,bufferedConstants);
    handleError(NULL,ERROR_SYNTAX,pos);
    return;
  }
  for(size_t i=0;i<nTypes;i++){
    *(typeOut)=popTypeConstant(pos,"operation argument",false);
    if(typeEquals(typeOut,&TYPE_UNDEFINED))
      handleError("invalid type in type buffer",ERROR_TYPE,pos);
    typeOut++;
  }
  int64_t intVal;
  size_t constCount=bufferedConstants;
  bufferedConstants=0;//set constant count to 0 to prevent infinite recursion
  for(size_t i=0;i<constCount;i++){
    switch(constBuffer[i].type){
      case CONSTANT_STRING:
        intVal=addProgString(constBuffer[i].valueAs.string,constBuffer[i].pos);
        pushOperation(state,opConstant(progStringType(),intVal,constBuffer[i].pos));
        break;
      case CONSTANT_CHAR:
        pushOperation(state,opConstant(primitiveType(PRIMITIVE_I8),constBuffer[i].valueAs.charId,constBuffer[i].pos));
        break;
      case CONSTANT_INT:
        intVal=constBuffer[i].valueAs.i64;
        pushOperation(state,opConstant(primitiveType((intVal<=INT32_MAX&&intVal>=INT32_MIN)?PRIMITIVE_I32:PRIMITIVE_I64),intVal,constBuffer[i].pos));
        break;
      case CONSTANT_TYPE:
        handleError("unused type constant",ERROR_TYPE,constBuffer[i].pos);
        break;
    }
  }
}

//returns true when allocation fails
bool ensureCap(void** mList,size_t* cap,size_t eltSize,size_t newSize){
  if(*cap>newSize)
    return false;
  size_t newCap=newSize;
  newCap+=((*cap)>>4)+16;//add some space depending on previous capacity
  //round up to next multiple of 64
  newCap&=~0x3f;
  if((newSize&0x3f)!=0)
    newCap+=0x40;
  void* newList=realloc(*mList,newCap*eltSize);
  if(newList==NULL)
    return true;
  *mList=newList;
  *cap=newCap;
  return false;
}
bool ensureOpCap(Operation** mList,size_t* cap,size_t newSize){
  void* ops=*mList;
  if(ensureCap(&ops,cap,sizeof(Operation),newSize))
    return true;
  *mList=ops;
  return false;
}

void pushOperation(ParserState* state,Operation op){
  if(ensureOpCap(&state->parsedOps,&state->parsedOpCap,state->parsedOpCount+bufferedConstants+16)){
    handleError(NULL,ERROR_MEMORY,op.filePos);
  }
  if(bufferedConstants>0){
    requireCompileTimeTypes(state,&EMPTY_STRING,NULL,0,op.filePos);
  }
  state->parsedOps[state->parsedOpCount++]=op;
}
Operation* peekOperation(ParserState* state,FilePosition pos){
  if(state->parsedOpCount==0){
    handleError("operation underflow",ERROR_MEMORY,pos);
  }
  return &state->parsedOps[state->parsedOpCount-1];
}
void readOperation(ParserState* state,CodeFile* codeFile){
  int wordType;
  String word=nextWord(codeFile,&wordType);
  DataType type;
  FilePosition wordPos=codeFile->wordStart;
  if(readConstants(word,wordType,codeFile,state))//is type
    return;
  wordPos=codeFile->wordStart;
  //1. operations that take a Type as argument
  if(wordEquals(&word,":")){//pre-declare
    requireCompileTimeTypes(state,&word,&type,1,wordPos);
    LabelId labelId=readLabel(codeFile,"variable names");
    wordPos=codeFile->wordStart;
    Label varName=label(labelId,wordPos);
    IdentifierType idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    if(isCallableType(&type)&&!isPointerType(&type)){
      handleError("directly predeclaring procedures is not supported",ERROR_SYNTAX,wordPos);
    }
    if(type.typeClass==TYPECLASS_TYPE_OF){
      if(!typeEquals(type.typeDataAs.type,&TYPE_UNDEFINED)){
        fputs("cannot pre-declare values of type: ",stderr);
        printTypeName(&type,stderr);
        fputs("\n For pre-declaring a type use 'type' without any prefix",stderr);
        handleError(NULL,ERROR_UNIMPLEMENTED,wordPos);
      }
      type=opaqueType(state->opaqueTypeCount++);
      idType=ID_TYPE;
    }
    ScopeNode* id=declareIdentifier(state->namespaceInfo,labelId,type,idType,nextId(idType,state),wordPos);
    if(idType==ID_TYPE)//declaring type does not produce any code
      return;
    pushOperation(state,(Operation){.opType=OP_PRE_DECLARE,.dataType=type,.filePos=varName.declaredAt,.dataAs={.idInfo={.type=idType,.id=id->id,.labelId=labelId,.isMutable=varName.isMutable}}});
    return;
  }else if(wordEquals(&word,"=:")){//declare
    requireCompileTimeTypes(state,&word,&type,1,wordPos);
    LabelId labelId=readLabel(codeFile,"variable names");
    wordPos=codeFile->wordStart;
    Label varName=label(labelId,wordPos);
    IdentifierType idType;
    ScopeNode* id;
    int r;
    switch(type.typeClass){
      case TYPECLASS_PROCEDURE:
        idType=ID_PROCEDURE;
        break;
      case TYPECLASS_TYPE_OF:
        type=*type.typeDataAs.type;//unwrap typeOf
        if(typeEquals(&type,&TYPE_UNDEFINED)){
          handleError("missing type for type definition",ERROR_SYNTAX,wordPos);
        }
        idType=ID_TYPE;
        r=getIdentifier(state->namespaceInfo,varName.label,&id);
        if(r<0)
          handleError("error while resolving identifier",r,wordPos);
        if(r!=0||id->idType!=ID_TYPE||id->type.typeClass!=TYPECLASS_OPAQUE)
          break;//can only override opaque types
        DataType* overwrite=bufferedType(&id->type);
        if(overwrite==NULL)
          handleError("error while resolving type defintion",r,wordPos);
        *overwrite=type;//override entry in wrapped type list
        id->type=type;//override previous definition
        return;
      default:
        idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    }
    id=declareIdentifier(state->namespaceInfo,labelId,type,idType,nextId(idType,state),wordPos);
    if(idType==ID_TYPE){
      //declaring type does not produce any code
      return;
    }else if(idType==ID_PROCEDURE){
      if(state->scopeLevel>0){
        fprintf(stderr,"invalid position for procedure %"PRI_STR" procedures can only be declared at top level\n",PRI_STR_ARGS(varName.label));
          handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      Scope* newScope=openScope(BLOCK_PROCEDURE,state->namespaceInfo);
      if(newScope==NULL)
        handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
      state->currentScope=newScope;
      state->scopeLevel++;
      state->procScope=state->scopeLevel;
      state->currentProcId=type.typeDataAs.procedure->id;
      state->localVars=0;
      if(type.typeDataAs.procedure->inType->typeClass==TYPECLASS_LABELED_PROC_IN){
         CompositeType const* inTypes=type.typeDataAs.procedure->inType->typeDataAs.composite;
         for(int32_t i=0;i<inTypes->typeCount;i++){
            declareIdentifier(state->namespaceInfo,inTypes->labelOffset+i,inTypes->types[i],ID_ARGUMENT,i,wordPos);
         }
      }
    }
    pushOperation(state,(Operation){.opType=OP_DECLARE,.dataType=type,.filePos=varName.declaredAt,.dataAs={.idInfo={.type=idType,.id=id->id,.labelId=labelId,.isMutable=varName.isMutable}}});
    return;
  }else if(wordEquals(&word,"new")){
    if(state->parsedOpCount>0&&peekOperation(state,wordPos)->opType==OP_CONSTANT&&peekOperation(state,wordPos)->dataType.typeClass==TYPECLASS_ENUM_LABEL){
      //change enum label to enum declaration
      peekOperation(state,wordPos)->opType=OP_NEW;
      peekOperation(state,wordPos)->filePos=wordPos;
      peekOperation(state,wordPos)->dataType.typeClass=TYPECLASS_ENUM;//change type-class back to enum
      return;
    }
    requireCompileTimeTypes(state,&word,&type,1,wordPos);
    if(type.typeClass==TYPECLASS_TUPLE||type.typeClass==TYPECLASS_STRUCT){
      pushOperation(state,(Operation){.opType=OP_NEW,.dataType=type,.filePos=wordPos,.dataAs={.i64=0}});
      return;
    }
    printTypeName(&type,stderr);
    fputs(" is currently not supported for operator new\n",stderr);
    if(type.typeClass==TYPECLASS_ENUM)
      fputs(" to create an enum specify the label of the current value\n",stderr);
    handleError(NULL,ERROR_TYPE,wordPos);
  }else if(wordEquals(&word,"cast")){ 
    requireCompileTimeTypes(state,&word,&type,1,wordPos);
    pushOperation(state,(Operation){.opType=OP_CAST,.dataType=type,.filePos=wordPos,.dataAs={.sourceType=&TYPE_UNDEFINED}});
      return;
  }else if(wordEquals(&word,"type")){
    if(bufferedConstants==0){//type without arguments
      pushTypeConstant(typeOfType(&TYPE_UNDEFINED),wordPos);
      return;//type does not generate any operations
    }
    requireCompileTimeTypes(state,&word,&type,1,wordPos);
    pushTypeConstant(typeOfType(&type),wordPos);
    return;//type does not generate any operations
  }else if(word.length>1&&charAt(word,0)=='.'){
    word=sliceStart(word,1);//remove first character
    if(bufferedConstants==0||constBuffer[bufferedConstants-1].type!=CONSTANT_TYPE){
      IntOrErrorCode index=parseInt(word,10);
      if(!index.isError){
        pushOperation(state,(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,
          .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.labelId=LABEL_ID_UNKNOWN,.id=index.as.i64,.isMutable=false}}});
        return;
      }
      pushOperation(state,(Operation){.opType=OP_GET_LABEL,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.string=word}});
      return;
    }
    requireCompileTimeTypes(state,&word,&type,1,wordPos);//try to get type field of enum
    int64_t index;
    if(type.typeClass!=TYPECLASS_ENUM||((index=findLabel(type.typeDataAs.composite->labelOffset,type.typeDataAs.composite->typeCount,&word))==-1)){
      fputs("type ",stderr);
      printTypeName(&type,stderr);
      fprintf(stderr," does not have a field '%"PRI_STR"'\n",PRI_STR_ARGS(word));
      handleError(NULL,ERROR_TYPE,wordPos);
    }
    type.typeClass=TYPECLASS_ENUM_LABEL;//change type-class to enum-label
    pushOperation(state,opConstant(type,index,wordPos));
    return;
  }else if(word.length>1&&charAt(word,0)=='#'){//compiler command
    word.chars++;//remove first character
    word.length--;
    SlicedString args=sliceAtChar(word,':');
    word=args.head;
    //stack manipulation
    if(wordEquals(&word,"dup")){//XXX dup:N drop:N -> dup/drop multiple values at once
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_DUP}}});
      return;
    }else if(wordEquals(&word,"drop")){
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_DROP}}});
      return;
    }else if(wordEquals(&word,"swap")){//XXX rot:N:K -> stack rotation
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_SWAP}}});
      return;
    }else if(wordEquals(&word,"over")){
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_OVER}}});
      return;
    }
    //compile-time code
    if(wordEquals(&word,"namespace")){
      if(state->scopeLevel>0){
        fprintf(stderr,"#%"PRI_STR" can only be used at global level\n",PRI_STR_ARGS(word));
        handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      word=nextWord(codeFile,&wordType);
      wordPos=codeFile->wordStart;
      if(wordType!=WORD_TYPE_IDENTIFIER)
        handleError("namespace names have to be identifiers",ERROR_SYNTAX,wordPos);
      if(word.length==0||charAt(word,0)=='#'||containsChar(word,'.')){
        fprintf(stderr,"'%"PRI_STR"' is not a valid namespace name",PRI_STR_ARGS(word));
        handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      startNamespace(&state->namespaceInfo,word,wordPos);
      printf("opened namespace %"PRI_STR"\n",PRI_STR_ARGS(word));//DEBUG
      return;
    }else if(wordEquals(&word,"using")){
      word=nextWord(codeFile,&wordType);
      wordPos=codeFile->wordStart;
      if(wordType!=WORD_TYPE_IDENTIFIER)
        handleError("namespace names have to be identifiers",ERROR_SYNTAX,wordPos);
      printf("using namespace %"PRI_STR"\n",PRI_STR_ARGS(word));//DEBUG
      importNamespace(&state->namespaceInfo,word,wordPos);
      return;
    }else if(wordEquals(&word,"end")){
      if(state->scopeLevel>0){
        fprintf(stderr,"#%"PRI_STR" can only be used at global level\n",PRI_STR_ARGS(word));
        handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      endCompileTimeBlock(&state->namespaceInfo,wordPos);
      printf("closed namespace\n");//DEBUG
      return;
    }
    //compiler commands
    if(wordEquals(&word,"types")){
      int64_t count=-1;
      if(args.tail.length>0){
        IntOrErrorCode p=parseInt(args.tail,10);
        if(p.isError||p.as.i64<=0){
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got '%"PRI_STR"'\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
          handleError(NULL,ERROR_SYNTAX,wordPos);
        }
        count=p.as.i64;
      }
      pushOperation(state,(Operation){.opType=OP_COMPILER_INFO,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.compilerInfo={.infoType=COMPILERINFO_TYPES,.maxCount=count}}});
      return;
    }else if(wordEquals(&word,"stack")){
      int64_t count=-1;
      if(args.tail.length>0){
        IntOrErrorCode p=parseInt(args.tail,10);
        if(p.isError||p.as.i64<=0){
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got '%"PRI_STR"'\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
          handleError(NULL,ERROR_SYNTAX,wordPos);
        }
        count=p.as.i64;
      }
      pushOperation(state,(Operation){.opType=OP_COMPILER_INFO,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.compilerInfo={.infoType=COMPILERINFO_STACK,.maxCount=count}}});
      return;
    }else if(wordEquals(&word,"find")){
      LabelId labelId=readLabel(codeFile,"variable names");
      wordPos=codeFile->wordStart;
      Label varName=label(labelId,wordPos);
      ScopeNode* asIdentifier;
      int r=getIdentifier(state->namespaceInfo,varName.label,&asIdentifier);//try to parse variable as identifier
      if(r<0)//internal error while reading identifier
        handleError("error while resolving identifier",r,wordPos);
      if(r==0){//found identifier TODO print shadowed matches
        puts("-----------------");
        printf("identifier '%"PRI_STR"':\n",PRI_STR_ARGS(varName.label));
        fputs("  ",stdout);
        Label mLabel=label(asIdentifier->labelId,wordPos);
        if(mLabel.isMutable)
          fputs("mutable ",stdout);
        printf("%s: ",idNames[asIdentifier->idType]);
        printTypeName(&asIdentifier->type,stdout);
        fputs("\n    at ",stdout);
        printFilePosition(mLabel.declaredAt,stdout);
        puts("");
        puts("-----------------");
        return;
      }
      fprintf(stderr,"could not find identifier '%"PRI_STR"'\n",PRI_STR_ARGS(varName.label));
      //TODO resolve global identifiers with later declarations pre-declared types identifiers
      return;
    }
    //XXX more compile time operations
    fprintf(stderr,"unknown compile time operation '%"PRI_STR"'\n",PRI_STR_ARGS(word));
    handleError(NULL,ERROR_SYNTAX,wordPos);
  }
  if(wordEquals(&word,"true")){
    pushOperation(state,opConstant(primitiveType(PRIMITIVE_BOOL),1,wordPos));
    return;
  }else if(wordEquals(&word,"false")){
    pushOperation(state,opConstant(primitiveType(PRIMITIVE_BOOL),0,wordPos));
    return;
  }else if(wordEquals(&word,"+")){
    pushOperation(state,opBinaryOperator(ADD,wordPos));
    return;
  }else if(wordEquals(&word,"-")){
    pushOperation(state,opBinaryOperator(SUBTRACT,wordPos));
    return;
  }else if(wordEquals(&word,"*")){
    pushOperation(state,opBinaryOperator(MULTIPLY,wordPos));
    return;
  }else if(wordEquals(&word,"/")){
    pushOperation(state,opBinaryOperator(DIVIDE,wordPos));
    return;
  }else if(wordEquals(&word,"%")){
    pushOperation(state,opBinaryOperator(MOD,wordPos));
    return;
  }else if(wordEquals(&word,"&")){
    pushOperation(state,opBinaryOperator(AND,wordPos));
    return;
  }else if(wordEquals(&word,"|")){
    pushOperation(state,opBinaryOperator(OR,wordPos));
    return;
  }else if(wordEquals(&word,"^")){
    pushOperation(state,opBinaryOperator(XOR,wordPos));
    return;
  }else if(wordEquals(&word,"&&")){//XXX implement short-circuit  and/or using code-blocks
    handleError("short circuit operations are currently not supported",ERROR_UNIMPLEMENTED,wordPos);
  }else if(wordEquals(&word,"||")){
    handleError("short circuit operations are currently not supported",ERROR_UNIMPLEMENTED,wordPos);
  }else if(wordEquals(&word,"==")){
    pushOperation(state,opBinaryOperator(EQ,wordPos));
    return;
  }else if(wordEquals(&word,"!=")){
    pushOperation(state,opBinaryOperator(NE,wordPos));
    return;
  }else if(wordEquals(&word,">")){
    pushOperation(state,opBinaryOperator(GT,wordPos));
    return;
  }else if(wordEquals(&word,">=")){
    pushOperation(state,opBinaryOperator(GE,wordPos));
    return;
  }else if(wordEquals(&word,"<=")){
    pushOperation(state,opBinaryOperator(LE,wordPos));
    return;
  }else if(wordEquals(&word,"<")){
    pushOperation(state,opBinaryOperator(LT,wordPos));
    return;
  }else if(wordEquals(&word,"neg")||wordEquals(&word,"negate")){
    pushOperation(state,opUnaryOperator(NEGATE,wordPos));
    return;
  }else if(wordEquals(&word,"++")){
    pushOperation(state,opUnaryOperator(INCREMENT,wordPos));
    return;
  }else if(wordEquals(&word,"--")){
    pushOperation(state,opUnaryOperator(DECREMENT,wordPos));
    return;
  }else if(wordEquals(&word,"=::")){//automatically choose type of declared variable
    LabelId labelId=readLabel(codeFile,"variable names");
    wordPos=codeFile->wordStart;
    Label varName=label(labelId,wordPos);
    wordPos=codeFile->wordStart;
    IdentifierType idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    DataType mType=TYPE_UNDEFINED;
    mType.typeDataAs.typeId=++state->predeclaredTypes;//store predeceased id in type
    ScopeNode* id=declareIdentifier(state->namespaceInfo,labelId,mType,idType,nextId(idType,state),wordPos);
    pushOperation(state,(Operation){.opType=OP_DECLARE,.dataType=mType,.filePos=varName.declaredAt,.dataAs={.idInfo={.type=idType,.id=id->id,.labelId=labelId,.isMutable=varName.isMutable}}});
    return;
  }else if(wordEquals(&word,"=")){
    if(state->parsedOpCount>0){
      switch(peekOperation(state,wordPos)->opType){
        case OP_GET:
          peekOperation(state,wordPos)->opType=OP_SET;
          return;
        case OP_GET_LABEL:
          peekOperation(state,wordPos)->opType=OP_SET_LABEL;
          return;
        case OP_IDENTIFIER:
          peekOperation(state,wordPos)->opType=OP_SET_IDENTIFIER;
          return;
        default:
          printf("cannot set operations of type %s\n",opName(peekOperation(state,wordPos)->opType));
          break;
      }
    }
    handleError("unexpected = operation",ERROR_SYNTAX,wordPos);
  }else if(wordEquals(&word,"@")){
    pushOperation(state,(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,
      .dataAs={.idInfo={.type=ID_POINTER,.id=0,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}});
    return;
  }else if(wordEquals(&word,"[]")){
    pushOperation(state,(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,
      .dataAs={.idInfo={.type=ID_ARRAY_ELEMENT,.id=0,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}});
    return;
  }else if(wordEquals(&word,"addrOf")){
    if(state->parsedOpCount>0&&peekOperation(state,wordPos)->opType==OP_CALL){
      peekOperation(state,wordPos)->dataType=asUnlabeledProc(&peekOperation(state,wordPos)->dataType,wordPos);
      peekOperation(state,wordPos)->opType=OP_GET;
    }
    if(state->parsedOpCount>0&&peekOperation(state,wordPos)->opType==OP_IDENTIFIER)
      peekOperation(state,wordPos)->opType=OP_IDENTIFIER_ADDRESS;
    pushOperation(state,(Operation){.opType=OP_ADDR_OF,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}});
    return;
  }else if(wordEquals(&word,"()")){
    pushOperation(state,(Operation){.opType=OP_CALL_PTR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}});
    return;
  }else if(wordEquals(&word,"if")){
    Scope* newScope=openScope(BLOCK_IF,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    state->scopeLevel++;
    pushOperation(state,opCodeBlock(BLOCK_IF,wordPos));
    return;
  }else if(wordEquals(&word,"_if")){
    //no scope change for _if
    pushOperation(state,opCodeBlock(BLOCK_IF2,wordPos));
    return;
  }else if(wordEquals(&word,"while")){
    Scope* newScope=openScope(BLOCK_WHILE,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    state->scopeLevel++;
    
    pushOperation(state,opCodeBlock(BLOCK_WHILE,wordPos));
    return;
  }else if(wordEquals(&word,"do")){//!!while syntax is different fro C:  WHILE cond DO exrp END   do-While: WHILE exrp cond DO END
    closeScope(&state->namespaceInfo);
    Scope* newScope=openScope(BLOCK_WHILE,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    //scope count does not change
        
    pushOperation(state,opCodeBlock(BLOCK_DO,wordPos));
    return;
  }else if(wordEquals(&word,"else")){
    closeScope(&state->namespaceInfo);
    Scope* newScope=openScope(BLOCK_ELSE,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    //scope count does not change
    
    pushOperation(state,opCodeBlock(BLOCK_ELSE,wordPos));
    return;
  }else if(wordEquals(&word,"break")){
    //current code-block does not change
    pushOperation(state,opCodeBlock(BLOCK_BREAK,wordPos));
    return;
  }else if(wordEquals(&word,"continue")){
    //current code-block does not change
    pushOperation(state,opCodeBlock(BLOCK_CONTINUE,wordPos));
    return;
  }else if(wordEquals(&word,"switch")){
    Scope* newScope=openScope(BLOCK_SWITCH,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    state->scopeLevel++;
    pushOperation(state,opCodeBlock(BLOCK_SWITCH,wordPos));
    return;
  }else if(wordEquals(&word,"case")){
    closeScope(&state->namespaceInfo);
    Scope* newScope=openScope(BLOCK_CASE,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    //scope count does not change
        
    pushOperation(state,opCodeBlock(BLOCK_CASE,wordPos));
    return;
  }else if(wordEquals(&word,"default")){
    closeScope(&state->namespaceInfo);
    Scope* newScope=openScope(BLOCK_CASE,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    //scope count does not change
        
    pushOperation(state,opCodeBlock(BLOCK_DEFAULT,wordPos));
    return;
  }else if(wordEquals(&word,"end")){
    closeScope(&state->namespaceInfo);
    state->scopeLevel--;
    if(state->scopeLevel<state->procScope){//exited procedure
      state->currentProcId=-1;
      state->procScope=-1;
    }
    
    pushOperation(state,opEndCodeBlock(BLOCK_UNKNOWN,wordPos));
    return;
  }else if(wordEquals(&word,"return")){
    if(state->currentProcId<0){
      handleError("unexpected return statement",ERROR_SYNTAX,wordPos);
    }
    pushOperation(state,(Operation){.opType=OP_RETURN,.dataType=*procTypes[state->currentProcId].outType,.filePos=wordPos,.dataAs={0}});
    return;
  }else if(wordEquals(&word,"entryPoint:")){
    if(state->entryPointIndex!=-1){
      fputs("program already has an entry point\n  at ",stderr);
      printFilePosition(state->parsedOps[state->entryPointIndex].filePos,stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_SYNTAX,wordPos);
    }
    Scope* newScope=openScope(BLOCK_PROCEDURE,state->namespaceInfo);
    if(newScope==NULL)
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    state->currentScope=newScope;
    state->scopeLevel++;
    pushOperation(state,(Operation){.opType=ENTRY_POINT,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}});
    state->entryPointIndex=state->parsedOpCount;
    return;
  }else if(wordEquals(&word,"print")){
    pushOperation(state,(Operation){.opType=OP_PRINT,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}});//printed type will be determined by type-checker
    return;
  }else if(wordEquals(&word,"mut")){
    handleError("mut can only be used after types or declaration operations ( ':' '=:' '=::' )",ERROR_SYNTAX,wordPos);
    return;
  } 
  
  ScopeNode* asIdentifier;
  int r=getIdentifier(state->namespaceInfo,word,&asIdentifier);//try to parse variable as identifier
  if(r<0)//internal error while reading identifier
    handleError("error while resolving identifier",r,wordPos);
  if(r==0){//identifier
    Label mLabel=label(asIdentifier->labelId,wordPos);
    pushOperation(state,(Operation){.opType=asIdentifier->idType==ID_PROCEDURE?OP_CALL:OP_GET,
      .dataType=asIdentifier->type,.filePos=wordPos,.dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id,.labelId=asIdentifier->labelId,.isMutable=mLabel.isMutable}}});
    return;
  }
  // could not find identifier, try again in type-check phase
  pushOperation(state,(Operation){.opType=OP_IDENTIFIER,.dataType=TYPE_UNDEFINED,.filePos=wordPos,
    .dataAs={.localLabel={.label=newLabel(word,false,codeFile->wordStart),.namespaceInfo=state->namespaceInfo}}});
}


Program compileToOps(CodeFile* codeFile){
  size_t opsCap=256;
  Operation* parsedOps=malloc(opsCap*sizeof(Operation));
  if(parsedOps==NULL){
    handleError(NULL,ERROR_MEMORY,codeFile->currentPos);
    exit(ERROR_MEMORY);
  }
  NamespaceInfo namespaceInfo=(NamespaceInfo){.current=0,.namespaceImports=NAMESPACE_IMPORT_NONE};
  openScope(BLOCK_UNKNOWN,namespaceInfo);
  ParserState state=(ParserState){.namespaceInfo=namespaceInfo,.currentScope=scopeBuffer,
    .currentProcId=-1,.procScope=0,.localVars=0,.globalVars=0,.scopeLevel=0,.entryPointIndex=-1,.predeclaredTypes=0
    ,.parsedOps=parsedOps,.parsedOpCap=opsCap,.parsedOpCount=0};
  parsedOps=NULL;//null out parsed ops to prevent access after reallocation during compilation
  while(codeFile->codeSize>0){
    readOperation(&state,codeFile);
  }
  return (Program){.ops=state.parsedOps,.opCount=state.parsedOpCount,.globalOps=NULL,.globalScope=scopeBuffer,.hasEntryPoint=state.entryPointIndex!=-1,.predeclaredTypes=state.predeclaredTypes};
}

void typeErrorMessage(char const* exprName,DataType expected,DataType got){
  fprintf(stderr,"wrong type for %s: expected ",exprName);
  printTypeName(&expected,stderr);
  fputs(" got ",stderr);
  printTypeName(&got,stderr);
  fputs("\n",stderr);
}
typedef struct{
  bool isError;
  union{
    DataType* type;
    int error;
  } as;
}TypeOrError;

DataType typeCheckPointerArithmetic(DataType* inTypes,bool subtract){
  if(!isPointerType(inTypes+0)||isCallableType(inTypes+0))
    return TYPE_UNDEFINED;//inTypes[0] is no pointer
  if(isIntType(&(inTypes[1])))
    return inTypes[0];
  if(subtract&&typeEquals(inTypes+0,inTypes+1)){//XXX? ptr - ptr mut
    return primitiveType(PRIMITIVE_I64);
  }
  return TYPE_UNDEFINED;
}
DataType typeCheckArithmetic(DataType* inTypes){
  if(!isPrimitiveType(&(inTypes[0]))||!isPrimitiveType(&(inTypes[1])))
    return TYPE_UNDEFINED;//arithmetic only on primitive types
  int r1=numberRank(inTypes[0].typeDataAs.primitive);
  int r2=numberRank(inTypes[1].typeDataAs.primitive);
  if(isInteger(inTypes[0].typeDataAs.primitive)!=isInteger(inTypes[1].typeDataAs.primitive))
    return TYPE_UNDEFINED;//implicit int to float conversion
  if(r1<=0||r2<=0)
    return TYPE_UNDEFINED;
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_VOID)
    return TYPE_UNDEFINED;
  inTypes[0]=primitiveType(res);
  inTypes[1]=inTypes[0];
  return inTypes[0];
}
DataType typeCheckCompare(DataType* inTypes){
  if(!isPrimitiveType(&(inTypes[0]))||!isPrimitiveType(&(inTypes[1])))
    return TYPE_UNDEFINED;//comparison only on primitive types
  int r1=numberRank(inTypes[0].typeDataAs.primitive);
  int r2=numberRank(inTypes[1].typeDataAs.primitive);
  if(isInteger(inTypes[0].typeDataAs.primitive)!=isInteger(inTypes[1].typeDataAs.primitive))
    return TYPE_UNDEFINED;//implicit int to float conversion
  if(r1<=0||r2<=0)
    return TYPE_UNDEFINED;//comparison only between numbers
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_VOID)
    return TYPE_UNDEFINED;
  inTypes[0]=primitiveType(res);
  inTypes[1]=inTypes[0];
  return primitiveType(PRIMITIVE_BOOL);
}
DataType typeCheckIntLogic(DataType* inTypes){
  if(!isIntType(&(inTypes[0]))||!isIntType(&(inTypes[1])))
    return TYPE_UNDEFINED;//both arguments have to be integers
  int r1=numberRank(inTypes[0].typeDataAs.primitive);
  int r2=numberRank(inTypes[1].typeDataAs.primitive);
  //r1 and r2 both are valid numbers
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_VOID)
    return TYPE_UNDEFINED;
  inTypes[0]=primitiveType(res);
  inTypes[1]=inTypes[0];
  return inTypes[0];
}

typedef struct{
  DataType type;
  int32_t opCount;
  bool isAddressable;
  bool isWritable;
}TypeInfo;

typedef struct{
  TypeInfo* types;
  size_t typeCount;
  Operation* ops;
  size_t opCount;

}StackState;
typedef struct{
  StackState inStack;
  StackState outStack;
  
  size_t elsePos;
  int32_t elifCount;
  bool endReachable;
}IfBlockInfo;
typedef struct{
  StackState inStack;
  StackState outStack;
  
  bool hasDo;
  bool hasBreak;
}WhileBlockInfo;
typedef struct{
  StackState inStack;
  StackState outStack;
  
  DataType switchType;
  
  SwitchData* switchData;
  
  bool endReachable;
  bool explicitBreak;
}SwitchBlockInfo;
typedef struct{
  DataType returnType;
}ProcedureBlockInfo;
typedef struct{
  BlockType type;
  size_t blockStart;
  int32_t blockId;
  union{
    int64_t i64;
    IfBlockInfo ifBlock;
    WhileBlockInfo whileBlock;
    SwitchBlockInfo switchBlock;
    ProcedureBlockInfo procBlock;
  }blockDataAs;
}BlockInfo;
typedef struct{
  Operation* globalOperations;
  size_t globalCap;
  size_t globalCount;
  
  Operation* compiledOperations;
  size_t opCap;
  size_t opCount;
  Operation* opStack;
  size_t opStackCap;
  size_t opStackCount;
  TypeInfo* typeStack;
  size_t typeStackCap;
  size_t typeCount;  
  
  BlockInfo* openBlocks;
  size_t blockCap;
  size_t blockCount;
  
  Scope* globalScope;
  
  int32_t tmpCount;   // number of temp vars that have been created
  int32_t ifCount;    // number of created if-statements
  int32_t whileCount; // number of created while statements
  
  size_t index;
  int32_t nPredeclaredTypes;
  DataType* predeclaredTypes;
  bool reachable;//is current code position reachable
  bool hasCheckBounds;
  bool hasCheckEnum;
}TypeCheckState;

//prints the type stack, if maxTypes>=0 only maxTypes many elements are printed
void printTypeStack(TypeCheckState* state,bool printOps,int64_t maxTypes,FILE* out){
  size_t offset=state->opStackCount;
  for(int64_t k=state->typeCount-1;k>=0;k--){
    if(maxTypes--==0)
      return;//reached limit
    if(state->typeStack[k].isAddressable)
      fputs("addressable ",out);
    if(state->typeStack[k].isWritable)
      fputs("writable ",out);
    printTypeName(&(state->typeStack[k].type),out);
    if(!printOps){
      fputs("\n",out);
      continue;
    }
    fprintf(out," %"PRIi32":\n",state->typeStack[k].opCount);
    offset-=state->typeStack[k].opCount;
    for(int32_t i=0;i<state->typeStack[k].opCount;i++){
      fputs("    ",out);//indent operations
      printOperation(state->opStack[offset+i],out);
    }
  }
}
void printTypesDebug(TypeCheckState* state,char const* label){
  printf("--------------\n%s:\n",label);
  printTypeStack(state,true,-1,stdout);
  puts("--------------");
}

bool ensureGlobalOpCap(TypeCheckState* state,size_t newSize){
  return ensureOpCap(&(state->globalOperations),&(state->globalCap),newSize);
}
bool ensureCompiledOpCap(TypeCheckState* state,size_t newSize){
  return ensureOpCap(&(state->compiledOperations),&(state->opCap),newSize);
}
bool ensureOpStackCap(TypeCheckState* state,size_t newSize){
  return ensureOpCap(&(state->opStack),&(state->opStackCap),newSize);
}
bool ensureTypeStackCap(TypeCheckState* state,size_t newSize){
  void* mList=state->typeStack;
  bool res=ensureCap(&mList,&(state->typeStackCap),sizeof(TypeInfo),newSize);
  state->typeStack=(TypeInfo*)mList;
  return res;
}
bool pushBlock(TypeCheckState* state,BlockInfo newBlock){
  //ensure cap
  void* blocks=state->openBlocks;
  if(ensureCap(&blocks,&(state->blockCap),sizeof(BlockInfo),state->blockCount+1))
    return true;
  state->openBlocks=(BlockInfo*)blocks;
  state->openBlocks[state->blockCount++]=newBlock;
  return false;
}
BlockInfo* peekBlock(TypeCheckState* state){
  if(state->blockCount>0)
    return state->openBlocks+(state->blockCount-1);
  return NULL;
}
BlockInfo popBlock(TypeCheckState* state){
  if(state->blockCount>0)
    return state->openBlocks[--state->blockCount];
  return (BlockInfo){.type=BLOCK_UNKNOWN,.blockId=-1,.blockStart=0,.blockDataAs={0}};
}
BlockInfo* findBreakableBlock(TypeCheckState* state,bool breakLoop,bool breakSwitch){
  for(int64_t i=state->blockCount-1;i>=0;i--){
    if(breakLoop&&(state->openBlocks[i].type==BLOCK_WHILE||state->openBlocks[i].type==BLOCK_DO))
      return state->openBlocks+i;
    if(breakSwitch&&(state->openBlocks[i].type==BLOCK_CASE||state->openBlocks[i].type==BLOCK_DEFAULT))
      return state->openBlocks+i;
  }
  return NULL;
}
void freeContents(TypeCheckState* state){
  free(state->globalOperations);
  state->globalOperations=NULL;
  free(state->compiledOperations);
  state->compiledOperations=NULL;
  free(state->opStack);
  state->opStack=NULL;
  free(state->typeStack);
  state->typeStack=NULL;
  free(state->openBlocks);
  state->openBlocks=NULL;
  free(state->predeclaredTypes);
  state->predeclaredTypes=NULL;
}

TypeInfo* peekTypeStack(TypeCheckState* state){
  return &state->typeStack[state->typeCount-1];
}
void setTypeStackTypeOffset(TypeCheckState* state,size_t offset,DataType newType){
  state->typeStack[state->typeCount-offset].type=newType;
  state->typeStack[state->typeCount-offset].isAddressable=false;
  state->typeStack[state->typeCount-offset].isWritable=false;
}
void setTypeStackType(TypeCheckState* state,DataType newType){
  setTypeStackTypeOffset(state,1,newType);
}
void setTypeStackFlags(TypeCheckState* state,bool addressable,bool writable){
  state->typeStack[state->typeCount-1].isAddressable=addressable;
  state->typeStack[state->typeCount-1].isWritable=writable;
}

bool checkNonemptyStack(TypeCheckState* state,char const* message){
  if(state->opStackCount>0){
    fputs(message,stderr);
    fprintf(stderr,": %s at ",opName(state->opStack[0].opType));
    printFilePosition(state->opStack[0].filePos,stderr);
    fputs("\n",stderr);
    return true;
  }
  return false;
}
bool resetStack(TypeCheckState* state,StackState* prevState){
  if(ensureOpStackCap(state,prevState->opCount)||ensureTypeStackCap(state,prevState->typeCount))
    return true;
  state->typeCount=prevState->typeCount;
  state->opStackCount=prevState->opCount;
  memcpy(state->typeStack,prevState->types,prevState->typeCount*sizeof(TypeInfo));
  memcpy(state->opStack,prevState->ops,prevState->opCount*sizeof(Operation));
  return false;
}

void pushCompiledOperation(TypeCheckState* state,Operation op){
  if(state->blockCount==0){
    if(ensureGlobalOpCap(state,state->globalCount+1)){
      handleError("exceeded global operation capacity",ERROR_MEMORY,op.filePos);
    }
    state->globalOperations[state->globalCount++]=op;
    return;
  }
  if(ensureCompiledOpCap(state,state->opCount+1)){
    handleError("exceeded operation capacity",ERROR_MEMORY,op.filePos);
  }
  state->compiledOperations[state->opCount++]=op;
}
void pushCompiledOperations(TypeCheckState* state,Operation* ops,size_t count){
  if(count==0)
    return;//nothing to do
  if(state->blockCount==0){
    if(ensureGlobalOpCap(state,state->globalCount+count)){
      handleError("exceeded global operation capacity",ERROR_MEMORY,ops->filePos);
    }
    memcpy(state->globalOperations+state->globalCount,ops,count*sizeof(Operation));
    state->globalCount+=count;
    return;
  }
  if(ensureCompiledOpCap(state,state->opCount+count)){
    handleError("exceeded operation capacity",ERROR_MEMORY,ops->filePos);
  }
  memcpy(state->compiledOperations+state->opCount,ops,count*sizeof(Operation));
  state->opCount+=count;
}

//append the first stackOps operations from the stack to the program, remove types elements from the type-stack
//if appendOp is true op will be appended to the program (before any stack operations are appended)
//already allocate space for skippedStackOps 
void addCompiledStackOps(TypeCheckState* state,Operation op,size_t types,bool appendOp){
  size_t stackOps=0;
  for(size_t i=0;i<types;i++)
    stackOps+=state->typeStack[state->typeCount-1-i].opCount;
  state->opStackCount-=stackOps;
  state->typeCount-=types;
  if(appendOp)
    pushCompiledOperation(state,op);
  pushCompiledOperations(state,state->opStack+state->opStackCount,stackOps);
}
//append op and the first types operations from the stack to the program
void addCompiledOps(TypeCheckState* state,Operation op,size_t types){
  addCompiledStackOps(state,op,types,true);
}

//ensures that none of the top type-count stack elements is a composite operation 
void extractCompositeOps(TypeCheckState* state,size_t nStackValues,bool keepWritable){
  if(state->blockCount==0)//don't extract composites at global level
    return;
  size_t offset=state->opStackCount;
  for(size_t i=1;i<=nStackValues;i++){
    offset-=state->typeStack[state->typeCount-i].opCount;
  }
  size_t newOffset=offset;
  bool reference;
  for(size_t i=state->typeCount-nStackValues;i<state->typeCount;i++){
    //extract multi-element operations and array constants to tmp variable
    if(state->typeStack[i].opCount>1||(state->opStack[offset].opType==OP_CONSTANT&&isArrayType(&(state->opStack[offset].dataType)))){
      reference=false;
      if(keepWritable&&state->typeStack[i].isWritable){
        if(!state->typeStack[i].isAddressable){
          memmove(state->opStack+newOffset,state->opStack+offset,state->typeStack[i].opCount);
          newOffset+=state->typeStack[i].opCount;
          continue;
        }
        reference=true;
      }
      int32_t tmpId=state->tmpCount++;
      DataType type=state->typeStack[i].type;
      if(reference)
        type=pointerType(&type,true);
      pushCompiledOperation(state,opDeclareIntermediate(&type,tmpId,state->opStack[offset].filePos));
      if(reference)
        pushCompiledOperation(state,(Operation){.opType=OP_ADDR_OF,.dataType=type,.filePos=state->opStack[offset].filePos,.dataAs={0}});
      pushCompiledOperations(state,state->opStack+offset,state->typeStack[i].opCount);
      if(reference)
        state->opStack[newOffset++]=(Operation){.opType=OP_GET,.dataType=state->typeStack[i].type,.filePos=state->opStack[offset].filePos,
            .dataAs={.idInfo={.type=ID_POINTER,.id=0,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
      state->opStack[newOffset++]=opGetIntermediate(&type,tmpId,state->opStack[offset].filePos);
      offset+=state->typeStack[i].opCount;
      state->typeStack[i].opCount=reference?2:1;
      state->typeStack[i].isAddressable=false;
      if(!keepWritable)
        state->typeStack[i].isWritable=false;
      continue;
    }
    if(newOffset!=offset)
      state->opStack[newOffset]=state->opStack[offset];
    offset++;
    newOffset++;
  }
  state->opStackCount=newOffset;
}


bool predeclareBlockVariables(TypeCheckState* state,size_t blockStart,StackState* blockStack){
  if(ensureCompiledOpCap(state,state->opCount+blockStack->typeCount))
    return true;
  memmove(state->compiledOperations+blockStart+blockStack->typeCount,state->compiledOperations+blockStart,(state->opCount-blockStart)*sizeof(Operation));
  for(size_t i=0;i<blockStack->typeCount;i++){
    state->compiledOperations[blockStart+i]=blockStack->ops[i];
    state->compiledOperations[blockStart+i].opType=OP_PRE_DECLARE;
  }
  state->opCount+=blockStack->typeCount;
  return false;
}
void declareBlockVariables(TypeCheckState* state,size_t blockStart,StackState* typeSource,StackState* valueSource,char const* blockName,FilePosition pos){
  if(typeSource->typeCount!=valueSource->typeCount){
    fprintf(stderr,"different branches of %s statement do not match up\n",blockName);//XXX print branches
    handleError(NULL,ERROR_TYPE,pos);
  }
  size_t count=typeSource->typeCount+valueSource->opCount;
  if(ensureCompiledOpCap(state,state->opCount+count))
    handleError("error while reallocating operations",ERROR_MEMORY,pos);
  memmove(state->compiledOperations+blockStart+count,state->compiledOperations+blockStart,(state->opCount-blockStart)*sizeof(Operation));
  size_t opOffset=blockStart,inTypesOffset=0;
  for(size_t i=0;i<typeSource->typeCount;i++){
    if(!typeEquals(&typeSource->types[i].type,&valueSource->types[i].type)){
      fprintf(stderr,"different branches of %s statement do not match up\n",blockName);
      fputs("expected ",stderr);
      printTypeName(&valueSource->types[i].type,stderr);
      fputs(" got ",stderr);
      printTypeName(&typeSource->types[i].type,stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_TYPE,pos);
    }
    state->compiledOperations[opOffset]=typeSource->ops[i];
    state->compiledOperations[opOffset].opType=OP_DECLARE;
    opOffset++;
    memcpy(state->compiledOperations+opOffset,valueSource->ops+inTypesOffset,(valueSource->types[i].opCount)*sizeof(Operation));
    inTypesOffset+=valueSource->types[i].opCount;
  }
  state->opCount+=count;
}
/*
function for stack update at start/end of loop section
stores all variables currently on the stack in temporary variables and clears the stack
parameters:
state           TypeCheckState
stackState      state in which the current data will be stored
expectedState   state with which the current data will be compared
errorMessage    statement name that will be displayed on type error returned message is "... at end of ... "
initStackState  if true the stack state will be initialized, otherwise it is only used to read the variable-ids
declare         if true temporary variables will be declared instead of set, use this only to declare variables before entering the respective code-block
ignoreFirst     if true the top element on the type-stack will be ignored and the type-stack will be kept intact
pos             current file position (for error reporting) 
*/
void storeStackValues(TypeCheckState* state,StackState* stackState,StackState* expectedState,char const* errorMessage,bool initStackState,bool declare,bool ignoreFirst,FilePosition pos){
  size_t typeCount=state->typeCount-(ignoreFirst?1:0);
  if(initStackState){
    stackState->typeCount=typeCount;
    stackState->opCount=typeCount;
    stackState->types=typeCount==0?NULL:malloc((typeCount)*sizeof(TypeInfo));
    stackState->ops=typeCount==0?NULL:malloc((typeCount)*sizeof(Operation));
  }
  if(typeCount>0&&(stackState->types==NULL||stackState->ops==NULL)){
      handleError(NULL,ERROR_MEMORY,pos);
  }
  if(!initStackState){
    if(typeCount!=expectedState->typeCount){
      fprintf(stderr,"wrong number of types at end of %s expected %zu got %zu\n",errorMessage,expectedState->typeCount,typeCount);
      handleError(NULL,ERROR_TYPE,pos);
    }
  }
  int32_t varId;
  size_t offset=state->opStackCount;
  if(ignoreFirst)
    offset-=state->typeStack[typeCount].opCount;
  for(int64_t i=typeCount-1;i>=0;i--){
    if(!initStackState&&!typeEquals(&(state->typeStack[i].type),&(expectedState->types[i].type))){
      fprintf(stderr,"wrong type at end of %s expected ",errorMessage);
      printTypeName(&(expectedState->types[i].type),stderr);
      fputs(" got ",stderr);
      printTypeName(&(state->typeStack[i].type),stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_TYPE,pos);
    }
    varId=initStackState?state->tmpCount++:stackState->ops[i].dataAs.idInfo.id;
    //save stack-elements to tmp-values
    if(declare){
      pushCompiledOperation(state,opDeclareTmpVar(&(state->typeStack[i].type),varId,pos));
    }else{
      pushCompiledOperation(state,opSetTmpVar(&(state->typeStack[i].type),varId,pos));
    }
    pushCompiledOperations(state,state->opStack+offset-state->typeStack[i].opCount,state->typeStack[i].opCount);
    offset-=state->typeStack[i].opCount;
    if(!ignoreFirst){//remove element from stack if ignoreFirst false
      state->opStackCount-=state->typeStack[i].opCount;
      state->typeCount--;
    }
    if(initStackState){
      stackState->types[i]=(TypeInfo){.opCount=1,.type=state->typeStack[i].type,.isWritable=false,.isAddressable=false};
      stackState->ops[i]=opGetTmpVar(&(state->typeStack[i].type),varId,pos);
    }
  }
}
void checkIfTypes(TypeCheckState* state,IfBlockInfo* ifBlock,FilePosition pos){
  bool needInit=!(ifBlock->endReachable);
  storeStackValues(state,&(ifBlock->outStack),(needInit)? &(ifBlock->inStack) : &(ifBlock->outStack),"if-branch",needInit,false,false,pos);
}
void initWhileTypes(TypeCheckState* state,WhileBlockInfo* whileBlock,FilePosition pos){
  storeStackValues(state,&(whileBlock->inStack),NULL,"while-loop",true,true,false,pos);
}
void checkWhileOutTypes(TypeCheckState* state,WhileBlockInfo* whileBlock,bool isDo,FilePosition pos){
  bool needInit=!(whileBlock->hasDo||whileBlock->hasBreak);
  storeStackValues(state,&(whileBlock->outStack),&(whileBlock->outStack),"while-loop",needInit,false,isDo,pos);
}
void checkWhileTypes(TypeCheckState* state,WhileBlockInfo* whileBlock,FilePosition pos){
  storeStackValues(state,&(whileBlock->inStack),&(whileBlock->inStack),"while-loop",false,false,false,pos);
}
void checkSwitchTypes(TypeCheckState* state,SwitchBlockInfo* switchBlock,FilePosition pos){
  bool needInit=!switchBlock->endReachable;
  storeStackValues(state,&(switchBlock->outStack),&(switchBlock->outStack),"switch-branch",needInit,false,false,pos);
}


bool canAutoCast(DataType const* src,DataType const* target){//? allow cast T ptr mut ptr -> T ptr ptr (allow allow casting mut away if out pointers are const)
  if(typeEquals(src,target))
    return true;
  if(src->typeClass==TYPECLASS_ENUM&&target->typeClass==TYPECLASS_ENUM_LABEL&&src->typeDataAs.composite->id==target->typeDataAs.composite->id)
    return true;//allow auto-cast from enum to enum-label
  if(isPointerType(src)&&!isMutableType(target)&&typeEquals(src->typeDataAs.type,target->typeDataAs.type))
    return true;//assigning pointer to const pointer
  if(src->typeClass==TYPECLASS_ARRAY_VIEW&&src->typeDataAs.array->sizeKnown&&isPointerType(target)&&
    (isMutableType(src)||!isMutableType(target))&&typeEquals(src->typeDataAs.array->base,target->typeDataAs.type))
    return true;//assigning fixed-size array-view to pointer
  if(!isPrimitiveType(src)||!isPrimitiveType(target))
    return false;
  return isInteger(src->typeDataAs.primitive)&&isInteger(target->typeDataAs.primitive)&&
    numberRank(src->typeDataAs.primitive)<=numberRank(target->typeDataAs.primitive);//implicit casts only from small int to large int
}
bool canCast(DataType const* src,DataType const* target){
  if(canAutoCast(src,target))
    return true;
  return numberRank(src->typeDataAs.primitive)>-1&&numberRank(target->typeDataAs.primitive)>-1;//casts only between numbers
}

void requireTypes(char const* opName,TypeCheckState* state,DataType const* types,size_t nTypes,FilePosition pos){//XXX? auto-create tuples
  if(state->typeCount<nTypes){
    fprintf(stderr,"not enough types for %s need %zu have %zu\n",opName,nTypes,state->typeCount);
    handleError(NULL,ERROR_TYPE,pos);
  }
  int32_t nCasts=0;
  size_t offset=state->opStackCount;
  //check types
  for(size_t k=1;k<=nTypes;k++){
    offset-=state->typeStack[state->typeCount-k].opCount;
    if(typeEquals(&(types[nTypes-k]),&(state->typeStack[state->typeCount-k].type)))
      continue;
    if(canAutoCast(&(state->typeStack[state->typeCount-k].type),&(types[nTypes-k]))){
      if(state->typeStack[state->typeCount-k].opCount==1&&state->opStack[offset].opType==OP_CONSTANT){//change constant to correct type
        setTypeStackTypeOffset(state,k,types[nTypes-k]);
        state->opStack[offset].dataType=types[nTypes-k];
        continue;
      }
      nCasts++;
      continue;
    }
    //convert enum labels to enum constants
    if(state->typeStack[state->typeCount-k].type.typeClass==TYPECLASS_ENUM_LABEL&&types[nTypes-k].typeClass==TYPECLASS_ENUM&&
      state->typeStack[state->typeCount-k].type.typeDataAs.composite->id==types[nTypes-k].typeDataAs.composite->id){
      if(state->typeStack[state->typeCount-k].opCount>1||state->opStack[offset].opType!=OP_CONSTANT){
        handleError("unexpected operation with type ENUM_LABEL",ERROR_SYNTAX,pos);//enum-label type should only exist on enum-label constants
      }
      state->typeStack[state->typeCount-k].type.typeClass=TYPECLASS_ENUM;
      if(!isVoidType(&(state->typeStack[state->typeCount-k].type.typeDataAs.composite->types[state->opStack[offset].dataAs.i64]))){
        String label=getLabelName(state->typeStack[state->typeCount-k].type.typeDataAs.composite->labelOffset+state->opStack[offset].dataAs.i64);
        fprintf(stderr,"missing data value for creating enum constant %"PRI_STR" in ",PRI_STR_ARGS(label));
        printTypeName(&(state->typeStack[state->typeCount-k].type),stderr);
        fputs("\nto create enum values with data use the 'new' operator\n",stderr);
        handleError(NULL,ERROR_SYNTAX,pos);
      }
      state->opStack[offset].opType=OP_NEW;
      state->opStack[offset].dataType.typeClass=TYPECLASS_ENUM;
      setTypeStackTypeOffset(state,k,state->opStack[offset].dataType);
      continue;
    }
    typeErrorMessage(opName,types[nTypes-k],state->typeStack[state->typeCount-k].type);
    handleError(NULL,ERROR_TYPE,pos);
  }
  if(nCasts==0)
    return;
  //modify-types
  if(ensureOpStackCap(state,state->opStackCount+nCasts)){
    handleError("exceeded op-stack capacity",ERROR_MEMORY,pos); 
  }
  offset=state->opStackCount;
  size_t shiftCount=0;
  state->opStackCount+=nCasts;//set opStackCount to new values
  for(size_t k=1;k<=nTypes;k++){
    offset-=state->typeStack[state->typeCount-k].opCount;
    shiftCount+=state->typeStack[state->typeCount-k].opCount;
    if(typeEquals(&(types[nTypes-k]),&(state->typeStack[state->typeCount-k].type)))
      continue;
    memmove(state->opStack+offset+nCasts,state->opStack+offset,shiftCount*sizeof(Operation));
    shiftCount=0;
    nCasts--;
    if(canCast(&(state->typeStack[state->typeCount-k].type),&(types[nTypes-k]))){
      DataType* src=bufferedType(&state->typeStack[state->typeCount-k].type);
      if(src==NULL)
        handleError("could not allocate source type",ERROR_MEMORY,pos);
      state->opStack[offset+nCasts]=(Operation){.opType=OP_CAST,.filePos=pos,.dataType=types[nTypes-k],.dataAs={.sourceType=src}};
      setTypeStackTypeOffset(state,k,types[nTypes-k]);
      state->typeStack[state->typeCount-k].opCount++;
      continue;
    }
    handleError("unexpected type-conversion",ERROR_UNIMPLEMENTED,pos); 
  }
}

void pushType(TypeCheckState* state,DataType dataType,FilePosition pos){
  if(ensureTypeStackCap(state,state->typeCount+1)){
    handleError("exceeded type stack capacity",ERROR_MEMORY,pos);
  }
  state->typeStack[state->typeCount++]=(TypeInfo){.type=dataType,.opCount=1,.isWritable=false,.isAddressable=false};
}
void pushValue(TypeCheckState* state,Operation op){
  if(ensureOpStackCap(state,state->opStackCount+1)){
    handleError("exceeded operation stack capacity",ERROR_MEMORY,op.filePos);
  }
  state->opStack[state->opStackCount++]=op;
  pushType(state,op.dataType,op.filePos);
}

void insertStackOperation(TypeCheckState* state,Operation op,size_t totalOps){
  if(ensureOpStackCap(state,state->opStackCount+1)){
    handleError("exceeded operation stack capacity",ERROR_MEMORY,op.filePos);
  }
  memmove(state->opStack+state->opStackCount-totalOps+1,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
  state->opStack[state->opStackCount-totalOps]=op;
  state->opStackCount++;
}

void checkLocal(TypeCheckState* state,Operation op){
  if(state->blockCount!=0)
    return;
  fputs("unexpected operation at global level: ",stderr);
  printOperation(op,stderr);
  handleError(NULL,ERROR_SYNTAX,op.filePos);
}
void checkReachable(TypeCheckState* state,Operation op){
  if(state->reachable)
    return;
  fputs("unreachable statement ",stderr);
  printOperation(op,stderr);
  handleError(NULL,ERROR_SYNTAX,op.filePos);
}


void typeCheckCall(Operation* op,TypeCheckState* state,bool isPtr){
  DataType calledType=op->dataType;
  if(isPtr){
    if(state->typeCount<1||!isCallableType(&(state->typeStack[state->typeCount-1].type))){
      fprintf(stderr,"the argument of %s has to be a callable Type\n",opName(op->opType));
      handleError(NULL,ERROR_TYPE,op->filePos);
    }
    calledType=state->typeStack[state->typeCount-1].type;
    if(isPointerType(&calledType))
      calledType=*(calledType.typeDataAs.type);
    op->dataType=calledType;
  }
  if(!isCallableType(&calledType)){
    fputs("cannot call objects of type ",stderr);
    printTypeName(&calledType,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_TYPE,op->filePos);
  }
  ProcedureType const* procType=calledType.typeDataAs.procedure;
  CompositeType const* outTypes=procType->outType->typeDataAs.composite;
  CompositeType const* inTypes=procType->inType->typeDataAs.composite;
  size_t argCount=inTypes->typeCount;
  size_t totalOps=0;
  if(state->typeCount<argCount){
    fprintf(stderr,"not enough operands for procedure call: need %zu got %zu\n",argCount,state->typeCount);
    handleError(NULL,ERROR_TYPE,op->filePos);
  }
  //extract operations
  extractCompositeOps(state,argCount+(isPtr?1:0),false);
  int32_t tmpId;
  if(outTypes->typeCount!=0){//store non-void return in temp variable
    tmpId=state->tmpCount++;
    pushCompiledOperation(state,opDeclareIntermediate(outTypes->typeCount==1?(outTypes->types+0):procType->outType,tmpId,op->filePos));
  }
  addCompiledOps(state,*op,isPtr?1:0);
  size_t offset=state->typeCount-argCount;
  requireTypes("procedure argument",state,inTypes->types,inTypes->typeCount,op->filePos);
  for(int32_t i=0;i<inTypes->typeCount;i++){
    totalOps+=state->typeStack[offset+i].opCount;
  }
  //update op-stack
  addCompiledStackOps(state,*op/*ignored*/,argCount,false);
  if(outTypes->typeCount==0)
    return;//no need to update stack if called function returns void
  //add values of call
  if(outTypes->typeCount==1){//single return value
    pushValue(state,opGetIntermediate(outTypes->types+0,tmpId,op->filePos));
    return;
  }
  //auto-unwrap multi-return values
  if(ensureTypeStackCap(state,state->typeCount+outTypes->typeCount)||ensureOpStackCap(state,state->opStackCount+3*outTypes->typeCount)){
    handleError("exceeded op-stack capacity",ERROR_MEMORY,op->filePos);
  }
  for(int32_t e=0;e<outTypes->typeCount;e++){
    state->typeStack[state->typeCount++]=(TypeInfo){.type=outTypes->types[e],.opCount=3,.isWritable=false,.isAddressable=false};
    state->opStack[state->opStackCount++]=(Operation){.opType=OP_GET,.dataType=*procType->outType,.filePos=op->filePos,
      .dataAs={.idInfo={.type=ID_TUPLE,.id=1,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}};
    state->opStack[state->opStackCount++]=opGetIntermediate(procType->outType,tmpId,op->filePos);
    state->opStack[state->opStackCount++]=(Operation){.opType=OP_GET,.dataType=outTypes->types[e],.filePos=op->filePos,
      .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.labelId=LABEL_ID_UNKNOWN,.id=e,.isMutable=false}}};
  }
}
void pushProcArgs(TypeCheckState* state,DataType* procType,FilePosition pos){
  if(!isCallableType(procType)||isPointerType(procType)){
    handleError("procedure type has to be callable",ERROR_TYPE,pos); 
  }
  if(procType->typeDataAs.procedure->inType->typeClass==TYPECLASS_LABELED_PROC_IN)
    return;//do not push values with input is labeled 
  CompositeType const* inTypes=procType->typeDataAs.procedure->inType->typeDataAs.composite;
  if(inTypes->typeCount==0)
    return;//no input arguments
  if(inTypes->typeCount==1){
    pushValue(state,(Operation){.opType=OP_GET,.dataType=inTypes->types[0],.filePos=pos,
      .dataAs={.idInfo={.type=ID_ARGUMENT,.id=0,.labelId=inTypes->labelOffset,.isMutable=false}}});
    setTypeStackFlags(state,true,false);
    return;
  }
  for(int32_t i=0;i<inTypes->typeCount;i++){
    LabelId labelId=inTypes->labelOffset==LABEL_ID_UNKNOWN?LABEL_ID_UNKNOWN:inTypes->labelOffset+i;
    pushValue(state,(Operation){.opType=OP_GET,.dataType=inTypes->types[i],.filePos=pos,
      .dataAs={.idInfo={.type=ID_ARGUMENT,.id=i,.labelId=labelId,.isMutable=false}}});
    setTypeStackFlags(state,true,false);
  }
}

void typeCheckSetVariable(TypeCheckState* state,Operation* op){
  if(!op->dataAs.idInfo.isMutable){
    LabelId labelId=op->dataAs.idInfo.labelId;
    if(labelId==-1){
      handleError("variable is not mutable",ERROR_TYPE,op->filePos);
    } 
    Label mLabel=label(labelId,op->filePos);
    fprintf(stderr,"variable %"PRI_STR" is not mutable\n",PRI_STR_ARGS( mLabel.label));
    fprintf(stderr,"  %"PRI_STR" was declared at",PRI_STR_ARGS( mLabel.label));
    printFilePosition(mLabel.declaredAt,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_TYPE,op->filePos);
  }
  requireTypes("variable assignment",state,&op->dataType,1,op->filePos);
  addCompiledStackOps(state,*op,1,true);
}
void typeCheckSetStackValue(TypeCheckState* state,Operation const* op,DataType const* valType){
  addCompiledStackOps(state,*op,1,false);
  requireTypes("value assignment",state,valType,1,op->filePos);
  addCompiledStackOps(state,*op,1,false);
}
bool canWriteTupleElement(DataType const* tupleType,int32_t index,FilePosition pos){
  if(!isTupleType(tupleType)){
    fputs("unexpected type for tuple access: ",stderr);
    printTypeName(tupleType,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_MEMORY,pos);
  }
  CompositeType const* tuple=tupleType->typeDataAs.composite;
  if(tuple->labelOffset!=LABEL_ID_UNKNOWN)
    return label(tuple->labelOffset+index,pos).isMutable;
  return true;
}
void checkTupleElementMutable(Operation const* elementAccess,int32_t depth){
  DataType const* currentTuple;
  for(int32_t i=0;i<depth;i++){
    if((elementAccess->opType!=OP_GET&&elementAccess->opType!=OP_SET)||elementAccess->dataAs.idInfo.type!=ID_TUPLE_ELEMENT){
      printOperation(*elementAccess,stderr);
      handleError("unexpected operation for tuple access",ERROR_MEMORY,elementAccess->filePos);
    }
    currentTuple=&elementAccess->dataType;
    if(!canWriteTupleElement(currentTuple,elementAccess->dataAs.idInfo.id,elementAccess->filePos)){
      fputs("element ",stderr);
      if(currentTuple->typeDataAs.composite->labelOffset!=LABEL_ID_UNKNOWN){
        String label=getLabelName(currentTuple->typeDataAs.composite->labelOffset+elementAccess->dataAs.idInfo.id);
        fprintf(stderr,"%"PRI_STR" ",PRI_STR_ARGS(label));
      }
      fprintf(stderr,"(%"PRIi32")",elementAccess->dataAs.idInfo.id);
      fputs(" in ",stderr);
      printTypeName(currentTuple,stderr);
      fputs(" is not mutable\n",stderr);
      if(currentTuple->typeDataAs.composite->labelOffset!=LABEL_ID_UNKNOWN){
        fputs("  declared at ",stderr);
        printFilePosition(label(currentTuple->typeDataAs.composite->labelOffset+elementAccess->dataAs.idInfo.id,elementAccess->filePos).declaredAt,stderr);
        fputs("\n",stderr);
      }
      handleError(NULL,ERROR_SYNTAX,elementAccess->filePos);
    }
    elementAccess++;
  }
}
void typeCheckGetTupleElement(TypeCheckState* state,DataType const* tupleType,bool tupleWritable,Operation* op){
  CompositeType const* tuple=tupleType->typeDataAs.composite;
  size_t offset=state->typeCount-1;
  DataType const* eltType=&tuple->types[op->dataAs.idInfo.id];
  Operation* blockStart=&(state->opStack[state->opStackCount-state->typeStack[offset].opCount]);
  bool mutable=canWriteTupleElement(tupleType,op->dataAs.idInfo.id,op->filePos);
  if((blockStart->opType==OP_GET||blockStart->opType==OP_SET)&&(
      blockStart->dataAs.idInfo.type==ID_POINTER||blockStart->dataAs.idInfo.type==ID_POINTER_OFFSET||blockStart->dataAs.idInfo.type==ID_ARRAY_ELEMENT||blockStart->dataAs.idInfo.type==ID_TUPLE)){
    if(ensureOpStackCap(state,state->opStackCount+1)){
      handleError("exceeded op-stack capacity",ERROR_MEMORY,op->filePos);
    }
    blockStart->dataAs.idInfo.id++;
    state->opStack[state->opStackCount++]=*op;
    setTypeStackType(state,*eltType);
    setTypeStackFlags(state,true,mutable&tupleWritable);
    state->typeStack[offset].opCount++;
    if(op->opType==OP_SET){
      blockStart->opType=OP_SET;
      checkTupleElementMutable(&state->opStack[state->opStackCount-blockStart->dataAs.idInfo.id],blockStart->dataAs.idInfo.id);
      typeCheckSetStackValue(state,op,eltType);
    }
    return;
  }
  //wrap composite operations
  extractCompositeOps(state,1,true);
  //update operation stack
  size_t totalOps=state->typeStack[offset].opCount;
  if(ensureOpStackCap(state,state->opStackCount+totalOps+2)){
    handleError("exceeded op-stack capacity",ERROR_MEMORY,op->filePos);
  }
  insertStackOperation(state,(Operation){.opType=op->opType/*OP_GET or OP_SET*/,.dataType=*tupleType,
    .dataAs={.idInfo={.type=ID_TUPLE,.id=1,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}},totalOps);
  state->opStack[state->opStackCount++]=*op;
  //update type-stack
  setTypeStackType(state,*eltType);
  setTypeStackFlags(state,true,mutable&tupleWritable);
  state->typeStack[offset].opCount+=2;
  if(op->opType==OP_SET){
    if(!tupleWritable)
      handleError("cannot write to field of constant tuple",ERROR_SYNTAX,op->filePos);
    checkTupleElementMutable(&state->opStack[state->opStackCount-1],1);
    typeCheckSetStackValue(state,op,eltType);
  }
}
void typeCheckArrayElementAccess(TypeCheckState* state,DataType const* arrayType,Operation* op){
  size_t typeOffset=state->typeCount-2;
  size_t offset=state->opStackCount-(state->typeStack[typeOffset].opCount+state->typeStack[typeOffset+1].opCount);
  ArrayType const* arrayData=arrayType->typeDataAs.array;
  //wrap composite operations
  extractCompositeOps(state,2,true);//XXX only keep array writeable
  //check array bounds
  state->hasCheckBounds=1;
  pushCompiledOperation(state,(Operation){.opType=OP_CHECK_ARRAY_BOUNDS,.dataType=TYPE_UNDEFINED,.filePos=op->filePos,.dataAs={0}});
  pushCompiledOperations(state,state->opStack+offset+state->typeStack[typeOffset].opCount,state->typeStack[offset+1].opCount);//index
  if(arrayType->typeDataAs.array->sizeKnown){//fixed-size array
    pushCompiledOperation(state,opConstant(primitiveType(PRIMITIVE_I64),arrayData->sizes[arrayData->dims-1],op->filePos));
  }else{
    pushCompiledOperation(state,(Operation){.opType=OP_GET,.dataType=*arrayType,.filePos=op->filePos,
            .dataAs={.idInfo={.type=ID_ARRAY_SIZE,.id=0,.labelId=-1,.isMutable=false}}});//length
    pushCompiledOperations(state,state->opStack+offset,state->typeStack[offset].opCount);
  }
  // ... array index []
  if(arrayData->dims==1){
    op->dataAs.idInfo.type=ID_ARRAY_ELEMENT;
    op->dataType=*arrayType;
    //wrap composite operations
    extractCompositeOps(state,2,true);
    //update operation stack
    insertStackOperation(state,*op,2);
    //update type-stack
    state->typeCount--;
    bool writable=isMutableType(&state->typeStack[offset].type);
    setTypeStackType(state,*arrayData->base);
    setTypeStackFlags(state,false,writable);
    state->typeStack[offset].opCount+=state->typeStack[offset+1].opCount+1;
    if(op->opType==OP_SET){
      if(!writable)
        handleError("cannot write to immutable pointer",ERROR_SYNTAX,op->filePos);
      typeCheckSetStackValue(state,op,arrayData->base);
    }
    return;
  }
  //TODO OP_GET_SUBARRAY/OP_SET_SUBARRAY
  handleError("multiarray access",ERROR_UNIMPLEMENTED,op->filePos);
}
void typeCheckGet(TypeCheckState* state,Operation* op){ 
  checkReachable(state,*op);
  checkLocal(state,*op);
  size_t offset;
  bool writable;
  switch(op->dataAs.idInfo.type){
    case ID_LOCAL_VAR:
    case ID_GLOBAL_VAR:
    case ID_ARGUMENT:
    case ID_PROCEDURE:
      if(op->dataType.typeClass!=TYPECLASS_UNDEFINED){
        if(op->opType==OP_SET){
          typeCheckSetVariable(state,op);
          return;
        }
        pushValue(state,*op);
        setTypeStackFlags(state,true,op->dataAs.idInfo.isMutable);
        return;
      }
      if(op->dataType.typeDataAs.typeId<=0||op->dataType.typeDataAs.typeId>state->nPredeclaredTypes){
        handleError("predeclared id out of expected range",ERROR_TYPE,op->filePos);
      }
      op->dataType=state->predeclaredTypes[op->dataType.typeDataAs.typeId-1];//get predeceased type
      if(op->dataType.typeClass!=TYPECLASS_UNDEFINED){
        if(op->opType==OP_SET){
          typeCheckSetVariable(state,op);
          return;
        }
        pushValue(state,*op);
        setTypeStackFlags(state,true,op->dataAs.idInfo.isMutable);
        return;
      }
      handleError("missing type declaration",ERROR_TYPE,op->filePos);
    case ID_TUPLE_ELEMENT:
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op->opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      offset=state->typeCount-1;
      op->dataType=state->typeStack[offset].type;
      writable=state->typeStack[offset].isWritable;
      if(op->dataType.typeClass!=TYPECLASS_TUPLE){
        printTypeName(&op->dataType,stderr);
        fputs(" is not a tuple\n",stderr);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      CompositeType const* tuple=op->dataType.typeDataAs.composite;
      if(tuple->typeCount<op->dataAs.idInfo.id){
        fprintf(stderr,"index %"PRIi32" exceeds element count of tuple %"PRIi32"\n",op->dataAs.idInfo.id,tuple->typeCount);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      typeCheckGetTupleElement(state,&op->dataType,writable,op);
      return;
    case ID_POINTER:
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s %s: need 1 got %zu\n",opName(op->opType),idNames[op->dataAs.idInfo.type],state->typeCount);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      offset=state->typeCount-1;
      if(!isPointerType(&(state->typeStack[offset].type))){
        fprintf(stderr,"invalid operand for %s %s : ",opName(op->opType),idNames[op->dataAs.idInfo.type]);
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs(" is not a pointer\n",stderr);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      op->dataType=*state->typeStack[offset].type.typeDataAs.type;
      //update operation stack
      //wrap composite operations
      extractCompositeOps(state,1,true);
      insertStackOperation(state,*op,1);
      //update type-stack
      writable=isMutableType(&state->typeStack[offset].type);
      setTypeStackType(state,op->dataType);
      setTypeStackFlags(state,false,writable);//dereferenced pointers are writable but not addressable
      state->typeStack[offset].opCount++;
      if(op->opType==OP_SET){
        if(!writable)
          handleError("cannot write to immutable pointer",ERROR_SYNTAX,op->filePos);
        typeCheckSetStackValue(state,op,&op->dataType);
      }
      return;
    case ID_ARRAY_ELEMENT:
    case ID_POINTER_OFFSET:
      if(state->typeCount<2){
        fprintf(stderr,"not enough operands for operation %s %s: need 2 got %zu\n",opName(op->opType),idNames[op->dataAs.idInfo.type],state->typeCount);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      offset=state->typeCount-2;
      if(!isIntType(&(state->typeStack[offset+1].type))){
        fprintf(stderr,"invalid second operand for %s %s : ",opName(op->opType),idNames[op->dataAs.idInfo.type]);
        printTypeName(&(state->typeStack[offset+1].type),stderr);
        fputs(" expected an integer\n",stderr);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      if(isArrayType(&(state->typeStack[offset].type))){
        typeCheckArrayElementAccess(state,&state->typeStack[offset].type,op);
        return;
      }
      op->dataAs.idInfo.type=ID_POINTER_OFFSET;
      if(!isPointerType(&(state->typeStack[offset].type))){
        fprintf(stderr,"invalid first operand for %s %s : ",opName(op->opType),idNames[op->dataAs.idInfo.type]);
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs(" is not a pointer or an array\n",stderr);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      op->dataType=*state->typeStack[offset].type.typeDataAs.type;
      //wrap composite operations
      extractCompositeOps(state,2,true);
      //update operation stack
      insertStackOperation(state,*op,2);
      //update type-stack
      state->typeCount--;
      writable=isMutableType(&state->typeStack[offset].type);
      setTypeStackType(state,op->dataType);
      setTypeStackFlags(state,false,writable);//dereferenced pointers are writable but not addressable
      state->typeStack[offset].opCount+=state->typeStack[offset+1].opCount+1;
      if(op->opType==OP_SET){
        if(!writable)
          handleError("cannot write to immutable pointer",ERROR_SYNTAX,op->filePos);
        typeCheckSetStackValue(state,op,&op->dataType);
      }
      return;
    case ID_INTERMEDIATE_RESULT:
    case ID_TMP_VAR:
    case ID_TUPLE:
      break;
    case ID_ENUM_LABEL:
    case ID_ENUM_ELEMENT:
    case ID_ARRAY_SIZE:
      fprintf(stderr,"direct access to %s should not exist at this stage of compilation\n",idNames[op->dataAs.idInfo.type]);
      handleError(NULL,ERROR_SYNTAX,op->filePos);
    case ID_TYPE:
      fputs("identifiers of type-names should not exist at this stage of compilation\n",stderr);
      handleError(NULL,ERROR_SYNTAX,op->filePos);
  }
}

void typeCheckReturn(TypeCheckState* state,Operation* op){
  if(op->dataType.typeClass!=TYPECLASS_PROC_OUT){
    fprintf(stderr,"unexpected procedure return type-class: %s\n",typeClassName(op->dataType.typeClass));
    handleError(NULL,ERROR_SYNTAX,op->filePos);
  }
  CompositeType const* outTypes=op->dataType.typeDataAs.composite;
  if(outTypes->typeCount==0){
      if(checkNonemptyStack(state,"unfinished operation at end of procedure")){
        handleError(NULL,ERROR_SYNTAX,op->filePos);
      }
    pushCompiledOperation(state,*op);
    return;
  }
  if(state->opStackCount==0){
    handleError("missing return value",ERROR_SYNTAX,op->filePos);
  }
  if(outTypes->typeCount<0||state->typeCount!=(size_t)outTypes->typeCount){
    fprintf(stderr,"wrong number of return values: expected %"PRIi32" got %zu\n",outTypes->typeCount,state->typeCount);
    handleError(NULL,ERROR_TYPE,op->filePos);
  }
  requireTypes("return statement",state,outTypes->types,state->typeCount,op->filePos);
  extractCompositeOps(state,state->typeCount,false);
  addCompiledOps(state,*op,state->typeCount);
}

void resolveIdentifiers(TypeCheckState* state,Operation* op){
  if(op->opType!=OP_IDENTIFIER&&op->opType!=OP_SET_IDENTIFIER&&op->opType!=OP_IDENTIFIER_ADDRESS)
    return; 
  BlockInfo* blockInfo=peekBlock(state);
  String mLabel=label(op->dataAs.localLabel.label,op->filePos).label;
  if(!state->reachable&&op->opType==OP_IDENTIFIER&&blockInfo!=NULL&&
    (blockInfo->type==BLOCK_SWITCH||blockInfo->type==BLOCK_CASE)&&blockInfo->blockDataAs.switchBlock.switchType.typeClass==TYPECLASS_ENUM_LABEL){
    CompositeType const* enumType=blockInfo->blockDataAs.switchBlock.switchType.typeDataAs.composite;
    for(int32_t i=0;i<enumType->typeCount;i++){
      if(stringCompare(mLabel,getLabelName(enumType->labelOffset+i))==0){//identifier is label of current switch
        *op=opConstant(blockInfo->blockDataAs.switchBlock.switchType,i,op->filePos);
        return;
      }
    }
  }
  ScopeNode* asIdentifier;
  int r=getIdentifier(op->dataAs.localLabel.namespaceInfo,mLabel,&asIdentifier);
  if(r!=0){
    fprintf(stderr," unknown identifier '%"PRI_STR"'\n",PRI_STR_ARGS(mLabel));
    handleError(NULL,r,op->filePos);
  }
  if(op->opType==OP_SET_IDENTIFIER&&asIdentifier->idType==ID_PROCEDURE)
    handleError("cannot set value of procedure",ERROR_SYNTAX,op->filePos);
  *op=(Operation){.opType=op->opType==OP_SET_IDENTIFIER?OP_SET:((asIdentifier->idType==ID_PROCEDURE)&&(op->opType!=OP_IDENTIFIER_ADDRESS))?OP_CALL:OP_GET,
    .dataType=asIdentifier->type,.filePos=op->filePos,
      .dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id,.labelId=asIdentifier->labelId,.isMutable=label(asIdentifier->labelId,op->filePos).isMutable}}};
}
void typeCheckOperation(Operation op,TypeCheckState* state){
  size_t totalOps=0;
  int32_t offset,tmpId;
  BlockInfo blockInfo;
  BlockInfo* blockInfoPtr;
  IfBlockInfo* ifBlock;
  SwitchBlockInfo* switchBlock;
  
  resolveIdentifiers(state,&op);
  switch(op.opType){
    case OP_CONSTANT:
      if(state->reachable){
        pushValue(state,op);
        return;
      }
      blockInfoPtr=peekBlock(state);
      if(blockInfoPtr!=NULL&&(blockInfoPtr->type==BLOCK_SWITCH||blockInfoPtr->type==BLOCK_CASE)){//switch label
        switchBlock=&blockInfoPtr->blockDataAs.switchBlock;
        if(!canAutoCast(&op.dataType,&switchBlock->switchType)){
          fputs("wrong type from switch label, expected ",stderr);
          printTypeName(&switchBlock->switchType,stderr);
          fputs(" got ",stderr);
          printTypeName(&op.dataType,stderr);
          fputs("\n",stderr);
          handleError(NULL,ERROR_TYPE,op.filePos);
        }
        if(switchBlock->switchData->labelCount>=switchBlock->switchData->labelCap)
          handleError("exceeded maximum number of allowed switch labels",ERROR_MEMORY,op.filePos);
        for(size_t i=0;i<switchBlock->switchData->labelCount;i++){//check for duplicate labels
          if(switchBlock->switchData->labelData[i].value==op.dataAs.i64){
            if(switchBlock->switchType.typeClass==TYPECLASS_ENUM_LABEL){
              String label=getLabelName(switchBlock->switchType.typeDataAs.composite->labelOffset+op.dataAs.i64);
              fprintf(stderr,"duplicate label %"PRI_STR" in switch-case\n",PRI_STR_ARGS(label));
            }else{
              fprintf(stderr,"duplicate label %"PRIi64" in switch-case\n",op.dataAs.i64);
            }
            fputs("  previous label at ",stderr);
            printFilePosition(switchBlock->switchData->labelData[i].pos,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
        }
        switchBlock->switchData->labelData[switchBlock->switchData->labelCount].value=op.dataAs.i64;
        switchBlock->switchData->labelData[switchBlock->switchData->labelCount++].pos=op.filePos;
        switchBlock->switchData->cases[switchBlock->switchData->caseCount].count++;
        return;
      }
      //other unreachable constants
      checkReachable(state,op);
      return;
    case OP_UNARY_OPERATOR:
      checkReachable(state,op);
      checkLocal(state,op);
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for unary operator %s: need 1 got %zu\n",binOpName(op.dataAs.binOp),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-1;
      //result of operation is neither addressable nor writable
      op.dataType=state->typeStack[offset].type;//unary operator returns value of same type
      switch(op.dataAs.unOp){
        case INCREMENT:
        case DECREMENT:
          if(!state->typeStack[offset].isWritable){//value has to be 
            fprintf(stderr,"operand of unary operator %s has to be writable \n",unOpName(op.dataAs.unOp));
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          if(isPointerType(&(state->typeStack[offset].type))){
            break;//pointer is an allowed type for increment
          }
          //fall through
        case NEGATE:
          if(!isNumberType(&(state->typeStack[offset].type))){
            fprintf(stderr,"wrong operand type for unary operator %s expected integer ",unOpName(op.dataAs.unOp));
            fputs(" got ",stderr);
            printTypeName(&(state->typeStack[offset].type),stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          break;
        case FLIP:
          if(!isIntType(&(state->typeStack[offset].type))){
            fprintf(stderr,"wrong operand type for unary operator %s expected integer ",unOpName(op.dataAs.unOp));
            fputs(" got ",stderr);
            printTypeName(&(state->typeStack[offset].type),stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          break;
        case NOT:
          if(!isBoolType(&(state->typeStack[offset].type))){
            typeErrorMessage("unary operator NOT",primitiveType(PRIMITIVE_BOOL),state->typeStack[offset].type);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          break;
      }
      //update op-stack
      //store result in temp variable
      extractCompositeOps(state,1,false);
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      addCompiledOps(state,op,1);
      //update stack
      pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
      return;
    case OP_BINARY_OPERATOR:
      checkReachable(state,op);
      checkLocal(state,op);
      if(state->typeCount<2){
        fprintf(stderr,"not enough operands for binary operator %s: need 2 got %zu\n",binOpName(op.dataAs.binOp),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-2;
      bool typesMatch=false;
      DataType inTypes[2]={state->typeStack[offset].type,state->typeStack[offset+1].type};
      switch(op.dataAs.binOp){
        case ADD:
        case SUBTRACT:
          op.dataType=typeCheckPointerArithmetic(inTypes,op.dataAs.binOp==SUBTRACT);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          op.dataType=typeCheckArithmetic(inTypes);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          break;
        case MULTIPLY:
        case DIVIDE:
        case MOD:
          op.dataType=typeCheckArithmetic(inTypes);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          break;
        case AND:
        case OR:
        case XOR:
          //integer bool ops
          //TODO? bit shifts 
          op.dataType=typeCheckIntLogic(inTypes);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          //bool ops
          if(isBoolType(&(state->typeStack[offset].type))&&isBoolType(&(state->typeStack[offset+1].type))){
            op.dataType=primitiveType(PRIMITIVE_BOOL);
            typesMatch=true;
            break;
          }
          break;
        case EQ:
        case NE:
          //pointer equality 
          if(isPointerType(&(inTypes[0]))&&isPointerType(&(inTypes[1]))&&
              typeEquals(inTypes[0].typeDataAs.type,inTypes[1].typeDataAs.type)){
            op.dataType=primitiveType(PRIMITIVE_BOOL);
            typesMatch=true;
            break;
          }
          //enum-entry equality 
          if((inTypes[0].typeClass==TYPECLASS_ENUM||inTypes[0].typeClass==TYPECLASS_ENUM_LABEL)&&inTypes[1].typeClass==TYPECLASS_ENUM_LABEL&&
              inTypes[0].typeDataAs.composite->id==inTypes[1].typeDataAs.composite->id){
            inTypes[0].typeClass=TYPECLASS_ENUM_LABEL;
            op.dataType=primitiveType(PRIMITIVE_BOOL);
            typesMatch=true;
            break;
          }
          //number equality
          op.dataType=typeCheckCompare(inTypes);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          break;
        case GT:
        case GE:
        case LE:
        case LT:
          //number comparison
          op.dataType=typeCheckCompare(inTypes);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          break;
      }
      if(!typesMatch){
        fprintf(stderr,"No version of binary operator %s supports the types ",binOpName(op.dataAs.binOp));
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs(" ",stderr);
        printTypeName(&(state->typeStack[offset+1].type),stderr);
        fputs("\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      //update operation stack
      //ensure operands have matching types
      requireTypes("binary operator",state,inTypes,2,op.filePos);
      //store result in temp variable
      extractCompositeOps(state,2,false);
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      addCompiledOps(state,op,2);
      //update stack
      pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
      return;
    case OP_PRINT:
      checkReachable(state,op);
      checkLocal(state,op);
      if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-1;
      if((isPrimitiveType(&(state->typeStack[offset].type))&&!isVoidType(&(state->typeStack[offset].type)))||
          (isPointerType(&(state->typeStack[offset].type))&&!isCallableType(&(state->typeStack[offset].type)))||
          state->typeStack[offset].type.typeClass==TYPECLASS_ARRAY_VIEW){
        op.dataType=state->typeStack[offset].type;
        //update operations
        extractCompositeOps(state,1,false);
        addCompiledOps(state,op,1);
        return;
      }
      fputs("cannot print values of type ",stderr);
      printTypeName(&(state->typeStack[offset].type),stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_TYPE,op.filePos);
      break;
    case OP_CHECK_ARRAY_BOUNDS:
    case OP_CHECK_ENUM_INDEX:
      break;
    case OP_GET:
    case OP_SET:
      typeCheckGet(state,&op);
      return;
    case OP_GET_LABEL:
    case OP_SET_LABEL:
      checkReachable(state,op);
      checkLocal(state,op);
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-1;
      DataType structType=peekTypeStack(state)->type;
      bool writable=peekTypeStack(state)->isWritable;
      totalOps=peekTypeStack(state)->opCount;
      if(isArrayType(&structType)){
        if(op.opType==OP_SET_LABEL){
          printTypeName(&structType,stderr);
          fprintf(stderr," does not have a writable field '%"PRI_STR"'\n",PRI_STR_ARGS(op.dataAs.string));
          handleError(NULL,ERROR_SYNTAX,op.filePos);
        }
        if(wordEquals(&op.dataAs.string,"length")){
          op=(Operation){.opType=OP_GET,.dataType=structType,.filePos=op.filePos,
            .dataAs={.idInfo={.type=ID_ARRAY_SIZE,.id=0,.labelId=-1,.isMutable=false}}};
          if(structType.typeDataAs.array->sizeKnown){
            state->opStackCount-=peekTypeStack(state)->opCount;
            insertStackOperation(state,op,0);
            peekTypeStack(state)->opCount=1;//ignore array (length only depends on type)
          }else{
            insertStackOperation(state,op,totalOps);
            peekTypeStack(state)->opCount++;
          }
          setTypeStackType(state,primitiveType(PRIMITIVE_I64));
          setTypeStackFlags(state,true,false);
          return;
        }
        if(wordEquals(&op.dataAs.string,"size")){
          arrayTypes[structType.typeDataAs.array->id].sizeUsed=true;
          op=(Operation){.opType=OP_GET,.dataType=structType,.filePos=op.filePos,
            .dataAs={.idInfo={.type=ID_ARRAY_SIZE,.id=1,.labelId=-1,.isMutable=false}}};
          if(structType.typeDataAs.array->sizeKnown){
            state->opStackCount-=peekTypeStack(state)->opCount;
            insertStackOperation(state,op,0);
            peekTypeStack(state)->opCount=1;//ignore array (size only depends on type)
          }else{
            insertStackOperation(state,op,totalOps);
            peekTypeStack(state)->opCount++;
          }
          DataType tmp=primitiveType(PRIMITIVE_I64);
          int64_t dims=structType.typeDataAs.array->dims;
          setTypeStackType(state,arrayType(true,&tmp,1,&dims,false));
          setTypeStackFlags(state,true,false);
          return;
        }
        printTypeName(&structType,stderr);
        fprintf(stderr," does not have a field '%"PRI_STR"'\n",PRI_STR_ARGS(op.dataAs.string));
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      if(structType.typeClass!=TYPECLASS_STRUCT&&structType.typeClass!=TYPECLASS_ENUM){
        printTypeName(&structType,stderr);
        fputs(" is not a struct or enum\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      CompositeType const* mStruct=structType.typeDataAs.composite;
      int32_t labelIndex=findLabel(mStruct->labelOffset,mStruct->typeCount,&op.dataAs.string);
      if(labelIndex==-1){
        printTypeName(&structType,stderr);
        fprintf(stderr," does not have a field '%"PRI_STR"'\n",PRI_STR_ARGS(op.dataAs.string));
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      if(structType.typeClass==TYPECLASS_STRUCT){
        op=(Operation){.opType=(op.opType==OP_SET_LABEL)?OP_SET:OP_GET,.dataType=structType,.filePos=op.filePos,
          .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=labelIndex,.labelId=mStruct->labelOffset+labelIndex,.isMutable=false}}};
        typeCheckGetTupleElement(state,&structType,writable,&op);
        return;
      }
      if(isVoidType(&(mStruct->types[labelIndex]))){
        fprintf(stderr,"'%"PRI_STR"' in ",PRI_STR_ARGS(op.dataAs.string));
        printTypeName(&structType,stderr);
        fputs(" does not hold a value\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      if(writable&&op.opType==OP_SET_LABEL){//XXX? overwrite only for mutable enum
        DataType lableType=structType;
        lableType.typeClass=TYPECLASS_ENUM_LABEL;
        pushCompiledOperation(state,(Operation){.opType=OP_SET,.filePos=op.filePos,.dataType=lableType,
          .dataAs={.idInfo={.type=ID_ENUM_LABEL,.id=0,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}});
      }else{
        pushCompiledOperation(state,(Operation){.opType=OP_CHECK_ENUM_INDEX,.dataType=(mStruct->types[labelIndex]),.filePos=op.filePos,.dataAs={.i64=labelIndex}});
      }
      pushCompiledOperations(state,state->opStack+state->opStackCount-totalOps,totalOps);//compile enum ops, but keep on stack
      if(writable&&op.opType==OP_SET_LABEL){
        DataType lableType=structType;
        lableType.typeClass=TYPECLASS_ENUM_LABEL;
        pushCompiledOperation(state,opConstant(lableType,labelIndex,op.filePos));
      }
      state->hasCheckEnum=1;
      Label mLabel=label(mStruct->labelOffset+labelIndex,op.filePos);
      op=(Operation){.opType=(op.opType==OP_SET_LABEL)?OP_SET:OP_GET,.dataType=mStruct->types[labelIndex],.filePos=op.filePos,
        .dataAs={.idInfo={.type=ID_ENUM_ELEMENT,.id=labelIndex,.labelId=mStruct->labelOffset+labelIndex,.isMutable=mLabel.isMutable}}};
      insertStackOperation(state,op,totalOps);
      peekTypeStack(state)->opCount+=totalOps;
      setTypeStackType(state,mStruct->types[labelIndex]);
      setTypeStackFlags(state,true,mLabel.isMutable&writable);
      if(op.opType==OP_SET){
        if(!writable)
          handleError("cannot write to field of constant enum",ERROR_SYNTAX,op.filePos);
        if(!mLabel.isMutable){
          fprintf(stderr,"element %"PRI_STR" (%"PRIi32") in ",PRI_STR_ARGS(mLabel.label),labelIndex);
          printTypeName(&structType,stderr);
          fputs(" is not mutable\n",stderr);
          fputs("  declared at ",stderr);
          printFilePosition(mLabel.declaredAt,stderr);
          fputs("\n",stderr);
          handleError(NULL,ERROR_SYNTAX,op.filePos);
        }
        typeCheckSetStackValue(state,&op,&op.dataType);
      }
      return;
    case OP_PRE_DECLARE:
      checkReachable(state,op);
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
          if(op.dataType.typeClass==TYPECLASS_UNDEFINED||isVoidType(&(op.dataType))){
            fputs("invalid type for predeclared variable: ",stderr);
            printTypeName(&(op.dataType),stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          addCompiledOps(state,op,0);
          return;
        case ID_PROCEDURE:
          if(isCallableType(&(op.dataType))||isPointerType(&(op.dataType))){
            fputs("invalid type for predeclared procedure: ",stderr);
            printTypeName(&(op.dataType),stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          addCompiledOps(state,op,0);
          return;
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_ARRAY_ELEMENT:
        case ID_INTERMEDIATE_RESULT:
        case ID_TMP_VAR:
        case ID_ARGUMENT:
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
        case ID_ARRAY_SIZE:
            fputs("cannot (directly) declare ",stderr);
            printIdInfo(op.dataAs.idInfo,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
        case ID_TYPE:
          fputs("identifiers of type-names should not exist at this stage of compilation\n",stderr);
          handleError(NULL,ERROR_SYNTAX,op.filePos);
      }
      break;
    case OP_DECLARE:
      checkReachable(state,op);
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
          if(isVoidType(&(op.dataType))){
            fputs("cannot declare variables of type: ",stderr);
            printTypeName(&(op.dataType),stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          offset=state->typeCount-1;
          //find types for auto-types
          if(op.dataType.typeClass==TYPECLASS_UNDEFINED){
            if(op.dataType.typeDataAs.typeId<=0||op.dataType.typeDataAs.typeId>state->nPredeclaredTypes)
              handleError("predeclared id outside expected range",ERROR_TYPE,op.filePos);
            int64_t typeId=op.dataType.typeDataAs.typeId-1;
            op.dataType=state->typeStack[offset].type;
            if(op.dataType.typeClass==TYPECLASS_ENUM_LABEL)
              op.dataType.typeClass=TYPECLASS_ENUM;
            state->predeclaredTypes[typeId]=op.dataType;
          }
          requireTypes("variable declaration",state,&op.dataType,1,op.filePos);
          if(state->blockCount==0){//don't extract operations at global level
            addCompiledStackOps(state,op,1,true);
            return;
          }
          extractCompositeOps(state,1,false);
          addCompiledOps(state,op,1);
          return;
        case ID_PROCEDURE:
          if(checkNonemptyStack(state,"unfinished global operation")){
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          if(state->blockCount==0){//predeclare procedure in global section
            op.opType=OP_PRE_DECLARE;
            pushCompiledOperation(state,op);
            op.opType=OP_DECLARE;
          }
          //block id will be ignored
          blockInfo=(BlockInfo){.type=BLOCK_PROCEDURE,.blockStart=state->opCount,.blockId=-1,.blockDataAs={.procBlock={.returnType=*op.dataType.typeDataAs.procedure->outType}}};
          if(pushBlock(state,blockInfo))
            handleError(NULL,ERROR_MEMORY,op.filePos);
          pushCompiledOperation(state,op);
          pushProcArgs(state,&op.dataType,op.filePos);
          return;
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_ARRAY_ELEMENT:
        case ID_INTERMEDIATE_RESULT:
        case ID_TMP_VAR:
        case ID_ARGUMENT:
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
        case ID_ARRAY_SIZE:
            fputs("cannot (directly) declare ",stderr);
            printIdInfo(op.dataAs.idInfo,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
        case ID_TYPE:
          fputs("identifiers of type-names should not exist at this stage of compilation\n",stderr);
          handleError(NULL,ERROR_SYNTAX,op.filePos);
      }
      break;
    case OP_NEW:
      checkReachable(state,op);
      if(op.dataType.typeClass==TYPECLASS_TUPLE||op.dataType.typeClass==TYPECLASS_STRUCT){
        offset=state->typeCount-op.dataType.typeDataAs.composite->typeCount;
        requireTypes("tuple creation",state,op.dataType.typeDataAs.composite->types,op.dataType.typeDataAs.composite->typeCount,op.filePos);
        totalOps=0;
        for(int32_t e=0;e<op.dataType.typeDataAs.composite->typeCount;e++){
          totalOps+=state->typeStack[offset+e].opCount;
        }
        if(state->blockCount==0){//create tuple in-place when in global level
          insertStackOperation(state,op,totalOps);
          state->typeCount-=op.dataType.typeDataAs.composite->typeCount;
          state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1,.isWritable=false,.isAddressable=false};
          return;
        }
        //store result in temp variable
        extractCompositeOps(state,op.dataType.typeDataAs.composite->typeCount,false);
        tmpId=state->tmpCount++;
        pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
        addCompiledOps(state,op,op.dataType.typeDataAs.composite->typeCount);
        //update stack
        pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
        return;
      }
      if(op.dataType.typeClass==TYPECLASS_ENUM){
        DataType const* entryData=op.dataType.typeDataAs.composite->types+op.dataAs.i64;
        if(isVoidType(entryData)){
          if(state->blockCount==0){//create enum in-place when in global level
            insertStackOperation(state,op,0);
            state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1,.isWritable=false,.isAddressable=false};
            return;
          }
          tmpId=state->tmpCount++;
          pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
          addCompiledOps(state,op,0);
          //update stack
          pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
          return;
        }
        requireTypes("enum creation",state,entryData,1,op.filePos);
        if(state->blockCount==0){//create enum in-place when in global level
          totalOps=state->typeStack[state->typeCount-1].opCount;
          insertStackOperation(state,op,totalOps);
          state->typeStack[state->typeCount-1]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1,.isWritable=false,.isAddressable=false};
          return;
        }
        extractCompositeOps(state,1,false);
        tmpId=state->tmpCount++;
        pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
        addCompiledOps(state,op,1);
        //update stack
        pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
        return;
      }
      break;
    case OP_CAST:
      checkReachable(state,op);
      checkLocal(state,op);
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s : need 1 got %zu\n",opName(op.opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-1;
      if(!canCast(&(state->typeStack[offset].type),&(op.dataType))){
        fputs("cannot cast ",stderr);
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs(" to ",stderr); 
        printTypeName(&(op.dataType),stderr);
        fputs("\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      op.dataAs.sourceType=bufferedType(&state->typeStack[offset].type);
      if(op.dataAs.sourceType==NULL)
        handleError("could not allocate source type",ERROR_MEMORY,op.filePos);
      //store previous result in temp value
      extractCompositeOps(state,1,false);
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      addCompiledOps(state,op,1);
      //update stack
      pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
      return;
    case OP_ADDR_OF:
      checkReachable(state,op);
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s : need 1 got %zu\n",opName(op.opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-1;
      if(!state->typeStack[offset].isAddressable){
          fprintf(stderr,"the operand of %s has to be an addressable type \n",opName(op.opType));
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      if(isCallableType(&state->typeStack[offset].type)&&!isPointerType(&state->typeStack[offset].type)){//don't use array for function pointers
        op.dataType=pointerType(&(state->typeStack[offset].type),state->typeStack[offset].isWritable);
      }else{
        op.dataType=arrayType(true,&(state->typeStack[offset].type),1,(int64_t[]){1},state->typeStack[offset].isWritable);
      }
      //store result in temp variable
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      addCompiledStackOps(state,op,1,true);
      //update stack
      pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
      return;
    case OP_CODE_BLOCK:
      checkLocal(state,op);
      switch(op.dataAs.block.type){
        case BLOCK_IF:
          checkReachable(state,op);
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr==NULL||blockInfoPtr->type==BLOCK_UNKNOWN){//block stack underflow
            fputs("unexpected IF statement, IF statements cannot be declared at global level\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          blockInfo=(BlockInfo){.type=BLOCK_IF,.blockStart=state->opCount,.blockDataAs={0}};
          ifBlock=&(blockInfo.blockDataAs.ifBlock);
          ifBlock->inStack.types=NULL;
          ifBlock->inStack.ops=NULL;
          ifBlock->outStack.types=NULL;
          ifBlock->outStack.ops=NULL;
          blockInfo.blockId=state->ifCount++;
          //store in-types
          if(state->typeCount>1){
            ifBlock->inStack.typeCount=state->typeCount-1;
            ifBlock->inStack.opCount=state->opStackCount-state->typeStack[state->typeCount-1].opCount;
            ifBlock->inStack.types=malloc((state->typeCount-1)*sizeof(TypeInfo));
            ifBlock->inStack.ops=malloc((ifBlock->inStack.opCount)*sizeof(Operation));
            if(ifBlock->inStack.types==NULL||ifBlock->inStack.ops==NULL)
              handleError(NULL,ERROR_MEMORY,op.filePos);
            memcpy(ifBlock->inStack.types,state->typeStack,(state->typeCount-1)*sizeof(TypeInfo));
            memcpy(ifBlock->inStack.ops,state->opStack,(ifBlock->inStack.opCount)*sizeof(Operation));
          }
          if(pushBlock(state,blockInfo))
            handleError(NULL,ERROR_MEMORY,op.filePos);
          
          op.dataType=primitiveType(PRIMITIVE_BOOL);
          requireTypes("if-condition",state,&op.dataType,1,op.filePos);
          extractCompositeOps(state,1,false);
          offset=state->typeCount-1;
          op.dataAs.block.id=blockInfo.blockId;
          addCompiledOps(state,op,1); 
          return;
        case BLOCK_ELSE:
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr==NULL||blockInfoPtr->type!=BLOCK_IF){//wrong position for ELSE
            fputs("ELSE can only appear in IF blocks\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          ifBlock=&(blockInfoPtr->blockDataAs.ifBlock);
          if(state->reachable){
            checkIfTypes(state,ifBlock,op.filePos);
            ifBlock->endReachable=true;
          }
          state->reachable=true;
          //reset stack to in-types 
          if(resetStack(state,&(ifBlock->inStack)))
            handleError(NULL,ERROR_TYPE,op.filePos);
          //update block
          blockInfoPtr->type=BLOCK_ELSE;
          ifBlock->elsePos=state->opCount;
          op.dataAs.block.id=blockInfoPtr->blockId;
          op.dataAs.block.subId=ifBlock->elifCount;
          pushCompiledOperation(state,op);
          return;
        case BLOCK_IF2:
          checkReachable(state,op);
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr==NULL||blockInfoPtr->type!=BLOCK_ELSE){//wrong position for _IF
            fputs("_IF can only appear in ELSE blocks\n",stderr);
            printf("%u\n",blockInfo.type);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          //update inTypes
          ifBlock=&(blockInfoPtr->blockDataAs.ifBlock);
          if(state->typeCount>1){
            if(ifBlock->inStack.typeCount<state->typeCount-1){
              //if allocation fails program will be terminated -> can directly assign result of realloc
              ifBlock->inStack.types=realloc(ifBlock->inStack.types,(state->typeCount-1)*sizeof(TypeInfo));
            }
            ifBlock->inStack.typeCount=state->typeCount-1;
            if(ifBlock->inStack.opCount<state->opStackCount-state->typeStack[state->typeCount-1].opCount){
              //if allocation fails program will be terminated -> can directly assign result of realloc
              ifBlock->inStack.types=realloc(ifBlock->inStack.types,(state->opStackCount-state->typeStack[state->typeCount-1].opCount)*sizeof(TypeInfo));
            }
            ifBlock->inStack.opCount=state->opStackCount-state->typeStack[state->typeCount-1].opCount;
            if(ifBlock->inStack.types==NULL||ifBlock->inStack.ops==NULL)
              handleError(NULL,ERROR_MEMORY,op.filePos);
            memcpy(ifBlock->inStack.types,state->typeStack,(state->typeCount-1)*sizeof(TypeInfo));
            memcpy(ifBlock->inStack.ops,state->opStack,(ifBlock->inStack.opCount)*sizeof(Operation));
          }else{
            ifBlock->inStack.typeCount=0;
            ifBlock->inStack.opCount=0;
            //the memory sections will be freed when an end-block is encountered
          }
          //update block
          blockInfoPtr->type=BLOCK_IF;
          ifBlock->elsePos=state->opCount;
          op.dataType=primitiveType(PRIMITIVE_BOOL);
          requireTypes("if-condition",state,&op.dataType,1,op.filePos);
          extractCompositeOps(state,1,false);
          offset=state->typeCount-1;
          op.dataAs.block.id=blockInfoPtr->blockId;
          op.dataAs.block.subId=ifBlock->elifCount++;
          addCompiledOps(state,op,1); 
          return;
        case BLOCK_WHILE:
          checkReachable(state,op);
          blockInfo=(BlockInfo){.type=BLOCK_WHILE,.blockStart=state->opCount,.blockDataAs={0}};
          blockInfo.blockDataAs.whileBlock.inStack.types=NULL;
          blockInfo.blockDataAs.whileBlock.inStack.ops=NULL;
          blockInfo.blockDataAs.whileBlock.outStack.types=NULL;
          blockInfo.blockDataAs.whileBlock.outStack.ops=NULL;
          blockInfo.blockId=state->whileCount++;
          //store types at loop start
          initWhileTypes(state,&(blockInfo.blockDataAs.whileBlock),op.filePos);
          if(resetStack(state,&(blockInfo.blockDataAs.whileBlock.inStack)))
            handleError(NULL,ERROR_TYPE,op.filePos);
          if(pushBlock(state,blockInfo))
            handleError(NULL,ERROR_TYPE,op.filePos);
          op.dataAs.block.type=BLOCK_DO;
          op.dataAs.block.id=blockInfo.blockId;
          pushCompiledOperation(state,op);
          return;
        case BLOCK_DO:
          checkReachable(state,op);
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr==NULL||blockInfoPtr->type!=BLOCK_WHILE){//wrong position for DO
            fputs("DO can only appear in WHILE-DO blocks\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          if(blockInfoPtr->blockDataAs.whileBlock.hasDo){//wrong position for DO
            fputs("DO cannot appear more than once per WHILE block\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          /*XXX simplify code for while-blocks with empty condition/body:
            while do #+body+# end -> 
            tmp =init
            while(tmp){
              // body
            }
            while #+body+# #+condition+# do end -> 
            do{
              //body
            }while(condition);
          */
          //store types at loop condition
          checkWhileOutTypes(state,&(blockInfoPtr->blockDataAs.whileBlock),true,op.filePos);
          //update block
          blockInfoPtr->blockDataAs.whileBlock.hasDo=true;
          op.dataType=primitiveType(PRIMITIVE_BOOL);
          op.dataAs.block.type=BLOCK_WHILE;
          requireTypes("while-condition",state,&op.dataType,1,op.filePos);
          offset=state->typeCount-1;
          extractCompositeOps(state,1,false);
          op.dataAs.block.id=blockInfoPtr->blockId;
          addCompiledOps(state,op,1); 
          //reset stack after compiling condition
          if(resetStack(state,&(blockInfoPtr->blockDataAs.whileBlock.outStack)))
            handleError(NULL,ERROR_TYPE,op.filePos);
          return;
        case BLOCK_BREAK:
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr!=NULL&&(blockInfoPtr->type==BLOCK_CASE||blockInfoPtr->type==BLOCK_DEFAULT)&&!blockInfoPtr->blockDataAs.switchBlock.explicitBreak){
            blockInfoPtr->blockDataAs.switchBlock.explicitBreak=true;
          }else{
            checkReachable(state,op);
          }
          blockInfoPtr=findBreakableBlock(state,true,true);
          if(blockInfoPtr==NULL){
            handleError("break can only appear in loops and switch-statements",ERROR_SYNTAX,op.filePos);
          }
          if(blockInfoPtr->type==BLOCK_WHILE||blockInfoPtr->type==BLOCK_DO){
            checkWhileOutTypes(state,&(blockInfoPtr->blockDataAs.whileBlock),false,op.filePos);
            blockInfoPtr->blockDataAs.whileBlock.hasBreak=true;
          }
          if(blockInfoPtr->type==BLOCK_CASE||blockInfoPtr->type==BLOCK_DEFAULT){
            checkSwitchTypes(state,&(blockInfoPtr->blockDataAs.switchBlock),op.filePos);
            blockInfoPtr->blockDataAs.switchBlock.endReachable=true;
          }
          op.dataAs.block.id=blockInfoPtr->blockId;
          pushCompiledOperation(state,op);
          state->reachable=false;
          return;
        case BLOCK_CONTINUE:
          checkReachable(state,op);
          blockInfoPtr=findBreakableBlock(state,true,false);
          if(blockInfoPtr==NULL||blockInfoPtr->type!=BLOCK_WHILE){
            fputs("break can only appear in while blocks\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          checkWhileTypes(state,&(blockInfoPtr->blockDataAs.whileBlock),op.filePos);
          op.dataAs.block.id=blockInfoPtr->blockId;
          pushCompiledOperation(state,op);
          state->reachable=false;
          return;
        case BLOCK_SWITCH:
          checkReachable(state,op);
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr==NULL||blockInfoPtr->type==BLOCK_UNKNOWN){//block stack underflow
            fputs("unexpected switch statement, switch statements cannot be declared at global level\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          blockInfo=(BlockInfo){.type=BLOCK_SWITCH,.blockStart=state->opCount,.blockDataAs={0}};
          switchBlock=&(blockInfo.blockDataAs.switchBlock);
          switchBlock->inStack.types=NULL;
          switchBlock->inStack.ops=NULL;
          switchBlock->outStack.types=NULL;
          switchBlock->outStack.ops=NULL;
          blockInfo.blockId=switchCount;
          switchBlock->switchData=newSwitchData(op.filePos);
          //store in-types
          if(state->typeCount>1){
            switchBlock->inStack.typeCount=state->typeCount-1;
            switchBlock->inStack.opCount=state->opStackCount-state->typeStack[state->typeCount-1].opCount;
            switchBlock->inStack.types=malloc((state->typeCount-1)*sizeof(TypeInfo));
            switchBlock->inStack.ops=malloc((switchBlock->inStack.opCount)*sizeof(Operation));
            if(switchBlock->inStack.types==NULL||switchBlock->inStack.ops==NULL)
              handleError(NULL,ERROR_MEMORY,op.filePos);
            memcpy(switchBlock->inStack.types,state->typeStack,(state->typeCount-1)*sizeof(TypeInfo));
            memcpy(switchBlock->inStack.ops,state->opStack,(switchBlock->inStack.opCount)*sizeof(Operation));
          }
          //determine switch type
          offset=state->typeCount-1;
          if(isIntType(&(state->typeStack[offset].type))){
            op.dataType=state->typeStack[offset].type;
          }else if(state->typeStack[offset].type.typeClass==TYPECLASS_ENUM||state->typeStack[offset].type.typeClass==TYPECLASS_ENUM_LABEL){
            op.dataType=state->typeStack[offset].type;
            op.dataType.typeClass=TYPECLASS_ENUM_LABEL;
          }else{
            fputs("cannot switch values of type ",stderr);
            printTypeName(&(state->typeStack[offset].type),stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          switchBlock->switchType=op.dataType;
          if(pushBlock(state,blockInfo))
            handleError(NULL,ERROR_MEMORY,op.filePos);
          requireTypes("switch-condition",state,&op.dataType,1,op.filePos);
          extractCompositeOps(state,1,false);
          op.dataAs.block.id=blockInfo.blockId;
          addCompiledOps(state,op,1);
          state->reachable=false;//section after switch is not reachable
          return;
        case BLOCK_CASE:
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr==NULL||(blockInfoPtr->type!=BLOCK_SWITCH&&blockInfoPtr->type!=BLOCK_CASE)){
            fputs("unexpected case statement, case statements are only allowed in switch-case blocks\n",stderr);
            if(blockInfoPtr!=NULL&&blockInfoPtr->type==BLOCK_DEFAULT)
              fputs("default has to be the last label in switch statement\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          switchBlock=&blockInfoPtr->blockDataAs.switchBlock;
          if(state->reachable)//case reachable ->  previous case not terminated
            handleError("unexpected case statement, case fall-through is not supported",ERROR_SYNTAX,op.filePos);
          if(switchBlock->switchData->cases[switchBlock->switchData->caseCount].count==0)
            handleError("case has be preceded by at least one label",ERROR_SYNTAX,op.filePos);
          if(switchBlock->switchData->caseCount>=switchBlock->switchData->caseCap-1)
            handleError("exceeded maximum case-count in switch",ERROR_MEMORY,op.filePos);
          switchBlock->switchData->cases[switchBlock->switchData->caseCount+1].offset=switchBlock->switchData->labelCount;
          state->reachable=true;
          //reset stack to in-types 
          if(resetStack(state,&(switchBlock->inStack)))
            handleError(NULL,ERROR_TYPE,op.filePos);
          //update block
          blockInfoPtr->type=BLOCK_CASE;
          switchBlock->explicitBreak=false;
          op.dataType=switchBlock->switchType;
          op.dataAs.block.id=blockInfoPtr->blockId;
          op.dataAs.block.subId=switchBlock->switchData->caseCount++;
          pushCompiledOperation(state,op);
          return;
        case BLOCK_DEFAULT:
          blockInfoPtr=peekBlock(state);
          if(blockInfoPtr==NULL||blockInfoPtr->type!=BLOCK_CASE){
            fputs("unexpected default statement, default statements are only allowed in switch-case blocks\n",stderr);
            if(blockInfoPtr!=NULL&&blockInfoPtr->type==BLOCK_SWITCH)
              fputs("switch statements have to contain at least one case\n",stderr);
            if(blockInfoPtr!=NULL&&blockInfoPtr->type==BLOCK_DEFAULT)
              fputs("switch statements can only contain one default block\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          switchBlock=&blockInfoPtr->blockDataAs.switchBlock;
          if(state->reachable)//case reachable ->  previous case not terminated
            handleError("unexpected default statement, case fall-through is not supported",ERROR_SYNTAX,op.filePos);
          if(switchBlock->switchData->cases[switchBlock->switchData->caseCount].count>0)
            handleError("default cannot be preceded by labels",ERROR_SYNTAX,op.filePos);
          if(switchBlock->switchData->caseCount++>=switchBlock->switchData->caseCap-1)
            handleError("exceeded maximum case-count in switch",ERROR_MEMORY,op.filePos);
          state->reachable=true;
          switchBlock->switchData->hasDefault=true;
          //reset stack to in-types 
          if(resetStack(state,&(switchBlock->inStack)))
            handleError(NULL,ERROR_TYPE,op.filePos);
          //update block
          blockInfoPtr->type=BLOCK_DEFAULT;
          switchBlock->explicitBreak=false;
          op.dataType=switchBlock->switchType;
          op.dataAs.block.id=blockInfoPtr->blockId;
          pushCompiledOperation(state,op);
          return;
        case BLOCK_UNKNOWN:
        case BLOCK_PROCEDURE:
          fprintf(stderr,"blocks of type %s are not supported",blockNames[op.dataAs.block.type]);
          handleError(NULL,ERROR_SYNTAX,op.filePos);
          break;
      }
      break;
    case OP_END_BLOCK:
      blockInfoPtr=peekBlock(state);//keep block on block stack until writing operations has finished
      if(blockInfoPtr==NULL||blockInfoPtr->type==BLOCK_UNKNOWN||(blockInfoPtr->type==BLOCK_WHILE&&!blockInfoPtr->blockDataAs.whileBlock.hasDo)||blockInfoPtr->type==BLOCK_SWITCH){
        fputs("unexpected END statement\n",stderr);
        handleError(NULL,ERROR_SYNTAX,op.filePos);
      }
      if(op.dataAs.block.type!=BLOCK_UNKNOWN){
        handleError("type-checking end for specified block-types is currently not implemented",ERROR_UNIMPLEMENTED,op.filePos);
      }
      op.dataAs.block.type=blockInfoPtr->type;
      op.dataAs.block.id=blockInfoPtr->blockId;
      int32_t endCount=1;
      switch(blockInfoPtr->type){
        case BLOCK_IF:
        case BLOCK_ELSE:
          ifBlock=&(blockInfoPtr->blockDataAs.ifBlock);
          endCount+=ifBlock->elifCount;
          if(state->reachable){
            checkIfTypes(state,ifBlock,op.filePos);
            ifBlock->endReachable=true;
          }
          bool endReachable=ifBlock->endReachable;
          if(endReachable){
            if(blockInfoPtr->type==BLOCK_ELSE){//if ends with else branch
              if(predeclareBlockVariables(state,blockInfoPtr->blockStart,&(ifBlock->outStack)))
                 handleError(NULL,ERROR_TYPE,op.filePos);
            }else{
              declareBlockVariables(state,blockInfoPtr->blockStart,&(ifBlock->outStack),&(ifBlock->inStack),"if",op.filePos);
            }
            if(resetStack(state,&(ifBlock->outStack))){
              handleError(NULL,ERROR_TYPE,op.filePos);
            }
          }else if(blockInfoPtr->type==BLOCK_IF){//reset stack to state before if-block
            if(resetStack(state,&(ifBlock->inStack))){
              handleError(NULL,ERROR_TYPE,op.filePos);
            }
          }else{//clear stack if end of if-block unreachable
            state->typeCount=0;
            state->opStackCount=0;
          }
          //free values on op-stack
          free(ifBlock->inStack.types);
          free(ifBlock->inStack.ops);
          free(ifBlock->outStack.types);
          free(ifBlock->outStack.ops);
          //next statement reachable if on if-branch terminates or if does not have an else branch
          state->reachable=endReachable||(blockInfoPtr->type==BLOCK_IF);
          break;
        case BLOCK_WHILE:
          if(!state->reachable){
            fputs("end of while block cannot be reached\n",stderr);
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          checkWhileTypes(state,&(blockInfoPtr->blockDataAs.whileBlock),op.filePos);
          if(predeclareBlockVariables(state,blockInfoPtr->blockStart,&(blockInfoPtr->blockDataAs.whileBlock.outStack)))
             handleError(NULL,ERROR_TYPE,op.filePos);
          if(resetStack(state,&(blockInfoPtr->blockDataAs.whileBlock.outStack)))
            handleError(NULL,ERROR_TYPE,op.filePos);
          state->reachable=true;
          break;
        case BLOCK_PROCEDURE:
          if(state->reachable&&blockInfoPtr->blockDataAs.procBlock.returnType.typeDataAs.composite->typeCount>0){//automatically add return statement at end of non-void procedures
            Operation ret=(Operation){.opType=OP_RETURN,.dataType=blockInfoPtr->blockDataAs.procBlock.returnType,.filePos=op.filePos,.dataAs={0}};
            typeCheckReturn(state,&ret);
          }else if(state->reachable&&checkNonemptyStack(state,"unfinished local operation")){
              handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          state->reachable=true;
          break;
        case BLOCK_CASE://TODO check if all enum labels of are covered
          if(state->reachable)
            handleError("missing break statement at end of case",ERROR_SYNTAX,op.filePos);
          switchBlock=&(blockInfoPtr->blockDataAs.switchBlock);
          switchBlock->switchData->caseCount++;//close last case
          declareBlockVariables(state,blockInfoPtr->blockStart,&(switchBlock->outStack),&(switchBlock->inStack),"switch",op.filePos);
          if(resetStack(state,&(switchBlock->outStack)))
            handleError(NULL,ERROR_TYPE,op.filePos);
          state->reachable=true;
          free(switchBlock->inStack.types);
          free(switchBlock->inStack.ops);
          free(switchBlock->outStack.types);
          free(switchBlock->outStack.ops);
          break;
        case BLOCK_DEFAULT:
          switchBlock=&(blockInfoPtr->blockDataAs.switchBlock);
          if(state->reachable){//end default section
            checkSwitchTypes(state,&(blockInfoPtr->blockDataAs.switchBlock),op.filePos);
            Operation tmp=opCodeBlock(BLOCK_BREAK,op.filePos);
            tmp.dataAs.block.id=blockInfoPtr->blockId;
            pushCompiledOperation(state,tmp);
          }
          if(switchBlock->endReachable){
            if(predeclareBlockVariables(state,blockInfoPtr->blockStart,&(switchBlock->outStack)))
               handleError(NULL,ERROR_TYPE,op.filePos);
            if(resetStack(state,&(switchBlock->outStack)))
              handleError(NULL,ERROR_TYPE,op.filePos);
          }
          state->reachable=switchBlock->endReachable;
          free(switchBlock->inStack.types);
          free(switchBlock->inStack.ops);
          free(switchBlock->outStack.types);
          free(switchBlock->outStack.ops);
          break;
        default:
          if(checkNonemptyStack(state,"unfinished local operation")){
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          state->reachable=true;
      }
      while(endCount-->0){
        pushCompiledOperation(state,op);
      }
      popBlock(state);//pop block after writing operations
      return;
    case OP_CALL_PTR:
      checkReachable(state,op);
      checkLocal(state,op);
      typeCheckCall(&op,state,true);
      return;
    case OP_CALL:
      checkReachable(state,op);
      checkLocal(state,op);
      typeCheckCall(&op,state,false);
      return;
    case OP_RETURN:  
      checkReachable(state,op);
      state->reachable=false;
      checkLocal(state,op);
      typeCheckReturn(state,&op);
      return;
    case ENTRY_POINT://start of procedure
      checkReachable(state,op);
      if(checkNonemptyStack(state,"unfinished global operation")){
        handleError(NULL,ERROR_SYNTAX,op.filePos);
      }
      //block id will be ignored
      blockInfo=(BlockInfo){.type=BLOCK_PROCEDURE,.blockStart=state->opCount,.blockId=-1,.blockDataAs={.procBlock={.returnType=compositeType(TYPECLASS_PROC_OUT,NULL,LABEL_ID_UNKNOWN,0)}}};
      if(pushBlock(state,blockInfo))
        handleError(NULL,ERROR_TYPE,op.filePos);
      pushCompiledOperation(state,op);
      return;
    case OP_IDENTIFIER:
    case OP_SET_IDENTIFIER:
    case OP_IDENTIFIER_ADDRESS:
      fprintf(stderr,"operation %s should not exist at this stage of compilation\n",opName(op.opType));
      handleError(NULL,ERROR_SYNTAX,op.filePos);
    //compile time ops
    case OP_MODIFY_STACK:
      checkReachable(state,op);
      switch(op.dataAs.stackMod.op){
        case STACK_OP_DUP://duplicate top value on stack
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          extractCompositeOps(state,1,true);
          totalOps=state->typeStack[state->typeCount-1].opCount;
          if(ensureOpStackCap(state,state->opStackCount+totalOps)||ensureTypeStackCap(state,state->typeCount+1))
            handleError(NULL,ERROR_TYPE,op.filePos);
          memmove(state->opStack+state->opStackCount,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
          memmove(state->typeStack+state->typeCount,state->typeStack+state->typeCount-1,sizeof(TypeInfo));
          state->opStackCount+=totalOps;
          state->typeCount++;
          return;
        case STACK_OP_DROP://remove top value from stack
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          state->typeCount--;
          state->opStackCount-=state->typeStack[state->typeCount].opCount;
          return;
        case STACK_OP_OVER:
          if(state->typeCount<2){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          extractCompositeOps(state,2,true);
          offset=state->typeStack[state->typeCount-1].opCount+state->typeStack[state->typeCount-2].opCount;
          totalOps=state->typeStack[state->typeCount-2].opCount;
          if(ensureOpStackCap(state,state->opStackCount+totalOps)||ensureTypeStackCap(state,state->typeCount+1))
            handleError(NULL,ERROR_TYPE,op.filePos);
          memmove(state->opStack+state->opStackCount,state->opStack+state->opStackCount-offset,totalOps*sizeof(Operation));
          memmove(state->typeStack+state->typeCount,state->typeStack+state->typeCount-2,sizeof(TypeInfo));
          state->opStackCount+=totalOps;
          state->typeCount++;
          return;
        case STACK_OP_SWAP:
          break;
      }
      break;
    case OP_COMPILER_INFO:
      switch(op.dataAs.compilerInfo.infoType){
        case COMPILERINFO_TYPES:
          puts("types:\n-----------------");
          printTypeStack(state,false,op.dataAs.compilerInfo.maxCount,stdout);
          puts("-----------------");
          return;
        case COMPILERINFO_STACK:
          puts("stack:\n-----------------");
          printTypeStack(state,true,op.dataAs.compilerInfo.maxCount,stdout);
          puts("-----------------");
          return;
      }
      break;
  }
  fputs("implementation for type checking of operation ",stderr);
  printOperation(op,stderr);
  fputs(" is incomplete\n",stderr);
  handleError(NULL,ERROR_UNIMPLEMENTED,op.filePos);
}
void typeCheckProgram(Program* prog,CodeFile* src){
  size_t opCap=prog->opCount>INIT_CAP?prog->opCount:INIT_CAP;
  TypeCheckState state=(TypeCheckState){
    .globalOperations=malloc(opCap*sizeof(Operation)),.globalCap=opCap,.globalCount=0,
    .compiledOperations=malloc(opCap*sizeof(Operation)),.opCap=opCap,.opCount=0,
    .opStack=malloc(INIT_CAP*sizeof(Operation)),.opStackCap=INIT_CAP,.opStackCount=0,
    .typeStack=malloc(INIT_CAP*sizeof(TypeInfo)),.typeStackCap=INIT_CAP,.typeCount=0,
    .openBlocks=malloc(INIT_CAP*sizeof(BlockInfo)),.blockCap=INIT_CAP,.blockCount=0,
    .predeclaredTypes=malloc(prog->predeclaredTypes*sizeof(DataType)),.nPredeclaredTypes=prog->predeclaredTypes,
    .globalScope=prog->globalScope,.tmpCount=0,.ifCount=0,.whileCount=0,.index=0,
    .reachable=true,.hasCheckBounds=false,.hasCheckEnum=false,};
  if(state.globalOperations==NULL||state.compiledOperations==NULL||state.opStack==NULL||state.typeStack==NULL||state.openBlocks==NULL||state.predeclaredTypes==NULL){//memory allocation failed
    freeContents(&state);
    handleError(NULL,ERROR_MEMORY,src->currentPos);
  }
  while(state.index<prog->opCount){
    typeCheckOperation(prog->ops[state.index++],&state);
  }
  if(state.blockCount>0){
    freeContents(&state);
    handleError("unfinished code-block",ERROR_SYNTAX,src->currentPos);
  }
  free(prog->ops);
  prog->globalOps=state.globalOperations;
  state.globalOperations=NULL;
  prog->globalCount=state.globalCount;
  prog->ops=state.compiledOperations;
  state.compiledOperations=NULL;
  prog->opCount=state.opCount;
  prog->hasCheckBounds=state.hasCheckBounds;
  prog->hasCheckEnum=state.hasCheckEnum;
  freeContents(&state);
}

/* Copied from StackOverflow
 * finds the size of the FILE at fp in byts 
 * returns a negative value if finding the size fails
 * fp is assumed to be non null
 * */
long int fsize(FILE *fp){
    long int prev=ftell(fp);
    if(fseek(fp, 0L, SEEK_END)!=0){
		  return -1;
    }
    long int sz=ftell(fp);
    //go back to where we were
    if(fseek(fp,prev,SEEK_SET)!=0){
		  return -1;
    }
    return sz;
}

char const* path;
char const* srcFile;
char const* targetFile;
int main(int argc,char** argv){
  (void)argc;
  char* code;
  int64_t codeSize;
  path=*(argv++);
  if(*argv==NULL){
    printf("usage: %s inputFile\n",path);
    printf("or     %s inputFile outputFile\n",path);
    return 0;
  }
  //initialization of uninitialized global variables 
  initStringLabels();
  if(namespaceTrieInit()){
    fputs("failed to initialize namespace storage",stderr);
    exit(EXIT_FAILURE);
  }
  //read main source file
  srcFile=*(argv++);
  FILE *file = fopen(srcFile, "r");
  if(*argv==NULL){
    targetFile="./out.c";
  }else{
    targetFile=*(argv++);
  }
	if(file==NULL){
	  fprintf(stderr,"IO Error while opening File: %s\n",srcFile);
		return EXIT_FAILURE;
	}
	long int size=fsize(file);
	if(size<0){//TODO?? recover form undetected fileSize (if seek worked)
		fputs("IO Error while detecting file-size\n",stderr);
		return EXIT_FAILURE;
	}
	code = malloc((size+1)*sizeof(char));//will be freed when the program exits
	if(code==NULL){
		printf("Memory Error\n");
		return ERROR_MEMORY;
	}
	codeSize=fread(code,sizeof(char),size,file);//TODO perform multiple reads if necessary
	if(codeSize<0){
		printf("IO Error while reading file\n");
		free(code);
		return EXIT_FAILURE;
	}
	fclose(file);//file no longer needed
	memset(code+codeSize,0,(size+1-codeSize)*sizeof(char));//fill remaining path of file with 0
	//1. compile file to operations
	CodeFile codeFile=(CodeFile){.code=code,.codeSize=codeSize,
	  .currentPos={.fileName=srcFile,.line=1,.posInLine=1},
	  .wordStart={.fileName=srcFile,.line=1,.posInLine=1}};
	Program p=compileToOps(&codeFile);
	if(p.ops==NULL)
	  return ERROR_SYNTAX;
  printf("found %zu operations\n",p.opCount);
  //2. save intermediate representation
  char const* opsFile="./parser.out";
  FILE* intermediate=fopen(opsFile,"w");
	if(intermediate==NULL){
	  fprintf(stderr,"IO Error while opening File: %s\n",opsFile);
		return EXIT_FAILURE;
	}
  for(size_t i=0;i<p.globalCount;i++){
    printOperation(p.globalOps[i],intermediate);
  }
  for(size_t i=0;i<p.opCount;i++){
    printOperation(p.ops[i],intermediate);
  }
  fclose(intermediate);
	//3. type-check operations
  typeCheckProgram(&p,&codeFile);
  printf("compiled to %zu operations\n",p.globalCount+p.opCount);
  //4. save intermediate representation
  opsFile="./typeCheck.out";
  intermediate=fopen(opsFile,"w");
	if(intermediate==NULL){
	  fprintf(stderr,"IO Error while opening File: %s\n",opsFile);
		return EXIT_FAILURE;
	}
  for(size_t i=0;i<p.globalCount;i++){
    printOperation(p.globalOps[i],intermediate);
  }
  for(size_t i=0;i<p.opCount;i++){
    printOperation(p.ops[i],intermediate);
  }
  fclose(intermediate);
	//5. compile operations to C
  FILE* out=fopen(targetFile,"w");
	if(out==NULL){
	  fprintf(stderr,"IO Error while opening File: %s\n",targetFile);
		return EXIT_FAILURE;
	}
  compileToC(out,&p);
  puts("compiled program");
  fclose(out);
  return EXIT_SUCCESS;
}
