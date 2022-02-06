#include <calc/assert.h>
#include <calc/ast.h>
#include <calc/token.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void evaluate_ast(AST *ast);

static value operate_binary_on_list_element(value a, value b,
                                            value (*func)(value, value)) {
static bool is_number(value a) { return a.type == Real || a.type == Complex; }
  CHECK(a.type == List);
  CHECK(b.type == List);
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
  return (value){.type = List, .list_val = list};
}

static value operate_binary_on_list_element_with_lvalue(value a, value b,
                                                        value (*func)(value,
                                                                      value)) {
  CHECK(is_number(a));
  CHECK(b.type == List);
  int N = Size(&b.list_val);
  vector list = NewVector();
  for (int i = 0; i < N; i++) {
    value *s1 = (value *)malloc(sizeof(value));
    value *r = GetVector(&b.list_val, i);
    *s1 = func(a, *r);
    PushVector(&list, s1);
  }
  return (value){.type = List, .list_val = list};
}

static value operate_binary_on_list_element_with_rvalue(value a, value b,
                                                        value (*func)(value,
                                                                      value)) {
  CHECK(a.type == List);
  CHECK(is_number(b));
  int N = Size(&a.list_val);
  vector list = NewVector();
  for (int i = 0; i < N; i++) {
    value *s1 = (value *)malloc(sizeof(value));
    value *r = GetVector(&a.list_val, i);
    *s1 = func(*r, b);
    PushVector(&list, s1);
  }
  return (value){.type = List, .list_val = list};
}

static value operate_unary_on_list_element(value a, value (*func)(value)) {
  CHECK(a.type == List);
  int N = Size(&a.list_val);
  vector list = NewVector();
  for (int i = 0; i < N; i++) {
    value *s1 = (value *)malloc(sizeof(value));
    value *r1 = GetVector(&a.list_val, i);
    *s1 = func(*r1);
    PushVector(&list, s1);
  }
  return (value){.type = List, .list_val = list};
}

static value sum(value a, value b) {
  if (a.type == Real && b.type == Real) {
    return (value){.type = Real, .double_val = a.double_val + b.double_val};
  } else if (a.type == Real && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.double_val + b.complex_val};
  } else if (a.type == Complex && b.type == Real) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val + b.double_val};
  } else if (a.type == Complex && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val + b.complex_val};
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, sum);
  } else {
    CHECK_NOT_REACHED();
  }
}

static value total(value a) {
  CHECK(a.type == List);
  value v = (value){.type = Real, .double_val = 0};
  int N = Size(&a.list_val);
  for (int i = 0; i < N; i++) {
    value *r1 = GetVector(&a.list_val, i);
    v = sum(v, *r1);
  }
  return v;
}

static value sub(value a, value b) {
  if (a.type == Real && b.type == Real) {
    return (value){.type = Real, .double_val = a.double_val - b.double_val};
  } else if (a.type == Real && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.double_val - b.complex_val};
  } else if (a.type == Complex && b.type == Real) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val - b.double_val};
  } else if (a.type == Complex && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val - b.complex_val};
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, sub);
  } else {
    CHECK_NOT_REACHED();
  }
}

static value mul(value a, value b) {
  if (a.type == Real && b.type == Real) {
    return (value){.type = Real, .double_val = a.double_val * b.double_val};
  } else if (a.type == Real && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.double_val * b.complex_val};
  } else if (a.type == Complex && b.type == Real) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val * b.double_val};
  } else if (a.type == Complex && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val * b.complex_val};
  } else if (is_number(a) && b.type == List) {
    return operate_binary_on_list_element_with_lvalue(a, b, mul);
  } else if (a.type == List && is_number(b)) {
    return operate_binary_on_list_element_with_rvalue(a, b, mul);
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, mul);
  } else {
    CHECK_NOT_REACHED();
  }
}

static value divide(value a, value b) {
  if (a.type == Real && b.type == Real) {
    return (value){.type = Real, .double_val = a.double_val / b.double_val};
  } else if (a.type == Real && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.double_val / b.complex_val};
  } else if (a.type == Complex && b.type == Real) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val / b.double_val};
  } else if (a.type == Complex && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = a.complex_val / b.complex_val};
  } else if (a.type == List && is_number(b)) {
    return operate_binary_on_list_element_with_rvalue(a, b, divide);
  } else if (is_number(a) && b.type == List) {
    return operate_binary_on_list_element_with_lvalue(a, b, divide);
  } else if (a.type == List && b.type == List) {
    return operate_binary_on_list_element(a, b, divide);
  } else {
    CHECK_NOT_REACHED();
  }
}

