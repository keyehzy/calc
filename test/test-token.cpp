#include <gtest/gtest.h>

#include <calc/stream.h>
#include <calc/token.h>

TEST(test_token, token_simple) {
    stream s = {.buffer="42"};
    token t = parse_number(&s);
    EXPECT_EQ(t.type, tk_number);
}

