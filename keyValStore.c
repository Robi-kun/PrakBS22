#include "keyValStore.h"

#define STRINGSIZE 100
#define KEYLISTSIZE 100

typedef struct keys_{
    char keyname[STRINGSIZE];
    char value[STRINGSIZE];
}keys;

keys keyArray[KEYLISTSIZE];


void arrayEinrichten(){
    for(int i = 0; i < KEYLISTSIZE; i++)
        keyArray[i].keyname[0] = '\0';
}

int put(char* key, char* value){
    for(int i = 0; i < KEYLISTSIZE; i++)
        if (strcmp(key, keyArray[i].keyname) == 0) {          //key schon vorhanden
            char temp[STRINGSIZE];
            strncpy(temp, keyArray[i].value, STRINGSIZE);
            strncpy(keyArray[i].value, value, STRINGSIZE);
            keyArray[i].value[STRINGSIZE - 1] = '\0';
            strncpy(value, temp, STRINGSIZE);
            return -1;
        }
    for(int i = 0; i < KEYLISTSIZE; i++)
        if(keyArray[i].keyname[0] == '\0'){         //sucht freies Key-Feld
            strncpy(keyArray[i].keyname,key,STRINGSIZE);
            strncpy(keyArray[i].value,value,STRINGSIZE);
            keyArray[i].keyname[STRINGSIZE-1] = '\0';
            keyArray[i].value[STRINGSIZE-1] = '\0';
            strncpy(key,keyArray[i].keyname,STRINGSIZE);
            strncpy(value,keyArray[i].value,STRINGSIZE);
            return 1;
        }
    return 0;
}

int get(char* key, char* res){
    for(int i = 0; i < KEYLISTSIZE; i++)
        if(strcmp(key, keyArray[i].keyname) == 0) {
            strncpy(res,keyArray[i].value,STRINGSIZE);
            return 1;
        }
    return 0;
}

int del(char* key){
    for(int i = 0; i < KEYLISTSIZE; i++)
        if (strcmp(key, keyArray[i].keyname) == 0) {
            keyArray[i].keyname[0] = '\0';
            return 1;
        }
    return 0;
}