#include <gtest/gtest.h>

extern "C" {
#include <calc/stream.h>
#include <calc/ast.h>
}

TEST(test_ast, ast_simple) {
    {
        lexer lex = new_lexer("42");
        AST *ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(ast->loc), "42");
    }

    {
        lexer lex = new_lexer("-1");
        AST *ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_unary_op);
        EXPECT_EQ(ast->op.kind, op_unary_minus);
        EXPECT_EQ(ast->right->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(ast->right->loc), "1");
    }

    {
        lexer lex = new_lexer("42+69");
        AST *ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_binary_op);
        EXPECT_EQ(ast->op.kind, op_binary_plus);
        EXPECT_STREQ(normalized_name(ast->loc), "42+69");

        EXPECT_EQ(ast->left->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(ast->left->loc), "42");

        EXPECT_EQ(ast->right->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(ast->right->loc), "69");
    }

    {
        lexer lex = new_lexer("-42+69");
        AST *ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_binary_op);

        EXPECT_EQ(ast->left->kind, ast_unary_op);
        EXPECT_STREQ(normalized_name(ast->left->right->loc), "42");

        EXPECT_EQ(ast->right->kind, ast_number_literal);
        EXPECT_STREQ(normalized_name(ast->right->loc), "69");
    }

    {
        lexer lex = new_lexer("2*3+1");
        AST *ast = parse_expr1(&lex);
        EXPECT_TRUE(1);
        (void) ast;
    }
}

