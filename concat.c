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
#define MIN_ERROR -3
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

//negate indices (internal errors have negative error codes)
const char* const internalErrors [] = {[-ERROR_MEMORY]="ERROR_MEMORY",[-ERROR_IO]="ERROR_IO",[-ERROR_UNIMPLEMENTED]="ERROR_UNIMPLEMENTED",};
const char* const compilerErrors [] = {
[ERROR_TYPE]="type error",[ERROR_SYNTAX]="syntax error",[ERROR_PARSE_INT]="invalid character while parsing integer",[ERROR_INT_OVERFLOW]="integer exceeds maximum allowed value",
[ERROR_REDECLARATION]="redeclaration",[ERROR_UNSUPPORTED_ESCAPE_SEQUENCE]="unsupported escape sequence",[WARNING_CODEPOINT_OUT_OF_RANGE]="code-point out of range",
[ERROR_EOF]="unexpected end of file",};
const char* errorName(int errorCode){
  if(errorCode<MIN_ERROR){
    return "unknown internal error";
  }
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
  int c=strncmp(a.chars,b.chars,a.length<b.length?a.length:b.length);
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
  for(size_t off=0;off<base.length-child.length;off++){
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
  OP_STRING_CONST,
  
  OP_PRE_DECLARE,
  OP_DECLARE,
  OP_GET,
  
  OP_SET_VALUE, //  [T] [T.writable] SET 
  
  OP_NEW,
  OP_CAST,
  OP_ADDR_OF,// (pointer to given value)
  
  OP_BINARY_OPERATOR, 
  OP_UNARY_OPERATOR,  
  
  OP_CHECK_ARRAY_BOUNDS,//special operation for checking array bounds   checkBounds id  length  compiles to  if(tmpId<0||tmpId>=LENGTH) exit(ARRAY_ACCESS);
  OP_CODE_BLOCK,  
  
  OP_DECLARE_PROCEDURE, 
  OP_RETURN,       
  OP_CALL,         // procType procId
  ENTRY_POINT,     //entry point of the program, starts the main code section, section will close at the matching BLOCK_END 
}OpType;
const char* opName(OpType type){
  switch(type){
    case OP_PRINT:return "OP_PRINT";
    case OP_CONSTANT:return "OP_CONSTANT";
    case OP_STRING_CONST:return "OP_STRING_CONST";
    case OP_DECLARE:return "OP_DECLARE";
    case OP_PRE_DECLARE:return "OP_PRE_DECLARE";
    case OP_GET:return "OP_GET";
    case OP_SET_VALUE:return "OP_SET_VALUE";
    case OP_BINARY_OPERATOR:return "OP_BINARY_OPERATOR";
    case OP_UNARY_OPERATOR:return "OP_UNARY_OPERATOR";  
    case OP_CODE_BLOCK:return "OP_CODE_BLOCK";
    case OP_DECLARE_PROCEDURE:return "OP_DECLARE_PROCEDURE"; 
    case OP_RETURN:return "OP_RETURN";      
    case OP_CALL:return "OP_CALL";
    case ENTRY_POINT:return "ENTRY_POINT";
    case OP_NEW:return "OP_NEW";
    case OP_CAST:return "OP_CAST";
    case OP_ADDR_OF:return "OP_ADDR_OF";
    case OP_CHECK_ARRAY_BOUNDS:return "OP_CHECK_ARRAY_BOUNDS";
  }
  return "UNDEFINED";
}
const char* CHECK_BOUNDS_NAME="concatInternal_checkArrayBounds";
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
  bool isAddressable;
  bool isWritable;
  union{
    PrimitiveType primitive;
    struct DataType* type;
    CompositeType* composite;
    TupleType* tuple;//name alias for composite
    ProcedureType* procedure;
    int64_t typeId;
  }typeDataAs;
}DataType;
#define FLAG_IS_TUPLE 1
#define FLAG_IS_FLAT_TUPLE 2
struct CompositeType{
  DataType* types;
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

#define MAX_TYPES  4096
#define MAX_COMPOSITE 1024
#define TYPE_BUFFER_CAP 1024
#define MAX_PROC_TYPES 1024

size_t wrappedTypeCount=0;
DataType wrappedTypes[MAX_TYPES];
int32_t compositeCount=0;
CompositeType compositeTypes[MAX_COMPOSITE];
size_t procTypeCount=0;
ProcedureType procTypes[MAX_PROC_TYPES];
//temporary buffer for construction of composite elements
size_t bufferedTypes=0;
DataType typeBuffer[TYPE_BUFFER_CAP];
size_t bufferedFieldTypes=0;
char* fieldNameBuffer[TYPE_BUFFER_CAP];

bool typeEquals(const DataType* a,const DataType* b){
  if(a->typeClass!=b->typeClass)
    return false;
  if(a->typeClass==TYPECLASS_UNDEFINED)
    return true;//all undefined types are equal
  if(a->typeClass==TYPECLASS_PRIMITIVE)
    return a->typeDataAs.primitive==b->typeDataAs.primitive;
  if(a->typeClass==TYPECLASS_POINTER||a->typeClass==TYPECLASS_CONST_POINTER||a->typeClass==TYPECLASS_TYPE_OF)
    return typeEquals(a->typeDataAs.type,b->typeDataAs.type);
  if(a->typeClass==TYPECLASS_TUPLE||a->typeClass==TYPECLASS_FLAT_TUPLE)
    return a->typeDataAs.composite->id==b->typeDataAs.composite->id;
  if(a->typeClass==TYPECLASS_PROCEDURE)
    return typeEquals(a->typeDataAs.procedure->inType,b->typeDataAs.procedure->inType)&&
            typeEquals(a->typeDataAs.procedure->outType,b->typeDataAs.procedure->outType);
  if(a->typeClass==TYPECLASS_OPAQUE)
    return a->typeDataAs.typeId==b->typeDataAs.typeId;
  return false;
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
DataType compositeType(TypeClass typeClass,DataType* elements,int32_t eltCount){
  if(eltCount==0)
    return typeClass==TYPECLASS_FLAT_TUPLE?primitiveType(PRIMITIVE_VOID):TYPE_UNDEFINED;//empty flat-tuple -> void other empty composites are undefined
  if(eltCount==1&&typeClass==TYPECLASS_FLAT_TUPLE)
    return elements[0];//auto unwrap 1-element flat-tuple
  int16_t classFlag=typeClass==TYPECLASS_FLAT_TUPLE?FLAG_IS_FLAT_TUPLE:FLAG_IS_TUPLE;
  int64_t match=-1;
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].typeCount==eltCount||(match==-1&&compositeTypes[i].typeCount>eltCount)){
      bool isMatch=true;
      for(int32_t a=0;a<eltCount;a++){//XXX? allow matches of sub-lists
        if(!typeEquals(&(compositeTypes[i].types[a]),&(elements[a]))){
          isMatch=false;
          break;
        }
      }
      if(!isMatch)
        continue;
      if(compositeTypes[i].typeCount==eltCount){
        compositeTypes[i].flags|=classFlag;
        return (DataType){.typeClass=typeClass,.typeDataAs.composite=compositeTypes+i,.isAddressable=false,.isWritable=false};
      }
      match=i;
    }
  }
  if(compositeCount+1>=MAX_COMPOSITE)
    return TYPE_UNDEFINED;
  DataType* types;
  if(match!=-1){
    compositeTypes[match].flags|=classFlag;
    types=compositeTypes[match].types;
  }else{
    types=malloc(eltCount*sizeof(DataType));//will persist until program exits
    memcpy(types,elements,eltCount*sizeof(DataType));
  }
  if(types==NULL)
    return TYPE_UNDEFINED;
  compositeTypes[compositeCount]=(CompositeType){.id=compositeCount,.typeCount=eltCount,.types=types,.flags=classFlag};
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
bool isIntType(const DataType* type){
  return type->typeClass==TYPECLASS_PRIMITIVE&&isInteger(type->typeDataAs.primitive);
}
bool isPointerType(const DataType* type){
  return type->typeClass==TYPECLASS_POINTER||type->typeClass==TYPECLASS_CONST_POINTER;
}
//checks id type is an array-type  a tuple consisting of a pointer and an integer
bool isArrayType(const DataType* type){
  if(type->typeClass!=TYPECLASS_TUPLE)
    return false;
  CompositeType* elts=type->typeDataAs.composite;
  if(elts->typeCount!=2)
    return false;
  if(!isPointerType(elts->types+0))
    return false;
  if(!isIntType(elts->types+1))
    return false;
  return true;
}

