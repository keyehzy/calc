#pragma once

#include <calc/codeloc.h>
#include <calc/stream.h>
#include <calc/token.h>
#include <calc/vector.h>

#define AST_BACK(x) ((AST *)BackVector(x))
#define AST_GET(x, i) ((AST *)GetVector(x, i))

typedef enum {
    ast_invalid,

    /* statements */
    ast_declaration,
    ast_block,
    ast_module,

    /* expressions */
    ast_variable,
    ast_binary_expr,
    ast_unary_expr,
    ast_number_literal,
    ast_paren_expr,
    ast_curly_expr,
    ast_comma_expr,

#define AST_CONST(constant) ast_const_##constant,
    ENUMERATE_CONSTANTS(AST_CONST)
#undef AST_CONST
} ast_kind;

typedef enum {
    op_none,
    op_binary_plus,
    op_binary_minus,
    op_binary_times,
    op_binary_div,
    op_binary_pow,
    op_unary_plus,
    op_unary_minus,
    op_comma,

#define OP_FUNCS(fn) op_##fn,
    ENUMERATE_FUNCTIONS(OP_FUNCS)
#undef OP_FUNCS

} op_kind;

typedef enum {
    prec_none,
    prec_comma,
    prec_addsub,
    prec_multdiv,
    prec_pow,
    prec_unary,
    prec_paren,
} precedence;

typedef enum {
    assoc_none,
    assoc_left,
    assoc_right,
} associativity;

typedef struct {
    op_kind       kind;
    precedence    prec;
    associativity assoc;
} operation;

typedef struct ast {
    ast_kind  kind;
    codeloc   loc;
    operation op;
    vector    children;
    vector    fn_declarations;
    vector    var_declarations;
} AST;

AST *parse_expr(lexer *, operation, int);
AST *parse_expr1(lexer *);
AST *parse_program(lexer *);
AST *child_0(AST *);
AST *child_1(AST *);
AST *child(AST *, int);
void free_ast(AST *);
