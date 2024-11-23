# Types
<!-- FIXME include int-literal types--->
## primitive types

* `bool` boolean value ( `true` or `false` )
* `i8` 8-bit integer
* `i16` 16-bit integer
* `i32` 32-bit integer
* `i64` 64-bit integer
* `f32` 32-bit IEEE binary float
* `f64` 64-bit IEEE binary float
* `type` a type ( can only exist at compile time )

The integer types can be interpreted as signed (represented as two's complement) or unsigned numbers.
Addition, subtraction an multiplication of integers are calculated modulo `2^N` where `N` is the number of bits.
For operations that have different effects for signed and unsigned numbers (division, remainder, casts, ...) there is both a signed and unsigned version.

## pointers & arrays

Syntax:

A pointer type, consists of a base-type and a possibly empty list of dimensions,
with an optional `_` at the end of the dimension list signaling an unknown number of elements.
The pointer type end with `ptr` followed by an optional `.mut` signaling that the targeted value can be modified through the pointer.
Array types follow the same syntax, but without the option to signal mutability or unknown size.
0-dimensional arrays are not supported,  <!-- XXX? support 0-D arrays ?-->

The array/pointer dimension can be either (expressions that evaluate to) integer constants or automatic procedure arguments.

Grammar:

```
<pointer-type> := <type> (<dimension>)* `_` ? `ptr`(.`mut`)?
<array-type> := <type> (<dimension>)+ `_` ? `array`
<dimension> := <int> | <int-const-expr> | <auto-argument>
```

Semantics:

An array is a sequential collection of elements, stored in []-major order. <!-- TODO determine if order is row or colum major --> )

A pointer points to an array of the given dimensions, single values are treated as 0-dimensional arrays.
If a `_` is present the pointer may point to an unspecified number (maybe 0) of sequential arrays of the given form.
If no `_` appears in the parameter list, the given memory location is guaranteed to be accessible.


Examples:

```Python
i32 2 3 array ## a 2x3 matrix for 32-bit integers

```

<!-- TODO describe type system -->

## Composite Types

Syntax:


Structure declarations start with `struct(` or `(` and end with `)`
Structure elements are declared as a type optionally followed by ` : ` and a label identifier.
If no label is given the element is labeled with its index (starting from 0).

Union declarations start with `union(` and end with `)`
Union elements are declared as a type followed by a name using the same syntax as labeled structure elements.
By default Unions remember which value is currently stored, the `??` operator can be used to check if a given value is currently stored in the union.
The `union.raw(` types, can be used to store multiple values in the same memory location without remembering the stored value.

Enum declarations start with `enum(` and end with `)`
An Enum is a list of labels, that acts like a restricted set of integers

Grammar:

```
<composite-type> := <struct-type> | <union-type> | <enum-type>
<struct-type> := ( `struct(` | `(` ) ( <type> ( `:` <label> )? )+ `)`
<union-type> := `union(` ( <type>   `:` <label> )+ `)`
<enum-type> :=  `enum(`  ( ( <type> `:` ) ? <label> )+ `)`
<label> := <modifier>* <identifier>
```

Semantics:

Structure elements store all their elements in independent locations, unlike in C there is NO guarantee that the elements are stored in a specific order. <!--XXX? add flag for specifying fixed order -->
Each element is stored at a position given by its alignment requirements, with padding being inserted between the elements.

<!-- XXX? mention that size is used instead of stride for element layout stride -->

The elements of an Enum are all stored in the same memory location.
The alignment of an Enum is the maximum alignment of all elements.
Enums store which element currently is in use and terminate the program if the wrong value is read from.


Unions store their elements the same way as Enums but do not check the indices of their elements.
Unions are intended as a way to save memory when the index of the current Enum-element can be computed from a different value of a structure.

<!-- XXX? is reading from wrong union element supported? -->

## procedures

Syntax:

Procedure type declarations start with `proc(` or `(`
followed by a list of optionally labeled argument types, using the same syntax as structure elements.
The list of argument types is followed by `->` and a list of (unlabeled) return types <!-- TODO? allow labelded return types -->
the list of return types is terminated by a `)`

Grammar:

```
<procedure-type> := ( `proc(` | `(` ) ( <type> ( `:` <label> )? )+ `->` <type> * `)`
```

Semantics:

Procedure types can only be used to declare procedures or as base-type of a procedure pointer.

<!-- TODO more detailed description of procedure pointer syntax -->

For procedure syntax see the general syntax section <!-- TODO section for general syntax -->

Procedure pointers:
<!-- TODO describe procedure pointers -->
Procedure pointers cannot be mutable.

## named types

<!-- casting rules for declared type aliases:

implicit casts only allow if one of the types is a direct descendant (w.r.t. naming) of the other

explicit casts allowed between any two named types with same source type

-->

## the empty type

## type generators

## internal types:
* reference
* auto-type
* arrayIndex

## Grammar

```
<type> := <primitive-type> | <pointer-type> | <array-type> |
            <composite-type> | <procedure-type> | <named-type>
            <empty-type> | <genrated-type> | <C-type> 

<pointer-type> := <type> (<dimension>)* `_` ? `ptr`(`.mut`?)
<array-type> := <type> (<dimension>)+ `_` ? `array`
<dimension> := <int> | <int-const-expr> | <auto-argument>

<composite-type> := <struct-type> | <union-type> | <enum-type>
<struct-type> := ( `struct(` | `(` ) ( <type> ( `:` <label> )? )+ `)`
<union-type> := `union(` ( <type>   `:` <label> )+ `)`
<enum-type> :=  `enum(`  ( <type> ? `:` <label> )+ `)`
<label> := <modifier>* <identifier>

<procedure-type> := ( `proc(` | `(` ) ( <type> ( `:` <label> )? )+ `->` <type> * `)`

<named-type> := <identifier>

<empty-type> := `( )`

<genrated-type> := <procedure: static, returns type> <!--TODO? make grammar explicit-->

<primitive-type> := `bool` | (`i`|`s`|`u`)(`8`|`16`|`32`|`64`) | `f32` | `f64` | `type`
<C-type> := `..ctype.` (
                `void`   |
                `bool`   |
                `schar`  |
                `u`?( `char` | `short` | `int` | `long` | `longlong` ) |
                `size`   |
                `float`  |
                `double` |
                `struct*`|
                `union*`
             )
```

