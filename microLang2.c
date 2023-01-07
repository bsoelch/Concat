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
//TODO split into less general errors
#define ERROR_SYNTAX 2 
#define ERROR_PARSE_INT 3
#define ERROR_REDECLARATION 4
#define ERROR_UNSUPPORTED_ESCAPE_SEQUENCE 5
#define WARNING_CODEPOINT_OUT_OF_RANGE 6
#define ERROR_EOF 7 //end of file
//maximum integer value of any error constant
#define MAX_ERROR 7

//negate indices (internal errors have negative error codes)
const char* const internalErrors [] = {[-ERROR_MEMORY]="ERROR_MEMORY",[-ERROR_IO]="ERROR_IO",[-ERROR_UNIMPLEMENTED]="ERROR_UNIMPLEMENTED",};
const char* const compilerErrors [] = {
[ERROR_TYPE]="type error",[ERROR_SYNTAX]="syntax error",[ERROR_PARSE_INT]="invalid character while parsing integer",
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
    return compilerErrors[errorCode>MAX_ERROR?ERROR_SYNTAX:errorCode];
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
  
  OP_DECLARE,
  OP_GET,
  OP_SET,
  //TODO tuple operations
  //ADDR_OF (pointer to given value)
  //OP_BUILD (tuple,union)
  
  OP_BINARY_OPERATOR, 
  OP_UNARY_OPERATOR,  
  
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
    case OP_GET:return "OP_GET";
    case OP_SET:return "OP_SET";
    case OP_BINARY_OPERATOR:return "OP_BINARY_OPERATOR";
    case OP_UNARY_OPERATOR:return "OP_UNARY_OPERATOR";  
    case OP_CODE_BLOCK:return "OP_CODE_BLOCK";
    case OP_DECLARE_PROCEDURE:return "OP_DECLARE_PROCEDURE"; 
    case OP_RETURN:return "OP_RETURN";      
    case OP_CALL:return "OP_CALL";
    case ENTRY_POINT:return "ENTRY_POINT";
  }
  return "UNDEFINED";
}
//types
typedef enum{
  TYPECLASS_UNDEFINED,
  TYPECLASS_PRIMITIVE,
  TYPECLASS_POINTER,
  TYPECLASS_CONST_POINTER,
  TYPECLASS_TUPLE,
  TYPECLASS_FLAT_TUPLE,//behaves like tuple but will not be directly used
  TYPECLASS_UNION,
  TYPECLASS_PROCEDURE,
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
typedef CompositeType UnionType;
typedef struct ProcedureType ProcedureType;
typedef struct DataType{
  TypeClass typeClass;
  union{
    PrimitiveType primitive;
    struct DataType* type;
    CompositeType* composite;
    TupleType* tuple;//name alias for composite
    UnionType* unionType;//name alias for composite
    ProcedureType* procedure;
  }typeDataAs;
}DataType;
#define FLAG_IS_TUPLE 1
#define FLAG_IS_UNION 2
#define FLAG_IS_FLAT_TUPLE 4
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

const DataType TYPE_UNDEFINED=(DataType){.typeClass=TYPECLASS_UNDEFINED,.typeDataAs={0}};

#define MAX_TYPES  4096
#define MAX_COMPOSITE 1024
#define TYPE_BUFFER_CAP 1024
#define MAX_PROC_TYPES 1024

size_t typeCount=0;
DataType typeData[MAX_TYPES];
int32_t compositeCount=0;
CompositeType compositeTypes[MAX_COMPOSITE];
size_t procTypeCount=0;
ProcedureType procTypes[MAX_PROC_TYPES];
//temporary buffer for construction of composite elements
int64_t bufferedTypes=0;
DataType typeBuffer[TYPE_BUFFER_CAP];

bool typeEquals(DataType a,DataType b){
  if(a.typeClass!=b.typeClass)
    return false;
  if(a.typeClass==TYPECLASS_UNDEFINED)
    return true;//all undefined types are equal
  if(a.typeClass==TYPECLASS_PRIMITIVE)
    return a.typeDataAs.primitive==b.typeDataAs.primitive;
  if(a.typeClass==TYPECLASS_POINTER||a.typeClass==TYPECLASS_CONST_POINTER)
    return typeEquals(*a.typeDataAs.type,*b.typeDataAs.type);
  if(a.typeClass==TYPECLASS_TUPLE||a.typeClass==TYPECLASS_UNION)
    return a.typeDataAs.composite->id==b.typeDataAs.composite->id;
  if(a.typeClass==TYPECLASS_PROCEDURE)
    return typeEquals(*a.typeDataAs.procedure->inType,*b.typeDataAs.procedure->inType)&&
            typeEquals(*a.typeDataAs.procedure->outType,*b.typeDataAs.procedure->outType);
  return false;
}
DataType primitiveType(PrimitiveType id){
  return (DataType){.typeClass=TYPECLASS_PRIMITIVE,.typeDataAs={.primitive=id}};
}
DataType pointerType(DataType target){
  for(size_t i=0;i<typeCount;i++){
    if(typeEquals(target,typeData[i]))
      return (DataType){.typeClass=TYPECLASS_POINTER,.typeDataAs={.type=typeData+i}};
  }
  if(typeCount+1>=MAX_TYPES){
    return TYPE_UNDEFINED;
  }
  typeData[typeCount]=target;
  return (DataType){.typeClass=TYPECLASS_POINTER,.typeDataAs={.type=typeData+typeCount++}};
}
DataType constPointerType(DataType target){
  DataType t=pointerType(target);
  if(t.typeClass==TYPECLASS_UNDEFINED)
    return t;
  t.typeClass=TYPECLASS_CONST_POINTER;//otherwise identical to pointer
  return t;
}
DataType compositeType(TypeClass typeClass,DataType* elements,int32_t eltCount){
  if(eltCount==0)
    return primitiveType(PRIMITIVE_VOID);//empty tuple/union -> void
  if(eltCount==1)
    return elements[0];//auto unwrap 1-element tuple/union
  int16_t classFlag=typeClass==TYPECLASS_UNION?FLAG_IS_UNION:typeClass==TYPECLASS_FLAT_TUPLE?FLAG_IS_FLAT_TUPLE:FLAG_IS_TUPLE;
  int64_t match=-1;
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].typeCount==eltCount||(match==-1&&compositeTypes[i].typeCount>eltCount)){
      bool isMatch=true;
      for(int32_t a=0;a<eltCount;a++){//TODO? allow matches of sub-lists
        if(!typeEquals(compositeTypes[i].types[a],elements[a])){
          isMatch=false;
          break;
        }
      }
      if(!isMatch)
        continue;
      if(compositeTypes[i].typeCount==eltCount){
        compositeTypes[i].flags|=classFlag;
        return (DataType){.typeClass=typeClass,.typeDataAs.composite=compositeTypes+i};
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
  return (DataType){.typeClass=typeClass,.typeDataAs={.composite=compositeTypes+(compositeCount++)}};
}
DataType procedureType(DataType inType,DataType outType){
  for(size_t i=0;i<procTypeCount;i++){
    if(typeEquals(*procTypes[i].inType,inType)&&typeEquals(*procTypes[i].outType,outType))
      return (DataType){.typeClass=TYPECLASS_PROCEDURE,.typeDataAs={.procedure=procTypes+i}};
  }
  int32_t inId=-1,outId=-1;
  for(size_t i=0;i<typeCount&&(inId==-1||outId==-1);i++){
    if(inId==-1&&typeEquals(inType,typeData[i]))
      inId=i;
    if(outId==-1&&typeEquals(outType,typeData[i]))
      outId=i;
  }
  if(inId==-1){
    if(typeCount+1>=MAX_TYPES){
      return TYPE_UNDEFINED;
    }
    inId=typeCount;
    typeData[typeCount++]=inType;
  }
  if(outId==-1){
    if(typeCount+1>=MAX_TYPES){
      return TYPE_UNDEFINED;
    }
    outId=typeCount;
    typeData[typeCount++]=outType;
  }
  procTypes[procTypeCount]=(ProcedureType){.id=procTypeCount,.inType=typeData+inId,.outType=typeData+outId};
  return (DataType){.typeClass=TYPECLASS_PROCEDURE,.typeDataAs={.procedure=procTypes+procTypeCount++}};
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
    case TYPECLASS_UNION:
      return "UNION";
    case TYPECLASS_PROCEDURE:
      return "PRODECURE";
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
int printTypeNameIntenal(DataType type,FILE* file,bool noRecurse){
  int i,j;
  switch(type.typeClass){
    case TYPECLASS_UNDEFINED:
      return fputs("UNDEFINED",file);
    case TYPECLASS_PRIMITIVE:
      return fprintf(file,"%s",primitiveName(type.typeDataAs.primitive));
    case TYPECLASS_CONST_POINTER:
    case TYPECLASS_POINTER:
      i=fprintf(file,"%s ",typeClassName(type.typeClass));
      if(i<0)
        return i;
      j=printTypeNameIntenal(*type.typeDataAs.type,file,noRecurse);
      return j<0?j:(i+j);
    case TYPECLASS_FLAT_TUPLE:
    case TYPECLASS_TUPLE:
    case TYPECLASS_UNION:
      i=fprintf(file,"%s (%"PRIi32")",typeClassName(type.typeClass),type.typeDataAs.composite->id);
      if(noRecurse||i<0)
        return i;
      for(int32_t e=0;e<type.typeDataAs.composite->typeCount;e++){
        if(e>0){
          j=fputs(" ",file);
          if(j<0)
            return j;
          i+=j;
        }
        j=printTypeNameIntenal(type.typeDataAs.composite->types[e],file,true);//only one recursion level
        if(j<0)
          return j;
        i+=j;
      } 
      return i;
    case TYPECLASS_PROCEDURE:
      i=fprintf(file,"%s (%"PRIi32")",typeClassName(type.typeClass),type.typeDataAs.procedure->id);
      if(noRecurse||i<0)
        return i;
      j=printTypeNameIntenal(*type.typeDataAs.procedure->inType,file,true);
      if(j<0)
        return j;
      i+=j;
      j=fputs(" ",file);
      if(j<0)
        return j;
      i+=j;
      j=printTypeNameIntenal(*type.typeDataAs.procedure->outType,file,true);
      return j<0?j:(i+j);
  }
  return fprintf(file,"unknown type-class %i\n",type.typeClass);
}
int printTypeName(DataType type,FILE* file){
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
int printTypeNameC(DataType type,FILE* file){
  int i,j;
  switch(type.typeClass){
    case TYPECLASS_UNDEFINED:
      return fputs("void",file);
    case TYPECLASS_PRIMITIVE:
      return fprintf(file,"%s",primitiveNameC(type.typeDataAs.primitive));
    case TYPECLASS_CONST_POINTER:
      i=fputs("const ",file); //only difference to TYPECLASS_POINTER
      if(i<0)
        return i;
      // fall through
    case TYPECLASS_POINTER:
      j=printTypeNameC(*type.typeDataAs.type,file);
      if(j<0)
        return j;
      i+=j;
      j=fputs("*",file);
      return j<0?j:(i+j);
    case TYPECLASS_FLAT_TUPLE:
    case TYPECLASS_TUPLE:
      return fprintf(file,"tuple%"PRIi32,type.typeDataAs.composite->id);
    case TYPECLASS_UNION:
      return fprintf(file,"union%"PRIi32,type.typeDataAs.composite->id);
    case TYPECLASS_PROCEDURE:
      return fprintf(file,"procPtr%"PRIi32,type.typeDataAs.procedure->id);
  }
  return fprintf(file,"unknown type-class %i\n",type.typeClass);
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
  FAST_AND,
  FAST_OR,
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
    case FAST_AND:return "FAST_AND";
    case FAST_OR:return "FAST_OR";
    case GT:return "GT";
    case GE:return "GE";
    case EQ:return "EQ";
    case NE:return "NE";
    case LE:return "LE";
    case LT:return "LT";
  }
  return "UNDEFINED";
}
typedef enum{//TODO? postfix unary operators (POST_INCREMENT)
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
  ID_TUPLE_ELEMENT,
  //TODO GET/SET for union values
  ID_POINTER
}IdentiferType;
typedef struct{
  int32_t id;
  IdentiferType type;
}IdentiferInfo;

typedef enum{
  BLOCK_PROCEDURE, 
  BLOCK_START,     // {
  BLOCK_IF,        // if( EXPR ){
  BLOCK_ELIF,      // } else if(EXPR){
  BLOCK_ELSE,      // }else{
  BLOCK_WHILE,     // while( EXPR ){
  BLOCK_DO,        // do{
  BLOCK_WHILE_END, // }while( EXPR );
  BLOCK_END,       // }
}BlockType;

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
    DataType stringElts[2]={constPointerType(primitiveType(PRIMITIVE_I8)),primitiveType(PRIMITIVE_I64)};
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

#define COMPILE_OP_RETURN_ERROR(target, op)\
                r=compileOp(target,op+size);\
                if(r.isError)\
                  return r;\
                size+=r.as.size;\

SizeOrError compileOp(FILE* target,const Operation* op){
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
          printTypeName(op->dataType,stderr);
          fputs(" is (currently) not supported\n",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
      }
      fputs("\\n\",",target);
      COMPILE_OP_RETURN_ERROR(target,op);
      if(boolMode){
        fputs("?\"true\":\"false\"",target);
      }
      fputs(");\n",target);
      break;
    case OP_CONSTANT:
      if(op->dataType.typeClass!=TYPECLASS_PRIMITIVE){
          fputs("constants of non-primitive type ",stderr);
          printTypeName(op->dataType,stderr);
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
    case OP_GET:
      switch(op->dataAs.idInfo.type){
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
        case ID_TUPLE_ELEMENT:
          //1. get tuple
          fputs("(",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          //2. get element
          fprintf(target,").e%" PRIi32,op->dataAs.idInfo.id);
          break;
        case ID_POINTER:
          fputs("*((",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          fprintf(target,")+%"PRIi32")",op->dataAs.idInfo.id);
          break;
      }
      break;
    case OP_SET:
      switch(op->dataAs.idInfo.type){
        case ID_LOCAL_VAR:
          fprintf(target,"local%" PRIi32" = ",op->dataAs.idInfo.id);
          break;
        case ID_ARGUMENT:
          fprintf(target,"arg%" PRIi32" = ",op->dataAs.idInfo.id);
          break;
        case ID_GLOBAL_VAR:
          fprintf(target,"global%" PRIi32" = ",op->dataAs.idInfo.id);
          break;
        case ID_PROCEDURE:
          fputs("procedures are immutable",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
        case ID_TUPLE_ELEMENT:
          //1. get tuple
          fputs("(",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          //2. set element
          fprintf(target,").e%" PRIi32" = ",op->dataAs.idInfo.id);
          break;
        case ID_POINTER:
          fputs("*((",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          fprintf(target,")+%"PRIi32") = ",op->dataAs.idInfo.id);
          break;
      }
      COMPILE_OP_RETURN_ERROR(target,op);
      fputs(";\n",target);
      break;
    case OP_DECLARE:
      printTypeNameC(op->dataType,target);
      switch(op->dataAs.idInfo.type){
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
        case ID_TUPLE_ELEMENT:
          fputs("cannot declare tuple elements",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
        case ID_POINTER:
          fputs("cannot declare pointers",stderr);
          return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=op->filePos}}};
      }
      COMPILE_OP_RETURN_ERROR(target,op);
      fputs(";\n",target);
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
      COMPILE_OP_RETURN_ERROR(target,op);
      fputs(")",target);
      break;
    case OP_BINARY_OPERATOR:
      fputs("(",target);
      COMPILE_OP_RETURN_ERROR(target,op);
      switch(op->dataAs.binOp){//TODO? use array/map instead
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
        case FAST_AND:
          fputs("&&",target);
          break;
        case FAST_OR:
          fputs("||",target);
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
      COMPILE_OP_RETURN_ERROR(target,op);
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
          fputs("if(",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          fputs("){\n",target);
          break;
        case BLOCK_ELIF:
          fputs("}else if(",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          fputs("){\n",target);
          break;
        case BLOCK_WHILE:
          fputs("while(",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          fputs("){\n",target);
          break;
        case BLOCK_DO:
          fputs("do{\n",target);
          break;
        case BLOCK_ELSE:
          fputs("}else{\n",target);
          break;
        case BLOCK_WHILE_END:
          fputs("}while(",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          fputs(");\n",target);
          break;
        case BLOCK_END:
          fputs("}\n",target);
          break;
      }
      break;
    case OP_DECLARE_PROCEDURE:{
      if(op->dataType.typeClass!=TYPECLASS_PROCEDURE)
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=op->filePos}}};
      printTypeNameC(*op->dataType.typeDataAs.procedure->outType,target);
      fprintf(target," procedure%" PRIi32" (",op->dataAs.idInfo.id);
      DataType* inType=op->dataType.typeDataAs.procedure->inType;
      if(inType->typeClass==TYPECLASS_FLAT_TUPLE){
        CompositeType* inTypes=inType->typeDataAs.composite;
        for(int32_t e=0;e<inTypes->typeCount;e++){
          if(e>0)
            fputs(", ",target);
          printTypeNameC(inTypes->types[e],target);
          fprintf(target," arg%"PRIi32,e);
        } 
      }else if(inType->typeClass==TYPECLASS_PRIMITIVE&&inType->typeDataAs.primitive==PRIMITIVE_VOID){
        fputs("void",target);
      }else{
        printTypeNameC(*inType,target);
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
        COMPILE_OP_RETURN_ERROR(target,op);
        fputs(";\n",target);
        break;
      }
      fprintf(target,"(tuple%"PRIi32"){",op->dataType.typeDataAs.composite->id);
      for(int32_t e=0;e<op->dataType.typeDataAs.composite->typeCount;e++){
        if(e>0)
          fputs(",",target);
        fprintf(target,".e%"PRIi32"=",e);
        COMPILE_OP_RETURN_ERROR(target,op);
      }
      fputs("};\n",target);
      break;
    case ENTRY_POINT:
      fputs("int main(void){\n",target);
      break;
    case OP_CALL:
      switch(op->dataAs.idInfo.type){
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
        case ID_TUPLE_ELEMENT:
          //1. get tuple
          fputs("(",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          //2. call element
          fprintf(target,".e%"PRIi32"(",op->dataAs.idInfo.id);
          break;
        case ID_POINTER:
          fputs("*((",target);
          COMPILE_OP_RETURN_ERROR(target,op);
          fprintf(target,")+%"PRIi32")(",op->dataAs.idInfo.id);
          break;
      }
      DataType* in=op->dataType.typeDataAs.procedure->inType;
      DataType* out=op->dataType.typeDataAs.procedure->outType;
      if(in->typeClass==TYPECLASS_FLAT_TUPLE){
        for(int32_t e=0;e<in->typeDataAs.composite->typeCount;e++){
          if(e>0)
            fputs(",",target);
          COMPILE_OP_RETURN_ERROR(target,op);
        }
      }else if(in->typeClass!=TYPECLASS_PRIMITIVE||in->typeDataAs.primitive!=PRIMITIVE_VOID){
        COMPILE_OP_RETURN_ERROR(target,op);
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

Error compileToC(FILE* target,const Operation* ops,size_t opCount,bool hasEntryPoint){
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
    }//no else
    if(compositeTypes[i].flags&FLAG_IS_UNION){
      fprintf(target,"typedef struct union%"PRIi32"Impl union%"PRIi32";\n",i,i);
    }
  }
  //declare procedure pointers
  for(size_t i=0;i<procTypeCount;i++){
    fputs("typedef ",target);
    printTypeNameC(*procTypes[i].outType,target);
    fprintf(target," (*procPtr%zu) (",i);
    if(procTypes[i].inType->typeClass==TYPECLASS_FLAT_TUPLE){//auto-unwrap procedure arguments
      CompositeType* inTypes=procTypes[i].inType->typeDataAs.composite;
      for(int32_t j=0;j<inTypes->typeCount;j++){
        if(j>0)
          fputs(",",target);
        printTypeNameC(inTypes->types[j],target);
      }
    }else{
      printTypeNameC(*procTypes[i].inType,target);
    }
    fputs(");\n",target);
  }
  //initialize composite types
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].flags&FLAG_IS_TUPLE){
      fprintf(target,"struct tuple%"PRIi32"Impl{\n",i);
      for(int16_t e=0;e<compositeTypes[i].typeCount;e++){
        printTypeNameC(compositeTypes[i].types[e],target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("};\n",target);
    }//no else
    if(compositeTypes[i].flags&FLAG_IS_UNION){
      fprintf(target,"struct union%"PRIi32"Impl{\n"
                     "%s state;\n"
                     "union{\n",i,primitiveNameC(PRIMITIVE_I32));
      for(int16_t e=0;e<compositeTypes[i].typeCount;e++){
        printTypeNameC(compositeTypes[i].types[e],target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("}value;\n};\n",target);
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
    fprintf(target,"const tuple%"PRIi32" string%"PRIi32" = (tuple%"PRIi32"){.e0=stringChars%"PRIi32"+%"PRIi32",.e1=%zu};\n",
      stringType.typeDataAs.tuple->id,programStrings[i].stringId,stringType.typeDataAs.tuple->id,
      programStrings[i].charsId,programStrings[i].charsOffset,programStrings[i].value.length);
  }
  if(!hasEntryPoint)//auto-wrap programs without entry point into a main function
    fputs("int main(void){\n",target);
  SizeOrError r;
  for(size_t p=0;p<opCount;){
    r=compileOp(target,ops+p);
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
  scopeBuffer[scopeCount].nodes=malloc(SCOPE_MAP_CAP*sizeof(ScopeNode*));
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
  bool hasEntryPoint;
}Program;

typedef struct{
  int32_t currentProcId;
  int32_t procScope;
  Scope* currentScope;
  int32_t scopeLevel;
  
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
//!!! This method may segfault if word contains \0 characters !!! 
bool wordEquals(const String* word,const char* string){
  int c=strncasecmp(word->chars,string,word->length);
  if(c!=0)
    return false;
  return string[word->length]=='\0';//check if string has same length as word (length is >= because c was 0)
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
  if(number.chars[0]=='-'){
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
  for(;i<number.length;i++){
    //TODO warning if value overflows
    value*=base;
    digit=toDigit(number.chars[i]);
    if(digit<0||digit>=base)
      return (IntOrErrorCode){.isError=true,.as={.error=ERROR_PARSE_INT}};
    value+=digit;
  }
  return (IntOrErrorCode){.isError=false,.as={.i64=negate?-value:value}};
}
String readStringLiteral(CodeFile* codeFile,char end,bool doEspaceSeqs,int* errorFlag){
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
//positive values are used to indicate syntax errors
#define WORD_TYPE_STRING (MAX_ERROR+1)
#define WORD_TYPE_CHAR   (MAX_ERROR+2)
String nextWord(CodeFile* codeFile,int* wordType){
  skipWhitespaces(codeFile);
  if(codeFile->codeSize<=0){//end of file
    //don't set wordType to ERROR_EOF, file is allowed to end at this point
    return (String){.chars=codeFile->code,.length=0};
  }
  codeFile->wordStart=codeFile->currentPos;
  if(wordType)
    *wordType=0;
  if(*(codeFile->code)=='"'){
    if(wordType)
      *wordType=WORD_TYPE_STRING;
    return readStringLiteral(codeFile,'"',true,wordType);
  }else if(*(codeFile->code)=='\''){
    if(wordType)
      *wordType=WORD_TYPE_CHAR;
    return readStringLiteral(codeFile,'\'',true,wordType);
  }else if(*(codeFile->code)=='#'){//TODO? inline comments, start comment with double hash
    readStringLiteral(codeFile,'\n',false,wordType);//ignore everything up to next new-line
    return (String){.chars=codeFile->code,.length=0};
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

int readType(String name,CodeFile* codeFile);
//reads a composite type of the given type-class, the result is stored in the type buffer
//return 0 if a type was read, otherwise a nonzero error-code 
int readCompositeType(TypeClass typeClass,CodeFile* codeFile,const char* endString){
  String word;
  int err;
  size_t initOffset=bufferedTypes,count=0;
  word=nextWord(codeFile,&err);
  while(!wordEquals(&word,endString)){
    if(err!=0){
      bufferedTypes=initOffset;
      return err>MAX_ERROR?ERROR_SYNTAX:err;//err >MAX_ERROR means word is a string or character
    }
    err=readType(word,codeFile);
    if(err!=0){
      bufferedTypes=initOffset;
      return err;
    }
    count++;
    word=nextWord(codeFile,&err);
  }
  typeBuffer[initOffset]=compositeType(typeClass,typeBuffer+initOffset,count);
  bufferedTypes=initOffset+1;
  return 0;
}
//reads a type starting with the identifier name, the result is stored in the type buffer
//return 0 if a type was read, ERROR_TYPE if name was not a type and the corresponding error code if another error occurs
int readType(String name,CodeFile* codeFile){
  if(name.length==0)
    return ERROR_TYPE;
  if(bufferedTypes>=TYPE_BUFFER_CAP){//buffer overflow
    return ERROR_MEMORY;
  }
  //primitive types
  if(wordEquals(&name,"VOID")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_VOID);
    return 0;
  }
  if(wordEquals(&name,"BOOL")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_BOOL);
    return 0;
  }
  if(wordEquals(&name,"I8")||wordEquals(&name,"CHAR")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_I8);
    return 0;
  }
  if(wordEquals(&name,"I32")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_I32);
    return 0;
  }
  if(wordEquals(&name,"I64")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_I64);
    return 0;
  }
  if(wordEquals(&name,"FLOAT")){
    typeBuffer[bufferedTypes++]=primitiveType(PRIMITIVE_FLOAT);
    return 0;
  }
  if(wordEquals(&name,"STRING")){
    typeBuffer[bufferedTypes++]=progStringType();
    return 0;
  }
  //composite types
  size_t initOffset=bufferedTypes;
  int r;
  if(wordEquals(&name,"PTR")){
    if(bufferedTypes==0)
      return ERROR_SYNTAX;
    typeBuffer[bufferedTypes-1]=pointerType(typeBuffer[bufferedTypes-1]);
    return 0;
  }
  if(wordEquals(&name,"PROC")){
    r=readCompositeType(TYPECLASS_FLAT_TUPLE,codeFile,"=>");
    if(r!=0)
      return r==ERROR_TYPE?ERROR_SYNTAX:r;
    r=readCompositeType(TYPECLASS_FLAT_TUPLE,codeFile,"END");
    if(r!=0)
      return r==ERROR_TYPE?ERROR_SYNTAX:r;
    typeBuffer[initOffset]=procedureType(typeBuffer[initOffset],typeBuffer[initOffset+1]);
    bufferedTypes--;
    return 0;
  }
  if(wordEquals(&name,"TUPLE")){
    r=readCompositeType(TYPECLASS_TUPLE,codeFile,"END");
    return r==ERROR_TYPE?ERROR_SYNTAX:r;
  }
  if(wordEquals(&name,"UNION")){
    r=readCompositeType(TYPECLASS_UNION,codeFile,"END");
    return r==ERROR_TYPE?ERROR_SYNTAX:r;
  }
  return ERROR_TYPE;
}

SizeOrError readOperation(Operation* op,CodeFile* codeFile,CompilerState* state){
  int err=0;
  String word=nextWord(codeFile,&err);
  FilePosition wordPos=codeFile->wordStart;
  if(err==WORD_TYPE_STRING){
    IntOrErrorCode strId=addProgString(word);
    if(strId.isError)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=strId.as.error,.pos=wordPos}}};
    (*op)=(Operation){.opType=OP_STRING_CONST,.dataType=progStringType(),.filePos=wordPos,.dataAs={.i64=strId.as.i64}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(err==WORD_TYPE_CHAR){
    if(word.length!=1){//TODO? handle Unicode characters
      fprintf(stderr,"character literal '%.*s' contains more that one character\n",(int)word.length,word.chars);
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    }
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(PRIMITIVE_I8),.filePos=wordPos,.dataAs={.i64=word.chars[0]}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(err!=0)
    return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
  IntOrErrorCode asInt=parseInt(word,0);//try to parse word as int
  if(!asInt.isError){//TODO catch overflows
    bool isI32=asInt.as.i64<=INT32_MAX&&asInt.as.i64>=INT32_MIN;
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(isI32?PRIMITIVE_I32:PRIMITIVE_I64),.filePos=wordPos,.dataAs={.i64=asInt.as.i64}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(word.length==0)
    return (SizeOrError){.isError=false,.as={.size=0}};
  err=readType(word,codeFile);//try to parse word as type
  if(err==0){//is type
    DataType type=typeBuffer[--bufferedTypes];
    //read operation that takes type as argument
    word=nextWord(codeFile,&err);
    if(err!=0){
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err>MAX_ERROR?ERROR_SYNTAX:err,.pos=wordPos}}};//err >MAX_ERROR means word is a string or character
    }
    if(wordEquals(&word,"DECLARE")){
      if(typeEquals(type,TYPE_UNDEFINED))
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=wordPos}}};
      String varName=nextWord(codeFile,&err);
      if(err!=0)
        return (SizeOrError){.isError=true,.as={.error={.errorCode=err>MAX_ERROR?ERROR_SYNTAX:err,.pos=wordPos}}};
      if(varName.length==0)
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
      IdentiferType idType=type.typeClass==TYPECLASS_PROCEDURE?ID_PROCEDURE:state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
      ScopeNode* id;
      int r=declareIdentifier(varName,type,idType,&id);
      if(r!=0)
        return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
      if(idType==ID_PROCEDURE){
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
    }else if(wordEquals(&word,"CAST")){
      //TODO op cast
    }
    fprintf(stderr,"invalid operand of argument of type TYPE: %.*s\n",(int)word.length,word.chars);
    return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_TYPE,.pos=wordPos}}};
  }
  if(err!=ERROR_TYPE)//unexpected error while reading type
    return (SizeOrError){.isError=true,.as={.error={.errorCode=err,.pos=wordPos}}};
  
  if(wordEquals(&word,"true")){
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(PRIMITIVE_BOOL),.filePos=wordPos,.dataAs={.i64=true}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"false")){
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(PRIMITIVE_BOOL),.filePos=wordPos,.dataAs={.i64=false}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ADD")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=ADD}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SUBTRACT")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=SUBTRACT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"MULTIPLY")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=MULTIPLY}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"DIVIDE")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=DIVIDE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"MOD")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=MOD}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"AND")||wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=AND}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"OR")||wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=OR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"XOR")||wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=XOR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"AND2")||wordEquals(&word,"&&")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=FAST_AND}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"OR2")||wordEquals(&word,"||")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=FAST_OR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"EQ")||wordEquals(&word,"==")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=EQ}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"NE")||wordEquals(&word,"NEQ")||wordEquals(&word,"!=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=NE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"GT")||wordEquals(&word,">")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=GT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"GE")||wordEquals(&word,">=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=GE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"LE")||wordEquals(&word,"<=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=LE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"LT")||wordEquals(&word,"<")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.binOp=LT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"NEG")||wordEquals(&word,"NEGATE")){
    (*op)=(Operation){.opType=OP_UNARY_OPERATOR,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.unOp=NEGATE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET")){
    String varName=nextWord(codeFile,NULL);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_SYNTAX,.pos=wordPos}}};
    ScopeNode* id;
    int r=getIdentifier(varName,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
    (*op)=(Operation){.opType=OP_SET,.dataType=id->type,.filePos=wordPos,.dataAs={.idInfo={.type=id->idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"GET_ELEMENT")){
    IntOrErrorCode index=parseInt(nextWord(codeFile,&err),0);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err>MAX_ERROR?ERROR_SYNTAX:err,.pos=wordPos}}};
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=index.as.error,.pos=wordPos}}};
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET_ELEMENT")){
    IntOrErrorCode index=parseInt(nextWord(codeFile,&err),0);
    if(err!=0)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=err>MAX_ERROR?ERROR_SYNTAX:err,.pos=wordPos}}};
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=index.as.error,.pos=wordPos}}};
    (*op)=(Operation){.opType=OP_SET,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"IF")){
    if(currentScopeType()!=BLOCK_ELIF){//don't open scope for IF after EL
      Scope* newScope=openScope(BLOCK_IF);
      if(newScope==NULL)
        return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
      state->currentScope=newScope;
      state->scopeLevel++;
    }
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_IF}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"EL")){
    closeScope();
    Scope* newScope=openScope(BLOCK_ELIF);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    //scope count does not change
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_ELIF}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"WHILE")){
    Scope* newScope=openScope(BLOCK_WHILE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_WHILE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"DO")){//!!while syntax is different fro C:  WHILE cond DO exrp END   do-While: WHILE exrp cond DO END
    closeScope();
    Scope* newScope=openScope(BLOCK_DO);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    //scope count does not change
        
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_DO}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ELSE")){
    closeScope();
    Scope* newScope=openScope(BLOCK_ELSE);
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error={.errorCode=ERROR_MEMORY,.pos=wordPos}}};
    state->currentScope=newScope;
    //scope count does not change
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_ELSE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"END")){
    closeScope();
    state->scopeLevel--;
    if(state->scopeLevel<state->procScope){//exited procedure
      state->currentProcId=-1;
      state->procScope=-1;
    }
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={.block=BLOCK_END}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"RETURN")){
    if(state->currentProcId<0){
      fputs("unexpected return statement\n",stderr);
    }
    (*op)=(Operation){.opType=OP_RETURN,.dataType=*procTypes[state->currentProcId].outType,.filePos=wordPos,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ENTRY_POINT")){
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
  }else if(wordEquals(&word,"PRINT")){
    (*op)=(Operation){.opType=OP_PRINT,.dataType=TYPE_UNDEFINED,.filePos=wordPos,.dataAs={0}};//printed type will be determined by type-checker
    return (SizeOrError){.isError=false,.as={.size=1}};
  } 
  
  ScopeNode* asIdentifier;
  int r=getIdentifier(word,&asIdentifier);//try to parse variable as identifier
  if(r<0)//internal error while reading identifier
    return (SizeOrError){.isError=true,.as={.error={.errorCode=r,.pos=wordPos}}};
  if(r==0){//identifier
    (*op)=(Operation){.opType=asIdentifier->idType==ID_PROCEDURE?OP_CALL:OP_GET,
      .dataType=asIdentifier->type,.filePos=wordPos,.dataAs={.idInfo={.type=asIdentifier->idType,.id=asIdentifier->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  //old parser code TODO update when needed
  /*
  if(wordEquals(&word,"ARG")||wordEquals(&word,"GET_ARG")){
    IntOrErrorCode index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    //TODO determine type of argument
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_ARGUMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET_ARG")){
    IntOrErrorCode index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_SET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_ARGUMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"VALUE_AT")||wordEquals(&word,"GET_VALUE_AT")){
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_POINTER,.id=0}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET_VALUE_AT")){
    (*op)=(Operation){.opType=OP_SET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_POINTER,.id=0}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"[]")){
    IntOrErrorCode index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_POINTER,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"[]=")){
    IntOrErrorCode index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_SET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_POINTER,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"CALL")){
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
  CompilerState state=(CompilerState){.currentProcId=-1,.procScope=0,.currentScope=scopeBuffer,.scopeLevel=0,.hasEntryPoint=false};
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
  return (Program){.ops=compileOps,.opCount=opCount,.globalScope=scopeBuffer,.hasEntryPoint=state.hasEntryPoint};
}

//TODO find correct error positions in file
void typeErrorMessage(const char* exprName,DataType expected,DataType got){
  fprintf(stderr,"wrong type for %s: expected ",exprName);
  printTypeName(expected,stderr);
  fputs(" got ",stderr);
  printTypeName(got,stderr);
  fputs("\n",stderr);
}
typedef struct{
  bool isError;
  union{
    DataType* type;
    int error;
  } as;
}TypeOrError;

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
DataType typeCheckPointerArithmetic(DataType a,DataType b,bool subtract){
  if(a.typeClass!=TYPECLASS_POINTER&&a.typeClass!=TYPECLASS_CONST_POINTER)
    return TYPE_UNDEFINED;//a is no pointer
  if(b.typeClass==TYPECLASS_PRIMITIVE&&isInteger(b.typeDataAs.primitive)){
    return a;
  }
  if(subtract&&typeEquals(a,b)){
    return primitiveType(PRIMITIVE_I64);
  }
  return TYPE_UNDEFINED;
}
DataType typeCheckArithmetic(DataType a,DataType b){
  if(a.typeClass!=TYPECLASS_PRIMITIVE||b.typeClass!=TYPECLASS_PRIMITIVE)
    return TYPE_UNDEFINED;//arithmetic only on primitive types
  int r1=numberRank(a.typeDataAs.primitive);
  int r2=numberRank(b.typeDataAs.primitive);
  if(isInteger(a.typeDataAs.primitive)!=isInteger(b.typeDataAs.primitive))
    return TYPE_UNDEFINED;//implicit int to float conversion
  if(r1<=0||r2<=0)
    return TYPE_UNDEFINED;
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_VOID)
    return TYPE_UNDEFINED;
  return primitiveType(res);
}
DataType typeCheckCompare(DataType a,DataType b){
  if(a.typeClass!=TYPECLASS_PRIMITIVE||b.typeClass!=TYPECLASS_PRIMITIVE)
    return TYPE_UNDEFINED;//comparison only on primitive types
  int r1=numberRank(a.typeDataAs.primitive);
  int r2=numberRank(b.typeDataAs.primitive);
  if(isInteger(a.typeDataAs.primitive)!=isInteger(b.typeDataAs.primitive))
    return TYPE_UNDEFINED;//implicit int to float conversion
  if(r1<=0||r2<=0)
    return TYPE_UNDEFINED;//comparison only between numbers
  return primitiveType(PRIMITIVE_BOOL);
}
DataType typeCheckIntLogic(DataType a,DataType b){
  if(a.typeClass!=TYPECLASS_PRIMITIVE||b.typeClass!=TYPECLASS_PRIMITIVE)
    return TYPE_UNDEFINED;//comparison only on primitive types
  if(!isInteger(a.typeDataAs.primitive)||!isInteger(b.typeDataAs.primitive))
    return TYPE_UNDEFINED;//both arguments have to be integers
  int r1=numberRank(a.typeDataAs.primitive);
  int r2=numberRank(b.typeDataAs.primitive);
  //r1 and r2 both are valid numbers
  PrimitiveType res=numberByRank(r1>r2?r1:r2);
  if(res==PRIMITIVE_VOID)
    return TYPE_UNDEFINED;
  return primitiveType(res);
}

#define INIT_CAP 128
typedef struct{
  DataType type;
  int32_t opCount;
  bool isPure;//checks if code block represents pure function
}TypeInfo;

typedef struct{
  size_t elsePos;
  int32_t elifCount;
}IfBlockInfo;
typedef struct{
  bool hasDo;
}DoBlockInfo;
typedef struct{
  BlockType type;
  size_t blockStart;
  union{
    int64_t i64;
    IfBlockInfo ifBlock;
    DoBlockInfo doBlock;
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
  
  size_t index;
}TypeCheckState;

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
}
//TODO extract shifting of types/operations into their own functions

bool checkNonemptyStack(TypeCheckState* state,const char* message){
  if(state->opStackCount>0){
    fputs(message,stderr);
    fprintf(stderr," %s\n",opName(state->opStack[0].opType));
    return true;
  }
  return false;
}
Error pushValue(TypeCheckState* state,Operation op,bool isConst){
  if(ensureOpStackCap(state,state->opStackCount+1)||ensureTypeStackCap(state,state->typeCount+1))
    return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
  state->opStack[state->opStackCount++]=op;
  state->typeStack[state->typeCount++]=(TypeInfo){.type=op.dataType,.opCount=1,.isPure=isConst};
  return (Error){.errorCode=0,.pos=op.filePos};
}

//append op and the first stackOps operations from the stack to the program, remove types elements from the typestack
Error addCompiledOp(TypeCheckState* state,Operation op,size_t stackOps,size_t types){
  if(ensureOpCap(state,state->opCount+stackOps+1))
      return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};
  state->compiledOperations[state->opCount++]=op;
  memcpy(state->compiledOperations+state->opCount,state->opStack+state->opStackCount-stackOps,stackOps*sizeof(Operation));
  state->opCount+=stackOps;
  state->opStackCount-=stackOps;
  state->typeCount-=types;
  return (Error){.errorCode=0,.pos=op.filePos};
}

Error typeCheckCall(Operation* op,TypeCheckState* state){
  DataType calledType=op->dataType;
  //TODO call of function pointer
  //  need check for value of pointer
  if(calledType.typeClass!=TYPECLASS_PROCEDURE&&
    ((calledType.typeClass!=TYPECLASS_POINTER&&calledType.typeClass!=TYPECLASS_CONST_POINTER)||
      calledType.typeDataAs.type->typeClass!=TYPECLASS_PROCEDURE)){//not procedure or pointer to procedure 
    fputs("cannot call objects of type ",stderr);
    printTypeName(calledType,stderr);
    fputs("\n",stderr);
    return (Error){.errorCode=ERROR_TYPE,.pos=op->filePos};
  }
  ProcedureType* procType=calledType.typeDataAs.procedure;
  size_t argCount=1;
  size_t totalOps=0;
  if(procType->inType->typeClass==TYPECLASS_PRIMITIVE&&procType->inType->typeDataAs.primitive==PRIMITIVE_VOID){//no arguments
    if(ensureOpCap(state,state->opCount+1))
      return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
    state->compiledOperations[state->opCount++]=*op;
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
    if(!typeEquals(*procType->inType,state->typeStack[offset].type)){
      typeErrorMessage("procedure argument",*procType->inType,state->typeStack[offset].type);
      return (Error){.errorCode=ERROR_TYPE,.pos=op->filePos};
    }
    totalOps=state->typeStack[offset].opCount;
  }
  if(argCount>1){//argument is flat tuple
    CompositeType* inTypes=procType->inType->typeDataAs.composite;
    for(int32_t i=0;i<inTypes->typeCount;i++){
      if(!typeEquals(inTypes->types[i],state->typeStack[offset+i].type)){
          typeErrorMessage("procedure argument",inTypes->types[i],state->typeStack[offset+i].type);
          return (Error){.errorCode=ERROR_TYPE,.pos=op->filePos};
      }
      totalOps+=state->typeStack[offset+i].opCount;
    }
  }
  
  DataType outType=*(procType->outType);
  if(outType.typeClass==TYPECLASS_PRIMITIVE&&outType.typeDataAs.primitive==PRIMITIVE_VOID){//no return values
    return addCompiledOp(state,*op,totalOps,argCount);
  }
  if(outType.typeClass==TYPECLASS_FLAT_TUPLE){//TODO auto-unwarp multi-return values using flat-tuple return values
    outType.typeClass=TYPECLASS_TUPLE;//convert flat tuple to tuple until there is auto-unwrapping 
  }
  //update op-stack
  if(ensureOpStackCap(state,state->opStackCount+1))
    return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
  memmove(state->opStack+state->opStackCount-totalOps+1,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
  state->opStack[state->opStackCount-totalOps]=*op;
  state->opStackCount++;
  //update type stack
  if(argCount<1&&ensureTypeStackCap(state,state->typeCount+1))
    return (Error){.errorCode=ERROR_MEMORY,.pos=op->filePos};
  state->typeCount-=argCount;
  state->typeStack[state->typeCount++]=(TypeInfo){.type=outType,.opCount=totalOps+1,.isPure=false};//TODO? pure functions
  return (Error){.errorCode=0,.pos=op->filePos};
}

Error typeCheckOperation(Operation op,TypeCheckState* state){
  size_t totalOps=0;
  int32_t offset;
  Error r;
  bool isPure;
  BlockInfo blockInfo;
  switch(op.opType){
    case OP_CONSTANT:
    case OP_STRING_CONST:
      return pushValue(state,op,true);
    case OP_UNARY_OPERATOR:
      if(state->typeCount<1){
        fprintf(stderr,"not enough operands for unary operator %s: need 1 got %zu\n",binOpName(op.dataAs.binOp),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      isPure=true;
      op.dataType=state->typeStack[offset].type;//unary operator returns value of same type
      switch(op.dataAs.unOp){
        case INCREMENT:
        case DECREMENT:
          isPure=false;//TODO check if value can be incremented/decremented
          //fall through
        case NEGATE:
        case FLIP:
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_PRIMITIVE||!isInteger(state->typeStack[offset].type.typeDataAs.primitive)){
            fprintf(stderr,"wrong operand type for unary operator %s expected integer ",unOpName(op.dataAs.unOp));
            fputs(" got ",stderr);
            printTypeName(state->typeStack[offset].type,stderr);
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
      totalOps=state->typeStack[offset].opCount;
      isPure&=state->typeStack[offset].isPure;
      if(ensureOpStackCap(state,state->opStackCount+1))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
      memmove(state->opStack+state->opStackCount-totalOps+1,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
      state->opStack[state->opStackCount-totalOps]=op;
      state->opStackCount++;
      //update type-stack
      state->typeStack[state->typeCount-1].opCount++;
      return (Error){.errorCode=0,.pos=op.filePos};
    case OP_BINARY_OPERATOR:
      if(state->typeCount<2){
        fprintf(stderr,"not enough operands for binary operator %s: need 2 got %zu\n",binOpName(op.dataAs.binOp),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-2;
      bool typesMatch=false;
      switch(op.dataAs.binOp){
        case ADD:
        case SUBTRACT:
          op.dataType=typeCheckPointerArithmetic(state->typeStack[offset].type,state->typeStack[offset+1].type,op.dataAs.binOp==SUBTRACT);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          // fall through
        case MULTIPLY:
        case DIVIDE:
        case MOD:
          op.dataType=typeCheckArithmetic(state->typeStack[offset].type,state->typeStack[offset+1].type);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          break;
        case AND:
        case OR:
        case XOR:
          //integer bool ops
          op.dataType=typeCheckIntLogic(state->typeStack[offset].type,state->typeStack[offset+1].type);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          // fall through
        case FAST_AND:
        case FAST_OR:
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
          
          // fall through
        case GT:
        case GE:
        case LE:
        case LT:
          //number comparison
          op.dataType=typeCheckCompare(state->typeStack[offset].type,state->typeStack[offset+1].type);
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED){
            typesMatch=true;
            break;
          }
          break;
      }
      //TODO implement remaining cases
      if(!typesMatch){
        fprintf(stderr,"No version of binary operator %s supports the types ",binOpName(op.dataAs.binOp));
        printTypeName(state->typeStack[offset].type,stderr);
        fputs(" ",stderr);
        printTypeName(state->typeStack[offset+1].type,stderr);
        fputs("\n",stderr);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      //operator has matching types
      //update operation stack
      totalOps=state->typeStack[offset].opCount+state->typeStack[offset+1].opCount;
      isPure=state->typeStack[offset].isPure&&state->typeStack[offset+1].isPure;
      if(ensureOpStackCap(state,state->opStackCount+1))
        return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
      memmove(state->opStack+state->opStackCount-totalOps+1,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
      state->opStack[state->opStackCount-totalOps]=op;
      state->opStackCount++;
      state->typeCount--;
      state->typeStack[state->typeCount-1]=(TypeInfo){.type=op.dataType,.opCount=totalOps+1,.isPure=isPure};
      return (Error){.errorCode=0,.pos=op.filePos};
    case OP_PRINT:
      if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      offset=state->typeCount-1;
      //can only print non-void primitive
      if(state->typeStack[offset].type.typeClass!=TYPECLASS_POINTER&&state->typeStack[offset].type.typeClass!=TYPECLASS_CONST_POINTER&&
          (state->typeStack[offset].type.typeClass!=TYPECLASS_PRIMITIVE||state->typeStack[offset].type.typeDataAs.primitive==PRIMITIVE_VOID)){
        fputs("cannot print values of type ",stderr);
        printTypeName(state->typeStack[offset].type,stderr);
        fputs("\n",stderr);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      op.dataType=state->typeStack[offset].type;
      //update operations
      return addCompiledOp(state,op,state->typeStack[offset].opCount,1);
    case OP_GET:
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
        case ID_ARGUMENT:
        case ID_PROCEDURE:
          if(op.dataType.typeClass!=TYPECLASS_UNDEFINED)
            return pushValue(state,op,false);
          //unexpected type error
          return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
        case ID_TUPLE_ELEMENT:
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-1;
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_TUPLE){
            printTypeName(state->typeStack[offset].type,stderr);
            fputs(" is not a tuple\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          CompositeType* tuple=state->typeStack[offset].type.typeDataAs.composite;
          if(tuple->typeCount<op.dataAs.idInfo.id){
            fprintf(stderr,"index %"PRIi32" exceeds element count of tuple %"PRIi32"\n",op.dataAs.idInfo.id,tuple->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          //update operation stack
          totalOps=state->typeStack[offset].opCount;
          if(ensureOpStackCap(state,state->opStackCount+1))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          memmove(state->opStack+state->opStackCount-totalOps+1,state->opStack+state->opStackCount-totalOps,totalOps*sizeof(Operation));
          state->opStack[state->opStackCount-totalOps]=op;
          state->opStackCount++;
          //update type-stack
          state->typeStack[offset].type=tuple->types[op.dataAs.idInfo.id];
          state->typeStack[offset].opCount++;
          return (Error){.errorCode=0,.pos=op.filePos};
        case ID_POINTER:
          break;
      }
      break;
    case OP_DECLARE:
    case OP_SET:
      switch(op.dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
        case ID_ARGUMENT:
          if(op.dataAs.idInfo.type==ID_ARGUMENT&&op.opType==OP_DECLARE){
            fputs("cannot declare procedure arguments",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands for operation %s: need 1 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-1;
          if(!typeEquals(op.dataType,state->typeStack[offset].type)){//TODO allow implicit casts
            typeErrorMessage("variable assignment",op.dataType,state->typeStack[offset].type);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          return addCompiledOp(state,op,state->typeStack[offset].opCount,1);
        case ID_TUPLE_ELEMENT:// tuple element SET
          if(op.opType==OP_DECLARE){
            fputs("cannot declare tuple elements",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(state->typeCount<2){
            fprintf(stderr,"not enough operands for operation %s: need 2 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-2;
          if(state->typeStack[offset].type.typeClass!=TYPECLASS_TUPLE){
            printTypeName(state->typeStack[offset].type,stderr);
            fputs(" is not a tuple\n",stderr);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          CompositeType* tuple=state->typeStack[offset].type.typeDataAs.composite;
          if(tuple->typeCount<op.dataAs.idInfo.id){
            fprintf(stderr,"index %"PRIi32" exceeds element count of tuple %"PRIi32"\n",op.dataAs.idInfo.id,tuple->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          if(!typeEquals(tuple->types[op.dataAs.idInfo.id],state->typeStack[offset+1].type)){
            typeErrorMessage("tuple element assignment",tuple->types[op.dataAs.idInfo.id],state->typeStack[offset+1].type);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          return addCompiledOp(state,op,state->typeStack[offset].opCount+state->typeStack[offset+1].opCount,2);
        case ID_POINTER:// pointer value SET
          if(op.opType==OP_DECLARE){
            fputs("cannot declare value at pointer",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(state->typeCount<2){
            fprintf(stderr,"not enough operands for operation %s: need 2 got %zu\n",opName(op.opType),state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-2;
          //TODO set pointer
          break;
        case ID_PROCEDURE:
          fputs("procedures cannot be modified",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
      }
      break;
    case OP_CODE_BLOCK://TODO allow operations to cross block boundaries (within procedures)
      switch(op.dataAs.block){
        case BLOCK_IF:
          blockInfo=peekBlock(state);
          if(blockInfo.type==BLOCK_END){//block stack underflow
            fputs("unexpected IF statement, IF statements cannot be declared at global level\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(blockInfo.type==BLOCK_ELIF){
            state->blockCount--;//update old block instead of creating new block
            blockInfo.type=BLOCK_IF;
          }else{
            blockInfo=(BlockInfo){.type=BLOCK_IF,.blockStart=state->opCount,.blockDataAs={0}};
          }
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          
          if(state->typeCount<1){
            fprintf(stderr,"not enough operands if-condition: need 1 got %zu\n",state->typeCount);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          offset=state->typeCount-1;
          if(!typeEquals(primitiveType(PRIMITIVE_BOOL),state->typeStack[offset].type)){
            typeErrorMessage("variable assignment",op.dataType,state->typeStack[offset].type);
            return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
          }
          r=addCompiledOp(state,op,state->typeStack[offset].opCount,1); 
          if(r.errorCode!=0)
            return r;
          if(checkNonemptyStack(state,"unfinished local operation")){//stack crossing block boundaries not implemented
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_ELSE:
          blockInfo=popBlock(state);
          if(blockInfo.type!=BLOCK_IF){//wrong position for ELSE
            fputs("ELSE can only appear in IF blocks\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(checkNonemptyStack(state,"unfinished local operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
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
        case BLOCK_ELIF:
          blockInfo=popBlock(state);
          if(blockInfo.type!=BLOCK_IF){//wrong position for EL ... IF
            fputs("EL ... IF can only appear in IF blocks\n",stderr);
            printf("%u\n",blockInfo.type);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          
          if(checkNonemptyStack(state,"unfinished local operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          //push updated block
          blockInfo.type=BLOCK_ELIF;
          blockInfo.blockDataAs.ifBlock.elifCount++;
          blockInfo.blockDataAs.ifBlock.elsePos=state->opCount;
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          if(ensureOpCap(state,state->opCount+1))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          state->compiledOperations[state->opCount++]=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=op.filePos,.dataAs={.block=BLOCK_ELSE}};
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_WHILE:
          if(checkNonemptyStack(state,"unfinished local operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(pushBlock(state,(BlockInfo){.type=BLOCK_DO,.blockStart=state->opCount,.blockDataAs={0}}))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          if(ensureOpCap(state,state->opCount+1))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          //1. start block as DO ... WHILE_END
          state->compiledOperations[state->opCount++]=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.filePos=op.filePos,.dataAs={.block=BLOCK_DO}};
          return (Error){.errorCode=0,.pos=op.filePos};
        case BLOCK_DO:
          blockInfo=popBlock(state);
          if(blockInfo.type!=BLOCK_DO){//wrong position for DO
            fputs("DO can only appear in WHILE-DO blocks\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(blockInfo.blockDataAs.doBlock.hasDo){//wrong position for DO
            fputs("DO cannot appear more than once per WHILE block\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          if(state->opCount==blockInfo.blockStart+1){//if do-body empty switch to WHILE ... END
            blockInfo.type=BLOCK_WHILE;
            if(pushBlock(state,blockInfo))
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
            if(state->compiledOperations[blockInfo.blockStart].opType!=OP_CODE_BLOCK)
              return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;//memory got corrupted, op at block start is no longer start of block
            //remove previous operation to be updated by addCompiledOp
            op=state->compiledOperations[--state->opCount];
            op.dataAs.block=BLOCK_WHILE;
            //read while operation
            if(state->typeCount<1){
              fprintf(stderr,"not enough operands for while-condition: need 1 got %zu\n",state->typeCount);
              return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
            }
            offset=state->typeCount-1;
            if(!typeEquals(primitiveType(PRIMITIVE_BOOL),state->typeStack[offset].type)){
              typeErrorMessage("variable assignment",op.dataType,state->typeStack[offset].type);
              return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
            }
            r=addCompiledOp(state,op,state->typeStack[offset].opCount,1); 
            if(r.errorCode!=0)
              return r;
            if(checkNonemptyStack(state,"unfinished local operation")){//stack crossing block boundaries not implemented
              return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
            }
            return (Error){.errorCode=0,.pos=op.filePos};
          }
          blockInfo.blockDataAs.doBlock.hasDo=true;
          if(pushBlock(state,blockInfo))
            return (Error){.errorCode=ERROR_MEMORY,.pos=op.filePos};;
          //TODO general do-while block
          break;
        case BLOCK_WHILE_END:
          fputs("WHILE_END blocks are not supported use WHILE ... DO END to build a do-while statement",stderr);
          return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          /* general Loop code    WHILE F C DO G END
            {
              bool loop2=false; 
              do{
                if(loop2){
                 G
                }
                loop2=true;
                F
              }while(C);
            }
          */
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
          if(blockInfo.type==BLOCK_END||blockInfo.type==BLOCK_ELIF||(blockInfo.type==BLOCK_DO&&!blockInfo.blockDataAs.doBlock.hasDo)){
            fputs("unexpected END statement\n",stderr);
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          //TODO check for procedures with missing return statements
          if(checkNonemptyStack(state,"unfinished local operation")){
            return (Error){.errorCode=ERROR_SYNTAX,.pos=op.filePos};
          }
          int32_t endCount=1;
          if(blockInfo.type==BLOCK_IF||blockInfo.type==BLOCK_ELSE){
            endCount+=blockInfo.blockDataAs.ifBlock.elifCount;
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
        if(!typeEquals(op.dataType,state->typeStack[0].type)){//TODO allow type promotion
          typeErrorMessage("return statement",op.dataType,state->typeStack[0].type);
          return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
        }
        return addCompiledOp(state,op,state->typeStack[0].opCount,1);
      }
      if(op.dataType.typeClass!=TYPECLASS_TUPLE&&op.dataType.typeClass!=TYPECLASS_FLAT_TUPLE){
        checkNonemptyStack(state,"unfinished operation at end of procedure");//this should always return true
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      if(op.dataType.typeClass==TYPECLASS_TUPLE){
        op.dataType.typeClass=TYPECLASS_FLAT_TUPLE;//notify compiler that tuple is flat
      }
      //check tuple elements
      if(op.dataType.typeDataAs.composite->typeCount<0||state->typeCount!=(size_t)op.dataType.typeDataAs.composite->typeCount){
        fprintf(stderr,"wrong number of return values: expected %zu got %i",state->typeCount,op.dataType.typeDataAs.composite->typeCount);
        return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
      }
      for(size_t i=0;i<state->typeCount;i++){
        totalOps+=state->typeStack[i].opCount;
        if(!typeEquals(state->typeStack[i].type,op.dataType.typeDataAs.composite->types[i])){
          typeErrorMessage("return statement",op.dataType.typeDataAs.composite->types[i],state->typeStack[i].type);
          return (Error){.errorCode=ERROR_TYPE,.pos=op.filePos};
        }
      }
      return addCompiledOp(state,op,totalOps,state->typeCount);
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
    .index=0};
  if(state.compiledOperations==NULL||state.opStack==NULL||state.typeStack==NULL){//memory allocation failed
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
int main(int argc,char** argv){
  (void)argc;
  char* code;
  int64_t codeSize;
  path=*(argv++);
  if(*argv==NULL){
    printf("usage: inputFile\n");
    return 0;
  }
  srcFile=*(argv++);
  FILE *file = fopen(srcFile, "r");
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
      printf("%s ",opName(p.ops[i].opType));
      printTypeName(p.ops[i].dataType,stdout);
      puts("");
    }
    puts("");
		//3. compile operations to C
    FILE* out=fopen("./out.c","w");
    err=compileToC(out,p.ops,p.opCount,p.hasEntryPoint);
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
