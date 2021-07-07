#include <calc/token.h>
#include <calc/stream.h>
#include <calc/assert.h>

token parse_number(stream *s) {
    const char* begin = S_PEEK(s);
    const char* it = S_PEEK(s);
    const char* end = 0;

    while(1) {
        switch( S_PEEK(s)[0] ) {

            CASE_NUMBERS
                it += 1;
                break;

            default:
                goto finish;
        }
    }

finish:
    end = it;
    s->buffer = end;
    codeloc loc = (codeloc) {.begin=begin, .end=end};
    return (token) {.type=tk_number, .loc=loc};
}

token next_token(stream *s) {
    switch( S_PEEK(s)[0] ) {

        CASE_NUMBERS
            return parse_number(s);

        CASE_OPERATORS { /* TODO handle multiple character operators */
            const char* pos = S_PEEK(s);
            codeloc loc = (codeloc) {.begin=pos, .end=pos+1};
            S_SKIP(s);
            return (token) {.type=tk_operator, .loc=loc};
        }

        case '\0': 
            return (token) {.type=tk_eof};

        default:
            break;

    }
    CHECK_NOT_REACHED();
}
