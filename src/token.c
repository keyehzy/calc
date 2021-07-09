#include <calc/assert.h>
#include <calc/stream.h>
#include <calc/token.h>
#include <stdlib.h>
#include <string.h>

static token parse_number(stream *);

static token parse_identifier(stream *);

static token next_token(stream *);

static void skip_whitespace(stream *);

token peek(lexer *self) { return self->last_token_; }

void skip(lexer *self) { self->last_token_ = next_token(&self->s); }

lexer new_lexer(const char *buffer) {
    lexer lex;
    lex.s           = new_stream(buffer);
    lex.last_token_ = next_token(&lex.s);
    return lex;
}

static token parse_number(stream *s) {
    const char *begin = S_PEEK(s);
    const char *it    = begin;
    const char *end   = 0;

    while (1) {
        switch (it[0]) {
        case '.':
            CASE_NUMBERS
            it += 1;
            break;

        default:
            goto finish;
        }
    }

finish:
    end         = it;
    s->buffer   = end;
    codeloc loc = (codeloc){.begin = begin, .end = end};
    return (token){.type = tk_number, .loc = loc};
}

static token parse_identifier(stream *s) {
    const char *begin = S_PEEK(s);
    const char *it    = begin;
    const char *end   = 0;

    while (1) {
        switch (it[0]) {
            CASE_CHARACTERS
            it += 1;
            break;

        default:
            goto finish;
        }
    }

finish:
    end       = it;
    s->buffer = end;

    char *  identifier_name = (char *)malloc(sizeof(char));
    codeloc loc             = (codeloc){.begin = begin, .end = end};

#define COMPARE(func)                                                          \
    if (strcmp(normalized_name(identifier_name, loc), #func) == 0) {           \
        free(identifier_name);                                                 \
        return (token){.type = tk_##func, .loc = loc};                         \
    }
    ENUMERATE_FUNCTIONS(COMPARE)
#undef COMPARE

    CHECK_NOT_REACHED();
}

static void skip_whitespace(stream *s) {
    const char *input = S_PEEK(s);
    while (input[0] == ' ' || input[0] == '\t' || input[0] == '\f' ||
           input[0] == '\v' || input[0] == '\n' || input[0] == '\r') {
        input += 1;
    }
    s->buffer = input;
}

static token next_token(stream *s) {
    skip_whitespace(s);
    switch (S_PEEK(s)[0]) {
        CASE_NUMBERS
        return parse_number(s);

        CASE_OPERATORS { /* TODO handle multiple character operators */
            const char *pos = S_PEEK(s);
            codeloc     loc = (codeloc){.begin = pos, .end = pos + 1};
            S_SKIP(s);
            return (token){.type = tk_operator, .loc = loc};
        }

        CASE_CHARACTERS
        return parse_identifier(s);

    case '(': {
        const char *pos = S_PEEK(s);
        codeloc     loc = (codeloc){.begin = pos, .end = pos + 1};
        S_SKIP(s);
        return (token){.type = tk_left_paren, .loc = loc};
    }

    case ')': {
        const char *pos = S_PEEK(s);
        codeloc     loc = (codeloc){.begin = pos, .end = pos + 1};
        S_SKIP(s);
        return (token){.type = tk_right_paren, .loc = loc};
    }

    case '\0':
        return (token){.type = tk_eof};

    default:
        break;
    }
    CHECK_NOT_REACHED();
}
