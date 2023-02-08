/*
#include <stdio.h>
#include <ctype.h>
*/
#include <string.h>
#include "strings.h"

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
SlicedString sliceString(String base,char chr){
  String head=base;
  head.length=0;
  String tail=base;
  for(;head.length<base.length;head.length++,tail.chars++,tail.length--)
    if(base.chars[head.length]==chr){
      tail.chars++;
      tail.length--;
      break;
    }
  return (SlicedString){.head=head,.tail=tail};
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
