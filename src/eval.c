#include <calc/codeloc.h>
#include <calc/assert.h>
#include <calc/ast.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EPS 1e-9

#define STATIC_FUNCS(func) static ReturnExpr fn_##func(ReturnExpr);
ENUMERATE_FUNCTIONS(STATIC_FUNCS)
#undef STATIC_FUNCS

#define STATIC_CONSTS(constant) static ReturnExpr const_##constant();
ENUMERATE_CONSTANTS(STATIC_CONSTS)
#undef STATIC_FUNCS

/* Evaluator */

typedef struct {
    vector scope;
} evaluator;

static evaluator NewEvaluator() {
    evaluator ev;
    ev.scope = NewVector();
    return ev;
}

static void push_scope(evaluator *ev, AST *scope) {
    PushVector(&ev->scope, scope);
}

static void pop_scope(evaluator *ev) { PopVector(&ev->scope); }

static AST *get_scope(evaluator *ev) { return AST_BACK(&ev->scope); }

/* Variables */

static int compare_variable(AST *a, AST *b) {
    return strcmp(normalized_name(a->loc), normalized_name(b->loc));
}

static ReturnExpr NewNumber(double val) {
    ReturnExpr ret;
    ret.type    = Real;
    ret.double_val = val;
    return ret;
}

static ReturnExpr NewComplex(double _Complex cmplx) {
    ReturnExpr ret;
    ret.type    = Complex;
    ret.complex_val = cmplx;
    return ret;
}

static ReturnExpr NewList(vector list) {
    ReturnExpr ret;
    ret.type  = List;
    ret.list_val = list;
    return ret;
}

static ReturnExpr
operate_binary_on_list_element(ReturnExpr a, ReturnExpr b,
                               ReturnExpr (*func)(ReturnExpr, ReturnExpr)) {
    int N = Size(&a.list_val);
    CHECK(N == Size(&b.list_val));
    vector list = NewVector();
    for (int i = 0; i < N; i++) {
        ReturnExpr *s1 = (ReturnExpr *)malloc(sizeof(ReturnExpr));
        ReturnExpr *r1 = GetVector(&a.list_val, i);
        ReturnExpr *r2 = GetVector(&b.list_val, i);
        *s1            = func(*r1, *r2);
        PushVector(&list, s1);
    }
    return NewList(list);
}

static ReturnExpr
operate_unary_on_list_element(ReturnExpr a, ReturnExpr (*func)(ReturnExpr)) {
    int    N    = Size(&a.list_val);
    vector list = NewVector();
    for (int i = 0; i < N; i++) {
        ReturnExpr *s1 = (ReturnExpr *)malloc(sizeof(ReturnExpr));
        ReturnExpr *r1 = GetVector(&a.list_val, i);
        *s1            = func(*r1);
        PushVector(&list, s1);
    }
    return NewList(list);
}

static ReturnExpr sum(ReturnExpr a, ReturnExpr b) {
  if (a.type == Real && b.type == Real) {
    return NewNumber(a.double_val + b.double_val);
  } else if (a.type == Real && b.type == Complex) {
    return NewComplex(a.double_val + b.complex_val);
  } else if (a.type == Complex && b.type == Real) {
    return NewComplex(a.complex_val + b.double_val);
  } else if (a.type == Complex && b.type == Complex) {
    return NewComplex(a.complex_val + b.complex_val);
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, sum);
  } else {
    CHECK_NOT_REACHED();
  }
}

static ReturnExpr sub(ReturnExpr a, ReturnExpr b) {
  if (a.type == Real && b.type == Real) {
    return NewNumber(a.double_val - b.double_val);
  } else if (a.type == Real && b.type == Complex) {
    return NewComplex(a.double_val - b.complex_val);
  } else if (a.type == Complex && b.type == Real) {
    return NewComplex(a.complex_val - b.double_val);
  } else if (a.type == Complex && b.type == Complex) {
    return NewComplex(a.complex_val - b.complex_val);
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, sub);
  } else {
    CHECK_NOT_REACHED();
  }
}

static ReturnExpr mul(ReturnExpr a, ReturnExpr b) {
  if (a.type == Real && b.type == Real) {
    return NewNumber(a.double_val * b.double_val);
  } else if (a.type == Real && b.type == Complex) {
    return NewComplex(a.double_val * b.complex_val);
  } else if (a.type == Complex && b.type == Real) {
    return NewComplex(a.complex_val * b.double_val);
  } else if (a.type == Complex && b.type == Complex) {
    return NewComplex(a.complex_val * b.complex_val);
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, mul);
  } else {
    CHECK_NOT_REACHED();
  }
}

static ReturnExpr divide(ReturnExpr a, ReturnExpr b) {
  if (a.type == Real && b.type == Real) {
    return NewNumber(a.double_val / b.double_val);
  } else if (a.type == Real && b.type == Complex) {
    return NewComplex(a.double_val / b.complex_val);
  } else if (a.type == Complex && b.type == Real) {
    return NewComplex(a.complex_val / b.double_val);
  } else if (a.type == Complex && b.type == Complex) {
    return NewComplex(a.complex_val / b.complex_val);
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, divide);
  } else {
    CHECK_NOT_REACHED();
  }
}

