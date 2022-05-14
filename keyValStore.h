#ifndef PRAKBS22_KEYVALSTORE_H
#define PRAKBS22_KEYVALSTORE_H

#define KEYSIZE 100
#define VALUESIZE 200
#define STORAGECAPACITY 2

#include "string.h"

typedef struct Key_Value {
    char key[KEYSIZE];
    char value[VALUESIZE];
} Key_Value;

typedef struct Storage {
    unsigned int capacity;
    unsigned int len;
    Key_Value data[STORAGECAPACITY];
} Storage;

void storage_init(Storage* set_storage);
void storage_set(Storage* set_storage);
void storage_unset();
int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);

#endif //PRAKBS22_KEYVALSTORE_H
