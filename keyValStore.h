#ifndef PRAKBS22_KEYVALSTORE_H
#define PRAKBS22_KEYVALSTORE_H

#define KEYSIZE 100
#define VALUESIZE 200
#define STORAGECAPACITY 2
#define SUBSCRIBERAMOUNT 5

#include <string.h>
#include <sys/msg.h>

typedef struct Key_Value {
    char key[KEYSIZE];
    char value[VALUESIZE];
    int subs[SUBSCRIBERAMOUNT];
} Key_Value;

typedef struct Storage {
    unsigned int capacity;
    unsigned int len;
    Key_Value data[STORAGECAPACITY];
} Storage;

typedef struct Text_message {
    long mtype;
    char mkey[KEYSIZE];
} Text_message;

void storage_init(Storage* set_storage);
void storage_set(Storage* set_storage);
void storage_unset();
int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);
int sub(char* key, int pid);
int pub(char* key, int msgQueueID);

#endif //PRAKBS22_KEYVALSTORE_H
