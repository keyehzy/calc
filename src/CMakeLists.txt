cmake_minimum_required(VERSION 3.18)

add_executable(
  calc
  calc.c
)

target_link_libraries(calc PRIVATE calc-lib editline m)

add_library(
  calc-lib
  calc/assert.h
  calc/ast.h
  calc/codeloc.h
  calc/token.h
  calc/vector.h
  assert.c
  ast.c
  codeloc.c
  token.c
  vector.c
)

target_include_directories(calc-lib PUBLIC .)
