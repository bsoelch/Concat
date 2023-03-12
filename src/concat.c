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
bool filePosEquals(FilePosition p1,FilePosition p2){
  return p1.line==p2.line&&p1.posInLine==p2.posInLine&&strcmp(p1.fileName,p2.fileName)==0;
}
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
  OP_ADDR_OF_LABEL,

  OP_IDENTIFIER,
  OP_SET_IDENTIFIER,
  OP_IDENTIFIER_ADDRESS,

  OP_NEW,
  OP_CAST,
  OP_ADDR_OF,// (pointer to given value)
  OP_ADDR_OF_ARRAY,

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
  OP_UNREACHABLE,

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
    case OP_ADDR_OF_LABEL:return "OP_ADDR_OF_LABEL";
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
    case OP_ADDR_OF_ARRAY:return "OP_ADDR_OF_ARRAY";
    case OP_CHECK_ARRAY_BOUNDS:return "OP_CHECK_ARRAY_BOUNDS";
    case OP_CHECK_ENUM_INDEX:return "OP_CHECK_ENUM_INDEX";
    case OP_UNREACHABLE:return "OP_UNREACHABLE";
    case OP_MODIFY_STACK:return "OP_MODIFY_STACK";
    case OP_COMPILER_INFO:return "OP_COMPILER_INFO";
  }
  return "UNDEFINED";
}
char const* CHECK_BOUNDS_NAME="concatInternal_checkArrayBounds";
char const* CHECK_ENUM_INDEX_NAME="concatInternal_checkEnumIndex";
//labels
typedef int32_t NamespaceId;
const NamespaceId NAMESPACE_ID_NONE=-1;
typedef int32_t NamespaceImportId;
const NamespaceImportId NAMESPACE_IMPORT_NONE=-1;
typedef struct {
  NamespaceId current;
  NamespaceImportId namespaceImports;
}NamespaceInfo;

#define LABEL_CAP 4096
typedef int32_t LabelId;
const LabelId LABEL_ID_UNKNOWN=-1;
typedef int16_t LabelFlags;
#define LABEL_FLAG_MUTABLE  0x01
#define LABEL_FLAG_STATIC   0x02
#define LABEL_FLAG_EXTERN   0x04
#define LABEL_FLAG_PRIVATE  0x08
#define LABEL_FLAG_PUBLIC   0x10
typedef struct{
  String label;
  FilePosition declaredAt;
  NamespaceId namespace;
  LabelFlags flags;
}Label;
Label labelBuffer[LABEL_CAP];
int32_t labelBufferCount=0;
String getLabelName(LabelId labelId){
  if(labelId<0||labelId>=labelBufferCount)
    return EMPTY_STRING;
  return labelBuffer[labelId].label;
}
Label const* label(LabelId labelId,FilePosition pos){
  if(labelId<0||labelId>=labelBufferCount)
    handleError("label id out of range",ERROR_MEMORY,pos);
  return &labelBuffer[labelId];
}
LabelId newLabel(String label,LabelFlags flags,NamespaceId namespace,FilePosition declaredAt){
  if(labelBufferCount>=LABEL_CAP)
    handleError("exceeded label capacity",ERROR_MEMORY,declaredAt);
  labelBuffer[labelBufferCount]=(Label){.label=label,.flags=flags,.namespace=namespace,.declaredAt=declaredAt};
  return labelBufferCount++;
}
int32_t findLabel(LabelId labelOffset,int32_t labelCount,String const* labelName){
  for(int32_t i=0;i<labelCount;i++){
    if(stringCompare(getLabelName(labelOffset+i),*labelName)==0)
      return i;
  }
  return -1;
}
bool isMutableLabel(Label const* label){
  return (label->flags&LABEL_FLAG_MUTABLE)!=0;
}
bool isExternLabel(Label const* label){
  return (label->flags&LABEL_FLAG_EXTERN)!=0;
}
bool isPublicLabel(Label const* label){
  return (label->flags&LABEL_FLAG_PUBLIC)!=0;
}
bool isMutableLabelId(LabelId labelId){
  if(labelId<0||labelId>=labelBufferCount)
    return false;
  return isMutableLabel(&labelBuffer[labelId]);
}
bool isStaticLabelId(LabelId labelId){
  if(labelId<0||labelId>=labelBufferCount)
    return false;
  return (labelBuffer[labelId].flags&LABEL_FLAG_STATIC)!=0;
}
void printAsciifiedString(String name,FILE* out){
  for(size_t i=0;i<name.length;i++){
    if((charAt(name,i)>='0'&&charAt(name,i)<='9')||
       (charAt(name,i)>='A'&&charAt(name,i)<='Z')||
       (charAt(name,i)>='a'&&charAt(name,i)<='z')){//keep 0-9a-zA-Z
      fputc(charAt(name,i),out);
      continue;
    }
    //additional escape sequences:
    // _F -> start local filename
    // _T -> start of type
    // _N -> start of (type)name
    // _E -> end block-section
    if(charAt(name,i)=='_'){
      fputs("__",out);
      continue;
    }
    if(charAt(name,i)=='.'){
      fputs("_d",out);
      continue;
    }
    fprintf(out,"_X%02x",charAt(name,i)&0xff);
  }
}


//types
typedef enum{
  TYPECLASS_PRIMITIVE=0,
  TYPECLASS_TUPLE,
  TYPECLASS_PROC_IN,
  TYPECLASS_LABELED_PROC_IN,
  TYPECLASS_PROC_OUT,//behaves like tuple but will not be directly used
  TYPECLASS_PROCEDURE,
  TYPECLASS_NAMED_TYPE,
  TYPECLASS_AUTO_TYPE,
  TYPECLASS_GENERIC_TYPE,
  TYPECLASS_STRUCT,
  TYPECLASS_ENUM,
  TYPECLASS_ENUM_LABEL,
  TYPECLASS_NAMED_ENUM_LABEL,//named-type containing enum that should be interpreted as enum-label
  TYPECLASS_ARRAY,
  TYPECLASS_ARRAY_VIEW,
}TypeClass;

typedef enum{
  PRIMITIVE_UNDEFINED=0,
  PRIMITIVE_BOOL,
  PRIMITIVE_I8,
  PRIMITIVE_I32,
  PRIMITIVE_I64,
  PRIMITIVE_FLOAT,
  PRIMITIVE_TYPE,
}PrimitiveType;

typedef struct{
  union{
    int32_t id;
    PrimitiveType primitive;
  } dataAs;
  TypeClass class;
}TypeId;
bool typeEquals(TypeId a,TypeId b){
   if(a.class!=b.class)
     return false;
   if(a.class==TYPECLASS_PRIMITIVE)
     return a.dataAs.primitive==b.dataAs.primitive;
   return a.dataAs.id==b.dataAs.id;
}

const TypeId TYPE_TYPE;
int32_t autoTypeId(TypeId);
typedef struct{
  LabelId label;
  union{
    TypeId type;
    int64_t genericId;
  }as;
  TypeId type;
}StaticArgument;
int32_t staticArgIndex(StaticArgument const* arg){
  if(typeEquals(arg->type,TYPE_TYPE))
    return autoTypeId(arg->as.type);
  return arg->as.genericId;
}

typedef struct{
  TypeId target;
  bool isMutable;
}PointerType;
#define FLAG_IS_TUPLE      1
#define FLAG_IS_PROC_IN    2
#define FLAG_IS_PROC_OUT   4
#define FLAG_IS_STRUCT     8
#define FLAG_IS_ENUM       16
#define FLAG_VOID_ONLY     32
typedef struct{
  TypeId const* types;
  int32_t id;
  LabelId labelOffset;//offset in labelBuffer
  int16_t typeCount;
  int16_t flags;
}CompositeType;
typedef struct{
  TypeId inType;
  TypeId outType;
  StaticArgument* staticArgs;
  int32_t staticArgsCount;
  int32_t procId;
  bool pointerUsed;
}ProcedureType;
typedef struct{
  int64_t value;
  bool isInt;
}ArraySize;
typedef struct{
  TypeId base;
  ArraySize const* sizes;
  int32_t dims;
  int32_t id;
  bool fixedSize;
  bool sizeUsed;
  bool viewOnly;
  bool isMutable;
}ArrayType;
typedef struct{
  LabelId name;
  TypeId type;
}NamedType;

#define MAX_TYPES       2048
#define MAX_POINTERS    1024
#define MAX_COMPOSITE   1024
#define MAX_ARRAY_TYPES 1024
#define MAX_PROC_TYPES  1024
#define MAX_NAMED_TYPES 1024

const TypeId TYPE_UNDEFINED={.class=TYPECLASS_PRIMITIVE,.dataAs.primitive=PRIMITIVE_UNDEFINED};
const TypeId TYPE_TYPE={.class=TYPECLASS_PRIMITIVE,.dataAs.primitive=PRIMITIVE_TYPE};
const TypeId TYPE_BOOL={.class=TYPECLASS_PRIMITIVE,.dataAs.primitive=PRIMITIVE_BOOL};
const TypeId TYPE_CHAR={.class=TYPECLASS_PRIMITIVE,.dataAs.primitive=PRIMITIVE_I8};
const TypeId TYPE_I64={.class=TYPECLASS_PRIMITIVE,.dataAs.primitive=PRIMITIVE_I64};

CompositeType compositeTypes[MAX_COMPOSITE];
int32_t compositeTypeCount=0;
ProcedureType procTypes[MAX_PROC_TYPES];
int32_t procTypeCount=0;
ArrayType arrayTypes[MAX_ARRAY_TYPES];
int32_t arrayTypeCount=0;
NamedType namedTypes[MAX_NAMED_TYPES];
int32_t namedTypeCount=0;

TypeId declaredMultiTypes[MAX_TYPES];
int32_t declaredMultiTypeCount=0;
void declareMultiType(TypeId id){
  if(declaredMultiTypeCount>=MAX_TYPES){
    fputs("exceeded maximum number of allowed multi-types",stderr);
    exit(EXIT_FAILURE);
  }
  declaredMultiTypes[declaredMultiTypeCount++]=id;
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
    case PRIMITIVE_UNDEFINED:
    case PRIMITIVE_TYPE:
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
  return PRIMITIVE_UNDEFINED;
}
bool isInteger(PrimitiveType t){
  switch(t){
    case PRIMITIVE_I8:
    case PRIMITIVE_I32:
    case PRIMITIVE_I64:
      return true;
    case PRIMITIVE_UNDEFINED:
    case PRIMITIVE_TYPE:
    case PRIMITIVE_BOOL:
    case PRIMITIVE_FLOAT:
      return false;
  }
  return false;
}

bool isNamedType(TypeId type){
  return type.class==TYPECLASS_NAMED_TYPE||type.class==TYPECLASS_NAMED_ENUM_LABEL;
}
TypeId unwrapNamedType(TypeId type){
  if(type.class==TYPECLASS_NAMED_ENUM_LABEL){
    TypeId enumID=namedTypes[type.dataAs.id].type;
    if(enumID.class!=TYPECLASS_ENUM){
      fputs("type in named-label has to be enum",stderr);
      exit(EXIT_FAILURE);
    }
    enumID.class=TYPECLASS_ENUM_LABEL;
    return enumID;
  }
  if(isNamedType(type))
    return namedTypes[type.dataAs.id].type;
  return type;
}
bool isPrimitiveType(TypeId type){
  type=unwrapNamedType(type);
  return type.class==TYPECLASS_PRIMITIVE;
}
PrimitiveType primitiveTypeData(TypeId type){
  type=unwrapNamedType(type);
  if(isPrimitiveType(type))
    return type.dataAs.primitive;
  return PRIMITIVE_UNDEFINED;
}
bool isBoolType(TypeId type){
  type=unwrapNamedType(type);
  return isPrimitiveType(type)&&type.dataAs.primitive==PRIMITIVE_BOOL;
}
bool isIntType(TypeId type){
  type=unwrapNamedType(type);
  return isPrimitiveType(type)&&isInteger(type.dataAs.primitive);
}
bool isNumberType(TypeId type){
  type=unwrapNamedType(type);
  return isPrimitiveType(type)&&numberRank(type.dataAs.primitive)>-1;
}
bool isPointerType(TypeId type){
  type=unwrapNamedType(type);
  return type.class==TYPECLASS_ARRAY_VIEW;
}
bool isArrayType(TypeId type){
  type=unwrapNamedType(type);
  return type.class==TYPECLASS_ARRAY;
}
bool isTupleType(TypeId type){
  type=unwrapNamedType(type);
  return type.class==TYPECLASS_TUPLE||type.class==TYPECLASS_STRUCT;
}
bool isProcInType(TypeId type){
  return type.class==TYPECLASS_PROC_IN||type.class==TYPECLASS_LABELED_PROC_IN;
}
bool isProcOutType(TypeId type){
  return type.class==TYPECLASS_PROC_OUT;
}
bool isEnumType(TypeId type){
  type=unwrapNamedType(type);
  return type.class==TYPECLASS_ENUM;
}
bool isEnumLabelType(TypeId type){
  type=unwrapNamedType(type);
  return type.class==TYPECLASS_ENUM_LABEL;
}
bool isMultiValueType(TypeId type){
  return isTupleType(type)||isEnumType(type)||isArrayType(type);
}
bool isMutableType(TypeId type){
  type=unwrapNamedType(type);
  if(isPointerType(type)||isArrayType(type))
    return arrayTypes[type.dataAs.id].isMutable;
  return false;
}
bool isAutoType(TypeId type){
  return type.class==TYPECLASS_AUTO_TYPE;
}
bool isProcedureType(TypeId type){
  type=unwrapNamedType(type);
  return type.class==TYPECLASS_PROCEDURE;
}
bool isCallableType(TypeId type){
  type=unwrapNamedType(type);
  if(isPointerType(type))
    type=arrayTypes[type.dataAs.id].base;
  return isProcedureType(type);
}
bool isComposite(TypeId type){
  type=unwrapNamedType(type);
  switch(type.class){
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      return true;
    case TYPECLASS_PRIMITIVE:
    case TYPECLASS_PROCEDURE:
    case TYPECLASS_NAMED_TYPE:
    case TYPECLASS_NAMED_ENUM_LABEL:
    case TYPECLASS_AUTO_TYPE:
    case TYPECLASS_GENERIC_TYPE:
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:
      return false;
  }
  return false;
}
bool isSwitchableType(TypeId type){
  return isIntType(type)||isEnumLabelType(type);
}

bool typeElementsLabeled(TypeId type){
  type=unwrapNamedType(type);
  if(type.class==TYPECLASS_TUPLE||type.class==TYPECLASS_PROC_IN||type.class==TYPECLASS_PROC_OUT)
    return false;
  return isComposite(type)&&compositeTypes[type.dataAs.id].labelOffset!=LABEL_ID_UNKNOWN;
}

TypeId getBaseType(TypeId pointer){
  pointer=unwrapNamedType(pointer);
  if(isPointerType(pointer)||isArrayType(pointer))
    return arrayTypes[pointer.dataAs.id].base;
  return TYPE_UNDEFINED;
}

ProcedureType const* procTypeData(TypeId type){
  type=unwrapNamedType(type);
  if(isProcedureType(type))
    return &procTypes[type.dataAs.id];
  return NULL;
}
ArrayType const* arrayTypeData(TypeId type){
  type=unwrapNamedType(type);
  if(isPointerType(type)||isArrayType(type))
    return &arrayTypes[type.dataAs.id];
  return NULL;
}
CompositeType const* compositeTypeData(TypeId type){
  type=unwrapNamedType(type);
  if(isComposite(type))
    return &compositeTypes[type.dataAs.id];
  return NULL;
}
bool isEnumLabel(TypeId enumType,TypeId labelType){
  enumType=unwrapNamedType(enumType);
  labelType=unwrapNamedType(labelType);
  return isEnumType(enumType)&&isEnumLabelType(labelType)&&
    compositeTypes[enumType.dataAs.id].id==compositeTypes[labelType.dataAs.id].id;
}
TypeId const* getTypeElements(TypeId type){
  type=unwrapNamedType(type);
  if(isComposite(type))
    return compositeTypes[type.dataAs.id].types;
  return NULL;
}
LabelId getTypeElementLabel(TypeId type,int32_t labelId){
  type=unwrapNamedType(type);
  if(!typeElementsLabeled(type))
    return LABEL_ID_UNKNOWN;
  return compositeTypes[type.dataAs.id].labelOffset+labelId;
}
int64_t getTypeElementCount(TypeId type){
  type=unwrapNamedType(type);
  if(isComposite(type))
    return compositeTypes[type.dataAs.id].typeCount;
  return 0;
}
void printTypeName(TypeId id,FILE* file);
bool setNamedType(TypeId type,TypeId newValue){
  newValue=unwrapNamedType(newValue);
  if(!isNamedType(type))
    return false;
  if(!typeEquals(namedTypes[type.dataAs.id].type,TYPE_UNDEFINED))
    return false;
  namedTypes[type.dataAs.id].type=newValue;
  return true;
}

TypeId pointerType(TypeId target,bool mutable);
TypeId arrayType(bool isView,TypeId base, int32_t dims,ArraySize const* sizes,bool isMutable);
bool makeMutable(TypeId* t){
  if(isMutableType(*t))
    return true;
  switch(t->class){
    case TYPECLASS_ARRAY_VIEW:
      *t=arrayType(true,getBaseType(*t),arrayTypes[t->dataAs.id].dims,arrayTypes[t->dataAs.id].sizes,true);
      return true;
    case TYPECLASS_PRIMITIVE:
    case TYPECLASS_ARRAY://mutability of composite types controlled by their container
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_PROCEDURE:
    case TYPECLASS_NAMED_TYPE:
    case TYPECLASS_NAMED_ENUM_LABEL:
    case TYPECLASS_AUTO_TYPE:
    case TYPECLASS_GENERIC_TYPE:
    case TYPECLASS_ENUM_LABEL:
      return false;
  }
  return false;
}
void printTypeName(TypeId,FILE*);
bool changeEnumType(TypeId* anEnum,bool isLabel){
  TypeId enumType=unwrapNamedType(*anEnum);
  if(enumType.class!=TYPECLASS_ENUM&&enumType.class!=TYPECLASS_ENUM_LABEL)
    return true;
  if((enumType.class==TYPECLASS_ENUM_LABEL)==isLabel)
    return false;//nothing to change
  if(isNamedType(*anEnum)){
    anEnum->class=isLabel?TYPECLASS_NAMED_ENUM_LABEL:TYPECLASS_NAMED_TYPE;
    return false;
  }
  enumType.class=isLabel?TYPECLASS_ENUM_LABEL:TYPECLASS_ENUM;
  *anEnum=enumType;
  return false;
}

TypeId primitiveType(PrimitiveType id){
  return (TypeId){.class=TYPECLASS_PRIMITIVE,.dataAs.primitive=id};
}
TypeId newNamedType(LabelId label,TypeId content){
  TypeClass mClass=TYPECLASS_NAMED_TYPE;
  if(content.class==TYPECLASS_ENUM_LABEL){
    mClass=TYPECLASS_NAMED_ENUM_LABEL;
    content.class=TYPECLASS_ENUM;
  }
  if(namedTypeCount>=MAX_NAMED_TYPES){
    fputs("exceeded named type capacity\n",stderr);
    return TYPE_UNDEFINED;
  }
  namedTypes[namedTypeCount]=(NamedType){.name=label,.type=content};
  return (TypeId){.class=mClass,.dataAs.id=namedTypeCount++};
}
TypeId newAutoType(int32_t id){
  return (TypeId){.class=TYPECLASS_AUTO_TYPE,.dataAs.id=id};
}
TypeId newGenericType(int32_t id){
  return (TypeId){.class=TYPECLASS_GENERIC_TYPE,.dataAs.id=id};
}
int32_t autoTypeId(TypeId autoType){
  if(autoType.class!=TYPECLASS_AUTO_TYPE&&autoType.class!=TYPECLASS_GENERIC_TYPE)
    return -1;
  return autoType.dataAs.id;
}
TypeId pointerType(TypeId target,bool mutable){
  return arrayType(true,target,1,NULL,mutable);
}
int64_t indexOfTypeArray(TypeId const* base,size_t baseLen,TypeId const* child,size_t childLen){
  if(childLen>baseLen)
    return -1;
  bool isMatch;
  for(size_t off=0;off<=baseLen-childLen;off++){
    isMatch=true;
    for(size_t i=0;i<childLen;i++){
      if(!typeEquals(base[i+off],child[i])){
        isMatch=false;
        break;
      }
    }
    if(isMatch)
      return off;
  }
  return -1;
}
//TODO prevent use of incomplete types (generics/opauqe-types) as fields
TypeId compositeType(TypeClass typeClass,TypeId const* elements,LabelId labelOffset,int32_t eltCount){
  if(eltCount==0&&(typeClass!=TYPECLASS_PROC_IN)&&(typeClass!=TYPECLASS_LABELED_PROC_IN)&&(typeClass!=TYPECLASS_PROC_OUT)){
    return TYPE_UNDEFINED;//only procedure in/out can be empty composites
  }
  int16_t classFlag;
  switch(typeClass){
    case TYPECLASS_PROC_IN:
      classFlag=FLAG_IS_PROC_IN;
      break;
    case TYPECLASS_LABELED_PROC_IN:
      if(labelOffset==LABEL_ID_UNKNOWN)
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
      if(labelOffset==LABEL_ID_UNKNOWN)
        return TYPE_UNDEFINED;
      classFlag=FLAG_IS_STRUCT;
      break;
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      if(labelOffset==LABEL_ID_UNKNOWN)
        return TYPE_UNDEFINED;
      classFlag=FLAG_IS_ENUM;
      break;
    default:
      return TYPE_UNDEFINED;
  }
  bool isVoid=true;
  for(int32_t i=0;i<eltCount;i++){
    if(!typeEquals(elements[i],TYPE_UNDEFINED)){
      isVoid=false;
      break;
    }
  }
  if(isVoid)
    classFlag|=FLAG_VOID_ONLY;
  if(eltCount==0){//empty composite
    int32_t match=-1;
    for(int32_t i=0;i<compositeTypeCount;i++){
      if(compositeTypes[i].typeCount==0){
        match=i;
        break;
      }
    }
    if(match==-1){
      if(compositeTypeCount+1>=MAX_COMPOSITE)
        return TYPE_UNDEFINED;
      match=compositeTypeCount;
      compositeTypes[compositeTypeCount]=(CompositeType){.id=compositeTypeCount,.typeCount=0,.types=NULL,.labelOffset=labelOffset,.flags=classFlag};
      declareMultiType((TypeId){.class=typeClass,.dataAs.id=compositeTypeCount});
      compositeTypeCount++;
    }
    return (TypeId){.class=typeClass,.dataAs.id=match};
  }
  int64_t typeMatch=-1,matchIndex,typesIndex;
  for(int32_t i=0;i<compositeTypeCount;i++){
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
        return (TypeId){.class=typeClass,.dataAs.id=i};
      }
    }
  }
  if(compositeTypeCount+1>=MAX_COMPOSITE)
    return TYPE_UNDEFINED;
  TypeId const* types;
  if(typeMatch!=-1){
    types=compositeTypes[typeMatch].types+matchIndex;
  }else{
    TypeId* newTypes=malloc(eltCount*sizeof(TypeId));//will persist until program exits
    if(newTypes==NULL)
      return TYPE_UNDEFINED;
    memcpy(newTypes,elements,eltCount*sizeof(TypeId));
    types=newTypes;
  }
  compositeTypes[compositeTypeCount]=(CompositeType){.id=compositeTypeCount,.typeCount=eltCount,.types=types,.labelOffset=labelOffset,.flags=classFlag};
  declareMultiType((TypeId){.class=typeClass,.dataAs.id=compositeTypeCount});
  return (TypeId){.class=typeClass,.dataAs.id=(compositeTypeCount++)};
}
TypeId procedureType(TypeId inType,TypeId outType,StaticArgument* staticArgs,int32_t staticArgsCount){
  if(typeEquals(inType,TYPE_UNDEFINED)||typeEquals(outType,TYPE_UNDEFINED))
    return TYPE_UNDEFINED;
  for(int32_t i=0;i<procTypeCount;i++){
    if(typeEquals(procTypes[i].inType,inType)&&typeEquals(procTypes[i].outType,outType))
      return (TypeId){.class=TYPECLASS_PROCEDURE,.dataAs.id=i};
  }
  if(staticArgs!=NULL){
    StaticArgument* mArgs=malloc(staticArgsCount*sizeof(StaticArgument));
    memcpy(mArgs,staticArgs,staticArgsCount*sizeof(StaticArgument));
    staticArgs=mArgs;
  }
  procTypes[procTypeCount]=(ProcedureType){.procId=procTypeCount,.inType=inType,.outType=outType,.staticArgsCount=staticArgsCount,.staticArgs=staticArgs};
  return (TypeId){.class=TYPECLASS_PROCEDURE,.dataAs.id=procTypeCount++};
}
TypeId asUnlabeledProc(TypeId procType,FilePosition pos){
  if(!isCallableType(procType))
    handleError("expected a callable type",ERROR_TYPE,pos);
  bool isPtr=false;
  TypeId baseType=procType;
  if(isPointerType(procType)){
    procType=getBaseType(procType);
    isPtr=true;
  }
  ProcedureType const* proc=procTypeData(procType);
  if(!typeElementsLabeled(proc->inType))
    return baseType;
  //replaces labeled types with their canonical unlabeled version
  TypeId in=compositeType(TYPECLASS_PROC_IN,getTypeElements(proc->inType),LABEL_ID_UNKNOWN,getTypeElementCount(proc->inType));
  if(typeEquals(in,TYPE_UNDEFINED))
    handleError("unexpected error while allocating type",ERROR_MEMORY,pos);
  TypeId newProc=procedureType(in,proc->outType,proc->staticArgs,proc->staticArgsCount);
  return isPtr?pointerType(newProc,false):newProc;
}
TypeId arrayType(bool isView,TypeId base, int32_t dims,ArraySize const* sizes,bool isMutable){
  if(dims<0||((dims==0)&&!isView))
    return TYPE_UNDEFINED;
  if(typeEquals(base,TYPE_UNDEFINED))
    return TYPE_UNDEFINED;
  if(isProcedureType(base)){
    if(!isView)
      return TYPE_UNDEFINED;
    procTypes[base.dataAs.id].pointerUsed=true;
  }
  for(int32_t i=0;i<arrayTypeCount;i++){
    if((!typeEquals(arrayTypes[i].base,base))||arrayTypes[i].dims!=dims||arrayTypes[i].isMutable!=isMutable)
      continue;
    if(dims==0||arrayTypes[i].sizes==sizes){//same array or both NULL
      arrayTypes[i].viewOnly&=isView;
      return (TypeId){.class=isView?TYPECLASS_ARRAY_VIEW:TYPECLASS_ARRAY,.dataAs.id=i};
    }
    if(sizes==NULL||arrayTypes[i].sizes==NULL)
      continue;
    bool match=true;
    for(int32_t j=0;j<dims;j++){
      if((arrayTypes[i].sizes[j].isInt!=sizes[j].isInt)||(arrayTypes[i].sizes[j].value!=sizes[j].value)){
        match=false;
        break;
      }
    }
    if(match){
      arrayTypes[i].viewOnly&=isView;
      return (TypeId){.class=isView?TYPECLASS_ARRAY_VIEW:TYPECLASS_ARRAY,.dataAs.id=i};
    }
  }
  ArraySize* mSizes=NULL;
  if(dims>0&&sizes!=NULL){
    mSizes=malloc(dims*sizeof(*mSizes));
    if(mSizes==NULL)
      return TYPE_UNDEFINED;
    memcpy(mSizes,sizes,dims*sizeof(*mSizes));
  }
  arrayTypes[arrayTypeCount]=(ArrayType){.base=base,.dims=dims,.sizes=mSizes,.id=arrayTypeCount,.sizeUsed=false,
    .fixedSize=(dims==0)||(sizes!=NULL),.isMutable=isMutable,.viewOnly=isView};
  TypeId newArray=(TypeId){.class=isView?TYPECLASS_ARRAY_VIEW:TYPECLASS_ARRAY,.dataAs.id=arrayTypeCount++};
  declareMultiType(newArray);
  return newArray;
}

