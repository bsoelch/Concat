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
  <condition> ((and|or) <condition>)* if
     <if-body>
  ( else <condition> ((and|or) <condition>)* _if
     <elif-body> )*
  ( else
     <else-body> )?
  end
```

An if-block starts with the keyword `if` and ends with the matching `end` keyword.
`else` can appear between `if` (or `_if`) and `end`
`_if` can appear after `else`, `_if` starts a new if-block that ends at the same `end` keyword as the enclosing block.

When the execution reaches `if`or `_if` a boolean is popped from the stack, if the value is `true` the execution continues on the current path, otherwise execution will jump to the position directly after the next `else` statement in the current `if` block, (if there is no `else`statement the execution will jump to the end of the if-block).
When the execution reaches an `else` keyword, the execution will jump to the end of the if-statement

`and` and `or` can be used within the if-condition to short-circuit evaluate a condition, `and` jumps directly to the `else` branch when the condition on top of the stack is false, `or` jumps directly to the `if` branch when the condition on top of the stack is true.
Local variables declared within a chain of `and` statements will stay accessible until the end of the `if`-branch, if an `or` appears as part of the condition, the local variable declared before `if` will not be preserved

The types at the end of all branches of the if statement have, need to the compatible (equal up to implicit casts).

The types on the stack after all `if`/`_if` statements jumping to the same `else` statement need to be identical.

Variables declared in the `if`-branch are accessible until reaching the matching `else` statement, variables declared in the `else`-branch are accessible until reaching the matching `end`.

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
X optional.hasData and X .data 0 >= if
  X .data
else
  -1
end =:: Y
```

## switch-block

syntax:

```
  <value> switch
     ( <label>+ case
       <case-body>
       break )+
     ( default
       <default-body>
       break )*
  end
```

A switch statement allows to execute different branches of code depending on the value of a given integer.

The type of the input value of a switch statement can either be an integer type, or an enum.
When switching an enum, the picked case is chosen depending on the current value stored in the enum.

A switch statement consists of one or more case-branches,optionally followed by an default branch.

Each case branch consists is labeled with a list of constants of the input type, or when switching an enum a list of element-labels of the type of the given enum.
Each value can be assigned to at most one case.
When switching over an enum-value, the compiler requires that all labels of the enum are handled by a branch of the `switch` statement.

When hitting a `break` statement the current case will be terminated and the execution will jump to the end of the `switch` block (for nested `switch` blocks only the innermost block will be terminated).
A case block ends when at a `break`, `continue`, `return` or `..unreachable` statement outside any enclosing nested blocks is reached (statements in nested blocks do not terminate the branch )
If the last statement in the case block is not a `break` statement, an optional `break` can be added at the end of the block.
Unlike in C fall-though between different branches of the `switch` statement is NOT allowed.

For values that are not assigned to any label the `default` case will be executed, if no `default` case is present the program will directly jump to the end of the `switch'  block.
In the current version of the compiler, `default` has to be the last branch in a `switch`-case statement.

The types at the end of all branches reaching the end of the `switch` statement have to be compatible (equal up to implicit conversions)

Variables declared within one `case` block are only accessible in that `case` block.

Examples:

```
anInt switch
  0 2 4 6 8 case
      0 return
    break ## explicit break at end of branch
  1 3 5 7 9 case
      1 return
    ## break at end of branch can be omitted, if end of branch is unreachable
  10 11 12 case
     'A' break
  default  ## there is a case branch modifying the stack so, default branch is necessary
    'B' break
end
## anInt is not between 0 and 9
```

```
## enum( A B i32 : C ) =:: anEnum
anEnum switch
  A ## only labels of enum are allowed as case labels
  case 1 return ## case ends at return
  B case 2 break  ## case ends at break
  C case anEnum .C 0 < if -1 break else 3 return end
  ## all cases handled -> no default neccessary
end
```


## while-block

basic forms:

```
  while <while-body> <condition> do
     <do-body>
  end
