#pragma once

#include <calc/ast.h>

typedef struct {
    enum { Number, List } type;
    union {
        double double_val;
        vector list_val;
    } value;
} ReturnExpr;

ReturnExpr NewNumber(double);
ReturnExpr NewList(vector);
ReturnExpr evaluate_ast(AST *);
ReturnExpr evaluate(const char *);
