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
  OP_PRINT,
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
  
  //TODO implement procedures
  BLOCK_PROCEDURE, // id,inTypes,outTypes
  OP_RETURN,       // types     
}OpType;
//types
typedef enum{
  TYPECLASS_UNDEFINED,
  TYPECLASS_PRIMITIVE,
  TYPECLASS_POINTER,//TODO typed pointers, structs, unions, procedure pointers
  TYPECLASS_STRUCT,
  TYPECLASS_UNION,
  TYPECLASS_PROCEDURE,
}TypeClass;
typedef enum{
  PRIMITIVE_I8,
  PRIMITIVE_I32,
  PRIMITIVE_I64,
  PRIMITIVE_FLOAT,
}PrimitiveType;
typedef struct DataType{
  TypeClass typeClass;
  union{
    PrimitiveType primitive;
    struct DataType* type;
    //TODO Array{DataType} structure/union
    //TODO 2x Array{DataType} procedure
  }typeDataAs;
}DataType;
DataType TYPE_UNDEFINED=(DataType){.typeClass=TYPECLASS_UNDEFINED,.typeDataAs={0}};
#define MAX_TYPES 1024
size_t typeCount=0;
DataType typeData[MAX_TYPES];
bool typeEquals(DataType a,DataType b){
  if(a.typeClass!=b.typeClass)
    return false;
  if(a.typeClass==TYPECLASS_UNDEFINED)
    return true;//all undefined types are equal
  if(a.typeClass==TYPECLASS_PRIMITIVE)
    return a.typeDataAs.primitive==b.typeDataAs.primitive;
  if(a.typeClass==TYPECLASS_POINTER)
    return typeEquals(*a.typeDataAs.type,*b.typeDataAs.type);
  //TODO compare other types
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
  return (DataType){.typeClass=TYPECLASS_POINTER,.typeDataAs={.type=typeData+typeCount++}};
}

const char* primitiveName(PrimitiveType t){
  switch(t){
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
    case TYPECLASS_STRUCT://TODO printing of composite types
      return fputs("struct ?",file);
    case TYPECLASS_UNION:
      return fputs("union ?",file);
    case TYPECLASS_PROCEDURE:
      return fputs("procedure ?",file);
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


SizeOrError compileOp(FILE* target,const Operation* op){
  SizeOrError r;
  size_t size=1;
  switch(op->opType){
    case OP_PRINT:
      fputs("printf(\"%",target);
      switch(op->dataType.typeClass){
        case TYPECLASS_PRIMITIVE:
          switch(op->dataType.typeDataAs.primitive){
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
      r=compileOp(target,op+1);
      if(r.isError)
        return r;
      fputs(");\n",target);
      size+=r.as.size;
      break;
    case OP_CONSTANT:
      if(op->dataType.typeClass!=TYPECLASS_PRIMITIVE){
          fputs("constants of non-primitive type ",stderr);
          printTypeName(op->dataType,stderr);
          fputs(" are not supported\n",stderr);
          return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
      }
      switch(op->dataType.typeDataAs.primitive){
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
      printTypeName(op->dataType,target);
      fprintf(target," local%" PRIu64 " = ",op->dataAs.id);
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
  fprintf(stderr,"unkown type name: %.*s \n",(int)name.length,name.chars);
  return TYPE_UNDEFINED;
}
SizeOrError readOperation(Operation* op,char** code,size_t* codeSize){
  String word=nextWord(code,codeSize);
  if(word.length==0)
    return (SizeOrError){.isError=false,.as={.size=0}};
  if(wordEquals(&word,"PRINT")){
    DataType type=readType(code,codeSize);
    if(typeEquals(type,TYPE_UNDEFINED))
      return (SizeOrError){.isError=true,.as={.error=ERROR_TYPE}};
    (*op)=(Operation){.opType=OP_PRINT,.dataType=type,.dataAs={.id=0}};
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
    if(typeEquals(type,TYPE_UNDEFINED))
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
