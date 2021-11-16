#include "calc/vector.h"
#include <calc/assert.h>
#include <calc/ast.h>
#include <calc/token.h>
#include <stdlib.h>

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
    AST *ast      = (AST *)malloc(sizeof(AST));
    *ast = (AST) {0};
    ast->children = NewVector();
    return ast;
}

static AST *make_ast(ast_kind kind) {
    AST *ast              = (AST *)malloc(sizeof(AST));
    *ast = (AST) {0};
    ast->kind             = kind;
    ast->children         = NewVector();
    ast->var_declarations = NewVector();
    ast->fn_declarations  = NewVector();
    return ast;
}

static AST *new_ast(ast_kind kind, codeloc loc, operation op) {
    AST *ast      = (AST *)malloc(sizeof(AST));
    *ast = (AST) {0};
    ast->kind     = kind;
    ast->loc      = loc;
    ast->op       = op;
    ast->children = NewVector();
    return ast;
}

static AST *invalid_ast() {
    AST *ast  = (AST *)malloc(sizeof(AST));
    *ast = (AST) {0};
    ast->kind = ast_invalid;
    return ast;
}

static operation operation_from_tk(token t, int context) {
    char *    name = normalized_name(t.loc);
    operation op = (operation) {0};
    if (context == 0) { /* binary operations */
        switch (name[0]) {
        case '+':
            op = (operation){.kind  = op_binary_plus,
                             .prec  = prec_addsub,
                             .assoc = assoc_right};
            break;
        case '-':
            op = (operation){.kind  = op_binary_minus,
                             .prec  = prec_addsub,
                             .assoc = assoc_right};
            break;
        case '*':
            op = (operation){.kind  = op_binary_times,
                             .prec  = prec_multdiv,
                             .assoc = assoc_right};
            break;
        case '/':
            op = (operation){.kind  = op_binary_div,
                             .prec  = prec_multdiv,
                             .assoc = assoc_right};
            break;
        case '^':
            op = (operation){
                .kind = op_binary_pow, .prec = prec_pow, .assoc = assoc_left};
            break;

        case ',':
            op = (operation){
                .kind = op_comma, .prec = prec_comma, .assoc = assoc_right};
            break;

        default:
            emit_error(error_unexpected_operation, t.loc);  /* error */
            break;
        }
    } else { /* unary operation */
        switch (name[0]) {
        case '+':
            op = (operation){
                .kind = op_unary_plus, .prec = prec_unary, .assoc = assoc_left};
            break;
        case '-':
            op = (operation){.kind  = op_unary_minus,
                             .prec  = prec_unary,
                             .assoc = assoc_left};
            break;
        default:
            emit_error(error_unexpected_operation, t.loc);  /* error */
            break;
        }
    }

    free(name);
    return op;
}

