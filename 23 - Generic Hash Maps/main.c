#include <stdio.h>

#include "hashmap.h"

void printEntry(void *key, void *val)
{
    printf("%s --> %s\n", (char *)key, val ? (char *)val : "NULL");
}

int main()
{
    hashmap m = hmap_allocateStrAsKey();

    hmap_put(&m, "key0", "val0");
    hmap_put(&m, "key1", "val1");
    hmap_put(&m, "key2", "val2");
    hmap_put(&m, "key3", "val3");
    hmap_put(&m, "key4", "val4");
    hmap_put(&m, "key5", "val5");
    hmap_put(&m, "key6", "val6");
    hmap_put(&m, "key7", "val7");
    hmap_put(&m, "key8", "val8");
    hmap_put(&m, "key9", "val9");
    hmap_put(&m, "keya", "vala");
    hmap_put(&m, "keyb", "valb");
    hmap_put(&m, "keyc", "valc");
    hmap_put(&m, "keyd", "vald");
    hmap_put(&m, "keye", "vale");
    hmap_put(&m, "keyf", "valf");

    printf("%s --> %s\n", "keye", (char *)hmap_get(&m, "keye"));

    hmap_put(&m, "keye", "valee");

    printf("%s --> %s\n", "keye", (char *)hmap_get(&m, "keye"));

    hmap_freeDeep(&m);

    return 0;
}