# Calc

This is a simple calculator. 

Constructs an Abstract Syntax Tree using a recursive descent parser and then evaluates it. It is
supposed to be easily hackable and straightforward to introduce new operations.

It is not supposed to be a lisp, so Turing completeness/general programming is not the goal. 

Features:

- [X] Evaluate common arithmetic operations such as sum, subtraction, division and multiplication.
- [x] Named functions such as `sin` and `cosine`.
- [x] Custom constants such as `pi` and the Euler number `e`.
- [ ] Arbitrary precision.
- [ ] Complex numbers algebra.
- [ ] Vector algebra using vectors and matrices.

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
