#include <gtest/gtest.h>

extern "C" {
#include <calc/ast.h>
}

double evaluate_to_number(const char *expr) {
  return evaluate(expr).double_val;
}

TEST(test_eval, eval_simple) {
  const double eps = 1e-9;

  { EXPECT_EQ(evaluate("42").double_val, 42.0); }

  { EXPECT_EQ(evaluate("-42").double_val, -42.0); }

  { EXPECT_EQ(evaluate("4.20i").complex_val, 0.0 + 4.20 * I); }

  { EXPECT_EQ(evaluate("1.0 + 0.5i").complex_val, 1.0 + 0.5 * I); }

  { EXPECT_EQ(evaluate("1.0 - 0.5i").complex_val, 1.0 - 0.5 * I); }

  { EXPECT_EQ(evaluate("1.0 * 0.5i").complex_val, 1.0 * 0.5 * I); }

  // { FIXME @@@:
  //   EXPECT_EQ(evaluate("1.0 / 2.0i").complex_val, 1.0 / 2.0*I);
  // }

  { EXPECT_EQ(evaluate("-0.5i").complex_val, -0.5 * I); }

  // { FIX ME @@@:
  //   EXPECT_EQ(evaluate("1i^2").complex_val, cpow(I, 2.0));
  // }

  { EXPECT_EQ(evaluate("sqrt(2.0i)").complex_val, csqrt(2.0 * I)); }

  { EXPECT_EQ(evaluate("2+3").double_val, 5.0); }

  { EXPECT_EQ(evaluate("2-3").double_val, -1.0); }

  { EXPECT_EQ(evaluate("2*3").double_val, 6.0); }

  { EXPECT_EQ(evaluate("9/2").double_val, 4.5); }

  { EXPECT_EQ(evaluate("2*3+1").double_val, 7.0); }

  { EXPECT_EQ(evaluate("2*3+1").double_val, 7.0); }

  { ASSERT_NEAR(evaluate("cos(2.0)").double_val, -0.416146836547142, eps); }

  { ASSERT_NEAR(evaluate("sin(2.0)").double_val, 0.909297426825681, eps); }

  { ASSERT_NEAR(evaluate("sqrt(2.0)").double_val, 1.41421356237309, eps); }

  { ASSERT_NEAR(evaluate("exp(2.0)").double_val, 7.38905609893065, eps); }

  { ASSERT_NEAR(evaluate("log(2.0)").double_val, 0.693147180559945, eps); }

  { ASSERT_NEAR(evaluate("2^3^2").double_val, 512.0, eps); }

  { ASSERT_NEAR(evaluate_to_number("pi"), 3.14159265358979, eps); }

  {
    ASSERT_NEAR(evaluate_to_number("2+2; sqrt(3); pi;"), 3.14159265358979, eps);
  }

  { ASSERT_NEAR(evaluate("let x = 2;").double_val, 2.0, eps); }

  { ASSERT_NEAR(evaluate("let x = sqrt(9); x;").double_val, 3.0, eps); }

  { ASSERT_NEAR(evaluate("let x = 2; let y = x;").double_val, 2.0, eps); }

  { ASSERT_NEAR(evaluate("let x = 2; x + 1;").double_val, 3.0, eps); }
}