static AST *parse_primary_expr(lexer *lex) {
    switch (L_PEEK().type) {

    case tk_identifier: {
        codeloc identifier_span = L_PEEK().loc;
        L_SKIP();
        return new_ast(ast_variable, identifier_span, (operation){0});
    }

    case tk_number: {
        codeloc number_span = L_PEEK().loc;
        L_SKIP();
        return new_ast(ast_number_literal, number_span, (operation){0});
    }

    case tk_left_paren: {
        L_SKIP();
        AST *paren_expr = parse_expr1(lex);
        CHECK(L_PEEK().type == tk_right_paren);
        L_SKIP();
        AST *ast = new_ast(ast_paren_expr, paren_expr->loc,
                           (operation){.prec = prec_paren});
        PushVector(&ast->children, paren_expr);
        return ast;
    }

    case tk_left_curly: {
        L_SKIP();
        AST *curly_expr = parse_expr1(lex);
        CHECK(L_PEEK().type == tk_right_curly);
        L_SKIP();
        AST *ast = new_ast(ast_curly_expr, curly_expr->loc,
                           (operation){.prec = prec_paren});
        PushVector(&ast->children, curly_expr);
        return ast;
    }

    case tk_operator: {
        operation op            = operation_from_tk(L_PEEK(), 1);
        codeloc   operator_span = L_PEEK().loc;
        L_SKIP();
        AST *ast = new_ast(ast_unary_expr, operator_span, op);
        PushVector(&ast->children, parse_primary_expr(lex));
        return ast;
    }

#define PARSE_CONST(constant)                                                  \
    case tk_##constant: {                                                      \
        codeloc const_span = L_PEEK().loc;                                     \
        L_SKIP();                                                              \
        return new_ast(ast_const_##constant, const_span, (operation){0});      \
    }
        ENUMERATE_CONSTANTS(PARSE_CONST);
#undef PARSE_CONST

#define PARSE_FUNC(fn)                                                         \
    case tk_##fn: {                                                            \
        operation op = (operation){                                            \
            .kind = op_##fn, .prec = prec_unary, .assoc = assoc_left};         \
        codeloc operator_span = L_PEEK().loc;                                  \
        L_SKIP();                                                              \
        CHECK(L_PEEK().type == tk_left_paren);                                 \
        AST *ast = new_ast(ast_unary_expr, operator_span, op);                 \
        PushVector(&ast->children, parse_primary_expr(lex));                   \
        return ast;                                                            \
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
    operation new_o      = {0};
    AST *     res        = NULL;
    int       comma_flag = /*false*/ 0;

    while (1) {
        switch (L_PEEK().type) {
        case tk_comma: {
            if (commas)
                goto end;
            comma_flag = /*true*/ 1;
            new_o      = operation_from_tk(L_PEEK(), 0);
            codeloc commas_loc = L_PEEK().loc;
            L_SKIP();
            PushVector(&tree, parse_expr(lex, new_o, /*commas*/ 1));

            if(AST_BACK(&tree)->kind == ast_invalid) {
              emit_error(error_invalid_expression_in_commas,
                         new_loc(lhs->loc.begin, commas_loc.end));
            }

            continue;
        }
        case tk_operator: {
            new_o = operation_from_tk(L_PEEK(), 0);

            if (o.prec > new_o.prec ||
                (o.prec == new_o.prec && new_o.assoc == assoc_right)) {
                goto end;
            }

            codeloc token_loc = L_PEEK().loc;
            L_SKIP();
            PushVector(&tree, parse_expr(lex, new_o, commas));

            if(AST_BACK(&tree)->kind == ast_invalid) {
              emit_error(error_invalid_rhs_for_binary_expression,
                         new_loc(lhs->loc.begin, token_loc.end));
            }

            break;
        }

        case tk_equal: {
            CHECK(lhs->kind == ast_variable);
            codeloc equal_loc = L_PEEK().loc;
            L_SKIP();

            PushVector(&tree, parse_expr1(lex));

            if(AST_BACK(&tree)->kind == ast_invalid) {
              emit_error(error_invalid_assignee_for_assignment,
                         new_loc(lhs->loc.begin, equal_loc.end));
            }

            combine_tree(&tree, (operation){0});
            AST_BACK(&tree)->kind = ast_assignment;
            goto end;
        }

        default:
            goto end;
        }

        combine_tree(&tree, new_o);
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
        AST *ast  = new_empty_ast();
        ast->kind = ast_binary_expr;
        ast->op   = o;
        ast->loc =
            new_loc(AST_GET(tree, 0)->loc.begin, AST_BACK(tree)->loc.end);

        int SIZE = Size(tree);
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
    if (L_PEEK().type == tk_comma)
        return ast;
    return parse_rest_expr(lex, ast, o, commas);
}

AST *parse_expr1(lexer *lex) {
    AST *ast = parse_primary_expr(lex);
    return parse_rest_expr(lex, ast, (operation){0}, /*commas*/ 0);
}

static AST *parse_let_statement(lexer *lex) {
    const char *begin = L_PEEK().loc.begin;

    L_SKIP(); /* skip 'let' */

    if (L_PEEK().type == tk_identifier) {
        AST *lhs = make_ast(ast_variable);
        lhs->loc = L_PEEK().loc;
        L_SKIP();

        AST *rhs = parse_rest_expr(lex, lhs, (operation){0}, /*commas*/ 0);

        AST *declaration = make_ast(ast_declaration);
        declaration->loc = new_loc(begin, rhs->loc.end);
        PushVector(&declaration->children, rhs);

        /* push into current scope variable declarations */
        AST *scope = (AST *)BackVector(&lex->scope);
        PushVector(&scope->var_declarations, declaration);

        L_SKIP_CHECKED(tk_semicolon);
        return declaration;
    }
    emit_error(error_unexpected_token_after_let, L_PEEK().loc);
    return invalid_ast();
}

static AST *parse_statement(lexer *lex) {
    switch (L_PEEK().type) {

    case tk_identifier: {
        AST *identifier = make_ast(ast_variable);
        identifier->loc = L_PEEK().loc;
        L_SKIP();

        if (L_PEEK().type == tk_semicolon) {
            L_SKIP();
            return identifier;
        } else {
            return parse_rest_expr(lex, identifier, (operation){0},
                                   /*commas*/ 0);
        }
    }

    case tk_let: {
        AST *let_statement = parse_let_statement(lex);
        return let_statement;
    }

    default:
        return parse_expr1(lex); /* try to parse as a trailling
                                  * expression */
    }
}

AST *parse_program(lexer *lex) {
    AST *module = make_ast(ast_module);
    lex->scope  = NewVector();
    PushVector(&lex->scope, module); /* push global scope for declaring
                                      * variables and functions */
    while (1) {
        AST *statement = parse_statement(lex);

        if (statement->kind != ast_invalid) {
            PushVector(&module->children, statement);
        } else {
            switch (L_PEEK().type) {
            case tk_eof:
                goto end;

            case tk_semicolon:
                L_SKIP();
                break;

            default:
                emit_error(error_malformed_expression, statement->loc);
                break;
            }
        }
    }
end:
    PopVector(&lex->scope);
    return module;
}
