# utf8-c

utf8-c is an extremely small C library which can be used for reading and
writing with known UTF-8 files, and for accessing the Unicode code points for
each individual character.

    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include "utf8.h"
    
    int main (int argc, char const *argv[])
    {
        FILE* file = fopen("./example.txt", "r");
        UTF8Stream in;
        uint32_t c;
        size_t length;
        utf8_stream(&in, file);
        while((length = utf8_getc(&c, &in)))
        {
            utf8_putc(c, stdout);
        }
        fclose(file);
        return 0;
    }
