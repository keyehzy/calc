#include <gtest/gtest.h>

extern "C" {
#include <calc/stream.h>
#include <calc/token.h>
}

TEST(test_token, token_simple) {
    char *name = (char *)malloc(sizeof(char));

    {
        lexer lex = new_lexer("42");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_number);
        EXPECT_STREQ(normalized_name(name, t.loc), "42");
    }
    {
        lexer lex = new_lexer("4.20");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_number);
        EXPECT_STREQ(normalized_name(name, t.loc), "4.20");
    }
    {
        lexer lex = new_lexer("+");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ(normalized_name(name, t.loc), "+");
    }
    {
        lexer lex = new_lexer("-");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ(normalized_name(name, t.loc), "-");
    }
    {
        lexer lex = new_lexer("*");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ(normalized_name(name, t.loc), "*");
    }

    {
        lexer lex = new_lexer("/");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ(normalized_name(name, t.loc), "/");
    }

    {
        lexer lex = new_lexer("(");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_left_paren);
        EXPECT_STREQ(normalized_name(name, t.loc), "(");
    }

    {
        lexer lex = new_lexer(")");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_right_paren);
        EXPECT_STREQ(normalized_name(name, t.loc), ")");
    }

    {
        lexer lex = new_lexer("sin");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_sin);
        EXPECT_STREQ(normalized_name(name, t.loc), "sin");
    }

    {
        lexer lex = new_lexer("cos");
        token t   = lex.last_token_;
        EXPECT_EQ(t.type, tk_cos);
        EXPECT_STREQ(normalized_name(name, t.loc), "cos");
    }

    free(name);
}
