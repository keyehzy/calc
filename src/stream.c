#include <calc/stream.h>

stream new_stream(const char *buffer) { return (stream){.buffer = buffer}; }
