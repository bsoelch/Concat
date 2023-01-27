#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

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
#define ERROR_UNSUPPORTED_ESCAPE_SEQUENCE 6
#define WARNING_CODEPOINT_OUT_OF_RANGE 7
#define ERROR_EOF 8 //end of file


//exit codes for errors in compiled program
#define PROG_EXIT_CODE_ARRAY_OUT_OF_RANGE 1
#define PROG_EXIT_CODE_WRONG_ENUM_INDEX   2

//negate indices (internal errors have negative error codes)
const char* const internalErrors [] = {[-ERROR_MEMORY]="ERROR_MEMORY",[-ERROR_IO]="ERROR_IO",[-ERROR_UNIMPLEMENTED]="ERROR_UNIMPLEMENTED",};
const char* const compilerErrors [] = {
[ERROR_TYPE]="type error",[ERROR_SYNTAX]="syntax error",[ERROR_PARSE_INT]="invalid character while parsing integer",[ERROR_INT_OVERFLOW]="integer exceeds maximum allowed value",
[ERROR_REDECLARATION]="redeclaration",[ERROR_UNSUPPORTED_ESCAPE_SEQUENCE]="unsupported escape sequence",[WARNING_CODEPOINT_OUT_OF_RANGE]="code-point out of range",
[ERROR_EOF]="unexpected end of file",};
const char* errorName(int errorCode){
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

typedef struct{
  const char* fileName;
  size_t line;
  size_t posInLine;
}FilePosition;
void printFilePosition(FilePosition pos,FILE* out){
  fprintf(out,"%s:%zu:%zu",pos.fileName,pos.line,pos.posInLine);
}
typedef struct{
  int errorCode;
  FilePosition pos;
}Error;
void printError(Error err,FILE* out){
  fprintf(out,"%s at ",errorName(err.errorCode));
  printFilePosition(err.pos,out);
  fputs("\n",out);
}
typedef struct{
  bool isError;
  union{
    size_t size;
    Error  error;
  }as;
}SizeOrError;
typedef struct{
  bool isError;
  union{
    int64_t  i64;
    int    error;
  }as;
}IntOrErrorCode;
typedef struct{
  char* chars;
  size_t length;
}String;
int stringCompare(const String a,const String b){
  int c=memcmp(a.chars,b.chars,a.length<b.length?a.length:b.length);
  if(c==0&&a.length!=b.length)
    return a.length<b.length?-1:1;
  return c;
}
int32_t stringHash(const String s){
  int32_t hash=0;
  for(size_t i=0;i<s.length;i++){
    hash=31*hash+s.chars[i];
  }
  return hash;
}
int64_t indexOfString(const String base,const String child){
  if(child.length>base.length)
    return -1;
  bool isMatch;
  for(size_t off=0;off<=base.length-child.length;off++){
    isMatch=true;
    for(size_t i=0;i<child.length;i++){
      if(base.chars[i+off]!=child.chars[i]){
        isMatch=false;
        break;
      }
    }
    if(isMatch)
      return off;
  }
  return -1;
}
int64_t indexOfStringArray(const String* base,size_t baseLen,const String* child,size_t childLen){
  if(childLen>baseLen)
    return -1;
  bool isMatch;
  for(size_t off=0;off<=baseLen-childLen;off++){
    isMatch=true;
    for(size_t i=0;i<childLen;i++){
      if(stringCompare(base[i+off],child[i])!=0){
        isMatch=false;
        break;
      }
    }
    if(isMatch)
      return off;
  }
  return -1;
}
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
  OP_GET_LABEL,
  
  OP_IDENTIFIER,
  OP_IDENTIFIER_ADDRESS,
  
  OP_SET_VALUE, //  [T] [T.writable] SET 
  
  OP_NEW,
  OP_CAST,
  OP_ADDR_OF,// (pointer to given value)
  
  OP_BINARY_OPERATOR, 
  OP_UNARY_OPERATOR,  
  
  OP_CHECK_ARRAY_BOUNDS,//special operation for checking array bounds             params: index length            exits the program if index < 0 or index >= length
  OP_CHECK_ENUM_INDEX,//special operation for checking if enum index corresponds to current value  params: enum   exits the program if enum.lable != data.asI64 
  
  OP_CODE_BLOCK,  
  
  OP_RETURN,       
  OP_CALL,         // procType procId  
  OP_CALL_PTR,    
  ENTRY_POINT,     //entry point of the program, starts the main code section, section will close at the matching BLOCK_END 
  
  //compile-time operations
  OP_MODIFY_STACK,  
  OP_TYPE_INFO,
}OpType;
const char* opName(OpType type){
  switch(type){
    case OP_PRINT:return "OP_PRINT";
    case OP_CONSTANT:return "OP_CONSTANT";
    case OP_DECLARE:return "OP_DECLARE";
    case OP_PRE_DECLARE:return "OP_PRE_DECLARE";
    case OP_GET:return "OP_GET";
    case OP_GET_LABEL:return "OP_GET_LABEL";
    case OP_IDENTIFIER:return "OP_IDENTIFIER";
    case OP_IDENTIFIER_ADDRESS:return "OP_IDENTIFIER_ADDRESS";
    case OP_SET_VALUE:return "OP_SET_VALUE";
    case OP_BINARY_OPERATOR:return "OP_BINARY_OPERATOR";
    case OP_UNARY_OPERATOR:return "OP_UNARY_OPERATOR";  
    case OP_CODE_BLOCK:return "OP_CODE_BLOCK";
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
    case OP_TYPE_INFO:return "OP_TYPE_INFO";
  }
  return "UNDEFINED";
}
const char* CHECK_BOUNDS_NAME="concatInternal_checkArrayBounds";
const char* CHECK_ENUM_INDEX_NAME="concatInternal_checkEnumIndex";
//types
typedef enum{
  TYPECLASS_UNDEFINED,
  TYPECLASS_PRIMITIVE,
  TYPECLASS_POINTER,
  TYPECLASS_CONST_POINTER,
  TYPECLASS_TUPLE,
  TYPECLASS_FLAT_TUPLE,//behaves like tuple but will not be directly used
  TYPECLASS_PROCEDURE,
  TYPECLASS_TYPE_OF,
  TYPECLASS_OPAQUE,
  TYPECLASS_STRUCT,
  TYPECLASS_ENUM,
  TYPECLASS_ENUM_LABEL,
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
typedef struct DataType{
  TypeClass typeClass;
  union{
    PrimitiveType primitive;
    struct DataType* type;
    CompositeType* composite;
    TupleType* tuple;//name alias for composite
    ProcedureType* procedure;
    int64_t typeId;
  }typeDataAs;
  bool isAddressable;
  bool isWritable;
}DataType;
#define FLAG_IS_TUPLE      1
#define FLAG_IS_FLAT_TUPLE 2
#define FLAG_IS_STRUCT     4
#define FLAG_IS_ENUM       8
#define FLAG_VOID_ONLY     16
struct CompositeType{
  DataType* types;
  String*   labels;
  int32_t id;
  int16_t typeCount;
  int16_t flags;
};
struct ProcedureType{
  int32_t id;
  struct DataType* inType;
  struct DataType* outType;
};

const DataType TYPE_UNDEFINED={.typeClass=TYPECLASS_UNDEFINED,.typeDataAs={0},.isAddressable=false,.isWritable=false};

#define MAX_TYPES       4096
#define MAX_COMPOSITE   1024
#define TYPE_BUFFER_CAP 1024
#define MAX_PROC_TYPES  1024

size_t wrappedTypeCount=0;
DataType wrappedTypes[MAX_TYPES];
int32_t compositeCount=0;
CompositeType compositeTypes[MAX_COMPOSITE];
size_t procTypeCount=0;
ProcedureType procTypes[MAX_PROC_TYPES];
//temporary buffer for construction of composite elements
size_t bufferedTypes=0;
DataType typeBuffer[TYPE_BUFFER_CAP];
size_t bufferedFieldNames=0;
String fieldNameBuffer[TYPE_BUFFER_CAP];

bool typeEquals(const DataType* a,const DataType* b){
  if(a->typeClass!=b->typeClass)
    return false;
  if(a->typeClass==TYPECLASS_UNDEFINED)
    return true;//all undefined types are equal
  if(a->typeClass==TYPECLASS_PRIMITIVE)
    return a->typeDataAs.primitive==b->typeDataAs.primitive;
  if(a->typeClass==TYPECLASS_POINTER||a->typeClass==TYPECLASS_CONST_POINTER||a->typeClass==TYPECLASS_TYPE_OF)
    return typeEquals(a->typeDataAs.type,b->typeDataAs.type);
  if(a->typeClass==TYPECLASS_TUPLE||a->typeClass==TYPECLASS_FLAT_TUPLE||a->typeClass==TYPECLASS_STRUCT||a->typeClass==TYPECLASS_ENUM||a->typeClass==TYPECLASS_ENUM_LABEL)
    return a->typeDataAs.composite->id==b->typeDataAs.composite->id;
  if(a->typeClass==TYPECLASS_PROCEDURE)
    return typeEquals(a->typeDataAs.procedure->inType,b->typeDataAs.procedure->inType)&&
            typeEquals(a->typeDataAs.procedure->outType,b->typeDataAs.procedure->outType);
  if(a->typeClass==TYPECLASS_OPAQUE)
    return a->typeDataAs.typeId==b->typeDataAs.typeId;
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
bool isPrimitiveType(const DataType* type){
  return type->typeClass==TYPECLASS_PRIMITIVE;
}
bool isVoidType(const DataType* type){
  return isPrimitiveType(type)&&type->typeDataAs.primitive==PRIMITIVE_VOID;
}
bool isBoolType(const DataType* type){
  return isPrimitiveType(type)&&type->typeDataAs.primitive==PRIMITIVE_BOOL;
}
bool isIntType(const DataType* type){
  return isPrimitiveType(type)&&isInteger(type->typeDataAs.primitive);
}
bool isNumberType(const DataType* type){
  return isPrimitiveType(type)&&numberRank(type->typeDataAs.primitive)>-1;
}
bool isPointerType(const DataType* type){
  return type->typeClass==TYPECLASS_POINTER||type->typeClass==TYPECLASS_CONST_POINTER;
}
bool isCallableType(const DataType* type){
  if(isPointerType(type))
    type=type->typeDataAs.type;
  return type->typeClass==TYPECLASS_PROCEDURE;
}
//checks id type is an array-type  a tuple consisting of a pointer and an integer
bool isArrayType(const DataType* type){
  if(type->typeClass!=TYPECLASS_STRUCT)
    return false;
  CompositeType* elts=type->typeDataAs.composite;
  if(elts->typeCount!=2)
    return false;
  //TODO check label names
  if(!isPointerType(elts->types+0))
    return false;
  if(!isIntType(elts->types+1))
    return false;
  return true;
}

DataType primitiveType(PrimitiveType id){
  return (DataType){.typeClass=TYPECLASS_PRIMITIVE,.typeDataAs={.primitive=id},.isAddressable=false,.isWritable=false};
}
DataType opaqueType(int64_t typeId){
  return (DataType){.typeClass=TYPECLASS_OPAQUE,.typeDataAs={.typeId=typeId},.isAddressable=false,.isWritable=false};
}
DataType wrapperType(TypeClass typeClass,const DataType* target){
  for(size_t i=0;i<wrappedTypeCount;i++){
    if(typeEquals(target,&(wrappedTypes[i])))
      return (DataType){.typeClass=typeClass,.typeDataAs={.type=wrappedTypes+i},.isAddressable=false,.isWritable=false};
  }
  if(wrappedTypeCount+1>=MAX_TYPES){
    return TYPE_UNDEFINED;
  }
  wrappedTypes[wrappedTypeCount]=*target;
  return (DataType){.typeClass=typeClass,.typeDataAs={.type=wrappedTypes+wrappedTypeCount++},.isAddressable=false,.isWritable=false};
}
DataType pointerType(const DataType* target){
  return wrapperType(TYPECLASS_POINTER,target);
}
DataType constPointerType(const DataType* target){
  return wrapperType(TYPECLASS_CONST_POINTER,target);
}
DataType typeOfType(const DataType* conent){
  return wrapperType(TYPECLASS_TYPE_OF,conent);
}
int64_t indexOfTypeArray(const DataType* base,size_t baseLen,const DataType* child,size_t childLen){
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
DataType compositeType(TypeClass typeClass,DataType* elements,String* labelNames,int32_t eltCount){
  if(eltCount==0)
    return typeClass==TYPECLASS_FLAT_TUPLE?primitiveType(PRIMITIVE_VOID):TYPE_UNDEFINED;//empty flat-tuple -> void other empty composites are undefined
  if(eltCount==1&&typeClass==TYPECLASS_FLAT_TUPLE)
    return elements[0];//auto unwrap 1-element flat-tuple
  int16_t classFlag;
  switch(typeClass){
    case TYPECLASS_FLAT_TUPLE:
      classFlag=FLAG_IS_FLAT_TUPLE;
      break;
    case TYPECLASS_TUPLE:
      classFlag=FLAG_IS_TUPLE;
      break;
    case TYPECLASS_STRUCT:
      if(labelNames==NULL)
        return TYPE_UNDEFINED;
      classFlag=FLAG_IS_STRUCT;
      break;
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      if(labelNames==NULL)
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
  int64_t typeMatch=-1,typesIndex;
  int64_t labelMatch=-1,labelsIndex;
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].typeCount==eltCount||(typeMatch==-1&&compositeTypes[i].typeCount>eltCount)){
      typesIndex=indexOfTypeArray(compositeTypes[i].types,compositeTypes[i].typeCount,elements,eltCount);
      if(typesIndex==-1)
        continue;
      typeMatch=i;
      if(labelNames!=NULL&&compositeTypes[i].labels!=NULL&&indexOfStringArray(compositeTypes[i].labels,compositeTypes[i].typeCount,labelNames,eltCount)!=0)
        continue;//labels do not match -> cannot reuse composite
      if(compositeTypes[i].typeCount==eltCount){
        if(labelNames!=NULL&&compositeTypes[i].labels==NULL){
          compositeTypes[i].labels=malloc(eltCount*sizeof(String));//will persist until program exits
          if(compositeTypes[i].labels==NULL)
            return TYPE_UNDEFINED;
          memcpy(compositeTypes[i].labels,labelNames,eltCount*sizeof(String));
        }
        compositeTypes[i].flags|=classFlag;
        return (DataType){.typeClass=typeClass,.typeDataAs.composite=compositeTypes+i,.isAddressable=false,.isWritable=false};
      }
    }
  }
  if(compositeCount+1>=MAX_COMPOSITE)
    return TYPE_UNDEFINED;
  DataType* types;
  String*  labels=NULL;
  if(labelNames!=NULL){
    for(int32_t i=0;i<compositeCount;i++){//find matching labels
      if(compositeTypes[i].typeCount>=eltCount){
        labelsIndex=indexOfTypeArray(compositeTypes[i].types,compositeTypes[i].typeCount,elements,eltCount);
        if(labelsIndex==-1)
          continue;
        labelMatch=i;
        break;
      }
    }
    if(labelMatch!=-1){
      labels=compositeTypes[typeMatch].labels+labelsIndex;
    }else{
      labels=malloc(eltCount*sizeof(String));//will persist until program exits
      if(labels==NULL)
        return TYPE_UNDEFINED;
      memcpy(labels,labelNames,eltCount*sizeof(String));
    }
  }
  if(typeMatch!=-1){
    types=compositeTypes[typeMatch].types+typesIndex;
  }else{
    types=malloc(eltCount*sizeof(DataType));//will persist until program exits
    if(types==NULL)
      return TYPE_UNDEFINED;
    memcpy(types,elements,eltCount*sizeof(DataType));
  }
  compositeTypes[compositeCount]=(CompositeType){.id=compositeCount,.typeCount=eltCount,.types=types,.labels=labels,.flags=classFlag};
  return (DataType){.typeClass=typeClass,.typeDataAs={.composite=compositeTypes+(compositeCount++)},.isAddressable=false,.isWritable=false};
}
DataType procedureType(const DataType* inType,const DataType* outType){
  for(size_t i=0;i<procTypeCount;i++){
    if(typeEquals(procTypes[i].inType,inType)&&typeEquals(procTypes[i].outType,outType))
      return (DataType){.typeClass=TYPECLASS_PROCEDURE,.typeDataAs={.procedure=procTypes+i},.isAddressable=false,.isWritable=false};
  }
  int32_t inId=-1,outId=-1;
  for(size_t i=0;i<wrappedTypeCount&&(inId==-1||outId==-1);i++){
    if(inId==-1&&typeEquals(inType,&(wrappedTypes[i])))
      inId=i;
    if(outId==-1&&typeEquals(outType,&(wrappedTypes[i])))
      outId=i;
  }
  if(inId==-1){
    if(wrappedTypeCount+1>=MAX_TYPES){
      return TYPE_UNDEFINED;
    }
    inId=wrappedTypeCount;
    wrappedTypes[wrappedTypeCount++]=*inType;
  }
  if(outId==-1){
    if(wrappedTypeCount+1>=MAX_TYPES){
      return TYPE_UNDEFINED;
    }
    outId=wrappedTypeCount;
    wrappedTypes[wrappedTypeCount++]=*outType;
  }
  procTypes[procTypeCount]=(ProcedureType){.id=procTypeCount,.inType=wrappedTypes+inId,.outType=wrappedTypes+outId};
  return (DataType){.typeClass=TYPECLASS_PROCEDURE,.typeDataAs={.procedure=procTypes+procTypeCount++},.isAddressable=false,.isWritable=false};
}
DataType asWritableType(DataType src,bool isAddressable){
  src.isAddressable=isAddressable;
  src.isWritable=true;
  return src;
}
DataType asAddressableType(DataType src){
  src.isAddressable=true;
  src.isWritable=false;
  return src;
}
DataType asConstType(DataType src){
  src.isAddressable=false;
  src.isWritable=false;
  return src;
}


const char* typeClassName(TypeClass cls){
  switch(cls){
    case TYPECLASS_UNDEFINED:
      return "UNDEFINED";
    case TYPECLASS_PRIMITIVE:
      return "primitive";
    case TYPECLASS_POINTER:
      return "pointer";
    case TYPECLASS_CONST_POINTER:
      return "pointer const";
    case TYPECLASS_TUPLE:
      return "tuple";
    case TYPECLASS_FLAT_TUPLE:
      return "flat tuple";
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
  }
  fprintf(stderr,"unexpected type-class %i",cls);
  return "";
}
const char* primitiveName(PrimitiveType t){
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
int printTypeNameIntenal(const DataType* type,FILE* file,bool noRecurse){
  int i,j;
  switch(type->typeClass){
    case TYPECLASS_UNDEFINED:
      if(type->typeDataAs.typeId>0){
        return fprintf(file,"PREDECLARED %"PRIi64,type->typeDataAs.typeId);
      }
      return fputs("UNDEFINED",file);
    case TYPECLASS_PRIMITIVE:
      return fprintf(file,"%s",primitiveName(type->typeDataAs.primitive));
    case TYPECLASS_OPAQUE:
      return fprintf(file,"OPAQUE %"PRIi64,type->typeDataAs.typeId);
    case TYPECLASS_CONST_POINTER:
    case TYPECLASS_POINTER:
    case TYPECLASS_TYPE_OF:
      i=printTypeNameIntenal(type->typeDataAs.type,file,noRecurse);
      if(i<0)
        return i;
      j=fprintf(file," %s",typeClassName(type->typeClass));
      return j<0?j:(i+j);
    case TYPECLASS_FLAT_TUPLE:
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      if(type->typeClass!=TYPECLASS_FLAT_TUPLE){
        i=fprintf(file,"%s (%"PRIi32") ",typeClassName(type->typeClass),type->typeDataAs.composite->id);
        if(noRecurse||i<0)
          return i;
      }
      j=fputs("(",file);
      if(j<0)
        return j;
      i+=j;
      for(int32_t e=0;e<type->typeDataAs.composite->typeCount;e++){
        if((type->typeClass==TYPECLASS_ENUM||type->typeClass==TYPECLASS_ENUM_LABEL)&&isVoidType(&(type->typeDataAs.composite->types[e]))){
          //void-type in enum -> only print label
          j=fprintf(file," %.*s",(int)type->typeDataAs.composite->labels[e].length,type->typeDataAs.composite->labels[e].chars);
          if(j<0)
            return j;
          i+=j;
          continue;
        }
        j=fputs(" ",file);
        if(j<0)
          return j;
        i+=j;
        j=printTypeNameIntenal(&(type->typeDataAs.composite->types[e]),file,true);//only one recursion level
        if(j<0)
          return j;
        i+=j;
        if(type->typeClass==TYPECLASS_TUPLE||type->typeClass==TYPECLASS_FLAT_TUPLE)
          continue;
        j=fprintf(file," : %.*s",(int)type->typeDataAs.composite->labels[e].length,type->typeDataAs.composite->labels[e].chars);
        if(j<0)
          return j;
        i+=j;
      }
      j=fputs(" )",file);
      if(j<0)
        return j;
      i+=j;
      return i;
    case TYPECLASS_PROCEDURE:
      i=fprintf(file,"%s (%"PRIi32") ",typeClassName(type->typeClass),type->typeDataAs.procedure->id);
      if(noRecurse||i<0)
        return i;
      j=fputs("( ",file);
      if(j<0)
        return j;
      i+=j;
      j=printTypeNameIntenal(type->typeDataAs.procedure->inType,file,true);
      if(j<0)
        return j;
      i+=j;
      j=fputs(" => ",file);
      if(j<0)
        return j;
      i+=j;
      j=printTypeNameIntenal(type->typeDataAs.procedure->outType,file,true);
      if(j<0)
        return j;
      j=fputs(" )",file);
      return j<0?j:(i+j);
  }
  return fprintf(file,"unknown type-class %i",type->typeClass);
}
int printTypeName(const DataType* type,FILE* file){
  return printTypeNameIntenal(type,file,false);
}

const char* primitiveNameC(PrimitiveType t){
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
int printTypeNameC(const DataType* type,FILE* file){
  int i,j;
  switch(type->typeClass){
    case TYPECLASS_TYPE_OF://type of does not correspond to a C-type
    case TYPECLASS_OPAQUE://pointer to opaque type -> void pointer
    case TYPECLASS_UNDEFINED:
      return fputs("void",file);
    case TYPECLASS_PRIMITIVE:
      return fprintf(file,"%s",primitiveNameC(type->typeDataAs.primitive));
    case TYPECLASS_CONST_POINTER:
      i=fputs("const ",file); //only difference to TYPECLASS_POINTER
      if(i<0)
        return i;
      // fall through
    case TYPECLASS_POINTER:
      j=printTypeNameC(type->typeDataAs.type,file);
      if(j<0)
        return j;
      if(isCallableType(type))
        return j;
      i+=j;
      j=fputs("*",file);
      return j<0?j:(i+j);
    case TYPECLASS_FLAT_TUPLE:
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
      return fprintf(file,"tuple%"PRIi32,type->typeDataAs.composite->id);
    case TYPECLASS_PROCEDURE:
      return fprintf(file,"procPtr%"PRIi32,type->typeDataAs.procedure->id);
    case TYPECLASS_ENUM:
      return fprintf(file,"enum%"PRIi32,type->typeDataAs.composite->id);
    case TYPECLASS_ENUM_LABEL:
      return fputs("int32_t",file);//XXX choose different int-type
  }
  return fprintf(file,"unknown type-class %i\n",type->typeClass);
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
const char* binOpName(BinaryOperator op){
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
const char* unOpName(UnaryOperator op){
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
  ID_INTERMEDIATE_RESULT,
  ID_TMP_VAR,
  ID_TYPE,
}IdentifierType;
typedef struct{
  int32_t id;
  IdentifierType type;
}IdentifierInfo;
const char* const idNames []={[ID_LOCAL_VAR]="local variable",[ID_GLOBAL_VAR]="global variable",[ID_ARGUMENT]="procedure argument",
  [ID_PROCEDURE]="procedure",[ID_TUPLE]="(tuple element)",[ID_TUPLE_ELEMENT]="tuple element",[ID_ENUM_LABEL]="enum label",[ID_ENUM_ELEMENT]="enum element",[ID_POINTER]="pointer value",[ID_POINTER_OFFSET]="array element",
  [ID_INTERMEDIATE_RESULT]="intermediate result",[ID_TMP_VAR]="temporary variable",[ID_TYPE]="type"};
void printIdInfo(IdentifierInfo info,FILE* out){
  fprintf(out,"%s (%"PRIi32")",idNames[info.type],info.id);
}

typedef enum{
  BLOCK_PROCEDURE, 
  BLOCK_START,     // {
  BLOCK_IF,        // if( EXPR ){
  BLOCK_IF2,       // if(EXPR){ ... } (auto-closes at end of current if -statement)
  BLOCK_ELSE,      // }else{
  BLOCK_WHILE,     // while( EXPR ){
  BLOCK_DO,        // do{
  BLOCK_WHILE_END, // }while( EXPR );
  BLOCK_END,       // }
}BlockType;
const char* const blockNames []={[BLOCK_PROCEDURE]="procedure",[BLOCK_START]="start",[BLOCK_IF]="if",
  [BLOCK_IF2]="_if",[BLOCK_ELSE]="else",[BLOCK_WHILE]="while",[BLOCK_DO]="do",[BLOCK_WHILE_END]="while end",[BLOCK_END]="end"};


typedef enum{
  STACK_OP_DUP,
  STACK_OP_DROP,
  STACK_OP_SWAP,
  //XXX? over, rotate
}StackOperation;
typedef struct{
  //XXX multi-drop/dup
  StackOperation op;
}StackModification;
typedef enum{
  TYPEINFO_TYPES,
  TYPEINFO_STACK,
}TypeInfoType;
typedef struct{
  int32_t maxCount;
  TypeInfoType infoType;
}TypeStackInfo;

typedef struct{
  OpType opType;
  DataType dataType;
  FilePosition filePos;
  union{
    int64_t i64;
    BinaryOperator binOp;
    UnaryOperator unOp;
    IdentifierInfo idInfo;
    BlockType block;
    String string;
    StackModification stackMod;
    TypeStackInfo typeInfo;
  }dataAs;
}Operation;

void printOperation(Operation op,FILE* out){
  fprintf(out,"%s ",opName(op.opType));
  if(op.dataType.typeClass!=TYPECLASS_UNDEFINED)
    printTypeName(&op.dataType,out);
  switch(op.opType){
    case OP_CONSTANT:
    case OP_CHECK_ENUM_INDEX:
      fprintf(out," (%"PRIi64")",op.dataAs.i64);
      break;
    case OP_GET:
    case OP_DECLARE:
    case OP_PRE_DECLARE:
    case OP_CALL:
    case OP_CHECK_ARRAY_BOUNDS:
      fputs(" ",out);
      printIdInfo(op.dataAs.idInfo,out);
      break;
    case OP_BINARY_OPERATOR:
      fprintf(out," %s",binOpName(op.dataAs.binOp));
      break;
    case OP_UNARY_OPERATOR:
      fprintf(out," %s",unOpName(op.dataAs.unOp));
      break;
    case OP_CODE_BLOCK:
      fprintf(out,"%s",blockNames[op.dataAs.block]);
      break;
    case OP_GET_LABEL:
    case OP_IDENTIFIER:
    case OP_IDENTIFIER_ADDRESS:
      fprintf(out,"%.*s",(int)op.dataAs.string.length,op.dataAs.string.chars);
      break;
    default:
      //ignore remaining types
      break;
  }
  fputs("\n",out);
}


#define SCOPE_NODE_CAP 8192
#define SCOPE_CAP 256
#define SCOPE_MAP_CAP 1024
typedef struct ScopeNode ScopeNode;
struct ScopeNode{
  String key;
  DataType type;
  ScopeNode* next;
  int32_t id;
  IdentifierType idType;
};
typedef struct Scope{
  ScopeNode** nodes;
  
  BlockType scopeType;
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
Scope* openScope(BlockType scopeType){
  if(scopeCount+1>=SCOPE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of nested scopes %i\n",SCOPE_CAP);
    return NULL;
  }
  scopeBuffer[scopeCount].nodes=calloc(SCOPE_MAP_CAP,sizeof(ScopeNode*));
  scopeBuffer[scopeCount].nodeBufferOffset=scopeNodeCount;
  scopeBuffer[scopeCount].scopeType=scopeType;
  scopeBuffer[scopeCount].parent=scopeCount>0?scopeBuffer+(scopeCount-1):NULL;
  return scopeBuffer+(scopeCount++);
}
BlockType currentScopeType(void){
  if(scopeCount<=0)
    return BLOCK_END;
  return scopeBuffer[scopeCount-1].scopeType;
}
bool closeScope(void){
  if(scopeCount<=0)
    return false;
  scopeCount--;
  free(scopeBuffer[scopeCount].nodes);
  scopeNodeCount=scopeBuffer[scopeCount].nodeBufferOffset;
  return true;
}
ScopeNode** findNode(Scope* scope,String name){
  if(scope==NULL)
    return NULL;
  uint32_t hash=stringHash(name);
  ScopeNode** node=scope->nodes+(hash%SCOPE_MAP_CAP);
  while(*node!=NULL){
    if(stringCompare((*node)->key,name)==0)
      return node;
    node=&((*node)->next);
  }
  return node;
}
int declareIdentifier(String name,DataType type,IdentifierType idType,ScopeNode** out){
  ScopeNode** node=findNode(scopeBuffer+(scopeCount-1),name);
  if(node==NULL)
    return ERROR_MEMORY;
  if(*node!=NULL)
    return ERROR_REDECLARATION;
  //TODO check for shadowed variable
  *node=allocScopeNode();
  if(*node==NULL)
    return ERROR_MEMORY;
  (*node)->key=name;
  (*node)->type=type;
  (*node)->idType=idType;
  (*node)->id=scopeNodeCount;
  (*node)->next=NULL;
  *out=*node;
  return 0;
}
int getIdentifier(String name,ScopeNode** out){
  int32_t level=scopeCount-1;
  ScopeNode** node;
  *out=NULL;
  while(level>=0){
    node=findNode(scopeBuffer+level,name);
    if(node==NULL)
      return ERROR_MEMORY;
    if(*node!=NULL){
      *out=*node;
      return 0;
    }
    level--;
  }
  return ERROR_SYNTAX;
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

DataType progStringType(void){
    DataType chr=primitiveType(PRIMITIVE_I8);//store in intermediate value to allow call by reference
    DataType stringElts[2]={constPointerType(&chr),primitiveType(PRIMITIVE_I64)};
    String   labels[2]={{.chars="raw",.length=3},{.chars="length",.length=6}};
    return compositeType(TYPECLASS_STRUCT,stringElts,labels,2);//ensure string-type exists
}
IntOrErrorCode addProgString(String s){
  if(progStringCount+1>=MAX_PROG_STRINGS)
    return (IntOrErrorCode){.isError=true,.as={.error=ERROR_MEMORY}};
  for(size_t i=0;i<progStringCount;i++){
    if(stringCompare(programStrings[i].value,s)==0)
      return (IntOrErrorCode){.isError=false,.as={.i64=i}};
  }
  programStrings[progStringCount]=(ProgramString){.value=s,.stringId=progStringCount,.charsId=-1,.charsOffset=-1};
  return (IntOrErrorCode){.isError=false,.as={.i64=progStringCount++}};
}
int progStringCmp(const void* a,const void* b){
  return ((const ProgramString*)b)->value.length-((const ProgramString*)a)->value.length;
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

SizeOrError compileOp(FILE* target,size_t compiledOps,const Operation* op,size_t opSize,bool isGlobal);

SizeOrError tupleElementAccess(FILE* target,int32_t depth,const Operation* op,size_t opCount,bool isPtr){
  if(depth<0||opCount<(size_t)depth)
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=op->filePos}}};
  size_t size=0;
  for(int32_t i=0;i<depth;i++){
    if((op+size)->opType!=OP_GET||(op+size)->dataAs.idInfo.type!=ID_TUPLE_ELEMENT){
      fputs("unexpected operations for tuple access: ",stderr);
      printOperation(*(op+size),stderr);
      fputs("\n",stderr);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
    }
    if(isPtr){
      fprintf(target,"->e%"PRIi32,(op+size)->dataAs.idInfo.id);
      isPtr=false;
    }else{
      fprintf(target,".e%"PRIi32,(op+size)->dataAs.idInfo.id);
    }
    size++;
  }
  return (SizeOrError){.isError=false,.as={.size=size}};
} 
void printProcedureSignatureC(ProcedureType* procedure,int32_t procId,FILE* target,bool printArgNames){
  printTypeNameC(procedure->outType,target);
  fprintf(target," procedure%" PRIi32" (",procId);
  DataType* inType=procedure->inType;
  if(inType->typeClass==TYPECLASS_FLAT_TUPLE){
    CompositeType* inTypes=inType->typeDataAs.composite;
    for(int32_t e=0;e<inTypes->typeCount;e++){
      if(e>0)
        fputs(", ",target);
      printTypeNameC(&(inTypes->types[e]),target);
      if(printArgNames)
        fprintf(target," arg%"PRIi32,e);
    } 
  }else if(isVoidType(inType)){
    fputs("void",target);
  }else{
    printTypeNameC(inType,target);
    fputs(" arg0",target);
  }         
  fputs(")",target);
}

#define COMPILE_OP_RETURN_ERROR(target, op,opSize)\
                r=compileOp(target,compiledOps+size,op+size,opSize-size,isGlobal);\
                if(r.isError)\
                  return r;\
                size+=r.as.size;\

SizeOrError compileProcArgs(FILE* target,size_t compiledOps,const Operation* op,size_t size,size_t opSize,bool isGlobal){
  SizeOrError r;
  DataType* in=op->dataType.typeDataAs.procedure->inType;
  DataType* out=op->dataType.typeDataAs.procedure->outType;
  fputs("(",target);
  if(in->typeClass==TYPECLASS_FLAT_TUPLE){
    for(int32_t e=0;e<in->typeDataAs.composite->typeCount;e++){
      if(e>0)
        fputs(",",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
    }
  }else if(!isVoidType(in)){
    COMPILE_OP_RETURN_ERROR(target,op,opSize);
  }
  fputs(")",target);
  if(isVoidType(out))//function without return value terminates statement
    fputs(";\n",target);
  return (SizeOrError){.isError=false,.as={.size=size}};
} 

SizeOrError compileOp(FILE* target,size_t compiledOps,const Operation* op,size_t opSize,bool isGlobal){
  if(opSize<1)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=op->filePos}}};
  SizeOrError r;
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
              return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
          }
          break;
        case TYPECLASS_POINTER:
        case TYPECLASS_CONST_POINTER:
          fputs("p",target);
          break;
        default:
          fputs("printing values of type ",stderr);
          printTypeName(&op->dataType,stderr);
          fputs(" is (currently) not supported\n",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
      }
      fputs("\\n\",",target);
      if(isPointerType(&op->dataType)){
        fputs("(void*)",target);
      }
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      if(boolMode){
        fputs("?\"true\":\"false\"",target);
      }
      fputs(");\n",target);
      return (SizeOrError){.isError=false,.as={.size=size}};
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
        return (SizeOrError){.isError=false,.as={.size=size}};
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
        fprintf(target,"{.e0=stringChars%"PRIi32"+%"PRIi32",.e1=%zu}",programStrings[i].charsId,programStrings[i].charsOffset,programStrings[i].value.length);
        if(needCast)
          fputs(")",target);
        return (SizeOrError){.isError=false,.as={.size=size}};
      }
      if(!isPrimitiveType(&(op->dataType))){
          fputs("constants of non-primitive type ",stderr);
          printTypeName(&op->dataType,stderr);
          fputs(" are not supported\n",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
      }
      switch(op->dataType.typeDataAs.primitive){
        case PRIMITIVE_BOOL:
        case PRIMITIVE_I8:
        case PRIMITIVE_I32:
        case PRIMITIVE_I64:
          fprintf(target,"%" PRIi64,op->dataAs.i64);
          if(needCast)
            fputs(")",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        default:
          fprintf(stderr,"%s constants are (currently) not supported",primitiveName(op->dataType.typeDataAs.primitive));
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
      }
      break;
    case OP_CHECK_ARRAY_BOUNDS:
      fprintf(target,"%s(",CHECK_BOUNDS_NAME);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//index
      fputs(",",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//length
      fputs(");\n",target);
      return (SizeOrError){.isError=false,.as={.size=size}};
    case OP_CHECK_ENUM_INDEX:
      fprintf(target,"%s(",CHECK_ENUM_INDEX_NAME);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//index
      fprintf(target,".label,%"PRIi64");\n",op->dataAs.i64);
      return (SizeOrError){.isError=false,.as={.size=size}};
    case OP_GET:
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
        case ID_INTERMEDIATE_RESULT:
          fprintf(target,"tmp%" PRIi32,op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_LOCAL_VAR:
          fprintf(target,"local%" PRIi32,op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_ARGUMENT:
          fprintf(target,"arg%" PRIi32,op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_GLOBAL_VAR:
          fprintf(target,"global%" PRIi32,op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_PROCEDURE:
          fprintf(target,"procedure%" PRIi32,op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_TUPLE:
          //1. get tuple
          fputs("(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(")",target);
          //2. tuple element access
          r=tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,false);
          if(r.isError)
            return r;
          size+=r.as.size;
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_TUPLE_ELEMENT:
          fputs("tuple access without base tuple\n",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_ENUM_LABEL:
          fputs("(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(").label",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_ENUM_ELEMENT:
          fputs("(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fprintf(target,").data.e%"PRIi32,op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_POINTER:
          if(op->dataAs.idInfo.id==0){
            fputs("(*(",target);
            COMPILE_OP_RETURN_ERROR(target,op,opSize);
            fputs("))",target);
            return (SizeOrError){.isError=false,.as={.size=size}};
          }
          //base value
          fputs("(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(")",target);
          //tuple element access
          r=tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,true);
          if(r.isError)
            return r;
          size+=r.as.size;
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_POINTER_OFFSET:
          if(op->dataAs.idInfo.id==0){
            fputs("(*((",target);
            COMPILE_OP_RETURN_ERROR(target,op,opSize);
            fputs(")+(",target);
            COMPILE_OP_RETURN_ERROR(target,op,opSize);
            fputs(")))",target);
            return (SizeOrError){.isError=false,.as={.size=size}};
          }
          //base value
          fputs("((",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(")+(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("))",target);
          //tuple element access
          r=tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,true);
          if(r.isError)
            return r;
          size+=r.as.size;
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_TYPE:
          fputs("type information is not accessible at runtime",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      break;
    case OP_SET_VALUE:
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(" = ",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(";\n",target);
      return (SizeOrError){.isError=false,.as={.size=size}};
    case OP_PRE_DECLARE:
      if(op->dataAs.idInfo.type!=ID_PROCEDURE)
        printTypeNameC(&(op->dataType),target);
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
          fprintf(target," tmp%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_LOCAL_VAR:
          fprintf(target," local%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_GLOBAL_VAR:
          fprintf(target," global%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_PROCEDURE:
          if(!isCallableType(&(op->dataType))||isPointerType(&(op->dataType)))
            return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
          printProcedureSignatureC(op->dataType.typeDataAs.procedure,op->dataAs.idInfo.id,target,false);
          fputs(";\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_INTERMEDIATE_RESULT:
        case ID_ARGUMENT:
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_TYPE:
          fprintf(stderr,"cannot pre-declare %s\n",idNames[op->dataAs.idInfo.type]);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      break;
    case OP_DECLARE:
      if(op->dataAs.idInfo.type!=ID_PROCEDURE)
        printTypeNameC(&(op->dataType),target);
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
          fprintf(target," tmp%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_INTERMEDIATE_RESULT:
          fprintf(target," const tmp%" PRIi32 " = ",op->dataAs.idInfo.id);//intermediate results are constant
          break;
        case ID_LOCAL_VAR:
          fprintf(target," local%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_GLOBAL_VAR:
          fprintf(target," global%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_PROCEDURE:
          if(!isCallableType(&(op->dataType))||isPointerType(&(op->dataType)))
            return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
          printProcedureSignatureC(op->dataType.typeDataAs.procedure,op->dataAs.idInfo.id,target,true);
          fputs("{\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case ID_ARGUMENT:
          fputs("cannot declare arguments",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
          fputs("cannot declare tuple elements",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
          fputs("cannot declare enum elements",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_POINTER:
        case ID_POINTER_OFFSET:
          fputs("cannot declare pointers",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_TYPE:
          fputs("cannot declare types",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      if(op->dataAs.idInfo.type!=ID_PROCEDURE){
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(";\n",target);
      }
      return (SizeOrError){.isError=false,.as={.size=size}};
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
        return (SizeOrError){.isError=false,.as={.size=size}};
      }
      if(op->dataType.typeClass==TYPECLASS_ENUM){
        if(needCast){
          fputs("(",target);
          printTypeNameC(&(op->dataType),target);
          fputs(")",target);
        }
        fprintf(target,"{.label=%"PRIi64,op->dataAs.i64);
        if(isVoidType(op->dataType.typeDataAs.composite->types+op->dataAs.i64)){
          fputs(",.data={0}}",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        }
        fprintf(target,",.data={.e%"PRIi64"=",op->dataAs.i64);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs("}}",target);
        return (SizeOrError){.isError=false,.as={.size=size}};
      }
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=op->filePos}}};
    case OP_CAST:
      fputs("((",target);
      printTypeNameC(&(op->dataType),target);
      fputs(")",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      return (SizeOrError){.isError=false,.as={.size=size}};
    case OP_ADDR_OF:
      fputs("&(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      return (SizeOrError){.isError=false,.as={.size=size}};
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
      return (SizeOrError){.isError=false,.as={.size=size}};
    case OP_BINARY_OPERATOR:
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      switch(op->dataAs.binOp){//XXX? use array/map instead
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
      return (SizeOrError){.isError=false,.as={.size=size}};
    case OP_CODE_BLOCK:
      switch(op->dataAs.block){
        case BLOCK_PROCEDURE:
          fputs("block procedure should be eliminated at compile time",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case BLOCK_START:
          fputs("{\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case BLOCK_IF:
        case BLOCK_IF2:
          fputs("if(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("){\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case BLOCK_WHILE:
          fputs("if(!",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(")\n  break;\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case BLOCK_DO:
          fputs("do{\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case BLOCK_ELSE:
          fputs("}else{\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case BLOCK_WHILE_END:
          fputs("}while(1);\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        case BLOCK_END:
          fputs("}\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
      }
      break;
    case OP_RETURN:
      fputs("return ",target);
      if(op->dataType.typeClass!=TYPECLASS_FLAT_TUPLE){
        if(isVoidType(&(op->dataType))){
          fputs(";\n",target);
          return (SizeOrError){.isError=false,.as={.size=size}};
        }
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(";\n",target);
        return (SizeOrError){.isError=false,.as={.size=size}};
      }
      fprintf(target,"(tuple%"PRIi32"){",op->dataType.typeDataAs.composite->id);
      for(int32_t e=0;e<op->dataType.typeDataAs.composite->typeCount;e++){
        if(e>0)
          fputs(",",target);
        fprintf(target,".e%"PRIi32"=",e);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
      }
      fputs("};\n",target);
      return (SizeOrError){.isError=false,.as={.size=size}};
    case ENTRY_POINT:
      fputs("int main(void){\n",target);
      return (SizeOrError){.isError=false,.as={.size=size}};
    case OP_CALL_PTR:
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      r=compileProcArgs(target,compiledOps,op,size,opSize,isGlobal);
      if(r.isError)
        return r;
      return (SizeOrError){.isError=false,.as={.size=r.as.size}};
    case OP_CALL:
      if(op->dataAs.idInfo.type!=ID_PROCEDURE){
        fprintf(stderr,"calling %s directly is not supported\n",idNames[op->dataAs.idInfo.type]);
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      fprintf(target,"procedure%"PRIi32,op->dataAs.idInfo.id);
      r=compileProcArgs(target,compiledOps,op,size,opSize,isGlobal);
      if(r.isError)
        return r;
      return (SizeOrError){.isError=false,.as={.size=r.as.size}};
    case OP_GET_LABEL:
    case OP_IDENTIFIER:
    case OP_IDENTIFIER_ADDRESS:
    case OP_MODIFY_STACK:
    case OP_TYPE_INFO:
      fprintf(stderr,"operation %s should not exist at this stage of compilation\n",opName(op->opType));
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
    default:
      fprintf(stderr,"operation %s is not implemented\n",opName(op->opType));
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=op->filePos}}};
  }
  return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=op->filePos}}};
}

Error compileToC(FILE* target,const Program* p){
  fputs("#include <stdlib.h>\n",target);
  fputs("#include <stdio.h>\n",target);
  fputs("#include <inttypes.h>\n",target);
  fputs("#include <string.h>\n",target);
  fputs("#include <stdbool.h>\n",target);
  fputs("//internal declarations\n",target);
  //initialize strings
  if(progStringCount>0)
    initProgStringChars();//initialize characters
  for(size_t i=0;i<procTypeCount;i++){
    if(procTypes[i].outType->typeClass==TYPECLASS_FLAT_TUPLE){//ensure flat-tuple return types are generated as tuples for code generation
      procTypes[i].outType->typeDataAs.composite->flags|=FLAG_IS_TUPLE;
    }
  }
  //declare composite types
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].flags&(FLAG_IS_TUPLE|FLAG_IS_STRUCT)){
      fprintf(target,"typedef struct tuple%"PRIi32"Impl tuple%"PRIi32";\n",i,i);
    }
    if(compositeTypes[i].flags&(FLAG_IS_ENUM)){
      fprintf(target,"typedef struct enum%"PRIi32"Impl enum%"PRIi32";\n",i,i);
    }
  }
  //declare procedure pointers
  for(size_t i=0;i<procTypeCount;i++){
    fputs("typedef ",target);
    printTypeNameC(procTypes[i].outType,target);
    fprintf(target," (*procPtr%zu) (",i);
    if(procTypes[i].inType->typeClass==TYPECLASS_FLAT_TUPLE){//auto-unwrap procedure arguments
      CompositeType* inTypes=procTypes[i].inType->typeDataAs.composite;
      for(int32_t j=0;j<inTypes->typeCount;j++){
        if(j>0)
          fputs(",",target);
        printTypeNameC(&(inTypes->types[j]),target);
      }
    }else{
      printTypeNameC(procTypes[i].inType,target);
    }
    fputs(");\n",target);
  }
  //initialize composite types
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].flags&(FLAG_IS_TUPLE|FLAG_IS_STRUCT)){
      fprintf(target,"struct tuple%"PRIi32"Impl{\n",i);
      for(int16_t e=0;e<compositeTypes[i].typeCount;e++){
        printTypeNameC(&(compositeTypes[i].types[e]),target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("};\n",target);
    }
    if(compositeTypes[i].flags&FLAG_IS_ENUM){
      fprintf(target,"struct enum%"PRIi32"Impl{\n",i);//XXX declare enum with all void members as integer
      if((compositeTypes[i].flags&FLAG_VOID_ONLY)==0){//omit output of empty union
        fputs("union{\n",target);
        for(int16_t e=0;e<compositeTypes[i].typeCount;e++){
          if(isVoidType(&(compositeTypes[i].types[e])))
            continue;//skip void types
          printTypeNameC(&(compositeTypes[i].types[e]),target);
          fprintf(target," e%"PRIi16";\n",e);
        }
        fputs("} data;\n",target);
      }
      fputs("int32_t const label;\n",target);
      fputs("};\n",target);
    }
  }
  //initialize strings
  for(size_t i=0;i<progStringCount;i++){
    if(programStrings[i].isBaseString){
      fprintf(target,"const %s stringChars%"PRIi32"[%"PRIi64"] = {",primitiveNameC(PRIMITIVE_I8),programStrings[i].charsId,programStrings[i].value.length+1);
      String str=programStrings[i].value;
      for(size_t j=0;j<str.length;j++){
        if(str.chars[j]<0)
          fprintf(target,"-0x%"PRIx8,-str.chars[j]);
        else
          fprintf(target,"0x%"PRIx8,str.chars[j]);
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
  SizeOrError r;
  fputs("//global code\n",target);
  for(size_t i=0;i<p->globalCount;){
    r=compileOp(target,i,p->globalOps+i,p->globalCount-i,true);
    if(r.isError)
      return r.as.error;
    i+=r.as.size;
  }
  fputs("//procedures code\n",target);
  for(size_t i=0;i<p->opCount;){
    r=compileOp(target,i,p->ops+i,p->opCount-i,false);
    if(r.isError)
      return r.as.error;
    i+=r.as.size;
  }
  return (Error){.errorCode=0,.pos={0}};
}


typedef struct{
  char* code;
  size_t codeSize;
  FilePosition currentPos;
  FilePosition wordStart;
}CodeFile;

typedef struct{
  Scope* currentScope;
  size_t compiledOps;
  int32_t currentProcId;
  int32_t procScope;
  int32_t scopeLevel;
  
  int32_t predeclaredTypes;
  int32_t opaqueTypeCount;
  bool hasEntryPoint;
}CompilerState;



Operation opDeclareIntermediate(DataType* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_DECLARE,.dataType=*type,.filePos=pos,.dataAs={.idInfo={.type=ID_INTERMEDIATE_RESULT,.id=tmpId}}};
}
Operation opGetIntermediate(DataType* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_GET,.dataType=*type,.filePos=pos,.dataAs={.idInfo={.type=ID_INTERMEDIATE_RESULT,.id=tmpId}}};
}
Operation opPredeclareTmpVar(DataType* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_PRE_DECLARE,.dataType=*type,.filePos=pos,.dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId}}};
}
Operation opDeclareTmpVar(DataType* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_DECLARE,.dataType=*type,.filePos=pos,.dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId}}};
}
Operation opGetTmpVar(DataType* type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_GET,.dataType=*type,.filePos=pos,.dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId}}};
}

Operation opBinaryOperator(BinaryOperator binOpType,FilePosition pos){
  return (Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={.binOp=binOpType}};
}
Operation opUnaryOperator(UnaryOperator unOpType,FilePosition pos){
  return (Operation){.opType=OP_UNARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={.unOp=unOpType}};
}
Operation opCodeBlock(BlockType blockType,FilePosition pos){
  return (Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={.block=blockType}};
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

bool wordEquals(const String* word,const char* string){
  size_t l=strlen(string);
  if(l!=word->length)
    return false;
  int c=memcmp(word->chars,string,word->length);
  return c==0;
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
  if(number.length>1&&number.chars[0]=='-'){
    i++;
    negate=true;
  }
  if(detectBase&&number.length>i+1&&number.chars[i]=='0'){
    i++;
    if(number.chars[i]=='x'||number.chars[i]=='X'){
      base=16;
      i++;
    }else if(number.chars[i]=='b'||number.chars[i]=='B'){
      base=2;
      i++;
    }
  }
  size_t i0=i;
  bool overflow=false;
  uint64_t maxSaveValue=negate?(INT64_MAX/base):-(INT64_MIN/base);
  for(;i<number.length;i++){
    if(i>i0&&i<number.length-1&&(number.chars[i]=='_'||number.chars[i]=='\''))
      continue;//ignore _ and ' if they are in the interior of the number
    if(value>maxSaveValue){
      overflow=true;//check if remaining word is integer before returning overflow error
    }
    value*=base;
    digit=toDigit(number.chars[i]);
    if(digit<0||digit>=base)
      return (IntOrErrorCode){.isError=true,.as={.error=ERROR_PARSE_INT}};
    value+=digit;
  }
  if(overflow){
    fprintf(stderr,"value %.*s does not fit in a 64-bit integer\n",(int)number.length,number.chars);
    return (IntOrErrorCode){.isError=true,.as={.error=ERROR_INT_OVERFLOW}};
  }
  return (IntOrErrorCode){.isError=false,.as={.i64=negate?-value:value}};
}
String readStringLiteral(CodeFile* codeFile,char* end,size_t endLength,bool doEspaceSeqs,int32_t* errorFlag){
  if(codeFile->codeSize<1){
    *errorFlag=ERROR_EOF;
    return (String){.chars=codeFile->code,.length=0};
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
    if(doEspaceSeqs&&*(codeFile->code)=='\\'){//escaped characters
      if(codeFile->codeSize<=1){
        *errorFlag=ERROR_EOF;
        return (String){.chars=codeFile->code,.length=0};
      }
      delta++;
      updateFilePosition(codeFile);//ignore the \ character
      switch(*(codeFile->code)){//decode escape sequence
        case 'b':
          *(codeFile->code)='\b';
          break;
        case 'n':
          *(codeFile->code)='\n';
          break;
        case 't':
          *(codeFile->code)='\t';
          break;
        case 'r':
          *(codeFile->code)='\r';
          break;
        case 'f':
          *(codeFile->code)='\f';
          break;
        case 'v':
          *(codeFile->code)='\v';
          break;
        case '\'':
        case '\\':
        case '"':
          break; //keep the original character
        case 'x':
        case 'u':
        case 'U':
          sequenceLength=*(codeFile->code)=='U'?9:*(codeFile->code)=='u'?5:3;
          if(codeFile->codeSize<sequenceLength){
            if(errorFlag)
              *errorFlag=ERROR_EOF;
            return (String){.chars=codeFile->code,.length=0};
          }
          IntOrErrorCode val=parseInt((String){.chars=(codeFile->code)+1,.length=(sequenceLength-1)},16);
          if(val.isError){
            if(errorFlag)
              *errorFlag=val.as.error;
            return (String){.chars=codeFile->code,.length=0};
          }
          int l;
          if(*(codeFile->code)=='x'){
            wordChars[wordLength]=val.as.i64;
            l=1;
          }else{
            if(errorFlag&&(val.as.i64<0||val.as.i64>MAX_CODEPOINT))
              *errorFlag=WARNING_CODEPOINT_OUT_OF_RANGE;
            l=writeUnicodeChar(val.as.i64,wordChars+wordLength);
          }
          wordLength+=l;
          delta+=sequenceLength-l;
          (codeFile->code)+=sequenceLength;
          (codeFile->codeSize)-=sequenceLength;
          codeFile->currentPos.posInLine+=sequenceLength;//no newline in unicode escape sequence
          continue;//skip to next iteration of loop
        default:
          if(errorFlag)
            *errorFlag=ERROR_UNSUPPORTED_ESCAPE_SEQUENCE;
          break; 
      }
    }
    if(delta>0){//copy chars to position in unescaped string
      wordChars[wordLength]=*(codeFile->code);
    }
    wordLength++;
    updateFilePosition(codeFile);
  }
  if(codeFile->codeSize==0){
    *errorFlag=ERROR_EOF;
    fprintf(stderr,"unfinished comment or string literal %.*s \n",(int)wordLength,codeFile->code);
    return (String){.chars=codeFile->code,.length=0};
  }
  //move code-pointer to position after word
  updateFilePosition(codeFile);
  wordChars[wordLength]=0;//zero terminate string
  return (String){.chars=wordChars,.length=wordLength};
}
//constants for the wordType flag of nextWord
//allow to determine which type of word was read
#define WORD_TYPE_IDENTIFIER 0
#define WORD_TYPE_STRING    1
#define WORD_TYPE_CHAR      2
typedef struct{
  int32_t errCode;
  int32_t wordType;
}WordTypeOrErrCode;
String nextWord(CodeFile* codeFile,WordTypeOrErrCode* wordType){
  skipWhitespaces(codeFile);
  if(codeFile->codeSize<=0){//end of file
    //don't set wordType to ERROR_EOF, file is allowed to end at this point
    return (String){.chars=codeFile->code,.length=0};
  }
  codeFile->wordStart=codeFile->currentPos;
  if(wordType)
    *wordType=(WordTypeOrErrCode){.errCode=0,.wordType=WORD_TYPE_IDENTIFIER};
  if(*(codeFile->code)=='"'){
    if(wordType)
      wordType->wordType=WORD_TYPE_STRING;
    return readStringLiteral(codeFile,"\"",1,true,&(wordType->errCode));
  }else if(*(codeFile->code)=='\''){
    if(wordType)
      wordType->wordType=WORD_TYPE_CHAR;
    return readStringLiteral(codeFile,"'",1,true,&(wordType->errCode));
  }else if(codeFile->codeSize>=2&&*(codeFile->code)=='#'){
    if(*(codeFile->code+1)=='#'){//line comment
      readStringLiteral(codeFile,"\n",1,false,&(wordType->errCode));//ignore everything up to next new-line
      return (String){.chars=codeFile->code,.length=0};
    }else if(*(codeFile->code+1)=='+'){//inline comment
      readStringLiteral(codeFile,"+#",2,false,&(wordType->errCode));
      return (String){.chars=codeFile->code,.length=0};
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
  return (String){.chars=wordChars,.length=wordLength};
}

#define LABEL_TYPE_NONE    0 // no labels
#define LABEL_TYPE_STRUCT  1 // exactly one label per type
#define LABEL_TYPE_ENUM    2 // labels without type are allowed
 
int readType(String name,CodeFile* codeFile,CompilerState* state);
//reads a composite type of the given type-class, the result is stored in the type buffer
//return 0 if a type was read, otherwise a nonzero error-code if a type error occurs this method will return a syntax error
int readCompositeType(TypeClass typeClass,CodeFile* codeFile,CompilerState* state,int labelType,const char* endString,bool checkEmpty){
  String word;
  WordTypeOrErrCode wordType;
  int err;
  size_t initOffset=bufferedTypes;
  size_t labelOffset=bufferedFieldNames;
  size_t currentOffset=initOffset;
  int typesSinceLabel=0;//if there has been a type since the last label
  do{
    word=nextWord(codeFile,&wordType);
    if(wordEquals(&word,endString))
      break;
    if(wordType.errCode!=0)
      return wordType.errCode;
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER){
      fputs("type names have to be identifiers\n",stderr);
      return ERROR_SYNTAX;
    }
    if(labelType!=LABEL_TYPE_NONE&&typesSinceLabel>0&&wordEquals(&word,":")){//start label
      if(typesSinceLabel>1){
        fprintf(stderr,"too many types for field declaration expected 1 got %i\n",typesSinceLabel);
        return ERROR_SYNTAX;
      }
      typesSinceLabel=0;
      String label=nextWord(codeFile,&wordType);
      if(wordType.errCode!=0)
        return wordType.errCode;
      if(wordType.wordType!=WORD_TYPE_IDENTIFIER){
        fputs("label names have to be identifiers\n",stderr);
        return ERROR_SYNTAX;
      }
      fieldNameBuffer[bufferedFieldNames++]=label;
      continue;
    }
    err=readType(word,codeFile,state);
    if(err==0){
      typesSinceLabel+=(bufferedTypes-currentOffset);
      currentOffset=bufferedTypes;
      continue;
    }
    if(err!=ERROR_TYPE)
      return err;
    if(labelType!=LABEL_TYPE_ENUM||typesSinceLabel>0){
      fprintf(stderr,"unknown type name '%.*s' \n",(int)word.length,word.chars);
      return ERROR_SYNTAX;
    }
    //untyped enum label
    fieldNameBuffer[bufferedFieldNames++]=word;
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_VOID);
    currentOffset=bufferedTypes;
    typesSinceLabel=0;
  }while(1);
  if(checkEmpty&&bufferedTypes==initOffset){
    fputs("empty composite type\n",stderr);
    return ERROR_SYNTAX;
  }
  if(labelType!=LABEL_TYPE_NONE){
    if(typesSinceLabel>0){
      fprintf(stderr,"missing label in %s\n",typeClassName(typeClass));
      return ERROR_SYNTAX;
    }
    typeBuffer[initOffset]=compositeType(typeClass,typeBuffer+initOffset,fieldNameBuffer+labelOffset,bufferedTypes-initOffset);
  }else{
    typeBuffer[initOffset]=compositeType(typeClass,typeBuffer+initOffset,NULL,bufferedTypes-initOffset);
  }
  if(typeEquals(&(typeBuffer[initOffset]),&TYPE_UNDEFINED))
    return ERROR_MEMORY;
  if(checkEmpty&&bufferedTypes-initOffset==1){
    fputs("WARNING:\n  single element composite type: ",stderr);
    printTypeName(&(typeBuffer[initOffset]),stderr);
    fputs(" at ",stderr);
    printFilePosition(codeFile->wordStart,stderr);
    fputs("\n",stderr);
  }
  bufferedTypes=initOffset+1;
  return 0;
}
//reads a type starting with the identifier name, the result is stored in the type buffer
//return 0 if a type was read, ERROR_TYPE if name was not a type and the corresponding error code if another error occurs
int readType(String name,CodeFile* codeFile,CompilerState* state){
  if(name.length==0)
    return ERROR_TYPE;
  if(bufferedTypes>=TYPE_BUFFER_CAP){//buffer overflow
    return ERROR_MEMORY;
  }
  //primitive types
  if(wordEquals(&name,"void")){
    fputs("using the void type directly is not supported\n",stderr);
    return ERROR_SYNTAX;
  }
  if(wordEquals(&name,"bool")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_BOOL);
    return 0;
  }
  if(wordEquals(&name,"i8")||wordEquals(&name,"char")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_I8);
    return 0;
  }
  if(wordEquals(&name,"i32")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_I32);
    return 0;
  }
  if(wordEquals(&name,"i64")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_I64);
    return 0;
  }
  if(wordEquals(&name,"float")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_FLOAT);
    return 0;
  }
  if(wordEquals(&name,"string")){
    typeBuffer[bufferedTypes++]=progStringType();
    return 0;
  }
  //composite types
  size_t initOffset=bufferedTypes;
  int r;
  if(wordEquals(&name,"ptr")){
    if(bufferedTypes==0){
      fputs("pointer type is missing its argument",stderr);
      return ERROR_SYNTAX;
    }
    typeBuffer[bufferedTypes-1]=pointerType(&(typeBuffer[bufferedTypes-1]));
    return 0;
  }
  if(wordEquals(&name,"proc(")){
    r=readCompositeType(TYPECLASS_FLAT_TUPLE,codeFile,state,LABEL_TYPE_NONE,"=>",false);
    if(r!=0)
      return r;
    r=readCompositeType(TYPECLASS_FLAT_TUPLE,codeFile,state,LABEL_TYPE_NONE,")",false);
    if(r!=0)
      return r;
    typeBuffer[initOffset]=procedureType(&(typeBuffer[initOffset]),&(typeBuffer[initOffset+1]));
    bufferedTypes--;
    return 0;
  }
  if(wordEquals(&name,"tuple(")||wordEquals(&name,"(")){
    return readCompositeType(TYPECLASS_TUPLE,codeFile,state,LABEL_TYPE_NONE,")",true);
  }
  if(wordEquals(&name,"struct(")){
    return readCompositeType(TYPECLASS_STRUCT,codeFile,state,LABEL_TYPE_STRUCT,")",true);
  }
  if(wordEquals(&name,"enum(")){
    return readCompositeType(TYPECLASS_ENUM,codeFile,state,LABEL_TYPE_ENUM,")",true);
  }
  ScopeNode* asIdentifier;
  r=getIdentifier(name,&asIdentifier);
  if(r<0)//internal error while reading identifier
    return r;
  if(r>0||asIdentifier->idType!=ID_TYPE)//identifier does not exist / is not a type
    return ERROR_TYPE;
  //identifier
  if(typeEquals(&(asIdentifier->type),&TYPE_UNDEFINED))
    return ERROR_TYPE;
  typeBuffer[bufferedTypes++]=asIdentifier->type;
  if(asIdentifier->type.typeClass==TYPECLASS_OPAQUE){//ensure token after opaque type is ptr
    WordTypeOrErrCode wordType;
    String word=nextWord(codeFile,&wordType);
    if(wordType.errCode!=0)
      return wordType.errCode;
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER){
      fputs("expected an identifier\n",stderr);
      return ERROR_SYNTAX;
    }
    if(!wordEquals(&word,"ptr")){
      fprintf(stderr,"unexpected word after opaque type '%.*s' expected 'ptr' got '%.*s'\n",(int)name.length,name.chars,(int)word.length,word.chars);
      return ERROR_SYNTAX;
    }
    typeBuffer[bufferedTypes-1]=pointerType(&(typeBuffer[bufferedTypes-1]));
  }
  return 0;
}

int requireCompileTimeType(String* opName,DataType* typeOut,size_t nTypes){
  if(bufferedTypes!=nTypes){
    fprintf(stderr,"wrong number of type arguments for operation '%.*s' expected %zu got %zu\n",(int)opName->length,opName->chars,nTypes,bufferedTypes);
    return ERROR_SYNTAX;
  }
  for(size_t i=0;i<nTypes;i++){
    *(typeOut)=typeBuffer[--bufferedTypes];
    if(typeEquals(typeOut,&TYPE_UNDEFINED))
      return ERROR_TYPE;
    typeOut++;
  }
  return 0;
}

SizeOrError readOperation(Operation* op,CodeFile* codeFile,CompilerState* state){
  int err=0;
  WordTypeOrErrCode wordType;
  String word=nextWord(codeFile,&wordType);
  DataType type;
  FilePosition wordPos=codeFile->wordStart;
  if(wordType.errCode!=0)
    return (SizeOrError){.isError=true,.as={.error={.errorCode=wordType.errCode,.pos=wordPos}}};
  if(wordType.wordType==WORD_TYPE_STRING){
    IntOrErrorCode strId=addProgString(word);
    if(strId.isError)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=strId.as.error,.pos=wordPos}}};
    (*op)=opConstant(progStringType(),strId.as.i64,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(wordType.wordType==WORD_TYPE_CHAR){
    if(word.length!=1){//TODO? handle Unicode characters
      fprintf(stderr,"character literal '%.*s' contains more that one character\n",(int)word.length,word.chars);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    }
    (*op)=opConstant(primitiveType(PRIMITIVE_I8),word.chars[0],wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  IntOrErrorCode asInt=parseInt(word,0);//try to parse word as int
  if(!asInt.isError){
    (*op)=opConstant(primitiveType((asInt.as.i64<=INT32_MAX&&asInt.as.i64>=INT32_MIN)?PRIMITIVE_I32:PRIMITIVE_I64),asInt.as.i64,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(asInt.as.error!=ERROR_PARSE_INT)
    return (SizeOrError){.isError=true,.as={.error={.errorCode=asInt.as.error,.pos=wordPos}}};
  if(word.length==0)
    return (SizeOrError){.isError=false,.as={.size=0}};
  err=readType(word,codeFile,state);//try to parse word as type
  wordPos=codeFile->wordStart;
  if(err==0)//is type
    return (SizeOrError){.isError=false,.as={.size=0}};
  if(err!=ERROR_TYPE)//unexpected error while reading type
    return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
  //1. operations that take a Type as argument
  if(wordEquals(&word,":")){//pre-declare
    err=requireCompileTimeType(&word,&type,1);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
    String varName=nextWord(codeFile,&wordType);
    wordPos=codeFile->wordStart;
    if(wordType.errCode!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=wordType.errCode,.pos=wordPos}}};
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    IdentifierType idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    if(isCallableType(&type)&&!isPointerType(&type)){
      fputs("directly predeclaring procedures is not supported",stderr);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    }
    if(type.typeClass==TYPECLASS_TYPE_OF){
      if(!typeEquals(type.typeDataAs.type,&TYPE_UNDEFINED)){
        fputs("cannot pre-declare values of type: ",stderr);
        printTypeName(&type,stderr);
        fputs("\n For pre-declaring a type use 'type' without any prefix",stderr);
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
      }
      type=opaqueType(state->opaqueTypeCount++);
      idType=ID_TYPE;
    }
    ScopeNode* id;
    int r=declareIdentifier(varName,type,idType,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
    if(idType==ID_TYPE)//declaring type does not produce any code
      return (SizeOrError){.isError=false,.as={.size=0}};
    (*op)=(Operation){.opType=OP_PRE_DECLARE,.dataType=type,.filePos=wordPos,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"=:")){//declare
    err=requireCompileTimeType(&word,&type,1);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
    String varName=nextWord(codeFile,&wordType);
    wordPos=codeFile->wordStart;
    if(wordType.errCode!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=wordType.errCode,.pos=wordPos}}};
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
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
          fputs("missing type for type definition",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
        }
        idType=ID_TYPE;
        r=getIdentifier(varName,&id);
        if(r<0)
          return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
        if(r!=0||id->idType!=ID_TYPE||id->type.typeClass!=TYPECLASS_OPAQUE)
          break;//can only override opaque types
        *pointerType(&id->type).typeDataAs.type=type;//override entry in wrapped type list XXX? better method
        id->type=type;//override previous definition
        return (SizeOrError){.isError=false,.as={.size=0}};
      default:
        idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;;
    }
    r=declareIdentifier(varName,type,idType,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
    if(idType==ID_TYPE){
      //declaring type does not produce any code
      return (SizeOrError){.isError=false,.as={.size=0}};
    }else if(idType==ID_PROCEDURE){
      if(state->scopeLevel>0){
        fprintf(stderr,"invalid position for procedure %.*s procedures can only be declared at top level\n",(int)varName.length,varName.chars);
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
      }
      Scope* newScope=openScope(BLOCK_PROCEDURE);
      if(newScope==NULL)
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
      state->currentScope=newScope;
      state->scopeLevel++;
      state->procScope=state->scopeLevel;
      state->currentProcId=type.typeDataAs.procedure->id;
    }
    (*op)=(Operation){.opType=OP_DECLARE,.dataType=type,.filePos=wordPos,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"new")){
    if(state->compiledOps>0&&(op-1)->opType==OP_CONSTANT&&(op-1)->dataType.typeClass==TYPECLASS_ENUM_LABEL){
      //change enum label to enum declaration
      (*(op-1)).opType=OP_NEW;
      (*(op-1)).filePos=wordPos;
      (*(op-1)).dataType.typeClass=TYPECLASS_ENUM;//change type-class back to enum
      return (SizeOrError){.isError=false,.as={.size=0}};
    }
    err=requireCompileTimeType(&word,&type,1);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
    if(type.typeClass==TYPECLASS_TUPLE||type.typeClass==TYPECLASS_STRUCT){
      (*op)=(Operation){.opType=OP_NEW,.dataType=type,.filePos=wordPos,.dataAs={.i64=0}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }
    printTypeName(&type,stderr);
    fputs(" is currently not supported for operator new\n",stderr);
    if(type.typeClass==TYPECLASS_ENUM)
      fputs(" to create an enum specify the label of the current value\n",stderr);
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=wordPos}}};
  }else if(wordEquals(&word,"cast")){ 
    err=requireCompileTimeType(&word,&type,1);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
    (*op)=(Operation){.opType=OP_CAST,.dataType=type,.filePos=wordPos,.dataAs={.i64=0}};
      return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"type")){
    if(bufferedTypes==0){//type without arguments
      typeBuffer[bufferedTypes++]=typeOfType(&TYPE_UNDEFINED);
      return (SizeOrError){.isError=false,.as={.size=0}};//type does not generate any operations
    }
    err=requireCompileTimeType(&word,&type,1);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
    typeBuffer[bufferedTypes++]=typeOfType(&type);
    return (SizeOrError){.isError=false,.as={.size=0}};//type does not generate any operations
  }else if(word.length>1&&word.chars[0]=='.'){
    word.chars++;//remove first character
    word.length--;
    if(bufferedTypes==0){
      IntOrErrorCode index=parseInt(word,10);
      if(!index.isError){
        (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
        return (SizeOrError){.isError=false,.as={.size=1}};
      }
      (*op)=(Operation){.opType=OP_GET_LABEL,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.string=word}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }
    err=requireCompileTimeType(&word,&type,1);//try to get type field of enum
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
    int64_t index;
    if(type.typeClass!=TYPECLASS_ENUM||(index=indexOfStringArray(type.typeDataAs.composite->labels,type.typeDataAs.composite->typeCount,&word,1))==-1){
      fputs("type ",stderr);
      printTypeName(&type,stderr);
      fprintf(stderr," does not have a field '%.*s'\n",(int)word.length,word.chars);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=wordPos}}};
    }
    (*op)=opConstant(type,index,wordPos);
    op->dataType.typeClass=TYPECLASS_ENUM_LABEL;//change type-class to enum-label
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(word.length>1&&word.chars[0]=='#'){//compiler command
    word.chars++;//remove first character
    word.length--;
    //stack manipulation
    if(wordEquals(&word,"dup")){//XXX dup:N drop:N -> dup/drop multiple values at once
      (*op)=(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_DUP}}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }else if(wordEquals(&word,"drop")){
      (*op)=(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_DROP}}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }else if(wordEquals(&word,"swap")){//XXX rot:N:K -> stack rotation
      (*op)=(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_SWAP}}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }
    //compiler commands
    if(wordEquals(&word,"types")){//XXX types:N -> limit number of printed types
      (*op)=(Operation){.opType=OP_TYPE_INFO,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.typeInfo={.infoType=TYPEINFO_TYPES,.maxCount=-1}}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }else if(wordEquals(&word,"stack")){
      (*op)=(Operation){.opType=OP_TYPE_INFO,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.typeInfo={.infoType=TYPEINFO_STACK,.maxCount=-1}}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }
    //XXX more compile time operations
    fprintf(stderr,"unknown compile time operation '%.*s'\n",(int)word.length,word.chars);
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
  }
  if(bufferedTypes>0){
    fprintf(stderr,"%.*s does not take a type as argument\n",(int)word.length,word.chars);
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=wordPos}}};
  }
  if(wordEquals(&word,"true")){
    (*op)=opConstant(primitiveType(PRIMITIVE_BOOL),1,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"false")){
    (*op)=opConstant(primitiveType(PRIMITIVE_BOOL),0,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"+")){
    (*op)=opBinaryOperator(ADD,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"-")){
    (*op)=opBinaryOperator(SUBTRACT,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"*")){
    (*op)=opBinaryOperator(MULTIPLY,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"/")){
    (*op)=opBinaryOperator(DIVIDE,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"%")){
    (*op)=opBinaryOperator(MOD,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"&")){
    (*op)=opBinaryOperator(AND,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"|")){
    (*op)=opBinaryOperator(OR,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"^")){
    (*op)=opBinaryOperator(XOR,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"&&")){//XXX implement short-circuit  and/or using code-blocks
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"||")){
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"==")){
    (*op)=opBinaryOperator(EQ,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"!=")){
    (*op)=opBinaryOperator(NE,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,">")){
    (*op)=opBinaryOperator(GT,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,">=")){
    (*op)=opBinaryOperator(GE,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"<=")){
    (*op)=opBinaryOperator(LE,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"<")){
    (*op)=opBinaryOperator(LT,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"neg")||wordEquals(&word,"negate")){
    (*op)=opUnaryOperator(NEGATE,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"++")){
    (*op)=opUnaryOperator(INCREMENT,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"--")){
    (*op)=opUnaryOperator(DECREMENT,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"=::")){//automatically choose type of declared variable
    String varName=nextWord(codeFile,&wordType);
    wordPos=codeFile->wordStart;
    if(wordType.errCode!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=wordType.errCode,.pos=wordPos}}};
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    IdentifierType idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    DataType mType=TYPE_UNDEFINED;
    mType.typeDataAs.typeId=++state->predeclaredTypes;//store predeceased id in type
    ScopeNode* id;
    int r=declareIdentifier(varName,mType,idType,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
    (*op)=(Operation){.opType=OP_DECLARE,.dataType=mType,.filePos=wordPos,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"=")){
    (*op)=(Operation){.opType=OP_SET_VALUE,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"@")){
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_POINTER,.id=0}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"[]")){
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_POINTER_OFFSET,.id=0}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"addrOf")){
    if(state->compiledOps>0&&(op-1)->opType==OP_CALL)
      (op-1)->opType=OP_GET;
    if(state->compiledOps>0&&(op-1)->opType==OP_IDENTIFIER)
      (op-1)->opType=OP_IDENTIFIER_ADDRESS;
    (*op)=(Operation){.opType=OP_ADDR_OF,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"()")){
    (*op)=(Operation){.opType=OP_CALL_PTR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"if")){
    Scope* newScope=openScope(BLOCK_IF);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    state->scopeLevel++;
    (*op)=opCodeBlock(BLOCK_IF,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"_if")){
    //no scope change for _if
    (*op)=opCodeBlock(BLOCK_IF2,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"while")){
    Scope* newScope=openScope(BLOCK_WHILE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    (*op)=opCodeBlock(BLOCK_WHILE,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"do")){//!!while syntax is different fro C:  WHILE cond DO exrp END   do-While: WHILE exrp cond DO END
    closeScope();
    Scope* newScope=openScope(BLOCK_WHILE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    //scope count does not change
        
    (*op)=opCodeBlock(BLOCK_DO,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"else")){
    closeScope();
    Scope* newScope=openScope(BLOCK_ELSE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    //scope count does not change
    
    (*op)=opCodeBlock(BLOCK_ELSE,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"break")){
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"continue")){
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"switch")){
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"case")){
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"default")){
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"end")){
    closeScope();
    state->scopeLevel--;
    if(state->scopeLevel<state->procScope){//exited procedure
      state->currentProcId=-1;
      state->procScope=-1;
    }
    
    (*op)=opCodeBlock(BLOCK_END,wordPos);
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"return")){
    if(state->currentProcId<0){
      fputs("unexpected return statement\n",stderr);
    }
    (*op)=(Operation){.opType=OP_RETURN,.dataType=*procTypes[state->currentProcId].outType,.filePos=wordPos,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"entryPoint:")){
    if(state->hasEntryPoint){
      fputs("program can only have one entry point",stderr);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    }
    Scope* newScope=openScope(BLOCK_PROCEDURE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    state->hasEntryPoint=true;
    (*op)=(Operation){.opType=ENTRY_POINT,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"print")){
    (*op)=(Operation){.opType=OP_PRINT,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}};//printed type will be determined by type-checker
    return (SizeOrError){.isError=false,.as={.size=1}};
  } 
  
  ScopeNode* asIdentifier;
  int r=getIdentifier(word,&asIdentifier);//try to parse variable as identifier
  if(r<0)//internal error while reading identifier
    return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
  if(r==0){//identifier
    if(asIdentifier->idType!=ID_PROCEDURE)
      asIdentifier->type=asWritableType(asIdentifier->type,true);
    else 
      asIdentifier->type=asAddressableType(asIdentifier->type);
    (*op)=(Operation){.opType=asIdentifier->idType==ID_PROCEDURE?OP_CALL:OP_GET,
      .dataType=asIdentifier->type,.filePos=wordPos,.dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  // could not find identifier, try again in type-check phase
  (*op)=(Operation){.opType=OP_IDENTIFIER,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.string=word}};
  return (SizeOrError){.isError=false,.as={.size=1}};
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

Program compileToOps(CodeFile* codeFile){
  size_t opsCap=256;
  SizeOrError r;
  Operation* compileOps=malloc(opsCap*sizeof(Operation));
  if(compileOps==NULL){
    printError((Error){.errorCode=ERROR_MEMORY,.pos=codeFile->currentPos},stderr);
    exit(ERROR_MEMORY);
  }
  openScope(BLOCK_START);
  CompilerState state=(CompilerState){.currentProcId=-1,.procScope=0,.currentScope=scopeBuffer,.scopeLevel=0,.hasEntryPoint=false,.predeclaredTypes=0,.compiledOps=0};
  while(codeFile->codeSize>0){
    r=readOperation(compileOps+state.compiledOps,codeFile,&state);
    if(r.isError){
      printError(r.as.error,stderr);
      exit(r.as.error.errorCode);
    }
    state.compiledOps+=r.as.size;
    if(ensureOpCap(&compileOps,&opsCap,state.compiledOps+16)){
      printError((Error){.errorCode=ERROR_MEMORY,.pos=codeFile->currentPos},stderr);
      exit(ERROR_MEMORY);
    }
  }
  return (Program){.ops=compileOps,.opCount=state.compiledOps,.globalOps=NULL,.globalScope=scopeBuffer,.hasEntryPoint=state.hasEntryPoint,.predeclaredTypes=state.predeclaredTypes};
}

void typeErrorMessage(const char* exprName,DataType expected,DataType got){
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
  if(subtract&&typeEquals(inTypes+0,inTypes+1)){//XXX? ptr - const ptr
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

#define INIT_CAP 128
typedef struct{
  DataType type;
  int32_t opCount;
}TypeInfo;

typedef struct{
  TypeInfo* types;
  size_t typeCount;
  Operation* ops;
  size_t opCount;

}StackState;
typedef struct{
  size_t elsePos;
  int32_t elifCount;
  
  StackState inStack;
  StackState outStack;
}IfBlockInfo;
typedef struct{
  bool hasDo;
  
  StackState inStack;
  StackState outStack;
}WhileBlockInfo;
typedef struct{
  BlockType type;
  size_t blockStart;
  union{
    int64_t i64;
    IfBlockInfo ifBlock;
    WhileBlockInfo whileBlock;
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
  
  int32_t tmpCount;
  
  size_t index;
  int32_t nPredeclaredTypes;
  DataType* predeclaredTypes;
  bool hasCheckBounds;
  bool hasCheckEnum;
}TypeCheckState;

//prints the type stack (for debug purposes)
void printTypeStack(TypeCheckState* state,bool printOps,FILE* out){
  size_t offset=0;
  for(int64_t k=state->typeCount-1;k>=0;k--){
    printTypeName(&(state->typeStack[k].type),out);
    if(!printOps){
      fputs("\n",out);
      continue;
    }
    fprintf(out," %"PRIi32":\n",state->typeStack[k].opCount);
    for(int32_t i=0;i<state->typeStack[k].opCount;i++){
      fputs("    ",out);//indent operations
      printOperation(state->opStack[offset++],out);
    }
  }
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
BlockInfo peekBlock(TypeCheckState* state){
  if(state->blockCount>0)
    return state->openBlocks[state->blockCount-1];
  return (BlockInfo){.type=BLOCK_END,.blockStart=0,.blockDataAs={0}};
}
BlockInfo popBlock(TypeCheckState* state){
  if(state->blockCount>0)
    return state->openBlocks[--state->blockCount];
  return (BlockInfo){.type=BLOCK_END,.blockStart=0,.blockDataAs={0}};
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

bool checkNonemptyStack(TypeCheckState* state,const char* message){
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
      fputs("exceeded global operation capacity",stderr);
      exit(ERROR_MEMORY);
    }
    state->globalOperations[state->globalCount++]=op;
    return;
  }
  if(ensureCompiledOpCap(state,state->opCount+1)){
    fputs("exceeded operation capacity",stderr);
    exit(ERROR_MEMORY);
  }
  state->compiledOperations[state->opCount++]=op;
}
void pushCompiledOperations(TypeCheckState* state,Operation* ops,size_t count){
  if(state->blockCount==0){
    if(ensureGlobalOpCap(state,state->globalCount+count)){
      fputs("exceeded global operation capacity",stderr);
      exit(ERROR_MEMORY);
    }
    memcpy(state->globalOperations+state->globalCount,ops,count*sizeof(Operation));
    state->globalCount+=count;
    return;
  }
  if(ensureCompiledOpCap(state,state->opCount+count)){
    fputs("exceeded operation capacity",stderr);
    exit(ERROR_MEMORY);
  }
  memcpy(state->compiledOperations+state->opCount,ops,count*sizeof(Operation));
  state->opCount+=count;
}

//append the first stackOps operations from the stack to the program, remove types elements from the type-stack
//if appendOp is true op will be appended to the program (before any stack operations are appended)
//already allocate space for skippedStackOps 
Error addCompiledStackOps(TypeCheckState* state,Operation op,size_t stackOps,size_t types,bool appendOp){
  state->opStackCount-=stackOps;
  state->typeCount-=types;
  if(appendOp)
    pushCompiledOperation(state,op);
  pushCompiledOperations(state,state->opStack+state->opStackCount,stackOps);
  return (Error){.errorCode=0,.pos=op.filePos};
}
//append op and the first types operations from the stack to the program
Error addCompiledOp(TypeCheckState* state,Operation op,size_t types){
  return addCompiledStackOps(state,op,types,types,true);
}

Error compileCompositeOp(TypeCheckState* state,DataType* type,Operation* ops,size_t nOps,int32_t tmpId){
  if(nOps==0)
    return (Error){.errorCode=0,.pos={0}};
  pushCompiledOperation(state,opDeclareIntermediate(type,tmpId,ops[0].filePos));
  pushCompiledOperations(state,ops,nOps);
  return (Error){.errorCode=0,.pos=ops[0].filePos};
}
//ensures that none of the top type-count stack elements is a composite operation 
Error extractCompositeOpsOffset(TypeCheckState* state,size_t nStackValues,size_t skipValues){
  if(state->blockCount==0)//don't extract composites at global level
    return (Error){.errorCode=0,.pos={0}};
  Error r;
  size_t offset=state->opStackCount,skipedOps=0;
  for(size_t i=1;i<=nStackValues+skipValues;i++){
    offset-=state->typeStack[state->typeCount-i].opCount;
  }
  for(size_t i=1;i<=skipValues;i++){
    skipedOps+=state->typeStack[state->typeCount-i].opCount;
  }
  size_t newOffset=offset;
  for(size_t i=state->typeCount-nStackValues-skipValues;i<state->typeCount-skipValues;i++){
    //extract multi-element operations and array constants to tmp variable
    if(state->typeStack[i].opCount>1||(state->opStack[offset].opType==OP_CONSTANT&&isArrayType(&(state->opStack[offset].dataType)))){
      int32_t tmpId=state->tmpCount++;
      r=compileCompositeOp(state,&(state->typeStack[i].type),state->opStack+offset,state->typeStack[i].opCount,tmpId);
      if(r.errorCode!=0)
        return r;
      state->opStack[newOffset++]=opGetIntermediate(&(state->typeStack[i].type),tmpId,state->opStack[offset].filePos);
      offset+=state->typeStack[i].opCount;
      state->typeStack[i].opCount=1;
      continue;
    }
    if(newOffset!=offset)
      state->opStack[newOffset]=state->opStack[offset];
    offset++;
    newOffset++;
  }
  if(skipValues>0){
    memmove(state->opStack+newOffset,state->opStack+offset,skipedOps*sizeof(Operation));
    newOffset+=skipedOps;
  }
  state->opStackCount=newOffset;
  return (Error){.errorCode=0,.pos=state->opStack[newOffset-1].filePos};
}
Error extractCompositeOps(TypeCheckState* state,size_t nStackValues){
  return extractCompositeOpsOffset(state,nStackValues,0);
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
bool declareBlockVariables(TypeCheckState* state,size_t blockStart,StackState* typeSource,StackState* valueSource){
  size_t count=typeSource->typeCount+valueSource->opCount;
  if(ensureCompiledOpCap(state,state->opCount+count))
     return true;
  memmove(state->compiledOperations+blockStart+count,state->compiledOperations+blockStart,(state->opCount-blockStart)*sizeof(Operation));
  size_t opOffset=blockStart,inTypesOffset=0;
  for(size_t i=0;i<typeSource->typeCount;i++){
    state->compiledOperations[opOffset]=typeSource->ops[i];
    state->compiledOperations[opOffset].opType=OP_DECLARE;
    opOffset++;
    memcpy(state->compiledOperations+opOffset,valueSource->ops+inTypesOffset,(valueSource->types[i].opCount)*sizeof(Operation));
    inTypesOffset+=valueSource->types[i].opCount;
  }
  state->opCount+=count;
  return false;
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
Error storeStackValues(TypeCheckState* state,StackState* stackState,StackState* expectedState,const char* errorMessage,bool initStackState,bool declare,bool ignoreFirst,FilePosition pos){
  size_t typeCount=state->typeCount-(ignoreFirst?1:0);
  if(initStackState){
    stackState->typeCount=typeCount;
    stackState->opCount=typeCount;
    stackState->types=typeCount==0?NULL:malloc((typeCount)*sizeof(TypeInfo));
    stackState->ops=typeCount==0?NULL:malloc((typeCount)*sizeof(Operation));
  }
  if(typeCount>0&&(stackState->types==NULL||stackState->ops==NULL))
      return (Error){.errorCode=ERROR_MEMORY,.pos=pos};
  if(!initStackState){
    if(typeCount!=expectedState->typeCount){
      fprintf(stderr,"wrong number of types at end of %s expected %zu got %zu\n",errorMessage,expectedState->typeCount,typeCount);
      return (Error){.errorCode=ERROR_TYPE,.pos=pos};
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
      return (Error){.errorCode=ERROR_TYPE,.pos=pos};
    }
    varId=initStackState?state->tmpCount++:stackState->ops[i].dataAs.idInfo.id;
    //save stack-elements to tmp-values
    if(declare){
      pushCompiledOperation(state,opDeclareTmpVar(&(state->typeStack[i].type),varId,pos));
    }else{
      pushCompiledOperation(state,(Operation){.opType=OP_SET_VALUE,.dataType=state->typeStack[i].type,.filePos=pos,.dataAs={0}});
      pushCompiledOperation(state,opGetTmpVar(&(state->typeStack[i].type),varId,pos));
    }
    pushCompiledOperations(state,state->opStack+offset-state->typeStack[i].opCount,state->typeStack[i].opCount);
    offset-=state->typeStack[i].opCount;
    if(!ignoreFirst){//remove element from stack if ignoreFirst false
      state->opStackCount-=state->typeStack[i].opCount;
      state->typeCount--;
    }
    if(initStackState){
      stackState->types[i]=(TypeInfo){.opCount=1,.type=asConstType(state->typeStack[i].type)};
      stackState->ops[i]=opGetTmpVar(&(state->typeStack[i].type),varId,pos);
    }
  }
  return (Error){.errorCode=0,.pos=pos};
}
Error checkIfTypes(TypeCheckState* state,IfBlockInfo* ifBlock,FilePosition pos){
  return storeStackValues(state,&(ifBlock->outStack),(ifBlock->elsePos!=0)? &(ifBlock->outStack) : &(ifBlock->inStack),"if-branch",ifBlock->elsePos==0,false,false,pos);
}
Error initWhileTypes(TypeCheckState* state,WhileBlockInfo* whileBlock,FilePosition pos){
  return storeStackValues(state,&(whileBlock->inStack),NULL,"while-loop",true,true,false,pos);
}
Error initWhileOutTypes(TypeCheckState* state,WhileBlockInfo* whileBlock,FilePosition pos){
  return storeStackValues(state,&(whileBlock->outStack),NULL,"while-loop",true,false,true,pos);
}
Error checkWhileTypes(TypeCheckState* state,WhileBlockInfo* whileBlock,FilePosition pos){
  return storeStackValues(state,&(whileBlock->inStack),&(whileBlock->inStack),"while-loop",false,false,false,pos);
}


bool canAssign(const DataType* src,const DataType* target){
  if(typeEquals(src,target))
    return true;
  if(src->typeClass==TYPECLASS_ENUM&&target->typeClass==TYPECLASS_ENUM_LABEL&&src->typeDataAs.composite->id==target->typeDataAs.composite->id)
    return true;//allow auto-cast from enum to enum-label
  if(!isPrimitiveType(src)||!isPrimitiveType(target))//XXX? assigning pointer to const pointer
    return false;
  return isInteger(src->typeDataAs.primitive)&&isInteger(target->typeDataAs.primitive)&&
    numberRank(src->typeDataAs.primitive)<=numberRank(target->typeDataAs.primitive);//implicit casts only from small int to large int
}
bool canCast(const DataType* src,const DataType* target){
  if(typeEquals(src,target))
    return true;
  if(!isPrimitiveType(src)||!isPrimitiveType(target))//XXX? cast between pointers
    return false;
  return numberRank(src->typeDataAs.primitive)>-1&&numberRank(target->typeDataAs.primitive)>-1;//casts only between numbers
}

int requireTypes(const char* opName,TypeCheckState* state,DataType* types,size_t nTypes,FilePosition pos){
  if(state->typeCount<nTypes){
    fprintf(stderr,"not enough types for %s need %zu have %zu\n",opName,nTypes,state->typeCount);
    return ERROR_TYPE;
  }
  int32_t nCasts=0;
  size_t offset=state->opStackCount;
  //check types
  for(size_t k=1;k<=nTypes;k++){
    offset-=state->typeStack[state->typeCount-k].opCount;
    if(typeEquals(&(types[nTypes-k]),&(state->typeStack[state->typeCount-k].type)))
      continue;
    if(canAssign(&(state->typeStack[state->typeCount-k].type),&(types[nTypes-k]))){
      if(state->typeStack[state->typeCount-k].opCount==1&&state->opStack[offset].opType==OP_CONSTANT){//change type of constant instead of cast
        state->typeStack[state->typeCount-k].type=types[nTypes-k];
        state->opStack[offset].dataType=types[nTypes-k];
        continue;
      }
      nCasts++;
      continue;
    }
    //convert enum labels to enum constants
    if(state->typeStack[state->typeCount-k].type.typeClass==TYPECLASS_ENUM_LABEL&&types[nTypes-k].typeClass==TYPECLASS_ENUM&&
      state->typeStack[state->typeCount-k].type.typeDataAs.composite->id==types[nTypes-k].typeDataAs.composite->id){
      if(state->typeStack[state->typeCount-k].opCount>1||state->opStack[offset].opType!=OP_CONSTANT)
        return ERROR_MEMORY;//enum-label type should only exist on enum-label constants
      state->typeStack[state->typeCount-k].type.typeClass=TYPECLASS_ENUM;
      if(!isVoidType(&(state->typeStack[state->typeCount-k].type.typeDataAs.composite->types[state->opStack[offset].dataAs.i64]))){
        String label=state->typeStack[state->typeCount-k].type.typeDataAs.composite->labels[state->opStack[offset].dataAs.i64];
        fprintf(stderr,"missing data value for creating enum constant %.*s in ",(int)label.length,label.chars);
        printTypeName(&(state->typeStack[state->typeCount-k].type),stderr);
        fputs("\nto create enum values with data use the 'new' operator\n",stderr);
        return ERROR_SYNTAX;
      }
      state->opStack[offset].opType=OP_NEW;
      state->opStack[offset].dataType.typeClass=TYPECLASS_ENUM;
      state->typeStack[state->typeCount-k].type=state->opStack[offset].dataType;
      continue;
    }
    typeErrorMessage(opName,types[nTypes-k],state->typeStack[state->typeCount-k].type);
    return ERROR_TYPE;
  }
  if(nCasts==0)
    return 0;
  //extract composites
  Error r=extractCompositeOps(state,nTypes);
  if(r.errorCode!=0)
    return r.errorCode;
  //modify-types
  if(ensureOpStackCap(state,state->opStackCount+nCasts))
    return ERROR_MEMORY;
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
      state->opStack[offset+nCasts]=(Operation){.opType=OP_CAST,.filePos=pos,.dataType=types[nTypes-k],.dataAs={0}};
      state->typeStack[state->typeCount-k].type=types[nTypes-k];
      state->typeStack[state->typeCount-k].opCount++;
      continue;
    }
    if(state->typeStack[state->typeCount-k].type.typeClass==TYPECLASS_ENUM&&types[nTypes-k].typeClass==TYPECLASS_ENUM_LABEL){
      state->opStack[offset+nCasts]=(Operation){.opType=OP_GET,.filePos=pos,.dataType=types[nTypes-k],.dataAs={.idInfo={.type=ID_ENUM_LABEL,.id=0}}};
      state->typeStack[state->typeCount-k].type=types[nTypes-k];
      state->typeStack[state->typeCount-k].opCount++;
      continue;
    }
    return ERROR_UNIMPLEMENTED;
  }
  return 0;
}

Error pushValue(TypeCheckState* state,Operation op){
  if(ensureOpStackCap(state,state->opStackCount+1)||ensureTypeStackCap(state,state->typeCount+1))
    return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
  state->opStack[state->opStackCount++]=op;
  state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=1};
  return (Error){.errorCode=0,.pos=op.filePos};
}

Error insertStackOperation(TypeCheckState* state,Operation op,size_t totalOps){
  if(ensureOpStackCap(state,state->opStackCount+1))
    return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
  memmove(state->opStack+state->opStackCount-totalOps+1,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
  state->opStack[state->opStackCount-totalOps]=op;
  state->opStackCount++;
  return (Error){.errorCode=0,.pos=op.filePos};;
}

Error typeCheckCall(Operation* op,TypeCheckState* state,bool isPtr){
  Error r;
  DataType calledType=op->dataType;
  if(isPtr){
    if(state->typeCount<1||!isCallableType(&(state->typeStack[state->typeCount-1].type))){
      fprintf(stderr,"the argument of %s has to be a callable Type\n",opName(op->opType));
      return (Error){.errorCode=ERROR_TYPE,.pos=op->filePos};
    }
    calledType=state->typeStack[state->typeCount-1].type;
    if(isPointerType(&calledType))
      calledType=*(calledType.typeDataAs.type);
    op->dataType=calledType;
  }
  //  need check for value of pointer
  if(!isCallableType(&calledType)){
    fputs("cannot call objects of type ",stderr);
    printTypeName(&calledType,stderr);
    fputs("\n",stderr);
    return (Error){.errorCode=ERROR_TYPE,.pos=op->filePos};
  }
  ProcedureType* procType=calledType.typeDataAs.procedure;
  DataType outType=*(procType->outType);
  size_t argCount=1;
  size_t totalOps=0;
  if(isVoidType(procType->inType)){//no arguments
    argCount=0;
    //don't return, output still has to be handled
  }
  if(procType->inType->typeClass==TYPECLASS_FLAT_TUPLE){
    argCount=procType->inType->typeDataAs.composite->typeCount;
  }
  if(state->typeCount<argCount){
    fprintf(stderr,"not enough operands for procedure call: need %zu got %zu\n",argCount,state->typeCount);
    return (Error){.errorCode=ERROR_TYPE,.pos=op->filePos};
  }
  //extract operations
  r=extractCompositeOps(state,argCount+(isPtr?1:0));
  if(r.errorCode!=0)
    return r;
  int32_t tmpId;
  if(!isVoidType(&outType)){//store result in temp variable
    tmpId=state->tmpCount++;
    pushCompiledOperation(state,opDeclareIntermediate(procType->outType,tmpId,op->filePos));
  }
  r=addCompiledOp(state,*op,isPtr?1:0);
  if(r.errorCode!=0)
    return r;
  size_t offset=state->typeCount-argCount;
  if(argCount==1){//function takes single argument, the single argument will not be a flat tuple (tuples have >= 2 elements)
    int err=requireTypes("procedure argument",state,procType->inType,1,op->filePos);
    if(err!=0){
      return (Error){.errorCode=err,.pos=op->filePos};
    }
    totalOps=state->typeStack[offset].opCount;
  }
  if(argCount>1){//argument is flat tuple
    CompositeType* inTypes=procType->inType->typeDataAs.composite;
    int err=requireTypes("procedure argument",state,inTypes->types,inTypes->typeCount,op->filePos);
    if(err!=0){
      return (Error){.errorCode=err,.pos=op->filePos};
    }
    for(int32_t i=0;i<inTypes->typeCount;i++){
      totalOps+=state->typeStack[offset+i].opCount;
    }
  }
  //update op-stack
  r=addCompiledStackOps(state,*op/*ignored*/,argCount,argCount,false);
  if(r.errorCode!=0||isVoidType(&outType))
    return r;//no need to update stack if called function returns void
  //add values of call
  if(outType.typeClass!=TYPECLASS_FLAT_TUPLE){//single return value
    return pushValue(state,opGetIntermediate(procType->outType,tmpId,op->filePos));
  }
  //auto-unwrap multi-return values using flat-tuple return values
  outType.typeClass=TYPECLASS_TUPLE;//convert flat tuple to tuple for correct variable type
  if(ensureTypeStackCap(state,state->typeCount+outType.typeDataAs.composite->typeCount)||
  ensureOpStackCap(state,state->opStackCount+3*outType.typeDataAs.composite->typeCount)){
    return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
  }
  for(int32_t e=0;e<outType.typeDataAs.composite->typeCount;e++){
    state->typeStack[state->typeCount++]=(TypeInfo){.type=outType.typeDataAs.composite->types[e],.opCount=3};
    state->opStack[state->opStackCount++]=(Operation){.opType=OP_GET,.dataType=outType.typeDataAs.composite->types[e],.filePos=op->filePos,.dataAs={.idInfo={.type=ID_TUPLE,.id=1}}};
    state->opStack[state->opStackCount++]=opGetIntermediate(&outType.typeDataAs.composite->types[e],tmpId,op->filePos);
    state->opStack[state->opStackCount++]=(Operation){.opType=OP_GET,.dataType=outType.typeDataAs.composite->types[e],.filePos=op->filePos,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=e}}};
  }
  return (Error){.errorCode=0,.pos=op->filePos};
}
Error pushProcArgs(TypeCheckState* state,DataType* procType,FilePosition pos){
  if(!isCallableType(procType)||isPointerType(procType))
     return (Error){.errorCode=ERROR_TYPE,.pos=pos};
  DataType* inType=procType->typeDataAs.procedure->inType;
  if(isVoidType(inType))
    return (Error){.errorCode=0,.pos=pos};//no input arguments
  if(inType->typeClass!=TYPECLASS_FLAT_TUPLE)
    return pushValue(state,(Operation){.opType=OP_GET,.dataType=*inType,.filePos=pos,.dataAs={.idInfo={.type=ID_ARGUMENT,.id=0}}});
  Error r;
  for(int32_t i=0;i<inType->typeDataAs.composite->typeCount;i++){
    r=pushValue(state,(Operation){.opType=OP_GET,.dataType=inType->typeDataAs.composite->types[i],.filePos=pos,.dataAs={.idInfo={.type=ID_ARGUMENT,.id=i}}});
    if(r.errorCode!=0)
      return r;
  }
  return (Error){.errorCode=0,.pos=pos};
}
Error compileGetTupleElement(TypeCheckState* state,CompositeType* tuple,Operation* op){
  size_t offset=state->typeCount-1;
  op->dataType=asWritableType(tuple->types[op->dataAs.idInfo.id],true);
  if(state->opStack[state->opStackCount-state->typeStack[offset].opCount].opType==OP_GET&&(
      state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.type==ID_POINTER||
      state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.type==ID_POINTER_OFFSET||
      state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.type==ID_TUPLE)){
    if(ensureOpStackCap(state,state->opStackCount+1))
      return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
    state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.id++;
    state->opStack[state->opStackCount++]=*op;
    state->typeStack[offset].type=op->dataType;
    state->typeStack[offset].opCount++;
    return (Error){.errorCode=0,.pos=op->filePos};
  }
  //wrap composite operations
  Error r=extractCompositeOps(state,1);
  if(r.errorCode!=0)
    return r;
  //update operation stack
  size_t totalOps=state->typeStack[offset].opCount;
  if(ensureOpStackCap(state,state->opStackCount+totalOps+2))
    return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
  r=insertStackOperation(state,(Operation){.opType=OP_GET,.dataType=op->dataType,.dataAs={.idInfo={.type=ID_TUPLE,.id=1}}},totalOps);
  if(r.errorCode!=0)
    return r;
  state->opStack[state->opStackCount++]=*op;
  //update type-stack
  state->typeStack[offset].type=op->dataType;
  state->typeStack[offset].opCount+=2;
  return (Error){.errorCode=0,.pos=op->filePos};
}

bool checkLocal(TypeCheckState* state,Operation op){
  if(state->blockCount!=0)
    return false;
  fputs("unexpected operation at global level: ",stderr);
  printOperation(op,stderr);
  return true;
}

Error typeCheckOperation(Operation op,TypeCheckState* state){
  if(op.opType==OP_IDENTIFIER||op.opType==OP_IDENTIFIER_ADDRESS){
    ScopeNode* asIdentifier;
    int r=getIdentifier(op.dataAs.string,&asIdentifier);
    if(r!=0){
      fprintf(stderr," unknown identifer '%.*s'\n",(int)op.dataAs.string.length,op.dataAs.string.chars);
      return (Error){.errorCode=r,.pos=op.filePos};
    }
    if(asIdentifier->idType!=ID_PROCEDURE)
      asIdentifier->type=asWritableType(asIdentifier->type,true);
    else 
      asIdentifier->type=asAddressableType(asIdentifier->type);
    op=(Operation){.opType=((asIdentifier->idType==ID_PROCEDURE)&&(op.opType!=OP_IDENTIFIER_ADDRESS))?OP_CALL:OP_GET,
      .dataType=asIdentifier->type,.filePos=op.filePos,.dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id}}};
  }
  size_t totalOps=0;
  int32_t offset,tmpId;
  Error r;
  BlockInfo blockInfo;
  switch(op.opType){
    case OP_CONSTANT:
      return pushValue(state,op);
    case OP_UNARY_OPERATOR:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for unary operator %s: need 1 got %zu\n",binOpName(op.dataAs.binOp),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      //result of operation is neither addressable nor writable
      op.dataType=asConstType(state->typeStack[offset].type);//unary operator returns value of same type
      switch(op.dataAs.unOp){
        case INCREMENT:
        case DECREMENT:
          if(!state->typeStack[offset].type.isWritable){//value has to be 
            fprintf(stderr,"operand of unary operator %s has to be writable \n",unOpName(op.dataAs.unOp));
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
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
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          break;
        case FLIP:
          if(!isIntType(&(state->typeStack[offset].type))){
            fprintf(stderr,"wrong operand type for unary operator %s expected integer ",unOpName(op.dataAs.unOp));
            fputs(" got ",stderr);
            printTypeName(&(state->typeStack[offset].type),stderr);
            fputs("\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          break;
        case NOT:
          if(!isBoolType(&(state->typeStack[offset].type))){
            typeErrorMessage("unary operator NOT",primitiveType(PRIMITIVE_BOOL),state->typeStack[offset].type);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          break;
      }
      //update op-stack
      //store result in temp variable
      r=extractCompositeOps(state,1);
      if(r.errorCode!=0)
        return r;
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      r=addCompiledOp(state,op,1);
      if(r.errorCode!=0)
        return r;
      //update stack
      return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
    case OP_BINARY_OPERATOR:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      if(state->typeCount<2){
        fprintf(stderr,"not enough operands for binary operator %s: need 2 got %zu\n",binOpName(op.dataAs.binOp),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
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
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      //update operation stack
      //ensure operands have matching types
      r=(Error){.errorCode=requireTypes("binary operator",state,inTypes,2,op.filePos),.pos=op.filePos};
      if(r.errorCode!=0){
        return r;
      }
      //store result in temp variable
      r=extractCompositeOps(state,2);
      if(r.errorCode!=0)
        return r;
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      r=addCompiledOp(state,op,2);
      if(r.errorCode!=0)
        return r;
      //update stack
      return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
    case OP_PRINT:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      //can only print pointer (excluding procedure pointer) or non-void primitive
      if((!isPointerType(&(state->typeStack[offset].type))||isCallableType(&(state->typeStack[offset].type)))
        &&(!isPrimitiveType(&(state->typeStack[offset].type))||isVoidType(&(state->typeStack[offset].type)))){
        fputs("cannot print values of type ",stderr);
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs("\n",stderr);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      op.dataType=state->typeStack[offset].type;
      //update operations
      r=extractCompositeOps(state,1);
      if(r.errorCode!=0)
        return r;
      return addCompiledOp(state,op,1);
    case OP_CHECK_ARRAY_BOUNDS:
    case OP_CHECK_ENUM_INDEX:
      break;
    case OP_GET:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
        case ID_ARGUMENT:
        case ID_PROCEDURE:
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED)
            return pushValue(state,op);
          if(op.dataType.typeDataAs.typeId<=0||op.dataType.typeDataAs.typeId>state->nPredeclaredTypes)
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          op.dataType=state->predeclaredTypes[op.dataType.typeDataAs.typeId-1];//get predeceased type
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED)
            return pushValue(state,op);
          return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
        case ID_TUPLE_ELEMENT:
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-1;
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_TUPLE){
            printTypeName(&(state->typeStack[offset].type),stderr);
            fputs(" is not a tuple\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          CompositeType* tuple=state->typeStack[offset].type.typeDataAs.composite;
          if(tuple->typeCount<op.dataAs.idInfo.id){
            fprintf(stderr,"index %"PRIi32" exceeds element count of tuple %"PRIi32"\n",op.dataAs.idInfo.id,tuple->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          return compileGetTupleElement(state,tuple,&op);
        case ID_POINTER:
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s %s: need 1 got %zu\n",opName(op.opType),idNames[op.dataAs.idInfo.type],state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-1;
          if(!isPointerType(&(state->typeStack[offset].type))){
            fprintf(stderr,"invalid operand for %s %s : ",opName(op.opType),idNames[op.dataAs.idInfo.type]);
            printTypeName(&(state->typeStack[offset].type),stderr);
            fputs(" is not a pointer\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          op.dataType=*state->typeStack[offset].type.typeDataAs.type;
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_CONST_POINTER)
            op.dataType=asWritableType(op.dataType,false);//dereferenced pointer are writable but not addressable
          //update operation stack
          //wrap composite operations
          r=extractCompositeOps(state,1);
          if(r.errorCode!=0)
            return r;
          r=insertStackOperation(state,op,1);
          if(r.errorCode!=0)
            return r;
          //update type-stack
          state->typeStack[offset].type=op.dataType;
          state->typeStack[offset].opCount++;
          return (Error){.errorCode=0,.pos=op.filePos};
        case ID_POINTER_OFFSET:
          if(state->typeCount<2){
            fprintf(stderr,"not enough operands for operation %s %s: need 2 got %zu\n",opName(op.opType),idNames[op.dataAs.idInfo.type],state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-2;
          if(!isIntType(&(state->typeStack[offset+1].type))){
            fprintf(stderr,"invalid second operand for %s %s : ",opName(op.opType),idNames[op.dataAs.idInfo.type]);
            printTypeName(&(state->typeStack[offset+1].type),stderr);
            fputs(" expected an integer\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          if(isArrayType(&(state->typeStack[offset].type))){//handle wrapped array types
            //1. store array and index in temporary variables
            size_t indexId=state->tmpCount++,arrayId=state->tmpCount++;
            DataType indexType=state->typeStack[offset+1].type,arrayType=state->typeStack[offset].type;
            r=addCompiledStackOps(state,opDeclareIntermediate(&indexType,indexId,op.filePos),state->typeStack[offset+1].opCount,1,true);
            if(r.errorCode!=0){
              return r;
            }      
            //XXX don't store constant values in intermediate
            r=addCompiledStackOps(state,opDeclareIntermediate(&arrayType,arrayId,op.filePos),state->typeStack[offset].opCount,1,true);
            if(r.errorCode!=0){
              return r;
            }      
            size_t ptrIndex=state->tmpCount++,lenIndex=state->tmpCount++;
            pushCompiledOperation(state,opDeclareIntermediate(&(arrayType.typeDataAs.composite->types[0]),ptrIndex,op.filePos));
            pushCompiledOperation(state,(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[0],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE,.id=1}}});
            pushCompiledOperation(state,opGetIntermediate(&arrayType,arrayId,op.filePos));//pointer
            pushCompiledOperation(state,(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[0],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=0}}});
            pushCompiledOperation(state,opDeclareIntermediate(&(arrayType.typeDataAs.composite->types[1]),lenIndex,op.filePos));
            pushCompiledOperation(state,(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[1],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE,.id=1}}});
            pushCompiledOperation(state,opGetIntermediate(&arrayType,arrayId,op.filePos));//length
            pushCompiledOperation(state,(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[1],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=1}}});
            //2. check array-bounds
            state->hasCheckBounds=1;
            pushCompiledOperation(state,(Operation){.opType=OP_CHECK_ARRAY_BOUNDS,.dataType=TYPE_UNDEFINED,.filePos=op.filePos,.dataAs={0}});
            pushCompiledOperation(state,opGetIntermediate(&indexType,indexId,op.filePos));//index
            pushCompiledOperation(state,opGetIntermediate(&(arrayType.typeDataAs.composite->types[1]),lenIndex,op.filePos));//length
            
            //3. array access XXX? keep array access on stack to allow chaining with tuple access
            op.dataType=*(arrayType.typeDataAs.composite->types[0].typeDataAs.type);//target-type of pointer in first element of tuple
            if(arrayType.typeDataAs.composite->types[0].typeClass!=TYPECLASS_CONST_POINTER)
              op.dataType=asWritableType(op.dataType,false);
            tmpId=state->tmpCount++;
            pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
            pushCompiledOperation(state,op);//get pointer offset 
            pushCompiledOperation(state,opGetIntermediate(&(arrayType.typeDataAs.composite->types[0]),ptrIndex,op.filePos));//pointer
            pushCompiledOperation(state,opGetIntermediate(&indexType,indexId,op.filePos));
            //update stack
            return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
          }
          if(!isPointerType(&(state->typeStack[offset].type))){
            fprintf(stderr,"invalid first operand for %s %s : ",opName(op.opType),idNames[op.dataAs.idInfo.type]);
            printTypeName(&(state->typeStack[offset].type),stderr);
            fputs(" is not a pointer\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          op.dataType=*state->typeStack[offset].type.typeDataAs.type;
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_CONST_POINTER)
            op.dataType=asWritableType(op.dataType,false);//dereferenced pointer are writable but not addressable
          //wrap composite operations
          r=extractCompositeOps(state,2);
          if(r.errorCode!=0)
            return r;
          //update operation stack
          r=insertStackOperation(state,op,2);
          if(r.errorCode!=0)
            return r;
          //update type-stack
          state->typeCount--;
          state->typeStack[offset].type=op.dataType;
          state->typeStack[offset].opCount+=state->typeStack[offset+1].opCount+1;
          return (Error){.errorCode=0,.pos=op.filePos};
        case ID_INTERMEDIATE_RESULT:
        case ID_TMP_VAR:
        case ID_TUPLE:
          break;
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
          fputs("direct access to enum elements should not exist at this stage of compilation\n",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
        case ID_TYPE:
          fputs("identifiers of type-names should not exist at this stage of compilation\n",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      break;
    case OP_GET_LABEL:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      if(state->typeStack[offset].type.typeClass!=TYPECLASS_STRUCT&&state->typeStack[offset].type.typeClass!=TYPECLASS_ENUM){
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs(" is not a struct or enum\n",stderr);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      CompositeType* mStruct=state->typeStack[offset].type.typeDataAs.composite;
      int32_t labelIndex=indexOfStringArray(mStruct->labels,mStruct->typeCount,
        &op.dataAs.string,1);
      if(labelIndex==-1){
        printTypeName(&(state->typeStack[offset].type),stderr);
        fprintf(stderr," does not have a field '%.*s'\n",(int)op.dataAs.string.length,op.dataAs.string.chars);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      if(state->typeStack[offset].type.typeClass==TYPECLASS_STRUCT){
        op=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=op.filePos,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=labelIndex}}};
        return compileGetTupleElement(state,mStruct,&op);
      }
      if(isVoidType(&(mStruct->types[labelIndex]))){
        fprintf(stderr,"'%.*s' in ",(int)op.dataAs.string.length,op.dataAs.string.chars);
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs(" does not hold a value\n",stderr);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      r=extractCompositeOps(state,1);
      if(r.errorCode!=0)
        return r;
      size_t enumIndex=state->tmpCount++;
      tmpId=state->tmpCount++;
      r=addCompiledOp(state,opDeclareIntermediate(&(state->typeStack[offset].type),enumIndex,op.filePos),1);
      if(r.errorCode!=0)
        return r;
      pushCompiledOperation(state,(Operation){.opType=OP_CHECK_ENUM_INDEX,.dataType=(mStruct->types[labelIndex]),.filePos=op.filePos,.dataAs={.i64=labelIndex}});
      pushCompiledOperation(state,opGetIntermediate(&(state->typeStack[offset].type),enumIndex,op.filePos));
      state->hasCheckEnum=1;
      pushCompiledOperation(state,opDeclareIntermediate(&(mStruct->types[labelIndex]),tmpId,op.filePos));
      pushCompiledOperation(state,(Operation){.opType=OP_GET,.dataType=mStruct->types[labelIndex],.filePos=op.filePos,.dataAs={.idInfo={.type=ID_ENUM_ELEMENT,.id=labelIndex}}});
      pushCompiledOperation(state,opGetIntermediate(&(state->typeStack[offset].type),enumIndex,op.filePos));
      //XXX? store element value on stack, to allow write operation
      return pushValue(state,opGetIntermediate(&(mStruct->types[labelIndex]),tmpId,op.filePos));
    case OP_SET_VALUE:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      if(state->typeCount<2){
        fprintf(stderr,"not enough operands for operation %s : need 2 got %zu\n",opName(op.opType),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      if(!state->typeStack[state->typeCount-1].type.isWritable){
        fprintf(stderr,"the second operand of %s has to be a writable type \n",opName(op.opType));
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      //update type of operation
      op.dataType=asConstType(state->typeStack[state->typeCount-1].type);
      //wrap composite operations
      r=extractCompositeOpsOffset(state,1,1);
      if(r.errorCode!=0)
        return r;
      //add compiled op to program
      r=addCompiledStackOps(state,op,state->typeStack[state->typeCount-1].opCount,1,true);
      if(r.errorCode!=0){
        return r;
      }      
      //check source type
      r=(Error){.errorCode=requireTypes("assignment",state,&op.dataType,1,op.filePos),.pos=op.filePos};
      if(r.errorCode!=0){
        return r;
      }
      return addCompiledStackOps(state,op,state->typeStack[state->typeCount-1].opCount,1,false);
    case OP_PRE_DECLARE:
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
          if(op.dataType.typeClass==TYPECLASS_UNDEFINED||isVoidType(&(op.dataType))){
            fputs("invalid type for predeclared variable: ",stderr);
            printTypeName(&(op.dataType),stderr);
            fputs("\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          return addCompiledOp(state,op,0);
        case ID_PROCEDURE:
          if(isCallableType(&(op.dataType))||isPointerType(&(op.dataType))){
            fputs("invalid type for predeclared procedure: ",stderr);
            printTypeName(&(op.dataType),stderr);
            fputs("\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          return addCompiledOp(state,op,0);
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_INTERMEDIATE_RESULT:
        case ID_TMP_VAR:
        case ID_ARGUMENT:
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
            fputs("cannot (directly) declare ",stderr);
            printIdInfo(op.dataAs.idInfo,stderr);
            fputs("\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
        case ID_TYPE:
          fputs("identifiers of type-names should not exist at this stage of compilation\n",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      break;
    case OP_DECLARE:
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
          if(isVoidType(&(op.dataType))){
            fputs("cannot declare variables of type: ",stderr);
            printTypeName(&(op.dataType),stderr);
            fputs("\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-1;
          //find types for auto-types
          if(op.dataType.typeClass==TYPECLASS_UNDEFINED){
            if(op.dataType.typeDataAs.typeId<=0||op.dataType.typeDataAs.typeId>state->nPredeclaredTypes)
                return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
            int64_t typeId=op.dataType.typeDataAs.typeId-1;
            op.dataType=state->typeStack[offset].type;
            if(op.dataType.typeClass==TYPECLASS_ENUM_LABEL)
              op.dataType.typeClass=TYPECLASS_ENUM;
            //XXX don't set constant variables to type writable
            op.dataType=asWritableType(op.dataType,true);
            state->predeclaredTypes[typeId]=op.dataType;
          }
          r=(Error){.errorCode=requireTypes("variable declaration",state,&op.dataType,1,op.filePos),.pos=op.filePos};
          if(r.errorCode!=0){
            return r;
          }
          if(state->blockCount==0)//don't extract operations at global level
            return addCompiledStackOps(state,op,state->typeStack[offset].opCount,1,true);
          r=extractCompositeOps(state,1);
          if(r.errorCode!=0)
            return r;
          return addCompiledOp(state,op,1);
        case ID_PROCEDURE:
          if(checkNonemptyStack(state,"unfinished global operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(state->blockCount==0){//predeclare procedure in global section
            op.opType=OP_PRE_DECLARE;
            pushCompiledOperation(state,op);
            op.opType=OP_DECLARE;
          }
          if(pushBlock(state,(BlockInfo){.type=BLOCK_PROCEDURE,.blockStart=state->opCount,.blockDataAs={0}}))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          pushCompiledOperation(state,op);
          return pushProcArgs(state,&op.dataType,op.filePos);
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_INTERMEDIATE_RESULT:
        case ID_TMP_VAR:
        case ID_ARGUMENT:
        case ID_ENUM_LABEL:
        case ID_ENUM_ELEMENT:
            fputs("cannot (directly) declare ",stderr);
            printIdInfo(op.dataAs.idInfo,stderr);
            fputs("\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
        case ID_TYPE:
          fputs("identifiers of type-names should not exist at this stage of compilation\n",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      break;
    case OP_NEW:
      if(op.dataType.typeClass==TYPECLASS_TUPLE||op.dataType.typeClass==TYPECLASS_STRUCT){
        offset=state->typeCount-op.dataType.typeDataAs.composite->typeCount;
        r=(Error){.errorCode=requireTypes("tuple creation",state,op.dataType.typeDataAs.composite->types,
          op.dataType.typeDataAs.composite->typeCount,op.filePos),.pos=op.filePos};
        if(r.errorCode!=0){
          return r;
        }
        totalOps=0;
        for(int32_t e=0;e<op.dataType.typeDataAs.composite->typeCount;e++){
          totalOps+=state->typeStack[offset+e].opCount;
        }
        if(state->blockCount==0){//create tuple in-place when in global level
          r=insertStackOperation(state,op,totalOps);
          if(r.errorCode!=0)
            return r;
          state->typeCount-=op.dataType.typeDataAs.composite->typeCount;
          state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1};
          return (Error){.errorCode=0,.pos=op.filePos};
        }
        //store result in temp variable
        r=extractCompositeOps(state,op.dataType.typeDataAs.composite->typeCount);
        if(r.errorCode!=0)
          return r;
        tmpId=state->tmpCount++;
        pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
        r=addCompiledOp(state,op,op.dataType.typeDataAs.composite->typeCount);
        if(r.errorCode!=0)
          return r;
        //update stack
        return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
      }
      if(op.dataType.typeClass==TYPECLASS_ENUM){
        DataType* entryData=op.dataType.typeDataAs.composite->types+op.dataAs.i64;
        if(isVoidType(entryData)){
          if(state->blockCount==0){//create enum in-place when in global level
            r=insertStackOperation(state,op,0);
            if(r.errorCode!=0)
              return r;
            state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1};
            return (Error){.errorCode=0,.pos=op.filePos};
          }
          tmpId=state->tmpCount++;
          pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
          r=addCompiledOp(state,op,0);
          if(r.errorCode!=0)
            return r;
          //update stack
          return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
        }
        r=(Error){.errorCode=requireTypes("enum creation",state,entryData,1,op.filePos),.pos=op.filePos};
        if(r.errorCode!=0)
          return r;
        if(state->blockCount==0){//create enum in-place when in global level
          totalOps=state->typeStack[state->typeCount-1].opCount;
          r=insertStackOperation(state,op,totalOps);
          if(r.errorCode!=0)
            return r;
          state->typeStack[state->typeCount-1]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1};
          return (Error){.errorCode=0,.pos=op.filePos};
        }
        r=extractCompositeOps(state,1);
        if(r.errorCode!=0)
          return r;
        tmpId=state->tmpCount++;
        pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
        r=addCompiledOp(state,op,1);
        if(r.errorCode!=0)
          return r;
        //update stack
        return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
      }
      break;
    case OP_CAST:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s : need 1 got %zu\n",opName(op.opType),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      if(!canCast(&(state->typeStack[offset].type),&(op.dataType))){
        fputs("cannot cast ",stderr);
        printTypeName(&(state->typeStack[offset].type),stderr);
        fputs(" to ",stderr); 
        printTypeName(&(op.dataType),stderr);
        fputs("\n",stderr);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      //store previous result in temp value
      r=extractCompositeOps(state,1);
      if(r.errorCode!=0)
        return r;
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      r=addCompiledOp(state,op,1);
      if(r.errorCode!=0)
        return r;
      //update stack
      return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
    case OP_ADDR_OF:
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s : need 1 got %zu\n",opName(op.opType),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      if(!state->typeStack[offset].type.isAddressable){
          fprintf(stderr,"the operand of %s has to be an addressable type \n",opName(op.opType));
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      op.dataType=pointerType(&(state->typeStack[offset].type));
      //store result in temp variable
      tmpId=state->tmpCount++;
      pushCompiledOperation(state,opDeclareIntermediate(&op.dataType,tmpId,op.filePos));
      r=addCompiledStackOps(state,op,state->typeStack[offset].opCount,1,true);
      if(r.errorCode!=0)
        return r;
      //update stack
      return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
    case OP_CODE_BLOCK:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      switch(op.dataAs.block){
        case BLOCK_IF:
          blockInfo=peekBlock(state);
          if(blockInfo.type==BLOCK_END){//block stack underflow
            fputs("unexpected IF statement, IF statements cannot be declared at global level\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          blockInfo=(BlockInfo){.type=BLOCK_IF,.blockStart=state->opCount,.blockDataAs={0}};
          blockInfo.blockDataAs.ifBlock.inStack.types=NULL;
          blockInfo.blockDataAs.ifBlock.inStack.ops=NULL;
          blockInfo.blockDataAs.ifBlock.outStack.types=NULL;
          blockInfo.blockDataAs.ifBlock.outStack.ops=NULL;
          //store in-types
          if(state->typeCount>1){
            blockInfo.blockDataAs.ifBlock.inStack.typeCount=state->typeCount-1;
            blockInfo.blockDataAs.ifBlock.inStack.opCount=state->opStackCount-state->typeStack[state->typeCount-1].opCount;
            blockInfo.blockDataAs.ifBlock.inStack.types=malloc((state->typeCount-1)*sizeof(TypeInfo));
            blockInfo.blockDataAs.ifBlock.inStack.ops=malloc((blockInfo.blockDataAs.ifBlock.inStack.opCount)*sizeof(Operation));
            if(blockInfo.blockDataAs.ifBlock.inStack.types==NULL||blockInfo.blockDataAs.ifBlock.inStack.ops==NULL)
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            memcpy(blockInfo.blockDataAs.ifBlock.inStack.types,state->typeStack,(state->typeCount-1)*sizeof(TypeInfo));
            memcpy(blockInfo.blockDataAs.ifBlock.inStack.ops,state->opStack,(blockInfo.blockDataAs.ifBlock.inStack.opCount)*sizeof(Operation));
          }
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          
          op.dataType=primitiveType(PRIMITIVE_BOOL);
          r=(Error){.errorCode=requireTypes("if-condition",state,&op.dataType,1,op.filePos),.pos=op.filePos};
          if(r.errorCode!=0){
            return r;
          }
          r=extractCompositeOps(state,1);
          if(r.errorCode!=0)
            return r;
          offset=state->typeCount-1;
          r=addCompiledOp(state,op,1); 
          if(r.errorCode!=0)
            return r;
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_ELSE:
          blockInfo=popBlock(state);
          if(blockInfo.type!=BLOCK_IF){//wrong position for ELSE
            fputs("ELSE can only appear in IF blocks\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          r=checkIfTypes(state,&(blockInfo.blockDataAs.ifBlock),op.filePos);
          if(r.errorCode!=0){
            return r;
          }
          //reset stack to in-types 
          if(resetStack(state,&(blockInfo.blockDataAs.ifBlock.inStack)))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          //push updated block
          blockInfo.type=BLOCK_ELSE;
          blockInfo.blockDataAs.ifBlock.elsePos=state->opCount;
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          pushCompiledOperation(state,op);
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_IF2:
          blockInfo=popBlock(state);
          if(blockInfo.type!=BLOCK_ELSE){//wrong position for _IF
            fputs("_IF can only appear in ELSE blocks\n",stderr);
            printf("%u\n",blockInfo.type);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          //update inTypes
          if(state->typeCount>1){
            if(blockInfo.blockDataAs.ifBlock.inStack.typeCount<state->typeCount-1){
              //if allocation fails program will be terminated -> can directly assign result of realloc
              blockInfo.blockDataAs.ifBlock.inStack.types=realloc(blockInfo.blockDataAs.ifBlock.inStack.types,
                (state->typeCount-1)*sizeof(TypeInfo));
            }
            blockInfo.blockDataAs.ifBlock.inStack.typeCount=state->typeCount-1;
            if(blockInfo.blockDataAs.ifBlock.inStack.opCount<state->opStackCount-state->typeStack[state->typeCount-1].opCount){
              //if allocation fails program will be terminated -> can directly assign result of realloc
              blockInfo.blockDataAs.ifBlock.inStack.types=realloc(blockInfo.blockDataAs.ifBlock.inStack.types,
                (state->opStackCount-state->typeStack[state->typeCount-1].opCount)*sizeof(TypeInfo));
            }
            blockInfo.blockDataAs.ifBlock.inStack.opCount=state->opStackCount-state->typeStack[state->typeCount-1].opCount;
            if(blockInfo.blockDataAs.ifBlock.inStack.types==NULL||blockInfo.blockDataAs.ifBlock.inStack.ops==NULL)
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            memcpy(blockInfo.blockDataAs.ifBlock.inStack.types,state->typeStack,(state->typeCount-1)*sizeof(TypeInfo));
            memcpy(blockInfo.blockDataAs.ifBlock.inStack.ops,state->opStack,(blockInfo.blockDataAs.ifBlock.inStack.opCount)*sizeof(Operation));
          }else{
            blockInfo.blockDataAs.ifBlock.inStack.typeCount=0;
            blockInfo.blockDataAs.ifBlock.inStack.opCount=0;
            //the memory sections will be freed when an end-block is encountered
          }
          //push updated block
          blockInfo.type=BLOCK_IF;
          blockInfo.blockDataAs.ifBlock.elifCount++;
          blockInfo.blockDataAs.ifBlock.elsePos=state->opCount;
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          op.dataType=primitiveType(PRIMITIVE_BOOL);
          r=(Error){.errorCode=requireTypes("if-condition",state,&op.dataType,1,op.filePos),.pos=op.filePos};
          if(r.errorCode!=0){
            return r;
          }
          r=extractCompositeOps(state,1);
          if(r.errorCode!=0)
            return r;
          offset=state->typeCount-1;
          r=addCompiledOp(state,op,1); 
          if(r.errorCode!=0)
            return r;
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_WHILE:
          blockInfo=(BlockInfo){.type=BLOCK_WHILE,.blockStart=state->opCount,.blockDataAs={0}};
          blockInfo.blockDataAs.whileBlock.inStack.types=NULL;
          blockInfo.blockDataAs.whileBlock.inStack.ops=NULL;
          blockInfo.blockDataAs.whileBlock.outStack.types=NULL;
          blockInfo.blockDataAs.whileBlock.outStack.ops=NULL;
          //store types at loop start
          r=initWhileTypes(state,&(blockInfo.blockDataAs.whileBlock),op.filePos);
          if(r.errorCode!=0)
            return r;
          if(resetStack(state,&(blockInfo.blockDataAs.whileBlock.inStack)))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          pushCompiledOperation(state,opCodeBlock(BLOCK_DO,op.filePos));
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_DO:
          blockInfo=popBlock(state);
          if(blockInfo.type!=BLOCK_WHILE){//wrong position for DO
            fputs("DO can only appear in WHILE-DO blocks\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(blockInfo.blockDataAs.whileBlock.hasDo){//wrong position for DO
            fputs("DO cannot appear more than once per WHILE block\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          //XXX convert while-blocks with empty condition to do-blocks
          blockInfo.blockDataAs.whileBlock.hasDo=true;
          //store types at loop condition
          r=initWhileOutTypes(state,&(blockInfo.blockDataAs.whileBlock),op.filePos);
          if(r.errorCode!=0)
            return r;
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          op.dataType=primitiveType(PRIMITIVE_BOOL);
          op.dataAs.block=BLOCK_WHILE;
          r=(Error){.errorCode=requireTypes("while-condition",state,&op.dataType,1,op.filePos),.pos=op.filePos};
          if(r.errorCode!=0){
            return r;
          }
          offset=state->typeCount-1;
          r=extractCompositeOps(state,1);
          if(r.errorCode!=0)
            return r;
          r=addCompiledOp(state,op,1); 
          if(r.errorCode!=0)
            return r;
          //reset stack after compiling condition
          if(resetStack(state,&(blockInfo.blockDataAs.whileBlock.outStack)))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_WHILE_END:
          fputs("WHILE_END blocks are not supported use WHILE ... DO END to build a do-while statement",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
        case BLOCK_START:
          if(checkNonemptyStack(state,"unfinished local operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(pushBlock(state,(BlockInfo){.type=BLOCK_START,.blockStart=state->opCount,.blockDataAs={0}}))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          pushCompiledOperation(state,op);
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_END:
          blockInfo=peekBlock(state);//keep block on block stack until writing operations has finished
          if(blockInfo.type==BLOCK_END||(blockInfo.type==BLOCK_WHILE&&!blockInfo.blockDataAs.whileBlock.hasDo)){
            fputs("unexpected END statement\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          int32_t endCount=1;
          if(blockInfo.type==BLOCK_IF||blockInfo.type==BLOCK_ELSE){
            endCount+=blockInfo.blockDataAs.ifBlock.elifCount;
            r=checkIfTypes(state,&(blockInfo.blockDataAs.ifBlock),op.filePos);
            if(r.errorCode!=0){
              return r;
            }
            if(blockInfo.type==BLOCK_ELSE){//if ends with else branch
              if(predeclareBlockVariables(state,blockInfo.blockStart,&(blockInfo.blockDataAs.ifBlock.outStack)))
                 return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            }else{
              if(declareBlockVariables(state,blockInfo.blockStart,&(blockInfo.blockDataAs.ifBlock.outStack),&(blockInfo.blockDataAs.ifBlock.inStack)))
                 return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            }
            if(resetStack(state,&(blockInfo.blockDataAs.ifBlock.outStack))){
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            }
            //free values on op-stack
            free(blockInfo.blockDataAs.ifBlock.inStack.types);
            free(blockInfo.blockDataAs.ifBlock.inStack.ops);
            free(blockInfo.blockDataAs.ifBlock.outStack.types);
            free(blockInfo.blockDataAs.ifBlock.outStack.ops);
          }else if(blockInfo.type==BLOCK_WHILE){
            op.dataAs.block=BLOCK_WHILE_END;
            r=checkWhileTypes(state,&(blockInfo.blockDataAs.whileBlock),op.filePos);
            if(r.errorCode!=0)
              return r;
            if(predeclareBlockVariables(state,blockInfo.blockStart,&(blockInfo.blockDataAs.whileBlock.outStack)))
               return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            if(resetStack(state,&(blockInfo.blockDataAs.whileBlock.outStack)))
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          }else{
            //TODO check for procedures with missing return statements
            if(checkNonemptyStack(state,"unfinished local operation")){
              return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
            }
          }
          while(endCount-->0){
            pushCompiledOperation(state,op);
          }
          popBlock(state);//pop block after writing operations
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_PROCEDURE:
          fputs("blocks of type BLOCK_PROCEDURE are not supported, procedure blocks start with the DECLARE_PROCEDURE operation",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      break;
    case OP_CALL_PTR:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      return typeCheckCall(&op,state,true);
    case OP_CALL:
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      return typeCheckCall(&op,state,false);
    case OP_RETURN:   
      if(checkLocal(state,op))
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};  
      if(isVoidType(&(op.dataType))){
          if(checkNonemptyStack(state,"unfinished operation at end of procedure"))
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
        pushCompiledOperation(state,op);
        return (Error){.errorCode=0,.pos=op.filePos};
      }
      if(state->opStackCount==0){
        fputs("missing return value",stderr);
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      if(state->typeCount==1){
        if(op.dataType.typeClass==TYPECLASS_FLAT_TUPLE){
          op.dataType.typeClass=TYPECLASS_TUPLE;//notify compiler that tuple is non-flat
        }
        r=(Error){.errorCode=requireTypes("return statement",state,&op.dataType,1,op.filePos),.pos=op.filePos};
        if(r.errorCode!=0){
          return r;
        }
        r=extractCompositeOps(state,1);
        if(r.errorCode!=0)
          return r;
        return addCompiledOp(state,op,1);
      }
      if(op.dataType.typeClass!=TYPECLASS_TUPLE&&op.dataType.typeClass!=TYPECLASS_FLAT_TUPLE){
        checkNonemptyStack(state,"unfinished operation at end of procedure");//this should always return true
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      if(op.dataType.typeClass==TYPECLASS_TUPLE){
        op.dataType.typeClass=TYPECLASS_FLAT_TUPLE;//notify compiler that tuple is flat
      }
      if(op.dataType.typeDataAs.composite->typeCount<0||state->typeCount!=(size_t)op.dataType.typeDataAs.composite->typeCount){
        fprintf(stderr,"wrong number of return values: expected %"PRIi32" got %zu\n",op.dataType.typeDataAs.composite->typeCount,state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      r=(Error){.errorCode=requireTypes("return statement",state,op.dataType.typeDataAs.composite->types,state->typeCount,op.filePos),.pos=op.filePos};
      if(r.errorCode!=0){
        return r;
      }
      r=extractCompositeOps(state,state->typeCount);
      if(r.errorCode!=0)
        return r;
      return addCompiledOp(state,op,state->typeCount);
    case ENTRY_POINT://start of procedure
      if(checkNonemptyStack(state,"unfinished global operation")){
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      if(pushBlock(state,(BlockInfo){.type=BLOCK_PROCEDURE,.blockStart=state->opCount,.blockDataAs={0}}))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
      pushCompiledOperation(state,op);
      return (Error){.errorCode=0,.pos=op.filePos};
    case OP_IDENTIFIER:
    case OP_IDENTIFIER_ADDRESS:
      fprintf(stderr,"operation %s should not exist at this stage of compilation\n",opName(op.opType));
      return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
    //compile time ops
    case OP_MODIFY_STACK:
      switch(op.dataAs.stackMod.op){
        case STACK_OP_DUP://duplicate top value on stack
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          totalOps=state->typeStack[state->typeCount-1].opCount;
          if(ensureOpStackCap(state,state->opStackCount+totalOps)||ensureTypeStackCap(state,state->typeCount+1))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          memmove(state->opStack+state->opStackCount,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
          memmove(state->typeStack+state->typeCount,state->typeStack+state->typeCount-1,sizeof(TypeInfo));
          state->opStackCount+=totalOps;
          state->typeCount++;
          return (Error){.errorCode=0,.pos=op.filePos};
        case STACK_OP_DROP://remove top value from stack
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          state->typeCount--;
          state->opStackCount-=state->typeStack[state->typeCount].opCount;
          return (Error){.errorCode=0,.pos=op.filePos};
        case STACK_OP_SWAP:
          break;
      }
      break;
    case OP_TYPE_INFO:
      switch(op.dataAs.typeInfo.infoType){
        case TYPEINFO_TYPES:
          //TODO allow to limit op-count
          puts("types:\n-----------------");
          printTypeStack(state,false,stdout);
          puts("-----------------");
          return (Error){.errorCode=0,.pos=op.filePos};
        case TYPEINFO_STACK:
          puts("stack:\n-----------------");
          printTypeStack(state,true,stdout);
          puts("-----------------");
          return (Error){.errorCode=0,.pos=op.filePos};
      }
      break;
  }
  printf("type checking %s is not implemented\n",opName(op.opType));
  return (Error){.errorCode=ERROR_UNIMPLEMENTED,.pos=op.filePos};
}
Error typeCheckProgram(Program* prog,CodeFile* src){
  size_t opCap=prog->opCount>INIT_CAP?prog->opCount:INIT_CAP;
  TypeCheckState state=(TypeCheckState){
    .globalOperations=malloc(opCap*sizeof(Operation)),.globalCap=opCap,.globalCount=0,
    .compiledOperations=malloc(opCap*sizeof(Operation)),.opCap=opCap,.opCount=0,
    .opStack=malloc(INIT_CAP*sizeof(Operation)),.opStackCap=INIT_CAP,.opStackCount=0,
    .typeStack=malloc(INIT_CAP*sizeof(TypeInfo)),.typeStackCap=INIT_CAP,.typeCount=0,
    .openBlocks=malloc(INIT_CAP*sizeof(BlockInfo)),.blockCap=INIT_CAP,.blockCount=0,
    .predeclaredTypes=malloc(prog->predeclaredTypes*sizeof(DataType)),.nPredeclaredTypes=prog->predeclaredTypes,
    .globalScope=prog->globalScope,.tmpCount=0,.index=0};
  if(state.globalOperations==NULL||state.compiledOperations==NULL||state.opStack==NULL||state.typeStack==NULL||state.openBlocks==NULL||state.predeclaredTypes==NULL){//memory allocation failed
    freeContents(&state);
    return (Error){.errorCode=ERROR_MEMORY,.pos=src->currentPos};
  }
  Error r;
  while(state.index<prog->opCount){
    r=typeCheckOperation(prog->ops[state.index++],&state);
    if(r.errorCode!=0){
      freeContents(&state);
      return r;
    }
  }
  if(state.blockCount>0){
    fputs("unfinished code-block\n",stderr);
    return (Error){.errorCode=ERROR_SYNTAX,.pos=src->currentPos};
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
  return (Error){.errorCode=0,.pos=src->currentPos};
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

const char* path;
const char* srcFile;
const char* targetFile;
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
  srcFile=*(argv++);
  FILE *file = fopen(srcFile, "r");
  if(*argv==NULL){
    targetFile="./out.c";
  }else{
    targetFile=*(argv++);
  }
	if(file!=NULL){
		long int size=fsize(file);
		if(size<0){//TODO?? recover form undetected fileSize (if seek worked)
			fputs("IO Error while detecting file-size\n",stderr);
			return ERROR_IO;
		}else{
			code = malloc((size+1)*sizeof(char));//will be freed when the program exits
			if(code==NULL){
				printf("Memory Error\n");
				return ERROR_MEMORY;
			}
			codeSize=fread(code,sizeof(char),size,file);//TODO perform multiple reads if necessary
			if(codeSize<0){
				printf("IO Error while reading file\n");
				free(code);
				return ERROR_IO;
			}
			fclose(file);//file no longer needed
			memset(code+codeSize,0,(size+1-codeSize)*sizeof(char));//fill remaining path of file with 0
		}
		//1. compile file to operations
		CodeFile codeFile=(CodeFile){.code=code,.codeSize=codeSize,
		  .currentPos={.fileName=srcFile,.line=1,.posInLine=1},
		  .wordStart={.fileName=srcFile,.line=1,.posInLine=1}};
		Program p=compileToOps(&codeFile);
		if(p.ops==NULL)
		  return ERROR_SYNTAX;
	  printf("found %zu operations\n",p.opCount);
		//2. type-check operations
	  Error err=typeCheckProgram(&p,&codeFile);
	  if(err.errorCode!=0){
      printError(err,stderr);
      return err.errorCode;
    }
    printf("compiled to %zu operations\n",p.globalCount+p.opCount);
    for(size_t i=0;i<p.globalCount;i++){
      printOperation(p.globalOps[i],stdout);
    }
    for(size_t i=0;i<p.opCount;i++){
      printOperation(p.ops[i],stdout);
    }
    puts("");
		//3. compile operations to C
    FILE* out=fopen(targetFile,"w");
    err=compileToC(out,&p);
    if(err.errorCode){
      printError(err,stderr);
      goto RETURN;
    }
	  puts("compiled program");
  RETURN:
    fclose(out);
    return err.errorCode;
	}else{
		fprintf(stderr,"IO Error while opening File: %s\n",srcFile);
		return ERROR_IO;
	}
}