void printTypeName(TypeId,FILE*);
typedef enum{
  CONSTANT_NONE=0,
  CONSTANT_BOOL,
  CONSTANT_INT,
  CONSTANT_CHAR,
  CONSTANT_STRING,
  CONSTANT_TYPE,
  CONSTANT_WILDCARD,
  GENERIC_INT,
}ConstantType;
char const* constTypeName(ConstantType type){
  switch(type){
    case CONSTANT_NONE:return "none";
    case CONSTANT_BOOL:return "bool";
    case CONSTANT_INT:return "int";
    case CONSTANT_CHAR:return "char";
    case CONSTANT_STRING:return "string";
    case CONSTANT_TYPE:return "type";
    case CONSTANT_WILDCARD:return "_";
    case GENERIC_INT:return "generic int";
  }
  return "unknown type";
}
typedef struct{
  union{
    String  string;
    TypeId  type;
    int64_t charId;
    int64_t i64;
    bool    boolean;
  }as;
  TypeId valueType;
  ConstantType constType;
}ConstantValue;
void printConstValue(ConstantValue constant,FILE* file){
  switch(constant.constType){
    case CONSTANT_NONE:
      return;
    case CONSTANT_BOOL:
      fputs(constant.as.i64?"true":"false",file);
      return;
    case CONSTANT_INT:
      fprintf(file,"%"PRIi64,constant.as.i64);
      return;
    case CONSTANT_CHAR:
      fprintf(file,"'%c' (0x%"PRIx64")",(char)constant.as.charId&0xff,constant.as.charId);
      return;
    case CONSTANT_STRING:
      fprintf(file,"\"%"PRI_STR"\"",PRI_STR_ARGS(constant.as.string));
      return;
    case CONSTANT_TYPE:
      printTypeName(constant.as.type,file);
      return;
    case CONSTANT_WILDCARD:
      fputs("_",file);
      return;
    case GENERIC_INT:
      fprintf(file,"(%"PRIi64")",constant.as.i64);
      return;
  }
}

void resolveTypeGenerics(TypeId src,TypeId expect,StaticArgument* args,ConstantValue* values,int32_t count){
  src=unwrapNamedType(src);//type names not important for generics
  expect=unwrapNamedType(expect);
  if(expect.class==TYPECLASS_GENERIC_TYPE){
    for(int32_t i=0;i<count;i++){
      if(values[i].constType==CONSTANT_NONE&&typeEquals(args[i].type,TYPE_TYPE)&&typeEquals(args[i].as.type,expect)){
        values[i]=(ConstantValue){.constType=CONSTANT_TYPE,.valueType=TYPE_TYPE,.as.type=src};
        break;
      }
    }
    return;
  }
  if(isComposite(src)&&isComposite(expect)){
    if(getTypeElementCount(src)!=getTypeElementCount(expect))
      return;//incompatible types
    for(int32_t i=0;i<getTypeElementCount(src);i++){
      resolveTypeGenerics(compositeTypeData(src)->types[i],compositeTypeData(expect)->types[i],args,values,count);
    }
    return;
  }
  if(src.class!=expect.class){
    return;//not equivalent -> resolving not possible
  }
  switch(src.class){
    case TYPECLASS_PRIMITIVE:
    case TYPECLASS_AUTO_TYPE://XXX? handle auto-types
      return;
    case TYPECLASS_PROCEDURE:
      resolveTypeGenerics(procTypeData(src)->inType,procTypeData(expect)->inType,args,values,count);
      resolveTypeGenerics(procTypeData(src)->outType,procTypeData(expect)->outType,args,values,count);
      return;
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:
      resolveTypeGenerics(getBaseType(src),getBaseType(expect),args,values,count);
      if(arrayTypeData(src)->fixedSize&&arrayTypeData(expect)->fixedSize&&arrayTypeData(src)->dims==arrayTypeData(expect)->dims){//XXX support different numbers of dimensions
        for(int32_t d=0;d<arrayTypeData(src)->dims;d++){
          if(!arrayTypeData(expect)->sizes[d].isInt){
            for(int32_t i=0;i<count;i++){
              if(values[i].constType==CONSTANT_NONE&&isIntType(args[i].type)&&args[i].as.genericId==arrayTypeData(expect)->sizes[d].value){
                if(arrayTypeData(src)->sizes[d].isInt)
                  values[i]=(ConstantValue){.constType=CONSTANT_INT,.valueType=TYPE_I64,.as.i64=arrayTypeData(src)->sizes[d].value};
                else
                  values[i]=(ConstantValue){.constType=GENERIC_INT,.valueType=TYPE_I64,.as.i64=arrayTypeData(src)->sizes[d].value};
                break;
              }
            }
          }
        }
      }
      return;
    case TYPECLASS_NAMED_TYPE:
    case TYPECLASS_NAMED_ENUM_LABEL:
      resolveTypeGenerics(unwrapNamedType(src),unwrapNamedType(expect),args,values,count);
      return;
    case TYPECLASS_TUPLE:
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
    case TYPECLASS_GENERIC_TYPE:
      fputs("unreachable",stderr);
      exit(EXIT_FAILURE);//should have been covered by if-statements before switch
      return;
  }
}
TypeId replaceGenericTypes(TypeId type,StaticArgument* args,ConstantValue* values,int32_t count){//XXX prevent unnecessary recreation of types
  TypeId tmp;
  switch(type.class){
    case TYPECLASS_PRIMITIVE:
    case TYPECLASS_AUTO_TYPE:
      return type;
    case TYPECLASS_PROCEDURE:
      return procedureType(replaceGenericTypes(procTypes[type.dataAs.id].inType,args,values,count),
              replaceGenericTypes(procTypes[type.dataAs.id].outType,args,values,count),
              procTypes[type.dataAs.id].staticArgs,procTypes[type.dataAs.id].staticArgsCount);
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:;
      ArraySize* newSizes=NULL;
      if(arrayTypes[type.dataAs.id].fixedSize){
        newSizes=malloc(arrayTypes[type.dataAs.id].dims*sizeof(ArraySize));
        for(int32_t d=0;d<arrayTypes[type.dataAs.id].dims;d++){
          if(!arrayTypes[type.dataAs.id].sizes[d].isInt){
            bool match=false;
            for(int32_t i=0;i<count;i++){
              if(isIntType(args[i].type)&&args[i].as.genericId==arrayTypes[type.dataAs.id].sizes[d].value){
                newSizes[d]=(ArraySize){.isInt=true,.value=values[i].as.i64};
                match=true;
                break;
              }
            }
            if(match)
              continue;
          }
          newSizes[d]=arrayTypes[type.dataAs.id].sizes[d];
        }
      }
       tmp=arrayType(type.class==TYPECLASS_ARRAY_VIEW,replaceGenericTypes(arrayTypes[type.dataAs.id].base,args,values,count),
        arrayTypes[type.dataAs.id].dims,newSizes,arrayTypes[type.dataAs.id].isMutable);
      free(newSizes);
      return tmp;
    case TYPECLASS_NAMED_TYPE:
    case TYPECLASS_NAMED_ENUM_LABEL:
      tmp=replaceGenericTypes(namedTypes[type.dataAs.id].type,args,values,count);
      if(typeEquals(tmp,unwrapNamedType(type)))
        return type;
      return newNamedType(namedTypes[type.dataAs.id].name,tmp);
    case TYPECLASS_TUPLE:
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:;
      TypeId* newTypes=malloc(compositeTypes[type.dataAs.id].typeCount*sizeof(TypeId));
      for(int32_t i=0;i<compositeTypes[type.dataAs.id].typeCount;i++){
        newTypes[i]=replaceGenericTypes(compositeTypes[type.dataAs.id].types[i],args,values,count);
      }
      TypeId res=compositeType(type.class,newTypes,compositeTypes[type.dataAs.id].labelOffset,compositeTypes[type.dataAs.id].typeCount);
      free(newTypes);//compositeType(...) does not store the given type-array
      return res;
    case TYPECLASS_GENERIC_TYPE:
      for(int32_t i=0;i<count;i++){
        if(typeEquals(args[i].type,TYPE_TYPE)&&typeEquals(args[i].as.type,type))
          return values[i].as.type;
      }
      return type;
  }
  return type;
}

