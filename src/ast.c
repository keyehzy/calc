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

void free_ast(AST *ast) {
    int n = Size(&ast->children);
    for (int i = 0; i < n; i++) {
        free_ast(GetVector(&ast->children, i));
    }
    free(ast->children.items);
    free(ast);
}

static AST *new_empty_ast() {
    AST *ast      = (AST *)malloc(sizeof(AST));
    ast->children = NewVector();
    return ast;
}

static AST *new_ast(ast_kind kind, codeloc loc, operation op) {
    AST *ast      = (AST *)malloc(sizeof(AST));
    ast->kind     = kind;
    ast->loc      = loc;
    ast->op       = op;
    ast->children = NewVector();
    return ast;
}

static AST *invalid_ast() {
    AST *ast  = (AST *)malloc(sizeof(AST));
    ast->kind = ast_invalid;
    return ast;
}

static operation operation_from_tk(token t, int context) {
    char *    name = normalized_name(t.loc);
    operation op;
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
            CHECK_NOT_REACHED();
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
            CHECK_NOT_REACHED();
        }
    }

    free(name);
    return op;
}

static AST *parse_primary_expr(lexer *lex) {
    switch (L_PEEK().type) {

    case tk_number: {
        codeloc number_span = L_PEEK().loc;
        L_SKIP();
        return new_ast(ast_number_literal, number_span,
                       (operation){.kind  = op_none,
                                   .prec  = prec_none,
                                   .assoc = assoc_none});
    }

    case tk_left_paren: {
        L_SKIP();
        AST *paren_expr = parse_expr1(lex);
        CHECK(L_PEEK().type == tk_right_paren);
        L_SKIP();
        AST *ast = new_ast(ast_paren_expr, paren_expr->loc,
                           (operation){.kind  = op_none,
                                       .prec  = prec_paren,
                                       .assoc = assoc_none});
        PushVector(&ast->children, paren_expr);
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
        return new_ast(ast_const_##constant, const_span,                       \
                       (operation){.kind  = op_none,                           \
                                   .prec  = prec_none,                         \
                                   .assoc = assoc_none});                      \
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
        AST *ast = new_ast(ast_unary_expr, operator_span, op);                   \
        PushVector(&ast->children, parse_primary_expr(lex));                   \
        return ast;                                                            \
    }
        ENUMERATE_FUNCTIONS(PARSE_FUNC);
#undef PARSE_FUNC

    default:
        return invalid_ast();
    }
    CHECK_NOT_REACHED();
}

static AST *parse_rest_expr(lexer *lex, AST *lhs, operation o, int commas) {
    vector tree = NewVector();
    PushVector(&tree, lhs);
    operation new_o;
    AST *     res    = NULL;

    while (1) {
        switch (L_PEEK().type) {
        case tk_comma: {
            if(commas) goto end;
            new_o = operation_from_tk(L_PEEK(), 0);
            L_SKIP();
            PushVector(&tree, parse_expr(lex, new_o,/*commas*/1));
            CHECK(AST_BACK(&tree)->kind != ast_invalid);
            continue;
        }
        case tk_operator: {
            new_o = operation_from_tk(L_PEEK(), 0);

            if (o.prec > new_o.prec ||
                (o.prec == new_o.prec && new_o.assoc == assoc_right)) {
                goto end;
            }

            L_SKIP();

            PushVector(&tree, parse_expr(lex, new_o, commas));
            CHECK(AST_BACK(&tree)->kind != ast_invalid);
            break;
        }

        default:
            goto end;
        }

        combine_tree(&tree, new_o);
    }
end:
    combine_tree(&tree, new_o);
    res = AST_BACK(&tree);
    free(tree.items);
    return res;
}

static void combine_tree(vector *tree, operation o) {
    if(Size(tree) > 1) {
        AST *ast  = new_empty_ast();
        ast->kind = ast_binary_expr;
        ast->op = o;
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
    CHECK_NOT_REACHED();
}

AST *parse_expr(lexer *lex, operation o, int commas) {
    AST *ast = parse_primary_expr(lex);
    if(L_PEEK().type == tk_comma)
        return ast;
    return parse_rest_expr(lex, ast, o, commas);
}

AST *parse_expr1(lexer *lex) {
    AST *     ast  = parse_primary_expr(lex);
    operation none = {.kind = op_none, .prec = prec_none};
    return parse_rest_expr(lex, ast, none, /*commas*/0);
}
