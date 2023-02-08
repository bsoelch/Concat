#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct{
  char* chars;
  size_t length;
}String;
typedef struct{
  String head;
  String tail;
}SlicedString;

bool wordEquals(const String* word,const char* string);
int stringCompare(const String a,const String b);
int32_t stringHash(const String s);
SlicedString sliceString(String base,char chr);
int64_t indexOfString(const String base,const String child);
int64_t indexOfStringArray(const String* base,size_t baseLen,const String* child,size_t childLen);
