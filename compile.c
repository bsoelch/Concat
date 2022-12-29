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

typedef enum{
  OP_CONSTANT,
  
  OP_LOCAL_READ,
  OP_LOCAL_DECLARE,
  OP_LOCAL_ASSIGN,
  
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
}OpType;
typedef enum{
  TYPE_UNDEFINED=-1,
  TYPE_I8=0,
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
  FAST_AND,
  FAST_OR,
}BinaryOperator;
typedef enum{
  NEGATE,
  INCREMENT,
  DECREMENT,
}UnaryOperator;

typedef struct{
  OpType opType;
  DataType dataType;
  union{
    uint64_t id;
    int64_t i64;
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
          fprintf(target,"((%s)%" PRIu64 ")",typeName(op->dataType),op->dataAs.i64);
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
    case OP_BINARY_OPERATOR:
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
        case FAST_AND:
          fputs("&&",target);
          break;
        case FAST_OR:
          fputs("||",target);
          break;
      }
      r=compileOp(target,op+size);
      if(r.isError)
        return r;
      size+=r.as.size;
      fputs(")",target);
      break;
    case BLOCK_START:
      fputs("{\n",target);
      break;
    case BLOCK_IF:
      fputs("if(",target);
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      fputs("){\n",target);
      size+=r.as.size;
      break;
    case BLOCK_ELIF:
      fputs("}else if(",target);
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      fputs("){\n",target);
      size+=r.as.size;
      break;
    case BLOCK_WHILE:
      fputs("while(",target);
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      fputs("){\n",target);
      size+=r.as.size;
      break;
    case BLOCK_DO:
      fputs("do{\n",target);
      break;
    case BLOCK_ELSE:
      fputs("}else{\n",target);
      break;
    case BLOCK_WHILE_END:
      fputs("}while(",target);
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      fputs(");\n",target);
      size+=r.as.size;
      break;
    case BLOCK_END:
      fputs("}\n",target);
      break;
    default:
      fprintf(stderr,"operation %i is not implemented\n",op->opType);
      return (SizeOrError){.isError=true,.as={.error=ERROR_UNIMPLEMENTED}};
  }
  return (SizeOrError){.isError=false,.as={.size=size}};
}
int compileToC(FILE* target,const Operation* ops,size_t opCount){
  fputs("#include <stdlib.h>\n",target);
  fputs("#include <stdio.h>\n",target);
  fputs("#include <inttypes.h>\n",target);
  fputs("#include <string.h>\n",target);
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

#define OPS_LENGTH 23
Operation ops[OPS_LENGTH]={
  {.opType=OP_LOCAL_DECLARE,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_BINARY_OPERATOR,.dataType=TYPE_I64,.dataAs={.binOp=ADD}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.i64=1}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.i64=1}},
  {.opType=OP_LOCAL_DECLARE,.dataType=TYPE_I64,.dataAs={.id=1}},
  {.opType=OP_BINARY_OPERATOR,.dataType=TYPE_I64,.dataAs={.binOp=MULTIPLY}},
  {.opType=OP_BINARY_OPERATOR,.dataType=TYPE_I64,.dataAs={.binOp=SUBTRACT}},
  {.opType=OP_LOCAL_READ,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.i64=2}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.i64=3}},
  {.opType=OP_LOCAL_ASSIGN,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_UNARY_PREFIX,.dataType=TYPE_I64,.dataAs={.unOp=NEGATE}},
  {.opType=OP_LOCAL_READ,.dataType=TYPE_I64,.dataAs={.id=1}},
  {.opType=BLOCK_IF,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_LOCAL_READ,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=BLOCK_ELIF,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_BINARY_OPERATOR,.dataType=TYPE_I64,.dataAs={.binOp=FAST_AND}},
  {.opType=OP_LOCAL_READ,.dataType=TYPE_I64,.dataAs={.id=1}},
  {.opType=OP_BINARY_OPERATOR,.dataType=TYPE_I64,.dataAs={.binOp=XOR}},
  {.opType=OP_LOCAL_READ,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=OP_CONSTANT,.dataType=TYPE_I64,.dataAs={.i64=42}},
  {.opType=BLOCK_ELSE,.dataType=TYPE_I64,.dataAs={.id=0}},
  {.opType=BLOCK_END,.dataType=TYPE_I64,.dataAs={.id=0}},
};

typedef struct{
  Operation* ops;
  size_t opCount;
  //TODO variable info
}Program;

void skipWhitespaces(char** code,size_t* codeSize){
  while(*codeSize>0&&((**code)==0||isspace(**code))){
    (*codeSize)--;
    (*code)++;
  }
}
typedef struct{
  char* chars;
  size_t length;
}String;
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
  (*code)+=wordLength+(wordLength<*codeSize?1:0);//do not exceed codesize
  (*codeSize)-=wordLength+(wordLength<*codeSize?1:0);
  return (String){.chars=wordChars,.length=wordLength};
}

