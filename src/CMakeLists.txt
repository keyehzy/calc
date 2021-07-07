cmake_minimum_required(VERSION 3.18)

add_executable(
  calc
  calc.c
)

target_link_libraries(calc PRIVATE calc-lib readline)

add_library(
  calc-lib
  token.c
)

target_include_directories(calc-lib PUBLIC .)

