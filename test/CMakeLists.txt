cmake_minimum_required(VERSION 3.18)

add_executable(
  calc-test
  test-token.cpp
)
target_include_directories(calc-test PRIVATE .)
target_link_libraries(
  calc-test
  PRIVATE
  gmock
  gmock_main
  gtest
  calc-lib
)

add_test(
  NAME calc-test
  COMMAND calc-test
)
