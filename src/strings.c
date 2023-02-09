#include <string.h>
#include "strings.h"

//printf modifer
String EMPTY_STRING={.chars="",.length=0};

String cstrToStr(const char* chars){
  return (String){.chars=chars,.length=strlen(chars)};
}
String newString(const char* chars,size_t len){
  return (String){.chars=chars,.length=len};
}

bool wordEquals(const String* word,const char* string){
  size_t l=strlen(string);
  if(l!=word->length)
    return false;
  int c=memcmp(word->chars,string,word->length);
  return c==0;
}
int stringCompare(const String a,const String b){
  int c=memcmp(a.chars,b.chars,a.length<b.length?a.length:b.length);
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
char charAt(const String str,int64_t index){
  if(index<0||index>(int64_t)str.length)
    return (char)0;
  return str.chars[index];
}
int64_t indexOfChar(const String str,char c){
  for(size_t i=0;i<str.length;i++){
    if(str.chars[i]==c)
      return i;
  }
  return -1;
}

int64_t indexOfString(const String base,const String child){
  if(child.length>base.length)
    return -1;
  bool isMatch;
  for(size_t off=0;off<=base.length-child.length;off++){
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
int64_t indexOfStringArray(const String* base,size_t baseLen,const String* child,size_t childLen){
  if(childLen>baseLen)
    return -1;
  bool isMatch;
  for(size_t off=0;off<=baseLen-childLen;off++){
    isMatch=true;
    for(size_t i=0;i<childLen;i++){
      if(stringCompare(base[i+off],child[i])!=0){
        isMatch=false;
        break;
      }
    }
    if(isMatch)
      return off;
  }
  return -1;
}

String sliceStart(String str,size_t k){
  if(str.length<k)
    return EMPTY_STRING;
  return (String){.chars=str.chars+k,.length=str.length-k};
}
SlicedString sliceAtChar(String base,char chr){
  int64_t i=indexOfChar(base,chr);
  if(i==-1)//chr not in string
    return (SlicedString){.head=base,.tail=EMPTY_STRING};
  return (SlicedString){.head={.chars=base.chars,.length=i},.tail={.chars=base.chars+(i+1),.length=base.length-(i+1)}};
}
