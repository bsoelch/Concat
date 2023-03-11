# Concat Language

Concat is a concatenative programming language 

## Syntax
Concat is a concatenative programming language in postfix notation,
meaning that the arguments of an operator/ an procedure come before the application of that procedure.


Example:
```
1 1 ( i32 i32 ) new =:: y   ## creates a new tuple of two 32-bit integers and saves it in the automatically typed variable y
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

Concat code is a sequence of 'words' separated by white-spaces

### string and characters literals

string literals start and end with a `"` , character literals use a `'` 
The type of a char literal is `char` ( which is an alias for `i8` ) while a string literals are represented by the array type `string` 

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


## procedures

a procedure declaration starts with a procedure type followed type `=:` and the name of the procedure
the procedure body starts after the declaration and ends at the corresponding `end` statement

Examples:
```
proc( i32 i32 => i32 ) =: add
  ## procedure arguments are implicitly pushed onto the type stack
  + 
  ## value is implicitly returned
end

## procedure arguments are allowed to have lables 
## but it is not possible to only label some of the arguments
proc( i32 : x i32 : y => i32 ) =: max 
## if the arguments are labeled they are not pushed onto the stack
## labeld arguments are implicitly declared as local variables
  x y > if
    x return ## return statement are allowed anywhere in the procedure body
  end
  y return 
end

entryPoint: ## the program entry point is marked with entry point, internally entryPoint declares a procedure with return type void
  ## using a procedure name implicitly calls that procedure
  1 2 add ## prints 3
  max addrOf =:: f ## addrOf can be used to obtain a procedure pointer
  1 2 f () ## () can be used to call procedure pointers
  #drop print ## return values are pushed onto the type stack and have to be used or discarded before the end of the procedure
## return statements are not allowed in the main code section
end ## main code section ends with end
```

### static arguments

procedure arguments can be marked as static, static procedure arguments are resolved at compile time and can be used to declare other types appearing later in the procedure signature

the type of a static procedure arguments has to be of type `type` or an integer type

resolving of static arguments:
procedure arguments are resolved from right to left, a static procedure argument is determined by its first (detectable) appearance:
- if the first occurrence of the static argument is as an parameter, the compiler expected a constant value of the correct type as the corresponding procedure argument
- if the first occurrence of a static argument is within the type of another argument, the matching type/integer will be used to define the static argument 

!!! The current parser cannot detect the target of `rawptr` if the given argument is of type array !!!
<!-- TODO update once parser is fixed -->

Examples:

```
proc( i64 : static k  i8 k ptr : str => i64 ) =: strLen ## split the compile-type length parameter from a sized-pointer
  k return
end

proc( i64 : k type : static T => T rawptr ) : extern malloc   ## return pointers of type given by parameter
proc( type : static T  T rawptr : a T rawptr : b i64 : k => ) : extern memcpy  ## multiple uses of the same generic argument
proc( type : static T  T rawptr : p => ) : extern free

entryPoint:
  "Hello World" strLen print  ## the value of k is determined by the compiler
  16 i8 malloc =:: p        
  p "Test" 4 memcpy           ## generic argument determined by type of constant string ( in current parser by type of p ) 
  p free
end

```


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

when a while block starts the condition part be executed, when reaching `do` the top stack value is checked, 
if it is `true` the while-body will be executed and the program jumps back to `while`,
if the top stack value when reaching `do` is `false` the program will jump to `end` statement
 
`break` and `continue` can be used withing while loops to jump to `end` and `while` respectively.
at least one execution path in a while block has to reach the end statement.

Examples:

```
## anonymous for-loop
0 ## it is possible to use stack values inside the loop
  while 1 + #dup 5 < do ## increment top stack value and check if it is <5 without consuming that value
    #dup print ## will print 1 2 3 4
  end
#drop ## cleanup stack

## do-while loop
0 =:: I
while 
  I print ## prints 0 1 2 3 4
## condition
I ++ 5 <
do end

## double loop with break
0 =:: X
0 =:: Y
false =:: doBreak 
while X ++ 5 < do
  while Y ++ 5 < do
    X Y check if
      true doBreak =
      break
    end
  end
  doBreak if 
    break
  end
end

"Hello World!" =:: s
-1 I =
while I ++ s .length < do
  s i [] 
    #dup ' ' == if ## ignore spaces
      continue
    end
  print
end

```

### switch-case

general syntax:
```
<value> switch
<label1> case 
  ## case 1
  break
<label2> <label3> case
  ## case 2
  return
default
  ## default case
end
```

if `<value>` is equal to one of the labels then the case containing that label is executed,
otherwise the `default` case is executed (if existent)

currently switch-case is supported for integer and enum types

switch case blocks start at their `case` label an end when all execution paths have run into a `return` `break` or `continue` statement,
the `default` block always has to be the last block in the if-statement

 
Examples:

```
i switch ## switch integer
-1 case 
  ## i is -1
  -1 return
0 1 2 case
  ## i is 0, 1 or 2
  break
default
  ## i is not in {-1, 0, 1, 2}
  -2 return
end

## switch-blocks can return values
c switch ## switch character
  ' ' '\n' '\t' '\v' '\f' '\r' case
    true
    break
  default
    false
end =:: isSpace

## when switching over enums, the enum constants can be used without preifx in labels
e switch 
A B case 
  ## A or B 
  break
C case
  e .C print
  break
D case 
  e .D print
  break
end 
```

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