DataType readType(char** code,size_t* codeSize){
  String name=nextWord(code,codeSize);
  if(name.length==0)
    return TYPE_UNDEFINED;
  if(wordEquals(&name,"I8")||wordEquals(&name,"CHAR"))
    return TYPE_I8;
  if(wordEquals(&name,"I32"))
    return TYPE_I32;
  if(wordEquals(&name,"I64"))
    return TYPE_I64;
  if(wordEquals(&name,"FLOAT"))
    return TYPE_FLOAT;
  if(wordEquals(&name,"PTR"))//TODO typed pointers
    return TYPE_PTR;
  fprintf(stderr,"unkown type: %.*s",(int)name.length,name.chars);
  return TYPE_UNDEFINED;
}
SizeOrError readOperation(Operation* op,char** code,size_t* codeSize){
  String word=nextWord(code,codeSize);
  if(word.length==0)
    return (SizeOrError){.isError=false,.as={.size=0}};
  if(wordEquals(&word,"CONST")){
    DataType type=readType(code,codeSize);
    if(type==TYPE_UNDEFINED)
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    String constValue=nextWord(code,codeSize);
    switch(type){
      case TYPE_I8:
      case TYPE_I32:
      case TYPE_I64:
        IntOrError val=parseInt(constValue);
        if(val.isError){
          printf("cannot parse \"%.*s\" to an integer \n",(int)constValue.length,constValue.chars);
          return (SizeOrError){.isError=true,.as={.error=val.as.error}};
        }
        (*op)=(Operation){.opType=OP_CONSTANT,.dataType=type,.dataAs={.i64=val.as.i64}};
        return (SizeOrError){.isError=false,.as={.size=1}};
      default:
        printf("constants of type %s (currently) not supported \n",typeName(type));
    }
    return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
  }else if(wordEquals(&word,"GET")){
    String varName=nextWord(code,codeSize);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    //TODO store variables by name
    (*op)=(Operation){.opType=OP_LOCAL_READ,.dataType=TYPE_UNDEFINED,.dataAs={.id=(uint64_t)varName.chars[0]}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"SET")){
    String varName=nextWord(code,codeSize);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    //TODO store variables by name
    (*op)=(Operation){.opType=OP_LOCAL_ASSIGN,.dataType=TYPE_UNDEFINED,.dataAs={.id=(uint64_t)varName.chars[0]}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"DECLARE")){
    DataType type=readType(code,codeSize);
    if(type==TYPE_UNDEFINED)
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    String varName=nextWord(code,codeSize);
    if(varName.length==0)
      return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
    //TODO distinguish local and global declarations
    //TODO store variables by name
    (*op)=(Operation){.opType=OP_LOCAL_DECLARE,.dataType=type,.dataAs={.id=(uint64_t)varName.chars[0]}};
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
    (*op)=(Operation){.opType=BLOCK_IF,.dataType=TYPE_UNDEFINED,.dataAs={.id=0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ELIF")){
    (*op)=(Operation){.opType=BLOCK_ELIF,.dataType=TYPE_UNDEFINED,.dataAs={.id=0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"WHILE")){
    (*op)=(Operation){.opType=BLOCK_WHILE,.dataType=TYPE_UNDEFINED,.dataAs={.id=0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"ELSE")){
    (*op)=(Operation){.opType=BLOCK_ELSE,.dataType=TYPE_UNDEFINED,.dataAs={.id=0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else if(wordEquals(&word,"END")){
    (*op)=(Operation){.opType=BLOCK_END,.dataType=TYPE_UNDEFINED,.dataAs={.id=0}};
    return (SizeOrError){.isError=false,.as={.size=1}};
  }else{
    printf("unknown command %.*s\n",(int)word.length,word.chars);
    return (SizeOrError){.isError=true,.as={.error=ERROR_SYNTAX}};
  }
  return (SizeOrError){.isError=false,.as={.size=0}};
}
Program compileToOps(char* code,size_t codeSize){
  size_t opCount=0;
  size_t opsCap=100;
  SizeOrError r;
  Operation* compileOps=malloc(opsCap*sizeof(Operation));
  while(codeSize>0){
    r=readOperation(compileOps+opCount,&code,&codeSize);
    if(r.isError)
      return (Program){.ops=NULL,.opCount=0};//TODO better error value
    opCount+=r.as.size;
  }
  return (Program){.ops=compileOps,.opCount=opCount};
}

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
    FILE* out=fopen("./out.c","w");
    int err=compileToC(out,ops,OPS_LENGTH);
    if(err)
      fprintf(stderr,"error %i\n",err);
    fclose(out);
    return err;
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
    FILE* out=fopen("./out.c","w");
    int err=compileToC(out,p.ops,p.opCount);
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