static value dot(value a, value b) {
  if (a.type == List && b.type == List) {
    return total(operate_binary_on_list_element(a, b, mul));
  } else {
    CHECK_NOT_REACHED();
  }
}

static value negate(value a) {
  if (a.type == Real) {
    return (value){.type = Real, .double_val = -a.double_val};
  } else if (a.type == Complex) {
    return (value){.type = Complex, .complex_val = -a.complex_val};
  } else if (a.type == List) {
    return operate_unary_on_list_element(a, negate);
  } else {
    CHECK_NOT_REACHED();
  }
}

static value exponentiate(value a, value b) {
  if (a.type == Real && b.type == Real) {
    return (value){.type = Real, .double_val = pow(a.double_val, b.double_val)};
  } else if (a.type == Real && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = cpow(a.double_val, b.complex_val)};
  } else if (a.type == Complex && b.type == Real) {
    return (value){.type = Complex,
                   .complex_val = cpow(a.complex_val, b.double_val)};
  } else if (a.type == Complex && b.type == Complex) {
    return (value){.type = Complex,
                   .complex_val = cpow(a.complex_val, b.complex_val)};
  } else {
    CHECK_NOT_REACHED();
  }
}

#define CASE_BUILTIN_FUNCS(funcs)                                              \
  static value fn_##funcs(value a) {                                           \
    if (a.type == Real) {                                                      \
      return (value){.type = Real, .double_val = funcs(a.double_val)};         \
    } else if (a.type == Complex) {                                            \
      return (value){.type = Complex, .complex_val = c##funcs(a.complex_val)}; \
    } else if (a.type == List) {                                               \
      return operate_unary_on_list_element(a, fn_##funcs);                     \
    } else {                                                                   \
      CHECK_NOT_REACHED();                                                     \
    }                                                                          \
  }
ENUMERATE_FUNCTIONS(CASE_BUILTIN_FUNCS)
#undef CASE_BUILTIN_FUNCS

void eval_number_literal(AST *ast) {
  char *name = normalized_name(ast->loc);
  char *endptr = NULL;
  double number = strtod(name, &endptr);
  if (endptr[0] == 'i') {
    ast->val = (value){.type = Complex, .complex_val = number * I};
  } else {
    ast->val = (value){.type = Real, .double_val = number};
  }
  free(name);
}

void eval_binary_expr(AST *ast) {
  switch (ast->op.kind) {
    case op_binary_plus:
      ast->val = sum(child_0(ast)->val, child_1(ast)->val);
      break;
    case op_binary_minus:
      ast->val = sub(child_0(ast)->val, child_1(ast)->val);
      break;
    case op_binary_times:
      ast->val = mul(child_0(ast)->val, child_1(ast)->val);
      break;
    case op_binary_div:
      ast->val = divide(child_0(ast)->val, child_1(ast)->val);
      break;
    case op_binary_pow:
      ast->val = exponentiate(child_0(ast)->val, child_1(ast)->val);
      break;
    case op_binary_dot:
      ast->val = dot(child_0(ast)->val, child_1(ast)->val);
      break;
    default:
      CHECK_NOT_REACHED();
  }
}

void eval_unary_expr(AST *ast) {
  switch (ast->op.kind) {
    case op_unary_plus:
      ast->val = child_0(ast)->val;
      break;

    case op_unary_minus:
      ast->val = negate(child_0(ast)->val);
      break;

#define CASE_EVAL_FUNCS(funcs)                \
  case op_##funcs:                            \
    ast->val = fn_##funcs(child_0(ast)->val); \
    break;
      ENUMERATE_FUNCTIONS(CASE_EVAL_FUNCS)
#undef CASE_EVAL_FUNCS

    default:
      CHECK_NOT_REACHED();
  }
}

void eval_paren_expr(AST *ast) { ast->val = child_0(ast)->val; }

void eval_curly_expr(AST *ast) {
  vector list_elements = NewVector();
  if (child_0(ast)->kind == ast_comma_expr) {
    for (int i = 0; i < Size(&child_0(ast)->children); i++) {
      value *elem = malloc(sizeof(value));
      *elem = child(child_0(ast), i)->val; /* bad code i think */
      PushVector(&list_elements, elem);
    }
  } else {
    value *elem = malloc(sizeof(value));
    *elem = child_0(ast)->val; /* bad code i think */
    PushVector(&list_elements, elem);
  }
  ast->val = (value){.type = List, .list_val = list_elements};
}

static int compare_variable(AST *a, AST *b) {
  return strcmp(normalized_name(a->loc), normalized_name(b->loc));
}

AST *find_declared_variable(AST *ast, AST *scope) {
  vector var_decl = scope->var_declarations;
  for (int i = 0; i < Size(&var_decl); i++) {
    AST *decl = GetVector(&var_decl, i);
    AST *assign = child_0(decl);

    if (compare_variable(ast, child_0(assign)) == 0) {
      return child_1(assign);
    }
  }
  return NULL;
}

void eval_identifier(AST *ast, AST *scope) {
  AST *variable;
  if ((variable = find_declared_variable(ast, scope))) {
    ast->val = variable->val;
  } else {
    emit_error(error_use_of_undeclared_variable, ast->loc);
  }
}

static value const_pi() { return (value){.type = Real, .double_val = M_PI}; }

static value const_e() { return (value){.type = Real, .double_val = M_E}; }

static AST *new_empty_ast();

static AST *new_ast(ast_kind, codeloc, operation);

static AST *parse_primary_expr(lexer *);

static AST *parse_rest_expr(lexer *, AST *, operation, int);

static void combine_tree(vector *, operation);

static operation operation_from_tk(token t, int context);

AST *child_0(AST *ast) {
  CHECK(Size(&ast->children) > 0);
  return GetVector(&ast->children, 0);
}

AST *child_1(AST *ast) {
  CHECK(Size(&ast->children) > 1);
  return GetVector(&ast->children, 1);
}

AST *child(AST *ast, int i) {
  CHECK(Size(&ast->children) > i);
  return GetVector(&ast->children, i);
}

void free_ast(AST *ast) { /* FIXME */
  /* int n = Size(&ast->children); */
  /* for (int i = 0; i < n; i++) { */
  /*     free_ast(GetVector(&ast->children, i)); */
  /* } */
  free(ast->children.items);
  free(ast);
}

static AST *new_empty_ast() {
  AST *ast = (AST *)malloc(sizeof(AST));
  *ast = (AST){0};
  ast->children = NewVector();
  return ast;
}

static AST *make_ast(ast_kind kind) {
  AST *ast = (AST *)malloc(sizeof(AST));
  *ast = (AST){0};
  ast->kind = kind;
  ast->children = NewVector();
  ast->var_declarations = NewVector();
  ast->fn_declarations = NewVector();
  return ast;
}

static AST *new_ast(ast_kind kind, codeloc loc, operation op) {
  AST *ast = (AST *)malloc(sizeof(AST));
  *ast = (AST){0};
  ast->kind = kind;
  ast->loc = loc;
  ast->op = op;
  ast->children = NewVector();
  return ast;
}

static AST *invalid_ast() {
  AST *ast = (AST *)malloc(sizeof(AST));
  *ast = (AST){0};
  ast->kind = ast_invalid;
  return ast;
}

static operation operation_from_tk(token t, int context) {
  char *name = normalized_name(t.loc);
  operation op = (operation){0};
  if (context == 0) { /* binary operations */
    switch (name[0]) {
      case '+':
        op = (operation){
            .kind = op_binary_plus, .prec = prec_addsub, .assoc = assoc_right};
        break;
      case '-':
        op = (operation){
            .kind = op_binary_minus, .prec = prec_addsub, .assoc = assoc_right};
        break;
      case '*':
        op = (operation){.kind = op_binary_times,
                         .prec = prec_multdiv,
                         .assoc = assoc_right};
        break;
      case '/':
        op = (operation){
            .kind = op_binary_div, .prec = prec_multdiv, .assoc = assoc_right};
        break;
      case '^':
        op = (operation){
            .kind = op_binary_pow, .prec = prec_pow, .assoc = assoc_left};
        break;

      case ',':
        op = (operation){
            .kind = op_comma, .prec = prec_comma, .assoc = assoc_right};
        break;
      case '.':
        op = (operation){
            .kind = op_binary_dot, .prec = prec_multdiv, .assoc = assoc_right};
        break;

      default:
        emit_error(error_unexpected_operation, t.loc); /* error */
        break;
    }
  } else { /* unary operation */
    switch (name[0]) {
      case '+':
        op = (operation){
            .kind = op_unary_plus, .prec = prec_unary, .assoc = assoc_left};
        break;
      case '-':
        op = (operation){
            .kind = op_unary_minus, .prec = prec_unary, .assoc = assoc_left};
        break;
      default:
        emit_error(error_unexpected_operation, t.loc); /* error */
        break;
    }
  }

  free(name);
  return op;
}

static AST *parse_primary_expr(lexer *lex) {
  switch (peek(lex).type) {
    case tk_identifier: {
      codeloc identifier_span = peek(lex).loc;
      skip(lex);
      AST *identifier = new_ast(ast_variable, identifier_span, (operation){0});
      eval_identifier(identifier, AST_BACK(&lex->scope));
      return identifier;
    }

    case tk_number: {
      codeloc number_span = peek(lex).loc;
      skip(lex);
      AST *number_ast =
          new_ast(ast_number_literal, number_span, (operation){0});
      eval_number_literal(number_ast);
      return number_ast;
    }

    case tk_left_paren: {
      skip(lex);
      AST *paren_expr = parse_expr1(lex);
      CHECK(peek(lex).type == tk_right_paren);
      skip(lex);
      AST *ast = new_ast(ast_paren_expr, paren_expr->loc,
                         (operation){.prec = prec_paren});
      PushVector(&ast->children, paren_expr);
      eval_paren_expr(ast);
      return ast;
    }

    case tk_left_curly: {
      skip(lex);
      AST *curly_expr = parse_expr1(lex);
      CHECK(peek(lex).type == tk_right_curly);
      skip(lex);
      AST *ast = new_ast(ast_curly_expr, curly_expr->loc,
                         (operation){.prec = prec_paren});
      PushVector(&ast->children, curly_expr);
      eval_curly_expr(ast);
      return ast;
    }

    case tk_operator: {
      operation op = operation_from_tk(peek(lex), 1);
      codeloc operator_span = peek(lex).loc;
      skip(lex);
      AST *ast = new_ast(ast_unary_expr, operator_span, op);
      PushVector(&ast->children, parse_primary_expr(lex));
      eval_unary_expr(ast);
      return ast;
    }

#define PARSE_CONST(constant)                                           \
  case tk_##constant: {                                                 \
    codeloc const_span = peek(lex).loc;                                 \
    skip(lex);                                                          \
    AST *c = new_ast(ast_const_##constant, const_span, (operation){0}); \
    c->val = const_##constant();                                        \
    return c;                                                           \
  }
      ENUMERATE_CONSTANTS(PARSE_CONST);
#undef PARSE_CONST

#define PARSE_FUNC(fn)                                                         \
  case tk_##fn: {                                                              \
    operation op =                                                             \
        (operation){.kind = op_##fn, .prec = prec_unary, .assoc = assoc_left}; \
    codeloc operator_span = peek(lex).loc;                                     \
    skip(lex);                                                                 \
    CHECK(peek(lex).type == tk_left_paren);                                    \
    AST *ast = new_ast(ast_unary_expr, operator_span, op);                     \
    PushVector(&ast->children, parse_primary_expr(lex));                       \
    eval_unary_expr(ast);                                                      \
    return ast;                                                                \
  }
      ENUMERATE_FUNCTIONS(PARSE_FUNC);
#undef PARSE_FUNC

    default:
      return invalid_ast();
  }
}

static AST *parse_rest_expr(lexer *lex, AST *lhs, operation o, int commas) {
  vector tree = NewVector();
  PushVector(&tree, lhs);
  operation new_o = {0};
  AST *res = NULL;
  int comma_flag = /*false*/ 0;

  while (1) {
    switch (peek(lex).type) {
      case tk_comma: {
        if (commas) goto end;
        comma_flag = /*true*/ 1;
        new_o = operation_from_tk(peek(lex), 0);
        codeloc commas_loc = peek(lex).loc;
        skip(lex);
        PushVector(&tree, parse_expr(lex, new_o, /*commas*/ 1));

        if (AST_BACK(&tree)->kind == ast_invalid) {
          emit_error(error_invalid_expression_in_commas,
                     new_loc(lhs->loc.begin, commas_loc.end));
        }

        continue;
      }
      case tk_operator: {
        new_o = operation_from_tk(peek(lex), 0);

        if (o.prec > new_o.prec ||
            (o.prec == new_o.prec && new_o.assoc == assoc_right)) {
          goto end;
        }

        codeloc token_loc = peek(lex).loc;
        skip(lex);
        PushVector(&tree, parse_expr(lex, new_o, commas));

        if (AST_BACK(&tree)->kind == ast_invalid) {
          emit_error(error_invalid_rhs_for_binary_expression,
                     new_loc(lhs->loc.begin, token_loc.end));
        }

        break;
      }

      case tk_equal: {
        if (lhs->kind != ast_variable) {
          emit_error(error_invalid_lhs_for_assignment, lhs->loc);
        }
        codeloc equal_loc = peek(lex).loc;
        skip(lex);

        PushVector(&tree, parse_expr1(lex));

        if (AST_BACK(&tree)->kind == ast_invalid) {
          emit_error(error_invalid_assignee_for_assignment,
                     new_loc(lhs->loc.begin, equal_loc.end));
        }

        combine_tree(&tree, (operation){0});
        AST *assignment = AST_BACK(&tree);
        assignment->kind = ast_assignment;
        assignment->val = child_1(assignment)->val;
        goto end;
      }

      default:
        goto end;
    }

    combine_tree(&tree, new_o);
    eval_binary_expr(AST_BACK(&tree));
  }
end:
  combine_tree(&tree, new_o);
  res = AST_BACK(&tree);

  if (comma_flag) { /* HACK */
    res->kind = ast_comma_expr;
  }

  free(tree.items);
  return res;
}

static void combine_tree(vector *tree, operation o) {
  if (Size(tree) > 1) {
    AST *ast = new_empty_ast();
    ast->kind = ast_binary_expr;
    ast->op = o;
    ast->loc = new_loc(AST_GET(tree, 0)->loc.begin, AST_BACK(tree)->loc.end);

    int SIZE = Size(tree);
    /* TODO: I think we can merge these two for loops */
    for (int i = 0; i < SIZE; i++) {
      PushVector(&ast->children, GetVector(tree, i));
    }
    for (int i = 0; i < SIZE; i++) {
      PopVector(tree);
    }
    PushVector(tree, ast);
    return;
  } else if (Size(tree) == 1) {
    return;
  }
}

AST *parse_expr(lexer *lex, operation o, int commas) {
  AST *ast = parse_primary_expr(lex);
  if (peek(lex).type == tk_comma) return ast;
  return parse_rest_expr(lex, ast, o, commas);
}

AST *parse_expr1(lexer *lex) {
  AST *ast = parse_primary_expr(lex);
  return parse_rest_expr(lex, ast, (operation){0}, /*commas*/ 0);
}

void declare_in_scope(lexer *lex, AST *declaration) {
  AST *scope = (AST *)BackVector(&lex->scope);
  PushVector(&scope->var_declarations, declaration);
}

static AST *parse_let_statement(lexer *lex) {
  const char *begin = peek(lex).loc.begin;

  skip(lex); /* skip 'let' */

  if (peek(lex).type == tk_identifier) {
    AST *lhs = make_ast(ast_variable);
    lhs->loc = peek(lex).loc;

    skip(lex);

    if (find_declared_variable(lhs, AST_BACK(&lex->scope))) {
      emit_error(error_redeclaring_declared_variable, lhs->loc);
    }

    AST *rhs = parse_rest_expr(lex, lhs, (operation){0}, /*commas*/ 0);

    AST *declaration = make_ast(ast_declaration);
    declaration->loc = new_loc(begin, rhs->loc.end);
    PushVector(&declaration->children, rhs);
    declaration->val = rhs->val;
    declare_in_scope(lex, declaration);
    skip_checked(lex, tk_semicolon);
    return declaration;
  }
  emit_error(error_unexpected_token_after_let, peek(lex).loc);
  return invalid_ast();
}

static AST *parse_statement(lexer *lex) {
  switch (peek(lex).type) {
    case tk_identifier: {
      AST *identifier = make_ast(ast_variable);
      identifier->loc = peek(lex).loc;
      eval_identifier(identifier, AST_BACK(&lex->scope));
      skip(lex);

      if (peek(lex).type == tk_semicolon) {
        skip(lex);
        return identifier;
      } else {
        return parse_rest_expr(lex, identifier, (operation){0},
                               /*commas*/ 0);
      }
    }

    case tk_let: {
      AST *let_statement = parse_let_statement(lex);
      let_statement->val = child_0(let_statement)->val;
      return let_statement;
    }

    default:
      return parse_expr1(lex); /* try to parse as a trailling
                                * expression */
  }
}

AST *parse_program(lexer *lex) {
  AST *module = make_ast(ast_module);
  lex->scope = NewVector();
  PushVector(&lex->scope, module); /* push global scope for declaring
                                    * variables and functions */
  while (1) {
    AST *statement = parse_statement(lex);

    if (statement->kind != ast_invalid) {
      PushVector(&module->children, statement);
    } else {
      switch (peek(lex).type) {
        case tk_eof:
          goto end;

        case tk_semicolon:
          skip(lex);
          break;

        default:
          emit_error(error_malformed_expression, statement->loc);
          break;
      }
    }
  }
end:
  PopVector(&lex->scope);
  module->val = AST_BACK(&module->children)->val;
  return module;
}

value evaluate(const char *input) {
  lexer lex = new_lexer(input);
  AST *ast = parse_program(&lex);
  return ast->val;
}
