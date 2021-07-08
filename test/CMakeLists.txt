cmake_minimum_required(VERSION 3.18)

add_executable(
  calc-test
  test-token.cpp
  test-ast.cpp
  test-eval.cpp
)

target_include_directories(calc-test PRIVATE .)

target_link_libraries(
  calc-test
  PRIVATE
  gtest_main
  calc-lib
)

add_test(
  NAME calc-test
  COMMAND calc-test
)
