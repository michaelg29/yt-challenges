#ifndef HASHMAP_H
#define HASHMAP_H

#define DEFAULT_NUM_BUCKETS 16
#define MAX_LOAD_FACTOR 0.75

typedef struct mapentry
{
    void *key;
    int hash;
    void *val;
    struct mapentry *next;
} mapentry;

typedef struct hashmap
{
    unsigned int numBuckets;
    unsigned int numEntries;

    mapentry **buckets;

    int (*hashfunc)(void *key);
    int (*keycmp)(void *key1, void *key2);
} hashmap;

hashmap hmap_allocate(int (*hashfunc)(void *key), int (*keycmp)(void *key1, void *key2));
hashmap hmap_allocateNumBuckets(unsigned int numBuckets, int (*hashfunc)(void *key), int (*keycmp)(void *key1, void *key2));
hashmap hmap_allocateNumEntries(unsigned int numEntries, int (*hashfunc)(void *key), int (*keycmp)(void *key1, void *key2));
void hmap_reallocate(hashmap *map);

mapentry *hmap_allocateEntryWithHash(void *key, int hash, void *val);

void hmap_put(hashmap *map, void *key, void *val);
void *hmap_get(hashmap *map, void *key);
mapentry *hmap_getEntry(hashmap *map, void *key);
void *hmap_remove(hashmap *map, void *key);

void hmap_traverse(hashmap *map, void (*visit)(void *key, void *val));
void hmap_traverse2(hashmap *map, void (*visit)(hashmap *map, void *key, void *val));

void hmap_free(hashmap *map);
void hmap_freeDeep(hashmap *map);
void hmap_freeEntry(mapentry *entry);
void hmap_freeEntryDeep(mapentry *entry);

// hashmap - string key specific
hashmap hmap_allocateStrAsKey();
hashmap hmap_allocateStrAsKeyWithNum(unsigned int numBuckets);

int strhash(void *key);
int strkeycmp(void *key1, void *key2);

#endif