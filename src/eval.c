#include <calc/assert.h>
#include <calc/ast.h>
#include <calc/codeloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EPS 1e-9

#define STATIC_FUNCS(func) static value fn_##func(value);
ENUMERATE_FUNCTIONS(STATIC_FUNCS)
#undef STATIC_FUNCS

#define STATIC_CONSTS(constant) static value const_##constant();
ENUMERATE_CONSTANTS(STATIC_CONSTS)
#undef STATIC_FUNCS

/* Evaluator */

typedef struct {
  vector scope;
} evaluator;

/* Variables */

static int compare_variable(AST *a, AST *b) {
  return strcmp(normalized_name(a->loc), normalized_name(b->loc));
}

static value NewNumber(double val) {
  value ret;
  ret.type = Real;
  ret.double_val = val;
  return ret;
}

static value NewComplex(double _Complex cmplx) {
  value ret;
  ret.type = Complex;
  ret.complex_val = cmplx;
  return ret;
}

static value NewList(vector list) {
  value ret;
  ret.type = List;
  ret.list_val = list;
  return ret;
}

static value operate_binary_on_list_element(value a, value b,
                                            value (*func)(value, value)) {
  int N = Size(&a.list_val);
  CHECK(N == Size(&b.list_val));
  vector list = NewVector();
  for (int i = 0; i < N; i++) {
    value *s1 = (value *)malloc(sizeof(value));
    value *r1 = GetVector(&a.list_val, i);
    value *r2 = GetVector(&b.list_val, i);
    *s1 = func(*r1, *r2);
    PushVector(&list, s1);
  }
  return NewList(list);
}

static value operate_unary_on_list_element(value a, value (*func)(value)) {
  int N = Size(&a.list_val);
  vector list = NewVector();
  for (int i = 0; i < N; i++) {
    value *s1 = (value *)malloc(sizeof(value));
    value *r1 = GetVector(&a.list_val, i);
    *s1 = func(*r1);
    PushVector(&list, s1);
  }
  return NewList(list);
}

static value sum(value a, value b) {
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

static value sub(value a, value b) {
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

static value mul(value a, value b) {
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

static value divide(value a, value b) {
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

static value negate(value a) {
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

static value exponentiate(value a, value b) {
  if (a.type == Real && b.type == Real) {
    return NewNumber(pow(a.double_val, b.double_val));
  } else if (a.type == Real && b.type == Complex) {
    return NewNumber(cpow(a.double_val, b.complex_val));
  } else if (a.type == Complex && b.type == Real) {
    return NewNumber(cpow(a.complex_val, b.double_val));
  } else if (a.type == Complex && b.type == Complex) {
    return NewNumber(cpow(a.complex_val, b.complex_val));
  } else {
    CHECK_NOT_REACHED();
  }
}

#define CASE_BUILTIN_FUNCS(funcs)                          \
  static value fn_##funcs(value a) {                       \
    if (a.type == Real) {                                  \
      return NewNumber(funcs(a.double_val));               \
    } else if (a.type == Complex) {                        \
      return NewComplex(c##funcs(a.complex_val));          \
    } else if (a.type == List) {                           \
      return operate_unary_on_list_element(a, fn_##funcs); \
    } else {                                               \
      CHECK_NOT_REACHED();                                 \
    }                                                      \
  }
ENUMERATE_FUNCTIONS(CASE_BUILTIN_FUNCS)
#undef CASE_BUILTIN_FUNCS

static value const_pi() { return NewNumber(M_PI); }

static value const_e() { return NewNumber(M_E); }

value evaluate_ast(AST *ast) {
  switch (ast->kind) {
    case ast_variable: {
      vector var_decl = ast->var_declarations;
      for (int i = 0; i < Size(&var_decl); i++) {
        AST *decl = GetVector(&var_decl, i);
        AST *assign = child_0(decl);

        if (compare_variable(ast, child_0(assign)) == 0) {
          return evaluate_ast(child_1(assign));
        }
      }
      emit_error(error_use_of_undeclared_variable, ast->loc);
      break;
    }

    case ast_declaration: {
      AST *assign = child_0(ast);
      return evaluate_ast(child_1(assign));
    }

    case ast_module: {
      vector statements = NewVector();
      for (int i = 0; i < Size(&ast->children); i++) {
        value *elem = malloc(sizeof(value));
        *elem = evaluate_ast(GetVector(&ast->children, i));
        PushVector(&statements, elem);
      }
      return *((value *)BackVector(&statements)); /* module returns
                                                   * result of last
                                                   * expression */
    }

    case ast_curly_expr: {
      vector list_elements = NewVector();
      if (child_0(ast)->kind == ast_comma_expr) {
        for (int i = 0; i < Size(&child_0(ast)->children); i++) {
          value *elem = malloc(sizeof(value));
          *elem =
              evaluate_ast(child(child_0(ast), i)); /* bad code i think */
          PushVector(&list_elements, elem);
        }
      } else {
        value *elem = malloc(sizeof(value));
        *elem = evaluate_ast(child_0(ast)); /* bad code i think */
        PushVector(&list_elements, elem);
      }
      return NewList(list_elements);
    }

    case ast_paren_expr:
      return evaluate_ast(child_0(ast));

    case ast_number_literal: {
      char *name = normalized_name(ast->loc);
      char *endptr = NULL;
      double value = strtod(name, &endptr); /* TODO: parse complex number */
      if (endptr[0] == 'i') {
        free(name);
        return NewComplex(value * I);
      }
      free(name);
      return NewNumber(value);
    }

    case ast_unary_expr: {
      switch (ast->op.kind) {
        case op_unary_plus:
          return evaluate_ast(child_0(ast));

        case op_unary_minus:
          return negate(evaluate_ast(child_0(ast)));

#define CASE_EVAL_FUNCS(funcs) \
  case op_##funcs:             \
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
          return sum(evaluate_ast(child_0(ast)),
                     evaluate_ast(child_1(ast)));
        case op_binary_minus:
          return sub(evaluate_ast(child_0(ast)),
                     evaluate_ast(child_1(ast)));
        case op_binary_times:
          return mul(evaluate_ast(child_0(ast)),
                     evaluate_ast(child_1(ast)));
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

#define CASE_EVAL_CONSTS(constant) \
  case ast_const_##constant:       \
    return const_##constant();
      ENUMERATE_CONSTANTS(CASE_EVAL_CONSTS)
#undef CASE_EVAL_CONSTS

    default:
      break;
  }
  CHECK_NOT_REACHED();
}

value evaluate(const char *input) {
  lexer lex = new_lexer(input);
  AST *ast = parse_program(&lex);
  /* value value = evaluate_ast(ast); */
  /* free_ast(ast); */
  return ast->val;
}
