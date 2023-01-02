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
//TODO split into less general errors
#define ERROR_SYNTAX 2 
#define ERROR_PARSE_INT 3
#define ERROR_REDECLARATION 4
#define ERROR_UNSUPPORTED_ESCAPE_SEQUENCE 5
#define WARNING_CODEPOINT_OUT_OF_RANGE 6
#define ERROR_EOF 7 //end of file
//maximum integer value of any error constant
#define MAX_ERROR 8

typedef struct{
  bool isError;
  union{
    size_t size;
    int    error;
  }as;
}SizeOrError;
typedef struct{
  bool isError;
  union{
    int64_t  i64;
    int    error;
  }as;
}IntOrError;
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
  //TODO tuple operations, access to function parameters
  //ADDR_OF (pointer to given value)
  //OP_BUILD (tuple,union)
  //OP_MULTI_DECLARE //declare multiple variables from a single tuple variable
  /*                             C-code:
                                    type1 name1;type2 name2;...typeN nameN;
                                    {
                                      tupleType tmpName=tupleExpr;
                                      name1=tmpName.e1;name2=tmpName.e2;...nameN=tmpName.eN;
                                    }
  */
  //OP_MULTI_SET     //set multiple variables from a single tuple variable
  
  OP_BINARY_OPERATOR, 
  OP_UNARY_OPERATOR,  
  
  OP_CODE_BLOCK,  
  
  OP_DECLARE_PROCEDURE, 
  OP_RETURN,       
  OP_CALL,         // procType procId
  ENTRY_POINT,     //entry point of the program, starts the main code section, section will close at the matching BLOCK_END 
}OpType;
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
#define MAX_COMPOSITE_ELEMENTS 1024
#define MAX_PROC_TYPES 1024

size_t typeCount=0;
DataType typeData[MAX_TYPES];
int32_t compositeCount=0;
CompositeType compositeTypes[MAX_COMPOSITE];
size_t procTypeCount=0;
ProcedureType procTypes[MAX_PROC_TYPES];
//temporary buffer for construction of composite elements
int64_t bufferOffset=0;
DataType compositeBuffer[MAX_COMPOSITE_ELEMENTS];


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
      return fprintf(file,"flatTuple%"PRIi32,type.typeDataAs.composite->id);
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
typedef enum{//TODO? postfix unary operators (POST_INCREMENT)
  NEGATE,
  INCREMENT,
  DECREMENT,
  NOT,
  FLIP,
}UnaryOperator;
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
IntOrError addProgString(String s){
  if(progStringCount+1>=MAX_PROG_STRINGS)
    return (IntOrError){.isError=true,.as={.error=ERROR_MEMORY}};
  //TODO find duplicate strings
  programStrings[progStringCount]=(ProgramString){.value=s,.stringId=progStringCount,.charsId=-1,.charsOffset=-1};
  return (IntOrError){.isError=false,.as={.i64=progStringCount++}};
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
              return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
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
          return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
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
          return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
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
          return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
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
          return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
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
          return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
        case ID_ARGUMENT:
          fputs("cannot declare arguments",stderr);
          return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
        case ID_TUPLE_ELEMENT:
          fputs("cannot declare tuple elements",stderr);
          return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
        case ID_POINTER:
          fputs("cannot declare pointers",stderr);
          return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
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
        return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
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
      fprintf(stderr,"operation %i is not implemented\n",op->opType);
      return (SizeOrError){.isError=true,.as={.error=ERROR_UNIMPLEMENTED}};
  }
  return (SizeOrError){.isError=false,.as={.size=size}};
}

