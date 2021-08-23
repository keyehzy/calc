#include <gtest/gtest.h>

extern "C" {
#include <calc/stream.h>
#include <calc/token.h>
}

TEST(test_token, token_simple) {
    {
        lexer lex  = new_lexer("42");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_number);
        EXPECT_STREQ((name = normalized_name(t.loc)), "42");
        free(name);
    }
    {
        lexer lex  = new_lexer("4.20");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_number);
        EXPECT_STREQ((name = normalized_name(t.loc)), "4.20");
        free(name);
    }
    {
        lexer lex  = new_lexer("4.20.");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_number);
        EXPECT_STREQ((name = normalized_name(t.loc)), "4.20");
        free(name);
    }
    {
        lexer lex  = new_lexer("+");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ((name = normalized_name(t.loc)), "+");
        free(name);
    }
    {
        lexer lex  = new_lexer("-");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ((name = normalized_name(t.loc)), "-");
        free(name);
    }
    {
        lexer lex  = new_lexer("*");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ((name = normalized_name(t.loc)), "*");
        free(name);
    }

    {
        lexer lex  = new_lexer("/");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ((name = normalized_name(t.loc)), "/");
        free(name);
    }

    {
        lexer lex  = new_lexer("^");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_operator);
        EXPECT_STREQ((name = normalized_name(t.loc)), "^");
        free(name);
    }

    {
        lexer lex  = new_lexer("(");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_left_paren);
        EXPECT_STREQ((name = normalized_name(t.loc)), "(");
        free(name);
    }

    {
        lexer lex  = new_lexer(")");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_right_paren);
        EXPECT_STREQ((name = normalized_name(t.loc)), ")");
        free(name);
    }

    {
        lexer lex  = new_lexer("{");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_left_curly);
        EXPECT_STREQ((name = normalized_name(t.loc)), "{");
        free(name);
    }

    {
        lexer lex  = new_lexer("}");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_right_curly);
        EXPECT_STREQ((name = normalized_name(t.loc)), "}");
        free(name);
    }

    {
        lexer lex  = new_lexer(",");
        token t    = lex.last_token_;
        char *name = NULL;
        EXPECT_EQ(t.type, tk_comma);
        EXPECT_STREQ((name = normalized_name(t.loc)), ",");
        free(name);
    }

    {
      lexer lex  = new_lexer("=");
      token t    = lex.last_token_;
      char *name = NULL;
      EXPECT_EQ(t.type, tk_equal);
      EXPECT_STREQ((name = normalized_name(t.loc)), "=");
      free(name);
    }

    {
      lexer lex  = new_lexer("foo");
      token t    = lex.last_token_;
      char *name = NULL;
      EXPECT_EQ(t.type, tk_identifier);
      EXPECT_STREQ((name = normalized_name(t.loc)), "foo");
      free(name);
    }



#define TEST_TOKEN_FUNCS(func)                                                 \
    {                                                                          \
        lexer lex  = new_lexer(#func);                                         \
        token t    = lex.last_token_;                                          \
        char *name = NULL;                                                     \
        EXPECT_EQ(t.type, tk_##func);                                          \
        EXPECT_STREQ((name = normalized_name(t.loc)), #func);                  \
        free(name);                                                            \
    }
    ENUMERATE_FUNCTIONS(TEST_TOKEN_FUNCS)
    ENUMERATE_CONSTANTS(TEST_TOKEN_FUNCS)
#undef TEST_TOKEN_FUNCS
}
