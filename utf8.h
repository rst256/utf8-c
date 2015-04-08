#ifndef UTF8_H
#define UTF8_H
#include <stdint.h>

typedef struct {
    uint32_t history[UINT16_MAX + 1];
    uint16_t pos, buffer;
    FILE* stream;
} UTF8Stream;

// Initializes a UTF8Stream type, <in>, and associates it with stream <stream>.
void utf8_stream (UTF8Stream* in, FILE* stream);

// Assigns the unicode value to <c>, reads from <in>.
// Returns number of bytes read.
size_t utf8_getc (uint32_t* c, UTF8Stream* in);

// Effectively rewinds the stream <in> a single UTF-8 character.
// Can be repeated 65536 times (the size of the 16bit buffer pointer).
void utf8_ungetc (UTF8Stream* in);

// Writes the UTF-8 series of bytes for Unicode value <c> to <out>/
// Returns the number of bytes written.
size_t utf8_putc (uint32_t c, FILE* out);

#endif