static ReturnExpr negate(ReturnExpr a) {
  if (a.type == Real) {
    return NewNumber(-a.double_val);
  } else if (a.type == Complex) {
    return NewComplex(-a.complex_val);
  } else if (a.type == List) {
    return operate_unary_on_list_element(a, negate);
  } else {
    CHECK_NOT_REACHED();
  }
}

static ReturnExpr exponentiate(ReturnExpr a, ReturnExpr b) {
    if(a.type == Real && b.type == Real) {
      return NewNumber(pow(a.double_val, b.double_val));
    } else if(a.type == Real && b.type == Complex) {
      return NewNumber(cpow(a.double_val, b.complex_val));
    } else if(a.type == Complex && b.type == Real) {
      return NewNumber(cpow(a.complex_val, b.double_val));
    } else if(a.type == Complex && b.type == Complex) {
      return NewNumber(cpow(a.complex_val, b.complex_val));
    } else {
      CHECK_NOT_REACHED();
    }
}

#define CASE_BUILTIN_FUNCS(funcs)                                              \
    static ReturnExpr fn_##funcs(ReturnExpr a) {                               \
        if (a.type == Real) {                                                  \
            return NewNumber(funcs(a.double_val));                             \
        } else if (a.type == Complex) {                                        \
            return NewComplex(c##funcs(a.complex_val));                        \
        } else if (a.type == List) {                                           \
            return operate_unary_on_list_element(a, fn_##funcs);               \
        } else {                                                               \
            CHECK_NOT_REACHED();                                               \
        }                                                                      \
    }
ENUMERATE_FUNCTIONS(CASE_BUILTIN_FUNCS)
#undef CASE_BUILTIN_FUNCS

static ReturnExpr const_pi() { return NewNumber(M_PI); }

static ReturnExpr const_e() { return NewNumber(M_E); }

ReturnExpr evaluate_ast(AST *ast, evaluator *ev) {
    switch (ast->kind) {

    case ast_variable: {
        vector var_decl = get_scope(ev)->var_declarations;
        for (int i = 0; i < Size(&var_decl); i++) {

            AST *decl   = GetVector(&var_decl, i);
            AST *assign = child_0(decl);

            if (compare_variable(ast, child_0(assign)) == 0) {
                return evaluate_ast(child_1(assign), ev);
            }
        }
        CHECK_NOT_REACHED(); /* error: use of undeclared variable */
        break;
    }

    case ast_declaration: {
        AST *assign = child_0(ast);
        return evaluate_ast(child_1(assign), ev);
    }

    case ast_module: {
        push_scope(ev, ast);
        vector statements = NewVector();
        for (int i = 0; i < Size(&ast->children); i++) {
            ReturnExpr *elem = malloc(sizeof(ReturnExpr));
            *elem            = evaluate_ast(GetVector(&ast->children, i), ev);
            PushVector(&statements, elem);
        }
        pop_scope(ev);
        return *((ReturnExpr *)BackVector(&statements)); /* module returns
                                                          * result of last
                                                          * expression */
    }

    case ast_curly_expr: {
        vector list_elements = NewVector();
        if (child_0(ast)->kind == ast_comma_expr) {
            for (int i = 0; i < Size(&child_0(ast)->children); i++) {
                ReturnExpr *elem = malloc(sizeof(ReturnExpr));
                *elem            = evaluate_ast(child(child_0(ast), i),
                                     ev); /* bad code i think */
                PushVector(&list_elements, elem);
            }
        } else {
            ReturnExpr *elem = malloc(sizeof(ReturnExpr));
            *elem = evaluate_ast(child_0(ast), ev); /* bad code i think */
            PushVector(&list_elements, elem);
        }
        return NewList(list_elements);
    }

    case ast_paren_expr:
        return evaluate_ast(child_0(ast), ev);

    case ast_number_literal: {
        char * name  = normalized_name(ast->loc);
        char * endptr = NULL;
        double value = strtod(name, &endptr);  /* TODO: parse complex number */
        if(endptr[0] == 'i') {
          free(name);
          return NewComplex(value * I);
        }
        free(name);
        return NewNumber(value);
    }

    case ast_unary_expr: {
        switch (ast->op.kind) {
        case op_unary_plus:
            return evaluate_ast(child_0(ast), ev);

        case op_unary_minus:
            return negate(evaluate_ast(child_0(ast), ev));

#define CASE_EVAL_FUNCS(funcs)                                                 \
    case op_##funcs:                                                           \
        return fn_##funcs(evaluate_ast(child_0(ast), ev));
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
            return sum(evaluate_ast(child_0(ast), ev),
                       evaluate_ast(child_1(ast), ev));
        case op_binary_minus:
            return sub(evaluate_ast(child_0(ast), ev),
                       evaluate_ast(child_1(ast), ev));
        case op_binary_times:
            return mul(evaluate_ast(child_0(ast), ev),
                       evaluate_ast(child_1(ast), ev));
        case op_binary_div:
            return divide(evaluate_ast(child_0(ast), ev),
                          evaluate_ast(child_1(ast), ev));
        case op_binary_pow:
            return exponentiate(evaluate_ast(child_0(ast), ev),
                                evaluate_ast(child_1(ast), ev));
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

ReturnExpr evaluate(const char *input) {
    lexer      lex   = new_lexer(input);
    AST *      ast   = parse_program(&lex);
    evaluator  ev    = NewEvaluator();
    ReturnExpr value = evaluate_ast(ast, &ev);
    free_ast(ast);
    return value;
}
