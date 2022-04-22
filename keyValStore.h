#ifndef PRAKBS22_KEYVALSTORE_H
#define PRAKBS22_KEYVALSTORE_H

void arrayEinrichten();
int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);

#endif //PRAKBS22_KEYVALSTORE_H
