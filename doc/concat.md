# Concat Language

Concat is a concatenative programming language 

## Syntax

<!-- TODO --> 

## Types

<!-- TODO -->

### primitive Types

Concat supports the following primitive types:

- `bool` a Boolean value
- `i8` 8-bit integer
- `char` name-alias for i8
- `i32` 32-bit integer
- `i64` 64-bit integer
- `float` floating point type (currently unimplemented)

<!-- TODO string (name alias for a special tuple) --> 

### pointers

appending `ptr` on a type name generates a pointer type to that type

internally Concat also supports const-pointers (pointers with unmodifiable content) 
const-pointers can currently not be used directly

### tuples
`tuple(` `)` or `(` `)` can be used to define tuple-types
the elements of a tuple are simple a list of types separated by spaces, a tuple has to contain at least one element

Examples:
```
( i64 i64 )          ## a pair of 64-bit integers
tuple( i8 ptr  i64 ) ## a tuple containing a pointer to i8 and a 64-bit number
```

### procedure types

<!-- TODO --> 

### type aliases & opaque types

`type` can be used to declare type-constants:

- `type =: typeName` declares a type-alias:
  type aliases can be used like primitive type names 
- `type : opaqueName` declares an opaque type:
  opaque types can only be used as pointer arguments,
  unlike constants opaque-type definitions can be overwritten by the declaration of a type alias 

Examples:
```
i32 type =: int                        ## define int as an alias for i32
int type =: Int                        ## define Int as an alias for int which is an alias for i32 
type : selfPtr                         ## create the opaque type selfPtr
( int selfPtr ptr ) type =: selfPtr    ## define selfPtr to be a tuple containing itself
```

### internal Types

<!-- TODO --> 

## Operators

<!-- TODO --> 

## Code Blocks

<!-- TODO --> 
