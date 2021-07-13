cmake_minimum_required(VERSION 3.18)

add_executable(
  calc
  calc.c
)

target_link_libraries(calc PRIVATE calc-lib readline m)

add_library(
  calc-lib 
  calc/assert.h
  calc/ast.h
  calc/codeloc.h
  calc/eval.h
  calc/stream.h
  calc/token.h
  ast.c
  codeloc.c
  eval.c
  stream.c
  token.c
)

target_include_directories(calc-lib PUBLIC .)

