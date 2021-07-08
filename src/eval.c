#include <stdlib.h>

#include <calc/eval.h>
#include <calc/ast.h>
#include <calc/assert.h>

static double sum(double a, double b);
static double sub(double a, double b);
static double mul(double a, double b);
static double divide(double a, double b);
static double negate(double a);

double evaluate(const char* input) {
    lexer lex = new_lexer(input);
    AST *ast = parse_expr1(&lex);
    double value = evaluate_ast(ast);
    free_ast(ast);
    return value;
}

double evaluate_ast(AST *ast) {
    switch (ast->kind) {
        case ast_number_literal: {
            char* name = normalized_name(ast->loc);
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
                default:
                    CHECK_NOT_REACHED();
            }
            break;
        }

        default:
            break;
    }
    CHECK_NOT_REACHED();
}

static double sum(double a, double b) {
    return a + b;
}

static double sub(double a, double b) {
    return a - b;
}

static double mul(double a, double b) {
    return a * b;
}

static double divide(double a, double b) {
    return a / b;
}

static double negate(double a) {
    return -a;
}

