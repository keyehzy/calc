#include <gtest/gtest.h>

extern "C" {
#include <calc/eval.h>
}

double evaluate_to_number(const char *expr) {
    return evaluate(expr).value.double_val;
}

TEST(test_eval, eval_simple) {
    const double eps = 1e-9;

    {
        double value = evaluate_to_number("42");
        EXPECT_EQ(value, 42.0);
    }

    {
        double value = evaluate_to_number("-42");
        EXPECT_EQ(value, -42.0);
    }

    {
        double value = evaluate_to_number("2+3");
        EXPECT_EQ(value, 5.0);
    }

    {
        double value = evaluate_to_number("2-3");
        EXPECT_EQ(value, -1.0);
    }

    {
        double value = evaluate_to_number("2*3");
        EXPECT_EQ(value, 6.0);
    }

    {
        double value = evaluate_to_number("9/2");
        EXPECT_EQ(value, 4.5);
    }

    {
        double value = evaluate_to_number("2*3+1");
        EXPECT_EQ(value, 7.0);
    }

    {
        double value = evaluate_to_number("2*3+1");
        EXPECT_EQ(value, 7.0);
    }

    {
        double value = evaluate_to_number("cos(2.0)");
        ASSERT_NEAR(value, -0.416146836547142, eps);
    }

    {
        double value = evaluate_to_number("sin(2.0)");
        ASSERT_NEAR(value, 0.909297426825681, eps);
    }

    {
        double value = evaluate_to_number("sqrt(2.0)");
        ASSERT_NEAR(value, 1.41421356237309, eps);
    }

    {
        double value = evaluate_to_number("exp(2.0)");
        ASSERT_NEAR(value, 7.38905609893065, eps);
    }

    {
        double value = evaluate_to_number("log(2.0)");
        ASSERT_NEAR(value, 0.693147180559945, eps);
    }

    {
        double value = evaluate_to_number("2^3^2");
        ASSERT_NEAR(value, 512.0, eps);
    }

    {
        double value = evaluate_to_number("pi");
        ASSERT_NEAR(value, 3.14159265358979, eps);
    }

    {
      double value = evaluate_to_number("2+2; sqrt(3); pi;");
      ASSERT_NEAR(value, 3.14159265358979, eps);
    }

}
