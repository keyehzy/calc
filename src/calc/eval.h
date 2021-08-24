#pragma once

#include <calc/ast.h>

typedef struct {
    enum { Number, List } type;
    union {
        double double_val;
        vector list_val;
    } value;
} ReturnExpr;

ReturnExpr evaluate(const char *);