const char* typeClassName(TypeClass cls){
  switch(cls){
    case TYPECLASS_UNDEFINED:
      return "UNDEFINED";
    case TYPECLASS_PRIMITIVE:
      return "PRIMITIVE";
    case TYPECLASS_POINTER:
      return "POINTER";
    case TYPECLASS_CONST_POINTER:
      return "CONST_POINTER";
    case TYPECLASS_TUPLE:
      return "TUPLE";
    case TYPECLASS_FLAT_TUPLE:
      return "FLAT_TUPLE";
    case TYPECLASS_PROCEDURE:
      return "PRODECURE";
    case TYPECLASS_TYPE_OF:
      return "TYPE_OF";
    case TYPECLASS_OPAQUE:
      return "TYPECLASS_OPAQUE";
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
      i=fprintf(file,"%s (%"PRIi32") ",typeClassName(type->typeClass),type->typeDataAs.composite->id);
      if(noRecurse||i<0)
        return i;
      j=fputs("(",file);
      if(j<0)
        return j;
      i+=j;
      for(int32_t e=0;e<type->typeDataAs.composite->typeCount;e++){
        j=fputs(" ",file);
        if(j<0)
          return j;
        i+=j;
        j=printTypeNameIntenal(&(type->typeDataAs.composite->types[e]),file,true);//only one recursion level
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
  return fprintf(file,"unknown type-class %i\n",type->typeClass);
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
      i+=j;
      j=fputs("*",file);
      return j<0?j:(i+j);
    case TYPECLASS_FLAT_TUPLE:
    case TYPECLASS_TUPLE:
      return fprintf(file,"tuple%"PRIi32,type->typeDataAs.composite->id);
    case TYPECLASS_PROCEDURE:
      return fprintf(file,"procPtr%"PRIi32,type->typeDataAs.procedure->id);
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
  ID_POINTER,
  ID_POINTER_OFFSET,
  ID_INTERMEDIATE_RESULT,
  ID_TMP_VAR,
  ID_TYPE,
}IdentiferType;
typedef struct{
  int32_t id;
  IdentiferType type;
}IdentiferInfo;
const char* const idNames []={[ID_LOCAL_VAR]="local variable",[ID_GLOBAL_VAR]="global variable",[ID_ARGUMENT]="procedure argument",
  [ID_PROCEDURE]="procedure",[ID_TUPLE]="(tuple element)",[ID_TUPLE_ELEMENT]="tuple element",[ID_POINTER]="pointer value",[ID_POINTER_OFFSET]="array element",
  [ID_INTERMEDIATE_RESULT]="intermediate result",[ID_TMP_VAR]="temporary variable",[ID_TYPE]="type"};
void printIdInfo(IdentiferInfo info,FILE* out){
  fprintf(out,"%s (%"PRIi32")",idNames[info.type],info.id);
}

typedef enum{
  BLOCK_PROCEDURE, 
  BLOCK_START,     // {
  BLOCK_IF,        // if( EXPR ){
  BLOCK_IF2,      // } else if(EXPR){
  BLOCK_ELSE,      // }else{
  BLOCK_WHILE,     // while( EXPR ){
  BLOCK_DO,        // do{
  BLOCK_WHILE_END, // }while( EXPR );
  BLOCK_END,       // }
}BlockType;
const char* const blockNames []={[BLOCK_PROCEDURE]="procedure",[BLOCK_START]="start",[BLOCK_IF]="if",
  [BLOCK_IF2]="_if",[BLOCK_ELSE]="else",[BLOCK_WHILE]="while",[BLOCK_DO]="do",[BLOCK_WHILE_END]="while end",[BLOCK_END]="end"};


typedef struct{
  OpType opType;
  DataType dataType;
  FilePosition filePos;
  union{
    int64_t i64;
    BinaryOperator binOp;
    UnaryOperator unOp;
    IdentiferInfo idInfo;
    BlockType block;
  }dataAs;
}Operation;

void printOperation(Operation op,FILE* out){
  fprintf(out,"%s ",opName(op.opType));
  if(op.opType!=OP_CODE_BLOCK&&op.opType!=ENTRY_POINT)
    printTypeName(&op.dataType,out);
  switch(op.opType){
    case OP_CONSTANT:
      fprintf(out," %"PRIi64"",op.dataAs.i64);
      break;
    case OP_STRING_CONST:
      fprintf(out," (%"PRIi64")",op.dataAs.i64);
      break;
    case OP_GET:
    case OP_DECLARE:
    case OP_PRE_DECLARE:
    case OP_DECLARE_PROCEDURE:
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
    default:
      //ignore remaining types
      break;
  }
  fputs("\n",out);
}

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
    return compositeType(TYPECLASS_TUPLE,stringElts,2);//ensure string-type exists
}
IntOrErrorCode addProgString(String s){
  if(progStringCount+1>=MAX_PROG_STRINGS)
    return (IntOrErrorCode){.isError=true,.as={.error=ERROR_MEMORY}};
  //TODO find duplicate strings
  programStrings[progStringCount]=(ProgramString){.value=s,.stringId=progStringCount,.charsId=-1,.charsOffset=-1};
  return (IntOrErrorCode){.isError=false,.as={.i64=progStringCount++}};
}
int progStringCmp(const void* a,const void* b){
  return -stringCompare(((const ProgramString*)a)->value,((const ProgramString*)b)->value);
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

#define COMPILE_OP_RETURN_ERROR(target, op,opSize)\
                r=compileOp(target,op+size,opSize-size);\
                if(r.isError)\
                  return r;\
                size+=r.as.size;\

SizeOrError compileOp(FILE* target,const Operation* op,size_t opSize){
  if(opSize<1)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=op->filePos}}};
  SizeOrError r;
  size_t size=1;
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
      break;
    case OP_CONSTANT:
      if(op->dataType.typeClass!=TYPECLASS_PRIMITIVE){
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
          fprintf(target,"((%s)%" PRIi64 ")",primitiveNameC(op->dataType.typeDataAs.primitive),op->dataAs.i64);
          break;
        default:
          fprintf(stderr,"%s constants are (currently) not supported",primitiveName(op->dataType.typeDataAs.primitive));
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
      }
      break;
    case OP_STRING_CONST:
      fprintf(target,"(string%" PRIi64")",op->dataAs.i64);
      break;
    case OP_CHECK_ARRAY_BOUNDS:
      fprintf(target,"%s(",CHECK_BOUNDS_NAME);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//index
      fputs(",",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);//length
      fputs(");\n",target);
      break;
    case OP_GET:
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
        case ID_INTERMEDIATE_RESULT:
          fprintf(target,"tmp%" PRIi32,op->dataAs.idInfo.id);
          break;
        case ID_LOCAL_VAR:
          fprintf(target,"local%" PRIi32,op->dataAs.idInfo.id);
          break;
        case ID_ARGUMENT:
          fprintf(target,"arg%" PRIi32,op->dataAs.idInfo.id);
          break;
        case ID_GLOBAL_VAR:
          fprintf(target,"global%" PRIi32,op->dataAs.idInfo.id);
          break;
        case ID_PROCEDURE:
          fprintf(target,"&(procedure%" PRIi32")",op->dataAs.idInfo.id);
          break;
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
          break;
        case ID_TUPLE_ELEMENT:
          fputs("tuple access without base tuple\n",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_POINTER:
          if(op->dataAs.idInfo.id==0){
            fputs("(*(",target);
            COMPILE_OP_RETURN_ERROR(target,op,opSize);
            fputs("))",target);
            break;
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
          break;
        case ID_POINTER_OFFSET:
          if(op->dataAs.idInfo.id==0){
            fputs("(*((",target);
            COMPILE_OP_RETURN_ERROR(target,op,opSize);
            fputs(")+(",target);
            COMPILE_OP_RETURN_ERROR(target,op,opSize);
            fputs(")))",target);
            break;
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
          break;
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
      break;
    case OP_PRE_DECLARE:
      printTypeNameC(&(op->dataType),target);
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
          fprintf(target," tmp%" PRIi32 ";\n",op->dataAs.idInfo.id);
          break;
        case ID_LOCAL_VAR:
          fprintf(target," local%" PRIi32 ";\n",op->dataAs.idInfo.id);
          break;
        case ID_GLOBAL_VAR:
          fprintf(target," global%" PRIi32 ";\n",op->dataAs.idInfo.id);
          break;
        case ID_INTERMEDIATE_RESULT:
        case ID_PROCEDURE:
        case ID_ARGUMENT:
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_TYPE:
          fprintf(stderr,"cannot pre-declare %s\n",idNames[op->dataAs.idInfo.type]);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      break;
    case OP_DECLARE:
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
          fputs("use OP_DECLARE_PROCEDURE to declare a procedure ",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_ARGUMENT:
          fputs("cannot declare arguments",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
          fputs("cannot declare tuple elements",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_POINTER:
        case ID_POINTER_OFFSET:
          fputs("cannot declare pointers",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_TYPE:
          fputs("cannot declare types",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(";\n",target);
      break;
    case OP_NEW:
      if(op->dataType.typeClass==TYPECLASS_TUPLE){
        fputs("(",target);
        printTypeNameC(&(op->dataType),target);
        fputs("){",target);
        for(int32_t e=0;e<op->dataType.typeDataAs.composite->typeCount;e++){
          if(e>0)
            fputs(",",target);
          fprintf(target,".e%"PRIi32"=",e);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
        }
        fputs("}",target);
      }
      break;
    case OP_CAST:
      fputs("((",target);
      printTypeNameC(&(op->dataType),target);
      fputs(")",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      break;
    case OP_ADDR_OF:
      fputs("&(",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      break;
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
      break;
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
      break;
    case OP_CODE_BLOCK:
      switch(op->dataAs.block){
        case BLOCK_PROCEDURE:
          fputs("block procedure should be eliminated at compile time",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case BLOCK_START:
          fputs("{\n",target);
          break;
        case BLOCK_IF:
        case BLOCK_IF2:
          fputs("if(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("){\n",target);
          break;
        case BLOCK_WHILE:
          fputs("if(!",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(")\n  break;\n",target);
          break;
        case BLOCK_DO:
          fputs("do{\n",target);
          break;
        case BLOCK_ELSE:
          fputs("}else{\n",target);
          break;
        case BLOCK_WHILE_END:
          fputs("}while(1);\n",target);
          break;
        case BLOCK_END:
          fputs("}\n",target);
          break;
      }
      break;
    case OP_DECLARE_PROCEDURE:{
      if(op->dataType.typeClass!=TYPECLASS_PROCEDURE)
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
      printTypeNameC(op->dataType.typeDataAs.procedure->outType,target);
      fprintf(target," procedure%" PRIi32" (",op->dataAs.idInfo.id);
      DataType* inType=op->dataType.typeDataAs.procedure->inType;
      if(inType->typeClass==TYPECLASS_FLAT_TUPLE){
        CompositeType* inTypes=inType->typeDataAs.composite;
        for(int32_t e=0;e<inTypes->typeCount;e++){
          if(e>0)
            fputs(", ",target);
          printTypeNameC(&(inTypes->types[e]),target);
          fprintf(target," arg%"PRIi32,e);
        } 
      }else if(inType->typeClass==TYPECLASS_PRIMITIVE&&inType->typeDataAs.primitive==PRIMITIVE_VOID){
        fputs("void",target);
      }else{
        printTypeNameC(inType,target);
        fputs(" arg0",target);
      }         
      fputs("){\n",target);
    }break;
    case OP_RETURN:
      fputs("return ",target);
      if(op->dataType.typeClass!=TYPECLASS_FLAT_TUPLE){
        if(op->dataType.typeClass==TYPECLASS_PRIMITIVE&&op->dataType.typeDataAs.primitive==PRIMITIVE_VOID){
          fputs(";\n",target);
          break;
        }
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(";\n",target);
        break;
      }
      fprintf(target,"(tuple%"PRIi32"){",op->dataType.typeDataAs.composite->id);
      for(int32_t e=0;e<op->dataType.typeDataAs.composite->typeCount;e++){
        if(e>0)
          fputs(",",target);
        fprintf(target,".e%"PRIi32"=",e);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
      }
      fputs("};\n",target);
      break;
    case ENTRY_POINT:
      fputs("int main(void){\n",target);
      break;
    case OP_CALL:
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
        case ID_INTERMEDIATE_RESULT:
          fprintf(target,"tmp%"PRIi32"(",op->dataAs.idInfo.id);
          break;
        case ID_LOCAL_VAR:
          fprintf(target,"local%"PRIi32"(",op->dataAs.idInfo.id);
          break;
        case ID_GLOBAL_VAR:
          fprintf(target,"global%"PRIi32"(",op->dataAs.idInfo.id);
          break;
        case ID_PROCEDURE:
          fprintf(target,"procedure%"PRIi32"(",op->dataAs.idInfo.id);
          break;
        case ID_ARGUMENT:
          fprintf(target,"arg%"PRIi32"(",op->dataAs.idInfo.id);
          break;
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
          fputs("calling tuple elements directly is not supported\n",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_POINTER:
          if(op->dataAs.idInfo.id>0){
            fputs("calling tuple elements directly is not supported\n",stderr);
            return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
          }
          fputs("(*(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("))(",target);
          break;
        case ID_POINTER_OFFSET:
          if(op->dataAs.idInfo.id>0){
            fputs("calling tuple elements directly is not supported\n",stderr);
            return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
          }
          fputs("(*((",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(")+",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("))(",target);
          break;
        case ID_TYPE:
          fputs("cannot call types",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      DataType* in=op->dataType.typeDataAs.procedure->inType;
      DataType* out=op->dataType.typeDataAs.procedure->outType;
      if(in->typeClass==TYPECLASS_FLAT_TUPLE){
        for(int32_t e=0;e<in->typeDataAs.composite->typeCount;e++){
          if(e>0)
            fputs(",",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
        }
      }else if(in->typeClass!=TYPECLASS_PRIMITIVE||in->typeDataAs.primitive!=PRIMITIVE_VOID){
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
      }
      if(out->typeClass==TYPECLASS_PRIMITIVE&&out->typeDataAs.primitive==PRIMITIVE_VOID){//function without return value terminates statement
        fputs(");\n",target);
        break;
      }
      fputs(")",target);
      break;
    default:
      fprintf(stderr,"operation %s is not implemented\n",opName(op->opType));
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=op->filePos}}};
  }
  return (SizeOrError){.isError=false,.as={.size=size}};
}

Error compileToC(FILE* target,const Operation* ops,size_t opCount,bool hasEntryPoint,bool hasCheckBounds){
  fputs("#include <stdlib.h>\n",target);
  fputs("#include <stdio.h>\n",target);
  fputs("#include <inttypes.h>\n",target);
  fputs("#include <string.h>\n",target);
  fputs("#include <stdbool.h>\n",target);
  //initialize strings
  DataType stringType=TYPE_UNDEFINED;
  if(progStringCount>0){
    stringType=progStringType();//ensure string-type exists
    initProgStringChars();//initialize characters
  }
  for(size_t i=0;i<procTypeCount;i++){
    if(procTypes[i].outType->typeClass==TYPECLASS_FLAT_TUPLE){//ensure flat-tuple return types are generated as tuples for code generation
      procTypes[i].outType->typeDataAs.composite->flags|=FLAG_IS_TUPLE;
    }
  }
  //declare composite types
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].flags&FLAG_IS_TUPLE){
      fprintf(target,"typedef struct tuple%"PRIi32"Impl tuple%"PRIi32";\n",i,i);
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
    if(compositeTypes[i].flags&FLAG_IS_TUPLE){
      fprintf(target,"struct tuple%"PRIi32"Impl{\n",i);
      for(int16_t e=0;e<compositeTypes[i].typeCount;e++){
        printTypeNameC(&(compositeTypes[i].types[e]),target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("};\n",target);
    }
  }
  //initialize strings
  for(size_t i=0;i<progStringCount;i++){
    if(programStrings[i].isBaseString){
      fprintf(target,"const %s stringChars%"PRIi32"[] = {",primitiveNameC(PRIMITIVE_I8),programStrings[i].charsId/*,programStrings[i].value.length*/);
      String str=programStrings[i].value;
      for(size_t j=0;j<str.length;j++){
        if(j>0)
          fputs(",",target);
        if(str.chars[j]<0)
          fprintf(target,"-0x%"PRIx8,-str.chars[j]);
        else
          fprintf(target,"0x%"PRIx8,str.chars[j]);
      }
      fputs("};\n",target);
    }
    fprintf(target,"const tuple%"PRIi32" string%"PRIi32" = {.e0=stringChars%"PRIi32"+%"PRIi32",.e1=%zu};\n",
      stringType.typeDataAs.tuple->id,programStrings[i].stringId,programStrings[i].charsId,programStrings[i].charsOffset,programStrings[i].value.length);
  }
  if(hasCheckBounds){
    fprintf(target,"void %s(int64_t index,int64_t length){\n",CHECK_BOUNDS_NAME);
    fputs("  if(index>=0 && index<length)\n    return;\n",target);
    fputs("  fprintf(stderr,\"array index out of bounds: %\"PRIi64\" size: %\"PRIi64\"\\n\",index,length);\n",target);
    fprintf(target,"  exit(%i);\n",PROG_EXIT_CODE_ARRAY_OUT_OF_RANGE);
    fputs("}\n",target);
  }
  if(!hasEntryPoint)//auto-wrap programs without entry point into a main function
    fputs("int main(void){\n",target);
  SizeOrError r;
  for(size_t p=0;p<opCount;){
    r=compileOp(target,ops+p,opCount-p);
    if(r.isError)
      return r.as.error;
    p+=r.as.size;
  }
  if(!hasEntryPoint){
    fputs("return 0;\n",target);
    fputs("}\n",target);
  }
  return (Error){.errorCode=0,.pos={0}};
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
  IdentiferType idType;
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
  int32_t hash=stringHash(name);
  ScopeNode** node=scope->nodes+(hash%SCOPE_MAP_CAP);
  while(*node!=NULL){
    if(stringCompare((*node)->key,name)==0)
      return node;
    node=&((*node)->next);
  }
  return node;
}
int declareIdentifier(String name,DataType type,IdentiferType idType,ScopeNode** out){
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
  char* code;
  size_t codeSize;
  FilePosition currentPos;
  FilePosition wordStart;
}CodeFile;

typedef struct{
  Operation* ops;
  size_t opCount;
  
  Scope* globalScope;
  int32_t predeclaredTypes;
  bool hasEntryPoint;
  bool hasCheckBounds;
}Program;

typedef struct{
  Scope* currentScope;
  int32_t currentProcId;
  int32_t procScope;
  int32_t scopeLevel;
  
  int32_t predeclaredTypes;
  int32_t opaqueTypeCount;
  bool hasEntryPoint;
}CompilerState;


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
  bool overflow=false;
  uint64_t maxSaveValue=negate?(INT64_MAX/base):-(INT64_MIN/base);
  for(;i<number.length;i++){
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
String readStringLiteral(CodeFile* codeFile,char end,bool doEspaceSeqs,int32_t* errorFlag){
  if(codeFile->codeSize<1){
    *errorFlag=ERROR_EOF;
    return (String){.chars=codeFile->code,.length=0};
  }
  //skip first char
  updateFilePosition(codeFile);
  char* wordChars=codeFile->code;
  size_t wordLength=0,delta=0;
  size_t sequenceLength;//length of escape sequence for multi-char sequences
  while(codeFile->codeSize>0&&*(codeFile->code)!=end){
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
    return readStringLiteral(codeFile,'"',true,&(wordType->errCode));
  }else if(*(codeFile->code)=='\''){
    if(wordType)
      wordType->wordType=WORD_TYPE_CHAR;
    return readStringLiteral(codeFile,'\'',true,&(wordType->errCode));
  }else if(codeFile->codeSize>=2&&*(codeFile->code)=='#'){
    if(*(codeFile->code+1)=='#'){//line comment
      readStringLiteral(codeFile,'\n',false,&(wordType->errCode));//ignore everything up to next new-line
      return (String){.chars=codeFile->code,.length=0};
    }else if(*(codeFile->code+1)=='+'){//inline comment
      readStringLiteral(codeFile,'#',false,&(wordType->errCode));//TODO end with '+#' not #
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

int readType(String name,CodeFile* codeFile,CompilerState* state);
//reads a composite type of the given type-class, the result is stored in the type buffer
//return 0 if a type was read, otherwise a nonzero error-code if a type error occurs this method will return a syntax error
int readCompositeType(TypeClass typeClass,CodeFile* codeFile,CompilerState* state,const char* endString,bool checkEmpty){
  String word;
  WordTypeOrErrCode wordType;
  int err;
  size_t initOffset=bufferedTypes;
  word=nextWord(codeFile,&wordType);
  while(!wordEquals(&word,endString)){
    if(wordType.errCode!=0)
      return wordType.errCode;
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER){
      fputs("type names have to be identifers\n",stderr);
      return ERROR_SYNTAX;
    }
    err=readType(word,codeFile,state);
    if(err!=0){
      bufferedTypes=initOffset;
      if(err==ERROR_TYPE){
        fprintf(stderr,"unknown type name %.*s \n",(int)word.length,word.chars);
        return ERROR_SYNTAX;
      }
      return err;
    }
    word=nextWord(codeFile,&wordType);
  }
  if(checkEmpty&&bufferedTypes==initOffset){
    fputs("empty composite type\n",stderr);
    return ERROR_SYNTAX;
  } 
  typeBuffer[initOffset]=compositeType(typeClass,typeBuffer+initOffset,bufferedTypes-initOffset);
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
    r=readCompositeType(TYPECLASS_FLAT_TUPLE,codeFile,state,"=>",false);
    if(r!=0)
      return r;
    r=readCompositeType(TYPECLASS_FLAT_TUPLE,codeFile,state,")",false);
    if(r!=0)
      return r;
    typeBuffer[initOffset]=procedureType(&(typeBuffer[initOffset]),&(typeBuffer[initOffset+1]));
    bufferedTypes--;
    return 0;
  }
  if(wordEquals(&name,"tuple(")||wordEquals(&name,"(")){
    return readCompositeType(TYPECLASS_TUPLE,codeFile,state,")",true);
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
  *typeOut=typeBuffer[--bufferedTypes];//TODO read multiple types
  if(typeEquals(typeOut,&TYPE_UNDEFINED))
    return ERROR_TYPE;
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
    (*op)=(Operation){.opType=OP_STRING_CONST,.dataType=progStringType(),.filePos=wordPos,.dataAs={.i64=strId.as.i64}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(wordType.wordType==WORD_TYPE_CHAR){
    if(word.length!=1){//TODO? handle Unicode characters
      fprintf(stderr,"character literal '%.*s' contains more that one character\n",(int)word.length,word.chars);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    }
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(PRIMITIVE_I8),.filePos=wordPos,.dataAs={.i64=word.chars[0]}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  IntOrErrorCode asInt=parseInt(word,0);//try to parse word as int
  if(!asInt.isError){
    bool isI32=asInt.as.i64<=INT32_MAX&&asInt.as.i64>=INT32_MIN;
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(isI32?PRIMITIVE_I32:PRIMITIVE_I64),.filePos=wordPos,.dataAs={.i64=asInt.as.i64}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(asInt.as.error!=ERROR_PARSE_INT)
    return (SizeOrError){.isError=true,.as={.error={.errorCode=asInt.as.error,.pos=wordPos}}};
  if(word.length==0)
    return (SizeOrError){.isError=false,.as={.size=0}};
  err=readType(word,codeFile,state);//try to parse word as type TODO allow use of declared type names
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
    if(wordType.errCode!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=wordType.errCode,.pos=wordPos}}};
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    if(type.typeClass==TYPECLASS_PROCEDURE){
      fputs("predeclaring procedures is not supported",stderr);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    }
    IdentiferType idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
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
    if(wordType.errCode!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=wordType.errCode,.pos=wordPos}}};
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    IdentiferType idType;
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
            
      (*op)=(Operation){.opType=OP_DECLARE_PROCEDURE,.dataType=type,.filePos=wordPos,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    }else{
      (*op)=(Operation){.opType=OP_DECLARE,.dataType=type,.filePos=wordPos,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    }
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"new")){
    err=requireCompileTimeType(&word,&type,1);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
    if(type.typeClass==TYPECLASS_TUPLE){
      (*op)=(Operation){.opType=OP_NEW,.dataType=type,.filePos=wordPos,.dataAs={.i64=0}};
      return (SizeOrError){.isError=false,.as={.size=1}};
    }
    printTypeName(&type,stderr);
    fputs(" is currently not supported for operator new\n",stderr);
    //TODO ? new pointer
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
  }
  if(bufferedTypes>0){
    fprintf(stderr,"%.*s does not take a type as argument\n",(int)word.length,word.chars);
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=wordPos}}};
  }
  if(wordEquals(&word,"true")){
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(PRIMITIVE_BOOL),.filePos=wordPos,.dataAs={.i64=true}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"false")){
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(PRIMITIVE_BOOL),.filePos=wordPos,.dataAs={.i64=false}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"+")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=ADD}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"-")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=SUBTRACT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"*")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=MULTIPLY}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"/")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=DIVIDE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"%")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=MOD}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"&")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=AND}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"|")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=OR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=XOR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"&&")){//XXX implement short-circuit  and/or using code-blocks
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"||")){
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_UNIMPLEMENTED,.pos=wordPos}}};
  }else if(wordEquals(&word,"==")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=EQ}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"!=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=NE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,">")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=GT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,">=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=GE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"<=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=LE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"<")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=LT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"neg")||wordEquals(&word,"negate")){
    (*op)=(Operation){.opType=OP_UNARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.unOp=NEGATE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"++")){
    (*op)=(Operation){.opType=OP_UNARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.unOp=INCREMENT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"--")){
    (*op)=(Operation){.opType=OP_UNARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.unOp=DECREMENT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"=::")){//automatically choose type of declared variable
    String varName=nextWord(codeFile,&wordType);
    if(wordType.errCode!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=wordType.errCode,.pos=wordPos}}};
    if(wordType.wordType!=WORD_TYPE_IDENTIFIER)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    IdentiferType idType=state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
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
  }else if(word.length>1&&word.chars[0]=='.'){
    word.chars++;//remove first character
    word.length--;
    IntOrErrorCode index=parseInt(word,10);
    if(index.isError)//XXX allow named elements
      return (SizeOrError){.isError=true,.as={.error={.errorCode=index.as.error,.pos=wordPos}}};
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"@")){
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_POINTER,.id=0}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"[]")){
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_POINTER_OFFSET,.id=0}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"addrOf")){
    (*op)=(Operation){.opType=OP_ADDR_OF,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"if")){
    Scope* newScope=openScope(BLOCK_IF);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    state->scopeLevel++;
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_IF}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"_if")){
    //no scope change for _if
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_IF2}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"while")){
    Scope* newScope=openScope(BLOCK_WHILE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_WHILE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"do")){//!!while syntax is different fro C:  WHILE cond DO exrp END   do-While: WHILE exrp cond DO END
    closeScope();
    Scope* newScope=openScope(BLOCK_WHILE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    //scope count does not change
        
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_DO}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"else")){
    closeScope();
    Scope* newScope=openScope(BLOCK_ELSE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    //scope count does not change
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_ELSE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"end")){
    closeScope();
    state->scopeLevel--;
    if(state->scopeLevel<state->procScope){//exited procedure
      state->currentProcId=-1;
      state->procScope=-1;
    }
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_END}};
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
    (*op)=(Operation){.opType=asIdentifier->idType==ID_PROCEDURE?OP_CALL:OP_GET,
      .dataType=asIdentifier->type,.filePos=wordPos,.dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  //TODO save identifiers as OP_IDENTIFIER
  //  remember global variable declarations
  //  if identifier matches global var def during type-check phase replace with global var
  
  
  //old parser code TODO OP_CALL_POINTER
  /*
  if(wordEquals(&word,"CALL")){
    String procName=nextWord(code,codeSize,NULL);
    if(procName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    ScopeNode* id;
    int r=getIdentifier(procName,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error=r}};
    //type has to be procedure or pointer to procedure
    if(id->type.typeClass!=TYPECLASS_PROCEDURE&&
      ((id->type.typeClass!=TYPECLASS_POINTER&&id->type.typeClass!=TYPECLASS_CONST_POINTER)||id->type.typeDataAs.type->typeClass!=TYPECLASS_PROCEDURE))
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    (*op)=(Operation){.opType=OP_CALL,.dataType=id->type,.dataAs={.idInfo={.type=id->idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else{
    fprintf(stderr,"unknown command: %.*s\n",(int)word.length,word.chars);
    return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
  }*/
  
  fprintf(stderr,"unknown command: %.*s\n",(int)word.length,word.chars);
  return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
}
Program compileToOps(CodeFile* codeFile){
  size_t opCount=0;
  size_t opsCap=256;
  SizeOrError r;
  Operation* compileOps=malloc(opsCap*sizeof(Operation));
  openScope(BLOCK_START);
  CompilerState state=(CompilerState){.currentProcId=-1,.procScope=0,.currentScope=scopeBuffer,.scopeLevel=0,.hasEntryPoint=false,.predeclaredTypes=0};
  while(codeFile->codeSize>0){
    r=readOperation(compileOps+opCount,codeFile,&state);
    if(r.isError){
      printError(r.as.error,stderr);
      return (Program){.ops=NULL,.opCount=0};//TODO better error value
    }
    opCount+=r.as.size;
    if(opCount>=opsCap-5){
      return (Program){.ops=NULL,.opCount=0};//TODO ensure there is enough capacity
    }
  }
  return (Program){.ops=compileOps,.opCount=opCount,.globalScope=scopeBuffer,.hasEntryPoint=state.hasEntryPoint,.predeclaredTypes=state.predeclaredTypes};
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
  if(!isPointerType(&(inTypes[0])))
    return TYPE_UNDEFINED;//inTypes[0] is no pointer
  if(isIntType(&(inTypes[1])))
    return inTypes[0];
  if(subtract&&typeEquals(&(inTypes[0]),&(inTypes[1]))){//XXX? ptr - const ptr
    return primitiveType(PRIMITIVE_I64);
  }
  return TYPE_UNDEFINED;
}
DataType typeCheckArithmetic(DataType* inTypes){
  if(inTypes[0].typeClass!=TYPECLASS_PRIMITIVE||inTypes[1].typeClass!=TYPECLASS_PRIMITIVE)
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
  if(inTypes[0].typeClass!=TYPECLASS_PRIMITIVE||inTypes[1].typeClass!=TYPECLASS_PRIMITIVE)
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
  if(inTypes[0].typeClass!=TYPECLASS_PRIMITIVE||inTypes[1].typeClass!=TYPECLASS_PRIMITIVE)
    return TYPE_UNDEFINED;//comparison only on primitive types
  if(!isInteger(inTypes[0].typeDataAs.primitive)||!isInteger(inTypes[1].typeDataAs.primitive))
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
  
  int32_t tmpCount;
  
  size_t index;
  int32_t nPredeclaredTypes;
  DataType* predeclaredTypes;
  bool hasCheckBounds;
}TypeCheckState;

