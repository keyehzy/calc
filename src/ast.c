#include <stdlib.h>

#include <calc/ast.h>
#include <calc/token.h>
#include <calc/assert.h>

static AST *new_ast(ast_kind, codeloc, operation, AST *, AST *);

static AST *parse_primary_expr(lexer *);

static AST *parse_rest_expr(lexer *, AST *, operation);

static AST *combine_binary(AST *, AST *, operation);

static operation operation_from_tk(token t, int context);

void free_ast(AST* ast) {
    switch(ast->kind) {
        case ast_binary_op:
            free(ast->left);
            free(ast->right);
            free(ast);
            break;

        case ast_unary_op:
            free(ast->right);
            free(ast);
            break;

        default:
            free(ast);
            break;
    }
}

static AST *new_ast(ast_kind kind, codeloc loc, operation op, AST *left, AST *right) {
    AST *ast = (AST *) malloc(sizeof(AST));
    ast->kind = kind;
    ast->loc = loc;
    ast->op = op;
    ast->left = left;
    ast->right = right;
    return ast;
}

static AST *invalid_ast() {
    AST *ast = (AST *) malloc(sizeof(AST));
    ast->kind = ast_invalid;
    return ast;
}

static operation operation_from_tk(token t, int context) {
    if (context == 0) { /* binary operations */
        switch (normalized_name(t.loc)[0]) {
            case '+':
                return (operation) {.kind=op_binary_plus, .prec=prec_addsub, .assoc=assoc_right};
            case '-':
                return (operation) {.kind=op_binary_minus, .prec=prec_addsub, .assoc=assoc_right};
            case '*':
                return (operation) {.kind=op_binary_times, .prec=prec_multdiv, .assoc=assoc_right};
            case '/':
                return (operation) {.kind=op_binary_div, .prec=prec_multdiv, .assoc=assoc_right};
        }
    } else { /* unary operation */
        switch (normalized_name(t.loc)[0]) {
            case '+':
                return (operation) {.kind=op_unary_plus, .prec=prec_unary, .assoc=assoc_left};
            case '-':
                return (operation) {.kind=op_unary_minus, .prec=prec_unary, .assoc=assoc_left};
        }
    }
    CHECK_NOT_REACHED();
}

static AST *parse_primary_expr(lexer *lex) {
    switch (L_PEEK().type) {
        case tk_number: {
            codeloc number_span = L_PEEK().loc;
            L_SKIP();
            return new_ast(
                    ast_number_literal,
                    number_span,
                    (operation) {.kind=op_none, .prec=prec_none, .assoc=assoc_none},
                    NULL,
                    NULL);
        }

        case tk_operator: {
            operation op = operation_from_tk(L_PEEK(), 1);
            codeloc operator_span = L_PEEK().loc;

            L_SKIP();

            return new_ast(
                    ast_unary_op,
                    operator_span,
                    op,
                    NULL,
                    parse_primary_expr(lex));
        }

        default:
            return invalid_ast();
    }
    CHECK_NOT_REACHED();
}

static AST *parse_rest_expr(lexer *lex, AST *lhs, operation o) {
    AST *rhs = NULL;
    operation new_o;

    while (1) {
        switch (L_PEEK().type) {
            case tk_operator: {

                new_o = operation_from_tk(L_PEEK(), 0);

                if (o.prec > new_o.prec ||
                        (o.prec == new_o.prec && new_o.assoc == assoc_right)) {
                    goto end;
                }

                L_SKIP();

                rhs = parse_expr(lex, new_o);
                CHECK(rhs->kind != ast_invalid);
                break;
            }
            default:
                goto end;
        }
        lhs = combine_binary(lhs, rhs, new_o);
    }
    end:
    return lhs;
}

static AST *combine_binary(AST *lhs, AST *rhs, operation o) {
    return new_ast(ast_binary_op, new_loc(lhs->loc.begin, rhs->loc.end), o, lhs, rhs);
}

AST *parse_expr(lexer *lex, operation o) {
    AST *ast = parse_primary_expr(lex);
    return parse_rest_expr(lex, ast, o);
}

AST *parse_expr1(lexer *lex) {
    AST *ast = parse_primary_expr(lex);
    operation none = {.kind=op_none, .prec=prec_none};
    return parse_rest_expr(lex, ast, none);
}
