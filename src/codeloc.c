#include <calc/codeloc.h>
#include <stdlib.h>
#include <string.h>

codeloc new_loc(const char *begin, const char *end) {
    return (codeloc){.begin = begin, .end = end};
}

char *normalized_name(char *name, codeloc loc) {
    size_t len = (loc.end - loc.begin);
    name       = (char *)realloc(name, len + 1);
    strncpy(name, loc.begin, len);
    name[len] = '\0';
    return name;
}
