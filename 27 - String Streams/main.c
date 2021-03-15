#include "strstream.h"
#include <stdio.h>

int main()
{
    printf("Hello, world!\n");

    strstream s = strstream_allocDefault();
    printf("{%d, %d, \"%s\"}\n", s.size, s.capacity, s.str);

    FILE *fp;
    fp = fopen("test.txt", "rb");
    strstream_readFile(&s, fp, 0);
    fclose(fp);
    printf("{%d, %d, \"%s\"}\n", s.size, s.capacity, s.str);

    strstream_concat(&s, "abcd");
    printf("{%d, %d, \"%s\"}\n", s.size, s.capacity, s.str);

    strstream_concat(&s, "My name is %s\t", "Facemaskdjlksahfalskjdfhlaksdjf");
    printf("{%d, %d, \"%s\"}\n", s.size, s.capacity, s.str);

    fp = fopen("test2.txt", "wb");
    strstream_writeFile(&s, fp, 0, 0);
    fclose(fp);

    strstream_clear(&s);

    return 0;
}