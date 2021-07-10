#include <calc/assert.h>
#include <calc/ast.h>
#include <calc/eval.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define RECURSIVE_MAX_DEPTH 15
#define EPS 1e-9

static double sum(double, double);
static double sub(double, double);
static double mul(double, double);
static double divide(double, double);
static double negate(double);
static double exponentiate(double, double);

#define EVAL_FUNCS(func) static double fn_##func(double);
ENUMERATE_FUNCTIONS(EVAL_FUNCS)
#undef EVAL_FUNCS

static double fn_sin_helper(int, double);
static double fn_cos_helper(int, double);
static double fn_exp_helper(int, double);
static double const_pi();

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
        char *name   = (char *)malloc(sizeof(char));
        name         = normalized_name(name, ast->loc);
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

    case ast_const_pi:
        return const_pi();

    default:
        break;
    }
    CHECK_NOT_REACHED();
}

static double sum(double a, double b) { return a + b; }

static double sub(double a, double b) { return a - b; }

static double mul(double a, double b) { return a * b; }

static double divide(double a, double b) { return a / b; }

static double negate(double a) { return -a; }

static double exponentiate(double a, double b) { return fn_exp(b * fn_log(a)); }

static double fn_cos(double a) { return fn_cos_helper(1, a); }

static double fn_sin(double a) { return a * fn_sin_helper(1, a); }

static double fn_exp(double a) { return fn_exp_helper(1, a); }

static double fn_sqrt(double a) {
    double x1 = 1.0;
    double x2;

    do {
        x2 = x1;
        x1 = x1 - (x1 * x1 - a) / (2.0 * x1);
    } while (fabs((x2 - x1) / x1) > EPS);

    return x1;
}

static double fn_log(double a) {
    double x1 = 1.0;
    double x2;

    do {
        x2 = x1;
        x1 = x1 + a * fn_exp(-x1) - 1.0;
    } while (fabs((x2 - x1) / x1) > EPS);

    return x1;
}

static double fn_cos_helper(int n, double a) {
    if (n > RECURSIVE_MAX_DEPTH) {
        return 1.0;
    }
    return 1.0 -
           a * a / ((2.0 * n - 1.0) * (2.0 * n)) * fn_cos_helper(n + 1, a);
}

static double fn_sin_helper(int n, double a) {
    if (n > RECURSIVE_MAX_DEPTH) {
        return 1.0;
    }
    return 1.0 -
           a * a / ((2.0 * n + 1.0) * (2.0 * n)) * fn_sin_helper(n + 1, a);
}

static double fn_exp_helper(int n, double a) {
    if (n > 2 * RECURSIVE_MAX_DEPTH) { /* veeery slow */
        return 1.0;
    }
    return 1.0 + a / n * fn_exp_helper(n + 1, a);
}

static double const_pi() {
    double x  = 1.41421356237309; /* sqrt(2) */
    double pi = 2.0 + x;          /* 2 + sqrt(2) */
    double y  = 1.18920711500272; /* 2^(1/4) */
    double old_x, old_y, old_pi;

    x = 0.5 * (fn_sqrt(x) + 1.0 / fn_sqrt(x));

    do {
        old_x  = x;
        old_y  = y;
        old_pi = pi;
        x      = 0.5 * (fn_sqrt(old_x) + 1.0 / fn_sqrt(old_x));
        y      = (y * fn_sqrt(old_x) + 1.0 / fn_sqrt(old_x)) / (old_y + 1.0);
        pi     = old_pi * (old_x + 1.0) / (old_y + 1.0);
    } while (fabs((old_pi - pi) / pi) > EPS);

    return pi;
}
