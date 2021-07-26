#include <calc/assert.h>
#include <calc/stream.h>
#include <calc/token.h>
#include <stdlib.h>
#include <string.h>

static token parse_number(stream *);

static token parse_identifier(stream *);

static token next_token(stream *);

static void skip_whitespace(stream *);

static token parse_singlechar_token(stream *, token_type);

token peek(lexer *self) { return self->last_token_; }

void skip(lexer *self) { self->last_token_ = next_token(&self->s); }

lexer new_lexer(const char *buffer) {
    lexer lex;
    lex.s           = new_stream(buffer);
    lex.last_token_ = next_token(&lex.s);
    return lex;
}

static const char *parse_decimal(const char *beg) {
    while (1) {
        switch (beg[0]) {
            CASE_NUMBERS
            beg += 1;
            break;

        default:
            return beg;
        }
    }
}

static token parse_number(stream *s) {
    const char *begin = S_PEEK(s);
    const char *it    = begin;
    const char *end;

    while (1) {
        switch (it[0]) {
            CASE_NUMBERS
            it += 1;
            break;

        case '.':
            it = parse_decimal(it + 1);
            goto finish;

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
    const char *end;

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

    char *  identifier_name = NULL;
    codeloc loc             = (codeloc){.begin = begin, .end = end};

#define COMPARE(func)                                                          \
    if (strcmp((identifier_name = normalized_name(loc)), #func) == 0) {        \
        free(identifier_name);                                                 \
        return (token){.type = tk_##func, .loc = loc};                         \
    } else {                                                                   \
        free(identifier_name);                                                 \
    }
    ENUMERATE_FUNCTIONS(COMPARE)
    ENUMERATE_CONSTANTS(COMPARE)
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

static token parse_singlechar_token(stream *s, token_type type) {
    const char *pos = S_PEEK(s);
    codeloc     loc = (codeloc){.begin = pos, .end = pos + 1};
    S_SKIP(s);
    return (token){.type = type, .loc = loc};
}

static token next_token(stream *s) {
    skip_whitespace(s);
    switch (S_PEEK(s)[0]) {
        CASE_NUMBERS
        return parse_number(s);

        CASE_CHARACTERS
        return parse_identifier(s);

        CASE_OPERATORS /* TODO handle multiple character operators */
            return parse_singlechar_token(s, tk_operator);

    case '(':
        return parse_singlechar_token(s, tk_left_paren);

    case ')':
        return parse_singlechar_token(s, tk_right_paren);

    case '{':
        return parse_singlechar_token(s, tk_left_curly);

    case '}':
        return parse_singlechar_token(s, tk_right_curly);

    case ',':
        return parse_singlechar_token(s, tk_comma);

    case '\0':
        return (token){.type = tk_eof};

    default:
        break;
    }
    CHECK_NOT_REACHED();
}