char const* typeClassName(TypeClass cls){
  switch(cls){
    case TYPECLASS_PRIMITIVE:
      return "primitive";
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
    case TYPECLASS_NAMED_TYPE:
      return "named type";
    case TYPECLASS_NAMED_ENUM_LABEL:
      return "named enum label";
    case TYPECLASS_AUTO_TYPE:
      return "auto";
    case TYPECLASS_GENERIC_TYPE:
      return "generic";
    case TYPECLASS_STRUCT:
      return "structure";
    case TYPECLASS_ENUM:
      return "enum";
    case TYPECLASS_ENUM_LABEL:
      return "enum label";
    case TYPECLASS_ARRAY:
      return "array";
    case TYPECLASS_ARRAY_VIEW:
      return "pointer";
  }
  fprintf(stderr,"unexpected type-class %i",cls);
  return "";
}
char const* primitiveName(PrimitiveType t){
  switch(t){
    case PRIMITIVE_UNDEFINED:
      return "undefined";
    case PRIMITIVE_TYPE:
      return "type";
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
void printTypeFlags(TypeId type,FILE* file){
  if(isMutableType(type))
    fputs(" mut",file);
}
void printTypeNameIntenal(TypeId type,FILE* file,bool noRecurse){
  String labelName;
  switch(type.class){
    case TYPECLASS_PRIMITIVE:
      fprintf(file,"%s",primitiveName(type.dataAs.primitive));
      printTypeFlags(type,file);
      return;
    case TYPECLASS_NAMED_TYPE:
    case TYPECLASS_NAMED_ENUM_LABEL:
      fprintf(file,"namedType \"%"PRI_STR"\"",PRI_STR_ARGS(getLabelName(namedTypes[type.dataAs.id].name)));
      if(!noRecurse){
        fputs(" [ ",file);
        printTypeNameIntenal(unwrapNamedType(type),file,true);
        fputs(" ]",file);
      }
      printTypeFlags(type,file);
      return;
    case TYPECLASS_AUTO_TYPE:
    case TYPECLASS_GENERIC_TYPE:
      fprintf(file,"%s (%"PRIi32")",typeClassName(type.class),type.dataAs.id);
      printTypeFlags(type,file);
      return;
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
    case TYPECLASS_ENUM:
    case TYPECLASS_ENUM_LABEL:
      if(!isProcInType(type)&&!isProcOutType(type)){
        fprintf(file,"%s (%"PRIi32")",typeClassName(type.class),type.dataAs.id);
        if(noRecurse){
          printTypeFlags(type,file);
          return;
        }
        fputs(" (",file);
      }
      for(int32_t e=0;e<getTypeElementCount(type);e++){
        if((isEnumType(type)||isEnumLabelType(type))&&typeEquals(getTypeElements(type)[e],TYPE_UNDEFINED)){
          labelName=getLabelName(getTypeElementLabel(type,e));
          //void-type in enum -> only print label
          fprintf(file," %"PRI_STR,PRI_STR_ARGS(labelName));
          continue;
        }
        fputs(" ",file);
        printTypeNameIntenal(getTypeElements(type)[e],file,true);//only one recursion level
        if(!typeElementsLabeled(type))
          continue;
        labelName=getLabelName(getTypeElementLabel(type,e));
        fprintf(file," : %"PRI_STR,PRI_STR_ARGS(labelName));
      }
      if(!isProcInType(type)&&!isProcOutType(type)){
        fputs(" )",file);
      }
      printTypeFlags(type,file);
      return;
    case TYPECLASS_PROCEDURE:
      fprintf(file,"%s (%"PRIi32")",typeClassName(type.class),type.dataAs.id);
      if(noRecurse){
        printTypeFlags(type,file);
        return;
      }
      fputs(" ( ",file);
      printTypeNameIntenal(procTypeData(type)->inType,file,true);
      fputs(" => ",file);
      printTypeNameIntenal(procTypeData(type)->outType,file,true);
      fputs(" )",file);
      printTypeFlags(type,file);
      return;
    case TYPECLASS_ARRAY:
    case TYPECLASS_ARRAY_VIEW:
      printTypeNameIntenal(getBaseType(type),file,noRecurse);
      for(int32_t i=0;i<arrayTypeData(type)->dims;i++){
        if(arrayTypeData(type)->fixedSize){
          if(arrayTypeData(type)->sizes[i].isInt){
            fprintf(file," %"PRIi64,arrayTypeData(type)->sizes[i].value);
            continue;
          }
          fprintf(file," generic(%"PRIi64")",arrayTypeData(type)->sizes[i].value);
          continue;
        }
        if(arrayTypeData(type)->dims>1)
          fputs(" _",file);
      }
      fprintf(file," %s",typeClassName(type.class));
      printTypeFlags(type,file);
      return;
  }
  fprintf(file,"unknown type-class %i",type.class);
}
void printTypeName(TypeId type,FILE* file){
  printTypeNameIntenal(type,file,false);
}

char const* primitiveNameC(PrimitiveType t){
  switch(t){
    case PRIMITIVE_UNDEFINED:
      return "void";
    case PRIMITIVE_TYPE:
      return "size_t";
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
void printTypeNameC(TypeId type,FILE* file){
  switch(type.class){
    case TYPECLASS_NAMED_TYPE:
    case TYPECLASS_NAMED_ENUM_LABEL:
      printTypeNameC(unwrapNamedType(type),file);
      return;
    case TYPECLASS_AUTO_TYPE:
      fputs("void",file);
      fputs("auto-types are not supported at compile type\n",stderr);
      return;
    case TYPECLASS_GENERIC_TYPE://generic pointer -> void*
      fputs("void",file);
      return;
    case TYPECLASS_PRIMITIVE:
      fprintf(file,"%s",primitiveNameC(type.dataAs.primitive));
      return;
    case TYPECLASS_ARRAY:
      fprintf(file,"array%"PRIi32,type.dataAs.id);
      return;
    case TYPECLASS_ARRAY_VIEW:
      printTypeNameC(getBaseType(type),file);
      if(isCallableType(type))
        return;
      if(!isMutableType(type))
        fputs(" const",file);
      fputs("*",file);
      return;
    case TYPECLASS_PROC_IN:
    case TYPECLASS_LABELED_PROC_IN:
    case TYPECLASS_PROC_OUT:
      if(getTypeElementCount(type)==0){
        fputs("void",file);
        return;
      }
      if(getTypeElementCount(type)==1){
        printTypeNameC(getTypeElements(type)[0],file);
        return;
      }
      fprintf(file,"tuple%"PRIi32,type.dataAs.id);
      return;
    case TYPECLASS_TUPLE:
    case TYPECLASS_STRUCT:
      fprintf(file,"tuple%"PRIi32,type.dataAs.id);
      return;
    case TYPECLASS_PROCEDURE:
      fprintf(file,"procPtr%"PRIi32,type.dataAs.id);
      return;
    case TYPECLASS_ENUM:
      fprintf(file,"enum%"PRIi32,type.dataAs.id);
      return;
    case TYPECLASS_ENUM_LABEL:
      fputs("int32_t",file);
      return;
  }
  fprintf(file,"unknown type-class %i\n",type.class);
}
void printUnsignedTypeNameC(TypeId type,FILE* file){
  if(!isIntType(type))
    printTypeNameC(type,file);
  switch(primitiveTypeData(type)){
    case PRIMITIVE_I8:
      fputs("uint8_t",file);
      return;
    case PRIMITIVE_I32:
      fputs("uint32_t",file);
      return;
    case PRIMITIVE_I64:
      fputs("uint64_t",file);
      return;
    default:
      break;
  }
  fprintf(file,"unknown type-class %i\n",type.class);
}
//operators
typedef enum{
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  UDIVIDE,
  MOD,
  UMOD,
  AND,
  OR,
  XOR,
  GT,
  GE,
  EQ,
  NE,
  LE,
  LT,
  LSHIFT,
  RSHIFT,
  URSHIFT,
}BinaryOperator;
char const* binOpName(BinaryOperator op){
  switch(op){
    case ADD:return "ADD";
    case SUBTRACT:return "SUBTRACT";
    case MULTIPLY:return "MULTIPLY";
    case DIVIDE:return "DIVIDE";
    case MOD:return "MOD";
    case UDIVIDE:return "UDIVIDE";
    case UMOD:return "UMOD";
    case AND:return "AND";
    case OR:return "OR";
    case XOR:return "XOR";
    case GT:return "GT";
    case GE:return "GE";
    case EQ:return "EQ";
    case NE:return "NE";
    case LE:return "LE";
    case LT:return "LT";
    case LSHIFT:return "LSHIFT";
    case RSHIFT:return "RSHIFT";
    case URSHIFT:return "URSHIFT";
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
  LabelId labelId;
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
}StackOperation;
char const* stackOpName(StackOperation op){
  switch(op){
    case STACK_OP_DUP:return "dup";
    case STACK_OP_OVER:return "over";
    case STACK_OP_DROP:return "drop";
    case STACK_OP_SWAP:return "swap";
  }
  return "unknown";
}
typedef struct{
  int32_t count;
  StackOperation op;
}StackModification;
typedef enum{
  COMPILERINFO_TYPES,
  COMPILERINFO_STACK,
}CompilerInfoType;
char const* compilerInfoName(CompilerInfoType op){
  switch(op){
    case COMPILERINFO_TYPES:return "types";
    case COMPILERINFO_STACK:return "stack";
  }
  return "unknown";
}
typedef struct{
  int32_t maxCount;
  CompilerInfoType infoType;
}CompilerInfo;

typedef struct{
  LabelId label;
  NamespaceInfo spaceInfo;
}LocalLabel;
typedef struct{
  OpType opType;
  TypeId dataType;
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
    TypeId sourceType;
  }dataAs;
}Operation;

void printOperation(Operation op,FILE* out){
  fprintf(out,"%s ",opName(op.opType));
  if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
    printTypeName(op.dataType,out);
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
    case OP_ADDR_OF:
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
    case OP_ADDR_OF_LABEL:
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
    case OP_MODIFY_STACK:
      fprintf(out,"%s %"PRIi32,stackOpName(op.dataAs.stackMod.op),op.dataAs.stackMod.count);
      break;
    case OP_COMPILER_INFO:
      fprintf(out,"%s %"PRIi32,compilerInfoName(op.dataAs.compilerInfo.infoType),op.dataAs.compilerInfo.maxCount);
      break;
    default:
      //ignore remaining types
      break;
  }
  fputs("\n",out);
}

//path of the main source file, mill be initialized in main()
String basePath={0};
String libPath={0};
bool quietMode=false;

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
String namespaceName(NamespaceId id){
  if(id==NAMESPACE_ID_NONE)
    return EMPTY_STRING;
  return namespaceBuffer[id].name;
}
NamespaceId parentNamespace(NamespaceId id){
  if(id==NAMESPACE_ID_NONE)
    return NAMESPACE_ID_NONE;
  return namespaceBuffer[id].parent;
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
    fprintf(stderr,"namespace \"%"PRI_STR"\" does not exist\n",PRI_STR_ARGS(label));
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
  TypeId type;
  ScopeNode* next;
  ConstantValue constValue;
  NamespaceId namespaceId;
  LabelId labelId;
  int32_t id;
  IdentifierType idType;
};
typedef struct{
  ScopeNode** nodes;

  BlockType scopeType;
  NamespaceImportId prevImports;
  size_t nodeBufferOffset;
}Scope;
ScopeNode scopeNodeBuffer [SCOPE_NODE_CAP];
size_t scopeNodeCount=0;
Scope scopeBuffer [SCOPE_CAP];
size_t localScopeCount=0;

typedef int32_t FileId;
const FileId FILE_ID_NONE=-1;
typedef struct{
  FilePosition includePos;
  FileId id;
}IncludedFile;
typedef struct{
  Scope globalScope;
  NamespaceInfo namespaceInfo;

  Operation* globalOps;
  size_t globalOpCount;
  size_t globalOpCap;
  Operation* localOps;
  size_t localOpCount;
  size_t localOpCap;
  IncludedFile* includes;
  size_t includeCount;
  size_t includeCap;

  int64_t entryPointIndex;

  String fileName;
  FileId id;
}ProgramFile;
typedef struct{
  ProgramFile* files;
  size_t filesCap;
  FileId fileCount;
  FileId currentFile;

  int32_t globalVars;
  int32_t localVars;

  int32_t currentProcId;

  FileId entryFile;
  int32_t autoTypes;
}ParserState;
NamespaceInfo* parserNamespace(ParserState const* state){
  return &state->files[state->currentFile].namespaceInfo;
}
Scope* getGlobalScopeParser(ParserState const* state){
  return &state->files[state->currentFile].globalScope;
}
bool hasEntryPointParser(ParserState const* state){
  return state->entryFile!=FILE_ID_NONE&&state->files[state->entryFile].entryPointIndex!=-1;
}
FilePosition getEntryPointPosParser(ParserState const* state){
  if(!hasEntryPointParser(state))
    return (FilePosition){0};
  ProgramFile* entryFile=&state->files[state->entryFile];
  return entryFile->localOps[entryFile->entryPointIndex].filePos;
}
void initEntryPointParser(ParserState const* state){
  ProgramFile* entryFile=&state->files[state->entryFile];
  entryFile->entryPointIndex=entryFile->localOpCount;
}


ScopeNode* allocScopeNode(void){
  if(scopeNodeCount+1>=SCOPE_NODE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of variables %i\n",SCOPE_NODE_CAP);
    return NULL;
  }
  return scopeNodeBuffer+(scopeNodeCount++);
}
void initScope(Scope* scope,BlockType scopeType,ParserState const* state){
  scope->nodes=calloc(SCOPE_MAP_CAP,sizeof(ScopeNode*));
  scope->nodeBufferOffset=scopeNodeCount;
  scope->scopeType=scopeType;
  scope->prevImports=parserNamespace(state)->namespaceImports;
}
bool openScope(BlockType scopeType,ParserState const* state){
  if(localScopeCount>=SCOPE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of nested scopes %i\n",SCOPE_CAP);
    return true;
  }
  initScope(&scopeBuffer[localScopeCount],scopeType,state);
  localScopeCount++;
  return false;
}
BlockType currentScope(void){
  if(localScopeCount<1)
    return BLOCK_UNKNOWN;
  return scopeBuffer[localScopeCount-1].scopeType;
}
bool closeScope(ParserState const* state){
  if(localScopeCount<1)
    return true;
  localScopeCount--;
  free(scopeBuffer[localScopeCount].nodes);
  parserNamespace(state)->namespaceImports=scopeBuffer[localScopeCount].prevImports;
  scopeNodeCount=scopeBuffer[localScopeCount].nodeBufferOffset;
  return false;
}
ScopeNode** findNode(Scope const* scope,String name,NamespaceId namespaceId){
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
typedef struct{
  Scope const* scope;
  size_t index;
  ScopeNode* n;
}ScopeIterator;
ScopeIterator getScopeIterator(Scope const* scope){
  return (ScopeIterator){.scope=scope,.index=0,.n=NULL};
}
ScopeNode* scopeItrNext(ScopeIterator* itr){
  if(itr==NULL)
    return NULL;
  while(itr->n==NULL){
    if(itr->index>=SCOPE_MAP_CAP)
      return NULL;
    itr->n=itr->scope->nodes[itr->index++];
  }
  ScopeNode* n=itr->n;
  itr->n=itr->n->next;
  return n;
}
void printIdentiferMatch(ScopeNode* asIdentifier,FilePosition pos){
  fputs("  - ",stdout);
  Label const* mLabel=label(asIdentifier->labelId,pos);
  if(isMutableLabel(mLabel))
    fputs("mutable ",stdout);
  printf("%s: ",idNames[asIdentifier->idType]);
  printTypeName(asIdentifier->type,stdout);
  if(asIdentifier->constValue.constType!=CONSTANT_NONE){
    fputs(" ",stdout);
    printConstValue(asIdentifier->constValue,stdout);
  }
  fputs("\n      at ",stdout);
  printFilePosition(mLabel->declaredAt,stdout);
  puts("");
}
int searchIdentifier(Scope const* globalScope,NamespaceInfo namespace,String name,ScopeNode** out,FilePosition pos,bool printMatches){
  int64_t dotIndex=lastIndexOfChar(name,'.');
  NamespaceId mNamespaceId=namespace.current;
  NamespaceId relativeSpace;
  if(mNamespaceId==NAMESPACE_ID_NONE){
    handleError("invalid value for namespace.current",ERROR_MEMORY,pos) ;
  }
  String path=dotIndex>0?sliceEnd(name,dotIndex):EMPTY_STRING;
  if(dotIndex>0){
    name=sliceStart(name,dotIndex+1);
  }
  int32_t level=localScopeCount-1;
  ScopeNode** node;
  *out=NULL;
  while(level>=0){//check local variables
    relativeSpace=findNamespace(mNamespaceId,path);
    if(relativeSpace!=NAMESPACE_ID_NONE){
      node=findNode(scopeBuffer+level,name,relativeSpace);//all non-global variables are in the same namespace
      if(node==NULL)
        handleError("could not find node",ERROR_MEMORY,pos) ;
      if(*node!=NULL){
        *out=*node;
        if(!printMatches)
          return 0;
        printIdentiferMatch(*out,pos);
      }
    }
    level--;
  }
  while(mNamespaceId!=NAMESPACE_ID_NONE){//check global variables in current namespace and all parent namespaces
    relativeSpace=findNamespace(mNamespaceId,path);
    if(relativeSpace!=NAMESPACE_ID_NONE){
      node=findNode(globalScope,name,relativeSpace);
      if(node==NULL)
        handleError("could not find node",ERROR_MEMORY,pos) ;
      if(*node!=NULL){
        *out=*node;
        if(!printMatches)
          return 0;
        printIdentiferMatch(*out,pos);
      }
    }
    mNamespaceId=namespaceBuffer[mNamespaceId].parent;
  }
  NamespaceImportId import=namespace.namespaceImports;
  NamespaceId match=NAMESPACE_ID_NONE;
  bool firstDuplicate=true;
  while(import!=NAMESPACE_IMPORT_NONE){//check all imports in reverse order
    relativeSpace=findNamespace(namespaceImportBuffer[import].imported,path);
    if(relativeSpace!=NAMESPACE_ID_NONE){
      node=findNode(globalScope,name,relativeSpace);
      if(node==NULL)
        handleError("could not find node",ERROR_MEMORY,pos) ;
      if(*node!=NULL){
        if(printMatches)
          printIdentiferMatch(*node,pos);
        if(match==NAMESPACE_ID_NONE){
          *out=*node;
          match=namespaceImportBuffer[import].imported;
        }else if(!printMatches){
          if(firstDuplicate){
            fprintf(stderr,"Warning:\nmultiple matches for identifier \"%"PRI_STR"\"\n",PRI_STR_ARGS(name));
            fprintf(stderr,"  - %s \"%"PRI_STR"\" at ",idNames[(*out)->idType],PRI_STR_ARGS((*out)->key));
            printFilePosition(label((*out)->labelId,pos)->declaredAt,stderr);
            fputs("\n",stderr);
            firstDuplicate=false;
          }
          fprintf(stderr,"  - %s \"%"PRI_STR"\" at ",idNames[(*node)->idType],PRI_STR_ARGS((*node)->key));
          printFilePosition(label((*node)->labelId,pos)->declaredAt,stderr);
          fputs("\n",stderr);
        }
      }
    }
    import=namespaceImportBuffer[import].parent;
  }
  if(!firstDuplicate)
    handleWarning(NULL,ERROR_SYNTAX,pos);
  if(match!=NAMESPACE_ID_NONE)
    return 0;//found matching namespace
  return ERROR_SYNTAX;
}
int getIdentifier(Scope const* globalScope,NamespaceInfo namespace,String name,ScopeNode** out,FilePosition pos){
  return searchIdentifier(globalScope,namespace,name,out,pos,false);
}

ScopeNode const* declareIdentifier(Scope* globalScope,NamespaceInfo namespace,LabelId labelId,TypeId type,IdentifierType idType,int32_t id,FilePosition pos,ConstantValue const* constValue){
  Label const* mLabel=label(labelId,pos);
  if(isMutableLabel(mLabel)){
    if(idType==ID_TYPE)
      handleError("type definitions cannot be mutable",ERROR_SYNTAX,mLabel->declaredAt);
    if(idType==ID_PROCEDURE)
      handleError("procedures cannot be mutable",ERROR_SYNTAX,mLabel->declaredAt);
  }
  if(charAt(mLabel->label,0)=='#')
    handleError("identifiers cannot start with '#'",ERROR_SYNTAX,pos);
  if(containsChar(mLabel->label,'.'))
    handleError("'.' is not allowed in declared identifiers",ERROR_SYNTAX,pos);
  Scope* currentScope=globalScope;
  if(localScopeCount>0){
    currentScope=scopeBuffer+(localScopeCount-1);
  }
  ScopeNode** node=findNode(currentScope,mLabel->label,namespace.current);
  if(node==NULL)
    handleError("unable to access scope node",ERROR_MEMORY,mLabel->declaredAt);
  if(*node!=NULL){
    fprintf(stderr,"re-declaration of %s \"%"PRI_STR"\"\n",idNames[idType],PRI_STR_ARGS(mLabel->label));
    fprintf(stderr,"previous declaration: %s \"%"PRI_STR"\" at ",idNames[(*node)->idType],PRI_STR_ARGS((*node)->key));
    printFilePosition(label((*node)->labelId,pos)->declaredAt,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_SYNTAX,mLabel->declaredAt);
  }
  ScopeNode* shaddow;
  getIdentifier(globalScope,namespace,mLabel->label,&shaddow,pos);
  if(shaddow!=NULL){
    fprintf(stderr,"Warning:\n  declaration of %s \"%"PRI_STR"\"\n",idNames[idType],PRI_STR_ARGS(mLabel->label));
    fprintf(stderr,"  shadows previous declaration: %s \"%"PRI_STR"\" at ",idNames[shaddow->idType],PRI_STR_ARGS(shaddow->key));
    printFilePosition(label(shaddow->labelId,pos)->declaredAt,stderr);
    fputs("\n",stderr);
    handleWarning(NULL,ERROR_SYNTAX,mLabel->declaredAt);
  }
  *node=allocScopeNode();
  if(*node==NULL)
    handleError("unable to allocate scope node",ERROR_MEMORY,mLabel->declaredAt);
  (*node)->key=mLabel->label;
  (*node)->namespaceId=namespace.current;
  (*node)->type=type;
  (*node)->idType=idType;
  (*node)->id=id;
  (*node)->labelId=labelId;
  if(constValue!=NULL&&!isMutableLabelId(labelId)){
    (*node)->constValue=*constValue;
    (*node)->constValue.valueType=type;
  }else{
    (*node)->constValue=(ConstantValue){.constType=CONSTANT_NONE,.valueType=TYPE_UNDEFINED,.as.i64=0};
  }
  (*node)->next=NULL;
  return *node;
}

bool includeIfGlobal(ScopeNode* aNode,bool onlyConst,Scope* globalScope,FilePosition pos){
  Label const* mLabel=label(aNode->labelId,pos);
  ScopeNode** mNode;
  if(onlyConst&&aNode->constValue.constType==CONSTANT_NONE)
    return false;
  if(!isPublicLabel(mLabel)&&!isExternLabel(mLabel))
      return false;
  mNode=findNode(globalScope,mLabel->label,aNode->namespaceId);
  if(mNode==NULL){
    fputs("unable to access scope node\n",stderr);
    return true;
  }
  if(*mNode!=NULL){
    if(!isPublicLabel(mLabel)&&!(isPublicLabel(label((*mNode)->labelId,pos))||isExternLabel(label((*mNode)->labelId,pos))))
      return false;//extern variable shadowed by local variable
    if(filePosEquals(label((*mNode)->labelId,pos)->declaredAt,label(aNode->labelId,pos)->declaredAt))
      return false;//identical node
    fprintf(stderr,"conflicting definitions for identifier \"%"PRI_STR"\":\n",PRI_STR_ARGS(mLabel->label));
    fprintf(stderr,"  %s \"%"PRI_STR"\" at ",idNames[(*mNode)->idType],PRI_STR_ARGS((*mNode)->key));
    printFilePosition(label((*mNode)->labelId,pos)->declaredAt,stderr);
    fputs("\n",stderr);
    fprintf(stderr,"  %s \"%"PRI_STR"\" at ",idNames[aNode->idType],PRI_STR_ARGS(mLabel->label));
    printFilePosition(mLabel->declaredAt,stderr);
    fputs("\n",stderr);
    return true;
  }
  *mNode=allocScopeNode();
  memcpy(*mNode,aNode,sizeof(ScopeNode));
  return false;
}
bool includeConstants(Scope* globalScope,Scope* src,FilePosition pos){
  ScopeIterator itr=getScopeIterator(src);
  ScopeNode* n;
  while((n=scopeItrNext(&itr))!=NULL){
    if(includeIfGlobal(n,true,globalScope,pos))
      return true;
  }
  return false;
}
bool includeGlobals(Scope* globalScope,Scope* src,FilePosition pos){
  ScopeIterator itr=getScopeIterator(src);
  ScopeNode* n;
  while((n=scopeItrNext(&itr))!=NULL){
    if(includeIfGlobal(n,false,globalScope,pos))
      return true;
  }
  return false;
}

typedef struct{
  ProgramFile* files;
  FileId fileCount;

  int32_t nAutoTypes;
  TypeId*  autoTypes;
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
    handleError("invalid depth for tuple element access",ERROR_MEMORY,op->filePos);
  size_t size=0;
  for(int32_t i=0;i<depth;i++){
    if(((op+size)->opType!=OP_GET&&(op+size)->opType!=OP_SET&&(op+size)->opType!=OP_ADDR_OF)||(op+size)->dataAs.idInfo.type!=ID_TUPLE_ELEMENT){
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

void printGlobalIdentifer(Label const* mName,FILE* target){
  fputs("concat_",target);
  if(!(isPublicLabel(mName)||isExternLabel(mName))){
    fputs("_F",target);
    printAsciifiedString(cstrToStr(mName->declaredAt.fileName+basePath.length),target);
    fputs("_E",target);
  }
  NamespaceId space=mName->namespace;
  char separator='.';
  while(space!=NAMESPACE_ID_NONE&&space>0){
    printAsciifiedString(namespaceName(space),target);
    printAsciifiedString(newString(&separator,1),target);
    space=parentNamespace(space);
  }
  printAsciifiedString(mName->label,target);
}
void printProcArgumentTypesC(ProcedureType const* proc,FILE* target,bool printArgNames){
  if(!isProcInType(proc->inType)){
    fprintf(stderr,"unexpected procedure argument type-class: %s\n",typeClassName(proc->inType.class));
    exit(EXIT_FAILURE);
  }
  CompositeType const* inTypes=compositeTypeData(proc->inType);
  if(inTypes->typeCount==0)
    fputs("void",target);
  for(int32_t e=0;e<inTypes->typeCount;e++){
    if(e>0)
      fputs(", ",target);
    printTypeNameC(inTypes->types[e],target);
    if(printArgNames)
      fprintf(target," arg%"PRIi32,e);
  }
}
void printProcedureSignatureC(ProcedureType const* procedure,Label const* procLabel,FILE* target,bool printArgNames){
  printTypeNameC(procedure->outType,target);
  fputs(" ",target);
  printGlobalIdentifer(procLabel,target);
  fputs(" (",target);
  printProcArgumentTypesC(procedure,target,printArgNames);
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
    case ID_PROCEDURE:
      printGlobalIdentifer(label(op->dataAs.idInfo.labelId,op->filePos),target);
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
      if(compositeTypeData(op->dataType)->flags&FLAG_VOID_ONLY){
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
      if(arrayTypeData(op->dataType)->dims==0){//0D array access
        fputs("(*(",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs("))",target);
        return size+tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,false);
      }
      fputs("((",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(")",target);
      if(!isPointerType(op->dataType))
        fputs(".data",target);
      for(int32_t i=0;i<arrayTypeData(op->dataType)->dims;i++){
        fputs("[",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs("]",target);
      }
      fputs(")",target);
      return size+tupleElementAccess(target,op->dataAs.idInfo.id,op+size,opSize-size,false);
    case ID_ARRAY_SIZE:
      if(!arrayTypeData(op->dataType)->fixedSize){
        handleError("array-type does not have a known size",ERROR_SYNTAX,op->filePos);
        break;
      }
      fprintf(target,"arraySizes%"PRIi32,arrayTypeData(op->dataType)->id);
      return size;
    case ID_TYPE:
      handleError("type information is not accessible at runtime",ERROR_SYNTAX,op->filePos);
      break;
  }
  fputs("unexpected operation for get value:\n",stderr);
  printOperation(*op,stderr);
  handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
  return size;
}
size_t compileProcArgs(FILE* target,size_t compiledOps,Operation const* op,size_t size,size_t opSize,bool isGlobal){
  TypeId in=procTypeData(op->dataType)->inType;
  TypeId out=procTypeData(op->dataType)->outType;
  if(!isProcInType(in)){
    fprintf(stderr,"unexpected procedure argument type-class: %s\n",typeClassName(in.class));
    handleError(NULL,ERROR_MEMORY,op->filePos);
  }
  if(!isProcOutType(out)){
    fprintf(stderr,"unexpected procedure return type-class: %s\n",typeClassName(out.class));
    handleError(NULL,ERROR_MEMORY,op->filePos);
  }
  fputs("(",target);
  for(int32_t e=0;e<getTypeElementCount(in);e++){
    if(e>0)
      fputs(",",target);
    COMPILE_OP_RETURN_ERROR(target,op,opSize);
  }
  fputs(")",target);
  if(getTypeElementCount(out)==0)//function without return value terminates statement
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
      switch(op->dataType.class){
        case TYPECLASS_PRIMITIVE:
          switch(op->dataType.dataAs.primitive){
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
              fprintf(stderr,"printing primitive Type %s is not supported\n",primitiveName(op->dataType.dataAs.primitive));
              handleError(NULL,ERROR_TYPE,op->filePos);
          }
          break;
        case TYPECLASS_ARRAY_VIEW:
          fputs("p",target);
          break;
        default:
          fputs("printing values of type ",stderr);
          printTypeName(op->dataType,stderr);
          fputs(" is (currently) not supported\n",stderr);
          handleError(NULL,ERROR_TYPE,op->filePos);
      }
      fputs("\\n\",",target);
      if(isPointerType(op->dataType)){
        fputs("(void const*)",target);
      }
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      if(boolMode){
        fputs("?\"true\":\"false\"",target);
      }
      fputs(");\n",target);
      return size;
    case OP_CONSTANT:
      if(needCast){
        fputs("((",target);
        printTypeNameC(op->dataType,target);
        fputs(")",target);
      }
      if(isEnumLabelType(op->dataType)){
        fprintf(target,"%" PRIi64,op->dataAs.i64);
        if(needCast)
          fputs(")",target);
        return size;
      }
      if(isPointerType(op->dataType)){
        int64_t i=-1;
        for(size_t j=0;j<progStringCount;j++){//find string in reordered string array
          if(programStrings[j].stringId==op->dataAs.i64){
            i=j;
            break;
          }
        }
        fprintf(target,"(arrayData%"PRIi32"+%"PRIi32")",programStrings[i].charsId,programStrings[i].charsOffset);
        if(needCast)
          fputs(")",target);
        return size;
      }
      if(!isPrimitiveType(op->dataType)){
          fputs("constants of non-primitive type ",stderr);
          printTypeName(op->dataType,stderr);
          fputs(" are not supported\n",stderr);
          handleError(NULL,ERROR_TYPE,op->filePos);
      }
      switch(primitiveTypeData(op->dataType)){
        case PRIMITIVE_BOOL:
        case PRIMITIVE_I8:
        case PRIMITIVE_I32:
        case PRIMITIVE_I64:
          if(op->dataAs.i64==INT64_MIN){//int64 min constant may need special handling
            fprintf(target,"%" PRIi64" -1",op->dataAs.i64+1);
          }else{
            fprintf(target,"%" PRIi64,op->dataAs.i64);
          }
          if(needCast)
            fputs(")",target);
          return size;
        case PRIMITIVE_TYPE://type-constants compile to the size of the given type
            fputs("sizeof(",target);
            printTypeNameC(op->dataAs.sourceType,target);
            fputs(")",target);
          if(needCast)
            fputs(")",target);
          return size;
        default:
          fprintf(stderr,"%s constants are (currently) not supported\n",primitiveName(primitiveTypeData(op->dataType)));
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
    case OP_UNREACHABLE:
      fputs("exit(EXIT_FAILURE);//unreachable\n",target);
      return size;
    case OP_GET:
      return compileGetValue(target,compiledOps,op,size,opSize,isGlobal);
    case OP_ADDR_OF:
      fputs("&(",target);
      size=compileGetValue(target,compiledOps,op,size,opSize,isGlobal);
      fputs(")",target);
      return size;
    case OP_ADDR_OF_ARRAY:
      size=compileGetValue(target,compiledOps,op,size,opSize,isGlobal);
      fputs(".data",target);
      return size;
    case OP_SET:
      size=compileGetValue(target,compiledOps,op,size,opSize,isGlobal);
      fputs(" = ",target);
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      fputs(";\n",target);
      return size;
    case OP_PRE_DECLARE:
      if(op->dataAs.idInfo.type!=ID_PROCEDURE)
        printTypeNameC(op->dataType,target);
      switch(op->dataAs.idInfo.type){
        case ID_TMP_VAR:
          if(!op->dataAs.idInfo.isMutable)
            fputs(" const",target);
          fprintf(target," tmp%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return size;
        case ID_LOCAL_VAR:
          if(!op->dataAs.idInfo.isMutable)
            fputs(" const",target);
          fprintf(target," local%" PRIi32 ";\n",op->dataAs.idInfo.id);
          return size;
        case ID_GLOBAL_VAR:
          if(!op->dataAs.idInfo.isMutable)
            fputs(" const",target);
          fputs(" ",target);
          printGlobalIdentifer(label(op->dataAs.idInfo.labelId,op->filePos),target);
          fputs(";\n",target);
          return size;
        case ID_PROCEDURE:
          if(!isProcedureType(op->dataType))
            handleError("invalid type for ID_PROCEDURE",ERROR_TYPE,op->filePos);
          printProcedureSignatureC(procTypeData(op->dataType),label(op->dataAs.idInfo.labelId,op->filePos),target,false);
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
        printTypeNameC(op->dataType,target);
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
          fputs(" ",target);
          if(!op->dataAs.idInfo.isMutable)
            fputs("const ",target);
          printGlobalIdentifer(label(op->dataAs.idInfo.labelId,op->filePos),target);
          fputs(" = ",target);
          break;
        case ID_PROCEDURE:
          if(!isProcedureType(op->dataType))
            handleError("invalid type for ID_PROCEDURE",ERROR_TYPE,op->filePos);
          printProcedureSignatureC(procTypeData(op->dataType),label(op->dataAs.idInfo.labelId,op->filePos),target,true);
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
      if(isTupleType(op->dataType)){
        if(needCast){
          fputs("(",target);
          printTypeNameC(op->dataType,target);
          fputs(")",target);
        }
        fputs("{",target);
        for(int32_t e=0;e<getTypeElementCount(op->dataType);e++){
          if(e>0)
            fputs(",",target);
          fprintf(target,".e%"PRIi32"=",e);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
        }
        fputs("}",target);
        return size;
      }
      if(isEnumType(op->dataType)){
        if(needCast){
          fputs("(",target);
          printTypeNameC(op->dataType,target);
          fputs(")",target);
        }
        if(compositeTypeData(op->dataType)->flags&FLAG_VOID_ONLY){
          fprintf(target,"/*enum*/%"PRIi64,op->dataAs.i64);
          return size;
        }
        fprintf(target,"{.label=%"PRIi64,op->dataAs.i64);
        if(typeEquals(getTypeElements(op->dataType)[op->dataAs.i64],TYPE_UNDEFINED)){
          fputs(",}",target);
          return size;
        }
        fprintf(target,",.data={.e%"PRIi64"=",op->dataAs.i64);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs("}}",target);
        return size;
      }
      if(isArrayType(op->dataType)){
        if(!arrayTypeData(op->dataType)->fixedSize)
          handleError("new is not implemented for var-size arrays",ERROR_UNIMPLEMENTED,op->filePos);
        if(needCast){
          fputs("(",target);
          printTypeNameC(op->dataType,target);
          fputs(")",target);
        }
        fputs("{0}",target);
        return size;
      }
      handleError("unexpected type for OP_NEW",ERROR_UNIMPLEMENTED,op->filePos);
      break;
    case OP_CAST:
      if(typeEquals(unwrapNamedType(op->dataAs.sourceType),unwrapNamedType(op->dataType))){//no cast neccessary
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        return size;
      }
      if(isArrayType(op->dataAs.sourceType)){
        if(isPointerType(op->dataType)){
          fputs("((",target);
          printTypeNameC(op->dataType,target);
          fputs(")",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs(".data)",target);
          return size;
        }
        break;//unknown cast
      }
      if(isEnumType(op->dataAs.sourceType)&&((compositeTypeData(op->dataAs.sourceType)->flags&FLAG_VOID_ONLY)==0)){
        fputs("(",target);
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(").label",target);
        return size;
      }
      //XXX ensure that casts fixed size pointer -> var-size pointer do not produce no-op C-casts
      fputs("((",target);
      printTypeNameC(op->dataType,target);
      fputs(")",target);
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
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("++)",target);
          return size;
        case DECREMENT:
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("--)",target);
          return size;
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
      if(op->dataAs.binOp==UDIVIDE||op->dataAs.binOp==UMOD||op->dataAs.binOp==URSHIFT){
        fputs("(",target);
        printTypeNameC(op->dataType,target);
        fputs(")(((",target);
        printUnsignedTypeNameC(op->dataType,target);
        fputs(")",target);
      }
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
        case UDIVIDE:
          fputs(")/((",target);
          printUnsignedTypeNameC(op->dataType,target);
          fputs(")",target);
          break;
        case UMOD:
          fputs(")%((",target);
          printUnsignedTypeNameC(op->dataType,target);
          fputs(")",target);
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
        case LSHIFT:
          fputs("<<",target);
          break;
        case RSHIFT:
          fputs(">>",target);
          break;
        case URSHIFT:
          fputs(")>>((",target);
          printUnsignedTypeNameC(op->dataType,target);
          fputs(")",target);
          break;
      }
      COMPILE_OP_RETURN_ERROR(target,op,opSize);
      if(op->dataAs.binOp==UDIVIDE||op->dataAs.binOp==UMOD||op->dataAs.binOp==URSHIFT)
        fputs("))",target);
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
          if(!isSwitchableType(op->dataType)){
            fputs("compiling switch-case of type ",stderr);
            printTypeName(op->dataType,stderr);
            fputs(" is not implemented\n",stderr);
            handleError(NULL,ERROR_UNIMPLEMENTED,op->filePos);
          }
          fputs("switch(",target);
          COMPILE_OP_RETURN_ERROR(target,op,opSize);
          fputs("){\n",target);
          return size;
        case BLOCK_CASE:
          if(!isSwitchableType(op->dataType)){
            fputs("compiling switch-case of type ",stderr);
            printTypeName(op->dataType,stderr);
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
          if(!isSwitchableType(op->dataType)){
            fputs("compiling switch-case of type ",stderr);
            printTypeName(op->dataType,stderr);
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
      if(getTypeElementCount(op->dataType)==0){
        fputs(";\n",target);
        return size;
      }
      if(getTypeElementCount(op->dataType)==1){
        COMPILE_OP_RETURN_ERROR(target,op,opSize);
        fputs(";\n",target);
        return size;
      }
      fputs("(",target);
      printTypeNameC(op->dataType,target);
      fputs("){",target);
      for(int32_t e=0;e<getTypeElementCount(op->dataType);e++){
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
      printGlobalIdentifer(label(op->dataAs.idInfo.labelId,op->filePos),target);
      return compileProcArgs(target,compiledOps,op,size,opSize,isGlobal);
    case OP_GET_LABEL:
    case OP_SET_LABEL:
    case OP_ADDR_OF_LABEL:
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
  for(int32_t i=0;i<compositeTypeCount;i++){
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
      if(arrayTypes[i].viewOnly)
        continue;//skip view-only arrays with known size
      fprintf(target,"typedef struct array%"PRIi32"Impl array%"PRIi32";\n",i,i);
  }
  //declare procedure pointers
  for(int32_t i=0;i<procTypeCount;i++){
    if(procTypes[i].pointerUsed){
      fputs("typedef ",target);
      printTypeNameC(procTypes[i].outType,target);
      fprintf(target," (*procPtr%"PRIi32") (",i);
      printProcArgumentTypesC(&procTypes[i],target,false);
      fputs(");\n",target);
    }
  }
  for(int32_t i=0;i<declaredMultiTypeCount;i++){//got through multi-types in order of declaration
    int32_t id=declaredMultiTypes[i].dataAs.id;
    if(isPointerType(declaredMultiTypes[i])||isArrayType(declaredMultiTypes[i])){
      if(arrayTypes[id].viewOnly)
        continue;//skip view-only arrays with known size
      //initialize array types
      fprintf(target,"struct array%"PRIi32"Impl{\n",id);
      printTypeNameC(arrayTypes[id].base,target);
      if(!arrayTypes[id].fixedSize)
        handleError("arrays cannot be variable-sized",ERROR_UNIMPLEMENTED,(FilePosition){0});
      fputs(" data",target);
      for(int32_t d=arrayTypes[id].dims-1;d>=0;d--){//C orders sizes the other way around
        if(!arrayTypes[id].sizes[d].isInt)
          handleError("non integer array sizes should not exist at this state of compilation",ERROR_MEMORY,(FilePosition){0});
        fprintf(target,"[%"PRIi64"]",arrayTypes[id].sizes[d].value);
      }
      fputs(";\n",target);
      fputs("};\n",target);
      if(!arrayTypes[id].sizeUsed)
        continue;
      fprintf(target,"int64_t const arraySizes%"PRIi32"[%"PRIi32"]={",arrayTypes[id].id,arrayTypes[id].dims);
      for(int32_t d=0;d<arrayTypes[id].dims;d++){
        if(!arrayTypes[id].sizes[d].isInt)
          handleError("non integer array sizes should not exist at this state of compilation",ERROR_MEMORY,(FilePosition){0});
        if(d>0)
          fputs(",",target);
        fprintf(target,"%"PRIi64,arrayTypes[id].sizes[d].value);
      }
      fputs("};\n",target);
      continue;
    }
    //initialize composite types
    if(isUsedTuple(&compositeTypes[id])){
      fprintf(target,"struct tuple%"PRIi32"Impl{\n",id);
      for(int16_t e=0;e<compositeTypes[id].typeCount;e++){
        printTypeNameC(compositeTypes[id].types[e],target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("};\n",target);
    }
    if(((compositeTypes[id].flags&FLAG_IS_ENUM)!=0)&&((compositeTypes[id].flags&FLAG_VOID_ONLY)==0)){
      fprintf(target,"struct enum%"PRIi32"Impl{\n",id);
      fputs("union{\n",target);
      for(int16_t e=0;e<compositeTypes[id].typeCount;e++){
        if(typeEquals(compositeTypes[id].types[e],TYPE_UNDEFINED))
          continue;//skip void types
        printTypeNameC(compositeTypes[id].types[e],target);
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
      fprintf(target,"const %s arrayData%"PRIi32"[%"PRIi64"] = {",primitiveNameC(PRIMITIVE_I8),programStrings[i].charsId,programStrings[i].value.length+1);
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
  fputs("//global declarations\n",target);
  for(FileId f=0;f<p->fileCount;f++){
    fprintf(target,"// %"PRI_STR"\n",PRI_STR_ARGS(p->files[f].fileName));
    ScopeIterator itr=getScopeIterator(&p->files[f].globalScope);
    ScopeNode* n;
    while((n=scopeItrNext(&itr))!=NULL){
      if(typeEquals(n->type,TYPE_TYPE))
        continue;//ignore types
      Label const* mLabel=label(n->labelId,(FilePosition){0});
      if(!wordEquals(&p->files[f].fileName,mLabel->declaredAt.fileName))
        continue;//declared in different file
      if(isExternLabel(mLabel))
        fputs("extern ",target);
      if(isProcedureType(n->type)){
        printProcedureSignatureC(procTypeData(n->type),mLabel,target,false);
        fputs(";\n",target);
        continue;
      }
      if(isAutoType(n->type)){
        if(p->autoTypes==NULL||autoTypeId(n->type)<0||autoTypeId(n->type)>=p->nAutoTypes){
          handleError("predeclared id out of expected range",ERROR_TYPE,mLabel->declaredAt);
        }
        n->type=p->autoTypes[autoTypeId(n->type)];//get predeceased type
      }
      printTypeNameC(n->type,target);
      fputs(" ",target);
      if(!isMutableLabel(mLabel))
        fputs("const ",target);
      printGlobalIdentifer(mLabel,target);
      fputs(";\n",target);
    }
  }
  fputs("//global code\n",target);
  for(FileId f=0;f<p->fileCount;f++){
    if(p->files[f].globalOpCount>0)
      fprintf(target,"// %"PRI_STR"\n",PRI_STR_ARGS(p->files[f].fileName));
    for(size_t i=0;i<p->files[f].globalOpCount;){
      i+=compileOp(target,i,p->files[f].globalOps+i,p->files[f].globalOpCount-i,true);
    }
  }
  fputs("//procedures code\n",target);
  for(FileId f=0;f<p->fileCount;f++){
    if(p->files[f].localOpCount>0)
      fprintf(target,"// %"PRI_STR"\n",PRI_STR_ARGS(p->files[f].fileName));
    for(size_t i=0;i<p->files[f].localOpCount;){
      i+=compileOp(target,i,p->files[f].localOps+i,p->files[f].localOpCount-i,false);
    }
  }
}

typedef struct{
  char* code;
  size_t codeSize;
  FilePosition currentPos;
  FilePosition wordStart;
}CodeFile;

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
bool readCodeFile(char const* fileName,CodeFile* res){
  bool isError=false;
  char* code=NULL;
  FILE *file=NULL;
    int64_t codeSize;
    file = fopen(fileName, "r");
	  if(file==NULL){
	    fprintf(stderr,"Could not open file: \"%s\"\n",fileName);
	    isError=true;
		  goto cleanup;
	  }
	  long int size=fsize(file);
	  if(size<16){
		  size=16;
	  }
	  code = malloc((size+1)*sizeof(char));//will be freed when the program exits
	  if(code==NULL){
	    fprintf(stderr,"Memory error while reading file: \"%s\"\n",fileName);
	    isError=true;
		  goto cleanup;
	  }
	  // while(!feof(file))
	  codeSize=fread(code,sizeof(char),size,file);//TODO perform multiple reads if necessary
	  if(ferror(file)){
	    fprintf(stderr,"IO Error while reading file: \"%s\"\n",fileName);
	    isError=true;
		  goto cleanup;
	  }
	  *res=(CodeFile){.code=code,.codeSize=codeSize,
	    .currentPos={.fileName=fileName,.line=1,.posInLine=1},
	    .wordStart={.fileName=fileName,.line=1,.posInLine=1}};
    code=NULL;//code not stored in CodeFile
cleanup:
  free(code);
  if(file)
    fclose(file);
  return isError;
}
FileId parseFile(ParserState*,CodeFile*);


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


Operation opDeclareIntermediate(TypeId type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_DECLARE,.dataType=type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_INTERMEDIATE_RESULT,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=isMultiValueType(type)}}};
}
Operation opGetIntermediate(TypeId type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_GET,.dataType=type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_INTERMEDIATE_RESULT,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=isMultiValueType(type)}}};
}
Operation opPredeclareTmpVar(TypeId type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_PRE_DECLARE,.dataType=type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}
Operation opDeclareTmpVar(TypeId type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_DECLARE,.dataType=type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}
Operation opGetTmpVar(TypeId type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_GET,.dataType=type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}
Operation opSetTmpVar(TypeId type,int32_t tmpId,FilePosition pos){
  return (Operation){.opType=OP_SET,.dataType=type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_TMP_VAR,.id=tmpId,.labelId=LABEL_ID_UNKNOWN,.isMutable=true}}};
}
Operation opGetArgument(TypeId type,int32_t argId,LabelId labelId,FilePosition pos){
  return (Operation){.opType=OP_GET,.dataType=type,.filePos=pos,
    .dataAs={.idInfo={.type=ID_ARGUMENT,.id=argId,.labelId=labelId,.isMutable=false}}};
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
Operation opConstant(TypeId type,int64_t constData,FilePosition pos){
  return (Operation){.opType=OP_CONSTANT,.dataType=type,.filePos=pos,.dataAs={.i64=constData}};
}
Operation opTypeConstant(TypeId type,TypeId constData,FilePosition pos){
  return (Operation){.opType=OP_CONSTANT,.dataType=type,.filePos=pos,.dataAs={.sourceType=constData}};
}
Operation opUnreachable(FilePosition pos){
  return (Operation){.opType=OP_UNREACHABLE,.dataType=TYPE_UNDEFINED,.filePos=pos,.dataAs={0}};
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
  uint64_t maxSaveValue=negate?-(INT64_MIN/base):(INT64_MAX/base);
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

bool checkLabelFlag(String const* name,char const* labelType,char const* flagName,LabelFlags labelFlag,LabelFlags* flags,FilePosition pos){
  if(!wordEquals(name,flagName))
    return false;
  if(((*flags)&labelFlag)!=0){
    fprintf(stderr,"%s is already marked as %s\n",labelType,flagName);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  (*flags)|=labelFlag;
  return true;
}
void checkFlagCombinations(LabelFlags flags,FilePosition pos){
  if((flags&(LABEL_FLAG_PUBLIC|LABEL_FLAG_PRIVATE))==(LABEL_FLAG_PUBLIC|LABEL_FLAG_PRIVATE))
    handleError("identifiers cannot be both 'public' and 'private'",ERROR_SYNTAX,pos);
  if((flags&(LABEL_FLAG_MUTABLE|LABEL_FLAG_STATIC))==(LABEL_FLAG_MUTABLE|LABEL_FLAG_STATIC))
    handleError("identifiers cannot be both 'mutable' and 'static'",ERROR_SYNTAX,pos);
  if((flags&(LABEL_FLAG_EXTERN|LABEL_FLAG_MUTABLE))==(LABEL_FLAG_EXTERN|LABEL_FLAG_MUTABLE))
    handleError("identifiers cannot be both 'extern' and 'mutable'",ERROR_SYNTAX,pos);
  if((flags&(LABEL_FLAG_EXTERN|LABEL_FLAG_STATIC))==(LABEL_FLAG_EXTERN|LABEL_FLAG_STATIC))
    handleError("identifiers cannot be both 'extern' and 'static'",ERROR_SYNTAX,pos);
}
LabelId readLabel(CodeFile* codeFile,char const* labelType,NamespaceId namespace,LabelFlags allowedFlags){
  int wordType=0;
  bool isModifer;
  LabelFlags labelFalgs=0;
  String label;
  do{
    label=nextWord(codeFile,&wordType);
    isModifer=false;
    if(wordType!=WORD_TYPE_IDENTIFIER){
      fprintf(stderr,"%s have to be identifiers\n",labelType);
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
    }
    if(label.length==0){//empty name -> go to next word
      isModifer=true;//reset is modifier to continue loop
      continue;
    }
    if(checkLabelFlag(&label,labelType,"mut",LABEL_FLAG_MUTABLE,&labelFalgs,codeFile->wordStart))
      isModifer=true;
    if(checkLabelFlag(&label,labelType,"static",LABEL_FLAG_STATIC,&labelFalgs,codeFile->wordStart))
      isModifer=true;
    if(checkLabelFlag(&label,labelType,"extern",LABEL_FLAG_EXTERN,&labelFalgs,codeFile->wordStart))
      isModifer=true;
    if(checkLabelFlag(&label,labelType,"private",LABEL_FLAG_PRIVATE,&labelFalgs,codeFile->wordStart))
      isModifer=true;
    if(checkLabelFlag(&label,labelType,"public",LABEL_FLAG_PUBLIC,&labelFalgs,codeFile->wordStart))
      isModifer=true;
    if(isModifer&&((labelFalgs|allowedFlags)!=allowedFlags)){
      fprintf(stderr,"flag \"%"PRI_STR"\" is not allowed on %s\n",PRI_STR_ARGS(label),labelType);
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
    }
    checkFlagCombinations(labelFalgs,codeFile->wordStart);
  }while(isModifer);
  return newLabel(label,labelFalgs,namespace,codeFile->wordStart);
}


//temporary buffer for storing constants
#define CONST_BUFFER_CAP       512
#define MAX_COMPOSITE_ELEMENTS 128
#define MAX_ARRAY_DIMS         64
#define MAX_STATIC_ARGS        128
typedef struct{
  FilePosition pos;
  ConstantValue value;
  IdentifierInfo idInfo;
  bool hasId;
}Constant;
size_t bufferedConstants=0;
Constant constBuffer[CONST_BUFFER_CAP];

TypeId compositeTypeBuffer[MAX_COMPOSITE_ELEMENTS];
ArraySize arrayDimsBuffer[MAX_ARRAY_DIMS];
int64_t arrayDimsCount=0;
int64_t arrayWildcardDimsCount=0;
StaticArgument staticArgsBuffer[MAX_STATIC_ARGS];
int64_t staticArgsCount=0;

void pushConstant(ConstantValue constValue,FilePosition pos,bool hasId,IdentifierInfo idInfo){
  if(bufferedConstants>=CONST_BUFFER_CAP)
    handleError("constant buffer overflow",ERROR_MEMORY,pos);
  constBuffer[bufferedConstants++]=(Constant){.value=constValue,.pos=pos,.hasId=hasId,.idInfo=idInfo};
}
void pushIntConstant(ConstantType constType,int64_t constId,FilePosition pos){
  TypeId type=(constType==CONSTANT_CHAR)?TYPE_CHAR:primitiveType(((constId<=INT32_MAX&&constId>=INT32_MIN)?PRIMITIVE_I32:PRIMITIVE_I64));
  pushConstant((ConstantValue){.constType=constType,.valueType=type,.as.i64=constId},pos,false,(IdentifierInfo){0});
}
void pushBoolConstant(bool value,FilePosition pos){
  pushConstant((ConstantValue){.constType=CONSTANT_BOOL,.valueType=TYPE_BOOL,.as.boolean=value},pos,false,(IdentifierInfo){0});
}
void pushStringConstant(String value,FilePosition pos){
  pushConstant((ConstantValue){.constType=CONSTANT_STRING,.valueType=arrayType(true,TYPE_CHAR,1,(ArraySize[]){{.value=value.length,.isInt=true}},false),.as.string=value}
    ,pos,false,(IdentifierInfo){0});
}
void pushTypeConstant(TypeId type,FilePosition pos){
  pushConstant((ConstantValue){.constType=CONSTANT_TYPE,.valueType=TYPE_TYPE,.as.type=type},pos,false,(IdentifierInfo){0});
}
void pushWildcardConstant(FilePosition pos){
  pushConstant((ConstantValue){.constType=CONSTANT_WILDCARD,.valueType=TYPE_UNDEFINED,.as.i64=-1},pos,false,(IdentifierInfo){0});
}
void addStaticArgument(int32_t index,LabelId label,TypeId type,FilePosition pos){
  StaticArgument arg;
  if(typeEquals(type,TYPE_TYPE)){
    arg=(StaticArgument){.label=label,.type=type,.as.type=newGenericType(index)};
  }else if(isIntType(type)){
    arg=(StaticArgument){.label=label,.type=type,.as.genericId=index};
  }else{
    handleError("static arguments have to be types",ERROR_SYNTAX,pos);
  }
  if(staticArgsCount>=MAX_STATIC_ARGS)
    handleError("static-argument buffer overflow",ERROR_MEMORY,pos);
  staticArgsBuffer[staticArgsCount]=arg;
  staticArgsCount++;
}
Constant const* peekConstant(void){
  if(bufferedConstants==0)
    return NULL;
  return &constBuffer[bufferedConstants-1];
}
ConstantValue const* peekConstValue(void){
  Constant const* constant=peekConstant();
  if(constant==NULL)
    return NULL;
  return &constant->value;
}
TypeId popTypeConstant(FilePosition pos,char const* argumentName,bool allowVoid){
  if(bufferedConstants==0){
    fprintf(stderr,"missing %s\n",argumentName);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  bufferedConstants--;
  if(constBuffer[bufferedConstants].value.constType!=CONSTANT_TYPE){
    fprintf(stderr,"wrong constant type for %s expected type got %s\n",argumentName,constTypeName(constBuffer[bufferedConstants].value.constType));
    fputs(" declared at ",stderr);
    printFilePosition(constBuffer[bufferedConstants].pos,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  if(!allowVoid&&typeEquals(constBuffer[bufferedConstants].value.as.type,TYPE_UNDEFINED)){
    fprintf(stderr,"missing %s\n",argumentName);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  return constBuffer[bufferedConstants].value.as.type;
}
TypeId peekTypeConstant(FilePosition pos,char const* argumentName,bool allowVoid){
  if(bufferedConstants==0){
    fprintf(stderr,"missing %s\n",argumentName);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  if(constBuffer[bufferedConstants-1].value.constType!=CONSTANT_TYPE){
    fprintf(stderr,"wrong constant type for %s expected type got %s\n",argumentName,constTypeName(constBuffer[bufferedConstants-1].value.constType));
    fputs(" declared at ",stderr);
    printFilePosition(constBuffer[bufferedConstants-1].pos,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  if(!allowVoid&&typeEquals(constBuffer[bufferedConstants-1].value.as.type,TYPE_UNDEFINED)){
    fprintf(stderr,"missing %s\n",argumentName);
    handleError(NULL,ERROR_SYNTAX,pos);
  }
  return constBuffer[bufferedConstants-1].value.as.type;
}
ArraySize* popArraySize(FilePosition pos){
  arrayDimsCount=0;
  arrayWildcardDimsCount=0;
  while(bufferedConstants>1&&constBuffer[bufferedConstants-1].value.constType!=CONSTANT_TYPE){
    bufferedConstants--;
    arrayDimsCount++;
  }
  for(int64_t i=0;i<arrayDimsCount;i++){
    if(constBuffer[bufferedConstants+i].value.constType==CONSTANT_WILDCARD){
      arrayWildcardDimsCount++;
      arrayDimsBuffer[i]=(ArraySize){.value=-1,.isInt=false};
      continue;
    }
    if(constBuffer[bufferedConstants+i].value.constType==GENERIC_INT){
      arrayDimsBuffer[i]=(ArraySize){.value=constBuffer[bufferedConstants+i].value.as.i64,.isInt=false};
      continue;
    }
    if(constBuffer[bufferedConstants+i].value.constType!=CONSTANT_INT){
      fprintf(stderr,"unexpected constant for array size expected int got %s\n",constTypeName(constBuffer[bufferedConstants+i].value.constType));
      handleError(NULL,ERROR_SYNTAX,pos);
    }
    if(constBuffer[bufferedConstants+i].value.as.i64<=0){
      fprintf(stderr,"invalid array size: %"PRIi64" array sizes have to be greater than 0\n",constBuffer[bufferedConstants+i].value.as.i64);
      handleError(NULL,ERROR_SYNTAX,pos);
    }
    arrayDimsBuffer[i]=(ArraySize){.value=constBuffer[bufferedConstants+i].value.as.i64,.isInt=true};
  }
  if(arrayWildcardDimsCount>0&&arrayDimsCount>1)
      handleError("wildcards are only allowed for one dimensional arrays",ERROR_SYNTAX,pos);
  return arrayDimsBuffer;
}
TypeId const* popTypeConstants(size_t count,FilePosition pos,char const* argumentName,bool allowVoid){
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
      fprintf(stderr,"character literal \"%"PRI_STR"\" contains more that one character\n",PRI_STR_ARGS(word));
      handleError(NULL,ERROR_SYNTAX,wordPos);
    }
    pushIntConstant(CONSTANT_CHAR,charAt(word,0),wordPos);
    return true;
  }
  if(wordEquals(&word,"true")){
    pushBoolConstant(true,wordPos);
    return true;
  }
  if(wordEquals(&word,"false")){
    pushBoolConstant(false,wordPos);
    return true;
  }
  if(wordEquals(&word,"_")){
    pushWildcardConstant(wordPos);
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
  for(int i=0;i<staticArgsCount;i++){
    if(stringCompare(getLabelName(staticArgsBuffer[i].label),word)==0){
      if(typeEquals(staticArgsBuffer[i].type,TYPE_TYPE)){
        pushTypeConstant(staticArgsBuffer[i].as.type,wordPos);
        return true;
      }
      if(isIntType(staticArgsBuffer[i].type)){
        pushIntConstant(GENERIC_INT,staticArgsBuffer[i].as.genericId,wordPos);
        return true;
      }
      handleError("unexpected static argument",ERROR_UNIMPLEMENTED,wordPos);
    }
  }
  ScopeNode* asIdentifier;
  int r=getIdentifier(getGlobalScopeParser(state),*parserNamespace(state),word,&asIdentifier,wordPos);
  if(r<0){//internal error while reading identifier
    handleError("error while reading identifier",r,codeFile->wordStart);
    return false;
  }
  if(r>0||asIdentifier->constValue.constType==CONSTANT_NONE)//identifier does not exist
    return readType(word,codeFile,state);
  pushConstant(asIdentifier->constValue,codeFile->wordStart,true,(IdentifierInfo){.type=asIdentifier->idType,.id=asIdentifier->id,.labelId=asIdentifier->labelId,
    .isMutable=isMutableLabelId(asIdentifier->labelId)});
  return true;
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
  LabelFlags allowedFlags=LABEL_FLAG_MUTABLE|LABEL_FLAG_PRIVATE|LABEL_FLAG_PUBLIC;
  if(typeClass==TYPECLASS_PROC_IN||typeClass==TYPECLASS_LABELED_PROC_IN)
    allowedFlags|=LABEL_FLAG_STATIC;
  do{
    word=nextWord(codeFile,&wordType);
    if(readConstants(word,wordType,codeFile,state)){
      typesSinceLabel+=(bufferedConstants-currentOffset);
      currentOffset=bufferedConstants;
      continue;
    }
    if(wordEquals(&word,endString))
      break;
    if(wordEquals(&word,":")){//start label
      if(labelType==LABEL_TYPE_NONE)
        handleError("expected type got ':' ",ERROR_SYNTAX,codeFile->wordStart);
      if(typesSinceLabel>1){
        fprintf(stderr,"too many types for field declaration expected 1 got %i\n",typesSinceLabel);
        handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
        return;
      }
      if(typesSinceLabel==0){
        if(labelType!=LABEL_TYPE_ENUM)
          handleError("expected type got ':' ",ERROR_SYNTAX,codeFile->wordStart);
        pushTypeConstant(TYPE_UNDEFINED,codeFile->wordStart);
      }
      typesSinceLabel=0;
      LabelId labelId=readLabel(codeFile,"composite type labels",parserNamespace(state)->current,allowedFlags);//label is stored in label buffer
      if(isStaticLabelId(labelId)){
        addStaticArgument((bufferedConstants-1)-initOffset,labelId,peekTypeConstant(codeFile->wordStart,"static argument",false),codeFile->wordStart);//remember static types
      }
      currentOffset=bufferedConstants;
      continue;
    }
    if(labelType!=LABEL_TYPE_ENUM||typesSinceLabel>0||wordEquals(&word,"mut")){
      fprintf(stderr,"unknown type name \"%"PRI_STR"\" \n",PRI_STR_ARGS(word));
      handleError(NULL,ERROR_SYNTAX,codeFile->wordStart);
      return;
    }
    //untyped enum label XXX prevent use of modifiers as labels
    newLabel(word,false,parserNamespace(state)->current,codeFile->wordStart);//label is stored in label buffer
    pushTypeConstant(TYPE_UNDEFINED,codeFile->wordStart);
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
          fprintf(stderr,"duplicate label \"%"PRI_STR"\" in %s \n",PRI_STR_ARGS(getLabelName(i)),typeClassName(typeClass));
          fputs("  previous declaration at ",stderr);
          printFilePosition(label(j,codeFile->wordStart)->declaredAt,stderr);
          fputs("\n",stderr);
          handleError(NULL,ERROR_SYNTAX,label(i,codeFile->wordStart)->declaredAt);
        }
      }
    }
  }
  size_t maxOffset=bufferedConstants;
  TypeId const* elements=popTypeConstants(maxOffset-initOffset,codeFile->wordStart,"composite elements",labelType==LABEL_TYPE_ENUM);
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
  if(typeEquals(peekTypeConstant(codeFile->wordStart,"composite type",true),TYPE_UNDEFINED)){
    handleError("unknown error while creating composite type",ERROR_SYNTAX,codeFile->wordStart);
    return;
  }
  if(checkEmpty&&maxOffset-initOffset==1){
    fputs("WARNING:\n  single element composite type: ",stderr);
    printTypeName(peekTypeConstant(codeFile->wordStart,"composite element",false),stderr);
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
  if(wordEquals(&name,"type")){
    pushTypeConstant(TYPE_TYPE,codeFile->wordStart);
    return true ;
  }
  if(wordEquals(&name,"bool")){
    pushTypeConstant(TYPE_BOOL,codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"i8")||wordEquals(&name,"char")){
    pushTypeConstant(TYPE_CHAR,codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"i32")){
    pushTypeConstant(primitiveType(PRIMITIVE_I32),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"i64")){
    pushTypeConstant(TYPE_I64,codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"float")){
    pushTypeConstant(primitiveType(PRIMITIVE_FLOAT),codeFile->wordStart);
    return true;
  }
  //composite types
  if(wordEquals(&name,"ptr")){
    ArraySize* dims=popArraySize(codeFile->wordStart);
    TypeId target=popTypeConstant(codeFile->wordStart,"pointer argument",false);
    if(isProcedureType(target)&&arrayDimsCount>0)
      handleError("procedure pointers cannot be multidimensional",ERROR_SYNTAX,codeFile->wordStart);
    pushTypeConstant(arrayType(true,target,arrayDimsCount,arrayWildcardDimsCount>0?NULL:dims,false),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"mut")){
    TypeId target=popTypeConstant(codeFile->wordStart,"mutability argument",false);
    if(isMutableType(target))
      handleError("type is already mutable",ERROR_TYPE,codeFile->wordStart);
    if(!makeMutable(&target)){
      fprintf(stderr,"%s types cannot be mutable\n",typeClassName(target.class));
      handleError(NULL,ERROR_TYPE,codeFile->wordStart);
    }
    pushTypeConstant(target,codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"array")){
    ArraySize* dims=popArraySize(codeFile->wordStart);
    TypeId target=popTypeConstant(codeFile->wordStart,"array argument",false);
    if(arrayDimsCount==0){//array without size arguments
      handleError("zero-dimensional arrays are not supported",ERROR_SYNTAX,codeFile->wordStart);
      return true;
    }
    if(arrayWildcardDimsCount>0){//array without size arguments
      handleError("array size has to be fixed",ERROR_SYNTAX,codeFile->wordStart);
      return true;
    }
    pushTypeConstant(arrayType(false,target,arrayDimsCount,dims,false),codeFile->wordStart);
    return true;
  }
  if(wordEquals(&name,"proc(")){
    readCompositeType(TYPECLASS_PROC_IN,codeFile,state,LABEL_TYPE_PROC_IN,"=>",false);
    readCompositeType(TYPECLASS_PROC_OUT,codeFile,state,LABEL_TYPE_NONE,")",false);
    TypeId out=popTypeConstant(codeFile->wordStart,"procedure input",false);
    TypeId in=popTypeConstant(codeFile->wordStart,"procedure output",false);
    pushTypeConstant(procedureType(in,out,staticArgsBuffer,staticArgsCount),codeFile->wordStart);
    staticArgsCount=0;
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
  return false;
}

void requireCompileTimeTypes(String* opName,TypeId* typeOut,size_t nTypes,FilePosition pos){
  if(bufferedConstants<nTypes){
    fprintf(stderr,"not enough type arguments for operation \"%"PRI_STR"\" need %zu got %zu\n",PRI_STR_ARGS(*opName),nTypes,bufferedConstants);
    handleError(NULL,ERROR_SYNTAX,pos);
    return;
  }
  for(size_t i=0;i<nTypes;i++){
    *(typeOut)=popTypeConstant(pos,"operation argument",false);
    if(typeEquals(*typeOut,TYPE_UNDEFINED))
      handleError("invalid type in type buffer",ERROR_TYPE,pos);
    typeOut++;
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
bool ensureInlcudesCap(IncludedFile** mList,size_t* cap,size_t newSize){
  void* ops=*mList;
  if(ensureCap(&ops,cap,sizeof(IncludedFile),newSize))
    return true;
  *mList=ops;
  return false;
}
bool ensureFilesCap(ProgramFile** mList,size_t* cap,size_t newSize){
  void* ops=*mList;
  if(ensureCap(&ops,cap,sizeof(ProgramFile),newSize))
    return true;
  *mList=ops;
  return false;
}
void includeFile(ParserState * state,char const* includePath,FilePosition includePos){
  FileId included=FILE_ID_NONE;
  for(FileId fId=0;fId<state->fileCount;fId++){
    if(wordEquals(&state->files[fId].fileName,includePath)){
      included=fId;
      break;
    }
  }
  if(!quietMode){
    printf("  include: %s\n",includePath);
  }
  if(included==FILE_ID_NONE){
    CodeFile includedFile;
    if(readCodeFile(includePath,&includedFile)){
      handleError("could not include file",ERROR_SYNTAX,includePos);
      return;
    }
    included=parseFile(state,&includedFile);
  }
  if(includeConstants(&state->files[state->currentFile].globalScope,&state->files[included].globalScope,includePos)){
    handleError(NULL,ERROR_SYNTAX,includePos);
  }
  //add include file to includes list
  ProgramFile* f=&state->files[state->currentFile];
  if(ensureInlcudesCap(&f->includes,&f->includeCap,f->includeCount+1))
    handleError("exceeded include capacity",ERROR_MEMORY,includePos);
  f->includes[f->includeCount++]=(IncludedFile){.id=included,.includePos=includePos};
}

void pushOperation(ParserState* state,Operation op);

void storeConstants(ParserState* state,FilePosition pos){
  int64_t intVal;
  size_t constCount=bufferedConstants;
  bufferedConstants=0;//set constant count to 0 to prevent infinite recursion
  for(size_t i=0;i<constCount;i++){
    switch(constBuffer[i].value.constType){
      case CONSTANT_STRING:
        intVal=addProgString(constBuffer[i].value.as.string,constBuffer[i].pos);
        pushOperation(state,opConstant(constBuffer[i].value.valueType,intVal,constBuffer[i].pos));
        break;
    case CONSTANT_BOOL:
        pushOperation(state,opConstant(constBuffer[i].value.valueType,constBuffer[i].value.as.boolean,constBuffer[i].pos));
        break;
      case CONSTANT_CHAR:
        pushOperation(state,opConstant(constBuffer[i].value.valueType,constBuffer[i].value.as.charId,constBuffer[i].pos));
        break;
      case CONSTANT_INT:
        intVal=constBuffer[i].value.as.i64;
        pushOperation(state,opConstant(constBuffer[i].value.valueType,intVal,constBuffer[i].pos));
        break;
      case CONSTANT_TYPE:
        pushOperation(state,opTypeConstant(constBuffer[i].value.valueType,constBuffer[i].value.as.type,constBuffer[i].pos));
        break;
      case CONSTANT_NONE:
        break;
      case CONSTANT_WILDCARD:
      case GENERIC_INT:
        fprintf(stderr,"cannot store constants of type %s\n",constTypeName(constBuffer[i].value.constType));
        handleError(NULL,ERROR_SYNTAX,pos);
        break;
    }
  }
}
void pushOperation(ParserState* state,Operation op){
  if(bufferedConstants>0){
    storeConstants(state,op.filePos);
  }
  ProgramFile* currentFile=&state->files[state->currentFile];
  if(localScopeCount>0){//local
    if(ensureOpCap(&currentFile->localOps,&currentFile->localOpCap,currentFile->localOpCount+bufferedConstants+16)){
      handleError("exceeded operation capacity",ERROR_MEMORY,op.filePos);
    }
    currentFile->localOps[currentFile->localOpCount++]=op;
    return;
  }
  if(ensureOpCap(&currentFile->globalOps,&currentFile->globalOpCap,currentFile->globalOpCount+bufferedConstants+16)){
    handleError("exceeded operation capacity",ERROR_MEMORY,op.filePos);
  }
  currentFile->globalOps[currentFile->globalOpCount++]=op;
}
bool canPeekOperationParser(ParserState* state){
  ProgramFile* currentFile=&state->files[state->currentFile];
  if(localScopeCount>0){//local
    return currentFile->localOpCount>0;
  }
  return currentFile->globalOpCount>0;
}
Operation* peekOperation(ParserState* state,FilePosition pos){
  ProgramFile* currentFile=&state->files[state->currentFile];
  if(localScopeCount>0){//local
    if(currentFile->localOpCount==0){
      handleError("operation underflow",ERROR_MEMORY,pos);
    }
    return &currentFile->localOps[currentFile->localOpCount-1];
  }
  if(currentFile->globalOpCount==0){
    handleError("operation underflow",ERROR_MEMORY,pos);
  }
  return &currentFile->globalOps[currentFile->globalOpCount-1];
}
void readOperation(ParserState* state,CodeFile* codeFile){
  int wordType;
  String word=nextWord(codeFile,&wordType);
  TypeId type;
  FilePosition wordPos=codeFile->wordStart;
  if(readConstants(word,wordType,codeFile,state))//is type
    return;
  wordPos=codeFile->wordStart;
  LabelFlags identiferFlags=LABEL_FLAG_MUTABLE;
  if(localScopeCount==0)
    identiferFlags|=LABEL_FLAG_PRIVATE|LABEL_FLAG_PUBLIC;
  //1. operations that take a Type as argument
  if(wordEquals(&word,":")){//pre-declare
    requireCompileTimeTypes(&word,&type,1,wordPos);
    LabelId labelId=readLabel(codeFile,localScopeCount>0?"local variables":"global variables",parserNamespace(state)->current,identiferFlags|LABEL_FLAG_EXTERN);
    wordPos=codeFile->wordStart;
    Label const* varName=label(labelId,wordPos);
    IdentifierType idType=localScopeCount>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    if(isProcedureType(type)){
      if(!isExternLabel(varName))
        handleError("directly pre-declaration is only supporte for extern procedures",ERROR_SYNTAX,wordPos);
      idType=ID_PROCEDURE;
    }
    ConstantValue val=(ConstantValue){.constType=CONSTANT_NONE,.valueType=TYPE_UNDEFINED,.as.i64=0};
    if(typeEquals(type,TYPE_TYPE)){
      val.constType=CONSTANT_TYPE;
      val.as.type=newNamedType(labelId,TYPE_UNDEFINED);
      idType=ID_TYPE;
    }
    ScopeNode const* id=declareIdentifier(getGlobalScopeParser(state),*parserNamespace(state),labelId,type,idType,nextId(idType,state),wordPos,&val);
    if(idType==ID_TYPE)//declaring type does not produce any code
      return;
    if(localScopeCount>0)//global identifiers are predeclared implicitly)
      pushOperation(state,(Operation){.opType=OP_PRE_DECLARE,.dataType=type,.filePos=varName->declaredAt,
        .dataAs={.idInfo={.type=idType,.id=id->id,.labelId=labelId,.isMutable=isMutableLabel(varName)}}});
    return;
  }else if(wordEquals(&word,"=:")){//declare
    requireCompileTimeTypes(&word,&type,1,wordPos);
    LabelId labelId=readLabel(codeFile,localScopeCount>0?"local variables":"global variables",parserNamespace(state)->current,identiferFlags);
    wordPos=codeFile->wordStart;
    Label const* varName=label(labelId,wordPos);
    IdentifierType idType;
    if(typeEquals(type,TYPE_TYPE)){
      idType=ID_TYPE;
      TypeId constType=popTypeConstant(wordPos,"type constant",false);
      type=newNamedType(labelId,constType);
      ConstantValue constValue=(ConstantValue){.constType=CONSTANT_TYPE,.valueType=TYPE_TYPE,.as.type=type};
      declareIdentifier(getGlobalScopeParser(state),*parserNamespace(state),labelId,type,idType,nextId(idType,state),wordPos,&constValue);
      return;
    }else if(isProcedureType(type)){
      idType=ID_PROCEDURE;
    }else{
      idType=localScopeCount>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    }
    ScopeNode const* id=declareIdentifier(getGlobalScopeParser(state),*parserNamespace(state),labelId,type,idType,nextId(idType,state),wordPos,peekConstValue());
    if(idType==ID_PROCEDURE){
      if(localScopeCount>0){
        fprintf(stderr,"invalid position for procedure %"PRI_STR" procedures can only be declared at top level\n",PRI_STR_ARGS(varName->label));
          handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      if(openScope(BLOCK_PROCEDURE,state))
        handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
      state->currentProcId=procTypeData(type)->procId;
      state->localVars=0;
      if(typeElementsLabeled(procTypeData(type)->inType)){
         CompositeType const* inTypes=compositeTypeData(procTypeData(type)->inType);
         for(int32_t i=0;i<inTypes->typeCount;i++){
            declareIdentifier(getGlobalScopeParser(state),*parserNamespace(state),inTypes->labelOffset+i,inTypes->types[i],ID_ARGUMENT,i,wordPos,NULL);
         }
      }
    }
    pushOperation(state,(Operation){.opType=OP_DECLARE,.dataType=type,.filePos=varName->declaredAt,.dataAs={.idInfo={.type=idType,.id=id->id,.labelId=labelId,.isMutable=isMutableLabel(varName)}}});
    return;
  }else if(wordEquals(&word,"new")){
    if(canPeekOperationParser(state)&&peekOperation(state,wordPos)->opType==OP_CONSTANT&&isEnumLabelType(peekOperation(state,wordPos)->dataType)){
      //change enum label to enum declaration
      peekOperation(state,wordPos)->opType=OP_NEW;
      peekOperation(state,wordPos)->filePos=wordPos;
      if(changeEnumType(&peekOperation(state,wordPos)->dataType,false))//change type-class back to enum
        handleError("could not update enum type",ERROR_MEMORY,wordPos);
      return;
    }
    requireCompileTimeTypes(&word,&type,1,wordPos);
    if(isTupleType(type)||isArrayType(type)){
      pushOperation(state,(Operation){.opType=OP_NEW,.dataType=type,.filePos=wordPos,.dataAs={.i64=0}});
      return;
    }
    printTypeName(type,stderr);
    fputs(" is currently not supported for operator new\n",stderr);
    if(isEnumType(type))
      fputs(" to create an enum specify the label of the current value\n",stderr);
    handleError(NULL,ERROR_TYPE,wordPos);
  }else if(wordEquals(&word,"cast")){
    requireCompileTimeTypes(&word,&type,1,wordPos);
    pushOperation(state,(Operation){.opType=OP_CAST,.dataType=type,.filePos=wordPos,.dataAs={.sourceType=TYPE_UNDEFINED}});
      return;
  }else if(word.length>1&&charAt(word,0)=='.'){
    word=sliceStart(word,1);//remove first character
    if(bufferedConstants==0||constBuffer[bufferedConstants-1].value.constType!=CONSTANT_TYPE){
      IntOrErrorCode index=parseInt(word,10);
      if(!index.isError){
        pushOperation(state,(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,
          .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.labelId=LABEL_ID_UNKNOWN,.id=index.as.i64,.isMutable=false}}});
        return;
      }
      pushOperation(state,(Operation){.opType=OP_GET_LABEL,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.string=word}});
      return;
    }
    requireCompileTimeTypes(&word,&type,1,wordPos);//try to get type field of enum
    int64_t index;
    if(!isEnumType(type)||((index=findLabel(getTypeElementLabel(type,0)/*type labels are array starting at 1st label*/,getTypeElementCount(type),&word))==-1)){
      fputs("type ",stderr);
      printTypeName(type,stderr);
      fprintf(stderr," does not have a field \"%"PRI_STR"\"\n",PRI_STR_ARGS(word));
      handleError(NULL,ERROR_TYPE,wordPos);
    }
    if(changeEnumType(&type,true))//change type-class to enum-label
      handleError("could not update enum type",ERROR_MEMORY,wordPos);
    pushOperation(state,opConstant(type,index,wordPos));
    return;
  }else if(word.length>1&&charAt(word,0)=='#'){//compiler command
    word.chars++;//remove first character
    word.length--;
    SlicedString args=sliceAtChar(word,':');
    word=args.head;
    //stack manipulation
    if(wordEquals(&word,"dup")){
      int64_t count=1;
      if(args.tail.length>0){
        IntOrErrorCode p=parseInt(args.tail,10);
        if(p.isError||p.as.i64<=0){
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got \"%"PRI_STR"\"\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
          handleError(NULL,ERROR_SYNTAX,wordPos);
        }
        count=p.as.i64;
      }
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_DUP,.count=count}}});
      return;
    }else if(wordEquals(&word,"drop")){
      int64_t count=1;
      if(args.tail.length>0){
        IntOrErrorCode p=parseInt(args.tail,10);
        if(p.isError||p.as.i64<=0){
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got \"%"PRI_STR"\"\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
          handleError(NULL,ERROR_SYNTAX,wordPos);
        }
        count=p.as.i64;
      }
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_DROP,.count=count}}});
      return;
    }else if(wordEquals(&word,"swap")){//XXX rot:N:K -> stack rotation
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_SWAP,.count=1}}});
      return;
    }else if(wordEquals(&word,"over")){
      int64_t count=1;
      if(args.tail.length>0){
        IntOrErrorCode p=parseInt(args.tail,10);
        if(p.isError||p.as.i64<=0){
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got \"%"PRI_STR"\"\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
          handleError(NULL,ERROR_SYNTAX,wordPos);
        }
        count=p.as.i64;
      }
      pushOperation(state,(Operation){.opType=OP_MODIFY_STACK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.stackMod={.op=STACK_OP_OVER,.count=count}}});
      return;
    }
    //compile-time code
    if(wordEquals(&word,"namespace")){
      if(localScopeCount>0){
        fprintf(stderr,"#%"PRI_STR" can only be used at global level\n",PRI_STR_ARGS(word));
        handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      word=nextWord(codeFile,&wordType);
      wordPos=codeFile->wordStart;
      if(wordType!=WORD_TYPE_IDENTIFIER)
        handleError("namespace names have to be identifiers",ERROR_SYNTAX,wordPos);
      if(word.length==0||charAt(word,0)=='#'||containsChar(word,'.')){
        fprintf(stderr,"\"%"PRI_STR"\" is not a valid namespace name",PRI_STR_ARGS(word));
        handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      startNamespace(parserNamespace(state),word,wordPos);
      return;
    }else if(wordEquals(&word,"using")){
      word=nextWord(codeFile,&wordType);
      wordPos=codeFile->wordStart;
      if(wordType!=WORD_TYPE_IDENTIFIER)
        handleError("namespace names have to be identifiers",ERROR_SYNTAX,wordPos);
      importNamespace(parserNamespace(state),word,wordPos);
      return;
    }else if(wordEquals(&word,"end")){
      if(localScopeCount>0){
        fprintf(stderr,"#%"PRI_STR" can only be used at global level\n",PRI_STR_ARGS(word));
        handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      endCompileTimeBlock(parserNamespace(state),wordPos);
      return;
    }else if(wordEquals(&word,"include")){
      if(localScopeCount>0){
        fprintf(stderr,"#%"PRI_STR" can only be used at global level\n",PRI_STR_ARGS(word));
        handleError(NULL,ERROR_SYNTAX,wordPos);
      }
      word=nextWord(codeFile,&wordType);
      wordPos=codeFile->wordStart;
      if(wordType==WORD_TYPE_STRING){//local include
        int64_t len=basePath.length+word.length+1;
        char* includePath=malloc(len*sizeof(char));
        memcpy(includePath,basePath.chars,basePath.length*sizeof(char));
        memcpy(includePath+basePath.length,word.chars,word.length*sizeof(char));
        includePath[len-1]='\0';
        includeFile(state,includePath,wordPos);
        return;
      }
      if(wordType==WORD_TYPE_IDENTIFIER){//library include
        int64_t len=libPath.length+word.length+strlen(".concat")+1;
        char* includePath=malloc(len*sizeof(char));
        memcpy(includePath,libPath.chars,libPath.length*sizeof(char));
        memcpy(includePath+libPath.length,word.chars,word.length*sizeof(char));
        memcpy(includePath+libPath.length+word.length,".concat",strlen(".concat")*sizeof(char));
        includePath[len-1]='\0';
        includeFile(state,includePath,wordPos);
        return;
      }
      handleError("included names have to be identifiers or strings",ERROR_SYNTAX,wordPos);
    }
    //compiler commands
    if(wordEquals(&word,"types")){
      int64_t count=-1;
      if(args.tail.length>0){
        IntOrErrorCode p=parseInt(args.tail,10);
        if(p.isError||p.as.i64<=0){
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got \"%"PRI_STR"\"\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
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
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got \"%"PRI_STR"\"\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
          handleError(NULL,ERROR_SYNTAX,wordPos);
        }
        count=p.as.i64;
      }
      pushOperation(state,(Operation){.opType=OP_COMPILER_INFO,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.compilerInfo={.infoType=COMPILERINFO_STACK,.maxCount=count}}});
      return;
    }else if(wordEquals(&word,"ops")){
      int64_t count=-1;
      if(args.tail.length>0){
        IntOrErrorCode p=parseInt(args.tail,10);
        if(p.isError||p.as.i64<=0){
          fprintf(stderr,"unexpected argument for %"PRI_STR" expected positive integer got \"%"PRI_STR"\"\n",PRI_STR_ARGS(word),PRI_STR_ARGS(args.tail));
          handleError(NULL,ERROR_SYNTAX,wordPos);
        }
        count=p.as.i64;
      }
      puts("-----------------");
      ProgramFile const* f=&state->files[state->currentFile];
      size_t opCount=localScopeCount>0?f->localOpCount:f->globalOpCount;
      size_t i0=(count<0||((size_t)count)>opCount)?0:opCount-count;
      for(size_t i=i0;i<opCount;i++){
        printOperation((localScopeCount>0?f->localOps:f->globalOps)[i],stdout);
      }
      puts("-----------------");
      return;
    }else if(wordEquals(&word,"find")){
      LabelId labelId=readLabel(codeFile,"identifier names",parserNamespace(state)->current,0);
      wordPos=codeFile->wordStart;
      Label const* varName=label(labelId,wordPos);
      ScopeNode* asIdentifier;
      puts("-----------------");
      printf("identifier \"%"PRI_STR"\":\n",PRI_STR_ARGS(varName->label));
      searchIdentifier(getGlobalScopeParser(state),*parserNamespace(state),varName->label,&asIdentifier,wordPos,true);//try to parse variable as identifier
      puts("-----------------");
      //TODO resolve global identifiers with later declarations pre-declared types identifiers
      return;
    }
    fprintf(stderr,"unknown compile time operation \"%"PRI_STR"\"\n",PRI_STR_ARGS(word));
    handleError(NULL,ERROR_SYNTAX,wordPos);
  }
  if(wordEquals(&word,"+")){
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
  }else if(wordEquals(&word,"/u")){
    pushOperation(state,opBinaryOperator(UDIVIDE,wordPos));
    return;
  }else if(wordEquals(&word,"%")){
    pushOperation(state,opBinaryOperator(MOD,wordPos));
    return;
  }else if(wordEquals(&word,"%u")){
    pushOperation(state,opBinaryOperator(UMOD,wordPos));
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
  }else if(wordEquals(&word,"<<")){
    pushOperation(state,opBinaryOperator(LSHIFT,wordPos));
    return;
  }else if(wordEquals(&word,">>")){
    pushOperation(state,opBinaryOperator(RSHIFT,wordPos));
    return;
  }else if(wordEquals(&word,">>u")){
    pushOperation(state,opBinaryOperator(URSHIFT,wordPos));
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
  }else if(wordEquals(&word,"!")){
    pushOperation(state,opUnaryOperator(NOT,wordPos));
    return;
  }else if(wordEquals(&word,"=::")){//automatically choose type of declared variable
    LabelId labelId=readLabel(codeFile,localScopeCount>0?"local variables":"global variables",parserNamespace(state)->current,identiferFlags);
    wordPos=codeFile->wordStart;
    Label const* varName=label(labelId,wordPos);
    wordPos=codeFile->wordStart;
    IdentifierType idType=localScopeCount>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    TypeId mType=newAutoType(state->autoTypes++);
    if(bufferedConstants>0){
      TypeId constType=peekConstValue()->valueType;
      if(!typeEquals(constType,TYPE_UNDEFINED))
        mType=constType;
      if(typeEquals(mType,TYPE_TYPE)){
        constType=popTypeConstant(wordPos,"type constant",false);
        type=newNamedType(labelId,constType);
        ConstantValue constValue=(ConstantValue){.constType=CONSTANT_TYPE,.valueType=TYPE_TYPE,.as.type=type};
        declareIdentifier(getGlobalScopeParser(state),*parserNamespace(state),labelId,TYPE_TYPE,idType,nextId(idType,state),wordPos,&constValue);
        return;
      }
    }
    ScopeNode const* id=declareIdentifier(getGlobalScopeParser(state),*parserNamespace(state),labelId,mType,idType,nextId(idType,state),wordPos,peekConstValue());
    pushOperation(state,(Operation){.opType=OP_DECLARE,.dataType=mType,.filePos=varName->declaredAt,
      .dataAs={.idInfo={.type=idType,.id=id->id,.labelId=labelId,.isMutable=isMutableLabel(varName)}}});
    return;
  }else if(wordEquals(&word,"=")){
    if(bufferedConstants>0){
      if(!peekConstant()->hasId||peekConstValue()->constType!=CONSTANT_TYPE)
        handleError("cannot assign values to constant",ERROR_SYNTAX,wordPos);
      Label const* mLabel=label(peekConstant()->idInfo.labelId,wordPos);
      if(isExternLabel(mLabel))
        handleError("cannot assign values to extern types",ERROR_SYNTAX,wordPos);
      ScopeNode * prevId;
      int r=getIdentifier(getGlobalScopeParser(state),*parserNamespace(state)/*name-space is still the same*/,mLabel->label,&prevId,wordPos);
      if(r!=0)
        handleError("error while resolving identifier",r,wordPos);
      //get previous value of type constant
      TypeId opaque=popTypeConstant(wordPos,"opaqueType",false);
      if(!isNamedType(opaque)||!typeEquals(unwrapNamedType(opaque),TYPE_UNDEFINED))
        handleError("can only replace opaque named types",ERROR_SYNTAX,wordPos);
      //get new value of constant
      TypeId constType=popTypeConstant(wordPos,"type constant",false);
      if(prevId->idType!=ID_TYPE||prevId->constValue.constType!=CONSTANT_TYPE||!setNamedType(prevId->constValue.as.type,constType)){//can only override named types
        handleError("error while changing Type information",ERROR_MEMORY,wordPos);
      }
      return;
    }
    if(canPeekOperationParser(state)){
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
  }else if(wordEquals(&word,"[]")){
    pushOperation(state,(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,
      .dataAs={.idInfo={.type=ID_ARRAY_ELEMENT,.id=0,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}});
    return;
  }else if(wordEquals(&word,"addrOf")){
    if(bufferedConstants>0){//try getting address of constant
      if(!peekConstant()->hasId)
        handleError("cannot get the address of a constant",ERROR_SYNTAX,wordPos);
      Constant const* constant=peekConstant();
      bufferedConstants--;//remove constant before pushing operation
      pushOperation(state,(Operation){.opType=OP_ADDR_OF,.dataType=constant->value.valueType,
        .filePos=constant->pos,.dataAs={.idInfo=constant->idInfo}});
      return;
    }
    if(canPeekOperationParser(state)&&peekOperation(state,wordPos)->opType==OP_CALL){
      peekOperation(state,wordPos)->dataType=asUnlabeledProc(peekOperation(state,wordPos)->dataType,wordPos);
      peekOperation(state,wordPos)->opType=OP_ADDR_OF;
      return;
    }
    if(canPeekOperationParser(state)&&peekOperation(state,wordPos)->opType==OP_GET){
      peekOperation(state,wordPos)->opType=OP_ADDR_OF;
      return;
    }
    if(canPeekOperationParser(state)&&peekOperation(state,wordPos)->opType==OP_IDENTIFIER){
      peekOperation(state,wordPos)->opType=OP_IDENTIFIER_ADDRESS;
      return;
    }
    if(canPeekOperationParser(state)&&peekOperation(state,wordPos)->opType==OP_GET_LABEL){
      peekOperation(state,wordPos)->opType=OP_ADDR_OF_LABEL;
      return;
    }
    handleError("stack value address",ERROR_UNIMPLEMENTED,wordPos);
    return;
  }else if(wordEquals(&word,"()")){
    pushOperation(state,(Operation){.opType=OP_CALL_PTR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}});
    return;
  }else if(wordEquals(&word,"if")){
    if(openScope(BLOCK_IF,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    pushOperation(state,opCodeBlock(BLOCK_IF,wordPos));
    return;
  }else if(wordEquals(&word,"_if")){
    //no scope change for _if
    pushOperation(state,opCodeBlock(BLOCK_IF2,wordPos));
    return;
  }else if(wordEquals(&word,"while")){
    if(openScope(BLOCK_WHILE,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    pushOperation(state,opCodeBlock(BLOCK_WHILE,wordPos));
    return;
  }else if(wordEquals(&word,"do")){//!!while syntax is different from C:  WHILE cond DO exrp END   do-While: WHILE exrp cond DO END
    closeScope(state);
    if(openScope(BLOCK_WHILE,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    //scope count does not change

    pushOperation(state,opCodeBlock(BLOCK_DO,wordPos));
    return;
  }else if(wordEquals(&word,"else")){
    closeScope(state);
    if(openScope(BLOCK_ELSE,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
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
    if(openScope(BLOCK_SWITCH,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    pushOperation(state,opCodeBlock(BLOCK_SWITCH,wordPos));
    return;
  }else if(wordEquals(&word,"case")){
    closeScope(state);
    if(openScope(BLOCK_CASE,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    //scope count does not change

    pushOperation(state,opCodeBlock(BLOCK_CASE,wordPos));
    return;
  }else if(wordEquals(&word,"default")){
    closeScope(state);
    if(openScope(BLOCK_CASE,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    //scope count does not change

    pushOperation(state,opCodeBlock(BLOCK_DEFAULT,wordPos));
    return;
  }else if(wordEquals(&word,"end")){
    //end operation block before closing scope
    BlockType closed=currentScope();
    pushOperation(state,opEndCodeBlock(closed,wordPos));
    if(closed==BLOCK_PROCEDURE){//exited procedure
      state->currentProcId=-1;
    }
    closeScope(state);
    return;
  }else if(wordEquals(&word,"return")){
    if(state->currentProcId<0){
      handleError("unexpected return statement",ERROR_SYNTAX,wordPos);
    }
    pushOperation(state,(Operation){.opType=OP_RETURN,.dataType=procTypes[state->currentProcId].outType,.filePos=wordPos,.dataAs={0}});
    return;
  }else if(wordEquals(&word,"entryPoint:")){
    if(hasEntryPointParser(state)){
      fputs("program already has an entry point\n  at ",stderr);
      printFilePosition(getEntryPointPosParser(state),stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_SYNTAX,wordPos);
    }
    if(openScope(BLOCK_PROCEDURE,state))
      handleError("scope buffer overflow",ERROR_MEMORY,wordPos);
    pushOperation(state,(Operation){.opType=ENTRY_POINT,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}});
    initEntryPointParser(state);
    return;
  }else if(wordEquals(&word,"print")){
    pushOperation(state,(Operation){.opType=OP_PRINT,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}});//printed type will be determined by type-checker
    return;
  }else if(wordEquals(&word,"mut")){
    handleError("mut can only be used after types or declaration operations ( ':' '=:' '=::' )",ERROR_SYNTAX,wordPos);
    return;
  }

  ScopeNode* asIdentifier;
  int r=getIdentifier(getGlobalScopeParser(state),*parserNamespace(state),word,&asIdentifier,wordPos);//try to parse variable as identifier
  if(r<0)//internal error while reading identifier
    handleError("error while resolving identifier",r,wordPos);
  if(r==0){//identifier
    pushOperation(state,(Operation){.opType=asIdentifier->idType==ID_PROCEDURE?OP_CALL:OP_GET,
      .dataType=asIdentifier->type,.filePos=wordPos,.dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id,.labelId=asIdentifier->labelId,
      .isMutable=isMutableLabelId(asIdentifier->labelId)}}});
    return;
  }
  // could not find identifier, try again in type-check phase
  pushOperation(state,(Operation){.opType=OP_IDENTIFIER,.dataType=TYPE_UNDEFINED,.filePos=wordPos,
    .dataAs={.localLabel=(LocalLabel){.label=newLabel(word,0,parserNamespace(state)->current,codeFile->wordStart),.spaceInfo=*parserNamespace(state)}}});
}


FileId parseFile(ParserState* state,CodeFile* codeFile){
  if(state==NULL){
    handleError("parser state has to be non-null",ERROR_MEMORY,codeFile->currentPos);
    return FILE_ID_NONE;
  }
  size_t opsCap=128;
  Operation*  globalOps=malloc(opsCap*sizeof(Operation));
  if(globalOps==NULL){
    handleError("could not allocate operation array",ERROR_MEMORY,codeFile->currentPos);
    return FILE_ID_NONE;
  }
  Operation*  localOps=malloc(opsCap*sizeof(Operation));
  if(localOps==NULL){
    handleError("could not allocate operation array",ERROR_MEMORY,codeFile->currentPos);
    return FILE_ID_NONE;
  }
  size_t includeCap=16;
  IncludedFile* includes=malloc(includeCap*sizeof(IncludedFile));
  if(includes==NULL){
    handleError("could not allocate includes array",ERROR_MEMORY,codeFile->currentPos);
    return FILE_ID_NONE;
  }
  NamespaceInfo namespaceInfo=(NamespaceInfo){.current=0,.namespaceImports=NAMESPACE_IMPORT_NONE};
  FileId prevFile=state->currentFile;
  FileId included=state->fileCount++;
  if(ensureFilesCap(&state->files,&state->filesCap,state->fileCount+1)){
    handleError("could not allocate files array",ERROR_MEMORY,codeFile->currentPos);
    return FILE_ID_NONE;
  }
  state->files[included]=(ProgramFile){
    .globalScope={0}/*initialized by initScope*/,.namespaceInfo=namespaceInfo,
    .includes=includes,.includeCount=0,.includeCap=includeCap,
    .globalOps=globalOps,.globalOpCount=0,.globalOpCap=opsCap,
    .localOps=localOps,.localOpCount=0,.localOpCap=opsCap,
    .entryPointIndex=-1,.fileName=cstrToStr(codeFile->currentPos.fileName),.id=included,
    };
  initScope(&state->files[included].globalScope,BLOCK_UNKNOWN,state);
  state->currentFile=included;
  while(codeFile->codeSize>0){
    readOperation(state,codeFile);
  }
  state->currentFile=prevFile;
  if(!quietMode)
    printf("  parsed file %"PRI_STR" : %zu global and %zu local operations\n",PRI_STR_ARGS(state->files[included].fileName),
      state->files[included].globalOpCount,state->files[included].localOpCount);
  return included;
}
Program compileToOps(CodeFile* rootFile){
  size_t filesCap=16;
  ProgramFile* progFiles=malloc(filesCap*sizeof(ProgramFile));
  if(progFiles==NULL){
    handleError("could not allocate file-data array",ERROR_MEMORY,rootFile->currentPos);
    exit(EXIT_FAILURE);
  }
  ParserState state=(ParserState){.files=progFiles,.filesCap=filesCap,.fileCount=0,.currentFile=FILE_ID_NONE,
    .localVars=0,.globalVars=0,.currentProcId=-1,.entryFile=FILE_ID_NONE,.autoTypes=0};
  parseFile(&state,rootFile);
  return (Program){.files=state.files,.fileCount=state.fileCount,
    .hasEntryPoint=hasEntryPointParser(&state),.nAutoTypes=state.autoTypes,.autoTypes=NULL};
}

void typeErrorMessage(char const* exprName,TypeId expected,TypeId got){
  fprintf(stderr,"wrong type for %s: expected ",exprName);
  printTypeName(expected,stderr);
  fputs(" got ",stderr);
  printTypeName(got,stderr);
  fputs("\n",stderr);
}

TypeId typeCheckPointerArithmetic(TypeId* inTypes,bool subtract){
  if(!isPointerType(inTypes[0])||isCallableType(inTypes[0]))
    return TYPE_UNDEFINED;//inTypes[0] is no pointer
  if(isIntType(inTypes[1]))
    return inTypes[0];
  if(subtract&&typeEquals(inTypes[0],inTypes[1])){//XXX? ptr - ptr mut
    return TYPE_I64;
  }
  return TYPE_UNDEFINED;
}
TypeId typeCheckArithmetic(TypeId* inTypes){
  if(!isPrimitiveType(inTypes[0])||!isPrimitiveType(inTypes[1]))
    return TYPE_UNDEFINED;//arithmetic only on primitive types
  int r1=numberRank(primitiveTypeData(inTypes[0]));
  int r2=numberRank(primitiveTypeData(inTypes[1]));
  if(isInteger(primitiveTypeData(inTypes[0]))!=isInteger(primitiveTypeData(inTypes[1])))
    return TYPE_UNDEFINED;//implicit int to float conversion
  if(r1<=0||r2<=0)
    return TYPE_UNDEFINED;
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_UNDEFINED)
    return TYPE_UNDEFINED;
  inTypes[0]=primitiveType(res);
  inTypes[1]=inTypes[0];
  return inTypes[0];
}
TypeId typeCheckCompare(TypeId* inTypes){
  if(!isPrimitiveType(inTypes[0])||!isPrimitiveType(inTypes[1]))
    return TYPE_UNDEFINED;//comparison only on primitive types
  int r1=numberRank(primitiveTypeData(inTypes[0]));
  int r2=numberRank(primitiveTypeData(inTypes[1]));
  if(isInteger(primitiveTypeData(inTypes[0]))!=isInteger(primitiveTypeData(inTypes[1])))
    return TYPE_UNDEFINED;//implicit int to float conversion
  if(r1<=0||r2<=0)
    return TYPE_UNDEFINED;//comparison only between numbers
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_UNDEFINED)
    return TYPE_UNDEFINED;
  inTypes[0]=primitiveType(res);
  inTypes[1]=inTypes[0];
  return TYPE_BOOL;
}
TypeId typeCheckIntArithmetic(TypeId* inTypes){
  if(!isIntType(inTypes[0])||!isIntType(inTypes[1]))
    return TYPE_UNDEFINED;//both arguments have to be integers
  int r1=numberRank(primitiveTypeData(inTypes[0]));
  int r2=numberRank(primitiveTypeData(inTypes[1]));
  //r1 and r2 both are valid numbers
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_UNDEFINED)
    return TYPE_UNDEFINED;
  inTypes[0]=primitiveType(res);
  inTypes[1]=inTypes[0];
  return inTypes[0];
}

typedef struct{
  TypeId type;
  int32_t opCount;
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

  TypeId switchType;

  SwitchData* switchData;

  bool endReachable;
  bool explicitBreak;
}SwitchBlockInfo;
typedef struct{
  TypeId returnType;
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
  int32_t nAutoTypes;
  TypeId* autoTypes;
  bool reachable;//is current code position reachable
  bool hasCheckBounds;
  bool hasCheckEnum;
}TypeCheckState;

int32_t newTmpId(TypeCheckState* state){
  return state->tmpCount++;
}

//prints the type stack, if maxTypes>=0 only maxTypes many elements are printed
void printTypeStack(TypeCheckState* state,bool printOps,int64_t maxTypes,FILE* out){
  size_t offset=state->opStackCount;
  for(int64_t k=state->typeCount-1;k>=0;k--){
    if(maxTypes--==0)
      return;//reached limit
    if(state->typeStack[k].isWritable)
      fputs("writable ",out);
    printTypeName(state->typeStack[k].type,out);
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
  free(state->compiledOperations);
  state->compiledOperations=NULL;
  free(state->opStack);
  state->opStack=NULL;
  free(state->typeStack);
  state->typeStack=NULL;
  free(state->openBlocks);
  state->openBlocks=NULL;
  free(state->autoTypes);
  state->autoTypes=NULL;
}

TypeInfo* peekTypeStack(TypeCheckState* state){
  return &state->typeStack[state->typeCount-1];
}
void setTypeStackTypeOffset(TypeCheckState* state,size_t offset,TypeId newType){
  state->typeStack[state->typeCount-offset].type=newType;
  state->typeStack[state->typeCount-offset].isWritable=false;
}
void setTypeStackType(TypeCheckState* state,TypeId newType){
  setTypeStackTypeOffset(state,1,newType);
}
void setTypeStackFlags(TypeCheckState* state,bool writable){
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
  if(ensureCompiledOpCap(state,state->opCount+1)){
    handleError("exceeded operation capacity",ERROR_MEMORY,op.filePos);
  }
  state->compiledOperations[state->opCount++]=op;
}
void pushCompiledOperations(TypeCheckState* state,Operation* ops,size_t count){
  if(count==0)
    return;//nothing to do
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
  for(size_t i=state->typeCount-nStackValues;i<state->typeCount;i++){
    //extract multi-element operations and array constants to tmp variable
    if(state->typeStack[i].opCount>1){
      if(keepWritable&&state->typeStack[i].isWritable){
        memmove(state->opStack+newOffset,state->opStack+offset,state->typeStack[i].opCount);
        offset+=state->typeStack[i].opCount;
        newOffset+=state->typeStack[i].opCount;
        continue;
      }
      int32_t tmpId=newTmpId(state);
      TypeId type=state->typeStack[i].type;
      pushCompiledOperation(state,opDeclareIntermediate(type,tmpId,state->opStack[offset].filePos));
      pushCompiledOperations(state,state->opStack+offset,state->typeStack[i].opCount);
      state->opStack[newOffset++]=opGetIntermediate(type,tmpId,state->opStack[offset].filePos);
      offset+=state->typeStack[i].opCount;
      state->typeStack[i].opCount=1;
      if(!keepWritable)
        state->typeStack[i].isWritable=state->opStack[newOffset-1].dataAs.idInfo.isMutable;
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
    if(!typeEquals(typeSource->types[i].type,valueSource->types[i].type)){
      fprintf(stderr,"different branches of %s statement do not match up\n",blockName);
      fputs("expected ",stderr);
      printTypeName(valueSource->types[i].type,stderr);
      fputs(" got ",stderr);
      printTypeName(typeSource->types[i].type,stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_TYPE,pos);
    }
    state->compiledOperations[opOffset]=typeSource->ops[i];
    state->compiledOperations[opOffset].opType=OP_DECLARE;
    opOffset++;
    memcpy(state->compiledOperations+opOffset,valueSource->ops+inTypesOffset,(valueSource->types[i].opCount)*sizeof(Operation));
    inTypesOffset+=valueSource->types[i].opCount;
    opOffset+=valueSource->types[i].opCount;
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
      handleError("allocation of stack-state failed",ERROR_MEMORY,pos);
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
    if(!initStackState&&!typeEquals(state->typeStack[i].type,expectedState->types[i].type)){
      fprintf(stderr,"wrong type at end of %s expected ",errorMessage);
      printTypeName(expectedState->types[i].type,stderr);
      fputs(" got ",stderr);
      printTypeName(state->typeStack[i].type,stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_TYPE,pos);
    }
    varId=initStackState?newTmpId(state):stackState->ops[i].dataAs.idInfo.id;
    //save stack-elements to tmp-values
    if(declare){
      pushCompiledOperation(state,opDeclareTmpVar(state->typeStack[i].type,varId,pos));
    }else{
      pushCompiledOperation(state,opSetTmpVar(state->typeStack[i].type,varId,pos));
    }
    pushCompiledOperations(state,state->opStack+offset-state->typeStack[i].opCount,state->typeStack[i].opCount);
    offset-=state->typeStack[i].opCount;
    if(!ignoreFirst){//remove element from stack if ignoreFirst false
      state->opStackCount-=state->typeStack[i].opCount;
      state->typeCount--;
    }
    if(initStackState){
      stackState->types[i]=(TypeInfo){.opCount=1,.type=state->typeStack[i].type,.isWritable=false};
      stackState->ops[i]=opGetTmpVar(state->typeStack[i].type,varId,pos);
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

bool sizesCompatible(ArrayType const* src,ArrayType const* target){
  if(!target->fixedSize)
    return true;
  if(!src->fixedSize)
    return false;
  if(src->dims<target->dims)
    return false;
  for(int32_t i=0;i<target->dims;i++){
    if(src->sizes[i].isInt!=src->sizes[i].isInt||src->sizes[i].value!=target->sizes[i].value)
      return false;
  }
  return true;
}
bool canAutoCast(TypeId src,TypeId target){//? allow cast T ptr mut ptr -> T ptr ptr (allow allow casting mut away if out pointers are const)
  if(typeEquals(src,target))
    return true;
  if(isNamedType(target)&&!isNamedType(src))
    target=unwrapNamedType(target);
  if(isEnumLabel(src,target))
    return true;//allow auto-cast from enum to enum-label
  if((isPointerType(src)||isArrayType(src))&&isPointerType(target)&&(isMutableType(src)||!isMutableType(target))&&
    sizesCompatible(arrayTypeData(src),arrayTypeData(target))&&typeEquals(getBaseType(src),getBaseType(target)))
    return true;//assigning fixed-size array/pointer to pointer / pointer to constant pointer
  if(!isPrimitiveType(src)||!isPrimitiveType(target))
    return false;
  return isInteger(primitiveTypeData(src))&&isInteger(primitiveTypeData(target))&&
    numberRank(primitiveTypeData(src))<=numberRank(primitiveTypeData(target));//implicit casts only from small int to large int
}
bool canCast(TypeId src,TypeId target){
  if(canAutoCast(src,target))
    return true;
  //XXX cast between arrays of different dimensions
  return numberRank(primitiveTypeData(src))>-1&&numberRank(primitiveTypeData(target))>-1;//casts only between numbers
}

void requireTypes(char const* opName,TypeCheckState* state,TypeId const* types,size_t nTypes,FilePosition pos){
  if(state->typeCount<nTypes){
    fprintf(stderr,"not enough types for %s need %zu have %zu\n",opName,nTypes,state->typeCount);
    handleError(NULL,ERROR_TYPE,pos);
  }
  int32_t nCasts=0;
  size_t offset=state->opStackCount;
  //check types
  for(size_t k=1;k<=nTypes;k++){
    offset-=state->typeStack[state->typeCount-k].opCount;
    if(typeEquals(types[nTypes-k],state->typeStack[state->typeCount-k].type))
      continue;
    if(canAutoCast(state->typeStack[state->typeCount-k].type,types[nTypes-k])){
      if(state->typeStack[state->typeCount-k].opCount==1&&state->opStack[offset].opType==OP_CONSTANT){//change constant to correct type
        setTypeStackTypeOffset(state,k,types[nTypes-k]);
        state->opStack[offset].dataType=types[nTypes-k];
        continue;
      }
      nCasts++;
      continue;
    }
    //convert enum labels to enum constants
    if(isEnumLabel(types[nTypes-k],state->typeStack[state->typeCount-k].type)){
      if(state->typeStack[state->typeCount-k].opCount>1||state->opStack[offset].opType!=OP_CONSTANT){
        handleError("unexpected operation with type ENUM_LABEL",ERROR_SYNTAX,pos);//enum-label type should only exist on enum-label constants
      }
      if(changeEnumType(&state->typeStack[state->typeCount-k].type,false))
        handleError("could not update enum type",ERROR_MEMORY,pos);
      if(!typeEquals(getTypeElements(state->typeStack[state->typeCount-k].type)[state->opStack[offset].dataAs.i64],TYPE_UNDEFINED)){
        String label=getLabelName(getTypeElementLabel(state->typeStack[state->typeCount-k].type,state->opStack[offset].dataAs.i64));
        fprintf(stderr,"missing data value for creating enum constant %"PRI_STR" in ",PRI_STR_ARGS(label));
        printTypeName(state->typeStack[state->typeCount-k].type,stderr);
        fputs("\nto create enum values with data use the 'new' operator\n",stderr);
        handleError(NULL,ERROR_SYNTAX,pos);
      }
      state->opStack[offset].opType=OP_NEW;
      if(changeEnumType(&state->opStack[offset].dataType,false))
        handleError("could not update enum type",ERROR_MEMORY,pos);
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
    if(typeEquals(types[nTypes-k],state->typeStack[state->typeCount-k].type))
      continue;
    memmove(state->opStack+offset+nCasts,state->opStack+offset,shiftCount*sizeof(Operation));
    shiftCount=0;
    nCasts--;
    if(canCast(state->typeStack[state->typeCount-k].type,types[nTypes-k])){
      state->opStack[offset+nCasts]=(Operation){.opType=OP_CAST,.filePos=pos,.dataType=types[nTypes-k],.dataAs={.sourceType=state->typeStack[state->typeCount-k].type}};
      setTypeStackTypeOffset(state,k,types[nTypes-k]);
      state->typeStack[state->typeCount-k].opCount++;
      continue;
    }
    handleError("unexpected type-conversion",ERROR_UNIMPLEMENTED,pos);
  }
}

void pushType(TypeCheckState* state,TypeId dataType,FilePosition pos){
  if(ensureTypeStackCap(state,state->typeCount+1)){
    handleError("exceeded type stack capacity",ERROR_MEMORY,pos);
  }
  state->typeStack[state->typeCount++]=(TypeInfo){.type=dataType,.opCount=1,.isWritable=false};
}
void pushValue(TypeCheckState* state,Operation op){
  if(ensureOpStackCap(state,state->opStackCount+1)){
    handleError("exceeded operation stack capacity",ERROR_MEMORY,op.filePos);
  }
  if(state->blockCount>0&&isMultiValueType(op.dataType)&&(op.opType!=OP_GET||!op.dataAs.idInfo.isMutable)){//make composite stack-values mutable
    int32_t tmpId=newTmpId(state);
    pushCompiledOperation(state,opDeclareIntermediate(op.dataType,tmpId,op.filePos));
    pushCompiledOperation(state,op);
    op=opGetIntermediate(op.dataType,tmpId,op.filePos);
  }
  state->opStack[state->opStackCount++]=op;
  pushType(state,op.dataType,op.filePos);
  if(op.opType==OP_GET){
    setTypeStackFlags(state,op.dataAs.idInfo.isMutable);
  }
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
  TypeId calledType=op->dataType;
  if(isPtr){
    if(state->typeCount<1||!isCallableType(state->typeStack[state->typeCount-1].type)){
      fprintf(stderr,"the argument of %s has to be a callable Type\n",opName(op->opType));
      handleError(NULL,ERROR_TYPE,op->filePos);
    }
    calledType=state->typeStack[state->typeCount-1].type;
    if(isPointerType(calledType))
      calledType=getBaseType(calledType);
    op->dataType=calledType;
  }
  if(!isCallableType(calledType)){
    fputs("cannot call objects of type ",stderr);
    printTypeName(calledType,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_TYPE,op->filePos);
  }
  ProcedureType const* procType=procTypeData(calledType);
  CompositeType const* outTypes=compositeTypeData(procType->outType);
  CompositeType const* inTypes=compositeTypeData(procType->inType);
  size_t argCount=inTypes->typeCount;
  size_t totalOps=0;
  if(procType->staticArgsCount>0){
    ConstantValue* argValues=calloc(procType->staticArgsCount,sizeof(ConstantValue));
    int64_t opOffset=state->opStackCount-(isPtr?state->typeStack[state->typeCount-1].opCount:0);
    int64_t typeOffset=state->typeCount-(isPtr?1:0);
    int32_t argId=procType->staticArgsCount-1;
    if(ensureTypeStackCap(state,state->typeCount+procType->staticArgsCount)||ensureOpStackCap(state,state->opStackCount+procType->staticArgsCount))
      handleError("allocating memory for static arguments failed",ERROR_MEMORY,op->filePos);
    for(int32_t i=getTypeElementCount(procType->inType)-1;i>=0;i--){
      if(argId<0)
        break;//resolved all arguments
      if(argId>=0&&i==staticArgIndex(&procType->staticArgs[argId])&&argValues[argId].constType!=CONSTANT_NONE){//insert constant value
        memmove(state->typeStack+typeOffset+1,state->typeStack+typeOffset,(state->typeCount-typeOffset)*sizeof(TypeInfo));
        state->typeStack[typeOffset]=(TypeInfo){.type=argValues[argId].valueType,.opCount=1,.isWritable=false};
        typeOffset--;
        state->typeCount++;
        memmove(state->opStack+opOffset+1,state->opStack+opOffset,(state->opStackCount-opOffset)*sizeof(Operation));
        if(typeEquals(argValues[argId].valueType,TYPE_TYPE)){
          if(argValues[argId].as.type.class==TYPECLASS_GENERIC_TYPE)
            state->opStack[opOffset]=opGetArgument(TYPE_TYPE,autoTypeId(argValues[argId].as.type),LABEL_ID_UNKNOWN,op->filePos);
          else
            state->opStack[opOffset]=opTypeConstant(argValues[argId].valueType,argValues[argId].as.type,op->filePos);
          state->opStackCount++;
          argId--;
          continue;
        }else if(isIntType(argValues[argId].valueType)){
          if(argValues[argId].constType==GENERIC_INT)
            state->opStack[opOffset]=opGetArgument(argValues[argId].valueType,argValues[argId].as.i64,LABEL_ID_UNKNOWN,op->filePos);
          else
            state->opStack[opOffset]=opConstant(argValues[argId].valueType,argValues[argId].as.i64,op->filePos);
          state->opStackCount++;
          argId--;
          continue;
        }
        handleError("unsupported static argument type",ERROR_UNIMPLEMENTED,op->filePos);
        continue;
      }
      if(typeOffset--<=0)//XXX better message
        handleError("not enough arguments for procedure",ERROR_SYNTAX,op->filePos);
      opOffset-=state->typeStack[typeOffset].opCount;
      if(opOffset<0)
        handleError("types and operations out of sync",ERROR_MEMORY,op->filePos);
      if(argId>=0&&i==staticArgIndex(&procType->staticArgs[argId])){//get constant value
        if(state->opStack[opOffset].opType!=OP_CONSTANT)//XXX allow static procedure arguments as values
          handleError("static arguments have to be constants",ERROR_SYNTAX,state->opStack[opOffset].filePos);
        if(!canAutoCast(state->opStack[opOffset].dataType,procType->staticArgs[argId].type)){
          typeErrorMessage("static procedure argument",procType->staticArgs[argId].type,state->opStack[opOffset].dataType);
          handleError(NULL,ERROR_TYPE,state->opStack[opOffset].filePos);
        }
        if(typeEquals(procType->staticArgs[argId].type,TYPE_TYPE)){
          argValues[argId]=(ConstantValue){.constType=CONSTANT_TYPE,.valueType=procType->staticArgs[argId].type,.as.type=state->opStack[opOffset].dataAs.sourceType};
          argId--;
          continue;
        }
        if(isIntType(procType->staticArgs[argId].type)){
          argValues[argId]=(ConstantValue){.constType=CONSTANT_INT,.valueType=procType->staticArgs[argId].type,.as.i64=state->opStack[opOffset].dataAs.i64};
          argId--;
          continue;
        }
        handleError("unsupported static argument type",ERROR_UNIMPLEMENTED,state->opStack[opOffset].filePos);
        continue;
      }
      resolveTypeGenerics(state->typeStack[typeOffset].type,compositeTypeData(procType->inType)->types[i],procType->staticArgs,argValues,procType->staticArgsCount);
    }
    inTypes=compositeTypeData(replaceGenericTypes(procType->inType,procType->staticArgs,argValues,procType->staticArgsCount));
    outTypes=compositeTypeData(replaceGenericTypes(procType->outType,procType->staticArgs,argValues,procType->staticArgsCount));
  }
  if(state->typeCount<argCount){
    fprintf(stderr,"not enough operands for procedure call: need %zu got %zu\n",argCount,state->typeCount);
    handleError(NULL,ERROR_TYPE,op->filePos);
  }
  //extract operations
  extractCompositeOps(state,argCount+(isPtr?1:0),false);
  int32_t tmpId=-1;
  if(outTypes->typeCount!=0){//store non-void return in temp variable
    tmpId=newTmpId(state);
    pushCompiledOperation(state,opDeclareIntermediate(outTypes->typeCount==1?(outTypes->types[0]):procType->outType,tmpId,op->filePos));
  }
  addCompiledOps(state,*op,isPtr?1:0);
  requireTypes("procedure argument",state,inTypes->types,inTypes->typeCount,op->filePos);
  size_t offset=state->typeCount-inTypes->typeCount;
  for(int32_t i=0;i<inTypes->typeCount;i++){
    totalOps+=state->typeStack[offset+i].opCount;
  }
  //update op-stack
  addCompiledStackOps(state,*op/*ignored*/,inTypes->typeCount,false);
  if(outTypes->typeCount==0)
    return;//no need to update stack if called function returns void
  //add values of call
  if(outTypes->typeCount==1){//single return value
    pushValue(state,opGetIntermediate(outTypes->types[0],tmpId,op->filePos));
    return;
  }
  //auto-unwrap multi-return values
  if(ensureTypeStackCap(state,state->typeCount+outTypes->typeCount)||ensureOpStackCap(state,state->opStackCount+3*outTypes->typeCount)){
    handleError("exceeded op-stack capacity",ERROR_MEMORY,op->filePos);
  }
  for(int32_t e=0;e<outTypes->typeCount;e++){
    state->typeStack[state->typeCount++]=(TypeInfo){.type=outTypes->types[e],.opCount=3,.isWritable=false};
    state->opStack[state->opStackCount++]=(Operation){.opType=OP_GET,.dataType=procType->outType,.filePos=op->filePos,
      .dataAs={.idInfo={.type=ID_TUPLE,.id=1,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}};
    state->opStack[state->opStackCount++]=opGetIntermediate(procType->outType,tmpId,op->filePos);
    state->opStack[state->opStackCount++]=(Operation){.opType=OP_GET,.dataType=outTypes->types[e],.filePos=op->filePos,
      .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.labelId=LABEL_ID_UNKNOWN,.id=e,.isMutable=false}}};
  }
}
void pushProcArgs(TypeCheckState* state,TypeId procType,FilePosition pos){
  if(!isProcedureType(procType)){
    handleError("procedure type has to be callable",ERROR_TYPE,pos);
  }
  if(typeElementsLabeled(procTypeData(procType)->inType))
    return;//do not push values with input is labeled
  CompositeType const* inTypes=compositeTypeData(procTypeData(procType)->inType);
  if(inTypes->typeCount==0)
    return;//no input arguments
  if(inTypes->typeCount==1){
    pushValue(state,opGetArgument(inTypes->types[0],0,inTypes->labelOffset,pos));
    return;
  }
  for(int32_t i=0;i<inTypes->typeCount;i++){
    LabelId labelId=inTypes->labelOffset==LABEL_ID_UNKNOWN?LABEL_ID_UNKNOWN:inTypes->labelOffset+i;
    pushValue(state,opGetArgument(inTypes->types[i],i,labelId,pos));
  }
}

void typeCheckSetVariable(TypeCheckState* state,Operation* op){
  if(!op->dataAs.idInfo.isMutable){
    LabelId labelId=op->dataAs.idInfo.labelId;
    if(labelId==-1){
      handleError("variable is not mutable",ERROR_TYPE,op->filePos);
    }
    Label const* mLabel=label(labelId,op->filePos);
    fprintf(stderr,"variable %"PRI_STR" is not mutable\n",PRI_STR_ARGS(mLabel->label));
    fprintf(stderr,"  %"PRI_STR" was declared at",PRI_STR_ARGS(mLabel->label));
    printFilePosition(mLabel->declaredAt,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_TYPE,op->filePos);
  }
  requireTypes("variable assignment",state,&op->dataType,1,op->filePos);
  addCompiledStackOps(state,*op,1,true);
}
void typeCheckSetStackValue(TypeCheckState* state,Operation const* op,TypeId valType){
  addCompiledStackOps(state,*op,1,false);
  requireTypes("value assignment",state,&valType,1,op->filePos);
  addCompiledStackOps(state,*op,1,false);
}
TypeId getAddressType(TypeId eltType,Operation* op){
  if(isArrayType(eltType)){//get address of fixed-sized array type as array-view
    ArrayType const* arrayData=arrayTypeData(eltType);
    op->opType=OP_ADDR_OF_ARRAY;
    return arrayType(true,getBaseType(eltType),arrayData->dims,arrayData->sizes,op->dataAs.idInfo.isMutable);
  }
  return arrayType(true,eltType,0,NULL,op->dataAs.idInfo.isMutable);
}

bool canWriteTupleElement(TypeId tupleType,int32_t index,FilePosition pos){
  if(!isTupleType(tupleType)){
    fputs("unexpected type for tuple access: ",stderr);
    printTypeName(tupleType,stderr);
    fputs("\n",stderr);
    handleError(NULL,ERROR_MEMORY,pos);
  }
  CompositeType const* tuple=compositeTypeData(tupleType);
  if(tuple->labelOffset!=LABEL_ID_UNKNOWN)
    return isMutableLabelId(tuple->labelOffset+index);
  return true;
}
void checkTupleElementMutable(Operation const* elementAccess,int32_t depth){
  TypeId currentTuple;
  for(int32_t i=0;i<depth;i++){
    if((elementAccess->opType!=OP_GET&&elementAccess->opType!=OP_SET)||elementAccess->dataAs.idInfo.type!=ID_TUPLE_ELEMENT){
      printOperation(*elementAccess,stderr);
      handleError("unexpected operation for tuple access",ERROR_MEMORY,elementAccess->filePos);
    }
    currentTuple=elementAccess->dataType;
    if(!canWriteTupleElement(currentTuple,elementAccess->dataAs.idInfo.id,elementAccess->filePos)){
      fputs("element ",stderr);
      if(typeElementsLabeled(currentTuple)){
        String label=getLabelName(getTypeElementLabel(currentTuple,elementAccess->dataAs.idInfo.id));
        fprintf(stderr,"%"PRI_STR" ",PRI_STR_ARGS(label));
      }
      fprintf(stderr,"(%"PRIi32")",elementAccess->dataAs.idInfo.id);
      fputs(" in ",stderr);
      printTypeName(currentTuple,stderr);
      fputs(" is not mutable\n",stderr);
      if(typeElementsLabeled(currentTuple)){
        fputs("  declared at ",stderr);
        printFilePosition(label(getTypeElementLabel(currentTuple,elementAccess->dataAs.idInfo.id),elementAccess->filePos)->declaredAt,stderr);
        fputs("\n",stderr);
      }
      handleError(NULL,ERROR_SYNTAX,elementAccess->filePos);
    }
    elementAccess++;
  }
}
void typeCheckGetTupleElement(TypeCheckState* state,TypeId tupleType,bool tupleWritable,Operation* op){
  CompositeType const* tuple=compositeTypeData(tupleType);
  size_t offset=state->typeCount-1;
  TypeId eltType=tuple->types[op->dataAs.idInfo.id];
  Operation* blockStart=&(state->opStack[state->opStackCount-state->typeStack[offset].opCount]);
  bool mutable=canWriteTupleElement(tupleType,op->dataAs.idInfo.id,op->filePos);
  if(op->opType==OP_ADDR_OF){//update type of address elements
    eltType=getAddressType(eltType,op);
  }
  if((blockStart->opType==OP_GET||blockStart->opType==OP_SET||blockStart->opType==OP_ADDR_OF)&&(
      blockStart->dataAs.idInfo.type==ID_POINTER||blockStart->dataAs.idInfo.type==ID_POINTER_OFFSET||
      blockStart->dataAs.idInfo.type==ID_ARRAY_ELEMENT||blockStart->dataAs.idInfo.type==ID_TUPLE)){
    if(ensureOpStackCap(state,state->opStackCount+1)){
      handleError("exceeded op-stack capacity",ERROR_MEMORY,op->filePos);
    }
    blockStart->dataAs.idInfo.id++;
    state->opStack[state->opStackCount++]=*op;
    setTypeStackType(state,eltType);
    setTypeStackFlags(state,mutable&tupleWritable);
    state->typeStack[offset].opCount++;
    if(op->opType==OP_SET){
      blockStart->opType=OP_SET;
      checkTupleElementMutable(&state->opStack[state->opStackCount-blockStart->dataAs.idInfo.id],blockStart->dataAs.idInfo.id);
      typeCheckSetStackValue(state,op,eltType);
    }else if(op->opType==OP_ADDR_OF){
      blockStart->opType=OP_ADDR_OF;
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
  insertStackOperation(state,(Operation){.opType=op->opType/*OP_GET, OP_SET or OP_ADDR_OF*/,.dataType=tupleType,
    .dataAs={.idInfo={.type=ID_TUPLE,.id=1,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}},totalOps);
  state->opStack[state->opStackCount++]=*op;
  //update type-stack
  setTypeStackType(state,eltType);
  setTypeStackFlags(state,mutable&tupleWritable);
  state->typeStack[offset].opCount+=2;
  if(op->opType==OP_SET){
    if(!tupleWritable)
      handleError("cannot write to field of constant tuple",ERROR_SYNTAX,op->filePos);
    checkTupleElementMutable(&state->opStack[state->opStackCount-1],1);
    typeCheckSetStackValue(state,op,eltType);
  } 
}
void typeCheckArrayElementAccess(TypeCheckState* state,TypeId arrayType,int32_t indexCount,Operation* op){
  if(indexCount>arrayTypeData(arrayType)->dims){
    fprintf(stderr,"too much indices for %"PRIi32" dimensional array access: %"PRIi32"\n",arrayTypeData(arrayType)->dims,indexCount);
    handleError(NULL,ERROR_SYNTAX,op->filePos);
  }
  size_t typeOffset=state->typeCount-(indexCount+1);
  //wrap composite operations
  extractCompositeOps(state,indexCount,false);//extract all indices as const-variables
  extractCompositeOps(state,indexCount+1,true);//keep array writeable
  ArrayType const* arrayData=arrayTypeData(arrayType);
  if(!arrayData->fixedSize)
    handleError("expected fixed size array",ERROR_MEMORY,op->filePos);
  //check array bounds
  state->hasCheckBounds=1;
  size_t indexOffset=state->opStackCount;
  for(int32_t i=1;i<=indexCount;i++){
    indexOffset-=state->typeStack[state->typeCount-i].opCount;
    pushCompiledOperation(state,(Operation){.opType=OP_CHECK_ARRAY_BOUNDS,.dataType=TYPE_UNDEFINED,.filePos=op->filePos,.dataAs={0}});
    pushCompiledOperations(state,state->opStack+indexOffset,state->typeStack[state->typeCount-i].opCount);//index
    if(!arrayData->sizes[i-1].isInt)
      handleError("non integer array sizes should not exist at this state of compilation",ERROR_MEMORY,op->filePos);
    pushCompiledOperation(state,opConstant(TYPE_I64,arrayData->sizes[i-1].value,op->filePos));
  }
  // ... array indices []
  if(indexCount<arrayData->dims)
    handleError("partial array access",ERROR_UNIMPLEMENTED,op->filePos);
    //TODO OP_GET_SUBARRAY/OP_SET_SUBARRAY
  op->dataAs.idInfo.type=ID_ARRAY_ELEMENT;
  op->dataType=arrayType;
  //update operation stack
  insertStackOperation(state,*op,indexCount+state->typeStack[typeOffset].opCount);
  //update type-stack
  state->typeCount-=indexCount;
  bool writable;
  if(isPointerType(arrayType)){
    writable=isMutableType(state->typeStack[typeOffset].type);
  }else{
    writable=state->typeStack[typeOffset].isWritable;
  }
  setTypeStackType(state,arrayData->base);
  setTypeStackFlags(state,writable);
  state->typeStack[typeOffset].opCount+=(state->opStackCount-indexOffset);
  if(op->opType==OP_SET){
    if(!writable)
      handleError("cannot write to immutable array",ERROR_SYNTAX,op->filePos);
    typeCheckSetStackValue(state,op,arrayData->base);
  }else if(op->opType==OP_ADDR_OF){
    op->dataType=getAddressType(op->dataType,op);
  }
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
      if(typeEquals(op->dataType,TYPE_UNDEFINED))
        handleError("missing type declaration",ERROR_TYPE,op->filePos);
      if(isAutoType(op->dataType)){
        if(autoTypeId(op->dataType)<0||autoTypeId(op->dataType)>=state->nAutoTypes){
          handleError("predeclared id out of expected range",ERROR_TYPE,op->filePos);
        }
        op->dataType=state->autoTypes[autoTypeId(op->dataType)];//get predeceased type
      }
      if(op->opType==OP_SET){
        typeCheckSetVariable(state,op);
        return;
      }
      if(op->opType==OP_ADDR_OF){
        op->dataType=getAddressType(op->dataType,op);
      }
      pushValue(state,*op);
      return;
    case ID_TUPLE_ELEMENT:
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op->opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      offset=state->typeCount-1;
      op->dataType=state->typeStack[offset].type;
      writable=state->typeStack[offset].isWritable;
      if(!isTupleType(op->dataType)){
        printTypeName(op->dataType,stderr);
        fputs(" is not a tuple\n",stderr);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      CompositeType const* tuple=compositeTypeData(op->dataType);
      if(tuple->typeCount<op->dataAs.idInfo.id){
        fprintf(stderr,"index %"PRIi32" exceeds element count of tuple %"PRIi32"\n",op->dataAs.idInfo.id,tuple->typeCount);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      typeCheckGetTupleElement(state,op->dataType,writable,op);
      return;
    case ID_POINTER:
    case ID_ARRAY_ELEMENT:
    case ID_POINTER_OFFSET:
      offset=state->typeCount-1;
      int32_t indexCount=0;
      while(offset>0&&isIntType(state->typeStack[offset].type)){
        offset--;
        indexCount++;
      }
      if(isPointerType(state->typeStack[offset].type)&&isProcedureType(getBaseType(state->typeStack[offset].type)))
        handleError("cannot dereference procedure pointers",ERROR_SYNTAX,op->filePos);
      if(isArrayType(state->typeStack[offset].type)||(isPointerType(state->typeStack[offset].type)&&arrayTypeData(state->typeStack[offset].type)->fixedSize)){
        typeCheckArrayElementAccess(state,state->typeStack[offset].type,indexCount,op);
        return;
      }
      if(indexCount>1){
        fprintf(stderr,"too much indices for pointer access: %"PRIi32" expected 1\n",indexCount);
        handleError(NULL,ERROR_SYNTAX,op->filePos);
      }
      op->dataAs.idInfo.type=indexCount==0?ID_POINTER:ID_POINTER_OFFSET;
      if(!isPointerType(state->typeStack[offset].type)){
        fprintf(stderr,"invalid first operand for %s %s : ",opName(op->opType),idNames[op->dataAs.idInfo.type]);
        printTypeName(state->typeStack[offset].type,stderr);
        fputs(" is not a pointer or an array\n",stderr);
        handleError(NULL,ERROR_TYPE,op->filePos);
      }
      op->dataType=getBaseType(state->typeStack[offset].type);
      //wrap composite operations
      extractCompositeOps(state,indexCount+1+(op->opType==OP_SET),true);
      size_t totalOps=0;
      for(int32_t i=0;i<indexCount+1;i++){
        totalOps+=state->typeStack[offset+i].opCount;
      }
      //update operation stack
      insertStackOperation(state,*op,totalOps);
      //update type-stack
      state->typeCount-=indexCount;
      writable=isMutableType(state->typeStack[offset].type);
      setTypeStackType(state,op->dataType);
      setTypeStackFlags(state,writable);
      state->typeStack[offset].opCount++;
      if(indexCount>0)//index count will be 1
        state->typeStack[offset].opCount+=state->typeStack[offset+1].opCount;
      if(op->opType==OP_SET){
        if(!writable)
          handleError("cannot write to immutable pointer",ERROR_SYNTAX,op->filePos);
        typeCheckSetStackValue(state,op,op->dataType);
      }else if(op->opType==OP_ADDR_OF){
        op->dataType=getAddressType(op->dataType,op);
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
  if(!isProcOutType(op->dataType)){
    fprintf(stderr,"unexpected procedure return type-class: %s\n",typeClassName(op->dataType.class));
    handleError(NULL,ERROR_SYNTAX,op->filePos);
  }
  CompositeType const* outTypes=compositeTypeData(op->dataType);
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
  String mLabel=label(op->dataAs.localLabel.label,op->filePos)->label;
  if(!state->reachable&&op->opType==OP_IDENTIFIER&&blockInfo!=NULL&&
    (blockInfo->type==BLOCK_SWITCH||blockInfo->type==BLOCK_CASE)&&isEnumLabelType(blockInfo->blockDataAs.switchBlock.switchType)){
    CompositeType const* enumType=compositeTypeData(blockInfo->blockDataAs.switchBlock.switchType);
    for(int32_t i=0;i<enumType->typeCount;i++){
      if(stringCompare(mLabel,getLabelName(enumType->labelOffset+i))==0){//identifier is label of current switch
        *op=opConstant(blockInfo->blockDataAs.switchBlock.switchType,i,op->filePos);
        return;
      }
    }
  }
  ScopeNode* asIdentifier;
  int r=getIdentifier(state->globalScope,op->dataAs.localLabel.spaceInfo,mLabel,&asIdentifier,op->filePos);
  if(r!=0){
    fprintf(stderr," unknown identifier \"%"PRI_STR"\"\n",PRI_STR_ARGS(mLabel));
    handleError(NULL,r,op->filePos);
  }
  if(op->opType==OP_SET_IDENTIFIER&&asIdentifier->idType==ID_PROCEDURE)
    handleError("cannot set value of procedure",ERROR_SYNTAX,op->filePos);
  *op=(Operation){.opType=op->opType==OP_SET_IDENTIFIER?OP_SET:(op->opType==OP_IDENTIFIER_ADDRESS?OP_ADDR_OF:(asIdentifier->idType==ID_PROCEDURE)?OP_CALL:OP_GET),
    .dataType=asIdentifier->type,.filePos=op->filePos,
      .dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id,.labelId=asIdentifier->labelId,.isMutable=isMutableLabelId(asIdentifier->labelId)}}};
}
void typeCheckOperation(Operation op,TypeCheckState* state){
  size_t totalOps=0;
  int32_t offset,tmpId;
  BlockInfo blockInfo;
  BlockInfo* blockInfoPtr;
  IfBlockInfo* ifBlock;
  SwitchBlockInfo* switchBlock;
  Label const* mLabel;

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
        if(!canAutoCast(op.dataType,switchBlock->switchType)){
          typeErrorMessage("switch label",switchBlock->switchType,op.dataType);
        }
        if(switchBlock->switchData->labelCount>=switchBlock->switchData->labelCap)
          handleError("exceeded maximum number of allowed switch labels",ERROR_MEMORY,op.filePos);
        for(size_t i=0;i<switchBlock->switchData->labelCount;i++){//check for duplicate labels
          if(switchBlock->switchData->labelData[i].value==op.dataAs.i64){
            if(isEnumLabelType(switchBlock->switchType)){
              String label=getLabelName(getTypeElementLabel(switchBlock->switchType,op.dataAs.i64));
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
      bool keepMutable=false;
      switch(op.dataAs.unOp){
        case INCREMENT:
        case DECREMENT:
          if(!state->typeStack[offset].isWritable){//value has to be
            fprintf(stderr,"operand of unary operator %s has to be writable \n",unOpName(op.dataAs.unOp));
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          keepMutable=true;
          if(isPointerType(state->typeStack[offset].type)&&!arrayTypeData(state->typeStack[offset].type)->fixedSize){
            break;//raw pointer is an allowed type for increment
          }
          if(!isNumberType(state->typeStack[offset].type)){
            fprintf(stderr,"wrong operand type for unary operator %s expected integer ",unOpName(op.dataAs.unOp));
            fputs(" got ",stderr);
            printTypeName(state->typeStack[offset].type,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          break;
        case NEGATE:
          if(!isNumberType(state->typeStack[offset].type)){
            fprintf(stderr,"wrong operand type for unary operator %s expected integer ",unOpName(op.dataAs.unOp));
            fputs(" got ",stderr);
            printTypeName(state->typeStack[offset].type,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          break;
        case FLIP:
          if(!isIntType(state->typeStack[offset].type)){
            fprintf(stderr,"wrong operand type for unary operator %s expected integer ",unOpName(op.dataAs.unOp));
            fputs(" got ",stderr);
            printTypeName(state->typeStack[offset].type,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          break;
        case NOT:
          if(!isBoolType(state->typeStack[offset].type)){
            typeErrorMessage("unary operator NOT",TYPE_BOOL,state->typeStack[offset].type);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          break;
      }
      //update op-stack
      //store result in temp variable
      extractCompositeOps(state,1,keepMutable);
      tmpId=newTmpId(state);
      pushCompiledOperation(state,opDeclareIntermediate(op.dataType,tmpId,op.filePos));
      addCompiledOps(state,op,1);
      //update stack
      pushValue(state,opGetIntermediate(op.dataType,tmpId,op.filePos));
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
      TypeId inTypes[2]={state->typeStack[offset].type,state->typeStack[offset+1].type};
      switch(op.dataAs.binOp){
        case ADD:
        case SUBTRACT:
          op.dataType=typeCheckPointerArithmetic(inTypes,op.dataAs.binOp==SUBTRACT);
          if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
            typesMatch=true;
            break;
          }
          op.dataType=typeCheckArithmetic(inTypes);
          if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
            typesMatch=true;
            break;
          }
          break;
        case MULTIPLY:
        case DIVIDE:
        case MOD:
          op.dataType=typeCheckArithmetic(inTypes);
          if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
            typesMatch=true;
            break;
          }
          break;
        case UDIVIDE:
        case UMOD:
        case LSHIFT:
        case RSHIFT:
        case URSHIFT:
          op.dataType=typeCheckIntArithmetic(inTypes);
          if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
            typesMatch=true;
            break;
          }
          break;
        case AND:
        case OR:
        case XOR:
          //integer bool ops
          op.dataType=typeCheckIntArithmetic(inTypes);
          if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
            typesMatch=true;
            break;
          }
          //bool ops
          if(isBoolType(state->typeStack[offset].type)&&isBoolType(state->typeStack[offset+1].type)){
            op.dataType=TYPE_BOOL;
            typesMatch=true;
            break;
          }
          break;
        case EQ:
        case NE:
          //pointer equality
          if(isPointerType(inTypes[0])&&isPointerType(inTypes[1])&&//XXX? check sizes 
              (arrayTypeData(inTypes[0])->fixedSize==arrayTypeData(inTypes[1])->fixedSize)&&
              typeEquals(getBaseType(inTypes[0]),getBaseType(inTypes[1]))){
            op.dataType=TYPE_BOOL;
            typesMatch=true;
            break;
          }
          //enum-entry equality
          if(isEnumLabel(inTypes[0],inTypes[1])||(isEnumType(inTypes[0])&&typeEquals(inTypes[0],inTypes[1]))){
            if(changeEnumType(&inTypes[0],true))
              handleError("could not update enum type",ERROR_MEMORY,op.filePos);
            op.dataType=TYPE_BOOL;
            typesMatch=true;
            break;
          }
          //number equality
          op.dataType=typeCheckCompare(inTypes);
          if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
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
          if(!typeEquals(op.dataType,TYPE_UNDEFINED)){
            typesMatch=true;
            break;
          }
          break;
      }
      if(!typesMatch){
        fprintf(stderr,"No version of binary operator %s supports the types ",binOpName(op.dataAs.binOp));
        printTypeName(state->typeStack[offset].type,stderr);
        fputs(" ",stderr);
        printTypeName(state->typeStack[offset+1].type,stderr);
        fputs("\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      //update operation stack
      //ensure operands have matching types
      requireTypes("binary operator",state,inTypes,2,op.filePos);
      //store result in temp variable
      extractCompositeOps(state,2,false);
      tmpId=newTmpId(state);
      pushCompiledOperation(state,opDeclareIntermediate(op.dataType,tmpId,op.filePos));
      addCompiledOps(state,op,2);
      //update stack
      pushValue(state,opGetIntermediate(op.dataType,tmpId,op.filePos));
      return;
    case OP_PRINT:
      checkReachable(state,op);
      checkLocal(state,op);
      if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-1;
      if((isPrimitiveType(state->typeStack[offset].type)&&!typeEquals(state->typeStack[offset].type,TYPE_UNDEFINED))||
          (isPointerType(state->typeStack[offset].type)&&!isCallableType(state->typeStack[offset].type))){
        op.dataType=unwrapNamedType(state->typeStack[offset].type);
        //update operations
        extractCompositeOps(state,1,false);
        addCompiledOps(state,op,1);
        return;
      }
      fputs("cannot print values of type ",stderr);
      printTypeName(state->typeStack[offset].type,stderr);
      fputs("\n",stderr);
      handleError(NULL,ERROR_TYPE,op.filePos);
      break;
    case OP_CHECK_ARRAY_BOUNDS:
    case OP_CHECK_ENUM_INDEX:
    case OP_UNREACHABLE:
      break;
    case OP_GET:
    case OP_SET:
    case OP_ADDR_OF:
      typeCheckGet(state,&op);
      return;
    case OP_GET_LABEL:
    case OP_SET_LABEL:
    case OP_ADDR_OF_LABEL:
      checkReachable(state,op);
      checkLocal(state,op);
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      offset=state->typeCount-1;
      TypeId structType=peekTypeStack(state)->type;
      bool writable=peekTypeStack(state)->isWritable;
      totalOps=peekTypeStack(state)->opCount;
      if(isArrayType(structType)||isPointerType(structType)){
        if(!arrayTypeData(structType)->fixedSize){
          printTypeName(structType,stderr);
          fprintf(stderr," does not have a field \"%"PRI_STR"\"\n",PRI_STR_ARGS(op.dataAs.string));
          handleError(NULL,ERROR_SYNTAX,op.filePos);
        }
        if(op.opType==OP_SET_LABEL){
          printTypeName(structType,stderr);
          fprintf(stderr," does not have a writable field \"%"PRI_STR"\"\n",PRI_STR_ARGS(op.dataAs.string));
          handleError(NULL,ERROR_SYNTAX,op.filePos);
        }
        if(op.opType==OP_ADDR_OF_LABEL){
          printTypeName(structType,stderr);
          fprintf(stderr," does not have an addressable field \"%"PRI_STR"\"\n",PRI_STR_ARGS(op.dataAs.string));
          handleError(NULL,ERROR_SYNTAX,op.filePos);
        }
        if(wordEquals(&op.dataAs.string,"length")){
          if(arrayTypeData(structType)->sizes[0].isInt)
            op=opConstant(TYPE_I64,arrayTypeData(structType)->sizes[0].value,op.filePos);
          else
            op=opGetArgument(TYPE_I64,arrayTypeData(structType)->sizes[0].value,LABEL_ID_UNKNOWN,op.filePos);
          //replace array with length (length only depends on type)
          state->opStackCount-=peekTypeStack(state)->opCount;
          insertStackOperation(state,op,0);
          peekTypeStack(state)->opCount=1;
          setTypeStackType(state,TYPE_I64);
          return;
        }
        if(wordEquals(&op.dataAs.string,"size")){
          arrayTypes[arrayTypeData(structType)->id].sizeUsed=true;
          op=(Operation){.opType=OP_GET,.dataType=structType,.filePos=op.filePos,
            .dataAs={.idInfo={.type=ID_ARRAY_SIZE,.id=1,.labelId=-1,.isMutable=false}}};
          //replace array with length (length only depends on type)
          state->opStackCount-=peekTypeStack(state)->opCount;
          insertStackOperation(state,op,0);
          peekTypeStack(state)->opCount=1;
          ArraySize dims=(ArraySize){.value=arrayTypeData(structType)->dims,.isInt=true};
          setTypeStackType(state,arrayType(true,TYPE_I64,1,&dims,false));
          return;
        }
        printTypeName(structType,stderr);
        fprintf(stderr," does not have a field \"%"PRI_STR"\"\n",PRI_STR_ARGS(op.dataAs.string));
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      if(!typeElementsLabeled(structType)){
        printTypeName(structType,stderr);
        fputs(" does not have any labeled type elements\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      CompositeType const* mStruct=compositeTypeData(structType);
      LabelId labelIndex=findLabel(mStruct->labelOffset,mStruct->typeCount,&op.dataAs.string);
      if(labelIndex==-1){
        printTypeName(structType,stderr);
        fprintf(stderr," does not have a field \"%"PRI_STR"\"\n",PRI_STR_ARGS(op.dataAs.string));
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      if(isTupleType(structType)){
        op=(Operation){.opType=(op.opType==OP_ADDR_OF_LABEL)?OP_ADDR_OF:(op.opType==OP_SET_LABEL)?OP_SET:OP_GET,.dataType=structType,.filePos=op.filePos,
          .dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=labelIndex,.labelId=mStruct->labelOffset+labelIndex,.isMutable=false}}};
        typeCheckGetTupleElement(state,structType,writable,&op);
        return;
      }
      if(typeEquals(mStruct->types[labelIndex],TYPE_UNDEFINED)){
        fprintf(stderr,"\"%"PRI_STR"\" in ",PRI_STR_ARGS(op.dataAs.string));
        printTypeName(structType,stderr);
        fputs(" does not hold a value\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      if(writable&&op.opType==OP_SET_LABEL){
        TypeId lableType=structType;
        if(changeEnumType(&lableType,true))
          handleError("could not update enum type",ERROR_MEMORY,op.filePos);
        pushCompiledOperation(state,(Operation){.opType=OP_SET,.filePos=op.filePos,.dataType=lableType,
          .dataAs={.idInfo={.type=ID_ENUM_LABEL,.id=0,.labelId=LABEL_ID_UNKNOWN,.isMutable=false}}});
      }else{
        pushCompiledOperation(state,(Operation){.opType=OP_CHECK_ENUM_INDEX,.dataType=(mStruct->types[labelIndex]),.filePos=op.filePos,.dataAs={.i64=labelIndex}});
      }
      pushCompiledOperations(state,state->opStack+state->opStackCount-totalOps,totalOps);//compile enum ops, but keep on stack
      if(writable&&op.opType==OP_SET_LABEL){
        TypeId lableType=structType;
        if(changeEnumType(&lableType,true))
          handleError("could not update enum type",ERROR_MEMORY,op.filePos);
        pushCompiledOperation(state,opConstant(lableType,labelIndex,op.filePos));
      }
      state->hasCheckEnum=1;
      mLabel=label(mStruct->labelOffset+labelIndex,op.filePos);
      op=(Operation){.opType=(op.opType==OP_ADDR_OF_LABEL)?OP_ADDR_OF:(op.opType==OP_SET_LABEL)?OP_SET:OP_GET,.dataType=mStruct->types[labelIndex],.filePos=op.filePos,
        .dataAs={.idInfo={.type=ID_ENUM_ELEMENT,.id=labelIndex,.labelId=mStruct->labelOffset+labelIndex,.isMutable=isMutableLabel(mLabel)}}};
      if(op.opType==OP_ADDR_OF){
        op.dataType=getAddressType(op.dataType,&op);
      }
      insertStackOperation(state,op,totalOps);
      peekTypeStack(state)->opCount+=totalOps;
      setTypeStackType(state,mStruct->types[labelIndex]);
      setTypeStackFlags(state,isMutableLabel(mLabel)&writable);
      if(op.opType==OP_SET){
        if(!writable)
          handleError("cannot write to field of constant enum",ERROR_SYNTAX,op.filePos);
        if(!isMutableLabel(mLabel)){
          fprintf(stderr,"element %"PRI_STR" (%"PRIi32") in ",PRI_STR_ARGS(mLabel->label),labelIndex);
          printTypeName(structType,stderr);
          fputs(" is not mutable\n",stderr);
          fputs("  declared at ",stderr);
          printFilePosition(mLabel->declaredAt,stderr);
          fputs("\n",stderr);
          handleError(NULL,ERROR_SYNTAX,op.filePos);
        }
        typeCheckSetStackValue(state,&op,op.dataType);
      } 
      return;
    case OP_PRE_DECLARE:
      checkReachable(state,op);
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
          if(typeEquals(op.dataType,TYPE_UNDEFINED)){
            fputs("invalid type for predeclared variable: ",stderr);
            printTypeName(op.dataType,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          addCompiledOps(state,op,0);
          return;
        case ID_PROCEDURE:
          if(!isProcedureType(op.dataType)){
            fputs("invalid type for predeclared procedure: ",stderr);
            printTypeName(op.dataType,stderr);
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
          if(typeEquals(op.dataType,TYPE_UNDEFINED)){
            fputs("cannot declare variables of type: ",stderr);
            printTypeName(op.dataType,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          offset=state->typeCount-1;
          //find types for auto-types
          if(isAutoType(op.dataType)){
            if(autoTypeId(op.dataType)<0||autoTypeId(op.dataType)>=state->nAutoTypes)
              handleError("predeclared id outside expected range",ERROR_TYPE,op.filePos);
            int64_t typeId=autoTypeId(op.dataType);
            op.dataType=state->typeStack[offset].type;
            if(isEnumLabelType(op.dataType)){
              if(changeEnumType(&op.dataType,false))
                handleError("could not update enum type",ERROR_MEMORY,op.filePos);
            }
            state->autoTypes[typeId]=op.dataType;
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
          if(state->blockCount!=0)//predeclare procedure in global section
            handleError("unexpected procedure declaration",ERROR_SYNTAX,op.filePos);
          //block id will be ignored
          blockInfo=(BlockInfo){.type=BLOCK_PROCEDURE,.blockStart=state->opCount,.blockId=-1,.blockDataAs={.procBlock={.returnType=procTypeData(op.dataType)->outType}}};
          if(pushBlock(state,blockInfo))
            handleError("could not push procedure block",ERROR_MEMORY,op.filePos);
          pushCompiledOperation(state,op);
          state->tmpCount=0;
          pushProcArgs(state,op.dataType,op.filePos);
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
      if(isTupleType(op.dataType)){
        offset=state->typeCount-getTypeElementCount(op.dataType);
        requireTypes("tuple creation",state,getTypeElements(op.dataType),getTypeElementCount(op.dataType),op.filePos);
        totalOps=0;
        for(int32_t e=0;e<getTypeElementCount(op.dataType);e++){
          totalOps+=state->typeStack[offset+e].opCount;
        }
        if(state->blockCount==0){//create tuple in-place when in global level
          insertStackOperation(state,op,totalOps);
          state->typeCount-=getTypeElementCount(op.dataType);
          state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1,.isWritable=false};
          return;
        }
        //store result in temp variable
        extractCompositeOps(state,getTypeElementCount(op.dataType),false);
        tmpId=newTmpId(state);
        pushCompiledOperation(state,opDeclareIntermediate(op.dataType,tmpId,op.filePos));
        addCompiledOps(state,op,getTypeElementCount(op.dataType));
        //update stack
        pushValue(state,opGetIntermediate(op.dataType,tmpId,op.filePos));
        return;
      }
      if(isEnumType(op.dataType)){
        TypeId entryData=getTypeElements(op.dataType)[op.dataAs.i64];
        if(typeEquals(entryData,TYPE_UNDEFINED)){
          if(state->blockCount==0){//create enum in-place when in global level
            insertStackOperation(state,op,0);
            state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1,.isWritable=false};
            return;
          }
          tmpId=newTmpId(state);
          pushCompiledOperation(state,opDeclareIntermediate(op.dataType,tmpId,op.filePos));
          addCompiledOps(state,op,0);
          //update stack
          pushValue(state,opGetIntermediate(op.dataType,tmpId,op.filePos));
          return;
        }
        requireTypes("enum creation",state,&entryData,1,op.filePos);
        if(state->blockCount==0){//create enum in-place when in global level
          totalOps=state->typeStack[state->typeCount-1].opCount;
          insertStackOperation(state,op,totalOps);
          state->typeStack[state->typeCount-1]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1,.isWritable=false};
          return;
        }
        extractCompositeOps(state,1,false);
        tmpId=newTmpId(state);
        pushCompiledOperation(state,opDeclareIntermediate(op.dataType,tmpId,op.filePos));
        addCompiledOps(state,op,1);
        //update stack
        pushValue(state,opGetIntermediate(op.dataType,tmpId,op.filePos));
        return;
      }
      if(isArrayType(op.dataType)){
        if(!arrayTypeData(op.dataType)->fixedSize)
          handleError("new is not implemented for var-size arrays",ERROR_UNIMPLEMENTED,op.filePos);
        pushValue(state,op);
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
      if(!canCast(state->typeStack[offset].type,op.dataType)){
        fputs("cannot cast ",stderr);
        printTypeName(state->typeStack[offset].type,stderr);
        fputs(" to ",stderr);
        printTypeName(op.dataType,stderr);
        fputs("\n",stderr);
        handleError(NULL,ERROR_TYPE,op.filePos);
      }
      op.dataAs.sourceType=state->typeStack[offset].type;
      if(typeEquals(op.dataAs.sourceType,TYPE_UNDEFINED))
        handleError("could not find source type",ERROR_MEMORY,op.filePos);
      //store previous result in temp value
      extractCompositeOps(state,1,false);
      tmpId=newTmpId(state);
      pushCompiledOperation(state,opDeclareIntermediate(op.dataType,tmpId,op.filePos));
      addCompiledOps(state,op,1);
      //update stack
      pushValue(state,opGetIntermediate(op.dataType,tmpId,op.filePos));
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
              handleError("ifBlock->inStack allocation failed",ERROR_MEMORY,op.filePos);
            memcpy(ifBlock->inStack.types,state->typeStack,(state->typeCount-1)*sizeof(TypeInfo));
            memcpy(ifBlock->inStack.ops,state->opStack,(ifBlock->inStack.opCount)*sizeof(Operation));
          }
          if(pushBlock(state,blockInfo))
            handleError("could not push if-block",ERROR_MEMORY,op.filePos);

          op.dataType=TYPE_BOOL;
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
              ifBlock->inStack.ops=realloc(ifBlock->inStack.ops,(state->opStackCount-state->typeStack[state->typeCount-1].opCount)*sizeof(Operation));
            }
            ifBlock->inStack.opCount=state->opStackCount-state->typeStack[state->typeCount-1].opCount;
            if(ifBlock->inStack.types==NULL||ifBlock->inStack.ops==NULL){
              handleError("ifBlock->inStack allocation failed",ERROR_MEMORY,op.filePos);
            }
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
          op.dataType=TYPE_BOOL;
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
          op.dataType=TYPE_BOOL;
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
              handleError("switchBlock->inStack allocation failed",ERROR_MEMORY,op.filePos);
            memcpy(switchBlock->inStack.types,state->typeStack,(state->typeCount-1)*sizeof(TypeInfo));
            memcpy(switchBlock->inStack.ops,state->opStack,(switchBlock->inStack.opCount)*sizeof(Operation));
          }
          //determine switch type
          offset=state->typeCount-1;
          if(isIntType(state->typeStack[offset].type)){
            op.dataType=state->typeStack[offset].type;
          }else if(isEnumType(state->typeStack[offset].type)||isEnumLabelType(state->typeStack[offset].type)){
            op.dataType=state->typeStack[offset].type;
            if(changeEnumType(&op.dataType,true))
              handleError("could not update enum type",ERROR_MEMORY,op.filePos);
          }else{
            fputs("cannot switch values of type ",stderr);
            printTypeName(state->typeStack[offset].type,stderr);
            fputs("\n",stderr);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          switchBlock->switchType=op.dataType;
          if(!isSwitchableType(switchBlock->switchType)){
            fputs("switch statements of type ",stderr);
            printTypeName(switchBlock->switchType,stderr);
            fputs("are not supported\n",stderr);
            handleError(NULL,ERROR_UNIMPLEMENTED,op.filePos);
          }
          if(pushBlock(state,blockInfo))
            handleError("could not push switch-block",ERROR_MEMORY,op.filePos);
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
      op.dataAs.block.type=blockInfoPtr->type;
      op.dataAs.block.id=blockInfoPtr->blockId;
      int32_t endCount=1;
      switch(blockInfoPtr->type){
        case BLOCK_IF:
        case BLOCK_ELSE:
          if(op.dataAs.block.type!=BLOCK_IF&&op.dataAs.block.type!=BLOCK_ELSE&&op.dataAs.block.type!=BLOCK_UNKNOWN){
            fprintf(stderr,"unexpected end for %s-block expected end-if\n",blockNames[op.dataAs.block.type]);
            handleError(NULL,ERROR_UNIMPLEMENTED,op.filePos);
          }
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
          if(op.dataAs.block.type!=BLOCK_WHILE&&op.dataAs.block.type!=BLOCK_UNKNOWN){
            fprintf(stderr,"unexpected end for %s-block expected end-while\n",blockNames[op.dataAs.block.type]);
            handleError(NULL,ERROR_UNIMPLEMENTED,op.filePos);
          }
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
          if(op.dataAs.block.type!=BLOCK_PROCEDURE&&op.dataAs.block.type!=BLOCK_UNKNOWN){
            fprintf(stderr,"unexpected end for %s-block expected end-procedure\n",blockNames[op.dataAs.block.type]);
            handleError(NULL,ERROR_UNIMPLEMENTED,op.filePos);
          }
          if(state->reachable&&getTypeElementCount(blockInfoPtr->blockDataAs.procBlock.returnType)>0){//automatically add return statement at end of non-void procedures
            Operation ret=(Operation){.opType=OP_RETURN,.dataType=blockInfoPtr->blockDataAs.procBlock.returnType,.filePos=op.filePos,.dataAs={0}};
            typeCheckReturn(state,&ret);
          }else if(state->reachable&&checkNonemptyStack(state,"unfinished local operation")){
              handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          state->reachable=true;
          break;
        case BLOCK_CASE:
          if(op.dataAs.block.type!=BLOCK_CASE&&op.dataAs.block.type!=BLOCK_UNKNOWN){
            fprintf(stderr,"unexpected end for %s-block expected end-case\n",blockNames[op.dataAs.block.type]);
            handleError(NULL,ERROR_UNIMPLEMENTED,op.filePos);
          }
          if(state->reachable)
            handleError("missing break statement at end of case",ERROR_SYNTAX,op.filePos);
          switchBlock=&(blockInfoPtr->blockDataAs.switchBlock);
          switchBlock->switchData->caseCount++;//close last case
          if(isEnumLabelType(switchBlock->switchType)){
            if(switchBlock->switchData->labelCount<(size_t)getTypeElementCount(switchBlock->switchType)){
              fputs("switch statement does not cover all labels in enum ",stderr);
              printTypeName(switchBlock->switchType,stderr);
              fputs("\nmissing labels:\n",stderr);
              for(int32_t i=0;i<getTypeElementCount(switchBlock->switchType);i++){
                bool match=false;
                for(size_t l=0;l<switchBlock->switchData->labelCount;l++){
                  if(switchBlock->switchData->labelData[l].value==i){
                    match=true;
                    break;
                  }
                }
                if(!match){
                  mLabel=label(compositeTypeData(switchBlock->switchType)->labelOffset+i,op.filePos);
                  fprintf(stderr," - %"PRI_STR"\n    declared at",PRI_STR_ARGS(mLabel->label));
                  printFilePosition(mLabel->declaredAt,stderr);
                  fputs("\n",stderr);
                }
              }
              handleError(NULL,ERROR_SYNTAX,op.filePos);
            }
            if(switchBlock->endReachable){
              if(predeclareBlockVariables(state,blockInfoPtr->blockStart,&(switchBlock->outStack)))
                 handleError(NULL,ERROR_TYPE,op.filePos);
              if(resetStack(state,&(switchBlock->outStack)))
                handleError(NULL,ERROR_TYPE,op.filePos);
            }
            state->reachable=switchBlock->endReachable;
          }else{
            declareBlockVariables(state,blockInfoPtr->blockStart,&(switchBlock->outStack),&(switchBlock->inStack),"switch",op.filePos);
            if(resetStack(state,&(switchBlock->outStack)))
              handleError(NULL,ERROR_TYPE,op.filePos);
            state->reachable=true;
          }
          free(switchBlock->inStack.types);
          free(switchBlock->inStack.ops);
          free(switchBlock->outStack.types);
          free(switchBlock->outStack.ops);
          break;
        case BLOCK_DEFAULT:
          if(op.dataAs.block.type!=BLOCK_DEFAULT&&op.dataAs.block.type!=BLOCK_UNKNOWN){
            fprintf(stderr,"unexpected end for %s-block expected end-default\n",blockNames[op.dataAs.block.type]);
            handleError(NULL,ERROR_UNIMPLEMENTED,op.filePos);
          }
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
          puts("unexpected block type");
          if(checkNonemptyStack(state,"unfinished local operation")){
            handleError(NULL,ERROR_SYNTAX,op.filePos);
          }
          state->reachable=true;
      }
      while(endCount-->0){
        pushCompiledOperation(state,op);
      }
      if(!state->reachable){
        pushCompiledOperation(state,opUnreachable(op.filePos));
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
    case OP_ADDR_OF_ARRAY:
      fprintf(stderr,"operation %s should not exist at this stage of compilation\n",opName(op.opType));
      handleError(NULL,ERROR_SYNTAX,op.filePos);
    //compile time ops
    case OP_MODIFY_STACK:
      checkReachable(state,op);
      int32_t count=op.dataAs.stackMod.count;
      if(count<0)
        handleError("unexpected value for count, expected non-negative integer",ERROR_MEMORY,op.filePos);
      switch(op.dataAs.stackMod.op){
        case STACK_OP_DUP://duplicate top value on stack
          if(state->typeCount<(uint64_t)count){
            fprintf(stderr,"not enough operands for operation %s: need %"PRIi32" got %zu\n",opName(op.opType),count,state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          extractCompositeOps(state,count,true);
          totalOps=0;
          for(int64_t i=1;i<=count;i++){
            totalOps+=state->typeStack[state->typeCount-i].opCount;
          }
          if(ensureOpStackCap(state,state->opStackCount+totalOps)||ensureTypeStackCap(state,state->typeCount+1))
            handleError(NULL,ERROR_TYPE,op.filePos);
          memmove(state->opStack+state->opStackCount,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
          memmove(state->typeStack+state->typeCount,state->typeStack+state->typeCount-count,count*sizeof(TypeInfo));
          state->opStackCount+=totalOps;
          state->typeCount+=count;
          return;
        case STACK_OP_DROP://remove top value from stack
          if(state->typeCount<(uint64_t)count){
            fprintf(stderr,"not enough operands for operation %s: need %"PRIi32" got %zu\n",opName(op.opType),count,state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          for(int64_t i=0;i<count;i++){
            state->typeCount--;
            state->opStackCount-=state->typeStack[state->typeCount].opCount;
          }
          return;
        case STACK_OP_OVER:
          if(state->typeCount<(uint64_t)count+1){
            fprintf(stderr,"not enough operands for operation %s: need %"PRIi32" got %zu\n",opName(op.opType),count+1,state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          extractCompositeOps(state,count+1,true);
          offset=0;
          for(int64_t i=1;i<=(count+1);i++){
            offset+=state->typeStack[state->typeCount-i].opCount;
          }
          totalOps=state->typeStack[state->typeCount-(count+1)].opCount;
          if(ensureOpStackCap(state,state->opStackCount+totalOps)||ensureTypeStackCap(state,state->typeCount+1))
            handleError(NULL,ERROR_TYPE,op.filePos);
          memmove(state->opStack+state->opStackCount,state->opStack+state->opStackCount-offset,totalOps*sizeof(Operation));
          memmove(state->typeStack+state->typeCount,state->typeStack+state->typeCount-(count+1),sizeof(TypeInfo));
          state->opStackCount+=totalOps;
          state->typeCount++;
          return;
        case STACK_OP_SWAP:
          if(state->typeCount<2){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            handleError(NULL,ERROR_TYPE,op.filePos);
          }
          extractCompositeOps(state,2,true);
          offset=state->typeStack[state->typeCount-1].opCount+state->typeStack[state->typeCount-2].opCount;
          totalOps=state->typeStack[state->typeCount-2].opCount;
          if(ensureOpStackCap(state,state->opStackCount+totalOps)||ensureTypeStackCap(state,state->typeCount+1))
            handleError(NULL,ERROR_TYPE,op.filePos);
          //1. copy lower operations above top operation
          memmove(state->opStack+state->opStackCount,state->opStack+state->opStackCount-offset,totalOps*sizeof(Operation));
          memmove(state->typeStack+state->typeCount,state->typeStack+state->typeCount-2,sizeof(TypeInfo));
          //2. move stack back down
          memmove(state->opStack+state->opStackCount-offset,state->opStack+state->opStackCount-(offset-totalOps),offset*sizeof(Operation));
          memmove(state->typeStack+state->typeCount-2,state->typeStack+state->typeCount-1,2*sizeof(TypeInfo));
          return;
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
  TypeCheckState state=(TypeCheckState){
    .compiledOperations=NULL,.opCap=0,.opCount=0,
    .opStack=malloc(INIT_CAP*sizeof(Operation)),.opStackCap=INIT_CAP,.opStackCount=0,
    .typeStack=malloc(INIT_CAP*sizeof(TypeInfo)),.typeStackCap=INIT_CAP,.typeCount=0,
    .openBlocks=malloc(INIT_CAP*sizeof(BlockInfo)),.blockCap=INIT_CAP,.blockCount=0,
    .autoTypes=malloc(prog->nAutoTypes*sizeof(TypeId)),.nAutoTypes=prog->nAutoTypes,
    .globalScope=NULL,.tmpCount=0,.ifCount=0,.whileCount=0,.index=0,
    .reachable=true,.hasCheckBounds=false,.hasCheckEnum=false,};
  int32_t globalTmpCount=0;
  if(state.opStack==NULL||state.typeStack==NULL||state.openBlocks==NULL||state.autoTypes==NULL){//memory allocation failed
    freeContents(&state);
    handleError("allocation of type-check state failed",ERROR_MEMORY,src->currentPos);
  }
  for(FileId fId=0;fId<prog->fileCount;fId++){
    ProgramFile* f=&prog->files[fId];
    for(size_t incId=0;incId<f->includeCount;incId++){
      IncludedFile inc=f->includes[incId];
      if(includeGlobals(&f->globalScope,&prog->files[inc.id].globalScope,inc.includePos)){
        freeContents(&state);
        handleError(NULL,ERROR_SYNTAX,inc.includePos);
      }
    }
    state.globalScope=&f->globalScope;
    //type check global operations
    state.index=0;
    state.tmpCount=globalTmpCount;
    state.opCap=f->globalOpCount;
    state.opCount=0;
    state.compiledOperations=malloc(state.opCap*sizeof(Operation));
    if(state.compiledOperations==NULL){
      freeContents(&state);
      handleError("allocation of global operations for type-check state failed",ERROR_MEMORY,(FilePosition){.fileName=f->fileName.chars/*null terminated*/,.line=0,.posInLine=0});
    }
    while(state.index<f->globalOpCount){
      typeCheckOperation(f->globalOps[state.index++],&state);
    }
    if(state.blockCount>0){
      freeContents(&state);
      handleError("unfinished code-block",ERROR_SYNTAX,src->currentPos);
    }
    free(f->globalOps);
    f->globalOps=state.compiledOperations;
    f->globalOpCount=state.opCount;
    globalTmpCount=state.tmpCount;
    //type-check local operations
    state.index=0;
    state.tmpCount=0;
    state.opCap=f->localOpCount;
    state.opCount=0;
    state.compiledOperations=malloc(state.opCap*sizeof(Operation));
    if(state.compiledOperations==NULL){
      freeContents(&state);
      handleError("allocation of local operations for type-check state failed",ERROR_MEMORY,(FilePosition){.fileName=f->fileName.chars/*null terminated*/,.line=0,.posInLine=0});
    }
    while(state.index<f->localOpCount){
      typeCheckOperation(f->localOps[state.index++],&state);
    }
    if(state.blockCount>0){
      freeContents(&state);
      handleError("unfinished code-block",ERROR_SYNTAX,src->currentPos);
    }
    free(f->localOps);
    f->localOps=state.compiledOperations;
    f->localOpCount=state.opCount;
    state.compiledOperations=NULL;
    if(!quietMode)
      printf("  typeChecked file %"PRI_STR" : %zu global and %zu local operations\n",PRI_STR_ARGS(f->fileName),f->globalOpCount,f->localOpCount);
  }
  prog->hasCheckBounds=state.hasCheckBounds;
  prog->hasCheckEnum=state.hasCheckEnum;
  prog->autoTypes=state.autoTypes;
  state.autoTypes=NULL;
  freeContents(&state);
}


char const* path;
char const* srcFile=NULL;
char const* outFile=NULL;
char const* parserTokensFile=NULL;
char const* compilerTokensFile=NULL;
//returns true if program should terminate
#define ARGUMENT_NONE 0
#define ARGUMENT_IN 1
#define ARGUMENT_OUT 2
#define ARGUMENT_DUMP_TOKENS_PARSER 3
#define ARGUMENT_DUMP_TOKENS_COMPILER 4
#define ARGUMENT_LIB_PATH 5
bool parseArgs(char** argv){
  path=*(argv++);//set path to first element of argv
  if(*argv==NULL){
    printf("usage: %s \"inputFile\" [-o \"outputFile\"] \n",path);
    return true;
  }
  bool hasLib=false;
  int state=ARGUMENT_IN;
  for(;(*argv!=NULL);argv++){
    if((*argv)[0]=='-'){
      if(state==ARGUMENT_OUT||state==ARGUMENT_DUMP_TOKENS_PARSER||state==ARGUMENT_DUMP_TOKENS_COMPILER){
        printf("expected filename got \"%s\"\n",*argv);
        return true;
      }
      switch((*argv)[1]){
        case '\0':
          puts("invalid argument '-'");
          return true;
        case 'h':
          printf("usage: %s \"inputFile\" [-o \"outputFile\"]\n",path);
          puts("options:");
          puts("  -h: print help text");
          puts("  -o \"fileName\": set output file  (default: \"./out.c\")");
          puts("  -p \"fileName\": dump the parsed tokens to the given file");
          puts("  -t \"fileName\": dump the compiled tokens to the given file");
          puts("  -l \"fileName\": set path to standard library (default: \"./lib/\"");
          puts("  -W: treat warnings as errors");
          puts("  -q: do not print compiler progress");
          return true;
        case 'o':
          state=ARGUMENT_OUT;
          break;
        case 'p':
          state=ARGUMENT_DUMP_TOKENS_PARSER;
          break;
        case 't':
          state=ARGUMENT_DUMP_TOKENS_COMPILER;
          break;
        case 'l':
          state=ARGUMENT_LIB_PATH;
          break;
        case 'W':
          allowWarnings=false;
          break;
        case 'q':
          quietMode=true;
          break;
        default:
          printf("unknown compiler option '-%c'\n",(*argv)[1]);
          return true;
      }
      continue;
    }
    if(state==ARGUMENT_NONE){
      printf("expected compiler option got \"%s\"\n",*argv);
      return true;
    }
    if(state==ARGUMENT_IN){
      srcFile=*argv;
      state=ARGUMENT_NONE;
      continue;
    }
    if(state==ARGUMENT_OUT){
      outFile=*argv;
      state=srcFile==NULL?ARGUMENT_IN:ARGUMENT_NONE;
      continue;
    }
    if(state==ARGUMENT_DUMP_TOKENS_PARSER){
      parserTokensFile=*argv;
      state=srcFile==NULL?ARGUMENT_IN:ARGUMENT_NONE;
      continue;
    }
    if(state==ARGUMENT_DUMP_TOKENS_COMPILER){
      compilerTokensFile=*argv;
      state=srcFile==NULL?ARGUMENT_IN:ARGUMENT_NONE;
      continue;
    }
    if(state==ARGUMENT_LIB_PATH){
      libPath=cstrToStr(*argv);
      state=srcFile==NULL?ARGUMENT_IN:ARGUMENT_NONE;
      hasLib=true;
      continue;
    }
  }
  if(srcFile==NULL){
    puts("missing input file");
    return true;
  }
  String srcStr=cstrToStr(srcFile);
  int64_t iDot=lastIndexOfChar(srcStr,'.');
  int64_t iSlash=lastIndexOfChar(srcStr,'/');//XXX Windows compatibility
  if(iDot<iSlash)
    iDot=-1;
  if(outFile==NULL){
    int64_t common=(iDot>=0?(size_t)iDot:srcStr.length);
    char* outPath=malloc((common+3)*sizeof(char));
    memcpy(outPath,srcFile,common*sizeof(char));
    outPath[common++]='.';
    outPath[common++]='c';
    outPath[common++]='\0';
    outFile=outPath;
  }
  if(iSlash<0){//XXX? explicitly prefix srcPath with ./
    basePath=EMPTY_STRING;
    return false;
  }
  basePath=newString(srcFile,iSlash+1);
  if(hasLib&&charAt(libPath,libPath.length-1)!='/'){
    puts("library path has to end with /");
    return true;
  }else{
    libPath=cstrToStr("./lib/");
  }//XXX check if libPath exists
  return false;
}
int main(int argc,char** argv){
  (void)argc;
  if(parseArgs(argv))
    return EXIT_FAILURE;

  //initialization of uninitialized global variables
  if(namespaceTrieInit()){
    fputs("failed to initialize namespace storage",stderr);
    return EXIT_FAILURE;
  }
  //read main source file
	CodeFile codeFile;
	if(readCodeFile(srcFile,&codeFile))
	  return EXIT_FAILURE;
	//1. compile file to operations
	Program p=compileToOps(&codeFile);
	if(!quietMode)
    printf("parsed %"PRIi32" files\n",p.fileCount);
  //2. save intermediate representation
  FILE* intermediate;
  if(parserTokensFile!=NULL){
    intermediate=fopen(parserTokensFile,"w");
	  if(intermediate==NULL){
	    fprintf(stderr,"IO Error while opening File: %s\n",parserTokensFile);
		  return EXIT_FAILURE;
	  }
	  for(FileId fId=0;fId<p.fileCount;fId++){
	    ProgramFile* f=&p.files[fId];
	    fprintf(intermediate," ## %"PRI_STR"\n",PRI_STR_ARGS(f->fileName));
	    fputs("## global\n",intermediate);
      for(size_t i=0;i<f->globalOpCount;i++){
        printOperation(f->globalOps[i],intermediate);
      }
	    fputs("## local\n",intermediate);
      for(size_t i=0;i<f->localOpCount;i++){
        printOperation(f->localOps[i],intermediate);
      }
    }
    fclose(intermediate);
  }
	//3. type-check operations
  typeCheckProgram(&p,&codeFile);
	if(!quietMode)
    printf("typeChecked %"PRIi32" files\n",p.fileCount);
  //4. save intermediate representation
  if(compilerTokensFile!=NULL){
    intermediate=fopen(compilerTokensFile,"w");
	  if(intermediate==NULL){
	    fprintf(stderr,"IO Error while opening File: %s\n",compilerTokensFile);
		  return EXIT_FAILURE;
	  }
	  for(FileId fId=0;fId<p.fileCount;fId++){
	    ProgramFile* f=&p.files[fId];
	    fprintf(intermediate," ## %"PRI_STR"\n",PRI_STR_ARGS(f->fileName));
	    fputs("## global\n",intermediate);
      for(size_t i=0;i<f->globalOpCount;i++){
        printOperation(f->globalOps[i],intermediate);
      }
	    fputs("## local\n",intermediate);
      for(size_t i=0;i<f->localOpCount;i++){
        printOperation(f->localOps[i],intermediate);
      }
    }
    fclose(intermediate);
  }
	//5. compile operations to C
  FILE* out=fopen(outFile,"w");
	if(out==NULL){
	  fprintf(stderr,"IO Error while opening File: %s\n",outFile);
		return EXIT_FAILURE;
	}
  compileToC(out,&p);
	if(!quietMode)
    puts("compiled program");
  fclose(out);
  return EXIT_SUCCESS;
}
