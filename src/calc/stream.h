#pragma once

#define S_SKIP(s) (s->buffer += 1)
#define S_PEEK(s) (s->buffer)
#define S_IS_EOF(s) (PEEK(s) == '\0')

typedef struct {
    const char* buffer;
} stream;
