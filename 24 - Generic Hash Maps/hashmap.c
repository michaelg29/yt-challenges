#include "hashmap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

hashmap hmap_allocate(int (*hashfunc)(void *key), int (*keycmp)(void *key1, void *key2))
{
    return hmap_allocateNumBuckets(DEFAULT_NUM_BUCKETS, hashfunc, keycmp);
}

hashmap hmap_allocateNumBuckets(unsigned int numBuckets, int (*hashfunc)(void *key), int (*keycmp)(void *key1, void *key2))
{
    if (!numBuckets)
    {
        numBuckets = DEFAULT_NUM_BUCKETS;
    }

    hashmap ret;

    ret.numBuckets = numBuckets;
    ret.numEntries = 0;
    ret.buckets = malloc(numBuckets * sizeof(mapentry *));
    ret.hashfunc = hashfunc;
    ret.keycmp = keycmp;

    for (unsigned int i = 0; i < ret.numBuckets; i++)
    {
        ret.buckets[i] = NULL;
    }

    return ret;
}

hashmap hmap_allocateNumEntries(unsigned int numEntries, int (*hashfunc)(void *key), int (*keycmp)(void *key1, void *key2))
{
    // find new number of buckets
    double numBuckets = DEFAULT_NUM_BUCKETS;
    double numEntries = (double)numEntries;
    while (numEntries / numBuckets > MAX_LOAD_FACTOR)
    {
        numBuckets *= 2.0;
    }

    return hmap_allocateNumBuckets((unsigned int)numBuckets, hashfunc, keycmp);
}

void hmap_reallocate(hashmap *map)
{
    // placeholder to store new set of buckets
    hashmap ret = hmap_allocateNumEntries(map->numEntries, map->hashfunc, map->keycmp);

    // traverse through element
    for (unsigned int i = 0; i < map->numBuckets; i++)
    {
        mapentry *current = map->buckets[i];

        while (current)
        {
            hmap_put(&ret, current->key, current->val);
            current = current->next;
        }
    }

    // swap buckets lists
    mapentry **tmpList = map->buckets;
    map->buckets = ret.buckets;
    ret.buckets = tmpList;

    // free temporary copy
    ret.numBuckets = map->numBuckets;
    hmap_free(&ret);

    // update num of buckets
    map->numBuckets = numBuckets;
}

mapentry *hmap_allocateEntryWithHash(void *key, int hash, void *val)
{
    mapentry *ret = malloc(sizeof(mapentry));

    ret->key = key;
    ret->hash = hash;
    ret->val = val;
    ret->next = NULL;
}

void hmap_put(hashmap *map, void *key, void *val)
{
    // determine index of target bucket
    int hash = map->hashfunc(key);
    int idx = abs(hash) % map->numBuckets;

    // allocate entry
    mapentry *newEntry = hmap_allocateEntryWithHash(key, hash, val);

    // add entry to list
    if (map->buckets[idx])
    {
        // bucket exists, add to the SLinkedList in sort order
        mapentry *current = map->buckets[idx];

        // iterate until end or find a greater hash value
        while (current->next && hash > current->hash)
        {
            current = current->next;
        }

        if (hash == current->hash && !map->keycmp(key, current->key))
        {
            // keys are equal
            current->val = val;
        }
        else
        {
            // no match, insert between the target node and its next sibling
            mapentry *tmp = current->next;
            current->next = newEntry;
            newEntry->next = tmp;

            map->numEntries++;
        }
    }
    else
    {
        // no bucket exists, create one
        map->buckets[idx] = newEntry;
        map->numEntries++;
    }

    // resize if necessary
    double loadFactor = map->numEntries;
    loadFactor /= (double)map->numBuckets;
    if (loadFactor > MAX_LOAD_FACTOR)
    {
        printf("reallocate\n");
        hmap_reallocate(map);
    }
}

void *hmap_get(hashmap *map, void *key)
{
    mapentry *entry = hmap_getEntry(map, key);

    return entry ? entry->val : NULL;
}

