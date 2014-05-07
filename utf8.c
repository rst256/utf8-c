#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utf8.h"

void utf8_stream (UTF8Stream* in, FILE* stream)
{
    memset(in->history, 0, sizeof(in->history));
    in->pos = 0;
    in->buffer = 0;
    in->stream = stream;
}

size_t utf8_getc (uint32_t* c, UTF8Stream* in)
{
    uint8_t byte[4];
    if(in->buffer)
    {
        *c = in->history[in->pos - in->buffer];
        in->buffer -= 1;
        if(*c < 0x80)
        {
            return 1;
        }
        else if(*c < 0x800)
        {
            return 2;
        }
        else if(*c < 0x10000)
        {
            return 3;
        }
        else if(*c < 0x200000)
        {
            return 4;
        }
    }
    if(!fread(byte, 1, 1, in->stream))
    {
        return 0;
    }
    if(!(byte[0] & 0200)) // Single byte-width
    {
        *c = in->history[in->pos++] = byte[0];
        return 1;
    }
    else if((byte[0] & 0340) == 0300) // Double byte-width
    {
        if(!fread(byte + 1, 1, 1, in->stream))
        {
            return 0;
        }
        if(byte[1] & 0100)
        {
            return 0;
        }
        *c = in->history[in->pos++] = ((byte[0] & 037) << 6) + (byte[1] & 077);
        return 2;
    }
    else if((byte[0] & 0360) == 0340) // Triple byte-width
    {
        if(fread(byte + 1, 1, 2, in->stream) != 2)
        {
            return 0;
        }
        if((byte[1] & 0100) || (byte[2] & 0100))
        {
            return 0;
        }
        *c = in->history[in->pos++] = ((byte[0] & 017) << 12) + ((byte[1] & 077) << 6) + (byte[2] & 077);
        return 3;
    }
    else if ((byte[0] & 0370) == 0360) // Quadruple byte-width
    {
        if(fread(byte + 1, 1, 3, in->stream) != 3)
        {
            return 0;
        }
        if((byte[1] & 0100) || (byte[2] & 0100) || (byte[3] & 0100))
        {
            return 0;
        }
        *c = in->history[in->pos++] = ((byte[0] & 07) << 18) + ((byte[1] & 077) << 12) + ((byte[2] & 077) << 6) + (byte[3] & 077);
        return 4;
    }
    return 0;
}

void utf8_ungetc (UTF8Stream* in)
{
    in->buffer += 1;
}

size_t utf8_putc (uint32_t c, FILE* out)
{
    if(c < 0x80) // Single byte-width
    {
        fputc(c, out);
        return 1;
    }
    else if(c < 0x800) // Double byte-width
    {
        fputc((c >> 6) | 0300, out);
        fputc((c & 077) | 0200, out);
        return 2;
    }
    else if(c < 0x10000) // Triple byte-width
    {
        fputc((c >> 12) | 0340, out);
        fputc(((c >> 6) & 077) | 0200, out);
        fputc((c & 077) | 0200, out);
        return 3;
    }
    else if(c < 0x200000) // Quadruple byte-width
    {
        fputc((c >> 18) | 0360, out);
        fputc(((c >> 12) & 077) | 0200, out);
        fputc(((c >> 6) & 077) | 0200, out);
        fputc((c & 077) | 0200, out);
        return 4;
    }
    return 0;
}