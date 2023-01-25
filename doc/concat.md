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

Concat code is a sequence of 'words' separated by any number of white-spaces 

### string and characters literals

string literals start and end with a `"` , character literals use a `'` 
The type of a char literal is `char` ( which is an alias for `i8` ) while a string literals are represented by the composite type `string` 

string and char literals are allowed to contain special characters including new-lines 
additionally the following escape sequences are supported

<!-- TODO escape sequences -->

Examples:
```
"Hello World!" string =: hello   ## create a new string saying Hello World
"Multi-line
string \" "                   
'A' char =: aChar 
```

  
### numbers constants

Integer literals are a sequence of digits optionally prefixed with a `-`
after the sign you can use he prefixes `0b` for binary and `0x` for hexadecimal numbers
within an integer literal `_` and `'` can be used as digit separators.

The type of an integer literal is `i32` if it first in a 32-bit integer otherwise `i64`.

Examples:
```
123_456_789           ## decimal integer 123456789 
0b11'00'10'01         ## binary 11001001 (decimal 201)
-0b11                 ## binary negative 3
0x1234_5678_9abc_def0 ## a large hexadecimal number, will be interpreted as 64-bit integer
```
  
### named constants
  <!-- TODO --> 
- `true` and `false` (type `bool`)
- type-names, see types-section <!-- TODO link -->
### comments 
  line comments start with `##` 
  multi-line comments start with `#+` and end with `+#`
### assignments
<!-- TODO --> 
### operators
<!-- TODO --> 
### identifiers
<!-- TODO --> 


## code blocks
### if-block

general syntax:
```
<condition> if
  <if-body>
else <condition> _if
  <elif-body>
else
  <else-body>
end
```

if statements start with a condition followed by `if` an optional `else` and end at the corresponding `end` 

if the condition is true the code-block after `if` is executed, otherwise the program jumps to the matching `else` or `end`

`_if` can be used within `else` blocks, to create `if`-statements that use the same `end` as that `else` block
this makes it easier to chain multiple if-statement


Examples:

```
## minimal if-statement
false if
 ## this code is unreachable
end

## values on stack before if-statement will be present in all if branches
b a if 
  ## a true
  if
    ## a and b both true
  end
else _if
 ## a false and b true
end

## the return values of all branches have to match up
a if 
 1 
else b _if
 2
else
 3
end 
## after the if-statement the type-stack will contain the types at the end of any of the if-blocks (they all have to end with the same types)
print
```

### while-blocks

general syntax:
```
while <condition> do
  <while-body>
end
```

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

### enumerations/unions
enumerations can be defined with `enum(` `)`
the elements of an enum are a list of field names, optionally preceded by a type-name followed by an `:` 
Typed enums can be used to emulate unions and optionals.

Examples:
```
enum( A B C D ) type =: anEnum                             ## decalre a new enum called anEnum
enum( empty i64 : asInt float : asFloat ) type =: number?  ## decalre an enum with 3 elements empty, asInt and asFloat asInt and asFloat can hold a value
```

The enum constants can be accessed using `.<elementName>`, 
if the label does not hold a value storing these constants in a variable or returning them from a function automatically creates a matching instance of the corresponding enum. 
To create enum instances with stored values use the `new` operator on the corresponding enum constant.
The `==` and `!=` operators can be used to check if an enum instance stores a value of a given constant
To access the value stored in the enum use `.<elementName>` on a matching instance, accessing the field on instances of other enum constants will lead to a run-time error.

```
anEnum .A  =:: e1              ## set e1 anEnum .A
3 number? .asInt new =:: n     ## set n to an enum of type number? with value 3
n number? .asFloat == if
  n .asFloat print ## n is a float
else n number? .asInt == if
  n .asInt print   ## n is an int  
end ## n is empty
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
