#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define ERROR_MEMORY -1
#define ERROR_IO -2
#define ERROR_UNIMPLEMENTED -3

#define ERROR_TYPE 1
#define ERROR_SYNTAX 2
#define ERROR_PARSE_INT 3
#define ERROR_REDECLARATION 4

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
int stringCompare(String a,String b){
  return strncmp(a.chars,b.chars,a.length<b.length?a.length:b.length);
}
int32_t stringHash(String s){
  int32_t hash=0;
  for(size_t i=0;i<s.length;i++){
    hash=31*hash+s.chars[i];
  }
  return hash;
}

typedef enum{
  OP_PRINT,
  OP_CONSTANT,
  
  OP_DECLARE,
  OP_GET,
  OP_SET,
  //TODO tuple operations, access to function parameters
  //OP_SET_AT *(EXPR1)=EXPR2
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
  OP_UNARY_PREFIX,  
  //TODO? post-fix unary
  
  BLOCK_START,     // {
  BLOCK_IF,        // if( EXPR ){
  BLOCK_ELIF,      // } else if(EXPR){
  BLOCK_ELSE,      // }else{
  BLOCK_WHILE,     // while( EXPR ){
  BLOCK_DO,        // do{
  BLOCK_WHILE_END, // }while( EXPR );
  BLOCK_END,       // }
  
  BLOCK_PROCEDURE, 
  OP_RETURN,       
  OP_CALL,         // procType procId
  //TODO OP_CALL_EXPR // CALL_EXPR EXPR ARGS
  ENTRY_POINT,     //entry point of the program, starts the main code section, section will close at the matching BLOCK_END 
}OpType;
//types
typedef enum{
  TYPECLASS_UNDEFINED,
  TYPECLASS_PRIMITIVE,
  TYPECLASS_POINTER,
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
  if(a.typeClass==TYPECLASS_POINTER)
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

const char* primitiveName(PrimitiveType t){
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
int printTypeName(DataType type,FILE* file){
  int i,j;
  switch(type.typeClass){
    case TYPECLASS_UNDEFINED:
      return fputs("void",file);
    case TYPECLASS_PRIMITIVE:
      return fprintf(file,"%s",primitiveName(type.typeDataAs.primitive));
    case TYPECLASS_POINTER:
      i=printTypeName(*type.typeDataAs.type,file);
      if(i<0)
        return i;
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
typedef enum{
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
  ID_TUPLE_ELEMENT
  //TODO GET/SET for union values
}IdentiferType;
typedef struct{
  int32_t id;
  IdentiferType type;
}IdentiferInfo;

typedef struct{
  OpType opType;
  DataType dataType;
  union{
    int64_t i64;
    BinaryOperator binOp;
    UnaryOperator unOp;
    IdentiferInfo idInfo;
  }dataAs;
}Operation;

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
          fprintf(target,"((%s)%" PRIu64 ")",primitiveName(op->dataType.typeDataAs.primitive),op->dataAs.i64);
          break;
        default:
          fprintf(stderr,"%s constants are (currently) not supported",primitiveName(op->dataType.typeDataAs.primitive));
          return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
      }
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
      }
      COMPILE_OP_RETURN_ERROR(target,op);
      fputs(";\n",target);
      break;
    case OP_DECLARE:
      printTypeName(op->dataType,target);
      switch(op->dataAs.idInfo.type){
        case ID_LOCAL_VAR:
          fprintf(target," local%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_GLOBAL_VAR:
          fprintf(target," global%" PRIi32 " = ",op->dataAs.idInfo.id);
          break;
        case ID_PROCEDURE:
          fputs("use BLOCK_PROCEDURE to declare a procedure ",stderr);
          return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
        case ID_ARGUMENT:
          fputs("cannot declare arguments",stderr);
          return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
        case ID_TUPLE_ELEMENT:
          fputs("cannot declare tuple elements",stderr);
          return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
      }
      COMPILE_OP_RETURN_ERROR(target,op);
      fputs(";\n",target);
      break;
    case OP_UNARY_PREFIX:
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
      switch(op->dataAs.binOp){//TODO? use array instead
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
    case BLOCK_PROCEDURE:{
      if(op->dataType.typeClass!=TYPECLASS_PROCEDURE)
        return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
      printTypeName(*op->dataType.typeDataAs.procedure->outType,target);
      fprintf(target," procedure%" PRIi32" (",op->dataAs.idInfo.id);
      DataType* inType=op->dataType.typeDataAs.procedure->inType;
      if(inType->typeClass==TYPECLASS_FLAT_TUPLE){
        CompositeType* inTypes=inType->typeDataAs.composite;
        for(int32_t e=0;e<inTypes->typeCount;e++){
          if(e>0)
            fputs(", ",target);
          printTypeName(inTypes->types[e],target);
          fprintf(target," arg%"PRIi32,e);
        } 
      }else if(inType->typeClass==TYPECLASS_PRIMITIVE&&inType->typeDataAs.primitive==PRIMITIVE_VOID){
        fputs("void",target);
      }else{
        printTypeName(*inType,target);
        fputs(" arg0",target);
      }         
      fputs("){\n",target);
    }break;
    case OP_RETURN:
      fputs("return ",target);
      if(op->dataType.typeClass!=TYPECLASS_FLAT_TUPLE &&op->dataType.typeClass!=TYPECLASS_TUPLE){//XXX remove check for tuple once compiler can detect types
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
    printTypeName(*procTypes[i].outType,target);
    fprintf(target," (*procPtr%zu) (",i);
    if(procTypes[i].inType->typeClass==TYPECLASS_FLAT_TUPLE){//auto-unwrap procedure arguments
      CompositeType* inTypes=procTypes[i].inType->typeDataAs.composite;
      for(int32_t j=0;j<inTypes->typeCount;j++){
        if(j>0)
          fputs(",",target);
        printTypeName(inTypes->types[j],target);
      }
    }else{
      printTypeName(*procTypes[i].inType,target);
    }
    fputs(");\n",target);
  }
  //initialize composite types
  for(int32_t i=0;i<compositeCount;i++){
    if(compositeTypes[i].flags&FLAG_IS_TUPLE){
      fprintf(target,"struct tuple%"PRIi32"Impl{\n",i);
      for(int16_t e=0;e<compositeTypes[i].typeCount;e++){
        printTypeName(compositeTypes[i].types[e],target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("};\n",target);
    }//no else
    if(compositeTypes[i].flags&FLAG_IS_UNION){
      fprintf(target,"struct union%"PRIi32"Impl{\n"
                     "%s state;\n"
                     "union{\n",i,primitiveName(PRIMITIVE_I32));
      for(int16_t e=0;e<compositeTypes[i].typeCount;e++){
        printTypeName(compositeTypes[i].types[e],target);
        fprintf(target," e%"PRIi16";\n",e);
      }
      fputs("}value;\n};\n",target);
    }
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
ScopeNode* allocScopeNode(){
  if(scopeNodeCount+1>=SCOPE_NODE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of variables %i",SCOPE_NODE_CAP);
    return NULL;
  }
  return scopeNodeBuffer+(scopeNodeCount++);
}
Scope* openScope(){
  if(scopeCount+1>=SCOPE_CAP){
    fprintf(stderr,"exceeded maximum allowed number of nested scopes %i",SCOPE_CAP);
    return NULL;
  }
  scopeBuffer[scopeCount].nodes=malloc(SCOPE_MAP_CAP*sizeof(ScopeNode*));
  scopeBuffer[scopeCount].nodeBufferOffset=scopeNodeCount;
  scopeBuffer[scopeCount].parent=scopeCount>0?scopeBuffer+(scopeCount-1):NULL;
  return scopeBuffer+(scopeCount++);
}
bool closeScope(){
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
bool wordEquals(const String* word,const char* string){
  return strncasecmp(word->chars,string,word->length)==0;
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
IntOrError parseInt(String number){
  int base=10;
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
  if(number.length>i+1&&number.chars[i]=='0'){
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
    value*=base;
    digit=toDigit(number.chars[i]);
    if(digit<0)
      return (IntOrError){.isError=true,.as={.error=ERROR_PARSE_INT}};
    value+=digit;
  }
  return (IntOrError){.isError=false,.as={.i64=negate?-value:value}};
}

String nextWord(char** code,size_t* codeSize){
  skipWhitespaces(code,codeSize);
  size_t wordLength=0;
  while(wordLength<*codeSize&&(*code)[wordLength]!=0&&!isspace((*code)[wordLength])){
    wordLength++;
  }
  if(wordLength<*codeSize)
    (*code)[wordLength]=0;//zero terminate command
  char* wordChars=*code;
  //move code-pointer to position after word
  (*code)+=wordLength+(wordLength<*codeSize?1:0);//do not exceed code size
  (*codeSize)-=wordLength+(wordLength<*codeSize?1:0);
  return (String){.chars=wordChars,.length=wordLength};
}

DataType readCompositeType(TypeClass typeClass,char** code,size_t* codeSize);//predeclare
DataType readType(char** code,size_t* codeSize){
  String name=nextWord(code,codeSize);
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
  fprintf(stderr,"unkown type name: %.*s \n",(int)name.length,name.chars);
  return TYPE_UNDEFINED;
}
DataType readCompositeType(TypeClass typeClass,char** code,size_t* codeSize){
  IntOrError count=parseInt(nextWord(code,codeSize));
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
  String word=nextWord(code,codeSize);
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
    
    String constValue=nextWord(code,codeSize);
    switch(type.typeDataAs.primitive){
      case PRIMITIVE_BOOL:
      case PRIMITIVE_I8:
      case PRIMITIVE_I32:
      case PRIMITIVE_I64:
        IntOrError val=parseInt(constValue);
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
    String varName=nextWord(code,codeSize);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    ScopeNode* id;
    int r=getIdentifier(varName,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error=r}};
    (*op)=(Operation){.opType=OP_GET,.dataType=id->type,.dataAs={.idInfo={.type=id->idType,.id=id->id}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET")){
    String varName=nextWord(code,codeSize);
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
    String varName=nextWord(code,codeSize);
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
            
      (*op)=(Operation){.opType=BLOCK_PROCEDURE,.dataType=type,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    }else{
      (*op)=(Operation){.opType=OP_DECLARE,.dataType=type,.dataAs={.idInfo={.type=idType,.id=id->id}}};
    }
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"GET_ELEMENT")){
    IntOrError index=parseInt(nextWord(code,codeSize));
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_GET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET_ELEMENT")){
    IntOrError index=parseInt(nextWord(code,codeSize));
    if(index.isError)
      return (SizeOrError){.isError=true,.as={.error=index.as.error}};
    (*op)=(Operation){.opType=OP_SET,.dataType=TYPE_UNDEFINED,.dataAs={.idInfo={.type=ID_TUPLE_ELEMENT,.id=index.as.i64}}};
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
  }else if(wordEquals(&word,"XOR")||wordEquals(&word,"^")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=XOR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"AND2")||wordEquals(&word,"&&")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=FAST_AND}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"OR2")||wordEquals(&word,"||")){
    (*op)=(Operation){.opType=OP_BINARY_OPERATOR,.dataType=TYPE_UNDEFINED,.dataAs={.binOp=FAST_OR}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"NEG")||wordEquals(&word,"NEGATE")){
    (*op)=(Operation){.opType=OP_UNARY_PREFIX,.dataType=TYPE_UNDEFINED,.dataAs={.unOp=NEGATE}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"IF")){
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    (*op)=(Operation){.opType=BLOCK_IF,.dataType=TYPE_UNDEFINED,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ELIF")){
    closeScope();
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    
    (*op)=(Operation){.opType=BLOCK_ELIF,.dataType=TYPE_UNDEFINED,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"WHILE")){
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    state->scopeLevel++;
    
    (*op)=(Operation){.opType=BLOCK_WHILE,.dataType=TYPE_UNDEFINED,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ELSE")){
    closeScope();
    Scope* newScope=openScope();
    if(newScope==NULL)
      return (SizeOrError){.isError=true,.as={.error=ERROR_MEMORY}};
    state->currentScope=newScope;
    
    (*op)=(Operation){.opType=BLOCK_ELSE,.dataType=TYPE_UNDEFINED,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"END")){
    closeScope();
    state->scopeLevel--;
    if(state->scopeLevel<state->procScope){//exited procedure
      state->currentProcId=-1;
      state->procScope=-1;
    }
    
    (*op)=(Operation){.opType=BLOCK_END,.dataType=TYPE_UNDEFINED,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"RETURN")){
    if(state->currentProcId<0){
      fputs("unexpected return statement\n",stderr);
    }
    
    
    (*op)=(Operation){.opType=OP_RETURN,.dataType=*procTypes[state->currentProcId].outType,.dataAs={0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"CALL")){
    String procName=nextWord(code,codeSize);
    if(procName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    ScopeNode* id;
    int r=getIdentifier(procName,&id);
    if(r!=0)
      return (SizeOrError){.isError=true,.as={.error=r}};
    //type has to be procedure or pointer to procedure
    if(id->type.typeClass!=TYPECLASS_PROCEDURE&&(id->type.typeClass!=TYPECLASS_POINTER||id->type.typeDataAs.type->typeClass!=TYPECLASS_PROCEDURE))
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
    fprintf(stderr,"unknown command %.*s\n",(int)word.length,word.chars);
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
    if(r.isError)
      return (Program){.ops=NULL,.opCount=0};//TODO better error value
    opCount+=r.as.size;
    if(opCount>=opsCap-5){
      return (Program){.ops=NULL,.opCount=0};//TODO ensure there is enough capacity
    }
  }
  return (Program){.ops=compileOps,.opCount=opCount,.globalScope=scopeBuffer,.hasEntryPoint=state.hasEntryPoint};
}
//TODO typecheck program

/* Copied from StackOverflow
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
    printf("usage: inputFile");
    return 0;
  }
  srcFile=*(argv++);
  FILE *file = fopen(srcFile, "r");
	if(file!=NULL){
		long int size=fsize(file);
		if(size<0){//XXX recover form Undetected fileSize (if seek worked)
			fputs("IO Error while detecting file-size\n",stderr);
			return ERROR_IO;
		}else{
			code = malloc((size+1)*sizeof(char));
			if(code==NULL){
				printf("Memory Error\n");
				return ERROR_MEMORY;
			}
			codeSize=fread(code,sizeof(char),size,file);//XXX perform multiple reads if necessary
			if(codeSize<0){
				printf("IO Error while reading file\n");
				free(code);
				return ERROR_IO;
			}
			fclose(file);//file no longer needed
			memset(code+codeSize,0,(size+1-codeSize)*sizeof(char));//fill remaining path of file with 0
		}
		Program p=compileToOps(code,codeSize);
		free(code);//code no longer needed
		//compile program to C
		if(p.ops==NULL)
		  return ERROR_SYNTAX;
	  printf("compile %zu operations\n",p.opCount);
    FILE* out=fopen("./out.c","w");
    int err=compileToC(out,p.ops,p.opCount,p.hasEntryPoint);
    if(err)
      fprintf(stderr,"error %i\n",err);
    fclose(out);
    return err;
		return ERROR_UNIMPLEMENTED;
	}else{
		fprintf(stderr,"IO Error while opening File: %s\n",srcFile);
		return ERROR_IO;
	}
}
