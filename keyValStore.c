#include "keyValStore.h"

Storage* storage = NULL;

void storage_init(Storage* set_storage) {
    storage_set(set_storage);
    storage->len = 0;
    storage->capacity = STORAGECAPACITY;
    storage_unset();
}

void storage_set(Storage* set_storage) {
    storage = set_storage;
}

void storage_unset() {
    storage = NULL;
}

int put(char* key, char* value) {

    for(unsigned int i = 0; i < storage->len; i++) {
        if (strcmp(key, storage->data[i].key) == 0) {          //key schon vorhanden
            strncpy(storage->data[i].value, value, VALUESIZE);
            return 0;
        }
    }

    if(storage->len == storage->capacity)
        return -1;

    unsigned int i = storage->len++;
    strncpy(storage->data[i].key, key, KEYSIZE);
    strncpy(storage->data[i].value, value, VALUESIZE);

    for(int j = 0; j < SUBSCRIBERAMOUNT; j++)
        storage->data[i].subs[j] = -1;

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
                for(int j = 0; j < SUBSCRIBERAMOUNT; j++) {
                    storage->data[i].subs[j] = storage->data[i + 1].subs[j];
                }
                //i++; ?
            }

            return 1;
        }

    return 0;
}

int sub(char* key, int pid){
    for(int i = 0; i < storage->len; i++)
        if (strcmp(key, storage->data[i].key) == 0)
            for(int j = 0; j < SUBSCRIBERAMOUNT; j++) {
                if (storage->data[i].subs[j] == pid)
                    return 0;

                else if (storage->data[i].subs[j] == -1) {
                    storage->data[i].subs[j] = pid;
                    return 1;
                }
            }

    return 0;
}

int pub(char* key, int msgQueueID){
    for(int i = 0; i < storage->len; i++)
        if (strcmp(key, storage->data[i].key) == 0) {
            for (int j = 0; j < SUBSCRIBERAMOUNT; j++)
                if(storage->data[i].subs[j] != -1) {
                    Text_message message;
                    message.mtype = storage->data[i].subs[j];
                    strcpy(message.mkey, key);
                    if (msgsnd(msgQueueID, &message, strlen(message.mkey), 0) < 0)
                        return 0;
                }

            return 1;
        }

    return 1;
}