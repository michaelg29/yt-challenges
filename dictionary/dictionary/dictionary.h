#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

#define LENGTH 45 // maximum length for word

bool load();
unsigned int size(void);
bool check(const char* word);
void unload();

#endif // DICTIONARY_H