# 0001: RPN Calculator

## Problem

Implement an RPN calculator
that takes an expression like `19 2.14 + 4.5 2 4.3 / - *`
which is usually expressed as `(19 + 2.14) * (4.5 - 2 / 4.3)`
and responds with `85.2974`.
The program should read expressions from standard input
and print the top of the stack to standard output when a newline is encountered.
The program should retain the state of the operand stack between expressions.

[Source](http://programmingpraxis.com/2009/02/19/rpn-calculator/)

## How to run the solutions

Python:

```
$ echo '1 1 +' | python rpn.py
$ echo '19 2.14 + 4.5 2 4.3 / - *' | python rpn.py
$ echo '1 1
+' | python rpn.py
```
