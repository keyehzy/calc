#pragma once

#include <assert.h>
#include <calc/codeloc.h>
#include <stdio.h>
#define CHECK assert
#define CHECK_NOT_REACHED() assert(0)
#define TODO CHECK_NOT_REACHED

#define __ERRORS(O)                                  \
  O(error_invalid_token_in_primary_expression)       \
  O(error_unexpected_operation)                      \
  O(error_unexpected_token)                          \
  O(error_unexpected_token_after_let)                \
  O(error_malformed_expression)                      \
  O(error_missing_expected_token)                    \
  O(error_invalid_operation_between_number_and_list) \
  O(error_invalid_rhs_for_binary_expression)         \
  O(error_invalid_assignee_for_assignment)           \
  O(error_invalid_expression_in_commas)              \
  O(error_use_of_undeclared_variable)

#define __ENUM(err) err,
#define __CASE_TOSTR(err) \
  case err:               \
    return #err;

typedef enum { __ERRORS(__ENUM) } error_kind;

const char *error_name(error_kind);
void emit_error(error_kind, codeloc);
