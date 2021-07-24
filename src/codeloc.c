#include <calc/codeloc.h>
#include <stdlib.h>
#include <string.h>

codeloc new_loc(const char *begin, const char *end) {
    return (codeloc){.begin = begin, .end = end};
}

char *normalized_name(codeloc loc) {
    size_t len  = (loc.end - loc.begin);
    char * name = (char *)malloc(len + 1);
    strncpy(name, loc.begin, len);
    name[len] = '\0';
    return name;
}
