#include <gtest/gtest.h>

extern "C" {
#include <calc/ast.h>
#include <calc/stream.h>
}

TEST(test_ast, test_statements) {
    {
        lexer lex = new_lexer("foo;");
        AST * ast = parse_program(&lex);
        EXPECT_EQ(ast->kind, ast_module);
        EXPECT_EQ(child_0(ast)->kind, ast_variable);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("let x = 42;");
        AST * ast = parse_program(&lex);
        EXPECT_EQ(ast->kind, ast_module);
        EXPECT_EQ(AST_BACK(&ast->var_declarations)->kind, ast_declaration);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("42");
        AST * ast = parse_program(&lex);
        EXPECT_EQ(ast->kind, ast_module);
        EXPECT_EQ(child_0(ast)->kind, ast_number_literal);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("-1");
        AST * ast = parse_program(&lex);
        EXPECT_EQ(ast->kind, ast_module);
        EXPECT_EQ(child_0(ast)->kind, ast_unary_expr);
        free_ast(ast);
    }
}

TEST(test_ast, ast_simple) {
    {
        lexer lex  = new_lexer("42");
        AST * ast  = parse_expr1(&lex);
        char *name = NULL;
        EXPECT_EQ(ast->kind, ast_number_literal);
        EXPECT_STREQ((name = normalized_name(ast->loc)), "42");
        free(name);
        free_ast(ast);
    }

    {
        lexer lex  = new_lexer("-1");
        AST * ast  = parse_expr1(&lex);
        char *name = NULL;
        EXPECT_EQ(ast->kind, ast_unary_expr);
        EXPECT_EQ(ast->op.kind, op_unary_minus);
        EXPECT_EQ(child_0(ast)->kind, ast_number_literal);
        EXPECT_STREQ((name = normalized_name(child_0(ast)->loc)), "1");
        free(name);
        free_ast(ast);
    }

    {
        lexer lex  = new_lexer("42+69");
        AST * ast  = parse_expr1(&lex);
        char *name = NULL;
        // EXPECT_EQ(ast->kind, ast_binary_op);
        EXPECT_EQ(ast->op.kind, op_binary_plus);
        EXPECT_STREQ((name = normalized_name(ast->loc)), "42+69");
        free(name);

        EXPECT_EQ(child_0(ast)->kind, ast_number_literal);
        EXPECT_STREQ((name = normalized_name(child_0(ast)->loc)), "42");
        free(name);

        EXPECT_EQ(child_1(ast)->kind, ast_number_literal);
        EXPECT_STREQ((name = normalized_name(child_1(ast)->loc)), "69");
        free(name);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("42^69");
        AST * ast = parse_expr1(&lex);
        // EXPECT_EQ(ast->kind, ast_binary_op);
        EXPECT_EQ(ast->op.kind, op_binary_pow);
        EXPECT_EQ(child_0(ast)->kind, ast_number_literal);
        EXPECT_EQ(child_1(ast)->kind, ast_number_literal);
        free_ast(ast);
    }

    {
        lexer lex  = new_lexer("-42+69");
        AST * ast  = parse_expr1(&lex);
        char *name = NULL;
        // EXPECT_EQ(ast->kind, ast_binary_op);

        EXPECT_EQ(child_0(ast)->kind, ast_unary_expr);
        EXPECT_STREQ((name = normalized_name(child_0(child_0(ast))->loc)),
                     "42");
        free(name);

        EXPECT_EQ(child_1(ast)->kind, ast_number_literal);
        EXPECT_STREQ((name = normalized_name(child_1(ast)->loc)), "69");
        free(name);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("2*3+1");
        AST * ast = parse_expr1(&lex);
        (void)ast;
        EXPECT_TRUE(1);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("(1+1)");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_paren_expr);
        EXPECT_EQ(child_0(ast)->op.kind, op_binary_plus);
        EXPECT_EQ(child_0(child_0(ast))->kind, ast_number_literal);
        EXPECT_EQ(child_1(child_0(ast))->kind, ast_number_literal);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("1, 1");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_comma_expr);
        EXPECT_EQ(child_0(ast)->kind, ast_number_literal);
        EXPECT_EQ(child_1(ast)->kind, ast_number_literal);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("1, 2, 3");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_comma_expr);
        EXPECT_EQ(child_0(ast)->kind, ast_number_literal);
        EXPECT_EQ(child_1(ast)->kind, ast_number_literal);
        EXPECT_EQ(child(ast, 2)->kind, ast_number_literal);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("1 + 2, 3 + 4");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_comma_expr);
        EXPECT_EQ(child_0(ast)->kind, ast_binary_expr);
        EXPECT_EQ(child_1(ast)->kind, ast_binary_expr);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("1 + 2, 3 + 4, 5 + 6");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_comma_expr);
        EXPECT_EQ(child_0(ast)->kind, ast_binary_expr);
        EXPECT_EQ(child_1(ast)->kind, ast_binary_expr);
        EXPECT_EQ(child(ast, 2)->kind, ast_binary_expr);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("{}");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_curly_expr);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("{42}");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_curly_expr);
        EXPECT_EQ(child_0(ast)->kind, ast_number_literal);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("{1, 2, 3, 4}");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_curly_expr);
        EXPECT_EQ(child_0(ast)->kind, ast_comma_expr);
        free_ast(ast);
    }

    {
        lexer lex = new_lexer("{1} + {1}");
        AST * ast = parse_expr1(&lex);
        EXPECT_EQ(ast->kind, ast_binary_expr);
        EXPECT_EQ(child_0(ast)->kind, ast_curly_expr);
        EXPECT_EQ(child_1(ast)->kind, ast_curly_expr);
        free_ast(ast);
    }

#define TEST_AST_FUNCS(func)                                                   \
    {                                                                          \
        lexer lex = new_lexer(#func "(2)");                                    \
        AST * ast = parse_expr1(&lex);                                         \
        EXPECT_EQ(ast->kind, ast_unary_expr);                                  \
        EXPECT_EQ(ast->op.kind, op_##func);                                    \
        EXPECT_EQ(child_0(child_0(ast))->kind, ast_number_literal);            \
        free_ast(ast);                                                         \
    }
    ENUMERATE_FUNCTIONS(TEST_AST_FUNCS)
#undef TEST_AST_FUNCS

#define TEST_AST_CONST(constant)                                               \
    {                                                                          \
        lexer lex = new_lexer(#constant);                                      \
        AST * ast = parse_expr1(&lex);                                         \
        EXPECT_EQ(ast->kind, ast_const_##constant);                            \
        free_ast(ast);                                                         \
    }
    ENUMERATE_CONSTANTS(TEST_AST_CONST)
#undef TEST_AST_CONST
}
