#include <gtest/gtest.h>

extern "C" {
#include <calc/eval.h>
}

TEST(test_eval, eval_simple) {
    {
        double value = evaluate("42");
        EXPECT_EQ(value, 42.0);
    }

    {
        double value = evaluate("-42");
        EXPECT_EQ(value, -42.0);
    }

    {
        double value = evaluate("2+3");
        EXPECT_EQ(value, 5.0);
    }

    {
        double value = evaluate("2-3");
        EXPECT_EQ(value, -1.0);
    }

    {
        double value = evaluate("2*3");
        EXPECT_EQ(value, 6.0);
    }

    {
        double value = evaluate("9/2");
        EXPECT_EQ(value, 4.5);
    }

    {
        double value = evaluate("2*3+1");
        EXPECT_EQ(value, 7.0);
    }

    {
        double value = evaluate("2*3+1");
        EXPECT_EQ(value, 7.0);
    }
}
