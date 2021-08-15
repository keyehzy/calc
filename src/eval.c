#include <calc/assert.h>
#include <calc/ast.h>
#include <calc/eval.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-9

#define STATIC_FUNCS(func) static ReturnExpr fn_##func(ReturnExpr);
ENUMERATE_FUNCTIONS(STATIC_FUNCS)
#undef STATIC_FUNCS

#define STATIC_CONSTS(constant) static ReturnExpr const_##constant();
ENUMERATE_CONSTANTS(STATIC_CONSTS)
#undef STATIC_FUNCS

#define NUMBER(a) (a).value.double_val
#define LIST(a) (a).value.list_val

ReturnExpr NewNumber(double val) {
    ReturnExpr ret;
    ret.type    = Number;
    NUMBER(ret) = val;
    return ret;
}

ReturnExpr NewList(vector list) {
    ReturnExpr ret;
    ret.type  = List;
    LIST(ret) = list;
    return ret;
}

static ReturnExpr sum(ReturnExpr a, ReturnExpr b) {
    CHECK(a.type == Number);
    CHECK(b.type == Number);
    return NewNumber(NUMBER(a) + NUMBER(b));
}
static ReturnExpr sub(ReturnExpr a, ReturnExpr b) {
    CHECK(a.type == Number);
    CHECK(b.type == Number);
    return NewNumber(NUMBER(a) - NUMBER(b));
}
static ReturnExpr mul(ReturnExpr a, ReturnExpr b) {
    CHECK(a.type == Number);
    CHECK(b.type == Number);
    return NewNumber(NUMBER(a) * NUMBER(b));
}
static ReturnExpr divide(ReturnExpr a, ReturnExpr b) {
    CHECK(a.type == Number);
    CHECK(b.type == Number);
    return NewNumber(NUMBER(a) / NUMBER(b));
}
static ReturnExpr negate(ReturnExpr a) {
    CHECK(a.type == Number);
    return NewNumber(-NUMBER(a));
}
static ReturnExpr exponentiate(ReturnExpr a, ReturnExpr b) {
    CHECK(a.type == Number);
    CHECK(b.type == Number);
    return NewNumber(pow(NUMBER(a), NUMBER(b)));
}

ReturnExpr evaluate(const char *input) {
    lexer      lex   = new_lexer(input);
    AST *      ast   = parse_expr1(&lex);
    ReturnExpr value = evaluate_ast(ast);
    free_ast(ast);
    return value;
}

ReturnExpr evaluate_ast(AST *ast) {
    switch (ast->kind) {

    case ast_curly_expr: {
        vector list_elements = NewVector();
        if (child_0(ast)->kind == ast_comma_expr) {
            for (int i = 0; i < Size(&child_0(ast)->children); i++) {
                ReturnExpr *elem = malloc(sizeof(ReturnExpr));
                *elem =
                    evaluate_ast(child(child_0(ast), i)); /* bad code i think */
                PushVector(&list_elements, elem);
            }
        } else {
            ReturnExpr *elem = malloc(sizeof(ReturnExpr));
            *elem = evaluate_ast(child_0(ast)); /* bad code i think */
            PushVector(&list_elements, elem);
        }
        return NewList(list_elements);
    }

    case ast_paren_expr:
        return evaluate_ast(child_0(ast));

    case ast_number_literal: {
        char * name  = normalized_name(ast->loc);
        double value = strtod(name, NULL);
        free(name);
        return NewNumber(value);
    }

    case ast_unary_expr: {
        switch (ast->op.kind) {
        case op_unary_plus:
            return evaluate_ast(child_0(ast));

        case op_unary_minus:
            return negate(evaluate_ast(child_0(ast)));

#define CASE_EVAL_FUNCS(funcs)                                                 \
    case op_##funcs:                                                           \
        return fn_##funcs(evaluate_ast(child_0(ast)));
            ENUMERATE_FUNCTIONS(CASE_EVAL_FUNCS)
#undef CASE_EVAL_FUNCS

        default:
            CHECK_NOT_REACHED();
        }
        break;
    }

    case ast_binary_expr: {
        switch (ast->op.kind) {
        case op_binary_plus:
            return sum(evaluate_ast(child_0(ast)), evaluate_ast(child_1(ast)));
        case op_binary_minus:
            return sub(evaluate_ast(child_0(ast)), evaluate_ast(child_1(ast)));
        case op_binary_times:
            return mul(evaluate_ast(child_0(ast)), evaluate_ast(child_1(ast)));
        case op_binary_div:
            return divide(evaluate_ast(child_0(ast)),
                          evaluate_ast(child_1(ast)));
        case op_binary_pow:
            return exponentiate(evaluate_ast(child_0(ast)),
                                evaluate_ast(child_1(ast)));
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

static ReturnExpr fn_cos(ReturnExpr a) {
    CHECK(a.type == Number);
    return NewNumber(cos(NUMBER(a)));
}

static ReturnExpr fn_sin(ReturnExpr a) {
    CHECK(a.type == Number);
    return NewNumber(sin(NUMBER(a)));
}

static ReturnExpr fn_exp(ReturnExpr a) {
    CHECK(a.type == Number);
    return NewNumber(exp(NUMBER(a)));
}

static ReturnExpr fn_log(ReturnExpr a) {
    CHECK(a.type == Number);
    return NewNumber(log(NUMBER(a)));
}

static ReturnExpr fn_sqrt(ReturnExpr a) {
    CHECK(a.type == Number);
    return NewNumber(sqrt(NUMBER(a)));
}

static ReturnExpr const_pi() { return NewNumber(M_PI); }

static ReturnExpr const_e() { return NewNumber(M_E); }
