# Calc

This is a simple calculator.

Constructs an Abstract Syntax Tree using a recursive descent parser
and then evaluates it. It is supposed to be easily hackable and
straightforward to introduce new operations.

The goal is to be a practical and powerful calculator that just
works. It is not supposed to be a lisp, so general programming is not
intended.

Features:

- [X] Evaluate common arithmetic operations such as sum, subtraction, division and multiplication.
- [x] Named functions such as 'sqrt', `sin`, `cos` and 'log'.
- [x] Custom constants such as `pi` and the Euler number `e`.
- [x] Lists and operation on lists.
- [x] Scripting (variable declaration with 'let', etc).
- [ ] Exception handling and error messages.
- [ ] Vector algebra using vectors and matrices.
- [ ] Arbitrary precision.
- [ ] Complex numbers algebra.

## Usage:
You can use it as a repl
```bash
$ ./calc
> 1/2/3/4
0.041667
>
```

or evaluate from the terminal emulator
```bash
$ ./calc -e 1/2/3/4
0.041667
```

## Examples
Internal named functions:
```bash
$ ./calc
> sqrt(2)+log(2)
2.107360743
>
```

Operation on lists:
```bash
$ ./calc
> {{0, 1}, {1, 0}} - {{1, 0}, {0, 1}}
{{ -1.000000000  1.000000000 }{ 1.000000000  -1.000000000 }}
> sin({pi, pi/2, pi/3, pi/4})
{ 0.000000000  1.000000000  0.866025404  0.707106781 }
>
```

Declaration and use of variable:
```bash
$ ./calc
> let x = {1,3,5,7,11}; sqrt(x);
{ 1.000000000  1.732050808  2.236067977  2.645751311  3.316624790 }
>
```
