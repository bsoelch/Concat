#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>


#define PRI_STR ".*s"
#define PRI_STR_ARGS(s) (int)(s).length,(s).chars 

typedef struct{
  const char* chars;
  size_t length;
}String;
typedef struct{
  String head;
  String tail;
}SlicedString;

extern String EMPTY_STRING;

String cstrToStr(const char* chars);
String newString(const char* chars,size_t len);

bool wordEquals(const String* word,const char* string);
int stringCompare(const String a,const String b);
int32_t stringHash(const String s);

char    charAt(const String s,int64_t index);
int64_t indexOfChar(const String str,char c);
int64_t indexOfString(const String base,const String child);
int64_t indexOfStringArray(const String* base,size_t baseLen,const String* child,size_t childLen);

String sliceStart(String str,size_t k);
SlicedString sliceAtChar(String base,char chr);
