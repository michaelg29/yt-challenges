#include "strstream.h"
#include <stdio.h>

int main()
{
    printf("Hello, world!\n");

    // concatenation and file io test
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
    printf("{%d, %d, \"%s\"}\n\n", s.size, s.capacity, s.str);

    fp = fopen("test2.txt", "wb");
    strstream_writeFile(&s, fp, 0, 0);
    fclose(fp);

    printf("%s\n", strstream_substrRange(&s, 0, 17));
    printf("%s\n", strstream_substrRange(&s, 17, 0));
    printf("%s\n", strstream_substrLength(&s, 0, 17));
    printf("%s\n", strstream_substrLength(&s, 17, -17));

    strstream_clear(&s);

    // split string test
    s = strstream_fromStr("hello this is a string with spaces");

    char **list;
    int n = strstream_split(&s, ' ', &list, 4);

    printf("%d strings\n", n);
    for (int i = 0; i < n; i++)
    {
        printf("%s\n", list[i]);
    }

    freeStringList(list, n);

    strstream_clear(&s);

    return 0;
}