#include <calc/assert.h>
#include <calc/token.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static token parse_number(lexer *);

static token parse_identifier(lexer *);

static token next_token(lexer *);

static void skip_whitespace(lexer *);

static token parse_singlechar_token(lexer *, token_type);

token peek(lexer *self) { return self->last_token_; }

void skip(lexer *self) { self->last_token_ = next_token(self); }

lexer new_lexer(const char *buffer) {
    lexer lex = {0};
    lex.buffer_ = buffer;
    lex.last_token_ = next_token(&lex);
    return lex;
}

static void s_skip(lexer *lex) {
  lex->buffer_ += 1;
}

static const char* s_peek(lexer *lex) {
  return lex->buffer_;
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

static token parse_number(lexer *lex) {
    const char *begin = s_peek(lex);
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
    if(it[0] == 'i') it += 1;
    end         = it;
    lex->buffer_   = end;
    codeloc loc = (codeloc){.begin = begin, .end = end};
    return (token){.type = tk_number, .loc = loc};
}

static token parse_identifier(lexer *lex) {
    const char *begin = s_peek(lex);
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
    lex->buffer_ = end;

    char *  identifier_name = NULL;
    codeloc loc             = (codeloc){.begin = begin, .end = end};

#define COMPARE(func)                                                          \
    if (strcmp((identifier_name = normalized_name(loc)), #func) == 0) {        \
        free(identifier_name);                                                 \
        return (token){.type = tk_##func, .loc = loc};                         \
    } else {                                                                   \
        free(identifier_name);                                                 \
    }
    ENUMERATE_KEYWORDS(COMPARE)
    ENUMERATE_FUNCTIONS(COMPARE)
    ENUMERATE_CONSTANTS(COMPARE)
#undef COMPARE

    return (token){.type = tk_identifier, .loc = loc};
}

static void skip_whitespace(lexer *lex) {
    const char *input = s_peek(lex);
    while (input[0] == ' ' || input[0] == '\t' || input[0] == '\f' ||
           input[0] == '\v' || input[0] == '\n' || input[0] == '\r') {
        input += 1;
    }
    lex->buffer_ = input;
}

static token parse_singlechar_token(lexer *lex, token_type type) {
    const char *pos = s_peek(lex);
    codeloc     loc = (codeloc){.begin = pos, .end = pos + 1};
    s_skip(lex);
    return (token){.type = type, .loc = loc};
}

static token next_token(lexer *lex) {
    skip_whitespace(lex);
    switch (s_peek(lex)[0]) {
        CASE_NUMBERS
        return parse_number(lex);

        CASE_CHARACTERS
        return parse_identifier(lex);

        CASE_OPERATORS /* TODO handle multiple character operators */
            return parse_singlechar_token(lex, tk_operator);

    case '=':
        return parse_singlechar_token(lex, tk_equal);

    case '(':
        return parse_singlechar_token(lex, tk_left_paren);

    case ')':
        return parse_singlechar_token(lex, tk_right_paren);

    case '{':
        return parse_singlechar_token(lex, tk_left_curly);

    case '}':
        return parse_singlechar_token(lex, tk_right_curly);

    case ',':
        return parse_singlechar_token(lex, tk_comma);

    case ';':
        return parse_singlechar_token(lex, tk_semicolon);

    case '\0':
        return (token){.type = tk_eof};

    default:
        emit_error(error_unexpected_token,
               new_loc(s_peek(lex), s_peek(lex) + 1));
        break;
    }
    return (token) {.type = tk_invalid };
}
