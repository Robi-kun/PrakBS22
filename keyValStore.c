#include "keyValStore.h"

Storage* storage = NULL;

void storage_init(Storage* set_storage) {
    storage_set(set_storage);
    storage->len = 0;
    storage->capacity = STORAGECAPACITY;
}

void storage_set(Storage* set_storage) {
    storage = set_storage;
}

void storage_unset() {
    storage = NULL;
}

int put(char* key, char* value) {
    if(storage->len == storage->capacity)
        return -1;

    for(unsigned int i = 0; i < storage->len; i++) {
        if (strcmp(key, storage->data[i].key) == 0) {          //key schon vorhanden
            strncpy(storage->data[i].value, value, VALUESIZE);
            return 0;
        }
    }

    unsigned int i = storage->len++;
    strncpy(storage->data[i].key, key, KEYSIZE);
    strncpy(storage->data[i].value, value, VALUESIZE);

    return 1;
}

int get(char* key, char* res){
    for(int i = 0; i < storage->len; i++)
        if(strcmp(key, storage->data[i].key) == 0) {
            strncpy(res,storage->data[i].value,VALUESIZE);

            return 1;
        }

    return 0;
}

int del(char* key){
    for(int i = 0; i < storage->len; i++)
        if (strcmp(key, storage->data[i].key) == 0) {
            storage->data[i].key[0] = '\0';
            storage->len--;

            while (storage->data[i+1].key[0] != '\0') {
                if(i == storage->capacity) {
                    break;
                }
                strcpy(storage->data[i].key, storage->data[i+1].key);
                storage->data[i+1].key[0] = '\0';
                strcpy(storage->data[i].value, storage->data[i+1].value);
            }

            return 1;
        }

    return 0;
}