# Control Flow

## if-block

basic form:

```
  <condition> if
    <body>
  else
    <body>
  end
```

general form:

```
  <condition> if <condition> _if
     <if-body>
  else <condition> _if <condition> _if
     <elif-body>
  else
     <else-body>
  end
```

An if-block starts with the keyword `if` and ends with the matching `end` keyword.
`else` can appear between `if` (or `_if`) and `end`
`_if` can appear anywhere in an if-block.

When the execution reaches `if`or `_if` a boolean is popped from the stack, if the value is `true` the execution continues on the current path, otherwise execution will jump after the matching `else` statement, if there is no `else`statement (next statement in the same if block) the execution will jump to the end of the if-block.
When the execution reaches an `else` keyword, the execution will jump to the end of the if-statement

<!-- XXX? definition of branch-->

At the end of each branch  of the if-statement the same types have to be on the stack (due to parsing constraints implicit conversion of types at the end of branches is not supported)

Variables declared within an if-block are accessible until the end of the current branch.

Examples:

```
false if
  ## unreachable code
end
```

```
## sort a pair of numbers on the stack (the numbers have to have the same type)
..over ..over < if
  ..swap
end
```

```
## unwrap optional and cleanup data
X optional.hasData if X .data 0 >= _if
  X .data
else
  -1
end =:: Y
```

## while-block
<!-- TODO while -->
## switch-block
<!-- TODO switch -->


