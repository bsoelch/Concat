# Types

## primitive types

* `bool` boolean value ( `true` or `false` )
* `i8` 8-bit integer
* `i16` 16-bit integer
* `i32` 32-bit integer
* `i64` 64-bit integer
* `float` 32-bit IEEE float
* `double` 64-bit IEEE float
* `type` a type ( can only exist at compile time )

The integer types can be interpreted as signed (represented as two's complement) or unsigned numbers.
Addition, subtraction an multiplication of integers are calculated modulo `2^N` where `N` is the number of bits.
For operations that have different effects for signed and unsigned numbers there is both a signed and unsigned version.

<!-- TODO describe type system -->

## pointers & arrays

## structures

## unions & enums

## procedures

## named types

<!-- casting rules for named types:

implicit casts only allow if one of the types is a direct descendant (w.r.t. naming) of the other

explicit casts allowed between any two named types with same source type

-->

## the empty type

## type generators
