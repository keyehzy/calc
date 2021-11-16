#pragma once

typedef struct {
  const char *begin;
  const char *end;
} codeloc;

char *normalized_name(codeloc loc);
codeloc new_loc(const char *begin, const char *end);
