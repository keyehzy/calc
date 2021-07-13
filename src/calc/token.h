#pragma once

#include <calc/codeloc.h>
#include <calc/stream.h>

#define L_SKIP() skip(lex)
#define L_PEEK() peek(lex)

#define ENUMERATE_NUMBERS(O)                                                   \
    O('0')                                                                     \
    O('1')                                                                     \
    O('2')                                                                     \
    O('3')                                                                     \
    O('4')                                                                     \
    O('5')                                                                     \
    O('6')                                                                     \
    O('7')                                                                     \
    O('8')                                                                     \
    O('9')

#define ENUMERATE_CHARACTERS_LOWERCASE(O)                                      \
    O('a')                                                                     \
    O('b')                                                                     \
    O('c')                                                                     \
    O('d')                                                                     \
    O('e')                                                                     \
    O('f')                                                                     \
    O('g')                                                                     \
    O('h')                                                                     \
    O('i')                                                                     \
    O('j')                                                                     \
    O('k')                                                                     \
    O('l')                                                                     \
    O('m')                                                                     \
    O('n')                                                                     \
    O('o')                                                                     \
    O('p')                                                                     \
    O('q')                                                                     \
    O('r')                                                                     \
    O('s')                                                                     \
    O('t')                                                                     \
    O('u')                                                                     \
    O('v')                                                                     \
    O('x')                                                                     \
    O('y')                                                                     \
    O('z')

#define ENUMERATE_CHARACTERS_UPPERCASE(O)                                      \
    O('A')                                                                     \
    O('B')                                                                     \
    O('C')                                                                     \
    O('D')                                                                     \
    O('E')                                                                     \
    O('F')                                                                     \
    O('G')                                                                     \
    O('H')                                                                     \
    O('I')                                                                     \
    O('J')                                                                     \
    O('K')                                                                     \
    O('L')                                                                     \
    O('M')                                                                     \
    O('N')                                                                     \
    O('O')                                                                     \
    O('P')                                                                     \
    O('Q')                                                                     \
    O('R')                                                                     \
    O('S')                                                                     \
    O('T')                                                                     \
    O('U')                                                                     \
    O('V')                                                                     \
    O('X')                                                                     \
    O('Y')                                                                     \
    O('Z')

#define ENUMERATE_OPERATORS(O)                                                 \
    O('+')                                                                     \
    O('-')                                                                     \
    O('*')                                                                     \
    O('/')                                                                     \
    O('^')

#define ENUMERATE_FUNCTIONS(O)                                                 \
    O(sqrt)                                                                    \
    O(sin)                                                                     \
    O(cos)                                                                     \
    O(exp)                                                                     \
    O(log)

#define ENUMERATE_CONSTANTS(O)                                                 \
    O(pi)                                                                      \
    O(e)

#define CASE(num) case num:

#define CASE_NUMBERS ENUMERATE_NUMBERS(CASE)

#define CASE_CHARACTERS                                                        \
    ENUMERATE_CHARACTERS_LOWERCASE(CASE)                                       \
    ENUMERATE_CHARACTERS_UPPERCASE(CASE)

#define CASE_OPERATORS ENUMERATE_OPERATORS(CASE)

typedef enum {
    tk_eof,
    tk_number,
    tk_operator,
    tk_left_paren,
    tk_right_paren,

#define TOKEN_TYPE_ENUM(func) tk_##func,
    ENUMERATE_FUNCTIONS(TOKEN_TYPE_ENUM) ENUMERATE_CONSTANTS(TOKEN_TYPE_ENUM)
#undef TOKEN_TYPE_FUNCS

} token_type;

typedef struct {
    token_type type;
    codeloc    loc;
} token;

typedef struct lexer {
    stream s;
    token  last_token_;
} lexer;

token peek(struct lexer *);
void  skip(struct lexer *);
lexer new_lexer(const char *);
