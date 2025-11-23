# dot if

`.if` `.else` `.and` and `.or` are syntax suggar to simplify woking with optional values and unions

* `.and` is short for `..if.prepare =:: $tmp  $tmp ..if.check and $tmp ..if.ifVal`
* `.or` is short for `..if.prepare =:: $tmp  $tmp ..if.check or $tmp ..if.elseVal`

* `.if` expands to `..if.prepare =:: $tmp  $tmp ..if.check if $tmp ..if.ifVal`, after the matching else-branch `..if.elseVal` will be evaluated.
* `.else` is a shorthand for `.if else`
* to prevent reads from undefined values `..if.ifVal`/`..if.elseVal` in the if/else branch will not be evaluated if the branch is reachable through a previous `and`/`or` statement

* `._if` and `._else?` work the same as `.if`and `.else` but with `_if` instead of `if`

For union-elements `..if.ifVal` evaluates to the value at the given element while `..if.elseVal` returns the original union (to simplify chaining `.if` checks)

For optionals `..if.ifVal` pushes the contained values while `..if.elseVal` is a no-op

## dot case

In union switch-statements `.case` can be used to push the value contained in the elements matched by the current branch (if all elements hitting the branch contain the same value)
