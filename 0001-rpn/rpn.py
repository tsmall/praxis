from collections import namedtuple
import fileinput
import operator


Number = namedtuple('Number', ['value'])
Operation = namedtuple('Operation', ['fn'])


def tokenize(line):
    return line.split()


def parse(tokens):
    for tok in tokens:
        if all( c.isdigit() or c == '.' for c in tok ):
            yield Number( float(tok) )
        elif tok == '+':
            yield Operation(operator.add)
        elif tok == '-':
            yield Operation(operator.sub)
        elif tok == '/':
            yield Operation(operator.div)
        elif tok == '*':
            yield Operation(operator.mul)


def main():
    q = []
    for line in fileinput.input():
        for tok in parse(tokenize(line)):
            if isinstance(tok, Number):
                q.append(tok.value)
            elif isinstance(tok, Operation):
                second, first = q.pop(), q.pop()
                q.append(tok.fn(first, second))
        print q


if __name__ == '__main__':
    main()