```

syntax:

```
  while <while-body> <condition> ( (and|or) <condition> )* ( do
     <do-body> )?
  ( finally
    <finally-body> )?
  end
```

A while loop first evaluates the code until `do` then pops a boolean from the stack, if the value is false the execution continues after `do`, otherwise the program jumps to the position after `end`.
Like for `if`-blocks `and` and `or` can be used to shot-circuit conditional statements.
If a while block does not contain a `do` statement the condition is instead checked when reaching `finally` or `end`.

When reaching the `end` of a loop the program jumps back to `while`.

Within the loop `break` can be used to break out of the loop ( jump to the position after end ) and `continue` can be used to restart the loop ( jump to `finally` or the end of the loop if there is not explicit finally statement ).
The types at the end of all `break` branches have to be equal ( implicitly convertible to ) the types on the stack when reaching `do` ( after popping the condition ).
The types obtained by merging the `continue` branches have to be equal to the types at the start of the loop.

Optionally a while loop can contain a `finally` statement, the code between `finally`  and `end` will be executed after each iteration of the loop, it is not possible to use `continue` in the section between `finally` and `end`.

## for-loop

syntax:

```
  <iterable> for
    <body>
  end

  <iterable> for
    <body>
  finally
    <finaly-body>
  end
```

For-loops are syntactic suggar to simplify iterating over a range or collection.

A `for` loop behaves identical to the following code
```
  <iterable> ..itr.prepare =:: mut: tmp while tmp ..itr.check do
    tmp ..itr.get
    <body>
  finally tmp ..itr.step tmp =
    <finally-body>
  end tmp ..itr.end
```

It is possible to iterate over user defined types by adding overloads to the `..itr.{}` procedure bundles,
the given sigantures are for an iterator `I` over a container `X` containing values of type `T`.

```
( X -> I )    : ..itr.prepare ## create an iterator from the top stack value(s)
( I -> bool ) : ..itr.check   ## check if the iterator has additional elements ( should return a bool )
( I -> T )    : ..itr.get     ## get the current value of the iterator
( I -> I )    : ..itr.step    ## advance the iterator by a single value
( I -> )      : ..itr.end     ## cleanup the iterator
```

By default the core library implements procedures for iterating over an integer ( the range from `0` to the given number) and fixed sized pointers (all elements in the memory region targeted by that array ), the standard library additionally adds iterators over the elements of slices and lists.
<!-- TODO? mention advanced iterators (slice.map) -->

Examples:
```
5 for =:: i
   ## i iterators from 0 to 4
end
```

## and / or

syntax (examples, number and type of and/or can be chosen arbitrarily ):
```
  <condition> ( (and|or) <condition> )+ end
```

`and` and `or` can be used to short-circuit evaluate logical expressions.
And/or blocks start with the first `and` or `or` statement, can contain any number of additional `and`/`or` statements and end with the matching `end` statement, for convenient use as part of conditional statements and/or blocks are automatically closed when hitting any of `if`, `_if` or `do`.

When the execution reaches an `and` or `or` statement a boolean is popped from the stack, if the value is `false` ( in the case of `and` ) or `true` ( in the case of `or` ) the program jumps to the end of the and/or block otherwise the execution continues along the previous path.

Examples:
```
  i 0 < or i x .length >= if ## C: if((i < 0) || (i >= x.length)) ...
      ## i is < 0 or i is >= x .length
  end

  while a and b do ##  C : while(a && (b || c)) ...
    ## both a and b are true
  end

  a  and  b or  c if  ##  C : if(a && (b || c)) ...
    ## a is true, and at least one of b and c is true
  else  a or b and c _if ##  C : else if(a || (b && c)) ...
    ## a is true, or b and c are both true
  end

  ##  C : bool x = a || ( b && c );
  a or b and c end =:: x ## x is true if a is true  or both b and c are true

  ## and blocks can be used in the condition part of other and/or -chains
  a and b end c or if ##  C : if(( a && b ) || c)
    ## either  a and b are both true or c is true
  end
```
