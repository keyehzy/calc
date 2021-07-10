#include <gtest/gtest.h>

extern "C" {
#include <calc/eval.h>
}

TEST(test_eval, eval_simple) {
    const double eps = 1e-9;

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

    {
        double value = evaluate("cos(2.0)");
        ASSERT_NEAR(value, -0.416146836547142, eps);
    }

    {
        double value = evaluate("sin(2.0)");
        ASSERT_NEAR(value, 0.909297426825681, eps);
    }

    {
        double value = evaluate("sqrt(2.0)");
        ASSERT_NEAR(value, 1.41421356237309, eps);
    }
}
