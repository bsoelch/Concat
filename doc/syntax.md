
## number literals

All number literals start with a digit.
A number literal consists of a sequence of digits, `'` and `_` can be used as separators between digits.
Optionally the digits can be proceeded by a sign and or a base-modifier.
Integer literals can be followed by a size specifier.
If the number contains a `.` or one of `eExX#` followed by and exponent it will be parsed as a float ( `eE` only up to base 14, `xX` only for bases between 11 and 33 ).

base modifiers:

* `0b` `0B` binary
* `0o` `0O` octal
* `0x` `0X` hexadecimal
* `<n>b` for integer `<n>` given in base 10 -> base n literal

digits for bases greater than 10:
For bases between 10 and 36 `a`-`z` and `A` - `Z` are used for the digits 10 to 35
starting at base 37 `a` to `z` will represent digits 36 to 61,
while `A` to `Z` preserve their previous values

integer sizes:
By default integer literals are parsed as `i32`, or `i64` if the number does not fit in a signed 32-bit integer.

To declare an integer literal as unsigned append `:u` at the end of the literal, the compiler will then automatically pick a matching unsigned integer type (at least `u32`).
To specify a concrete type for a literal append a `:` followed by the name of the type

floating point literals:
`.` is used to separate the integer part of the number form its fractional part.
If an exponent is specified the fractional part will be multiplied with the base take to the power of the exponent
All digits of a base-n float  including the exponent are given as digits in the same base.

C-style hex-float literals: <!-- XXX? remove C-style literals-->
For convenience reason C-style hexadecimal float literals of the form `0x[0-9a-fA-F]+(.[0-9a-fA-F]+)?[pP][+-]?[0-9]+`
The exponent of these literals is written in base 10 and will multiply the number by the given power of two (instead of 16).

Examples:

```
1            ## the integer 1 as i32
1.0          ## 1 as (64-bit) float
-12:i8       ## -12 as 8-bit integer
0xffff:u16   ## a 16-bit integer with all bits set to 1
12345678987654321 ## a decimal 64-bit integer
4b100        ## 16 in base 4
05           ## decimal 5, leading zeros are allowed
0x1'0000     ## 65536 given in base-16
62bBaseNInt  ## 40832636929509 written in base 62
0b11.0010010000111111011010101 ## pi in binary
1E9          ## 10^9 as float
0x1E2        ## 482 as base-16 integer
0x1X2        ## 256 as base-16 float
0xffffffff   ## 4294967295 as i64
0xffffffff:u ## -1 (4294967295u) as i32
0x3          ## 3
1x3          ## -> not a number
```

edge cases:
```
0x0x0       ## 0 as float
62b1:u32     ## 1 as 32-bit integer
62b1u32     ## 453780 as 32-bit integer
```


## overloads

overloads can be used to define an overloaded name bundling multiple procedures

An overload is defined by using the keyword `overload:` followed by the name of that overload and a list of procedure names terminated by `end`

When an overload is defined the parser tries to find an existing overload with the same name in the current scope and if successful merges the current overload into that overload.
If no existing overload with the given name is found a new overload is created using the same rules used for identifier declarations.

Most built-in functions, including all arithmetic operators, are overloaded names that can be extened by user defined functions.

Example:
```
## internal declaration of built-in multiplication ( * )
overload: export: *
  i32.mult
  i64.mult
  f32.mult
  f64.mult
end
```
