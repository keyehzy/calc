#include <calc/assert.h>
#include <stdio.h>

const char *error_name(error_kind kind) {
  switch (kind) { __ERRORS(__CASE_TOSTR) }
  CHECK_NOT_REACHED();
}

void emit_error(error_kind kind, codeloc loc) {
  fprintf(stderr, "error `%s` emitted for `%s`\n", error_name(kind),
          normalized_name(loc));
}
