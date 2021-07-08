#pragma once

#include <calc/stream.h>
#include <calc/codeloc.h>
#include <calc/token.h>

typedef enum {
    ast_invalid,
    ast_binary_op,
    ast_unary_op,
    ast_number_literal,
} ast_kind;

typedef enum {
    op_none,
    op_binary_plus,
    op_binary_minus,
    op_binary_times,
    op_binary_div,
    op_unary_plus,
    op_unary_minus,
} op_kind;

typedef enum {
    prec_none,
    prec_addsub,
    prec_multdiv,
    prec_unary,
} precedence;

typedef enum {
    assoc_none,
    assoc_left,
    assoc_right,
} associativity;

typedef struct {
    op_kind kind;
    precedence prec;
    associativity assoc;
} operation;

typedef struct ast {
    ast_kind kind;
    codeloc loc;
    operation op;
    struct ast* left;
    struct ast* right;
} AST;

AST *parse_expr(lexer*, operation);
AST *parse_expr1(lexer*);
void free_ast(AST* );
