#pragma once
#include <calc/stream.h>
#include <calc/codeloc.h>

#define ENUMERATE_NUMBERS(O) \
    O('0')  \
    O('1')  \
    O('2')  \
    O('3')  \
    O('4')  \
    O('5')  \
    O('6')  \
    O('7')  \
    O('8')  \
    O('9')  \

#define ENUMERATE_CHARACTERS_LOWERCASE(O) \
    O('a')  \
    O('b')  \
    O('c')  \
    O('d')  \
    O('e')  \
    O('f')  \
    O('g')  \
    O('h')  \
    O('i')  \
    O('j')  \
    O('k')  \
    O('l')  \
    O('m')  \
    O('n')  \
    O('o')  \
    O('p')  \
    O('q')  \
    O('r')  \
    O('s')  \
    O('t')  \
    O('u')  \
    O('v')  \
    O('x')  \
    O('y')  \
    O('z')  \

#define ENUMERATE_CHARACTERS_UPPERCASE(O) \
    O('A')  \
    O('B')  \
    O('C')  \
    O('D')  \
    O('E')  \
    O('F')  \
    O('G')  \
    O('H')  \
    O('I')  \
    O('J')  \
    O('K')  \
    O('L')  \
    O('M')  \
    O('N')  \
    O('O')  \
    O('P')  \
    O('Q')  \
    O('R')  \
    O('S')  \
    O('T')  \
    O('U')  \
    O('V')  \
    O('X')  \
    O('Y')  \
    O('Z')  \

#define ENUMERATE_OPERATORS(O) \
    O('+')  \
    O('-')  \
    O('*')  \
    O('/')  \

#define CASE(num) \
    case num:

#define CASE_NUMBERS \
    ENUMERATE_NUMBERS(CASE)

#define CASE_CHARACTER \
    ENUMERATE_CHARACTERS_LOWERCASE(CASE) \
    ENUMERATE_CHARACTERS_UPPERCASE(CASE)

#define CASE_OPERATORS \
    ENUMERATE_OPERATORS(CASE)

typedef enum {
    tk_eof,
    tk_number,
    tk_operator
} token_type;

typedef struct {
    token_type type;
    codeloc loc;
} token;

token parse_number(stream *s);
