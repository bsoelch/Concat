# Concat Language

Concat is a concatenative programming language 

## Syntax
Concat is a concatenative programming language in postfix notation,
meaning that the arguments of an operator/ an procedure come before the application of that procedure.


Example:
```
1 1 ( i32 i32 ) new y =::   ## creates a new tuple of two 32-bit integers and saves it in the automatically typed variable y
1 1 + 2 *                   ## results in (1+1)*2
i32 =: x                    ## assigns the result of the previous calculation to a new integer named x
x x * x =                   ## replaces x with its square
y                           ## push the value y onto the stack
x y .0 < if                 ## check if x is less than the first element of y
  .1 print                  ## prints the second element of y (still on stack)
else .1 x < _if             ## checks if the seconds element of y (still on stack) is less than x
  x  print                  ## print the value of x 
end                         ## end the if-statement
```


Code is read as a sequence of tokens separated by white-spaces, there are 4 types of tokens 

### string literals
  start and end with a `"` can contain escape sequences <!-- TODO link --> 
### characters literals
  start and end with a `'` can contain escape sequences <!-- TODO link -->
### numbers constants
  Number literals (currently only integers) are a sequence of digits optionally prefixed with a `-`
  After the `-` you can use he prefixes `0b` for binary and `0x` for hexadecimal numbers
### named constants
  <!-- TODO --> 
- `true` and `false`
- type-names, see types-section<!-- TODO link -->
### comments 
  line comments start with `##` 
  multi-line comments start with `#+` and end with `+#`
### assignments
<!-- TODO --> 
### operators
<!-- TODO --> 
### identifiers
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
the elements of a tuple are given as a list of types separated by spaces, a tuple has to contain at least one element

Tuples can be created by using the `new` operator with the correct type after a list of the element values.
The elements of a tuple can be accessed using `.0` to `.<N-1>` with `N` being the element count of that tuple

Examples:
```
( i64 i64 )                    ## a pair of 64-bit integers
tuple( i8 ptr  i64 )           ## a tuple containing a pointer to i8 and a 64-bit number
0 1 ( i64 i64 ) new =:: aPair  ## creates a new tuple of type ( i64 i64 ) with elements 0 and 1 and strores the result in 'aPair'
aPair .0                       ## gets the first element of the tuple aPair
aPair .1                       ## gets the second element of the tuple aPair
```

### structure
structures can be defined with `struct(` `)`
the elements of a struct are a list of types followed by a `:` an the corresponding field name

Structs can be created by using the `new` operator with the correct type after a list of the element values in the order they appeared in the original definition.
The elements of a struct can be accessed using `.<elementName>`


Examples:
```
struct( i8 ptr :chars i64 :length )           ## a tuple containing a pointer to i8 and a 64-bit number

4 2 struct( i64 : x  i64 : y ) new =:: point  ## creates a pair of 64-bit integers and stores it in the variable 'point'
point .x                                      ## gets the first component of the struct point
point .y                                      ## gets the second component of the struct point
```

### enums

<!-- TODO --> 

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
