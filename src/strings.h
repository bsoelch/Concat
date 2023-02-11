#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>


#define PRI_STR ".*s"
#define PRI_STR_ARGS(s) (int)(s).length,(s).chars 

typedef struct{
  char const* chars;
  size_t length;
}String;
typedef struct{
  String head;
  String tail;
}SlicedString;

extern String EMPTY_STRING;

String cstrToStr(char const* chars);
String newString(char const* chars,size_t len);

bool wordEquals(String const* word,char const* string);
int stringCompare(const String a,const String b);
int32_t stringHash(const String s);

bool    containsChar(const String str,char c);
bool    containsString(const String base,const String child);

char    charAt(const String s,int64_t index);
int64_t indexOfChar(const String str,char c);
int64_t lastIndexOfChar(const String str,char c);
int64_t indexOfString(const String base,const String child);
int64_t indexOfStringArray(String const* base,size_t baseLen,String const* child,size_t childLen);

String sliceStart(String str,size_t k);
String sliceEnd(String str,size_t k);
SlicedString sliceAtChar(String base,char chr);