int compileToC(FILE* target,const Operation* ops,size_t opCount,bool hasEntryPoint){
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
  return 0;
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
Scope* openScope(void){
  if(scopeCount+1>=SCOPE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of nested scopes %i\n",SCOPE_CAP);
    return NULL;
  }
  scopeBuffer[scopeCount].nodes=malloc(SCOPE_MAP_CAP*sizeof(ScopeNode*));
  scopeBuffer[scopeCount].nodeBufferOffset=scopeNodeCount;
  scopeBuffer[scopeCount].parent=scopeCount>0?scopeBuffer+(scopeCount-1):NULL;
  return scopeBuffer+(scopeCount++);
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
  fprintf(stderr,"cannot find identfier %.*s\n",(int)name.length,name.chars);
  return ERROR_SYNTAX;
}


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


void skipWhitespaces(char** code,size_t* codeSize){
  while(*codeSize>0&&((**code)==0||isspace(**code))){
    (*codeSize)--;
    (*code)++;
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
IntOrError parseInt(String number,int base){
  bool detectBase=(base==0);
  if(detectBase)
    base=10;
  size_t i=0;
  int digit;
  uint64_t value=0;
  bool negate=false;
  if(number.length==0)
    return (IntOrError){.isError=true,.as={.error=ERROR_PARSE_INT}};
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
    if(digit<0)
      return (IntOrError){.isError=true,.as={.error=ERROR_PARSE_INT}};
    value+=digit;
  }
  return (IntOrError){.isError=false,.as={.i64=negate?-value:value}};
}
String readStringLiteral(char** code,size_t* codeSize,char end,bool doEspaceSeqs,int* errorFlag){
  if(*codeSize<1){
    *errorFlag=ERROR_EOF;
    return (String){.chars=*code,.length=0};
  }
  //skip first char
  (*code)++;
  (*codeSize)--;
  char* wordChars=*code;
  size_t wordLength=0,delta=0;
  size_t sequenceLength;//length of escape sequence for multi-char sequences
  while(*codeSize>0&&**code!=end){
    if(doEspaceSeqs&&**code=='\\'){//escaped characters
      if(*codeSize<=1){
        *errorFlag=ERROR_EOF;
        return (String){.chars=*code,.length=0};
      }
      delta++;
      (*codeSize)--;//ignore the \ character
      (*code)++;
      switch(**code){//decode escape sequence
        case 'b':
          **code='\b';
          break;
        case 'n':
          **code='\n';
          break;
        case 't':
          **code='\t';
          break;
        case 'r':
          **code='\r';
          break;
        case 'f':
          **code='\f';
          break;
        case 'v':
          **code='\v';
          break;
        case '\'':
        case '\\':
        case '"':
          break; //keep the original character
        case 'x':
        case 'u':
        case 'U':
          sequenceLength=**code=='U'?9:**code=='u'?5:3;
          if(*codeSize<sequenceLength){
            if(errorFlag)
              *errorFlag=ERROR_EOF;
            return (String){.chars=*code,.length=0};
          }
          IntOrError val=parseInt((String){.chars=(*code)+1,.length=(sequenceLength-1)},16);
          if(val.isError){
            if(errorFlag)
              *errorFlag=val.as.error;
            return (String){.chars=*code,.length=0};
          }
          int l;
          if(**code=='x'){
            wordChars[wordLength]=val.as.i64;
            l=1;
          }else{
            if(errorFlag&&(val.as.i64<0||val.as.i64>MAX_CODEPOINT))
              *errorFlag=WARNING_CODEPOINT_OUT_OF_RANGE;
            l=writeUnicodeChar(val.as.i64,wordChars+wordLength);
          }
          wordLength+=l;
          delta+=sequenceLength-l;
          (*code)+=sequenceLength;
          (*codeSize)-=sequenceLength;
          continue;//skip to next iteration of loop
        default:
          if(errorFlag)
            *errorFlag=ERROR_UNSUPPORTED_ESCAPE_SEQUENCE;
          break; 
      }
    }
    if(delta>0){//copy chars to position in unescaped string
      wordChars[wordLength]=**code;
    }
    wordLength++;
    (*codeSize)--;
    (*code)++;
  }
  if(*codeSize==0){
    *errorFlag=ERROR_EOF;
    fprintf(stderr,"unfinished comment or string literal %.*s \n",(int)wordLength,*code);
    return (String){.chars=*code,.length=0};//TODO error handling
  }
  //move code-pointer to position after word
  (*codeSize)--;
  (*code)++;
  wordChars[wordLength]=0;//zero terminate string
  return (String){.chars=wordChars,.length=wordLength};
}
//constants for the wordType flag of nextWord
//allow to determine which type of word was read
//positive values are used to indicate syntax errors
#define WORD_TYPE_STRING (MAX_ERROR+1)
#define WORD_TYPE_CHAR   (MAX_ERROR+2)
String nextWord(char** code,size_t* codeSize,int* wordType){
  skipWhitespaces(code,codeSize);
  if(*codeSize<=0)
    return (String){.chars=*code,.length=0};
  if(**code=='"'){
    if(wordType)
      *wordType=WORD_TYPE_STRING;
    return readStringLiteral(code,codeSize,'"',true,wordType);
  }else if(**code=='\''){
    if(wordType)
      *wordType=WORD_TYPE_CHAR;
    return readStringLiteral(code,codeSize,'\'',true,wordType);
  }else if(**code=='#'){//TODO? inline comments, start comment with double hash
    readStringLiteral(code,codeSize,'\n',false,wordType);//ignore everything up to next new-line
    return (String){.chars=*code,.length=0};
  }
  char* wordChars=*code;
  size_t wordLength=0;
  while(*codeSize>0&&(**code)!=0&&!isspace(**code)){
    (*code)++;
    (*codeSize)--;
    wordLength++;
  }
  if(*codeSize>0)
    **code=0;//zero terminate command
  return (String){.chars=wordChars,.length=wordLength};
}

DataType readCompositeType(TypeClass typeClass,char** code,size_t* codeSize);//predeclare
DataType readType(char** code,size_t* codeSize){
  String name=nextWord(code,codeSize,NULL);
  if(name.length==0)
    return TYPE_UNDEFINED;
  if(wordEquals(&name,"VOID"))
    return primitiveType(PRIMITIVE_VOID);
  if(wordEquals(&name,"BOOL"))
    return primitiveType(PRIMITIVE_BOOL);
  if(wordEquals(&name,"I8")||wordEquals(&name,"CHAR"))
    return primitiveType(PRIMITIVE_I8);
  if(wordEquals(&name,"I32"))
    return primitiveType(PRIMITIVE_I32);
  if(wordEquals(&name,"I64"))
    return primitiveType(PRIMITIVE_I64);
  if(wordEquals(&name,"FLOAT"))
    return primitiveType(PRIMITIVE_FLOAT);
  if(wordEquals(&name,"STRING"))
    return progStringType();
  if(wordEquals(&name,"PTR")){
    DataType target=readType(code,codeSize);
    if(typeEquals(target,TYPE_UNDEFINED))
      return TYPE_UNDEFINED;
    return pointerType(target);
  }
  if(wordEquals(&name,"TUPLE")){
    return readCompositeType(TYPECLASS_TUPLE,code,codeSize);
  }
  if(wordEquals(&name,"UNION")){
    return readCompositeType(TYPECLASS_UNION,code,codeSize);
  }
  if(wordEquals(&name,"PROC")||wordEquals(&name,"PROCEDURE")){
    DataType inTypes=readCompositeType(TYPECLASS_FLAT_TUPLE,code,codeSize);
    if(typeEquals(inTypes,TYPE_UNDEFINED))
      return TYPE_UNDEFINED;
    DataType outType=readType(code,codeSize);
    if(typeEquals(outType,TYPE_UNDEFINED))
      return TYPE_UNDEFINED;
    return procedureType(inTypes,outType);
  }
  fprintf(stderr,"unknown type name: %.*s \n",(int)name.length,name.chars);
  return TYPE_UNDEFINED;
}
DataType readCompositeType(TypeClass typeClass,char** code,size_t* codeSize){
  IntOrError count=parseInt(nextWord(code,codeSize,NULL),0);
  if(count.isError){
    fprintf(stderr,"parser error %i while reading type\n",count.as.error);//TODO better error handling
    return TYPE_UNDEFINED;
  }
  if(count.as.i64>MAX_COMPOSITE_ELEMENTS-bufferOffset){
    fprintf(stderr,"structure size %"PRIi64" exceed maximum size %"PRIi16" \n",count.as.i64,MAX_COMPOSITE_ELEMENTS);
    return TYPE_UNDEFINED;
  }
  size_t initOffset=bufferOffset;
  for(int16_t k=0;k<count.as.i64;k++,bufferOffset++){
    compositeBuffer[bufferOffset]=readType(code,codeSize);
    if(typeEquals(compositeBuffer[bufferOffset],TYPE_UNDEFINED)){
      bufferOffset=initOffset;
      return TYPE_UNDEFINED;
    }
  }
  bufferOffset=initOffset;
  return compositeType(typeClass,compositeBuffer+bufferOffset,count.as.i64);
}

SizeOrError readOperation(Operation* op,char** code,size_t* codeSize,CompilerState* state){
  int wordType=0;
  String word=nextWord(code,codeSize,&wordType);
  if(wordType==WORD_TYPE_STRING){
    IntOrError strId=addProgString(word);
    if(strId.isError)
      return (SizeOrError){.isError=true,.as={.error=strId.as.error}};
    (*op)=(Operation){.opType=OP_STRING_CONST,.dataType=progStringType(),.dataAs={.i64=strId.as.i64}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(wordType==WORD_TYPE_CHAR){
    if(word.length!=1){//TODO? handle unicode characters
      fprintf(stderr,"character literal '%.*s' contains more that one character\n",(int)word.length,word.chars);
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    }
    (*op)=(Operation){.opType=OP_CONSTANT,.dataType=primitiveType(PRIMITIVE_I8),.dataAs={.i64=word.chars[0]}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }
  if(wordType!=0)
    return (SizeOrError){.isError=true,.as={.error=wordType}};
  
  if(word.length==0)
    return (SizeOrError){.isError=false,.as={.size=0}};
  if(wordEquals(&word,"PRINT")){
    DataType type=readType(code,codeSize);
    if(typeEquals(type,TYPE_UNDEFINED))
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    (*op)=(Operation){.opType=OP_PRINT,.dataType=type,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"CONST")){
    DataType type=readType(code,codeSize);
    if(typeEquals(type,TYPE_UNDEFINED))
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    if(type.typeClass!=TYPECLASS_PRIMITIVE){
      fputs("constants of non-primitive type ",stderr);
      printTypeName(type,stderr);
      fputs(" are not supported \n",stderr);
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    }
    
    String constValue=nextWord(code,codeSize,NULL);
    switch(type.typeDataAs.primitive){
      case PRIMITIVE_BOOL:
      case PRIMITIVE_I8:
      case PRIMITIVE_I32:
      case PRIMITIVE_I64:
        IntOrError val=parseInt(constValue,0);
        if(val.isError){
          printf("cannot parse \"%.*s\" to an integer \n",(int)constValue.length,constValue.chars);
          return (SizeOrError){.isError=true,.as={.error=val.as.error}};
        }
        (*op)=(Operation){.opType=OP_CONSTANT,.dataType=type,.dataAs={.i64=val.as.i64}};
        return (SizeOrError){.isError=false,.as={.size=1}};
      default:
        fprintf(stderr,"constants of type %s are currently not supported",primitiveName(type.typeDataAs.primitive));
        return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    }
    return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
  }else if(wordEquals(&word,"GET")){
    String varName=nextWord(code,codeSize,NULL);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    ScopeNode* id;
    int r=getIdentifier(varName,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error=r}};
    (*op)=(Operation){.opType=OP_GET,.dataType=id->type,.dataAs={.idInfo={.type=id->idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET")){
    String varName=nextWord(code,codeSize,NULL);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    ScopeNode* id;
    int r=getIdentifier(varName,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error=r}};
    (*op)=(Operation){.opType=OP_SET,.dataType=id->type,.dataAs={.idInfo={.type=id->idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"DECLARE")){
    DataType type=readType(code,codeSize);
    if(typeEquals(type,TYPE_UNDEFINED))
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    String varName=nextWord(code,codeSize,NULL);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    IdentiferType idType=type.typeClass==TYPECLASS_PROCEDURE?ID_PROCEDURE:state->scopeLevel>0?ID_LOCAL_VAR:ID_GLOBAL_VAR;
    ScopeNode* id;
    int r=declareIdentifier(varName,type,idType,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error=r}};
    if(idType==ID_PROCEDURE){
      if(state->scopeLevel>0){
        fprintf(stderr,"invalid position for procedure %.*s procedures can only be declared at top level\n",(int)varName.length,varName.chars);
        return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
      }
      Scope* newScope=openScope();
      if(newScope==NULL)
        return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
      state->currentScope=newScope;
      state->scopeLevel++;
      state->procScope=state->scopeLevel;
      state->currentProcId=type.typeDataAs.procedure->id;
            
      (*op)=(Operation){.opType=OP_DECLARE_PROCEDURE,.dataType=type,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    }else{
      (*op)=(Operation){.opType=OP_DECLARE,.dataType=type,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    }
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"GET_ELEMENT")){
    IntOrError index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET_ELEMENT")){
    IntOrError index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_SET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ARG")||wordEquals(&word,"GET_ARG")){
    IntOrError index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    //TODO determine type of argument
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_ARGUMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET_ARG")){
    IntOrError index=parseInt(nextWord(code,codeSize,NULL),0);
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
    IntOrError index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_POINTER,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"[]=")){
    IntOrError index=parseInt(nextWord(code,codeSize,NULL),0);
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_SET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_POINTER,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ADD")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=ADD}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SUBTRACT")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=SUBTRACT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"MULTIPLY")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=MULTIPLY}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"DIVIDE")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=DIVIDE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"MOD")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=MOD}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"AND")||wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=AND}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"OR")||wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=OR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"XOR")||wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=XOR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"AND2")||wordEquals(&word,"&&")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=FAST_AND}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"OR2")||wordEquals(&word,"||")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=FAST_OR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"EQ")||wordEquals(&word,"==")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=EQ}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"NE")||wordEquals(&word,"NEQ")||wordEquals(&word,"!=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=NE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"GT")||wordEquals(&word,">")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=GT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"GE")||wordEquals(&word,">=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=GE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"LE")||wordEquals(&word,"<=")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=LE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"LT")||wordEquals(&word,"<")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=LT}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"NEG")||wordEquals(&word,"NEGATE")){
    (*op)=(Operation){.opType=OP_UNARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.unOp=NEGATE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"IF")){//TODO check scope boundaries 
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.dataAs={.block=BLOCK_IF}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ELIF")){
    closeScope();
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.dataAs={.block=BLOCK_ELIF}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"WHILE")){
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.dataAs={.block=BLOCK_WHILE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ELSE")){
    closeScope();
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.dataAs={.block=BLOCK_ELSE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"END")){
    closeScope();
    state->scopeLevel--;
    if(state->scopeLevel<state->procScope){//exited procedure
      state->currentProcId=-1;
      state->procScope=-1;
    }
    
    (*op)=(Operation){.opType=OP_CODE_BLOCK,.dataType=TYPE_UNDEFINED,.dataAs={.block=BLOCK_END}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"RETURN")){
    if(state->currentProcId<0){
      fputs("unexpected return statement\n",stderr);
    }
    
    
    (*op)=(Operation){.opType=OP_RETURN,.dataType=*procTypes[state->currentProcId].outType,.dataAs={0}};
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
  }else if(wordEquals(&word,"START")){
    if(state->hasEntryPoint){
      fputs("program can only have one entry point",stderr);
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    }
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    state->hasEntryPoint=true;
    (*op)=(Operation){.opType=ENTRY_POINT,.dataType=TYPE_UNDEFINED,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else{
    fprintf(stderr,"unknown command: %.*s\n",(int)word.length,word.chars);
    return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
  }
  return (SizeOrError){.isError=false,.as={.size=0}};
}
Program compileToOps(char* code,size_t codeSize){
  size_t opCount=0;
  size_t opsCap=256;
  SizeOrError r;
  Operation* compileOps=malloc(opsCap*sizeof(Operation));
  openScope();
  CompilerState state=(CompilerState){.currentProcId=-1,.procScope=0,.currentScope=scopeBuffer,.scopeLevel=0,.hasEntryPoint=false};
  while(codeSize>0){
    r=readOperation(compileOps+opCount,&code,&codeSize,&state);
    if(r.isError){
      fprintf(stderr,"error %i\n",r.as.error);
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
TypeOrError typeCheckExpression(Program prog,size_t* offset);
TypeOrError typeCheckCall(DataType calledType,Program prog,size_t* offset){
  //TODO call of function pointer
  //  need check for value of pointer
  if(calledType.typeClass!=TYPECLASS_PROCEDURE&&
    ((calledType.typeClass!=TYPECLASS_POINTER&&calledType.typeClass!=TYPECLASS_CONST_POINTER)||
      calledType.typeDataAs.type->typeClass!=TYPECLASS_PROCEDURE)){//not procedure or pointer to procedure 
    fputs("cannot call objects of type ",stderr);
    printTypeName(calledType,stderr);
    fputs("\n",stderr);
    return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
  }
  if(calledType.typeClass!=TYPECLASS_PROCEDURE)
    calledType=*calledType.typeDataAs.type;//dereference procedure-pointer
  ProcedureType* procType=calledType.typeDataAs.procedure;
  if(procType->inType->typeClass==TYPECLASS_PRIMITIVE&&procType->inType->typeDataAs.primitive==PRIMITIVE_VOID)
    return (TypeOrError){.isError=false,.as={.type=procType->outType}};//void function
  TypeOrError r=typeCheckExpression(prog,offset);//first argument
  if(r.isError)
    return (TypeOrError){.isError=true,.as={.error=r.as.error}};
  if(procType->inType->typeClass!=TYPECLASS_FLAT_TUPLE){//function takes single argument
    if(!typeEquals(*procType->inType,*r.as.type)){
      typeErrorMessage("procedure argument",*procType->inType,*r.as.type);
      return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    }
    return (TypeOrError){.isError=false,.as={.type=procType->outType}};
  }
  CompositeType* inTypes=procType->inType->typeDataAs.composite;
  if(!typeEquals(inTypes->types[0],*r.as.type)){
      typeErrorMessage("procedure argument",inTypes->types[0],*r.as.type);
      return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
  }
  for(int32_t i=1;i<inTypes->typeCount;i++){
    r=typeCheckExpression(prog,offset);//i-th argument
    if(r.isError)
      return (TypeOrError){.isError=true,.as={.error=r.as.error}};
    if(!typeEquals(inTypes->types[i],*r.as.type)){
        typeErrorMessage("procedure argument",inTypes->types[i],*r.as.type);
        return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    }
  }
  return (TypeOrError){.isError=false,.as={.type=procType->outType}};//all arguments correct
}
TypeOrError typeCheckExpression(Program prog,size_t* offset){
  if(*offset>=prog.opCount)//unexpected end of program
    return (TypeOrError){.isError=true,.as={.error=ERROR_EOF}};
  Operation* op=prog.ops+*offset;
  DataType* type;
  DataType tmpType;
  (*offset)++;
  TypeOrError r;
  switch(op->opType){
    //0 to 1:
    case OP_CONSTANT:
    case OP_STRING_CONST:
      return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
    case OP_GET:
      switch(op->dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
        case ID_ARGUMENT:
        case ID_PROCEDURE:
          type=&(op->dataType);
          if(type->typeClass!=TYPECLASS_UNDEFINED)
            return (TypeOrError){.isError=false,.as={.type=type}};
          return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
        case ID_TUPLE_ELEMENT:
          r=typeCheckExpression(prog,offset);//tuple
          if(r.isError)
            return (TypeOrError){.isError=true,.as={.error=r.as.error}};
          if(r.as.type->typeClass!=TYPECLASS_TUPLE){
            printTypeName(*r.as.type,stderr);
            fputs(" is not a tuple\n",stderr);
            return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
          }
          CompositeType* tuple=r.as.type->typeDataAs.composite;
          if(tuple->typeCount<op->dataAs.idInfo.id){
            fprintf(stderr,"index %"PRIi32" exceeds element count of tuple %"PRIi32"\n",op->dataAs.idInfo.id,tuple->typeCount);
            return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
          }
          return (TypeOrError){.isError=false,.as={.type=tuple->types+op->dataAs.idInfo.id}};
        case ID_POINTER:
          break;
      }
      //TODO get pointer
      break;
    //n to 1:
    case OP_UNARY_OPERATOR:
      r=typeCheckExpression(prog,offset);//operand
      if(r.isError)
        return (TypeOrError){.isError=true,.as={.error=r.as.error}};
      op->dataType=*r.as.type;//unary operator returns value of same type
      switch(op->dataAs.unOp){
        case NEGATE:
        case FLIP:
        case INCREMENT://TODO check if value can be incremented/decremented
        case DECREMENT:
          if(r.as.type->typeClass!=TYPECLASS_PRIMITIVE||!isInteger(r.as.type->typeDataAs.primitive)){
            fputs("wrong type for unary operator  expected integer ",stderr);//TODO print operator name
            fputs(" got ",stderr);
            printTypeName(*r.as.type,stderr);
            fputs("\n",stderr);
            return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
          }
          return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
        case NOT:
          if(r.as.type->typeClass!=TYPECLASS_PRIMITIVE||r.as.type->typeDataAs.primitive!=PRIMITIVE_BOOL){
            typeErrorMessage("unary operator NOT",primitiveType(PRIMITIVE_BOOL),*r.as.type);
            return (TypeOrError){.isError=true,.as={.error=ERROR_TYPE}};
          }
          return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
      }
      break;
    case OP_BINARY_OPERATOR:
      r=typeCheckExpression(prog,offset);//first operand
      if(r.isError)
        return (TypeOrError){.isError=true,.as={.error=r.as.error}};
      tmpType=*r.as.type;//remember first operand
      r=typeCheckExpression(prog,offset);//second operand
      if(r.isError)
        return (TypeOrError){.isError=true,.as={.error=r.as.error}};
      switch(op->dataAs.binOp){
        case ADD:
        case SUBTRACT:
          op->dataType=typeCheckPointerArithmetic(tmpType,*r.as.type,op->dataAs.binOp==SUBTRACT);
          if(op->dataType.typeClass!=TYPECLASS_UNDEFINED)
            return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
          // fall through
        case MULTIPLY:
        case DIVIDE:
        case MOD:
          op->dataType=typeCheckArithmetic(tmpType,*r.as.type);
          if(op->dataType.typeClass!=TYPECLASS_UNDEFINED)
            return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
          break;
        case AND:
        case OR:
        case XOR:
          //integer bool ops
          op->dataType=typeCheckIntLogic(tmpType,*r.as.type);
          if(op->dataType.typeClass!=TYPECLASS_UNDEFINED)
            return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
          // fall through
        case FAST_AND:
        case FAST_OR:
          //bool ops
          if(tmpType.typeClass==TYPECLASS_PRIMITIVE&&tmpType.typeDataAs.primitive==PRIMITIVE_BOOL&&
              r.as.type->typeClass==TYPECLASS_PRIMITIVE&&r.as.type->typeDataAs.primitive==PRIMITIVE_BOOL){
            op->dataType=primitiveType(PRIMITIVE_BOOL);
            return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
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
          op->dataType=typeCheckCompare(tmpType,*r.as.type);
          if(op->dataType.typeClass!=TYPECLASS_UNDEFINED)
            return (TypeOrError){.isError=false,.as={.type=&(op->dataType)}};
          break;
      }
      //TODO implement remaining cases
      //TODO error message for fall-through cases
      break;
    case OP_CALL:
      return typeCheckCall(op->dataType,prog,offset);
    //n to 0:
    case OP_DECLARE:
    case OP_SET:
    case OP_RETURN:
    case OP_PRINT:
    case OP_CODE_BLOCK:
    case OP_DECLARE_PROCEDURE:
    case ENTRY_POINT:
      fprintf(stderr,"unexpected operation %i\n",op->opType);
      return (TypeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
  }
  fprintf(stderr,"type-checking operation %i is not implemented\n",op->opType);
  return (TypeOrError){.isError=true,.as={.error=ERROR_UNIMPLEMENTED}};//unexpected operation
}
int typeCheckStatement(Program prog,size_t* offset){
  if(*offset>=prog.opCount)
    return 0;//nothing to do
  Operation* op=prog.ops+*offset;
  TypeOrError r;
  (*offset)++;
  switch(op->opType){
    //n to 0:
    case OP_PRINT:
      r=typeCheckExpression(prog,offset);
      if(r.isError)
        return r.as.error;
      if(r.as.type->typeClass==TYPECLASS_POINTER||r.as.type->typeClass==TYPECLASS_CONST_POINTER)
        return 0;//printing pointer is always possible
      if(r.as.type->typeClass!=TYPECLASS_PRIMITIVE||r.as.type->typeDataAs.primitive==PRIMITIVE_VOID){
        fputs("cannot print values of type ",stderr);
        printTypeName(*r.as.type,stderr);
        fputs("\n",stderr);
        return ERROR_TYPE;
      }
      return 0;//non-void primitive
      break;
    case OP_DECLARE:
    case OP_SET:
      r=typeCheckExpression(prog,offset);
      if(r.isError)
        return r.as.error;
      switch(op->dataAs.idInfo.type){
        case ID_LOCAL_VAR:
        case ID_GLOBAL_VAR:
        case ID_ARGUMENT:
          if(op->dataAs.idInfo.type==ID_ARGUMENT&&op->opType==OP_DECLARE){
            fputs("cannot declare procedure arguments",stderr);
            return ERROR_SYNTAX;
          }
          if(!typeEquals(op->dataType,*r.as.type)){//TODO allow implicit casts
            typeErrorMessage("variable assignment",op->dataType,*r.as.type);
            return ERROR_TYPE;
          }
          return 0;
        case ID_TUPLE_ELEMENT:
          if(op->opType==OP_DECLARE){
            fputs("cannot declare tuple elements",stderr);
            return ERROR_SYNTAX;
          }
          if(r.as.type->typeClass!=TYPECLASS_TUPLE){
            printTypeName(*r.as.type,stderr);
            fputs(" is not a tuple\n",stderr);
            return ERROR_TYPE;
          }
          CompositeType* tuple=r.as.type->typeDataAs.composite;
          if(tuple->typeCount<op->dataAs.idInfo.id){
            fprintf(stderr,"index %"PRIi32" exceeds element count of tuple %"PRIi32"\n",op->dataAs.idInfo.id,tuple->typeCount);
            return ERROR_TYPE;
          }
          r=typeCheckExpression(prog,offset);
          if(r.isError)
            return r.as.error;
          if(!typeEquals(tuple->types[op->dataAs.idInfo.id],*r.as.type)){
            typeErrorMessage("tuple element assignment",tuple->types[op->dataAs.idInfo.id],*r.as.type);
            return ERROR_TYPE;
          }
          return 0;
        case ID_POINTER:
          if(op->opType==OP_DECLARE){
            fputs("cannot declare value at pointer",stderr);
            return ERROR_SYNTAX;
          }
          break;
        case ID_PROCEDURE:
          fputs("procedures cannot be modified",stderr);
          return ERROR_SYNTAX;
        }
        break;
    case OP_RETURN:
      if(op->dataType.typeClass==TYPECLASS_PRIMITIVE&&op->dataType.typeDataAs.primitive==PRIMITIVE_VOID)
        return 0;//no types to check
      r=typeCheckExpression(prog,offset);
      if(r.isError)
        return r.as.error;
      if(op->dataType.typeClass!=TYPECLASS_FLAT_TUPLE&&typeEquals(op->dataType,*r.as.type))
        return 0;
      if(op->dataType.typeClass!=TYPECLASS_TUPLE&&op->dataType.typeClass!=TYPECLASS_FLAT_TUPLE){
        typeErrorMessage("return statement",op->dataType,*r.as.type);
        return ERROR_TYPE;
      }
      //try to auto-wrap tuple data
      op->dataType=(DataType){.typeClass=TYPECLASS_FLAT_TUPLE,.typeDataAs=op->dataType.typeDataAs};//notify compiler
      if(!typeEquals(op->dataType.typeDataAs.composite->types[0],*r.as.type)){
        typeErrorMessage("return statement",op->dataType.typeDataAs.composite->types[0],*r.as.type);
        return ERROR_TYPE;
      }
      for(int32_t i=1;i<op->dataType.typeDataAs.composite->typeCount;i++){
        r=typeCheckExpression(prog,offset);
        if(r.isError)
          return r.as.error;
        if(!typeEquals(op->dataType.typeDataAs.composite->types[i],*r.as.type)){
          typeErrorMessage("return statement",op->dataType.typeDataAs.composite->types[i],*r.as.type);
          return ERROR_TYPE;
        }
      }
      return 0;
    case OP_CODE_BLOCK:
      switch(op->dataAs.block){
        case BLOCK_IF:
        case BLOCK_WHILE:
        case BLOCK_WHILE_END:
        case BLOCK_ELIF://consume one bool
          r=typeCheckExpression(prog,offset);
          if(r.isError)
            return r.as.error;
          if(r.as.type->typeClass!=TYPECLASS_PRIMITIVE||r.as.type->typeDataAs.primitive!=PRIMITIVE_BOOL){
            typeErrorMessage("condition parameter",primitiveType(PRIMITIVE_BOOL),*r.as.type);
            return ERROR_TYPE;
          }
          return 0;
        case BLOCK_START:
        case BLOCK_ELSE:
        case BLOCK_DO:
        case BLOCK_END:
          return 0;//no types to check
      }
      break;
    case OP_CALL:
      r=typeCheckCall(op->dataType,prog,offset);
      if(r.isError)
        return r.as.error;
      if(r.as.type->typeClass!=TYPECLASS_PRIMITIVE||r.as.type->typeDataAs.primitive!=PRIMITIVE_VOID){
        fputs("return value of procedure is ignored",stderr);
      }
      return 0;
    case OP_DECLARE_PROCEDURE:
    case ENTRY_POINT:
      return 0;//no types to check
    //0 to 1:
    case OP_CONSTANT:
    case OP_STRING_CONST:
    case OP_GET:
    //n to 1:
    case OP_UNARY_OPERATOR:
    case OP_BINARY_OPERATOR:
      fprintf(stderr,"unexpected operation %i\n",op->opType);
      return ERROR_SYNTAX;
  }
  fprintf(stderr,"type-checking operation %i is not implemented\n",op->opType);
  return ERROR_UNIMPLEMENTED;
}
int typeCheckProgram(Program prog){
  size_t offset=0;
  int err;
  while(offset<prog.opCount){
    err=typeCheckStatement(prog,&offset);
    if(err!=0)
      return err;
  }
  return 0;
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
		Program p=compileToOps(code,codeSize);
		if(p.ops==NULL)
		  return ERROR_SYNTAX;
	  printf("found %zu operations\n",p.opCount);
		//2. type-check operations
	  int err=typeCheckProgram(p);
	  if(err){
      fprintf(stderr,"error %i\n",err);
      return err;
    }
	  puts("type-checked program");
		//3. compile operations to C
    FILE* out=fopen("./out.c","w");
    err=compileToC(out,p.ops,p.opCount,p.hasEntryPoint);
    if(err){
      fprintf(stderr,"error %i\n",err);
      goto RETURN;
    }
	  puts("compiled program");
  RETURN:
    fclose(out);
    return err;
	}else{
		fprintf(stderr,"IO Error while opening File: %s\n",srcFile);
		return ERROR_IO;
	}
}
