For compatibility with C procedures with multiple return values are converted to procedures with a single return value

## pointer calling convention (default)
* If there is more than one return value, the last value will be the return value all other return values are returned through pointers
* the return pointers are appended at the end of the argument list

examples:
```
proc( i64 : a i64 : b -> i64 : c i64 : d ) : divMod
## -> int64_t divMod( int64_t a, int64_t b, int64_t* c)

proc( i32 -> ( i32 ) ) : wrap
## -> void wrap( int32_t a0, struct{int32_t e0;}* o0 )

```

## struct calling convention ( currently not accessible )

* if there are multiple return values, the list of return values is treated as if it were wrapped in a structure

examples:
```
proc( i64 : a i64 : b -> i64 : c i64 : d ) : divMod
## -> struct{int64_t c;int64_t d;} divMod( int64_t a, int64_t b, int64_t* c)

proc( i32 -> ( i32 ) ) : wrap
## -> struct{int32_t e0;} wrap( int32_t a0)

```