//prints the type stack (for debug purposes)
void printTypeStack(TypeCheckState* state,FILE* out){
  size_t offset=0;
  for(size_t k=0;k<state->typeCount;k++){
    printTypeName(&(state->typeStack[k].type),out);
    fprintf(out," %"PRIi32":\n",state->typeStack[k].opCount);
    for(int32_t i=0;i<state->typeStack[k].opCount;i++){
      fputs("    ",out);//indent operations
      printOperation(state->opStack[offset++],out);
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
bool ensureOpCap(TypeCheckState* state,size_t newSize){
  void* mList=state->compiledOperations;//void* and operation* may have different size
  bool res=ensureCap(&mList,&(state->opCap),sizeof(Operation),newSize);
  state->compiledOperations=(Operation*)mList;
  return res;
}
bool ensureOpStackCap(TypeCheckState* state,size_t newSize){
  void* mList=state->opStack;
  bool res=ensureCap(&mList,&(state->opStackCap),sizeof(Operation),newSize);
  state->opStack=(Operation*)mList;
  return res;
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
  free(state->compiledOperations);//freeing NULL has no check for null necessary
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
//TODO more operation generator functions

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


Error checkIfTypes(TypeCheckState* state,IfBlockInfo* ifBlock,bool isElse/*else-branch or end of if-statement*/,FilePosition pos){
  if(ifBlock->elsePos==0&&state->typeCount>0){//first branch
    ifBlock->outStack.typeCount=state->typeCount;
    ifBlock->outStack.opCount=state->typeCount;
    ifBlock->outStack.types=malloc((state->typeCount)*sizeof(TypeInfo));
    ifBlock->outStack.ops=malloc((state->typeCount)*sizeof(Operation));
    if(ifBlock->outStack.types==NULL||ifBlock->outStack.ops==NULL)
      return (Error){.errorCode=ERROR_MEMORY,.pos=pos};
  }
  int32_t varId;
  if(isElse&&ifBlock->elsePos==0&&state->typeCount>0){//first branch
    if(ensureOpCap(state,state->opCount+2*state->typeCount+state->opStackCount))
      return (Error){.errorCode=ERROR_MEMORY,.pos=pos};
    for(int64_t i=state->typeCount-1;i>=0;i--){
      varId=state->tmpCount++;
      //save stack-elements to tmp-values
      state->compiledOperations[state->opCount++]=(Operation){.opType=OP_SET_VALUE,.dataType=state->typeStack[i].type,.filePos=pos,.dataAs={0}};
      state->compiledOperations[state->opCount++]=opGetTmpVar(&(state->typeStack[i].type),varId,pos);
      memcpy(state->compiledOperations+state->opCount,state->opStack+state->opStackCount-state->typeStack[i].opCount,state->typeStack[i].opCount*sizeof(Operation));
      state->opCount+=state->typeStack[i].opCount;
      state->opStackCount-=state->typeStack[i].opCount;
      state->typeCount--;
      
      ifBlock->outStack.types[i]=(TypeInfo){.opCount=1,.type=asConstType(state->typeStack[i].type)};
      ifBlock->outStack.ops[i]=opGetTmpVar(&(ifBlock->outStack.types[i].type),varId,pos);
    }
    return (Error){.errorCode=0,.pos=pos};
  }
  StackState* requiredState=(ifBlock->elsePos!=0)? &(ifBlock->outStack) : &(ifBlock->inStack);
  if(state->typeCount!=requiredState->typeCount){
    fprintf(stderr,"wrong number of types at end of if-branch expected %zu got %zu\n",requiredState->typeCount,state->typeCount);
    return (Error){.errorCode=ERROR_TYPE,.pos=pos};
  }
  if(ensureOpCap(state,state->opCount+2*state->typeCount+state->opStackCount))
    return (Error){.errorCode=ERROR_MEMORY,.pos=pos};
  for(int64_t i=state->typeCount-1;i>=0;i--){
    if(!typeEquals(&(state->typeStack[i].type),&(requiredState->types[i].type))){//XXX allow type-promotion
      fputs("wrong type at end of if-branch expected ",stderr);
      printTypeName(&(requiredState->types[i].type),stderr);
      fputs(" got ",stderr);
      printTypeName(&(state->typeStack[i].type),stderr);
      fputs("\n",stderr);
      return (Error){.errorCode=ERROR_TYPE,.pos=pos};
    }
    varId=ifBlock->elsePos!=0?ifBlock->outStack.ops[i].dataAs.idInfo.id:state->tmpCount++;
    //save stack-elements to tmp-values
    state->compiledOperations[state->opCount++]=(Operation){.opType=OP_SET_VALUE,.dataType=requiredState->types[i].type,.filePos=pos,.dataAs={0}};
    state->compiledOperations[state->opCount++]=opGetTmpVar(&(requiredState->types[i].type),varId,pos);
    memcpy(state->compiledOperations+state->opCount,state->opStack+state->opStackCount-state->typeStack[i].opCount,state->typeStack[i].opCount*sizeof(Operation));
    state->opCount+=state->typeStack[i].opCount;
    state->opStackCount-=state->typeStack[i].opCount;
    state->typeCount--;
    
    if(ifBlock->elsePos==0){
      ifBlock->outStack.types[i]=(TypeInfo){.opCount=1,.type=asConstType(state->typeStack[i].type)};
      ifBlock->outStack.ops[i]=opGetTmpVar(&(ifBlock->outStack.types[i].type),varId,pos);
    }
  }
  return (Error){.errorCode=0,.pos=pos};
}


Error compileCompositeOp(TypeCheckState* state,DataType* type,Operation* ops,size_t nOps,int32_t tmpId){
  if(nOps==0)
    return (Error){.errorCode=0,.pos={0}};
  if(ensureOpCap(state,state->opCount+nOps+1))
      return (Error){.errorCode=ERROR_MEMORY,.pos=ops[0].filePos};
  state->compiledOperations[state->opCount++]=opDeclareIntermediate(type,tmpId,ops[0].filePos);
  memcpy(state->compiledOperations+state->opCount,ops,nOps*sizeof(Operation));
  state->opCount+=nOps;
  return (Error){.errorCode=0,.pos=ops[0].filePos};
}
//ensures that none of the top type-count stack elements is a composite operation 
Error extractCompositeOpsOffset(TypeCheckState* state,size_t nStackValues,size_t skipValues){
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
    if(state->typeStack[i].opCount>1){
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


bool canAssign(DataType src,DataType target){
  if(typeEquals(&src,&target))
    return true;
  if(src.typeClass!=TYPECLASS_PRIMITIVE||target.typeClass!=TYPECLASS_PRIMITIVE)//XXX? assigning pointer to const pointer
    return false;
  
  return isInteger(src.typeDataAs.primitive)&&isInteger(target.typeDataAs.primitive)&&
    numberRank(src.typeDataAs.primitive)<=numberRank(target.typeDataAs.primitive);//implicit casts only from small int to large int
}
//TODO canCast (only number->number)

int requireTypes(const char* opName,TypeCheckState* state,DataType* types,size_t nTypes,FilePosition pos){
  if(state->typeCount<nTypes){
    fprintf(stderr,"not enough types of %s need %zu have %zu",opName,nTypes,state->typeCount);
    return ERROR_TYPE;
  }
  bool directMatch=true;
  int32_t nCasts=0;
  //check types
  for(size_t k=0;k<nTypes;k++){
    if(typeEquals(&(types[k]),&(state->typeStack[state->typeCount-nTypes+k].type)))
      continue;
    directMatch=false;
    if(canAssign(state->typeStack[state->typeCount-nTypes+k].type,types[k])){
      nCasts++;
      continue;
    }
    typeErrorMessage(opName,types[k],state->typeStack[state->typeCount-nTypes+k].type);
    return ERROR_TYPE;
  }
  if(directMatch)
    return 0;
  //extract composites
  Error r=extractCompositeOps(state,nTypes);
  if(r.errorCode!=0)
    return r.errorCode;
  //modify-types
  if(ensureOpStackCap(state,state->opStackCount+nCasts))
    return ERROR_MEMORY;
  size_t offset=state->opStackCount,shiftCount=0;
  state->opStackCount+=nCasts;//set opStackCount to new values
  for(size_t k=1;k<=nTypes;k++){
    offset-=state->typeStack[state->typeCount-k].opCount;
    shiftCount+=state->typeStack[state->typeCount-k].opCount;
    if(typeEquals(&(types[nTypes-k]),&(state->typeStack[state->typeCount-k].type)))
      continue;
    if(canAssign(state->typeStack[state->typeCount-k].type,types[nTypes-k])){
      memmove(state->opStack+offset+nCasts,state->opStack+offset,shiftCount*sizeof(Operation));
      shiftCount=0;
      nCasts--;
      state->opStack[offset+nCasts]=(Operation){.opType=OP_CAST,.filePos=pos,.dataType=types[nTypes-k],.dataAs={0}};
      state->typeStack[state->typeCount-k].type=types[nTypes-k];
      state->typeStack[state->typeCount-k].opCount++;
      continue;
    }
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
    return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
  memmove(state->opStack+state->opStackCount-totalOps+1,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
  state->opStack[state->opStackCount-totalOps]=op;
  state->opStackCount++;
  return (Error){.errorCode=0,.pos=op.filePos};;
}

//append the first stackOps operations from the stack to the program, remove types elements from the type-stack
//if appendOp is true op will be appended to the program (before any stack operations are appended)
//already allocate space for skippedStackOps 
Error addCompiledStackOps(TypeCheckState* state,Operation op,size_t skippedStackOps,size_t stackOps,size_t types,bool appendOp){
  if(ensureOpCap(state,state->opCount+stackOps+skippedStackOps+1))//already allocate space for skipped operations
      return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
  state->opStackCount-=stackOps;
  state->typeCount-=types;
  if(appendOp)
    state->compiledOperations[state->opCount++]=op;
  memcpy(state->compiledOperations+state->opCount,state->opStack+state->opStackCount,stackOps*sizeof(Operation));
  state->opCount+=stackOps;
  return (Error){.errorCode=0,.pos=op.filePos};
}
//append op and the first types operations from the stack to the program
Error addCompiledOp(TypeCheckState* state,Operation op,size_t types){
  return addCompiledStackOps(state,op,0,types,types,true);
}

Error typeCheckCall(Operation* op,TypeCheckState* state){
  DataType calledType=op->dataType;
  //TODO call of function pointer
  //  need check for value of pointer
  if(calledType.typeClass!=TYPECLASS_PROCEDURE&&
    ((!isPointerType(&calledType))||
      calledType.typeDataAs.type->typeClass!=TYPECLASS_PROCEDURE)){//not procedure or pointer to procedure 
    fputs("cannot call objects of type ",stderr);
    printTypeName(&calledType,stderr);
    fputs("\n",stderr);
    return (Error){.errorCode=ERROR_TYPE,.pos=op->filePos};
  }
  ProcedureType* procType=calledType.typeDataAs.procedure;
  size_t argCount=1;
  size_t totalOps=0;
  if(procType->inType->typeClass==TYPECLASS_PRIMITIVE&&procType->inType->typeDataAs.primitive==PRIMITIVE_VOID){//no arguments
    if(ensureOpCap(state,state->opCount+1))
      return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
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
  size_t offset=state->typeCount-argCount;
  
  if(argCount==1){//function takes single argument, the single argument will not be a flat tuple (tuples have >= 2 elements)
    int r=requireTypes("procedure argument",state,procType->inType,1,op->filePos);
    if(r!=0){
      return (Error){.errorCode=r,.pos=op->filePos};
    }
    totalOps=state->typeStack[offset].opCount;
  }
  if(argCount>1){//argument is flat tuple
    CompositeType* inTypes=procType->inType->typeDataAs.composite;
    int r=requireTypes("procedure argument",state,inTypes->types,inTypes->typeCount,op->filePos);
    if(r!=0){
      return (Error){.errorCode=r,.pos=op->filePos};
    }
    for(int32_t i=0;i<inTypes->typeCount;i++){
      totalOps+=state->typeStack[offset+i].opCount;
    }
  }
  Error r;
  DataType outType=*(procType->outType);
  if(outType.typeClass==TYPECLASS_PRIMITIVE&&outType.typeDataAs.primitive==PRIMITIVE_VOID){//no return values
    r=extractCompositeOps(state,argCount);
    if(r.errorCode!=0)
      return r;
    return addCompiledOp(state,*op,argCount);
  }
  r=extractCompositeOps(state,argCount);
  if(r.errorCode!=0)
    return r;
  //store result in temp variable
  if(ensureOpCap(state,state->opCount+1))
    return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
  int32_t tmpId=state->tmpCount++;
  state->compiledOperations[state->opCount++]=opDeclareIntermediate(procType->outType,tmpId,op->filePos);
  //update op-stack
  r=addCompiledOp(state,*op,argCount);
  if(r.errorCode!=0)
    return r;
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
  if(procType->typeClass!=TYPECLASS_PROCEDURE)
     return (Error){.errorCode=ERROR_TYPE,.pos=pos};
  DataType* inType=procType->typeDataAs.procedure->inType;
  if(inType->typeClass==TYPECLASS_PRIMITIVE&&inType->typeDataAs.primitive==PRIMITIVE_VOID)
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

//TODO only allow compile time code at global level (only constants and addresses)
//     run all global code before procedure implementations
Error typeCheckOperation(Operation op,TypeCheckState* state){
  size_t totalOps=0;
  int32_t offset,tmpId;
  Error r;
  BlockInfo blockInfo;
  switch(op.opType){
    case OP_CONSTANT:
    case OP_STRING_CONST:
      return pushValue(state,op);
    case OP_UNARY_OPERATOR:
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
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_PRIMITIVE||numberRank(state->typeStack[offset].type.typeDataAs.primitive)<0){
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
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_PRIMITIVE||state->typeStack[offset].type.typeDataAs.primitive!=PRIMITIVE_BOOL){
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
      if(ensureOpCap(state,state->opCount+3))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
      state->compiledOperations[state->opCount++]=opDeclareIntermediate(&op.dataType,tmpId,op.filePos);
      r=addCompiledOp(state,op,1);
      if(r.errorCode!=0)
        return r;
      //update stack
      return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
    case OP_BINARY_OPERATOR:
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
          // fall through
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
          if(state->typeStack[offset].type.typeClass==TYPECLASS_PRIMITIVE&&state->typeStack[offset].type.typeDataAs.primitive==PRIMITIVE_BOOL&&
              state->typeStack[offset+1].type.typeClass==TYPECLASS_PRIMITIVE&&state->typeStack[offset+1].type.typeDataAs.primitive==PRIMITIVE_BOOL){
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
          // fall through
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
      if(ensureOpCap(state,state->opCount+4))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
      state->compiledOperations[state->opCount++]=opDeclareIntermediate(&op.dataType,tmpId,op.filePos);
      r=addCompiledOp(state,op,2);
      if(r.errorCode!=0)
        return r;
      //update stack
      return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
    case OP_PRINT:
      if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      //can only print pointer or non-void primitive
      if(!isPointerType(&(state->typeStack[offset].type))&&
          (state->typeStack[offset].type.typeClass!=TYPECLASS_PRIMITIVE||state->typeStack[offset].type.typeDataAs.primitive==PRIMITIVE_VOID)){
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
      break;
    case OP_GET:
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
          op.dataType=asWritableType(tuple->types[op.dataAs.idInfo.id],true);
          if(state->opStack[state->opStackCount-state->typeStack[offset].opCount].opType==OP_GET&&(
              state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.type==ID_POINTER||
              state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.type==ID_POINTER_OFFSET||
              state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.type==ID_TUPLE)){
            if(ensureOpStackCap(state,state->opStackCount+1))
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            state->opStack[state->opStackCount-state->typeStack[offset].opCount].dataAs.idInfo.id++;
            state->opStack[state->opStackCount++]=op;
            state->typeStack[offset].type=op.dataType;
            state->typeStack[offset].opCount++;
            return (Error){.errorCode=0,.pos=op.filePos};
          }
          //wrap composite operations
          r=extractCompositeOps(state,1);
          if(r.errorCode!=0)
            return r;
          //update operation stack
          totalOps=state->typeStack[offset].opCount;
          if(ensureOpStackCap(state,state->opStackCount+totalOps+2))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          r=insertStackOperation(state,(Operation){.opType=OP_GET,.dataType=op.dataType,.dataAs={.idInfo={.type=ID_TUPLE,.id=1}}},totalOps);
          if(r.errorCode!=0)
            return r;
          state->opStack[state->opStackCount++]=op;
          //update type-stack
          state->typeStack[offset].type=op.dataType;
          state->typeStack[offset].opCount+=2;
          return (Error){.errorCode=0,.pos=op.filePos};
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
            r=addCompiledStackOps(state,opDeclareIntermediate(&indexType,indexId,op.filePos),state->typeStack[offset].opCount,state->typeStack[offset+1].opCount,1,true);
            if(r.errorCode!=0){
              return r;
            }      
            //XXX don't store constant values in intermediate
            r=addCompiledStackOps(state,opDeclareIntermediate(&arrayType,arrayId,op.filePos),0,state->typeStack[offset].opCount,1,true);
            if(r.errorCode!=0){
              return r;
            }      
            if(ensureOpCap(state,state->opCount+15))
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            size_t ptrIndex=state->tmpCount++,lenIndex=state->tmpCount++;
            state->compiledOperations[state->opCount++]=opDeclareIntermediate(&(arrayType.typeDataAs.composite->types[0]),ptrIndex,op.filePos);
            state->compiledOperations[state->opCount++]=(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[0],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE,.id=1}}};
            state->compiledOperations[state->opCount++]=opGetIntermediate(&arrayType,arrayId,op.filePos);//pointer
            state->compiledOperations[state->opCount++]=(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[0],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=0}}};
            state->compiledOperations[state->opCount++]=opDeclareIntermediate(&(arrayType.typeDataAs.composite->types[1]),lenIndex,op.filePos);
            state->compiledOperations[state->opCount++]=(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[1],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE,.id=1}}};
            state->compiledOperations[state->opCount++]=opGetIntermediate(&arrayType,arrayId,op.filePos);//length
            state->compiledOperations[state->opCount++]=(Operation){.opType=OP_GET,.dataType=arrayType.typeDataAs.composite->types[1],.filePos=op.filePos,
              .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=1}}};
            //2. check array-bounds
            state->hasCheckBounds=1;
            state->compiledOperations[state->opCount++]=(Operation){.opType=OP_CHECK_ARRAY_BOUNDS,.dataType=TYPE_UNDEFINED,.filePos=op.filePos,.dataAs={0}};
            state->compiledOperations[state->opCount++]=opGetIntermediate(&indexType,indexId,op.filePos);//index
            state->compiledOperations[state->opCount++]=opGetIntermediate(&(arrayType.typeDataAs.composite->types[1]),lenIndex,op.filePos);//length
            
            //3. array access XXX? keep array access on stack to allow chaining with tuple access
            op.dataType=*(arrayType.typeDataAs.composite->types[0].typeDataAs.type);//target-type of pointer in first element of tuple
            if(arrayType.typeDataAs.composite->types[0].typeClass!=TYPECLASS_CONST_POINTER)
              op.dataType=asWritableType(op.dataType,false);
            tmpId=state->tmpCount++;
            state->compiledOperations[state->opCount++]=opDeclareIntermediate(&op.dataType,tmpId,op.filePos);
            state->compiledOperations[state->opCount++]=op;//get pointer offset 
            state->compiledOperations[state->opCount++]=opGetIntermediate(&(arrayType.typeDataAs.composite->types[0]),ptrIndex,op.filePos);//pointer
            state->compiledOperations[state->opCount++]=opGetIntermediate(&indexType,indexId,op.filePos);
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
        case ID_TYPE:
          fputs("identifiers of type-names should not exist at this stage of compilation\n",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      break;
    case OP_SET_VALUE:
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
      r=addCompiledStackOps(state,op,state->typeStack[state->typeCount-2].opCount,state->typeStack[state->typeCount-1].opCount,1,true);
      if(r.errorCode!=0){
        return r;
      }      
      //check source type
      r=(Error){.errorCode=requireTypes("assignment",state,&op.dataType,1,op.filePos),.pos=op.filePos};
      if(r.errorCode!=0){
        return r;
      }
      return addCompiledStackOps(state,op,0,state->typeStack[state->typeCount-1].opCount,1,false);
    case OP_PRE_DECLARE:
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
          if(op.dataType.typeClass==TYPECLASS_UNDEFINED||(op.dataType.typeClass==TYPECLASS_PRIMITIVE&&op.dataType.typeDataAs.primitive==PRIMITIVE_VOID)){
            fputs("invalid type for predeclared variable: ",stderr);
            printTypeName(&(op.dataType),stderr);
            fputs("\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          return addCompiledOp(state,op,0);
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_PROCEDURE:
        case ID_INTERMEDIATE_RESULT:
        case ID_TMP_VAR:
        case ID_ARGUMENT:
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
          if(op.dataType.typeClass==TYPECLASS_PRIMITIVE&&op.dataType.typeDataAs.primitive==PRIMITIVE_VOID){
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
          if(op.dataType.typeClass==TYPECLASS_UNDEFINED){
            if(op.dataType.typeDataAs.typeId<=0||op.dataType.typeDataAs.typeId>state->nPredeclaredTypes)
                return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
            //TODO don't set constant variables to type writable
            state->predeclaredTypes[op.dataType.typeDataAs.typeId-1]=asWritableType(state->typeStack[offset].type,true);//set predeceased type
            op.dataType=asWritableType(state->typeStack[offset].type,true);
          }else{
            r=(Error){.errorCode=requireTypes("variable declaration",state,&op.dataType,1,op.filePos),.pos=op.filePos};
            if(r.errorCode!=0){
              return r;
            }
          }
          r=extractCompositeOps(state,1);
          if(r.errorCode!=0)
            return r;
          return addCompiledOp(state,op,1);
        case ID_TUPLE:
        case ID_TUPLE_ELEMENT:
        case ID_POINTER:
        case ID_POINTER_OFFSET:
        case ID_PROCEDURE:
        case ID_INTERMEDIATE_RESULT:
        case ID_TMP_VAR:
        case ID_ARGUMENT:
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
      if(op.dataType.typeClass==TYPECLASS_TUPLE){
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
        //store result in temp variable
        r=extractCompositeOps(state,op.dataType.typeDataAs.composite->typeCount);
        if(r.errorCode!=0)
          return r;
        tmpId=state->tmpCount++;
        if(ensureOpCap(state,state->opCount+totalOps+1))
          return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
        state->compiledOperations[state->opCount++]=opDeclareIntermediate(&op.dataType,tmpId,op.filePos);
        r=addCompiledOp(state,op,op.dataType.typeDataAs.composite->typeCount);
        if(r.errorCode!=0)
          return r;
        //update stack
        return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
      }
      break;
    case OP_CAST:
      break;
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
      if(ensureOpCap(state,state->opCount+state->typeStack[offset].opCount+1))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
      tmpId=state->tmpCount++;
      state->compiledOperations[state->opCount++]=opDeclareIntermediate(&op.dataType,tmpId,op.filePos);
      r=addCompiledStackOps(state,op,0,state->typeStack[offset].opCount,1,true);
      if(r.errorCode!=0)
        return r;
      //update stack
      return pushValue(state,opGetIntermediate(&op.dataType,tmpId,op.filePos));
    case OP_CODE_BLOCK://TODO? allow operations to cross while-block boundaries
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
          r=checkIfTypes(state,&(blockInfo.blockDataAs.ifBlock),true,op.filePos);
          if(r.errorCode!=0){
            return r;
          }
          //reset stack to in-types 
          if(resetStack(state,&(blockInfo.blockDataAs.ifBlock.inStack))){
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          }
          //push updated block
          blockInfo.type=BLOCK_ELSE;
          blockInfo.blockDataAs.ifBlock.elsePos=state->opCount;
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          if(ensureOpCap(state,state->opCount+1))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          state->compiledOperations[state->opCount++]=op;
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
          if(checkNonemptyStack(state,"unfinished local operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(pushBlock(state,(BlockInfo){.type=BLOCK_WHILE,.blockStart=state->opCount,.blockDataAs={0}}))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          if(ensureOpCap(state,state->opCount+1))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          state->compiledOperations[state->opCount++]=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=op.filePos,.dataAs={.block=BLOCK_DO}};
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
          blockInfo.blockDataAs.whileBlock.hasDo=true;
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
          op.dataType=primitiveType(PRIMITIVE_BOOL);
          op.dataAs.block=BLOCK_WHILE;
          r=(Error){.errorCode=requireTypes("if-condition",state,&op.dataType,1,op.filePos),.pos=op.filePos};
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
          if(checkNonemptyStack(state,"unfinished local operation")){//stack crossing block boundaries not implemented
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_WHILE_END:
          fputs("WHILE_END blocks are not supported use WHILE ... DO END to build a do-while statement",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
        case BLOCK_START:
          if(checkNonemptyStack(state,"unfinished local operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(pushBlock(state,(BlockInfo){.type=BLOCK_START,.blockStart=state->opCount,.blockDataAs={0}}))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          if(ensureOpCap(state,state->opCount+1))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          state->compiledOperations[state->opCount++]=op;
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_END:
          blockInfo=popBlock(state);
          if(blockInfo.type==BLOCK_END||blockInfo.type==BLOCK_IF2||(blockInfo.type==BLOCK_WHILE&&!blockInfo.blockDataAs.whileBlock.hasDo)){
            fputs("unexpected END statement\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(blockInfo.type==BLOCK_WHILE){
            op.dataAs.block=BLOCK_WHILE_END;
          }
          int32_t endCount=1;
          if(blockInfo.type==BLOCK_IF||blockInfo.type==BLOCK_ELSE){
            endCount+=blockInfo.blockDataAs.ifBlock.elifCount;
            r=checkIfTypes(state,&(blockInfo.blockDataAs.ifBlock),true,op.filePos);
            if(r.errorCode!=0){
              return r;
            }
            if(blockInfo.type==BLOCK_ELSE){//block ends if else-branch
              if(ensureOpCap(state,state->opCount+blockInfo.blockDataAs.ifBlock.outStack.typeCount))
                 return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
              memmove(state->compiledOperations+blockInfo.blockStart+blockInfo.blockDataAs.ifBlock.outStack.typeCount,state->compiledOperations+blockInfo.blockStart,
                (state->opCount-blockInfo.blockStart)*sizeof(Operation));
              for(size_t i=0;i<blockInfo.blockDataAs.ifBlock.outStack.typeCount;i++){
                state->compiledOperations[blockInfo.blockStart+i]=blockInfo.blockDataAs.ifBlock.outStack.ops[i];
                state->compiledOperations[blockInfo.blockStart+i].opType=OP_PRE_DECLARE;
              }
              state->opCount+=blockInfo.blockDataAs.ifBlock.outStack.typeCount;
            }else{
              size_t count=blockInfo.blockDataAs.ifBlock.outStack.typeCount+blockInfo.blockDataAs.ifBlock.inStack.opCount;
              if(ensureOpCap(state,state->opCount+count))
                 return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
              memmove(state->compiledOperations+blockInfo.blockStart+count,state->compiledOperations+blockInfo.blockStart,
                (state->opCount-blockInfo.blockStart)*sizeof(Operation));
              size_t opOffset=blockInfo.blockStart,inTypesOffset=0;
              for(size_t i=0;i<blockInfo.blockDataAs.ifBlock.outStack.typeCount;i++){
                state->compiledOperations[opOffset]=blockInfo.blockDataAs.ifBlock.outStack.ops[i];
                state->compiledOperations[opOffset].opType=OP_DECLARE;
                opOffset++;
                memcpy(state->compiledOperations+opOffset,blockInfo.blockDataAs.ifBlock.inStack.ops+inTypesOffset,
                  (blockInfo.blockDataAs.ifBlock.inStack.types[i].opCount)*sizeof(Operation));
                inTypesOffset+=blockInfo.blockDataAs.ifBlock.inStack.types[i].opCount;
              }
              state->opCount+=count;
            }
            if(resetStack(state,&(blockInfo.blockDataAs.ifBlock.outStack))){
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
            }
            //free values on op-stack
            free(blockInfo.blockDataAs.ifBlock.inStack.types);
            free(blockInfo.blockDataAs.ifBlock.inStack.ops);
            free(blockInfo.blockDataAs.ifBlock.outStack.types);
            free(blockInfo.blockDataAs.ifBlock.outStack.ops);
          }else{
            //TODO check for procedures with missing return statements
            if(checkNonemptyStack(state,"unfinished local operation")){
              return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
            }
          }
          if(ensureOpCap(state,state->opCount+endCount))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          while(endCount-->0){
            state->compiledOperations[state->opCount++]=op;
          }
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_PROCEDURE:
          fputs("blocks of type BLOCK_PROCEDURE are not supported, procedure blocks start with the DECLARE_PROCEDURE operation",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      break;
    case OP_CALL:
      return typeCheckCall(&op,state);
    case OP_RETURN:     
      if(op.dataType.typeClass==TYPECLASS_PRIMITIVE&&op.dataType.typeDataAs.primitive==PRIMITIVE_VOID){
          if(checkNonemptyStack(state,"unfinished operation at end of procedure"))
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
        if(ensureOpCap(state,state->opCount+state->typeStack[0].opCount+1))
          return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
        state->compiledOperations[state->opCount++]=op;
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
        fprintf(stderr,"wrong number of return values: expected %zu got %i\n",state->typeCount,op.dataType.typeDataAs.composite->typeCount);
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
    case OP_DECLARE_PROCEDURE:
    case ENTRY_POINT://start of procedure
      if(checkNonemptyStack(state,"unfinished global operation")){
        return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      if(pushBlock(state,(BlockInfo){.type=BLOCK_PROCEDURE,.blockStart=state->opCount,.blockDataAs={0}}))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
      if(ensureOpCap(state,state->opCount+1))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
      state->compiledOperations[state->opCount++]=op;
      if(op.opType==OP_DECLARE_PROCEDURE)
        return pushProcArgs(state,&op.dataType,op.filePos);
      return (Error){.errorCode=0,.pos=op.filePos};
  }
  printf("type checking %s is not implemented\n",opName(op.opType));
  return (Error){.errorCode=ERROR_UNIMPLEMENTED,.pos=op.filePos};
}
Error typeCheckProgram(Program* prog,CodeFile* src){
  size_t opCap=prog->opCount>INIT_CAP?prog->opCount:INIT_CAP;
  TypeCheckState state=(TypeCheckState){.compiledOperations=malloc(opCap*sizeof(Operation)),.opCap=opCap,.opCount=0,
    .opStack=malloc(INIT_CAP*sizeof(Operation)),.opStackCap=INIT_CAP,.opStackCount=0,
    .typeStack=malloc(INIT_CAP*sizeof(TypeInfo)),.typeStackCap=INIT_CAP,.typeCount=0,
    .openBlocks=malloc(INIT_CAP*sizeof(BlockInfo)),.blockCap=INIT_CAP,.blockCount=0,
    .predeclaredTypes=malloc(prog->predeclaredTypes*sizeof(DataType)),.nPredeclaredTypes=prog->predeclaredTypes,
    .tmpCount=0,.index=0};
  if(state.compiledOperations==NULL||state.opStack==NULL||state.typeStack==NULL||state.openBlocks==NULL||state.predeclaredTypes==NULL){//memory allocation failed
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
  prog->ops=state.compiledOperations;
  prog->opCount=state.opCount;
  prog->hasCheckBounds=state.hasCheckBounds;
  state.compiledOperations=NULL;
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
    printf("compiled to %zu operations\n",p.opCount);
    for(size_t i=0;i<p.opCount;i++){
      printOperation(p.ops[i],stdout);
    }
    puts("");
		//3. compile operations to C
    FILE* out=fopen(targetFile,"w");
    err=compileToC(out,p.ops,p.opCount,p.hasEntryPoint,p.hasCheckBounds);
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
