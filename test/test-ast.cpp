#include <gtest/gtest.h>

extern "C" {
#include <calc/ast.h>
#include <calc/stream.h>
}

TEST(test_ast, ast_simple) {
    char *name = (char *)malloc(sizeof(char));

    {
        lexer lex = new_lexer("42");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(name, ast->loc), "42");
    }

    {
        lexer lex = new_lexer("-1");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_unary_op);
        EXPECT_EQ(ast->op.kind, op_unary_minus);
        EXPECT_EQ(ast->right->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(name, ast->right->loc), "1");
    }

    {
        lexer lex = new_lexer("42+69");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_binary_op);
        EXPECT_EQ(ast->op.kind, op_binary_plus);
        EXPECT_STREQ(normalized_name(name, ast->loc), "42+69");

        EXPECT_EQ(ast->left->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(name, ast->left->loc), "42");

        EXPECT_EQ(ast->right->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(name, ast->right->loc), "69");
    }

    {
        lexer lex = new_lexer("-42+69");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_binary_op);

        EXPECT_EQ(ast->left->kind, ast_unary_op);
        EXPECT_STREQ(normalized_name(name, ast->left->right->loc), "42");

        EXPECT_EQ(ast->right->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(name, ast->right->loc), "69");
    }

    {
        lexer lex = new_lexer("2*3+1");
        AST * ast = parse_expr1(&lex);
        (void)ast;
        EXPECT_TRUE(1);
    }

    {
        lexer lex = new_lexer("(1+1)");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_binary_op);
        EXPECT_EQ(ast->op.kind, op_binary_plus);
        EXPECT_EQ(ast->left->kind, ast_number_literal);
        EXPECT_EQ(ast->right->kind, ast_number_literal);
    }

    {
        lexer lex = new_lexer("sin(2)");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_unary_op);
        EXPECT_EQ(ast->op.kind, op_sin);
        EXPECT_EQ(ast->right->kind, ast_number_literal);
    }

    {
        lexer lex = new_lexer("cos(2)");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_unary_op);
        EXPECT_EQ(ast->op.kind, op_cos);
        EXPECT_EQ(ast->right->kind, ast_number_literal);
    }

    free(name);
}
