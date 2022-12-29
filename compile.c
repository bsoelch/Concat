#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#define ERROR_UNIMPLEMENTED -1
#define ERROR_TYPE 1

typedef struct{
  bool isError;
  union{
    size_t size;
    int    error;
  }as;
}SizeOrError;

typedef enum{
  OP_CONSTANT,
  
  OP_LOCAL_READ,
  OP_LOCAL_DECLARE,
  OP_LOCAL_ASSIGN,
  
  OP_BINARY_ARITHMETIC, 
  OP_UNARY_PREFIX,  
  
  //TODO control flow 
  //OP_IF_START    if(EXPR){
  //OP_ELIF        }else if(EXPR){
  //OP_ELSE        }else{
  //OP_WHILE_START while(EXPR){
  //OP_DO_START    do{
  //OP_BLOCK_END   }
}OpType;
typedef enum{
  TYPE_UNDEFINED,
  TYPE_I8,
  TYPE_I32,
  TYPE_I64,
  TYPE_FLOAT,
  TYPE_PTR,//TODO typed pointers
}DataType;
typedef enum{
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  MOD,
  AND,
  OR,
  XOR,
}BinaryOperator;
typedef enum{
  NEGATE,
  INCREMENT,
  DECREMENT,
}UnaryOperator;
//TODO? post-fix unary

typedef struct{
  OpType opType;
  DataType dataType;
  union{
    uint64_t id;
    BinaryOperator binOp;
    UnaryOperator unOp;
  }dataAs;
}Operation;

const char* typeName(const DataType t){
  switch(t){
    case TYPE_UNDEFINED:
      return "void";
    case TYPE_I8:
      return "int8_t";
    case TYPE_I32:
      return "int32_t";
    case TYPE_I64:
      return "int64_t";
    case TYPE_FLOAT:
      return "double";
    case TYPE_PTR:
      return "void*";
  }
  fprintf(stderr,"unexpected type %i",t);
  return "";
}

SizeOrError compileOp(FILE* target,const Operation* op){
  SizeOrError r;
  size_t size=1;
  switch(op->opType){
    case OP_CONSTANT:
      switch(op->dataType){
        case TYPE_I8:
        case TYPE_I32:
        case TYPE_I64:
          fprintf(target,"((%s)%" PRIu64 ")",typeName(op->dataType),op->dataAs.id);
          break;
        default:
          fprintf(stderr,"%s constants are (currently) not supported",typeName(op->dataType));
          return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
      }
      break;
    case OP_LOCAL_READ:
      fprintf(target,"local%" PRIu64,op->dataAs.id);
      break;
    case OP_LOCAL_ASSIGN:
      fprintf(target,"local%" PRIu64" = ",op->dataAs.id);
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      fputs(";\n",target);
      size+=r.as.size;
      break;
    case OP_LOCAL_DECLARE:
      fprintf(target,"%s local%" PRIu64 " = ",typeName(op->dataType),op->dataAs.id);
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      fputs(";\n",target);
      size+=r.as.size;
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
      }
      r=compileOp(target,op+size);
      if(r.isError)
        return r;
      size+=r.as.size;
      fputs(")",target);
      break;
    case OP_BINARY_ARITHMETIC:
      fputs("(",target);
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      size+=r.as.size;
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
      }
      r=compileOp(target,op+size);
      if(r.isError)
        return r;
      size+=r.as.size;
      fputs(")",target);
      break;
    default:
      fprintf(stderr,"operation %i is not implemented\n",op->opType);
      return (SizeOrError){.isError=true,.as={.error=ERROR_UNIMPLEMENTED}};
  }
  return (SizeOrError){.isError=false,.as={.size=size}};
}
int compile(FILE* target,const Operation* ops,size_t opCount){
  fputs("#include <stdlib.h>\n",target);
  fputs("#include <stdio.h>\n",target);
  fputs("#include <inttypes.h>\n",target);
  fputs("#include <stdbool.h>\n",target);
  fputs("int main(void){\n",target);
  SizeOrError r;
  for(size_t p=0;p<opCount;){
    r=compileOp(target,ops+p);
    if(r.isError)
      return r.as.error;
    p+=r.as.size;
  }
  fputs("return 0;\n",target);
  fputs("}\n",target);
  return 0;
}

#define OPS_LENGTH 13
Operation ops[OPS_LENGTH]={
  {.opType=OP_LOCAL_DECLARE,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_BINARY_ARITHMETIC,.dataType=TYPE_I64,.dataAs={.binOp=ADD}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.id=1}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.id=1}},
  {.opType=OP_LOCAL_DECLARE,.dataType=TYPE_I64,.dataAs={.id=1}},
  {.opType=OP_BINARY_ARITHMETIC,.dataType=TYPE_I64,.dataAs={.binOp=MULTIPLY}},
  {.opType=OP_BINARY_ARITHMETIC,.dataType=TYPE_I64,.dataAs={.binOp=SUBTRACT}},
  {.opType=OP_LOCAL_READ,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.id=2}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.id=3}},
  {.opType=OP_LOCAL_ASSIGN,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_UNARY_PREFIX,.dataType=TYPE_I64,.dataAs={.unOp=NEGATE}},
  {.opType=OP_LOCAL_READ,.dataType=TYPE_I64,.dataAs={.id=1}},
};

//TODO assembly-like language for operations

int main(void){
  FILE* out=fopen("./out.c","w");
  int err=compile(out,ops,OPS_LENGTH);
  if(err)
    fprintf(stderr,"error %i\n",err);
  fclose(out);
  return err;
}