mapentry *hmap_getEntry(hashmap *map, void *key)
{
    // determine index of target bucket
    int hash = map->hashfunc(key);
    int idx = abs(hash) % map->numBuckets;

    if (map->buckets[idx])
    {
        // traverse through bucket
        mapentry *current = map->buckets[idx];

        while (current)
        {
            if (hash < current->hash)
            {
                current = NULL;
                break;
            }

            // test for equality
            if (hash == current->hash && !map->keycmp(key, current->key))
            {
                break;
            }

            current = current->next;
        }

        return current;
    }
    else
    {
        return NULL;
    }
}

void *hmap_remove(hashmap *map, void *key)
{
    // determine index of target bucket
    int hash = map->hashfunc(key);
    int idx = abs(hash) % map->numBuckets;

    if (map->buckets[idx])
    {
        // traverse through bucket
        mapentry *current = map->buckets[idx];
        mapentry *prev = NULL;

        while (current)
        {
            if (hash < current->hash)
            {
                current = NULL;
                break;
            }

            if (hash == current->hash && !map->keycmp(key, current->key))
            {
                break;
            }

            prev = current;
            current = current->next;
        }

        if (current)
        {
            // found node with the target key, remove
            void *ret = current->val;

            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                map->buckets[idx] = current->next;
            }

            free(current);

            map->numEntries--;
            return ret;
        }
    }

    return NULL;
}

void hmap_traverse(hashmap *map, void (*visit)(void *key, void *val))
{
    for (unsigned int i = 0; i < map->numBuckets; i++)
    {
        mapentry *current = map->buckets[i];

        while (current)
        {
            visit(current->key, current->val);
            current = current->next;
        }
    }
}

void hmap_traverse2(hashmap *map, void (*visit)(hashmap *map, void *key, void *val))
{
    for (unsigned int i = 0; i < map->numBuckets; i++)
    {
        mapentry *current = map->buckets[i];

        while (current)
        {
            visit(map, current->key, current->val);
            current = current->next;
        }
    }
}

void hmap_free(hashmap *map)
{
    for (unsigned int i = 0; i < map->numBuckets; i++)
    {
        if (map->buckets[i])
        {
            hmap_freeEntry(map->buckets[i]);
            free(map->buckets[i]);
        }
    }

    map->numBuckets = 0;
    map->numEntries = 0;
    free(map->buckets);
}

void hmap_freeDeep(hashmap *map)
{
    for (unsigned int i = 0; i < map->numBuckets; i++)
    {
        if (map->buckets[i])
        {
            hmap_freeEntryDeep(map->buckets[i]);
            free(map->buckets[i]);
        }
    }

    map->numBuckets = 0;
    map->numEntries = 0;
    free(map->buckets);
}

void hmap_freeEntry(mapentry *entry)
{
    if (entry)
    {
        if (entry->next)
        {
            hmap_freeEntry(entry->next);
            free(entry->next);
        }
    }
}
void hmap_freeEntryDeep(mapentry *entry)
{
    if (entry)
    {
        if (entry->next)
        {
            hmap_freeEntryDeep(entry->next);
            free(entry->next);
        }

        free(entry->key);
        free(entry->val);
    }
}

// hashmap - string key specific
hashmap hmap_allocateStrAsKey()
{
    return hmap_allocate(strhash, strkeycmp);
}

hashmap hmap_allocateStrAsKeyWithNum(unsigned int numBuckets)
{
    return hmap_allocateNum(numBuckets, strhash, strkeycmp);
}

int strhash(void *key)
{
    int ret = 0;

    if (!key)
    {
        return ret;
    }

    char *strkey = (char *)key;

    int len = strlen(strkey);

    for (int i = 0; i < len; i++)
    {
        ret = strkey[i] + 31 * ret;
    }

    return ret;
}

int strkeycmp(void *key1, void *key2)
{
    if (!key1)
    {
        if (!key2)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (!key2)
    {
        return 1;
    }

    char *strkey1 = (char *)key1;
    char *strkey2 = (char *)key2;

    return strcmp(strkey1, strkey2);
}