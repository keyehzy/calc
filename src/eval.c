#include <calc/assert.h>
#include <calc/ast.h>
#include <calc/eval.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-9

#define STATIC_FUNCS(func) static double fn_##func(double);
ENUMERATE_FUNCTIONS(STATIC_FUNCS)
#undef STATIC_FUNCS

#define STATIC_CONSTS(constant) static double const_##constant();
ENUMERATE_CONSTANTS(STATIC_CONSTS)
#undef STATIC_FUNCS

static double sum(double a, double b) { return a + b; }
static double sub(double a, double b) { return a - b; }
static double mul(double a, double b) { return a * b; }
static double divide(double a, double b) { return a / b; }
static double negate(double a) { return -a; }
static double exponentiate(double a, double b) { return pow(a, b); }

double evaluate(const char *input) {
    lexer  lex   = new_lexer(input);
    AST *  ast   = parse_expr1(&lex);
    double value = evaluate_ast(ast);
    free_ast(ast);
    return value;
}

double evaluate_ast(AST *ast) {
    switch (ast->kind) {

    case ast_paren_expr:
        return evaluate_ast(ast->right);

    case ast_number_literal: {
        char * name  = normalized_name(ast->loc);
        double value = strtod(name, NULL);
        free(name);
        return value;
    }

    case ast_unary_op: {
        switch (ast->op.kind) {
        case op_unary_plus:
            return evaluate_ast(ast->right);

        case op_unary_minus:
            return negate(evaluate_ast(ast->right));

#define CASE_EVAL_FUNCS(funcs)                                                 \
    case op_##funcs:                                                           \
        return fn_##funcs(evaluate_ast(ast->right));
            ENUMERATE_FUNCTIONS(CASE_EVAL_FUNCS)
#undef CASE_EVAL_FUNCS

        default:
            CHECK_NOT_REACHED();
        }
        break;
    }

    case ast_binary_op: {
        switch (ast->op.kind) {
        case op_binary_plus:
            return sum(evaluate_ast(ast->left), evaluate_ast(ast->right));
        case op_binary_minus:
            return sub(evaluate_ast(ast->left), evaluate_ast(ast->right));
        case op_binary_times:
            return mul(evaluate_ast(ast->left), evaluate_ast(ast->right));
        case op_binary_div:
            return divide(evaluate_ast(ast->left), evaluate_ast(ast->right));
        case op_binary_pow:
            return exponentiate(evaluate_ast(ast->left),
                                evaluate_ast(ast->right));
        default:
            CHECK_NOT_REACHED();
        }
        break;
    }

#define CASE_EVAL_CONSTS(constant)                                             \
    case ast_const_##constant:                                                 \
        return const_##constant();
        ENUMERATE_CONSTANTS(CASE_EVAL_CONSTS)
#undef CASE_EVAL_CONSTS

    default:
        break;
    }
    CHECK_NOT_REACHED();
}

static double fn_cos(double a) { return cos(a); }
static double fn_sin(double a) { return sin(a); }
static double fn_exp(double a) { return exp(a); }
static double fn_log(double a) { return log(a); }
static double fn_sqrt(double a) { return sqrt(a); }
static double const_pi() { return M_PI; }
static double const_e() { return M_E; }