<!-- TODO string (name alias for a special array) --> 

### pointers


Pointer types can be created by appending `ptr`, optionally preceded with an array size <!-- TODO  like to array section -->
 after the name of the target type.

Pointer types point to a fixed number of elements (the given array size or 1 if no size is given), trying to access elements outside the allowed area will trigger a run-time error.
If the 


The "raw" pointer type `rawptr` can be used for unknown size pointer (e.g. pointers returned for C-functions)
unlike the default pointer operations on raw pointers are unchecked.
C-style pointer arithmetic can be used on raw pointers.

By default the pointer target cannot be modified, if the pointer should allow modification this can be signaled by appending `mut`

pointers can be assigned to values pointers types of the same type, with a lower or equals dimension, if the sizes in the common dimensions are identical
static sized pointers can be assigned to dynamic sized pointers of the same dimension.
All pointers can be assigned to a raw-pointers of the matching base type.


Examples:
```
0 =:: mut x                    ## declare a mutable integer x 
x addrOf =:: xAddr             ## the address of x  (will have type  i32 ptr mut)
x addrOf i32 rawptr =: xPtr    ## get the address of x as (immutable) raw i32 pointer
"Hello" i8 5 ptr =: str        ## strings are i8 array-pointers with the length as argument
str i8 _ ptr cast  =: str2     ## fixed size arrays can be assigned to dynamic size arrays
str2 .length                   ## the length in the most significant dimension of array&pointer types can be read through the '.length' field
```

### arrays

<!-- TODO array types --> 


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
  opaque types can only be used as pointer arguments
- `<type> opaqueName =` adds a definition for the opaque type `opaqueName`, once a opaque type has been defined it can no longer be changed
  

Examples:
```
i32 type =: int                        ## define int as an alias for i32
int type =: Int                        ## define Int as an alias for int which is an alias for i32 
type : selfPtr                         ## create the opaque type selfPtr
( int selfPtr ptr ) selfPtr =          ## replace definition of selfPtr with a tuple containing itself
```

### internal Types

<!-- TODO --> 

## Operators

<!-- TODO --> 

## Code Blocks

<!-- TODO --> 

## Namespaces

namespaces allow to use the same identifier in different contexts. <!-- TODO formulation ---> 
namespaces can be declared with the `#namespace` compiler keyword followed by the name of the namespace, 
the namespace will be closed by the matching `#end` statement

namespace names are not allowed to start with `#` or contain `.` 
`.` is used to access variables in other namespaces

The `#using` keyword allows to import a namespace

When resolving variables the compiler checks first the current namespace, then all parent namespace and then the currently active imports.
imports are valid until the end of the namespace (including in subnamespaces) or the end of the current codeblock

```
0 i32 := anInt
#namespace base
  1 i32 := anInt ## variabes that already exist at global level can also be declared in namespaces (depending on the rules for shaddowed variables)
  #namespace child ## namespaces can again contain namespaces
   2 i32 := anInt 
  #end
#end
anInt print ## prints global variable anInt
base.anInt print ## print base.anInt
#using base ## imports namespace base
anInt print ## still prints anInt in current namespace 
subspace.anInt print ## addreses with . are resolved relative to the current namespace
#namespace base
 #using base.subspace ## namespace imports allways have to include the full name
#end
```

## Includes

Source code can be split between multiple file, files can be includes using `#include` 
there are two kinds of includes, local includes and library includes.

Local includes include files relative to the "base-directory" (directory the file passed to the compiler is located) of the current program,
the file path has to supplied as a string

Library includes includes files from the standard library, the file path has to be supplied as an identifier name
!!! currently library includes are not supported !!!

<!-- TODO lib includes --> 

The files are included in the order they are declared in the source code, included within included files are treated in depth first order.
If a included file is already in the process of being included it is skipped.

public global variables from included files can be used independent of the position of the include statement.

All constants declared in included files can be used as constants, 
if the included file is skipped due to an include loop then constants declared after the include loop cannot be used in constant expressions.

Example:

`src/main.concat`:

```
entryPoint:
  42 f ## calls f in 'anotherFile'
end
#include "anotherFile.concat"  ## includes 'anotherFile.concat' located in the same directory as the current file
1 =:: mainConstant
proc( i32 => bool ) =: isConst 
  switch
    mainConstant subIncludeConstant case 
      return true
  end
  return false
end
```

`src/anotherFile.concat`:

```
#include "subDir/subInclude.concat"
proc( i32 => ) =: public f  ## the procedure f in the included file is not visible
  print
end

proc( i32 i32 => i32 ) =: externProc ## it is allowed to shaddow extern procedures with local procedures 
  +
end
```

`src/subDir/subInclude.concat`:

```

#include "../main.concat" ## files in subdirectorys have to used .. to reference files in superdirectories
0 i32 =: subIncludeConstant
proc( => ) =: f ## private procedure f
  mainConstant print ## main constant is accessible but not constant
end
proc( => i32 ) : extern externProc ## local extern procedure
```


 


